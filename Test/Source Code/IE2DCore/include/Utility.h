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
#include <ctime>

BEGIN_NAMESPACE

class DLL_DECLARATION Utility
{
public:
	static bool FileExists(const String &FilePath);
	static bool DirectoryExists(const String &DirectoryPath);

	static void MakeDirectory(const String &DirectoryPath);

	static StringsList GetFiles(const String &Directory);

	static StringsList GetDirectories(const String &Directory);

	static unsigned int Random(const unsigned int &Minimum, const unsigned int &Maximum);
};

END_NAMESPACE