/////////////////////////////////////////////////////////////////////////////
//		    		main.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "ft_main.h"
#include <QtWidgets/QApplication>
#include "status.h"
#include "support.h"

#ifdef WIN32
	#include "windows.h"
#endif

#include "vector_stats.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <numeric>


using namespace std;

typedef long double ld;
typedef unsigned int uint;
typedef std::vector<ld>::iterator vec_iter_ld;


//////////////////////////////////////////////////////////////
//																						 //
//*************************************************//
//                         MAIN													//
//*************************************************//
//																						//

/*! \mainpage
 * \copyright  2019 Robert White and NREL. This project is released under the GNU Public License 
 * 
 *
 * <b> Overview </b>\n 
 * Flap Talley is a program used to analyze radar signals of avian targets
 * in an effort to monitor and determine the flap rate, wing beat, of the 
 * target. By identifying flap ratesin the range of a normal avian species,
 * systems can be alerted in an effort to avoid harmiong the animals. Flap
 * rates higher than a normal avian species could indicate the presence of
 * swarms of insects or others that are not in danger. By only alerting in
 * when necessary systems can remain online and not suffer downtime responding
 * to targets that are unimportant.
 *\n\n 
 * <b> Code Base </b>\n
 * Flap Talley is built on C++ using a gcc compiler. the code is wirteen to be used
 * on the Windows 10 operating system. Additional libraries that are important:
 * * Qt 5.0+   - The Gui libray. Items are built in designer and then connected into code (\link https://www.qt.io/ \endlink)
 * * FFTW - A Fourier Transform library built for speed and efficiency (\link http://www.fftw.org/ \endlink)
 *\n\n 
 * <b>Operation</b>\n
 * The code consists of two main elements. One is for simulating a target and checking the process
 * and methods for analyzing targets. The second is a data acquisition section that retreives data from
 * a data aquisition card supplied to capture the radar signal. Once the target signal is received, regardless
 * of the source, it follows the same analysis process.
 * \n\n
 * The software is comprised of a series of POSIX threaded operations to allow for data acquisition, 
 * simulation (2 threads), and GUI operations to function indepedently and not interfere with processes
 * occuring on other threads. Each of the threads can communicate via Mutex triggers or through 
 * blocking flags.
 *\n\n
 *
*/
///
////////////////////////////////////////////////////////////////


Support g_support;							// Global class access to the support functions and dialog windows
Status g_status;								// Global class access to the support functions and dialog windows

QSemaphore sem_recdata(1);			// semaphore to alert about new data available
QSemaphore sem_flapdata(1);			// semaphore to alert about new flap data available

/// Main entry point to the program. Intiates first stack insert to control GUI thread. 
/// Triggers thread starting class to intiiate other operations.
int main(int argc, char *argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication mainwindow(argc, argv);
	
	// Activate Main GUI and start Qt daemon
	FTMain ftmain;
	ftmain.show();

	return mainwindow.exec();
}

