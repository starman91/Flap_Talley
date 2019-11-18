/////////////////////////////////////////////////////////////////////////////
//		    		sig_analysis.h
//
//              -------------------
/**
 * @file sig_analysis.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Class of static functions for performing analysis on signal data 
*/
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <numeric>
#include "support.h"
#include "status.h"
#include "vector_stats.h"
#include "target.h"

using namespace std;

/// A class containing a sereis of static functions to perform signal processing. 
class SigAnalysis
{
	public:
		SigAnalysis(void);
		~SigAnalysis(void);

		static unordered_map<string, vector<long double>> z_score_thresholding(vector<long double>&, vector<Target>& );
		static QList<long double> rollingAverage(QList<long double>&);
		static double computeFFT(QList<QPointF>&);
	
	private:
	

};
extern Support g_support;
extern Status g_status;
extern QSemaphore sem_recdata;
