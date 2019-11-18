/////////////////////////////////////////////////////////////////////////////
//		    		ft_calc.h 
//
//	Multi thread class for controlling calculations of data so they are
// kept separate and do not slow down GUI processing.
//              -------------------
/**
 * @file ft_calc.h
 * @author Robert R. White
 * @date  Tue Oct 1 2019
 * @brief Threaded Class to handle triggering any intesive signal processing calculations
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

/// QThread based class. Used to offboard and control triggering of calculations, keeping DAQ speed at needed rate. 
class FTCalc : public QThread
{
	Q_OBJECT

	public:
		FTCalc(void);
		~FTCalc(void);
		virtual void run(void);
		void shutdown(void);

		inline void setKillThread(const bool state)						{ m_killThread = state;					};
		inline bool getKillThread(void) const								{ return m_killThread;					};

	private:
		bool m_killThread;
		int m_loopTime;

	
};
extern Support g_support;
extern Status g_status;
