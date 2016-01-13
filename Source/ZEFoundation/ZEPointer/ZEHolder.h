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
		ZEReferenceCounted* Pointer;

	public:
		bool IsNull() const;
		Type* GetPointer() const;

		void Assign(Type* RawPointer);
		void Release();

		void Copy(const ZEHolder<Type>& OtherHolder);
		Type* Transfer();

		template<typename TargetType>
		TargetType* Cast() const;
		template<typename TargetType>
		const TargetType* ConstCast() const;


		Type& operator*() const;
		Type* operator->() const;
		Type** operator&() const;

		ZEHolder<Type>& operator=(Type* Source);
		ZEHolder<Type>& operator=(const ZEHolder<Type>& Source);

		bool operator==(const Type* RawPointer) const;
		bool operator==(const ZEHolder<Type>& Pointer) const;
		bool operator!=(const Type* RawPointer) const;
		bool operator!=(const ZEHolder<Type>& Pointer) const;


		operator Type*() const;
		operator ZEHolder<const Type>() const;
		#if ZE_DISABLED_CODE
		// Not available unless "C++11 - Explicit Casting Operators" becomes available.
			template<typename TargetType>
			explicit operator TargetType*();
			template<typename TargetType>
			explicit operator const TargetType*();
		#endif


		ZEHolder();
		ZEHolder(Type* RawPointer);
		explicit ZEHolder(const ZEHolder<Type>& OtherHolder);
		~ZEHolder();
};

template<typename Type>
void ZEHolder<Type>::Assign(Type* RawPointer)
{
	if (reinterpret_cast<Type*>(Pointer) == RawPointer)
		return;

	Release();

	if (RawPointer == NULL)
		return;

	Pointer = (ZEReferenceCounted*)RawPointer;
	Pointer->Reference();
}

template<typename Type>
bool ZEHolder<Type>::IsNull() const
{
	return Pointer == NULL;
}

template<typename Type>
void ZEHolder<Type>::Copy(const ZEHolder<Type>& Source)
{
	Assign(Source.GetPointer());
}

template<typename Type>
void ZEHolder<Type>::Release()
{
	if (Pointer != NULL)
		Pointer->Release();

	Pointer = NULL;
}

template<typename Type>
Type* ZEHolder<Type>::GetPointer() const
{
	return reinterpret_cast<Type*>(Pointer);
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
	return (TargetType*)Pointer;
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
	return *(Type*)Pointer;
}

template<typename Type>
Type* ZEHolder<Type>::operator->() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return (Type*)Pointer;
}

template<typename Type>
Type** ZEHolder<Type>::operator&() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return (Type**)&Pointer;
}

template<typename Type>
ZEHolder<Type>& ZEHolder<Type>::operator=(Type* Source)
{
	Assign(Source);
	return *this;
}

template<typename Type>
ZEHolder<Type>& ZEHolder<Type>::operator=(const ZEHolder<Type>& Source)
{
	Copy(Source);
	return *this;
}

template<typename Type>
bool ZEHolder<Type>::operator==(const ZEHolder<Type>& Holder) const
{
	return (Pointer == Holder.GetPointer());
}

template<typename Type>
bool ZEHolder<Type>::operator==(const Type* RawPointer) const
{
	return (Pointer == RawPointer);
}

template<typename Type>
bool ZEHolder<Type>::operator!=(const ZEHolder<Type>& Holder) const
{
	return (Pointer != Holder.GetPointer());
}

template<typename Type>
bool ZEHolder<Type>::operator!=(const Type* RawPointer) const
{
	return (Pointer != RawPointer);
}

template<typename Type>
ZEHolder<Type>::operator Type*() const
{
	return (Type*)Pointer;
}

template<typename Type>
ZEHolder<Type>::operator ZEHolder<const Type>() const
{
	return ZEHolder<const Type>((const Type*)Pointer);
}

#if ZE_DISABLED
template<typename Type>
template<typename TargetType>
ZEHolder<Type>::operator TargetType*()
{
	return Cast<TargetType>();
}

template<typename Type>
template<typename TargetType>
ZEHolder<Type>::operator const TargetType*()
{
	return ConstCast<TargetType>();
}
#endif

template<typename Type>
ZEHolder<Type>::ZEHolder()
{
	Pointer = NULL;
}

template<typename Type>
ZEHolder<Type>::ZEHolder(Type* RawPointer)
{
	Pointer = NULL;
	Assign(RawPointer);
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
