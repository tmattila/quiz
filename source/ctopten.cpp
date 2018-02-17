/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#include "ctopten.h"

/**
 * Constructor
 */
cTopTen::cTopTen()
{
}


/**
 * Destructor
 */
cTopTen::~cTopTen()
{
}

/**
 * Method reads TopTen listing from disk.
 * First we need to figure out where the file
 * might be. Search order is:
 * <ul>
 *  <li>application directory</li>
 *  <li>current user home directory</li>
 * </ul>
 */
void cTopTen::readTopTen( )
{
}

/**
 * Method writes TopTen listing to disk.
 * First we need to figure out where the file
 * should (or could) be written. Write order is:
 * <ul>
 *  <li>application directory</li>
 *  <li>current user home directory</li>
 * </ul>
 * If these can't be used then return false, otherwise true.
 */
bool cTopTen::writeTopTen( )
{
    return 1;
}

/**
 *
 */
string cTopTen::topTen( )
{
    return "";
}


