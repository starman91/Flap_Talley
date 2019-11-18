/////////////////////////////////////////////////////////////////////////////
//		    		flapper.h
//
//              -------------------
/**
 * @file flapper.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Threaded class to create a sinusoidal signal that is applied to a simulated target to represent flapping
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <vector>
#include <QtCore>
#include "support.h"
#include "status.h"

/// QThread based class. Creates a sinusoidal signal that can be accessed by the Simulation() class to create a flapping effect on the target.
class Flapper : public QThread
{
	public:
		Flapper();
		~Flapper();

		virtual void run(void);
		void shutdown(void);

		inline void setKillThread(const bool state)						{ m_killThread = state;					};
		inline bool getKillThread(void) const								{ return m_killThread;					};

		//inline QList<QPointF> getRawDataPoints(void) const		{ return l_dataPoints;						};

	private:
		bool m_killThread;
		int m_loopTime;
};
extern Support g_support;
extern Status g_status;
extern QSemaphore sem_recdata;
