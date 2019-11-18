/////////////////////////////////////////////////////////////////////////////
//		    		status.h
//
//              -------------------
/**
 * @file status.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Global class to hold the state of various states of software elements and hold temporary values from other threads
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <QMutex>

///Global class to hold the state of various states of software elements and hold temporary values from other threads
class Status 
{
	public:
		Status(void);
		~Status(void);
		void Init(void);			// Init class

		inline void setSysOK(const bool state)							{ QMutexLocker locker(&mutex_status); m_ok = state;					};
		inline bool getSysOK(void) const									{ QMutexLocker locker(&mutex_status); return m_ok;					};

		inline void setSimMode(const bool state)						{ QMutexLocker locker(&mutex_status); m_simMode = state;		};
		inline bool getSimMode(void) const								{ QMutexLocker locker(&mutex_status); return m_simMode;			};

		inline void setSimTarget(const bool state)						{ QMutexLocker locker(&mutex_status); m_simTarget = state;		};
		inline bool getSimTarget(void) const								{ QMutexLocker locker(&mutex_status); return m_simTarget;		};

		inline void setSimFlap(const bool state)						{ QMutexLocker locker(&mutex_status); m_simFlap = state;			};
		inline bool getSimFlap(void) const								{ QMutexLocker locker(&mutex_status); return m_simFlap;			};

		inline void setSimMove(const bool state)						{ QMutexLocker locker(&mutex_status); m_simMove = state;		};
		inline bool getSimMove(void) const								{ QMutexLocker locker(&mutex_status); return m_simMove;			};

		inline void setFindTarget(const bool state)					{ QMutexLocker locker(&mutex_status); m_findTarget = state;		};
		inline bool getFindTarget(void) const							{ QMutexLocker locker(&mutex_status); return m_findTarget;		};

		inline void setSimTargetSize(const double value)			{ QMutexLocker locker(&mutex_status); m_targetSize = value;		};
		inline double getSimTargetSize(void)							{ QMutexLocker locker(&mutex_status); return m_targetSize;		};

		inline void setCalcLag(const int value)							{ QMutexLocker locker(&mutex_status); m_calcLag = value;			};
		inline int getCalcLag(void)												{ QMutexLocker locker(&mutex_status); return m_calcLag;			};

		inline void setCalcThreshold(const double value)			{ QMutexLocker locker(&mutex_status); m_threshold = value;		};
		inline double getCalcTheshold(void)								{ QMutexLocker locker(&mutex_status); return m_threshold;		};

		inline void setCalcInfluence(const double value)			{ QMutexLocker locker(&mutex_status); m_influence = value;		};
		inline double getCalcInfluence(void)								{ QMutexLocker locker(&mutex_status); return m_influence;			};

		inline void setFlapAmp(const double value)					{ QMutexLocker locker(&mutex_status); m_flapAmp = value;			};
		inline double getFlapAmp(void)									{ QMutexLocker locker(&mutex_status); return m_flapAmp;			};

		inline void setFlapRate(const double value)					{ QMutexLocker locker(&mutex_status); m_flapRate = value;			};
		inline double getFlapRate(void)										{ QMutexLocker locker(&mutex_status); return m_flapRate;			};

		inline void setCalcFlap(const bool state)						{ QMutexLocker locker(&mutex_status); m_calcFlap = state;			};
		inline bool getCalcFlap(void)											{ QMutexLocker locker(&mutex_status); return m_calcFlap;			};

		inline void setFlapVal(const double value)						{ m_flapVal = value;			};
		inline double getFlapVal(void)										{ return m_flapVal;			};

		inline void setFoundFlapRate(const double value)			{ QMutexLocker locker(&mutex_status); m_calcRate = value;			};
		inline double getFoundFlapRate(void)							{ QMutexLocker locker(&mutex_status); return m_calcRate;			};

		inline void setCalculating(const bool state)					{ QMutexLocker locker(&mutex_status); m_calculating = state;		};
		inline bool getCalculating(void) const							{ QMutexLocker locker(&mutex_status); return m_calculating;		};

		inline void setPlotFlap(const bool state)						{ QMutexLocker locker(&mutex_status); m_plotFlap = state;			};
		inline bool getPlotFlap(void) const								{ QMutexLocker locker(&mutex_status); return m_plotFlap;			};
		
		inline void setPlotTargetAmplitude(const bool state)		{ QMutexLocker locker(&mutex_status); m_plotAmplitudes = state;			};
		inline bool getPlotTargetAmplitude(void) const				{ QMutexLocker locker(&mutex_status); return m_plotAmplitudes;			};
	
	private:
		/// Instrument is OK
		bool m_ok;		
		/// test flag, start simulation
		bool m_simMode;			
		/// add a target to simulation
		bool m_simTarget;		
		/// add flapping to targets in simulation
		bool m_simFlap;			
		/// addmovement to targets in simulation
		bool m_simMove;		
		/// Signals that the finding routine is active
		bool m_findTarget;					
		/// FFT found fklap rate of a target in Hz
		double m_calcRate;			
		/// Flag to indicate calculation process is busy
		bool m_calculating;			
		/// flag to control plotting of amplitude rate
		bool m_plotAmplitudes;			
		/// size of target to use in simulation (default 1.0)
		double m_targetSize;				

		// for target signal finder
		int m_calcLag;
		long double m_threshold;
		long double m_influence;
		
		//For flapping simulation
		/// simulated flap amplitude
		double m_flapAmp;	
		/// simulated flap rate in Hz
		double m_flapRate;	
		/// flag on whether to calculate flap rate or not
		bool m_calcFlap;					
		/// Current flapping amplitude
		double m_flapVal;					
		/// flag on whether to plot simulated flap rate
		bool m_plotFlap;						

		mutable QMutex mutex_status;
};


