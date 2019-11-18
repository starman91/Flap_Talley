/////////////////////////////////////////////////////////////////////////////
//		    		ft_main.cpp
//
/// Main program controlling trhe GUI thread
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "ft_main.h"

/// Constructor
FTMain::FTMain(QWidget *parent) : QMainWindow(parent)
{
	main_ui.setupUi(this);
	   
	connect(main_ui.pushButton_exit, SIGNAL(clicked()), this, SLOT(close()));
	connect(main_ui.pushButton_startSim, SIGNAL(toggled(bool)), this, SLOT(startSimulation(bool)));
	connect(main_ui.pushButton_addTarget, SIGNAL(toggled(bool)), this, SLOT(addTarget(bool)));
	connect(main_ui.pushButton_flapTarget, SIGNAL(toggled(bool)), this, SLOT(flapTarget(bool)));
	connect(main_ui.pushButton_moveTarget, SIGNAL(toggled(bool)), this, SLOT(moveTarget(bool)));
	connect(main_ui.pushButton_findTarget, SIGNAL(toggled(bool)), this, SLOT(findTarget(bool)));
	connect(main_ui.pushButton_getFlapRate, SIGNAL(toggled(bool)), this, SLOT(computeFlapRate(bool)));

	connect(main_ui.horizontalSlider_size, SIGNAL(valueChanged(int)), this, SLOT(sizeChange(int)));
	connect(main_ui.horizontalSlider_flapAmp, SIGNAL(valueChanged(int)), this, SLOT(flapAmpChanged(int)));
	connect(main_ui.horizontalSlider_flapRate, SIGNAL(valueChanged(int)), this, SLOT(flapRateChanged(int)));
	connect(main_ui.horizontalSlider_lag, SIGNAL(valueChanged(int)), this, SLOT(lagChange(int)));
	connect(main_ui.horizontalSlider_thresh, SIGNAL(valueChanged(int)), this, SLOT(thresholdChange(int)));
	connect(main_ui.horizontalSlider_infl, SIGNAL(valueChanged(int)), this, SLOT(influenceChange(int)));

	//Intitialize
	Init();
}

