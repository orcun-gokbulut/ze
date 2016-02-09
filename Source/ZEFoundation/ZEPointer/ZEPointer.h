//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPointer.h
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
#include "ZEDeletor.h"

template<typename Type, typename Deletor = ZEDeletor<Type>>
class ZEPointer
{
	private:
		Type*					Pointer;

	public:
		Type*					GetPointer() const;
		bool					IsNull() const;

		void					Assign(Type* RawPointer);
		void					Release();
		void					Copy(ZEPointer& OtherPointer);
		Type*					Transfer();

		operator				Type*();
		Type&					operator*();
		Type*					operator->();

		ZEPointer&				operator=(Type* RawPointer);
		ZEPointer&				operator=(ZEPointer& OtherPointer);

								ZEPointer();
								ZEPointer(Type* RawPointer);
		explicit				ZEPointer(ZEPointer& OtherPointer);
								~ZEPointer();


};

template<typename Type, typename Deletor>
bool ZEPointer<Type, Deletor>::IsNull() const
{
	return Pointer == NULL;
}

template<typename Type, typename Deletor>
Type* ZEPointer<Type, Deletor>::GetPointer() const
{
	return Pointer;
}

template<typename Type, typename Deletor>
void ZEPointer<Type, Deletor>::Assign(Type* RawPointer)
{
	if (RawPointer == Pointer)
		return;

	Release();

	if (RawPointer == NULL)
		return;

	Pointer = RawPointer;
}

template<typename Type, typename Deletor>
void ZEPointer<Type, Deletor>::Release()
{
	if (Pointer != NULL)
		Deletor::Delete(Pointer);

	Pointer = NULL;
}

template<typename Type, typename Deletor>
void ZEPointer<Type, Deletor>::Copy(ZEPointer<Type, Deletor>& OtherPointer)
{
	if (OtherPointer.Pointer == NULL)
	{
		Release();
	}
	else
	{
		Pointer = OtherPointer.Pointer;
		OtherPointer.Pointer = NULL;
	}
}

template<typename Type, typename Deletor>
Type* ZEPointer<Type, Deletor>::Transfer()
{
	Type* Temp = Pointer;
	Pointer = NULL;
	return Temp;
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>::operator Type*()
{
	return Pointer;
}

template<typename Type, typename Deletor>
Type& ZEPointer<Type, Deletor>::operator*()
{
	zeDebugCheck(Pointer == NULL, "ZEPointer does not points any data structure.");
	return *Pointer;
}

template<typename Type, typename Deletor>
Type* ZEPointer<Type, Deletor>::operator->()
{
	zeDebugCheck(Pointer == NULL, "ZEPointer does not points any data structure.");
	return Pointer;
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>& ZEPointer<Type, Deletor>::operator=(Type* RawPointer)
{
	Assign(RawPointer);
	return *this;
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>& ZEPointer<Type, Deletor>::operator=(ZEPointer<Type, Deletor>& OtherPointer)
{
	Copy(OtherPointer);
	return *this;
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>::ZEPointer()
{
	Pointer = NULL;
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>::ZEPointer(Type* RawPointer)
{
	Pointer = NULL;
	Assign(RawPointer);
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>::ZEPointer(ZEPointer<Type, Deletor>& OtherPointer)
{
	Pointer = NULL;
	Copy(OtherPointer);
}

template<typename Type, typename Deletor>
ZEPointer<Type, Deletor>::~ZEPointer()
{
	Release();
}
