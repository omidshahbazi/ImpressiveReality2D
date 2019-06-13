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

#include "IGUIObject.h"

BEGIN_NAMESPACE

class IProgressBar : public IGUIObject
{
public:
	virtual ~IProgressBar(void) {}
	
	virtual void SetPrecent(const float &Precent) = 0;

	virtual void SetBarColour(const Colour &Colour) = 0;
	virtual const Colour &GetBarColour(void) = 0;

	virtual void SetBarImage(IImage *Image) = 0;
	virtual IImage *GetBarImage(void) = 0;

	virtual void SetDisplayText(const bool &DisplayText) = 0;
	virtual const bool &GetDisplayText(void) = 0;
};

END_NAMESPACE