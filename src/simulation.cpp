/////////////////////////////////////////////////////////////////////////////
//		    		simulation.cpp
//
// Separate thread to create an asynchronus signal to be merged
// with a simulated target in the ft_daq class. This produces a 
// varying amplitude ( flapping) signature on top of the base target 
// simulated radar pulse.
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "simulation.h"


Simulation::Simulation()
{
	m_loopTime = 10;
	m_killThread = false;
	return;
}

Simulation::~Simulation()
{
	return;
}


///////////////////////////////////////////////////////////////////////////////////
/// Master processing loop for createing a data simulation of flapping 
///////////////////////////////////////////////////////////////////////////////////
void Simulation::run(void)
{
	//Monitor testing
	//QFile file("flapping_input_amplitudes.csv");
      //if (!file.open(QIODevice::Append| QIODevice::Text))
         // return;
     //QTextStream out(&file);
	
	int maxY = 1000;
	double mu = 70.0;
	double sig = 0.50;
	
	for (;;)
	{
		if (g_status.getSimMode())
		{
			l_dataPoints.clear();
			double xVal = 0.0;
			for (int i = 0; i < maxY; i++)
			{
				QPointF newPoint;
				// Build points
				newPoint.setX(xVal);
				double noise = ((double)rand() / (RAND_MAX));		// Random white noise pattern
				double shape = 0.0;   //(-0.05 * xVal + 4) + 1 / xVal;		// to throw big noise into trend : decaying assymptotic curve, rather than flat

				// Create simulated target pulse. If flpping has been selcted, then flapping value adds to amplitude (see simulation class)
				double pulse = 0.0;
				if (g_status.getSimTarget())
				{
					// Base data is a gaussian y= A * e^( -(x-b)^2 / (2*c^2) ): A= amplitude, b=mean, c=standard deviation(width)
					pulse = (g_status.getSimTargetSize() + g_status.getFlapVal()) * exp(-(pow((xVal - mu), 2.0)) / (2 * pow(sig, 2.0)));
					//if (g_status.getSimFlap())
						//out <<  QTime::currentTime().msecsSinceStartOfDay() << ", " << g_status.getFlapVal() << "\n";
				}
				// Add any noise or pulse shaping
				newPoint.setY(noise / 8.0 + shape + pulse);
				l_dataPoints.push_back(newPoint);

				xVal += 0.1;
			}
			g_support.addInData(l_dataPoints);		// Store new full target data to bucket
			g_support.addSimData(l_dataPoints);		// Store new full target data to analysis
			
			//End testing
			sem_recdata.acquire(1);						// Notify new data available
		}

		// Stop running thread if signalled
		if (m_killThread)
		{
			//file.close();
			// Report shutdown
			qDebug() << "Simulation thread has been stopped";
			//Shutdown and exit thread
			exit();
			return;
		}
		usleep(1);
	}
	return;
}
