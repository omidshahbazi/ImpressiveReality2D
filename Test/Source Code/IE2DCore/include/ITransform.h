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

#include "Component.h"
#include "ISerializable.h"

BEGIN_NAMESPACE

class IGameObject;
class AABB;
class OBB;
class Vector3D;

class ITransform : public Component, public ISerializable
{
public:
	typedef Vector<ITransform*> TransformsList;

public:
	virtual IGameObject *GetHolder(void) = 0;
	
	virtual void SetParent(ITransform *Parent) = 0;
	virtual ITransform *GetParent(void) const = 0;
	
	virtual const AABB &GetWorldAABB(void) const = 0;
	virtual const OBB &GetWorldOBB(void) const = 0;

	virtual void SetPosition(const float &X, const float &Y, const float &Z) = 0;
	virtual void SetPosition(const Vector3D &Position) = 0;
	virtual const Vector3D &GetPosition(void) const = 0;
	
	virtual const Vector3D &GetWorldPosition(void) const = 0;

	virtual void SetRotation(const float &Value) = 0;
	virtual const float &GetRotation(void) const = 0;
		
	virtual void NeedManualUpdate(void) = 0;

	virtual TransformsList &GetChildren(void) = 0;
	virtual TransformsList GetChildrenCopy(void) = 0;
};

END_NAMESPACE