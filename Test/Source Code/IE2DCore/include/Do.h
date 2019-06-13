/*/////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "Common.h"
#include "IBaseObject.h"

USING_NAMESPACE

struct DLL_DECLARATION Do
{
public:
	enum ActionType
	{
		DT_NEW,
		DT_CHANGE,
		DT_REMOVE
	};

	Do(IBaseObject* Object, const ActionType &Type);


	bool operator == (const Do &Action) const;
	bool operator != (const Do &Action) const;

public:
	IBaseObject *m_Object;
	IBaseObject *m_CopyObject;
	IBaseObject *m_ParentObject;

	ActionType m_Type;
};