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

class ITextBox : public IGUIObject
{
public:
	virtual ~ITextBox(void) {}
	
	virtual const String &GetText(void) = 0;

	virtual void SetFocus(const bool &Focus) = 0;
	virtual const bool &GetFocus(void) = 0;
};

END_NAMESPACE