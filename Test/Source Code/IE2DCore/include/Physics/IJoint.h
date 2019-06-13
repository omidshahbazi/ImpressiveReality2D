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
#include "IPhysicsScene.h"

BEGIN_NAMESPACE

class IBody;

class IJoint : public ISerializable
{
public:
	enum JointType
	{
		JT_REVOLUTE = 0,
		JT_PRISMATIC,
		JT_DISTANCE,
		JT_ROPE,
		JT_PULLEY,
		JT_GEAR,
		JT_WHEEL,
		JT_WELD
	};

	virtual ~IJoint(void) {};

	void SetHolder(IPhysicsScene *Holder)
	{
		m_Holder = Holder;
	}

	void SetName(const String &Name)
	{
		if (m_Holder->GetJoint(Name))
		{
			LOG_ERROR("Joint with the name [" + Name + "] already exists");
			return;
		}

		m_Name = Name;
	}

	const String &GetName(void) const
	{
		return m_Name;
	}

	IBody *GetBodyA(void)
	{
		return m_BodyA;
	}

	IBody *GetBodyB(void)
	{

		return m_BodyB;
	}


	const bool &GetCollideConnected(void) const
	{
		return m_CollideConnected;
	}


	const JointType &GetType(void) const
	{
		return m_Type;
	}

	virtual void *GetData(void) = 0;

	virtual void Prepare(void) = 0;

protected:
	IPhysicsScene *m_Holder;
	IBody *m_BodyA, *m_BodyB;

	String m_Name;

	bool m_CollideConnected;

	JointType m_Type;

};

END_NAMESPACE