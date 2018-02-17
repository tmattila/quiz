/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#ifndef CTOPTEN_H
#define CTOPTEN_H

#include <string>
using namespace std;

/**
 * Class maintains TopTen file. It offers methods to save TopTen to
 * disk and also to read it from disk. A method for getting current
 * TopTen listing as a string is also offered.
 *
 *	@author Tapio Mattila <tapsa.mattila@gmail.com>
 */
class cTopTen {
public:
    cTopTen();
    ~cTopTen();
	void readTopTen();
    bool writeTopTen();
    string topTen();
};

#endif
