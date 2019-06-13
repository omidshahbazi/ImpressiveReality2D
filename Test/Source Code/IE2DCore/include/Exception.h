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
#ifdef _DEBUG

#pragma once

#include "Common.h"
#include "Singleton.h"

BEGIN_NAMESPACE

class DLL_DECLARATION Exception
{
	DECLARE_SINGLETON(Exception)

public:
	enum ExceptionReason
	{
		ER_UNKNOWN = 0,
		ER_FILE_NOT_FOUND,
		ER_INVALID_PARAMETER,
		ER_PERFORMANCE_HIT,
		ER_INVALID_OPERATION
	};

public:
	void Throw(const ExceptionReason &Reason, const String &Text, const String &Detail, const bool &StopProcess = false);
};

END_NAMESPACE

#endif