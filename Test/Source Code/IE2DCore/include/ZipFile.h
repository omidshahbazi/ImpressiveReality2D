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

class DLL_DECLARATION ZipFile
{
public:
	enum Mode
	{
		M_ZIP = 0,
		M_UNZIP
	};

public:
	ZipFile(const String &Path, const Mode &Mode, const String &Password = "");
	~ZipFile(void);

	void AddFile(const String &Path);
	void AddFile(const String &Path, const String &FileName);

private:
	int m_Handle;
};

END_NAMESPACE