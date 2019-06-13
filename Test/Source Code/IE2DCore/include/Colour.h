///////////////////////////////////////////////////////////////////////////////////
///
///  Impressive Engine 2D
///
/// Copyright (c) 2012-2013 Impressive Reality team
///
/// The license is
///
/// Permission is denied, to any person or company
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// Project leader : O.Shahbazi <sh_omid_m@yahoo.com>
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
struct DLL_DECLARATION Colour
{
public:
	Colour(void);
	Colour(const unsigned int &Scalar);
	Colour(const unsigned int &R, const unsigned int &G, const unsigned int &B, const unsigned int &A = 255);
	Colour(const Colour &Colour);
	//~Colour(void);

	bool operator == (const Colour &Colour) const;
	bool operator != (const Colour &Colour) const
	{
		return !this->operator==(Colour);
	}

	Colour &operator = (const Colour &Colour);

public:
	unsigned int R, G, B, A;
};

const Colour Colour_BLACK(0, 0, 0, 255);
const Colour Colour_WHITE(255);
const Colour Colour_RED(255, 0, 0);
const Colour Colour_GREEN(0, 255, 0);
const Colour Colour_BLUE(0, 0, 255);

END_NAMESPACE