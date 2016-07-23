//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEHolder.h
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

#pragma once

#include "ZEError.h"
#include "ZEReferenceCounted.h"

template<typename Type>
class ZEHolder
{
	private:
		const ZEReferenceCounted*			Pointer;

	public:
		bool								IsNull() const;
		Type*								GetPointer() const;
		Type&								GetValue() const;

		void								Reference(Type* RawPointer);
		void								Overwrite(Type* RawPointer);
		void								Release();
		void								Copy(const ZEHolder<Type>& OtherHolder);
		Type*								Transfer();

		template<typename TargetType>
		TargetType*							Cast() const;
		template<typename TargetType>
		const TargetType*					ConstCast() const;

		Type&								operator*() const;
		Type*								operator->() const;

		ZEHolder<Type>&						operator=(Type* RawPointer);
		ZEHolder<Type>&						operator=(const ZEHolder<Type>& OtherHolder);

											operator Type*() const;
											operator ZEHolder<const Type>() const;

											ZEHolder();
											ZEHolder(Type* RawPointer);
		explicit							ZEHolder(const ZEHolder<Type>& OtherHolder);
											~ZEHolder();
};

template<typename Type>
bool ZEHolder<Type>::IsNull() const
{
	return Pointer == NULL;
}

template<typename Type>
Type* ZEHolder<Type>::GetPointer() const
{
	return static_cast<Type*>(const_cast<ZEReferenceCounted*>(Pointer));
}

template<typename Type>
Type& ZEHolder<Type>::GetValue() const
{
	return *GetPointer();
}
template<typename Type>
void ZEHolder<Type>::Reference(Type* RawPointer)
{
	const ZEReferenceCounted* CastedPointer = static_cast<const ZEReferenceCounted*>(RawPointer);
	if (Pointer == CastedPointer)
		return;

	Release();

	Pointer = CastedPointer;
	if (Pointer != NULL)
		Pointer->Reference();
}

template<typename Type>
void ZEHolder<Type>::Release()
{
	if (Pointer != NULL)
		Pointer->Release();

	Pointer = NULL;
}

template<typename Type>
void ZEHolder<Type>::Copy(const ZEHolder<Type>& OtherHolder)
{
	Release();
	
	Pointer = OtherHolder.Pointer;
	if (Pointer != NULL)
		Pointer->Reference();
}

template<typename Type>
void ZEHolder<Type>::Overwrite(Type* RawPointer)
{
	Release();
	Pointer = RawPointer;
}

template<typename Type>
Type* ZEHolder<Type>::Transfer()
{
	Type* Temp = Pointer;
	Pointer = NULL;
	return Temp;
}

template<typename Type>
template<typename TargetType>
TargetType* ZEHolder<Type>::Cast() const
{
	return static_cast<TargetType*>(GetPointer());
}

template<typename Type>
template<typename TargetType>
const TargetType* ZEHolder<Type>::ConstCast() const
{
	return const_cast<const Type*>(Pointer);
}

template<typename Type>
Type& ZEHolder<Type>::operator*() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return *GetPointer();
}

template<typename Type>
Type* ZEHolder<Type>::operator->() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return GetPointer();
}

template<typename Type>
ZEHolder<Type>& ZEHolder<Type>::operator=(Type* RawPointer)
{
	Reference(RawPointer);
	return *this;
}

template<typename Type>
ZEHolder<Type>& ZEHolder<Type>::operator=(const ZEHolder<Type>& OtherHolder)
{
	Copy(OtherHolder);
	return *this;
}

template<typename Type>
ZEHolder<Type>::operator Type*() const
{
	return GetPointer();
}

template<typename Type>
ZEHolder<Type>::operator ZEHolder<const Type>() const
{
	return ZEHolder<const Type>(GetPointer());
}

template<typename Type>
ZEHolder<Type>::ZEHolder()
{
	Pointer = NULL;
}

template<typename Type>
ZEHolder<Type>::ZEHolder(Type* RawPointer)
{
	Pointer = NULL;
	Reference(RawPointer);
}

template<typename Type>
ZEHolder<Type>::ZEHolder(const ZEHolder<Type>& OtherHolder)
{
	Pointer = NULL;
	Copy(OtherHolder);
}

template<typename Type>
ZEHolder<Type>::~ZEHolder()
{
	Release();
}
