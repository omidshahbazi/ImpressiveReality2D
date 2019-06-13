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

#include "Vector3D.h"
#include "ISerializable.h"
#include "IBaseObject.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class ICamera : public ISerializable
{
public:
	virtual ~ICamera(void) {}

	virtual void SetPosition(const Vector3D &Position) = 0;
	virtual const Vector3D &GetPosition(void) = 0;

	virtual void SetRotation(const float &Value) = 0;
	virtual const float &GetRotation(void) = 0;

	virtual void SetAspectRatio(const float &Value) = 0;
	virtual const float GetAspectRatio(void) = 0;
};

END_NAMESPACE