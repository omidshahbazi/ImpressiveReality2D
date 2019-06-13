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

class IFont
{
public:
	virtual ~IFont(void) {}

	virtual const String &GetName(void) const = 0;

	virtual void SetSize(const unsigned int &Size) = 0;
	virtual const unsigned int &GetSize(void) const = 0;
	
	virtual void SetNewLineRatio(const float &Ratio) = 0;
	virtual const float &GetNewLineRatio(void) const = 0;
};

END_NAMESPACE