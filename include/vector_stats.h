/////////////////////////////////////////////////////////////////////////////
//		    		vector_stats.h
//
//              -------------------
/**
 * @file vector_stats.h
 * @author Robert R. White
 * @date  Mon Aug 5 2019
 * @brief Support class used for calcualtion of target identification. Called by SigAnalysis() class
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

#include "status.h"

using namespace std;

typedef std::vector<long double>::iterator vec_iter_ld;

/// A support class to provide the calcualtions to locate a target in the signal.
class VectorStats
{
	public:
		VectorStats (vec_iter_ld start, vec_iter_ld end);
		~VectorStats();

		void compute();
		inline long double mean()							{	 return m1;	};
		inline long double standard_deviation()	 {	 return m2;	};

	private:
		vec_iter_ld start;	// First iterator position in vector 
		vec_iter_ld end;		// Last iterator position in vector
		long double m1;	// mean
		long double m2;	// standard deviation
};
