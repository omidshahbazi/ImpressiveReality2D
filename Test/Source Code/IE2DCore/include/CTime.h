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

//
// CTime.h because we have a conflict with time.h of std 
//


#pragma once

#include "Common.h"
#include "Singleton.h"
#include <Windows.h>


BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class DLL_DECLARATION CTime
{
	DECLARE_SINGLETON(CTime)

public:
	CTime(void);
	//~CTime(void);

	void Reset(void);
	void Tick(void);

	const unsigned int GetRealTime(void);
	
	const float GetTime(void);
	
	const float GetTimeInMilliseconds(void);
	
	const unsigned int &GetDeltaTime(void);
	
	const unsigned int &GetStartTime(void)
	{
		return m_StartTime;
	}
	
	const float &GetTimeScale(void)
	{
		return m_TimeScale;
	}

	void SetTimeScale(const float &Value)
	{
		m_TimeScale = Value;
	}

	const unsigned int GetTimeSinceLastUpdate(void)
	{
		if (m_LastTime > m_CurrentTime)
			return 0;

		return m_CurrentTime - m_LastTime;
	}

private:
	unsigned int m_StartTime;
	unsigned int m_CurrentTime;
	float m_TimeScale;
	unsigned int m_LastTime;
	unsigned int m_DeltaTime;
};

typedef CTime Time;

END_NAMESPACE