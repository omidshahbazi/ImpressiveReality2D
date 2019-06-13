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

#include "Renderer\IMovie.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class IMovieManager
{
public:
	virtual ~IMovieManager(void) {}

	virtual void Update(void) = 0;

	virtual IMovie *LoadMovie(const String &RelativeFilePath) = 0;
	virtual void DestroyMovie(IMovie *Movie) = 0;
};

END_NAMESPACE