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

#include "Colour.h"
#include "Vector2D.h"
#include "Vector3D.h"

BEGIN_NAMESPACE
	
struct TreeElement;

class IAttributes
{
public:
	virtual ~IAttributes(void) {}

	virtual void Read(TreeElement *Element) = 0;

	virtual void Write(TreeElement *Element) = 0;

	virtual void AddBoolean(const String &Name, const bool &Value) = 0;
	virtual void AddInteger(const String &Name, const int &Value) = 0;
	virtual void AddFloat(const String &Name, const float &Value) = 0;
	virtual void AddString(const String &Name, const String &Value) = 0;
	virtual void AddColour(const String &Name, const Colour &Value) = 0;
	virtual void AddVector2D(const String &Name, const Vector2D &Value) = 0;
	virtual void AddVector3D(const String &Name, const Vector3D &Value) = 0;
	
	virtual void SetBoolean(const String &Name, const bool &Value) = 0;
	virtual void SetInteger(const String &Name, const int &Value) = 0;
	virtual void SetFloat(const String &Name, const float &Value) = 0;
	virtual void SetString(const String &Name, const String &Value) = 0;
	virtual void SetColour(const String &Name, const Colour &Value) = 0;
	virtual void SetVector2D(const String &Name, const Vector2D &Value) = 0;
	virtual void SetVector3D(const String &Name, const Vector3D &Value) = 0;
	
	virtual const bool GetBoolean(const String &Name) = 0;
	virtual const int &GetInteger(const String &Name) = 0;
	virtual const float &GetFloat(const String &Name) = 0;
	virtual const String GetString(const String &Name) = 0;
	virtual const Colour &GetColour(const String &Name) = 0;
	virtual const Vector2D &GetVector2D(const String &Name) = 0;
	virtual const Vector3D &GetVector3D(const String &Name) = 0;

	virtual void Remove(const String &Name) = 0;

	virtual const bool Has(const String &Name) = 0;
	
	virtual void SetTagName(const String &TagName) = 0;
	virtual const String &GetTagName(void) = 0;

	static const String GetDefaultTagName(void)
	{
		return "ATTRIBUTES";
	}
};

DLL_DECLARATION IAttributes *CreateAttributes(void);
DLL_DECLARATION void DestroyAttributes(IAttributes *Attributes);


END_NAMESPACE