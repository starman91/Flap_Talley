/////////////////////////////////////////////////////////////////////////////
//		    		status.cpp
//
//              -------------------
// author		: Robert R. White
// date         : Mon Aug 5 2019
// copyright : (C) 2019 by  NREL
// email        : robert.white@nrel.gov
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "status.h"
///Constructor
Status::Status(void)
{
	Init();
	return;
}
///DeConstructor
Status::~Status()
{
}



/////////////////////////////////////////////////////////////////////////////
/// Support contructor, variable intialization
///////////////////////////////////////////////////////////////////////////
void Status::Init(void)
{
	m_ok = false;
	m_simMode = false;
	m_simFlap = false;
	m_simMove = false;
	m_simTarget = false;

	m_targetSize = 0.0;
	m_calcLag = 0;
	m_threshold = 0.0;
	m_influence = 0.0;
	m_flapAmp = 0.0;
	m_flapRate = 0.0;
	m_calcRate = 0.0;
	m_calcFlap = false;
	m_calculating = false;
	m_plotFlap = false;
	return;
}

