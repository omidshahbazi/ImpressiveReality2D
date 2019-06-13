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
class IFixture;
class IContact;
class IContactImpulse;
class IGameObject;

class IBody : public ISerializable
{
public:
	enum BodyType
	{
		BT_STATIC = 0,
		BT_KINEMATIC,
		BT_DYNAMIC
	};

public:
	typedef Vector<IFixture*> FixturesList;

public:
	virtual ~IBody(void) {}

	virtual IPhysicsScene *GetSceneHolder(void) const = 0;
	virtual IGameObject *GetHolder(void) const = 0;


	virtual void SetTransform(const Vector2D &Position, const float &Rotation) = 0;
	virtual void SetPosition(const Vector2D &Position) = 0;
	virtual void SetRotation(const float &Rotation) = 0;

	virtual const Vector2D &GetPosition(void) const = 0;

	virtual const float GetAngle(void) const = 0;

	virtual void SetLinearDamping(const float &Value) = 0;
	virtual const float &GetLinearDamping(void) const = 0;

	virtual void SetAngularDamping(const float &Value) = 0;
	virtual const float &GetAngularDamping(void) const = 0;

	virtual void SetLinearVelocity(const Vector2D &Velocity) = 0;
	virtual const Vector2D &GetLinearVelocity(void) const = 0;

	virtual void SetAngularVelocity(const float &Value) = 0;
	virtual const float &GetAngularVelocity(void) const = 0;

	virtual void SetGravityScale(const float &Value) = 0;
	virtual const float &GetGravityScale(void) const = 0;

	virtual void SetSleepingAllowed(const bool &Flag) = 0;
	virtual const bool IsSleepingAllowed(void) const = 0;

	virtual void SetAwake(const bool &Flag) = 0;
	virtual const bool IsAwake(void) const = 0;

	virtual void SetUseCCD(const bool &Flag) = 0;
	virtual const bool &IsUsingCCD(void) const = 0;

	virtual void SetActive(const bool &Flag) = 0;
	virtual const bool &IsActive(void) const = 0;

	virtual void SetType(const BodyType &Type) = 0;
	virtual const BodyType &GetType(void) const = 0;

	virtual const Vector2D GetWorldCenterOfMass(void) const = 0;
	virtual const Vector2D GetLocalCenterOfMass(void) const = 0;

	virtual void ApplyForce(const Vector2D &Force, const Vector2D &Point) = 0;
	virtual void ApplyLinearImpulse(const Vector2D &Force, const Vector2D &Point) = 0;
	virtual void ApplyTorque(const float &Value) = 0;
	virtual void ApplyAngularImpulse(const float &Value) = 0;

#ifndef LAUNCE_MODE
	virtual void AddFixture(IFixture *Fixture) = 0;
	virtual void RemoveFixture(IFixture *Fixture) = 0;
#endif
	virtual IFixture *CreateFixture(const String &Name) = 0;
	virtual void DestroyFixture(const String &Name) = 0;
	virtual void DestroyFixture(IFixture *Fixture) = 0;

	virtual IFixture *GetFixture(const int &Index) = 0;
	virtual IFixture *GetFixture(const String &Name) = 0;
	virtual FixturesList &GetFixtures(void)= 0;
	virtual const unsigned int GetFixturesCount(void) const = 0;

	virtual void SetContact(IContact *Contact) = 0;
	virtual IContact *GetContact(void) const = 0;
	
	virtual const bool IsShapeConvex(const List<Vector2D> &Vertices) = 0;
};

END_NAMESPACE