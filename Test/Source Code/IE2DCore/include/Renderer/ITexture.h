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
class ITexture
{
public:
	virtual ~ITexture(void) {}

	virtual const String &GetName(void) = 0;
	
	virtual void Destroy(void) = 0;
	virtual void Reload(void) = 0;

	virtual const bool &IsLoaded(void) const = 0;
};

END_NAMESPACE