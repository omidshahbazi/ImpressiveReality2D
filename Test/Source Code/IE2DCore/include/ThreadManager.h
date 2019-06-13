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
#include "Singleton.h"
#include "IThread.h"
#include "IThreadWorker.h"

BEGIN_NAMESPACE

class DLL_DECLARATION ThreadManager
{
	DECLARE_SINGLETON(ThreadManager)

public:
	ThreadManager(void);
	~ThreadManager(void);

	IThread *CreateThread(IThreadWorker *Worker);

	void DestroyThread(IThread *Thread);

	static void SleepCurrentThread(const unsigned int &Seconds);

private:
	typedef Vector<IThread*> ThreadList;

	ThreadList m_Threads;
};

END_NAMESPACE