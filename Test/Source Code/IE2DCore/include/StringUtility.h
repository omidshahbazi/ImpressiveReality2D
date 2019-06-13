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

class DLL_DECLARATION StringUtility
{
public:
	static String StandardizePath(const String &Value);

	static void SplitFilename(const String &QualifiedName, String *OutBasename, String *OutPath);

	static void SplitFullFilename(const String &QualifiedName, String *OutBasename, String *OutExtention, String *OutPath);

	static void SplitBaseFilename(const String &FullName, String *OutBasename, String *OutExtention);

	static const bool IsLowerCase(const char &Character);
	static const bool IsUpperCase(const char &Character);

	//static String ReplaceAll(const String &Source, const String &ReplaceWhat, const String &ReplaceWithWhat);
};


END_NAMESPACE