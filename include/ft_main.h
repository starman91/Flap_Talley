/////////////////////////////////////////////////////////////////////////////
//		    		ft_main.h
//
//              -------------------
/**
 * @file ft_main.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Operational class forcontrolling all GUI interactions. All user activity is triggered from here and distributed to other classes.
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer> 
#include <QChartView> 
#include "ui_ft_main.h"
#include "support.h"
#include "thread_control.h"

/// QMainwindow based class. Operational class for controlling all GUI interactions. All displaying of data and controlling of parameters is handled within this class.
class FTMain : public QMainWindow
{
	Q_OBJECT

	public:
		FTMain(QWidget *parent = Q_NULLPTR);
		~FTMain();

	public slots:
		//void close(void);
		void startSimulation(bool);
		void addTarget(bool);
		void flapTarget(bool);
		void moveTarget(bool);
		void findTarget(bool);
		void sizeChange(int);
		void lagChange(int);
		void thresholdChange(int);
		void influenceChange(int);
		void flapAmpChanged(int);
		void flapRateChanged(int);
		void computeFlapRate(bool);

	protected:
		void timerEvent(QTimerEvent *);

	private:
		void Init(void);
		void updateDiplay(void);
		void updateFlapPlot(void);
		void updateTargetPlot(void);

	private:
		/// instance of the GUI
		Ui::FTMainWindow main_ui;	
		/// Instance of thread start and control class
		ThreadControl threads;			
		/// Call back timer to update GUI
		int windowUpdateTimer;		
		/// Chart for dedisplaying radar signal
		QChart* radarChart;				
		/// Chart for displaying identified target flapping data
		QChart* targetChart;
		/// Chart for displaying flapping data input
		QChart* inputChart;
};
extern QSemaphore sem_recdata;
extern Support g_support;