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


#define BEGIN_DECLARE_LISTENER_HOLDER \
	public: \
		class IListener \
		{ \
		public:


#define END_DECLARE_LISTENER_HOLDER \
		}; \
	public: \
		void AddListener(IListener *Listener) \
		{ \
			m_Listeners.Add(Listener); \
		} \
		\
		void RemoveListener(IListener *Listener) \
		{ \
			Vector<IListener*>::Iterator it = FIND_ITERATOR(Listener, m_Listeners); \
			if (it != m_Listeners.GetEnd()) \
				m_Listeners.Remove(it); \
		} \
		\
	protected: \
		Vector<IListener*> m_Listeners;
	//	}; \
	//private: \
	//	typedef Vector<IListener*> IListenersList; \
	//	\
	//public: \
	//	void AddListener(IListener *Listener) \
	//	{ \
	//		m_Listeners.Add(Listener); \
	//	} \
	//	\
	//	void RemoveListener(IListener *Listener) \
	//	{ \
	//		IListenersList::Iterator it = FIND_ITERATOR(Listener, m_Listeners); \
	//		if (it != m_Listeners.GetEnd()) \
	//			m_Listeners.Remove(it); \
	//	} \
	//	\
	//protected: \
	//	IListenersList m_Listeners;


#define BEGIN_DECLARE_STATIC_LISTENER_HOLDER \
	public: \
		class IListener \
		{ \
		public:


#define END_DECLARE_STATIC_LISTENER_HOLDER \
		}; \
	public: \
		static void AddListener(IListener *Listener); \
		static void RemoveListener(IListener *Listener); \
		\
	protected: \
		typedef Vector<IListener*> IListenersList; \
		static IListenersList m_Listeners; \


#define DEFINE_STATIC_LISTENER_HOLDER(Class) \
	Class::IListenersList Class::m_Listeners; \
	\
	void Class::AddListener(IListener *Listener) \
	{ \
		m_Listeners.Add(Listener); \
	} \
	\
	void Class::RemoveListener(IListener *Listener) \
	{ \
		IListenersList::Iterator it = FIND_ITERATOR(Listener, m_Listeners); \
		if (it != m_Listeners.GetEnd()) \
			m_Listeners.Remove(it); \
	}