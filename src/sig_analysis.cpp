/////////////////////////////////////////////////////////////////////////////
//		    		sig_analysis.cpp
//
// Methods for calculation of various aspects of a signal
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include <QtCharts>
#include <QLineSeries> 
#include <cmath>
#include "sig_analysis.h"


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

///Constructor
SigAnalysis::SigAnalysis(void)
{
	return;
}

///DeConstructor
SigAnalysis::~SigAnalysis()
{
	return;	
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// This is the implementation of the Smoothed Z-Score Algorithm.
/// This is direction translation of https://stackoverflow.com/a/22640362/1461896.
///
/// @param input - input signal
/// @param lag - the lag of the moving window
/// @param threshold - the z-score at which the algorithm signals
/// @param influence - the influence (between 0 and 1) of new signals on the mean and standard deviation
///returns a hashmap containing the filtered signal and corresponding mean and standard deviation.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unordered_map<string, vector<long double>> SigAnalysis::z_score_thresholding(vector<long double>& input, vector<Target>& v_targets)
{
	unordered_map<string, vector<long double>> output;
	int lag = g_status.getCalcLag();
	long double threshold = g_status.getCalcTheshold();
	long double influence = g_status.getCalcInfluence();

	vector<long double> signal(input.size());
	vector<long double> filtered_input(input.begin(), input.end());
	vector<long double> filtered_mean(input.size());
	vector<long double> filtered_stddev(input.size());

	VectorStats lag_subvector_stats(input.begin(), input.begin() + lag);
	filtered_mean[lag - 1] = lag_subvector_stats.mean();
	filtered_stddev[lag - 1] = lag_subvector_stats.standard_deviation();

	// Loop through ll available points in signal
	for (int i = lag; i < input.size(); i++) 
	{
		if (abs(input[i] - filtered_mean[i - 1]) > threshold * filtered_stddev[i - 1]) 
		{
			signal[i] = (input[i] > filtered_mean[i - 1]) ? 1.0 : -1.0;
			filtered_input[i] = influence * input[i] + (1 - influence) * filtered_input[i - 1];
		}
		else 
		{
			signal[i] = 0.0;
			filtered_input[i] = input[i];
		}
		VectorStats lag_subvector_stats(filtered_input.begin() + (i - lag), filtered_input.begin() + i);
		filtered_mean[i] = lag_subvector_stats.mean();
		filtered_stddev[i] = lag_subvector_stats.standard_deviation();
	}
	
	//identify target
	vector<long double>::iterator it;
	it = find(signal.begin(), signal.end(), 1.0);
	if (it != signal.end())
	{
		//found a target
		// TODO: Fine tune code to build multiple targets here. Each gets own instance.
		// TODO: Monitor exisiting targets. Remove as disappear.
		if (v_targets.size() == 0)
		{
			// Make one
			Target tmpTarget;
			tmpTarget.setID(v_targets.size());
			tmpTarget.setPosition( it - signal.begin());
			v_targets.push_back(tmpTarget);
		}
		else
		{	//Looking at one TODO: recognize which one and update
			v_targets.at(0).setPosition( it - signal.begin());
		}
	}
	else
	{	//No target found; clear
		v_targets.clear();
	}


	output["signals"] = signal;
	//output["filtered_mean"] = filtered_mean;
	//output["filtered_stddev"] = filtered_stddev;
	
	return output;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Not currently used: Creates an average of values across a woindow of data.This filters noise from
/// the signal.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
QList<long double> SigAnalysis::rollingAverage(QList<long double>& rawData)
{
	QList<long double>averagedData;
	long double value=0;

	for (int i = 0; i < rawData.size() - 5; i++)
	{
		value = (rawData[i] + rawData[i + 1] + rawData[i + 2] + rawData[i + 3] + rawData[i + 4]) / 5.0;
		averagedData.push_back(value);
	}
	
	return averagedData;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// A static function to determine frequency of signal using. The function is utilizing a one dimensional DFT. 
/// This incorporates a Hanning window to help contain the cycle information. Distortions caused by the 
/// Hanning window are removed as part of the FFT calculation. The flap rate is then found by analyzing the 
/// output of the FFT and finding the max value in that set.
///
/// @param N - size of the sample packet. Determined in the buffering functions and found by sizing the buffer here.
/// @param Fs - Sampling frequency.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
double SigAnalysis::computeFFT(QList<QPointF>& dataPoints)
{
	// TO DO: A ability to call individual target's amplitude, rather than general location. 
	int i;
	double y;
	int N = dataPoints.size();			//Number of points acquired inside the window
	double Fs=200;							//sampling frequency                                           
	fftw_plan plan_forward;

	// Setup input
	double *in;
	in = (double*) fftw_malloc(sizeof(double) * N);
	for (int i=0; i< N; i++)
	{
		in[i] = dataPoints.at(i).y();	
		double multiplier = 0.5 * (1 - cos(2*M_PI*i/(N-1)));  //Hanning Window
		in[i] = multiplier * in[i];
	}
	
	//Setup sampling
	std::vector<double>ff(N);			// sampling array
	for (int i=0; i<= ((N/2)-1); i++)
	{	
		ff[i]=Fs*i/N; 	
	}
	
	//Set up and perform FFT
	fftw_complex *out;
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	plan_forward = fftw_plan_dft_r2c_1d ( N, in, out, FFTW_ESTIMATE );
	fftw_execute ( plan_forward );

	//Calculate the y axis of the power spectrum in dB
	std::vector<double>v(N);
	for (int i = 0; i<= ((N/2)-1); i++)
	{
		v[i]=(20*log(sqrt(out[i][0]*out[i][0]+ out[i][1]*out[i][1])))/N;  
    }

	// find max
	double max = *std::max_element(v.begin(), v.end());
	int element = 0;
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == max)
		{
			element = i;
			break;
		}
	}
	double flapRate = ff[element];
	g_status.setFoundFlapRate(flapRate);

	//Clean up
	fftw_destroy_plan ( plan_forward );
	fftw_free ( in );
	fftw_free ( out );
	 
	// Pass to frequency finder.
	return 0.0;
}