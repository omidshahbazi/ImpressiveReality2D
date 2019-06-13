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

#include "SecondaryCommon.h"

BEGIN_NAMESPACE

template <class T> class Vector
{
public:
	class Iterator
	{
	public:
		Iterator(void) :
			m_Vector(NULL),
			m_Current(-1)
		{
		}

	private:
		explicit Iterator(Vector<T> *Vector, const int &ItemIndex) :
			m_Vector(Vector),
			m_Current(ItemIndex)
		{
		}
		
	public:
		Iterator &operator ++ (void)
		{
			if (m_Current + 1 == m_Vector->GetSize())
				m_Current = -1;
			else
				m_Current++;

			return *this;
		}
		Iterator &operator -- (void)
		{
			if (m_Current - 1 == -1)
				m_Current = -1;
			else
				m_Current = (m_Current - 1);

			return *this;
		}

		Iterator &operator +=(unsigned int Number)
		{
			if (Number > 0)
			{
				while (Number-- && m_Current)
					this->operator++();
			}
			else
			{
				while (Number++ && m_Current)
					this->operator--();
			}

			return *this;
		}
		Iterator &operator -= (unsigned int Number)
		{
			return this->operator+=(-Number);
		}

		bool operator ==(const Iterator &Other) const
		{
			return (m_Current == Other.m_Current); 
		}
		bool operator !=(const Iterator &Other) const
		{
			return (m_Current != Other.m_Current);
		}

		T *operator * ()
		{
			return &m_Vector->m_Buffer[m_Current];
		}
		T &operator & ()
		{
			return m_Vector->m_Buffer[m_Current];
		}
		T &operator -> ()
		{
			return m_Vector->m_Buffer[m_Current];
		}

	private:
		Vector<T> *m_Vector;
		int m_Current;

		friend class Vector<T>;
	};

	//class ConstIterator
	//{
	//public:
	//	ConstIterator(void) :
	//		m_Vector(NULL),
	//		m_Current(-1)
	//	{
	//	}

	//private:
	//	explicit ConstIterator(const Vector<T> *Vector, const int &ItemIndex) :
	//		m_Vector(static_cast<Vector<T>*>(Vector)),
	//		m_Current(ItemIndex)
	//	{
	//	}

	//public:
	//	ConstIterator &operator ++ (void)
	//	{
	//		if (m_Current + 1 == m_Vector->GetSize())
	//			m_Current = -1;
	//		else
	//			m_Current++;

	//		return *this;
	//	}
	//	ConstIterator &operator -- (void)
	//	{
	//		if (m_Current - 1 == -1)
	//			m_Current = -1;
	//		else
	//			m_Current = (m_Current - 1);

	//		return *this;
	//	}

	//	ConstIterator &operator +=(unsigned int Number)
	//	{
	//		if (Number > 0)
	//		{
	//			while (Number-- && m_Current)
	//				this->operator++();
	//		}
	//		else
	//		{
	//			while (Number++ && m_Current)
	//				this->operator--();
	//		}

	//		return *this;
	//	}
	//	ConstIterator &operator -= (unsigned int Number)
	//	{
	//		return this->operator+=(-Number);
	//	}

	//	bool operator ==(const ConstIterator &Other) const
	//	{
	//		return (m_Current == Other.m_Current); 
	//	}
	//	bool operator !=(const ConstIterator &Other) const
	//	{
	//		return (m_Current != Other.m_Current);
	//	}

	//	T *operator * ()
	//	{
	//		return &m_Vector->m_Buffer[m_Current];
	//	}
	//	const T &operator & ()
	//	{
	//		return m_Vector->m_Buffer[m_Current];
	//	}
	//	const T &operator -> ()
	//	{
	//		return m_Vector->m_Buffer[m_Current];
	//	}
	//	
	//private:
	//	Vector<T> *m_Vector;
	//	int m_Current;

	//	friend class Vector<T>;
	//};

public:
	Vector(void) :
		m_Buffer(NULL),
		m_Size(0)
	{
		Clear();
	}

	Vector(const Vector<T> &Other) :
		m_Buffer(NULL),
		m_Size(0)
	{
		this->operator=(Other);
	}

	~Vector(void)
	{
		Free();
	}

	void Clear(void)
	{
		Free();

		m_Size = 0;
		m_Buffer = NULL;
	}

	void Add(const T &Value)
	{
		T *buffer = new T[m_Size + 1];

		unsigned int i;
		for (i = 0; i < m_Size; i++)
			buffer[i] = m_Buffer[i];

		buffer[m_Size] = Value;

		Free();

		m_Size = i + 1;
		m_Buffer = buffer;
	}

	void Add(const Vector<T> &Other)
	{
		T *buffer = new T[m_Size + Other.GetSize()];

		unsigned int i;
		for (i = 0; i < m_Size; i++)
			buffer[i] = m_Buffer[i];

		for (i = 0; i < Other.GetSize(); i++)
			buffer[m_Size + i] = Other[i];

		Free();

		m_Size += Other.GetSize();
		m_Buffer = buffer;
	}

	void Insert(const unsigned int &Index, const T &Value)
	{
		T *buffer = new T[m_Size + 1];

		unsigned int i;
		for (i = 0; i < Index; i++)
			buffer[i] = m_Buffer[i];

		buffer[Index] = Value;
		
		for (i = Index; i < m_Size; i++)
			buffer[i + 1] = m_Buffer[i];

		Free();

		m_Size = i + 1;
		m_Buffer = buffer;
	}

	void Remove(const unsigned int &Index)
	{
		//if (Index >= m_Length)
		//	Throw

		if (m_Size == 1)
		{
			Free();

			m_Size = 0;
			m_Buffer = NULL;

			return;
		}

		T *buffer = new T[m_Size - 1];

		unsigned int i;
		for (i = 0; i < Index; i++)
			buffer[i] = m_Buffer[i];

		//delete &m_Buffer[Index];
		
		for (i = Index + 1; i < m_Size; i++)
			buffer[i - 1] = m_Buffer[i];

		Free();

		m_Size = i - 1;
		m_Buffer = buffer;
	}
	
	Iterator Remove(const Iterator &Item)
	{
		//if (!Item.m_Current)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Item cannot be null", "")

		Iterator returnValue(this, Item.m_Current);
		returnValue++;

		Remove(Item.m_Current);

		return returnValue;
	}

	const int Find(const T &Item)
	{
		for (unsigned int i = 0; i < m_Size; i++)
			if (m_Buffer[i] == Item)
				return i;

		return -1;
	}

	const Iterator FindIterator(const T &Item)
	{
		const int index = Find(Item);

		if (index > -1)
			return Iterator(this, index);

		return GetEnd();
	}

	T &GetItem(const unsigned int &Index)
	{
		//if (m_Size <= Index)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		return m_Buffer[Index];
	}

	const T &GetItem(const unsigned int &Index) const
	{
		//if (m_Size <= Index)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		return m_Buffer[Index];
	}

private:
	void Free(void) const
	{
		if (m_Size)
			delete []m_Buffer;
	}

public:
	void operator =(const Vector<T> &Other)
	{
		Free();

		m_Size = Other.GetSize();

		m_Buffer = new T[m_Size];

		int index = m_Size - 1;

		while (index >= 0)
		{
			m_Buffer[index] = Other[index];
			index--;
		}
	}

	T &operator [](const unsigned int &Index) const
	{
		//if (Index >= m_Length)
		//	Throw

		return m_Buffer[Index];
	}

	Iterator GetFirst(void)
	{
		if (!m_Size)
			return GetEnd();

		return Iterator(this, 0);
	}

	Iterator GetLast(void)
	{
		if (!m_Size)
			return GetEnd();

		return Iterator(this, m_Size - 1);
	}

	Iterator GetEnd(void)
	{
		return Iterator(this, -1);
	}

	//ConstIterator GetFirst(void) const
	//{
	//	if (!m_Size)
	//		return GetEnd();

	//	return ConstIterator(this, 0);
	//}

	//ConstIterator GetLast(void) const
	//{
	//	if (!m_Size)
	//		return GetEnd();

	//	return ConstIterator(this, m_Size - 1);
	//}

	//ConstIterator GetEnd(void) const
	//{
	//	return ConstIterator(this, -1);
	//}

	const unsigned int &GetSize(void) const
	{
		return m_Size;
	}

private:
	T *m_Buffer;
	unsigned int m_Size;
};

END_NAMESPACE