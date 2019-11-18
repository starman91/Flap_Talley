/////////////////////////////////////////////////////////////////////////////
//		    		ft_daq.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "ft_daq.h"
#include "sig_analysis.h"

#include <QtCharts>
#include <QLineSeries> 
#include <cmath>
#include <unordered_map>

FTDAQ::FTDAQ(void)
{
	m_loopTime = 10;
	m_killThread = false;
	v_targets.clear();
	return;
}

FTDAQ::~FTDAQ()
{
	return;
}

///////////////////////////////////////////////////////////////////////////////////
///Used to shutdown all devices to a 0 or default state
///////////////////////////////////////////////////////////////////////////////////
void FTDAQ::shutdown(void)
{
	//Clean up here
	return;
}


///////////////////////////////////////////////////////////////////////////////////
/// Master processing loop for handling all data acquisition. Also calls the 
/// simulation thread as needed for operation under test conditions.
///////////////////////////////////////////////////////////////////////////////////
void FTDAQ::run(void)
{
	//qDebug() << "Data aquisition thread starting";
	Target target;
						
	for (;;)
	{
		if (g_status.getSimMode())
		{	
			l_dataPoints = g_support.getSimData();
		}
		//Radar DAQ
		else
		{
			// TODO: At DAQ and data marshalling code here
			//qDebug() << "Acquiring radar data";
			//sem_recdata.acquire(1);					// Notify new data available
		}
	
		//Find targets
		//qDebug() << "Starting: " << QTime::currentTime().msecsSinceStartOfDay();
		if (g_status.getFindTarget())
		{
			if (0 < l_dataPoints.size())	
			{
				//Convert for analysis
				std::vector<long double> dp = convertListToVector(l_dataPoints);
				if (0 < size(dp))
				{
					// Perform analysis
					#ifdef TEST	// Test with canned data
						testAnalysisVectorStats();
					#endif 
					unordered_map<string, vector<long double>> output;
					output = SigAnalysis::z_score_thresholding(dp, v_targets);
					//qDebug() << "Z Thresholding: " << QTime::currentTime().msecsSinceStartOfDay();
					
					if (0 < v_targets.size())
					{ 
						storeTargetTriggerPoints(output["signals"]);
						//qDebug() << "Store triggers: " << QTime::currentTime().msecsSinceStartOfDay();

						storeTargetAmplitudePoints(output["signals"], dp);
						//qDebug() << "Store amplitudes: " << QTime::currentTime().msecsSinceStartOfDay();
					}
				}
			}
		}

		// Stop running thread if signalled
		if (m_killThread)
		{
			// Report shutdown
			qDebug() << "Data Acquisition thread has been stopped";
			//Shutdown and exit thread
			exit();
			return;
		}
		// End of loop. Delay before repeating
		usleep(m_loopTime);
	}
}


///////////////////////////////////////////////////////////////////////////////////
///Convert ther analysis QList into a vector. Populates the needed analysis
/// tracking data points too.
///@param dataPoints - a x-y point series of a single time slice of radar signal
///////////////////////////////////////////////////////////////////////////////////
std::vector<long double> FTDAQ::convertListToVector(QList<QPointF>& dataPoints)
{
	std::vector<long double> converted;
	for (int i = 0; i < dataPoints.size(); i++)
	{	
		converted.push_back(dataPoints.at(i).y());		
	}
	return converted;
}


///////////////////////////////////////////////////////////////////////////////////
/// Takes output of target tracking algorithm and builts plottable points to pass
/// to main window for plotting.
/// @param triggerData - an isolated pulse signal from the radar signal
///////////////////////////////////////////////////////////////////////////////////
void FTDAQ::storeTargetTriggerPoints(std::vector<long double>& triggerData)
{
	QList<QPointF> trackPoints;
	for (int i = 0; i < triggerData.size(); i++)
	{
		//Sets up nice plot of all targets
		QPointF newPoint;
		newPoint.setX(double(i) / 10.0);
		newPoint.setY(triggerData.at(i));
		trackPoints.push_back(newPoint);
	}
	g_support.addTrackData(trackPoints);
	return;
}


