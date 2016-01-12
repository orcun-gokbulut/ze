//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRHolder.h
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

#include "ZEGRResource.h"
#include "ZEError.h"

template<typename Type>
class ZEGRHolder
{
	private:
		ZEGRResource*			Resource;

	public:
		bool					IsNull() const;
		Type*					GetPointer() const;

		void					Assign(Type* RawPointer);
		void					Release();

		void					Copy(const ZEGRHolder<Type>& OtherHolder);
		Type*					Transfer();

		template<typename TargetType>
		TargetType*				Cast() const;
		template<typename TargetType>
		const TargetType*		ConstCast() const;

								operator Type*() const;
								operator const Type*() const;

		// Not available unless "C++11 - Explicit Casting Operators" becomes available.
		/*template<typename TargetType>
		explicit				operator TargetType*();
		template<typename TargetType>
		explicit				operator const TargetType*();*/

		Type&					operator*() const;
		Type*					operator->() const;
		Type**					operator&() const;

		ZEGRHolder<Type>&		operator=(Type* Source);
		ZEGRHolder<Type>&		operator=(const ZEGRHolder<Type>& Source);

		bool					operator==(const Type* RawPointer) const;
		bool					operator!=(const Type* RawPointer) const;

								ZEGRHolder();
								ZEGRHolder(Type* RawPointer);
		explicit				ZEGRHolder(const ZEGRHolder<Type>& OtherHolder);
								~ZEGRHolder();
};

template<typename Type>
void ZEGRHolder<Type>::Assign(Type* const RawPointer)
{
	if ((ZEGRResource*)RawPointer == Resource)
		return;

	Release();

	if (RawPointer == NULL)
		return;

	Resource = (ZEGRResource*)RawPointer;
	Resource->AddRef();
}

template<typename Type>
bool ZEGRHolder<Type>::IsNull() const
{
	return Resource == NULL;
}

template<typename Type>
void ZEGRHolder<Type>::Copy(const ZEGRHolder<Type>& Source)
{
	Assign(Source.GetPointer());
}

template<typename Type>
void ZEGRHolder<Type>::Release()
{
	if (Resource != NULL)
		Resource->Release();

	Resource = NULL;
}

template<typename Type>
Type* ZEGRHolder<Type>::GetPointer() const
{
	return reinterpret_cast<Type*>(Resource);
}

template<typename Type>
Type* ZEGRHolder<Type>::Transfer()
{
	Type* Temp = Resource;
	Resource = NULL;
	return Temp;
}

template<typename Type>
template<typename TargetType>
TargetType* ZEGRHolder<Type>::Cast() const
{
	return (TargetType*)Resource;
}

template<typename Type>
template<typename TargetType>
const TargetType* ZEGRHolder<Type>::ConstCast() const
{
	return reinterpret_cast<const Type*>(Resource);
}

template<typename Type>
ZEGRHolder<Type>::operator Type*() const
{
	return (Type*)Resource;
}

template<typename Type>
ZEGRHolder<Type>::operator const Type*() const
{
	return Resource;
}

/*template<typename Type>
template<typename TargetType>
ZEGRHolder<Type>::operator TargetType*()
{
	return Cast<TargetType>();
}

template<typename Type>
template<typename TargetType>
ZEGRHolder<Type>::operator const TargetType*()
{
	return ConstCast<TargetType>();
}*/

template<typename Type>
Type& ZEGRHolder<Type>::operator*() const
{
	zeDebugCheck(Resource == NULL, "ZEPointer does not points any data structure.");
	return *(Type*)Resource;
}

template<typename Type>
Type* ZEGRHolder<Type>::operator->() const
{
	zeDebugCheck(Resource == NULL, "ZEPointer does not points any data structure.");
	return (Type*)Resource;
}

template<typename Type>
Type** ZEGRHolder<Type>::operator&() const
{
	zeDebugCheck(Resource == NULL, "ZEPointer does not points any data structure.");
	return (Type**)&Resource;
}

template<typename Type>
ZEGRHolder<Type>& ZEGRHolder<Type>::operator=(Type* Source)
{
	Assign(Source);
	return *this;
}

template<typename Type>
ZEGRHolder<Type>& ZEGRHolder<Type>::operator=(const ZEGRHolder<Type>& Source)
{
	Copy(Source);
	return *this;
}

template<typename Type>
bool ZEGRHolder<Type>::operator==(const Type* RawPointer) const
{
	return (Resource == RawPointer);
}

template<typename Type>
bool ZEGRHolder<Type>::operator!=(const Type* RawPointer) const
{
	return (Resource != RawPointer);
}

template<typename Type>
ZEGRHolder<Type>::ZEGRHolder()
{
	Resource = NULL;
}

template<typename Type>
ZEGRHolder<Type>::ZEGRHolder(Type* RawPointer)
{
	Resource = NULL;
	Assign(RawPointer);
}

template<typename Type>
ZEGRHolder<Type>::ZEGRHolder(const ZEGRHolder<Type>& OtherHolder)
{
	Resource = NULL;
	Copy(OtherHolder);
}

template<typename Type>
ZEGRHolder<Type>::~ZEGRHolder()
{
	Release();
}
