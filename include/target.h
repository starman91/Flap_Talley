/////////////////////////////////////////////////////////////////////////////
//		    		target.h
//
//              -------------------
/**
 * @file target.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief A storage class instance for target information. 
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////


#pragma once
#include <QtCore>
#include <QDialog>
#include <QTimer>
#include <QList>
#include "status.h"

using namespace std;

/// A storage class to hold all information concerning a target. An instance of this class should be generated for each current identified target.
//Partially implemented throughout software: TODO Fully implement class.
class Target
{
	public:
		Target(void);
		~Target(void);
		
		void setID(int); 
		void setCurrentAmplitude(long double); 
		inline void setPosition(const int value)									{ m_targetPosition = value;			};
		inline int getPosition(const int value)									{ return m_targetPosition;				};
	
		inline void clearAmplitudes(void)											{ l_amplitudes.clear();					};
		inline QList<QPointF> getAmplitudes(void) const					{ return l_amplitudes;					};
		inline int getAmplitudesSize(void) const								{ return l_amplitudes.size();			};
		
		inline void clearSignal(void)													{ v_lastSignal.clear();						};
		inline vector<long double> getSignal(void) const					{ return v_lastSignal;						};
		inline int getSignalSize(void) const										{ return v_lastSignal.size();				};
		inline void setSignal(vector<long double> points)					{ v_lastSignal = points;					};

	private:
		/// Unique idenfier to represent target
		int m_id;
		/// Last found amplitude of target signal
		long double m_currentAmplitude;
		/// Position of target on radar signal
		int m_targetPosition;									
		vector<long double>v_lastSignal;				// Store for entire last peak signal
		/// List of last two seconds of data on target aplitudes
		QList<QPointF> l_amplitudes;					
};
extern Status g_status;



