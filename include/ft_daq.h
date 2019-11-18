/////////////////////////////////////////////////////////////////////////////
//		    		ft_daq
//
//              -------------------
/**
 * @file ft_daq.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Threaded class for handling data moving to and from radar. Also contains sections for bringing in simulated data
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
#include "target.h"

/// QThread based class. Main data acquisition point. Future radar data and current simulation data merges here and analysis of the signals begins.
class FTDAQ : public QThread
{
	Q_OBJECT

	public:
		FTDAQ(void);
		~FTDAQ(void);
		virtual void run(void);
		void shutdown(void);

		inline void setKillThread(const bool state)						{ m_killThread = state;					};
		inline bool getKillThread(void) const								{ return m_killThread;					};
	
	private:
		void testAnalysisVectorStats(void);		// Used to test against the VectorStats class.
		std::vector<long double>convertListToVector(QList<QPointF>&);
		void storeTargetTriggerPoints(std::vector<long double>&);
		void storeTargetAmplitudePoints(std::vector<long double>&, std::vector<long double>&);
	
	private:
		/// A vector of discovered targets
		std::vector<Target> v_targets;		
		
		QList<QPointF> l_dataPoints;
		QList<QPointF> l_flapPoints;
		/// monitoring loop time in milliseconds
		int m_loopTime;	
		/// flag to tell thread to drop out.
		bool m_killThread;
		/// flag to indcate system has been intitialized
		bool m_initialize;			
		
		//For Simulation work
		/// position of x referenced to the simulated radar signal. Range is from 0 to frequency in Hz (usually 0-100000)
		double m_x;			
};
extern Support g_support;
extern Status g_status;
extern QSemaphore sem_recdata;
extern QSemaphore sem_flapdata;