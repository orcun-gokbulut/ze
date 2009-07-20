//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - LinkedList.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#ifndef __ZE_LINKED_LIST__
#define __ZE_LINKED_LIST__

template <typename T> class ZELinkedList 
{
private:
	struct LinkedListElement
	{
		LinkedListElement *Forward;
		LinkedListElement *Backward;
		T *Object;
	};

	LinkedListElement *HeadElement;
	LinkedListElement *TailElement;
	int Size;

public:
	class Iterator
	{
	private:
		LinkedListElement *Pointed;
		ZELinkedList *List;
	public:
		Iterator(LinkedListElement *Instantiated,ZELinkedList *Belong);
		Iterator(const Iterator &Copied);
		~Iterator();
		T *operator++();
		T *operator++(int);
		T *operator--();
		T *operator--(int);
		Iterator operator=(const Iterator &Other)
		{
			Pointed = Other.Pointed;

			return *this;
		}
		bool IsEnd();
		T *GetPointed();
		T *DeletePointed();
		void AddElementForward(T *);
		void AddElementBackward(T *);
	};

	ZELinkedList();
	~ZELinkedList();

	Iterator GenerateHeadIterator()
	{
		Iterator That(HeadElement,this);
		That++;

		return That;
	}
	Iterator GenerateTailIterator()
	{
		Iterator That(TailElement,this);
		That--;

		return That;
	}

	inline int GetSize()
	{
		return Size;
	}
};

//-------------------------------definitions----------------------------//

template <typename T> ZELinkedList<T>::Iterator::Iterator(LinkedListElement *Instantiated,ZELinkedList *Belong)
{
	List = Belong;
	
	Pointed = Instantiated;
}

template <typename T> ZELinkedList<T>::Iterator::Iterator(const Iterator &Copied)
{
	List = Copied.List;
	Pointed = Copied.Pointed;			
}

template <typename T> ZELinkedList<T>::Iterator::~Iterator()
{}

template <typename T> ZELinkedList<T>::ZELinkedList()
{
	HeadElement = new LinkedListElement;
	TailElement = new LinkedListElement;

	HeadElement->Backward = NULL;
	HeadElement->Forward = TailElement;
	HeadElement->Object = NULL;
	TailElement->Forward = NULL;
	TailElement->Backward = HeadElement;
	TailElement->Object = NULL;

	Size = 0;
}

template <typename T> ZELinkedList<T>::~ZELinkedList()
{
	Iterator Z(HeadElement,this);
	Z++;

	while (1)
	{
		if (Z.GetPointed() == NULL) break;
		delete Z.DeletePointed();
		Z++;
	}
}

template <typename T> T *ZELinkedList<T>::Iterator::operator++()
{
	Pointed = Pointed->Forward;
	return Pointed->Object;
}

template <typename T> T *ZELinkedList<T>::Iterator::operator++(int)
{
	Pointed = Pointed->Forward;
	return Pointed->Object;
}

template <typename T> T *ZELinkedList<T>::Iterator::operator--()
{
	Pointed = Pointed->Backward;
	return Pointed->Object;
}

template <typename T> T *ZELinkedList<T>::Iterator::operator--(int)
{
	Pointed = Pointed->Backward;
	return Pointed->Object;
}

template <typename T> bool ZELinkedList<T>::Iterator::IsEnd()
{
	return (*this).GetPointed() == NULL;
}

template <typename T> T *ZELinkedList<T>::Iterator::GetPointed()
{
	return Pointed->Object;
}

template <typename T> T *ZELinkedList<T>::Iterator::DeletePointed()
{
	LinkedListElement *ToBeDeleted = Pointed;
	T *ToBeReturned;
	ToBeReturned = Pointed->Object;

	Pointed->Forward->Backward = Pointed->Backward;
	Pointed = Pointed->Backward->Forward = Pointed->Forward;

	delete ToBeDeleted;

	List->Size--;

	return ToBeReturned;
}

template <typename T> void ZELinkedList<T>::Iterator::AddElementForward(T *ToBeAdded)
{
	LinkedListElement *Element = new LinkedListElement;
	Element->Object = ToBeAdded;

	Element->Forward = Pointed->Forward;
	Element->Backward = Pointed;

	Pointed->Forward = Element;
	Element->Forward->Backward = Element;

	Pointed = Element;

	List->Size++;
}

template <typename T> void ZELinkedList<T>::Iterator::AddElementBackward(T *ToBeAdded)
{
	LinkedListElement *Element = new LinkedListElement;
	Element->Object = ToBeAdded;

	Element->Backward = Pointed->Backward;
	Element->Forward = Pointed;

	Pointed->Backward = Element;
	Element->Backward->Forward = Element;

	Pointed = Element;

	List->Size++;
}

#endif
