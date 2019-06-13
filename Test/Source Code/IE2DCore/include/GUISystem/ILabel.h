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

class ILabel : public IGUIObject
{
public:
	virtual ~ILabel(void) {}
	
	virtual void SetOffset(const Vector2D &Offset) = 0;
	virtual const Vector2D &GetOffset(void) = 0;

	virtual void SetText(const String &Text) = 0;
	virtual const String &GetText(void) = 0;
};

END_NAMESPACE