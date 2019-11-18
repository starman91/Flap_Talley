/////////////////////////////////////////////////////////////////////////////
//		    		vector_stats.cpp
//
// Support class used for calcualtion of target identification. Called by SigAnalysis() class
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include "vector_stats.h"


VectorStats::VectorStats(vec_iter_ld start, vec_iter_ld end)
{
	this->start = start;   ///This is the iterator position of the start of the window,
	this->end = end;     /// This is the iterator position of the end of the window,
	this->compute();
};

VectorStats::~VectorStats()
{
	return;
}


/////////////////////////////////////////////////////////////////////////////
/// This method calculates the mean and standard deviation using STL functions.
/// This is the Two-Pass implementation of the Mean & Variance calculation.
/////////////////////////////////////////////////////////////////////////////
void VectorStats::compute()
{
	long double sum = std::accumulate(start, end, 0.0);
	unsigned int slice_size = std::distance(start, end);
	long double mean = sum / slice_size;
	std::vector<long double> diff(slice_size);
	std::transform(start, end, diff.begin(), [mean](long double x) { return x - mean; });
	long double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	long double std_dev = std::sqrt(sq_sum / slice_size);

	this->m1 = mean;
	this->m2 = std_dev;
};

	