//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		thread_control.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "thread_control.h"

ThreadControl::ThreadControl()
{
}

ThreadControl::~ThreadControl()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Master function to begin spinning up several thread process from a single start
/// call orginating from the main function. Controls setting up varaibles based on
/// config values. Kick starts the threads and sets them (via main) into the process
/// loop for Qt.
///
/// Is using Qt based POSIX threading to avail cross platform porting if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ThreadControl::startSystem()
{
	thread_ftdaq.start();
	thread_ftcalc.start();
	thread_sim.start();
	thread_flap.start();
	return true;
}



//////////////////////////////////////////////////////////////////////////////
/// Call back from timer. Verify threads are running
////////////////////////////////////////////////////////////////////////////////
void ThreadControl::threadCheck(void)
{
	return;
}


//////////////////////////////////////////////////////////////////////////////
/// Shutdowns all running threads
////////////////////////////////////////////////////////////////////////////////
void ThreadControl::shutdown(void)
{
	if (thread_ftdaq.isRunning() )
	{
		while (!thread_ftdaq.isFinished() )
		{
			thread_ftdaq.setKillThread(true);
			if (!thread_ftdaq.wait(1000) )
			{
				qDebug() << "Thread thread_ftdaq exited normally";
			}
			else
			{
				qDebug() << "Timeout, thread_ftdaq did not complete in time";	
			}
		}
	}

	if (thread_ftcalc.isRunning() )
	{
		while (!thread_ftcalc.isFinished() )
		{
			thread_ftcalc.setKillThread(true);
			if (!thread_ftcalc.wait(1000) )
			{
				qDebug() << "Thread thread_ftcalc exited normally";
			}
			else
			{
				qDebug() << "Timeout, thread_ftcalc did not complete in time";	
			}
		}
	}
	if (thread_flap.isRunning() )
	{
		while (!thread_flap.isFinished() )
		{
			thread_flap.setKillThread(true);
			if (!thread_flap.wait(1000) )
			{
				qDebug() << "Thread thread_flap exited normally";
			}
			else
			{
				qDebug() << "Timeout, thread_flap did not complete in time";	
			}
		}
	}
	if (thread_sim.isRunning() )
	{
		while (!thread_sim.isFinished() )
		{
			thread_sim.setKillThread(true);
			if (!thread_sim.wait(1000) )
			{
				qDebug() << "Thread thread_sim exited normally";
			}
			else
			{
				qDebug() << "Timeout, thread_sim did not complete in time";	
			}
		}
	}
	return;
}
