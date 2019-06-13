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

template <class T> class List
{
private:
	struct Node
	{
	public:
		Node(const T &Value) :
			Next(NULL),
			Prev(NULL),
			Item(Value)
		{
		}

		Node *Next;
		Node *Prev;
		T Item;
	};

public:
	class Iterator
	{
	public:
		Iterator(void) :
			m_Current(NULL)
		{
		}

	private:
		explicit Iterator(Node *Element) :
			m_Current(Element)
		{
		}
		
	public:
		Iterator &operator ++ (void)
		{
			m_Current = m_Current->Next;
			return *this;
		}
		Iterator &operator -- (void)
		{
			m_Current = m_Current->Prev;
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

		T &operator * ()
		{
			return m_Current->Item;
		}
		T *operator -> ()
		{
			return &m_Current->Item;
		}

	private:
		Node *m_Current;

		friend class List<T>;
	};

	class ConstIterator
	{
	public:
		ConstIterator(void) :
			m_Current(NULL)
		{
		}

	private:
		explicit ConstIterator(Node *Element) :
			m_Current(Element)
		{
		}

	public:
		ConstIterator &operator ++ (void)
		{
			m_Current = m_Current->Next;
			return *this;
		}
		ConstIterator &operator -- (void)
		{
			m_Current = m_Current->Prev;
			return *this;
		}

		ConstIterator &operator += (unsigned int Number)
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
		ConstIterator &operator -= (unsigned int Number)
		{
			return this->operator+=(-Number);
		}
		
		bool operator == (const Iterator &Other) const
		{
			return (m_Current == Other.m_Current); 
		}
		bool operator != (const Iterator &Other) const
		{
			return (m_Current != Other.m_Current);
		}
		bool operator == (const ConstIterator &Other) const
		{
			return (m_Current == Other.m_Current); 
		}
		bool operator != (const ConstIterator &Other) const
		{
			return (m_Current != Other.m_Current);
		}

		const T &operator * ()
		{
			return m_Current->Item;
		}
		const T *operator -> ()
		{
			return &m_Current->Item;
		}

	private:
		Node *m_Current;

		friend class List<T>;
	};

public:
	List(void) :
		m_First(NULL),
		m_Last(NULL),
		m_Size(0)
	{
	}
	
	List(const List<T> &Other) :
		m_First(NULL),
		m_Last(NULL),
		m_Size(0)
	{
		*this = Other;
	}

	~List(void)
	{
		Clear();
	}

	void Clear(void)
	{
		Node *next = NULL;

		while (m_First)
		{
			next = m_First->Next;

			delete m_First;

			m_First = next;
		}

		m_Last = NULL;
		m_Size = 0;
	}

	void Add(const T &Item)
	{
		m_Size++;

		Node *node = new Node(Item);

		if (!m_First)
			m_First = node;

		node->Prev = m_Last;

		if (m_Last)
			m_Last->Next = node;

		m_Last = node;
	}

	void Add(const List<T> &Other)
	{
		for (List<T>::ConstIterator it = Other.GetFirst(), end = Other.GetEnd(); it != end; it++)
			Add(*it);
	}

	void Insert(const unsigned int &Index, const T &Item)
	{
		//if (Index >= m_Size)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		Node *node = m_First;

		unsigned int i = Index;

		while (i)
		{
			i--;
			node = node->Next;
		}

		Node *newNode = new Node(Item);

		newNode->Prev = node->Prev;
		newNode->Next = node;

		if (node->Prev)
			node->Prev->Next = newNode;

		node->Prev = newNode;
		
		m_Size++;



		//if (Index >= m_Size)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		//Element *elem = m_First;

		//unsigned int i = Index - 1;

		//while (i)
		//{
		//	i--;
		//	elem = elem->Next;
		//}

		//Element *newElem = new Element(Item);

		//newElem->Prev = elem;
		//newElem->Next = elem->Next;

		//if (elem->Next)
		//	elem->Next->Prev = newElem;

		//elem->Next = newElem;
		//
		//m_Size++;
	}

	void Remove(const unsigned int &Index)
	{
		//if (m_Size <= Index)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		Node *node = m_First;

		unsigned int i = Index;

		while (i)
		{
			i--;
			node = node->Next;
		}

		if (node->Prev)
			node->Prev->Next = node->Next;

		if (node->Next)
			node->Next->Prev = node->Prev;

		delete node;

		m_Size--;
	}

	Iterator Remove(const Iterator &Item)
	{
		//if (!Item.m_Current)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Item cannot be null", "")

		Iterator returnValue(Item);
		returnValue++;

		Node *node = Item.m_Current;

		if (node == m_First)
			m_First = node->Next;
		else if (node->Prev)
			node->Prev->Next = node->Next;

		if (node == m_Last)
			m_Last = node->Prev;
		else if (node->Next)
			node->Next->Prev = node->Prev;

		delete node;

		m_Size--;

		return returnValue;
	}

	const unsigned int Find(const T &Item)
	{
		Node *node = m_Last;

		unsigned int i = 0;

		while (node->Item != Item && i > -1)
		{
			node = node->Prev;
			i--;
		}

		return i;
	}

	const Iterator FindIterator(const T &Item)
	{
		Node *node = m_First;

		while (node)
		{
			if (node->Item == Item)
				break;

			node = node->Next;
		}

		if (!node)
			return Iterator(NULL);

		return Iterator(node);
	}

	T &GetItem(const unsigned int &Index)
	{
		//if (m_Size <= Index)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		Node *node = m_First;

		unsigned int i = Index;

		while (i)
		{
			i--;
			node = node->Next;
		}

		return node->Item;
	}

	const T &GetItem(const unsigned int &Index) const
	{
		//if (m_Size <= Index)
		//	THROW_EXCEPTION_AND_STOP(Exception::ER_INVALID_PARAMETER, "Index is out of bounding of List", "")

		Node *node = m_First;

		unsigned int i = Index;

		while (i)
		{
			i--;
			node = node->Next;
		}

		return node->Item;
	}

	void operator = (const List<T> &Other)
	{
		if (this == &Other)
			return;

		Clear();

		Node *node = Other.m_First;

		while (node)
		{
			Add(node->Item);

			node = node->Next;
		}
	}

	Iterator GetFirst(void)
	{
		return Iterator(m_First);
	}

	Iterator GetLast(void)
	{
		return Iterator(m_First);
	}

	Iterator GetEnd(void)
	{
		return Iterator(NULL);
	}

	ConstIterator GetFirst(void) const
	{
		return ConstIterator(m_First);
	}

	ConstIterator GetLast(void) const
	{
		return ConstIterator(m_First);
	}

	ConstIterator GetEnd(void) const
	{
		return ConstIterator(NULL);
	}

	T &operator [] (const unsigned int &Index)
	{
		return GetItem(Index);
	}

	const T &operator [] (const unsigned int &Index) const
	{
		return GetItem(Index);
	}

	const unsigned int &GetSize(void) const
	{
		return m_Size;
	}

private:
	Node *m_First;
	Node *m_Last;
	unsigned int m_Size;
};

END_NAMESPACE