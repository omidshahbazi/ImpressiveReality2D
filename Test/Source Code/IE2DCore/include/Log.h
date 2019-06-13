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

#ifdef USE_LOG

#include "Singleton.h"
#include "ListenerHolder.h"


BEGIN_NAMESPACE

class IFile;

//<Description>
//This is sample description
class DLL_DECLARATION Log
{
	DECLARE_SINGLETON(Log)

	BEGIN_DECLARE_LISTENER_HOLDER
		virtual void OnAddInfo(const String &Text) = 0;
		virtual void OnAddWarning(const String &Text) = 0;
		virtual void OnAddError(const String &Text) = 0;
	END_DECLARE_LISTENER_HOLDER

public:
	Log(void);
	~Log(void);

	void AddText(const String &Text);

	void AddInfo(const String &Text);
	void AddWarning(const String &Text);
	void AddError(const String &Text);

	static const String GetTimePrefix(void);

//#ifndef LAUNCH_MODE
	IFile *m_File;
//#endif
};

END_NAMESPACE

#endif