//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		thread_control.h
//
//              -------------------
/**
 * @file thread_control.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Master control class for intializing, performing start up, and control of all the process threads
*/
// copyright    : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////


#pragma once

#include <QtCore>
#include "support.h"
#include "status.h"
#include "ft_daq.h"
#include "ft_calc.h"
#include "simulation.h"
#include "flapper.h"


/// QObject based class. The class controls the starting and stopping of all the ruinning threads. Is triggered by the start up of the FTMain() class
class ThreadControl : public QObject
{
	public:
		ThreadControl();
		~ThreadControl();

		bool startSystem(void);
		void shutdown(void);

	public:
		FTDAQ thread_ftdaq;						// Data Acquisition process
		FTCalc thread_ftcalc;
		Simulation thread_sim;
		Flapper thread_flap;

	private slots:
		virtual void threadCheck(void);


	private:
		QTimer *threadWatchTimer;
		int m_threadWatchTimer;					//timer id
};
extern Support g_support;
extern Status g_status;
