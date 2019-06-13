////////////////////////////////////////////////////////////////////////////////////
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
#include "TreeElement.h"

BEGIN_NAMESPACE

class ITreeParser
{
public:
	virtual ~ITreeParser(void) {}

	virtual const String ToString(void) = 0;

public:
	virtual TreeElement *GetRoot(void) = 0;
};

END_NAMESPACE