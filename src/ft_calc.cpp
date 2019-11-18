/////////////////////////////////////////////////////////////////////////////
//		    		ft_daq.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "ft_calc.h"
#include "sig_analysis.h"

#include <QtCharts>
#include <QLineSeries> 
#include <cmath>
#include <unordered_map>

FTCalc::FTCalc(void)
{
	m_loopTime = 1;
	m_killThread = false;
	return;
}

FTCalc::~FTCalc()
{
	return;
}

///////////////////////////////////////////////////////////////////////////////////
///Used to shutdown all devices to a 0 or default state
///////////////////////////////////////////////////////////////////////////////////
void FTCalc::shutdown(void)
{
	//Clean up here
	return;
}


///////////////////////////////////////////////////////////////////////////////////
/// Master processing loop for handling all calculation triggers
///////////////////////////////////////////////////////////////////////////////////
void FTCalc::run(void)
{
	//qDebug() << "Data calculation thread starting";
						
	for (;;)
	{
		if (g_status.getCalculating())
		{
			qDebug() << "Calculating!";
			//SigAnalysis::computeFFT(g_support.getPlotAmplitudeData());
			SigAnalysis::computeFFT(g_support.getPlotAmplitudeData());
			g_status.setCalculating(false);
		}
		usleep(m_loopTime);
	
		// Stop running thread if signalled
		if (m_killThread)
		{
			// Report shutdown
			qDebug() << "Calculation thread has been stopped";
			//Shutdown and exit thread
			exit();
			return;
		}
		// End of loop. Delay before repeating

	
	}
}