/// Destructor
FTMain::~FTMain()
{
	qDebug() << "Program terminated.";
	threads.shutdown();
	exit(0);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Initialize program. Sets up all plot areas and intitiales variables and flag states.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::Init(void)
{
	main_ui.label_found_flap_rate->setText("");

	// Start POSIX threads
	threads.startSystem();

	// Set up Radar Chart
	radarChart = new QChart();
	main_ui.chartView_radarData->setChart(radarChart);
	radarChart->legend()->hide();
	radarChart->createDefaultAxes();

	QValueAxis *axisY = new QValueAxis;
	radarChart->addAxis(axisY, Qt::AlignLeft);
	QValueAxis *axisX = new QValueAxis;
	radarChart->addAxis(axisX, Qt::AlignBottom);
	
	radarChart->axisX()->setRange(0, 100);
	radarChart->axisX()->setTitleText("Distance (m)");
	radarChart->axisY()->setRange(-1, 10);
	radarChart->axisX()->setTitleText("Signal Strength");
	radarChart->setTheme(QChart::ChartThemeBlueCerulean);
		
	// Set up Target Chart
	targetChart = new QChart();
	main_ui.chartView_targetData->setChart(targetChart);
	targetChart->legend()->hide();
	targetChart->createDefaultAxes();

	QValueAxis *axisY_T = new QValueAxis;
	targetChart->addAxis(axisY_T, Qt::AlignLeft);
	QValueAxis *axisX_T = new QValueAxis;
	targetChart->addAxis(axisX_T, Qt::AlignBottom);

	targetChart->axisY()->setRange(-3,3);
	targetChart->axisX()->setRange(0, 100);
	targetChart->axisX()->setTitleText("Samples");
	targetChart->setTheme(QChart::ChartThemeBlueCerulean);
	
	// Set up Flap Input Chart
	inputChart = new QChart();
	main_ui.chartView_targetInputData->setChart(inputChart);
	inputChart->legend()->hide();
	inputChart->createDefaultAxes();

	QValueAxis *axisY_I = new QValueAxis;
	inputChart->addAxis(axisY_I, Qt::AlignLeft);
	QValueAxis *axisX_I = new QValueAxis;
	inputChart->addAxis(axisX_I, Qt::AlignBottom);

	inputChart->axisY()->setRange(-3,3);
	inputChart->axisX()->setRange(0, 100);
	inputChart->axisX()->setTitleText("Samples");
	inputChart->setTheme(QChart::ChartThemeBlueCerulean);
	
	//Set values in status for initial GUI positions
	g_status.setSimTargetSize(main_ui.horizontalSlider_size->sliderPosition() / 33.0);
	g_status.setFlapAmp(main_ui.horizontalSlider_flapAmp->sliderPosition() / 100.0);
	g_status.setFlapRate(main_ui.horizontalSlider_flapRate->sliderPosition() / 10.0);
	g_status.setCalcLag(main_ui.horizontalSlider_lag->sliderPosition());
	g_status.setCalcThreshold(main_ui.horizontalSlider_thresh->sliderPosition()/10.0);
	g_status.setCalcInfluence(main_ui.horizontalSlider_infl->sliderPosition()/10.0);

	// Set lables for sliders based on stored values.
	main_ui.label_value_lag->setText(QString::number(g_status.getCalcLag()));
	main_ui.label_value_thresh->setText(QString::number(g_status.getCalcTheshold()));
	main_ui.label_value_inf->setText(QString::number(g_status.getCalcInfluence()));
	main_ui.label_value_flapAmp->setText(QString::number(g_status.getFlapAmp()));
	main_ui.label_value_flapRate->setText(QString::number(g_status.getFlapRate()));
	
	//Create callback loop for window updates		
	windowUpdateTimer = startTimer(10);			// run every 10 msec to handle operational controls
	
	//int value = 0;
	//value = main_ui.horizontalSlider_lag->sliderPosition();
	QLineSeries* incomingData = new QLineSeries();
												

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Call back from timer (see Init()) This creates the update loop for program
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::timerEvent(QTimerEvent *e)
{
	int t_id = e->timerId();
	if (t_id == windowUpdateTimer)
	{
		updateDiplay();
		if (g_status.getFindTarget())
			updateTargetPlot();
		if (g_status.getSimFlap() )
			updateFlapPlot();
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Call to update all values on display. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::updateDiplay(void)
{
	if (sem_recdata.available() < 1)
	{
		if (1000 == g_support.getInDataSize())
		{
			radarChart->removeAllSeries();
			QLineSeries* incomingData = new QLineSeries();
			//qDebug() << " In Data = " << QString::number(g_support.getInDataSize());
			//g_support.DisplayInData();
			incomingData->append(g_support.getInData());

			radarChart->addSeries(incomingData);
			incomingData->attachAxis(radarChart->axisY());
			incomingData->attachAxis(radarChart->axisX());

			if (g_status.getFindTarget())
			{
				// Shows tracking discovery line
				QLineSeries*trackingData = new QLineSeries();
				trackingData->setColor("red");
				trackingData->append(g_support.getTrackData());
				radarChart->addSeries(trackingData);
				trackingData->attachAxis(radarChart->axisY());
				trackingData->attachAxis(radarChart->axisX());

				// Marks maximum value point found during any target discovery
				QScatterSeries*targetingData = new QScatterSeries();
				targetingData->setColor("red");
				targetingData->setMarkerSize(5);
				targetingData->append(g_support.getTargetPointData());
				radarChart->addSeries(targetingData);
				targetingData->attachAxis(radarChart->axisY());
				targetingData->attachAxis(radarChart->axisX());
			}

			radarChart->createDefaultAxes();
			radarChart->axisX()->setRange(0, 100);
			radarChart->axisY()->setRange(-1, 10);
			g_support.clearInData();		//Clear the data bucket
			sem_recdata.release(1);
		}
	}

	if (g_status.getCalcFlap())
		main_ui.label_found_flap_rate->setText(QString::number(g_status.getFoundFlapRate(), 'f', 3));
	else
		main_ui.label_found_flap_rate->setText("");

	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Plot showing the composite discovered target signal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::updateTargetPlot(void)
{
	targetChart->removeAllSeries();
	QLineSeries* targetData = new QLineSeries();
	if (g_status.getPlotTargetAmplitude() )
	{
		targetData->append(g_support.getPlotAmplitudeData());

		QList<QPointF> test = targetData->points();
		targetChart->addSeries(targetData);
		targetData->attachAxis(targetChart->axisY());
		targetData->attachAxis(targetChart->axisX());
		g_status.setPlotTargetAmplitude(false);
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Plot showing the input flapping signal merged into the simulation target
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::updateFlapPlot(void)
{
	inputChart->removeAllSeries();
	QLineSeries* flapData = new QLineSeries();
	if (g_status.getPlotFlap() )
	{
		flapData->append(g_support.getPlotFlapData());
		inputChart->addSeries(flapData);
		flapData->attachAxis(inputChart->axisY());
		flapData->attachAxis(inputChart->axisX());
		g_status.setPlotFlap(false);
	}
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Button "Start" activates this and begins simulation. All plots are cleared on start.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::startSimulation(bool state)
{
	qDebug() << "Starting data simulation";
	g_status.setSimMode(state);
	radarChart->removeAllSeries();
	if (!state)
	{
		radarChart->removeAllSeries();
		inputChart->removeAllSeries();
		targetChart->removeAllSeries();
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Button "Add Target" triggers this and notfys system to add simulated target signal 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::addTarget(bool state)
{
	qDebug() << "Adding target";
	g_status.setSimTarget(state);
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Button "Flap" trigfgers this and notifys systme thrteads to actiate simualted flapping signal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::flapTarget(bool state)
{
	qDebug() << "Making targets flap";
	g_status.setSimFlap(state);
	if (!state)
	{
		inputChart->removeAllSeries();
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UNDER CONSTRUCTION: Button "Move Target" triggers this and notifys system to track target back and forth over an area in
/// simulated signal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::moveTarget(bool state)
{
	qDebug() << "Making targets move";
	g_status.setSimMove(state);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::findTarget(bool state)
{
	qDebug() << "Toggling target find";
	g_status.setFindTarget(state);
	if (!state)
	{
		targetChart->removeAllSeries();
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to double value for target size
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::sizeChange(int position)
{
	g_status.setSimTargetSize(position / 33.0);	
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to double value, scaled for statistical lag for target finding
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::lagChange(int position)
{
	g_status.setCalcLag(position);
	main_ui.label_value_lag->setText(QString::number(g_status.getCalcLag()));
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to double value, scaled for statistical threshold for target finding
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::thresholdChange(int position)
{
	g_status.setCalcThreshold(position / 10.0);
	main_ui.label_value_thresh->setText(QString::number(g_status.getCalcTheshold()));
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to double value, scaled for statistical influence for target finding
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::influenceChange(int position)
{
	g_status.setCalcInfluence(position /10.0);
	main_ui.label_value_inf->setText(QString::number(g_status.getCalcInfluence()));
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to doulbe value for target size
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::flapAmpChanged(int position)
{
	g_status.setFlapAmp(position/100.0);
	main_ui.label_value_flapAmp->setText(QString::number(g_status.getFlapAmp()));
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translate int slider position to doulbe value for target size
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::flapRateChanged(int position)
{
	g_status.setFlapRate(position /10.0);
	main_ui.label_value_flapRate->setText(QString::number(g_status.getFlapRate()));
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Triggers system to begin analyzing flap rate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FTMain::computeFlapRate(bool state)
{
	g_status.setCalcFlap(state);
	return;
}
