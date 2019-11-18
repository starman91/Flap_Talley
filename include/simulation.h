/////////////////////////////////////////////////////////////////////////////
//		    		simulation.h
//
//              -------------------
/**
 * @file simulation.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Threaded main class for creating a simulated target. Applies flapping from Flapper() class if needed
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
#include "flapper.h"


/// QThread based class. Used to generate a simulated pulse target. Combines data from the Flapper() class if needed.
class Simulation : public QThread
{
	public:
		Simulation();
		~Simulation();

		virtual void run(void);
		void shutdown(void);

		inline void setKillThread(const bool state)						{ m_killThread = state;					};
		inline bool getKillThread(void) const								{ return m_killThread;					};

		inline QList<QPointF> getRawDataPoints(void) const		{ return g_support.getInData();		};

	private:
		bool m_killThread;
		int m_loopTime;
		QList<QPointF> l_dataPoints;
};
extern Support g_support;
extern Status g_status;
extern QSemaphore sem_recdata;
