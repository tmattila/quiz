/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#include "cnumbergenerator.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sstream>

/**
 * Default constructor
 */
cNumberGenerator::cNumberGenerator()
{
	initialize();
}

/**
 * Destructor
 */
cNumberGenerator::~cNumberGenerator()
{
}


/**
 *	Initialize class members and random number generator.
 */
void cNumberGenerator::initialize()
{
	m_x = 0;
	m_y = 0;
	m_maxX = 10;
	m_maxY = 10;
	
	time_t systemClock;
	tm *localClock;
	time( &systemClock );
	localClock = localtime( &systemClock );
	srand( (localClock->tm_sec * localClock->tm_min) );
}



/**
 *	Generates X and Y values based on given maximum values.
 */
void cNumberGenerator::generateFrom(cSettings* settings)
{
	while ( !(m_x = generate(settings->xPart())) );
	while ( !(m_y = generate(settings->yPart())) );
	m_maxX = settings->xPart();
	m_maxY = settings->yPart();
}




/**
 *	Generates a random value based on given maximum value and
 * returns it.
 */
int cNumberGenerator::generate(int max)
{
	int temp = rand() % max;
	return temp;
}


/**
 *	Method calculates the multiplication of generated values and
 * returns the result of multiplication.
 * 
 * Numbers must be greater than 0 or otherwise we return just an empty
 * string.
 */
int cNumberGenerator::intValue(cSettings* settings)
{
	if ( m_x<=0 || m_y<=0 )
		return 0;
	switch (settings->gameType())
	{
		case MULTIPLY:
			return m_x * m_y;
		case DIVIDE:
			return m_x / m_y;
		case ADD:
			return m_x + m_y;
		case SUBTRACT:
			return m_x - m_y;
	}
	return 0;
}

/**
 *	Method calculates the multiplication of generated values and
 * returns a string where the whole calculation is. An example would
 * be like: "5 * 3 = 15".
 * 
 * Numbers must be greater than 0 or otherwise we return just an empty
 * string.
 */
string cNumberGenerator::strValue(cSettings* settings)
{
	if ( m_x<=0 || m_y<=0 )
		return "";
	ostringstream os;
	switch (settings->gameType())
	{
		case MULTIPLY:
            os << m_x << " x " << m_y << " = ";
			break;
		case DIVIDE:
			os << m_x << " / " << m_y << " = ";
			break;
		case ADD:
			os << m_x << " + " << m_y << " = ";
			break;
		case SUBTRACT:
			os << m_x << " - " << m_y << " = ";
			break;
	}
	return os.str();
}

/**
 *	Method calculates the division of generated values and
 * returns a string where the whole calculation is. An example would
 * be like: "15 / 5 = 3".
 * 
 * Numbers must be greater than 0 or otherwise we return just an empty
 * string.
 * 
 * \Note At the moment we calculate 'proper' values here, so that we can
 * organize integer result of division. So we do not use values already
 * calculated!
 */
string cNumberGenerator::strDivide()
{
	if ( m_x<=0 || m_y<=0 )
		return "";
	int result = m_x * m_y;
	ostringstream os;
	os << result << " / " << m_y << " = " << m_x;
	return os.str();
}

/**
 *	Method calculates the addition of generated values and
 * returns a string where the whole calculation is. An example would
 * be like: "5 + 3 = 8".
 * 
 * Numbers must be greater than 0 or otherwise we return just an empty
 * string.
 */
string cNumberGenerator::strAdd()
{
	if ( m_x<=0 || m_y<=0 )
		return "";
	ostringstream os;
	os << m_x << " + " << m_y << " = " << m_x + m_y;
	return os.str();
}
	

/**
 *	Method calculates the subtraction of generated values and
 * returns a string where the whole calculation is. An example would
 * be like: "5 - 3 = 2". We take care that result is always positive.
 * 
 * Numbers must be greater than 0 or otherwise we return just an empty
 * string.
 */
string cNumberGenerator::strSubtract()
{
	if ( m_x<=0 || m_y<=0 )
		return "";
	ostringstream os;
	if ( m_x > m_y )
		os << m_x << " - " << m_y << " = " << m_x - m_y;
	else
		os << m_y << " - " << m_x << " = " << m_y - m_x;
	return os.str();
}

