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
#include "ISerializable.h"
#include "IBaseObject.h"

BEGIN_NAMESPACE

class IBody;
class IJoint;
class IPrismaticJoint;
class IRevoluteJoint;
class IGameObject;
struct Vector2D;

class IPhysicsScene : public ISerializable
{
public:
	typedef Vector<IJoint*> JointList;

public:
	virtual ~IPhysicsScene(void) {}

	virtual void SetGravity(const Vector2D &Gravity) = 0;
	virtual const Vector2D &GetGravity(void) const = 0;

	virtual void Update(void) = 0;

	virtual IBody *CreateBody(IGameObject *GameObject) = 0;
	virtual void DestroyBody(IBody *Body) = 0;

	virtual void SetAllowSleeping(const bool &Flag) = 0;
	virtual const bool &GetAllowSleeping(void) const = 0;

	virtual IJoint *CreateRevoluteJoint(const String &Name,
										IBody *BodyA,
										IBody *BodyB,
										const bool &CollideConnected,
										const Vector2D &LocalAnchorA,
										const Vector2D &LocalAnchorB,
										const float &ReferenceAngle) = 0;

	virtual IJoint *CreatePrismaticJoint(const String &Name,
										 IBody *BodyA,
										 IBody *BodyB,
										 const bool &CollideConnected,
										 const Vector2D &LocalAxisA,
										 const Vector2D &LocalAnchorA,
										 const Vector2D &LocalAnchorB,
										 const float &ReferenceAngle) = 0;

	virtual IJoint *CreateDistanceJoint(const String &Name,
										IBody *BodyA,
										IBody *BodyB,
										const bool &CollideConnected,
										const Vector2D &LocalAnchorA,
										const Vector2D &LocalAnchorB) = 0;

	virtual IJoint *CreateRopeJoint(const String &Name,
									IBody *BodyA,
									IBody *BodyB,
									const bool &CollideConnected,
									const Vector2D &LocalAnchorA,
									const Vector2D &LocalAnchorB) = 0;

	virtual IJoint *CreatePulleyJoint(const String &Name,
									  IBody *BodyA,
									  IBody *BodyB,
									  const bool &CollideConnected,
									  const Vector2D &GroundAnchorA,
									  const Vector2D &LocalAnchorA,
									  const Vector2D &GroundAnchorB,
									  const Vector2D &LocalAnchorB,
									  const float LengthA,
									  const float LengthB,
									  const float Ratio) = 0;

	virtual IJoint *CreateGearJoint(const String &Name,
									IBody *BodyA,
									IBody *BodyB,
									const bool &CollideConnected,
									IRevoluteJoint *RevoluteJoint,
									IPrismaticJoint *PrismaticJoint) = 0;

	virtual IJoint *CreateWheelJoint(const String &Name,
									 IBody *BodyA,
									 IBody *BodyB,
									 const bool &CollideConnected,
								 	 const Vector2D &LocalAxisA,
									 const Vector2D &LocalAnchorA,
									 const Vector2D &LocalAnchorB) = 0;

	virtual IJoint *CreateWeldJoint(const String &Name,
									IBody *BodyA,
									IBody *BodyB,
									const bool &CollideConnected,
									const Vector2D &LocalAnchorA,
									const Vector2D &LocalAnchorB,
									const float &ReferenceAngle) = 0;

#ifndef LAUNCE_MODE
	virtual void AddJoint(IJoint *Joint) = 0;
	virtual void RemoveJoint(IJoint *Joint) = 0;
#endif
	virtual void DestroyJoint(IJoint *Joint) = 0;
	virtual IJoint *GetJoint(const String &Name) = 0;
	virtual JointList &GetJoints(void) = 0;
};

END_NAMESPACE