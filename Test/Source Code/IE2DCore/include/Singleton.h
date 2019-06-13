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
//#include "Common.h"

//BEGIN_NAMESPACE
//
//template<typename T> class Singleton 
//{
//public:
//	//Singleton(void)
//	//{
//	//	if (!m_Instance)
//	//		m_Instance = static_cast<T*>(this);
//	//}
//
//	static T &GetReference() 
//	{
//		if (!m_Instance)
//			m_Instance = new T();
//
//		return *m_Instance;
//	}
//	
//	static T *GetPointer() 
//	{
//		if (!m_Instance)
//			m_Instance = new T();
//
//		return m_Instance;
//	}
//
//protected:
//	static T *m_Instance;
//};
//
//#define DEFINE_SINGLETON_FOR(Class) template<> Class *Singleton<Class>::m_Instance = NULL;
//
//END_NAMESPACE

#define DECLARE_SINGLETON(Class) \
	public: \
		static Class &GetReference(); \
		static Class *GetPointer(); \
		\
	protected: \
		static Class *m_Instance;

#define DEFINE_SINGLETON(Class) \
	Class &Class::GetReference() \
	{ \
		if (!m_Instance) \
			m_Instance = new Class(); \
		\
		return *m_Instance; \
	} \
	\
	Class *Class::GetPointer() \
	{ \
		if (!m_Instance) \
			m_Instance = new Class(); \
		\
		return m_Instance; \
	} \
	\
	Class *Class::m_Instance = NULL;


#define DECLARE_OBJECTIVE_SINGLETON(Class) \
	public: \
		static Class &GetReference(); \
		static Class *GetPointer(); \
		\
	protected: \
		static Class m_Instance;

#define DEFINE_OBJECTIVE_SINGLETON(Class) \
	Class &Class::GetReference() \
	{ \
		return m_Instance; \
	} \
	\
	Class *Class::GetPointer() \
	{ \
		return &m_Instance; \
	} \
	\
	Class Class::m_Instance;