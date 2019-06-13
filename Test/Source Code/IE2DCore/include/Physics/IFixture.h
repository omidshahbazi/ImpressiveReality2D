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

#include "ISerializable.h"
#include "IBaseObject.h"

BEGIN_NAMESPACE

class IPhysicsScene;
class IBody;
class IShape;

struct Vector2D;

class IFixture : public ISerializable
{
public:
	virtual ~IFixture(void) {}

	virtual IBody *GetBody(void) const = 0;

	virtual void SetName(const String &Name) = 0;
	virtual const String &GetName(void) const = 0;

	virtual void SetDensity(const float &Value) = 0;
	virtual const float &GetDensity(void) const = 0;

	virtual void SetFriction(const float &Value) = 0;
	virtual const float &GetFriction(void) const = 0;

	virtual void SetRestitution(const float &Value) = 0;
	virtual const float &GetRestitution(void) const = 0;

	virtual void SetSensor(const bool Flag) = 0;
	virtual const bool &IsSensor(void) const = 0;

	virtual void SetCollisionCategoryBits(const unsigned short &Value) = 0;
	virtual const unsigned short GetCollisionCategoryBits(void) const = 0;

	virtual void SetCollisionMaskBits(const unsigned short &Value) = 0;
	virtual const unsigned short GetCollisionMaskBits(void) const = 0;

	virtual void SetCollisionGroupIndex(const unsigned short &Value) = 0;
	virtual const short &GetCollisionGroupIndex(void) const = 0;

	virtual void Prepare(IShape *Shape) = 0;
	virtual void Prepare(const float &radius, const Vector2D &Center) = 0;
	virtual void Prepare(const float &hx, const float &hy, const Vector2D &Center = Vector2D()) = 0;
	virtual void Prepare(const List<Vector2D> &Vertices) = 0;
	virtual void Prepare(const Vector2D &Vertex1, const Vector2D &Vertex2) = 0;

	virtual IShape *GetShape(void) const = 0;
};

END_NAMESPACE