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
#include "Vector2D.h"
#include "Vector3D.h"
#include "Colour.h"


BEGIN_NAMESPACE

class DLL_DECLARATION StringConverter
{
public:
	static String ToString(const float &Value, const unsigned short &Precision = 6, const unsigned short &Width = 0, const char &Fill = ' ', std::ios::fmtflags Flags = std::ios::fmtflags(0));

	static String ToString(const int &Value, const unsigned short &Width = 0, const char &Fill = ' ', std::ios::fmtflags Flags = std::ios::fmtflags(0));

	static String ToString(const unsigned int Value, const unsigned short &Width = 0, const char &Fill = ' ', std::ios::fmtflags Flags = std::ios::fmtflags(0));

	static String ToString(const unsigned long &Value, const unsigned short &Width = 0, const char &Fill = ' ', std::ios::fmtflags Flags = std::ios::fmtflags(0));

	static String ToString(const long &Value, const unsigned short &Width = 0, const char &Fill = ' ', std::ios::fmtflags Flags = std::ios::fmtflags(0));

	static String ToString(const bool &Value, const bool &YesNo = false);

	static String ToString(const Colour &Value);

	static String ToString(const Vector2D &Value);

	static String ToString(const Vector3D &Value);

	//static String ToString(const StringVector& Value);

	static float ParseFloat(const String &Value, const float &DefaultValue = 0.f);

	static int ParseInteger(const String &Value, const int &DefaultValue = 0);

	static unsigned int ParseUnsignedInt(const String &Value, const unsigned int &DefaultValue = 0);

	static long ParseLong(const String &Value, const long &DefaultValue = 0);

	static unsigned long ParseUnsignedLong(const String &Value, const unsigned long &DefaultValue = 0);

	static bool ParseBool(const String &Value, const bool &DefaultValue = false);

	static Vector2D ParseVector2D(const String &Value, const Vector2D &DefaultValue = Vector2D_ZERO);

	static Vector3D ParseVector3D(const String &Value, const Vector3D &DefaultValue = Vector3D_ZERO);

	static Colour ParseColour(const String &Value, const Colour &DefaultValue = Colour_WHITE);

	//static StringList ParseStringVector(const String &Value);

	static bool IsNumber(const String &Value);
};


END_NAMESPACE