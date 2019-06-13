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
#include "Singleton.h"
#include "Do.h"

USING_NAMESPACE

class DLL_DECLARATION DoManager
{
	DECLARE_SINGLETON(DoManager)

public:
	typedef std::vector<Do> DoVector;

public:
	DoManager(void);
	~DoManager(void);

	void SetMaxActionCount(const unsigned int &Value)
	{
		m_MaxActionCount = Value;
	}

	void AddAction(const Do &Action);

	bool IsUndoEmpty(void) const;
	bool IsRedoEmpty(void) const;

	void Undo(void);
	void Redo(void);

private:
	void ClearRedoVector(void);
	void AddUndoAction(const Do &Action);
	void AddRedoAction(const Do &Action);

private:
	DoVector m_Undo;
	DoVector m_Redo;

	unsigned int m_MaxActionCount;
};