///////////////////////////////////////////////////////////////////////////////////
/// Takes output of target tracking algorithm and builts analyzes the data set to find
/// the target pulse. Once identified it then records the maximum point of the amplitude.
/// Values are then stored in buffers in the Support class to be accessed by both then
/// SigAnalysi and FtMain class for analysis and plotting respectively.
/// 
/// @param rise and fall - the markers for the end points of the pulse signal
/// @param triggerData - identified points of target signal.
/// @parma rawData - The full radar signal for a period in time, vectorized.
/// @param target - class stroes object to hold items calculated int his function.
///////////////////////////////////////////////////////////////////////////////////
void FTDAQ::storeTargetAmplitudePoints(std::vector<long double>& triggerData, std::vector<long double>& rawData)
{
	//Monitor testing
	//QFile file("flapping_found_amplitudes.csv");
     // if (!file.open(QIODevice::Append| QIODevice::Text))
         // return;
     //QTextStream out(&file);
	
	QTime mark;
	//Plottables
	QList<QPointF> targetPoints;

	//Store raw data points at this time slice
	// TODO: Set up for multiple targets
	v_targets.at(0).setSignal(rawData);
	
	// Init tracking variables
	int rise = -1;
	int fall = -1;
	for (int i = 0; i < triggerData.size(); i++)
	{
		if ((0 < i) && (i < triggerData.size() - 1))
		{
			// Check for rising edge of signal
			if ((triggerData.at(i) == 0) && (triggerData.at(i + 1) == 1))
			{
				rise = i;
			}
			//Find falling edge of ID signal
			else if ((triggerData.at(i) == 1) && (triggerData.at(i + 1) == 0))
			{
				fall = i;
			}
			//Examine whole trigger
			if ((rise > 0) && (fall > 0))
			{
				int targetX = 0;
				(fall - rise != 1) ? targetX = fall - (fall - rise) / 2 : targetX = rise;

				// Set plot point of center of peak.
				QPointF centerPoint;
				centerPoint.setX(double(targetX + 1) / 10.0);
				centerPoint.setY(rawData.at(targetX + 1));
				targetPoints.push_back(centerPoint);
				
				// Find maximum value
				QList<long double> signal;
				for (int point = rise - 5; point < fall + 5; point++)
				{
					signal.append(rawData.at(point));
				}
				//target.setSignal(signal);
				long double max = *std::max_element(signal.begin(), signal.end());
				// TODO: Expand for multiple targets
				v_targets.at(0).setCurrentAmplitude(max);		// tracking point
				QPointF targetPoint;
				targetPoint.setX (QTime::currentTime().msecsSinceStartOfDay()/1000.0);
				targetPoint.setY ( g_status.getSimTargetSize() - max);
				g_support.addTargetData(targetPoint);
				//if (g_status.getSimFlap())
					//out <<  QTime::currentTime().msecsSinceStartOfDay() << ", " << double(max) << "\n";

				// Reset tracking variables
				rise = -1;
				fall = -1;
			}
		}
	}
	return;
}


///////////////////////////////////////////////////////////////////////////////////
// Test method for checking VectorCalc processes.
///////////////////////////////////////////////////////////////////////////////////
void FTDAQ::testAnalysisVectorStats(void)
{
	
	std::vector<long double> testInput = { 1.0, 1.0, 1.1, 1.0, 0.9, 1.0, 1.0, 1.1, 1.0, 0.9, 1.0, 1.1, 1.0, 1.0, 0.9, 1.0, 1.0, 1.1, 1.0,
					1.0, 1.0, 1.0, 1.1, 0.9, 1.0, 1.1, 1.0, 1.0, 0.9, 1.0, 1.1, 1.0, 1.0, 1.1, 1.0, 0.8, 0.9, 1.0,
					1.2, 0.9, 1.0, 1.0, 1.1, 1.2, 1.0, 1.5, 1.0, 3.0, 2.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 0.9, 1.0,
					1.0, 3.0, 2.6, 4.0, 3.0, 3.2, 2.0, 1.0, 1.0, 0.8, 4.0, 4.0, 2.0, 2.5, 1.0, 1.0, 1.0 };

	std::vector<long double> expectedResults = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,
																			0,0,0,1,1,1,1,0,0,0};
	unordered_map<string, vector<long double>> output;
	vector<Target>dummyVector;
	//output = SigAnalysis::z_score_thresholding(testInput);// , dummyVector);
	vector<long double>tmp = output["signals"];
	
	if (tmp == expectedResults)
	{
		qDebug() << "Sig Analysis z score test OK........";
	}
	else
	{
		qDebug() << "Vector Calc test failed!  Results returned are:";
		for (std::vector<long double>::iterator it = tmp.begin(); it != tmp.end(); ++it)
		{
			qDebug() << static_cast<double>(*it);
		}
	}
	return;
}



	
		
