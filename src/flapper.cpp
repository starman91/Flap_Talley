/////////////////////////////////////////////////////////////////////////////
//		    		flapper.cpp
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

#include "flapper.h"


Flapper::Flapper()
{
	m_loopTime = 10;
	m_killThread = false;
	return;
}

Flapper::~Flapper()
{
	return;
}


///////////////////////////////////////////////////////////////////////////////////
/// Master processing loop for createing a data simulation of flapping 
///////////////////////////////////////////////////////////////////////////////////
void Flapper::run(void)
{
	//Monitor testing
	//QFile file("flapping.csv");
      //if (!file.open(QIODevice::Append| QIODevice::Text))
          //return;
     //QTextStream out(&file);

	 // Start flapping
	// Begin data simulation loop
	int secCount = 0;
	int time = 100;			
	double timePoint = 0.0;
	double flap = 0.0;
	double m_time = 100.0;

	for (;;)
	{
		// Simulate flapping, deliver it through amplitude variable
		if (g_status.getSimFlap())
		{
			QPointF flapPoint;		//Used to track input to flapping
			double timePoint = static_cast<double>(secCount) / static_cast<double>(time);
			flap = (g_status.getFlapAmp() * sin((2.0 * M_PI * timePoint) / (1.0 / g_status.getFlapRate())));
			(secCount < m_time) ? secCount++ : secCount = 0;
			//Store simulated flap input data, use simple timestamp for x value.
			g_status.setFlapVal(flap);
			flapPoint.setX(QTime::currentTime().msecsSinceStartOfDay());
			flapPoint.setY(flap);
			g_support.addFlapData(flapPoint);
			//if (g_status.getSimFlap())
				//out << QTime::currentTime().msecsSinceStartOfDay() << ", " << flap << "\n";
		}
		else
		{
			g_status.setFlapVal(0.0);
		}
		
		// Stop running thread if signalled
		if (m_killThread)
		{
			//file.close();
			// Report shutdown
			qDebug() << "Flapper thread has been stopped";
			//Shutdown and exit thread
			exit();
			return;
		}
		// End for loop
		msleep(10);
	}
	return;
}