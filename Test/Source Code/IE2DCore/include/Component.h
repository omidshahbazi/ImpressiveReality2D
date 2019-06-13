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

#include "IObject.h"

BEGIN_NAMESPACE

class IGameObject;

class Component : public IObject
{
	friend class ScriptSystem;

public:
	Component(void) :
		m_Holder(NULL)
	{
	}
	virtual ~Component(void) {}

	virtual void Awake(void) {}
	virtual void Start(void) {}
	virtual void Update(void) {}
	virtual void Render(void) {}

	IGameObject *GetHolder(void)
	{
		return m_Holder;
	}

private:
	void SetHolder(IGameObject *Holder)
	{
		m_Holder = Holder;
	}

private:
	IGameObject *m_Holder;
};

#define DEFINE_COMPONENT(ComponentType) \
	extern "C" __declspec(dllexport) ComponentType *Instantiate##ComponentType(void) \
	{ \
		return new ComponentType; \
	}

#define DEFINE_ENTRY_POINT(ComponentType) \
	extern "C" __declspec(dllexport) void EntryPoint(void)

END_NAMESPACE