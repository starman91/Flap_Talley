/////////////////////////////////////////////////////////////////////////////
//		    		support.h
//
// 
//              -------------------
/**
 * @file support.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Global class for handling utility functions and storage classes to support program operations. Also contains data buffers used to hold full data streams from other threads.
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtWidgets>
#include <QtCore>
#include <fstream>
#include "status.h"

/// A global class to provide common storage points for data or simple static functions needed by several other classes.  
class Support
{
	public:
		Support(void);
		~Support(void);

		// Support methods
		static int roundThisNumber(const double);
		static QString toStringRemoveTrailZeros( qreal num );
		static int dialog (int , QWidget*, QString , QString );
		static QString intToZeroPrependStr(int, int);
		static QString appendZeroToDecimal(double);

		inline void addInData(QList<QPointF> points)				{ QMutexLocker locker(&mutex_support); inData = points;							};		// Add list of x-y points to dataset
		inline void clearInData(void)											{ QMutexLocker locker(&mutex_support); inData.clear();								};		// clear all points from dataset
		inline QList<QPointF> getInData(void) const					{ QMutexLocker locker(&mutex_support); return inData;								};		// returns the points from dataset as a QList
		inline int getInDataSize(void) const								{ QMutexLocker locker(&mutex_support); return inData.size();						};		// gets QList size
		void DisplayInData(void) const;

		inline void addSimData(QList<QPointF> points)				{ QMutexLocker locker(&mutex_support); simulationData = points;				};		// Add list of x-y points to dataset
		inline void clearSimData(void)										{ QMutexLocker locker(&mutex_support); simulationData.clear();					};		// clear all points from dataset
		inline QList<QPointF> getSimData(void) const				{ QMutexLocker locker(&mutex_support); return simulationData;					};		// returns the points from dataset as a QList
		inline int getSimDataSize(void) const							{ QMutexLocker locker(&mutex_support); return simulationData.size();			};		// gets QList size

		inline void addTrackData(QList<QPointF> points)			{ QMutexLocker locker(&mutex_support);  trackData = points;						};		// Add list of x-y points to dataset
		inline void clearTrackData(void)									{ QMutexLocker locker(&mutex_support);  trackData.clear();							};		// clear all points from dataset
		inline QList<QPointF> getTrackData(void) const			{ QMutexLocker locker(&mutex_support);  return trackData;							};		// returns the points from dataset as a QList
		
		inline void addTargetPointData(QList<QPointF> points) {QMutexLocker locker(&mutex_support);   targetpointData = points;			};		// Add list of x-y points to dataset
		inline void clearTargetPointData(void)							 { QMutexLocker locker(&mutex_support); targetpointData.clear();				};		// clear all points from dataset
		inline QList<QPointF> getTargetPointData(void) const	 { QMutexLocker locker(&mutex_support);  return targetpointData;				};		// returns the points from dataset as a QList

		inline void addAllFlapData(QList<QPointF> points)		{ QMutexLocker locker(&mutex_support);  flapData = points;						};		// Add list of x-y points to dataset
		void addFlapData(QPointF point);																																								// Add list of x-y points to dataset
		inline void clearFlapData(void)										{ QMutexLocker locker(&mutex_support);  flapData.clear();							};		// clear all points from dataset
		inline QList<QPointF> getFlapData(void) const				{ QMutexLocker locker(&mutex_support);  return flapData;							};		// returns the points from dataset as a vector
		inline int getFlapDataSize(void) const							{ return flapData.size();																					};
		void DisplayFlapData(void) const;

		inline QList<QPointF> getPlotFlapData(void) const		{ QMutexLocker locker(&mutex_support);  return plotFlapData;					};		// returns the points from dataset as a QList
		inline int getPlotFlapDataSize(void) const						{ return plotFlapData.size();																			};

		inline void addAllTargetData(QList<QPointF> points)	{ QMutexLocker locker(&mutex_support);  targetData = points;						};		// Add list of x-y points to dataset
		void addTargetData(QPointF point);
		inline void clearTargetData(void)									{ QMutexLocker locker(&mutex_support);  targetData.clear();						};		// clear all points from dataset
		inline QList<QPointF> getTargetData(void) const			{ QMutexLocker locker(&mutex_support);  return targetData;						};		// returns the points from dataset as a QList

		inline QList<QPointF> getPlotAmplitudeData(void) const	{ QMutexLocker locker(&mutex_support);  return plotTargetAmplitudeData;				};		// returns the points from dataset as a QList
		inline int getPlotAmplitudeDataSize(void) const				{ return plotTargetAmplitudeData.size();																	};

		int getFirstTimePoint(void) const;
		int getLastTimePoint(void) const;

	public:
		QList<QPointF>inData;										// dataset of points to plot
		QList<QPointF>trackData;									// matrix data points used for identfying a target
		QList<QPointF>targetpointData;						// center of target data 
		
		QList<QPointF>simulationData;							//Buffer for simulated target data (pushed by Simulation, pulled by FTDAQ class) 
		QList<QPointF> flapData;									//Temporary spot to store input of flapping for calculation
		QList<QPointF> plotFlapData;							//Temporary spot to store flap data for plotting
		QList<QPointF> targetData;								//Temporary spot to store found and monitored target data.
		QList<QPointF> plotTargetAmplitudeData;		//Temporary spot to store amplitude data fro plotting and calculation.

	private:
		QString m_statusMessage;								// To be dispayed in status label of status bar
		QString m_infoMessage;									// To be dispayed in info label of status bar

		int m_sampleSize;												// Maximum number of flap data points held in storage for analysis, based on sample rate
		mutable QMutex mutex_support;
};
extern QSemaphore sem_flapdata;
extern Status g_status;
