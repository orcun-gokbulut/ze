//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSHolder.h
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

class ZERSResource;

template<typename Type>
class ZERSHolder
{
	private:
		const ZERSResource*			Pointer;

	public:
		bool						IsNull() const;
		Type*						GetPointer() const;
		Type**						GetPointerToPointer() const;

		void						Assign(Type* RawPointer);
		void						Release();
		void						Copy(const ZERSHolder<Type>& OtherHolder);
		Type*						Transfer();

		template<typename TargetType>
		TargetType*					Cast() const;
		template<typename TargetType>
		const TargetType*			ConstCast() const;

		Type&						operator*() const;
		Type*						operator->() const;

		ZERSHolder<Type>&			operator=(Type* Source);
		ZERSHolder<Type>&			operator=(const ZERSHolder<Type>& Source);

		bool						operator==(const Type* RawPointer) const;
		bool						operator==(const ZERSHolder<Type>& Pointer) const;
		bool						operator!=(const Type* RawPointer) const;
		bool						operator!=(const ZERSHolder<Type>& Pointer) const;

									operator Type*() const;
									operator ZERSHolder<const Type>() const;

									#if ZE_DISABLED_CODE
									// Not available unless "C++11 - Explicit Casting Operators" becomes available.
										template<typename TargetType>
										explicit operator TargetType*();
										template<typename TargetType>
										explicit operator const TargetType*();
									#endif


									ZERSHolder();
									ZERSHolder(Type* RawPointer);
									explicit ZERSHolder(const ZERSHolder<Type>& OtherHolder);
									~ZERSHolder();
};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

template<typename Type>
bool ZERSHolder<Type>::IsNull() const
{
	return Pointer == NULL;
}

template<typename Type>
Type* ZERSHolder<Type>::GetPointer() const
{
	return static_cast<Type*>(const_cast<ZERSResource*>(Pointer));
}

template<typename Type>
Type** ZERSHolder<Type>::GetPointerToPointer() const
{
	return (Type**)&Pointer;
}

template<typename Type>
void ZERSHolder<Type>::Assign(Type* RawPointer)
{
	const ZERSResource* CastedPointer = static_cast<const ZERSResource*>(RawPointer);
	if (Pointer == CastedPointer)
		return;

	Release();

	Pointer = CastedPointer;
	if (Pointer != NULL)
		Pointer->Reference();
}

template<typename Type>
void ZERSHolder<Type>::Release()
{
	if (Pointer != NULL)
		Pointer->Release();

	Pointer = NULL;
}

template<typename Type>
void ZERSHolder<Type>::Copy(const ZERSHolder<Type>& Source)
{
	Release();
	
	Pointer = Source.Pointer;
	if (Pointer != NULL)
		Pointer->Reference();
}

template<typename Type>
Type* ZERSHolder<Type>::Transfer()
{
	Type* Temp = Pointer;
	Pointer = NULL;
	return Temp;
}

template<typename Type>
template<typename TargetType>
TargetType* ZERSHolder<Type>::Cast() const
{
	return static_cast<TargetType*>(GetPointer());
}

template<typename Type>
template<typename TargetType>
const TargetType* ZERSHolder<Type>::ConstCast() const
{
	return const_cast<const Type*>(Pointer);
}

template<typename Type>
Type& ZERSHolder<Type>::operator*() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return *GetPointer();
}

template<typename Type>
Type* ZERSHolder<Type>::operator->() const
{
	zeDebugCheck(Pointer == NULL, "Holder does not points any data structure.");
	return GetPointer();
}

template<typename Type>
ZERSHolder<Type>& ZERSHolder<Type>::operator=(Type* Source)
{
	Assign(Source);
	return *this;
}

template<typename Type>
ZERSHolder<Type>& ZERSHolder<Type>::operator=(const ZERSHolder<Type>& Source)
{
	Copy(Source);
	return *this;
}

template<typename Type>
bool ZERSHolder<Type>::operator==(const ZERSHolder<Type>& Holder) const
{
	return (Pointer == Holder.Pointer);
}

template<typename Type>
bool ZERSHolder<Type>::operator==(const Type* RawPointer) const
{
	return (GetPointer() == RawPointer);
}

template<typename Type>
bool ZERSHolder<Type>::operator!=(const ZERSHolder<Type>& Holder) const
{
	return (Pointer != Holder.Pointer);
}

template<typename Type>
bool ZERSHolder<Type>::operator!=(const Type* RawPointer) const
{
	return (GetPointer() != RawPointer);
}

template<typename Type>
ZERSHolder<Type>::operator Type*() const
{
	return GetPointer();
}

template<typename Type>
ZERSHolder<Type>::operator ZERSHolder<const Type>() const
{
	return ZERSHolder<const Type>(GetPointer());
}

#if ZE_DISABLED
template<typename Type>
template<typename TargetType>
ZERSHolder<Type>::operator TargetType*()
{
	return Cast<TargetType>();
}

template<typename Type>
template<typename TargetType>
ZERSHolder<Type>::operator const TargetType*()
{
	return ConstCast<TargetType>();
}
#endif

template<typename Type>
ZERSHolder<Type>::ZERSHolder()
{
	Pointer = NULL;
}

template<typename Type>
ZERSHolder<Type>::ZERSHolder(Type* RawPointer)
{
	Pointer = NULL;
	Assign(RawPointer);
}

template<typename Type>
ZERSHolder<Type>::ZERSHolder(const ZERSHolder<Type>& OtherHolder)
{
	Pointer = NULL;
	Copy(OtherHolder);
}

template<typename Type>
ZERSHolder<Type>::~ZERSHolder()
{
	Release();
}
