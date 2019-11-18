/////////////////////////////////////////////////////////////////////////////
//		    		support.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "support.h"


enum dialog_types {INFORMATION, QUESTION, WARNING, CRITICAL, CONSTRUCTION, SAVE	};

Support::Support(void)
{
	m_sampleSize = 200;
	return;
}

Support::~Support(void)
{
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Creates notification dialog boxes as necessary.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Support::dialog (int type, QWidget *parent, QString title, QString message)
{
	int retVal;
	switch (type)
	{
		case INFORMATION:
			retVal = QMessageBox::information(parent, title, message, QMessageBox::Ok);
			break;
		case QUESTION:
			retVal = QMessageBox::question(parent, title, message, QMessageBox::Yes | QMessageBox::No);
			break;
		case WARNING:
			retVal = QMessageBox::warning(parent, title, message, QMessageBox::Ok | QMessageBox::Cancel);
			break;
		case CRITICAL:
			retVal = QMessageBox::critical(parent, title, message, QMessageBox::Ok | QMessageBox::Cancel);
			break;
		case CONSTRUCTION:
			retVal = QMessageBox::information(parent, "Man At Work!!!", "This feature is still under construction", QMessageBox::Ok);
			break;
		case SAVE:
			retVal = QMessageBox::information(parent, title, message, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			break;
		default:
			retVal=-1;
			std::cout<<"ERROR: Bad dialog type: "<< type<<"\n";
			break;
	}
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// takes an integer number passed in, converts to string and prepends requested number of 0s.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Support::intToZeroPrependStr(int value, int numDigits)
{
	int diff;
	QString text = QString::number(value);
	(0 < value) ? diff = numDigits - text.length() :	diff = numDigits-1;		//compensate for 0 passed in as value

	if ( 0 < diff )
	{
		for (int i=1; i<=diff; i++)
		{
			text.prepend("0");
		}
	}
	return text;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// takes a number passed in, converts to string and appends two zeros after decimal place for whole numbers
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Support::appendZeroToDecimal(double value)
{
	QString fixedNumber = QString::number(value, 'f', 4);

	return fixedNumber;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Method to compensate for the lack of a rounding method in Microsoft's OS. This function will round a number by 
/// convention of using a boundary of 0.5 and the MS floor() function. Linux distros can use the standard built in round 
///  function.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Support::roundThisNumber(const double oldNum)
{
	#ifdef WIN32
		return floor(oldNum + 0.5 );
	#endif

	#ifdef LINUX
		return round(oldNum);
	#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Method to remove trailign zeros from a number and return the value as a string
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Support::toStringRemoveTrailZeros( qreal num )
{
    QString str = QString::number( num, 'f', 4 );

    str.remove( QRegExp("0+$") ); // Remove any number of trailing 0's
    str.remove( QRegExp("\\.$") ); // If the last character is just a '.' then remove it

    return str;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Support::getFirstTimePoint(void) const
{
	if (flapData.isEmpty())
	{
		return 0;
	}
	QPointF first = flapData.first();
	return static_cast<int>(first.x());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Support::getLastTimePoint(void) const
{
	if (flapData.isEmpty() || flapData.size() < 2 )
	{
		return 2;
	}
	
	QPointF last = flapData.last();
	return static_cast<int>(last.x());
}


///////////////////////////////////////////////////////////////////////////////////
/// Keeps a list of flap data points up to a max value
///////////////////////////////////////////////////////////////////////////////////
 void Support::addFlapData(const QPointF point)
 { 
	QMutexLocker locker(&mutex_support); 
	
	flapData.append(point);	
	if (m_sampleSize < flapData.size())
	{
		flapData.pop_front();
		if ( !g_status.getPlotFlap())
		{
			plotFlapData.clear();
			// Reset X values to simple integers
			for (int i = 1; i < flapData.size(); i++)
			{
				QPointF plotPoint;
				plotPoint.setX(i);
				plotPoint.setY(flapData.at(i).y());
				plotFlapData.append(plotPoint);
			}
			g_status.setPlotFlap(true); // Blocking flag to trigger plotting on main thread
		}
	}
	
	return;
 };


 //////////////////////////////////////////////////////////////////////////////////
/// Keeps a list of target amplitude data points up to a max value for analysis
///////////////////////////////////////////////////////////////////////////////////
 void Support::addTargetData(const QPointF point)
 { 
	QMutexLocker locker(&mutex_support); 
	
	targetData.append(point);	
	if (m_sampleSize < targetData.size())
	{
		targetData.pop_front();
		if (!g_status.getPlotTargetAmplitude())
		{
			plotTargetAmplitudeData.clear();
			// Reset X values to simple integers
			for (int i = 1; i < targetData.size(); i++)
			{
				QPointF plotPoint;
				plotPoint.setX(i);
				plotPoint.setY(targetData.at(i).y());
				plotTargetAmplitudeData.append(plotPoint);
			}
			if (g_status.getCalcFlap() && !g_status.getCalculating())
			{	
				g_status.setCalculating(true);   // Blocking flag to trigger calculation on ft_calc thread 
			}
			//TODO: Add section to pull in specific target and add amplitude data to target instance not a general reference.
			g_status.setPlotTargetAmplitude(true); // Blocking flag to trigger plotting on main thread
		}
	}
	
	return;
 };


 ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
 void Support::DisplayFlapData(void)const
 { 
	 for (int i = 0; i < flapData.size(); i++)
	 {
		 qDebug() << flapData.at(i).y();
	 }
 };

 ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
 void Support::DisplayInData(void)const
 { 
	 for (int i = 0; i < inData.size(); i++)
	 {
		 qDebug() << inData.at(i).x() << ",  " << inData.at(i).y();
	 }
 };

