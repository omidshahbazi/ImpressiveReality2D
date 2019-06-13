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

class IButton : public IGUIObject
{
public:
	virtual ~IButton(void) {}
	
	virtual void SetCaption(const String &Text) = 0;

	virtual void SetMouseOverColour(const Colour &Colour) = 0;
	virtual const Colour &GetMouseOverColour(void) = 0;

	virtual void SetMouseClickColour(const Colour &Colour) = 0;
	virtual const Colour &GetMouseClickColour(void) = 0;

	virtual void SetFocus(const bool &Focus) = 0;
	virtual const bool &GetFocus(void) = 0;

	virtual const bool &IsClicked(void) = 0;
};

END_NAMESPACE