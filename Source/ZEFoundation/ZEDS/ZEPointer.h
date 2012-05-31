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
#ifndef __ZE_POINTER_H__
#define __ZE_POINTER_H__

#include "ZEError.h"

template<typename Type>
class ZEPointer
{
	private:
		Type* Pointer;

	public:
		void Create(Type* RawPointer)
		{
			if (RawPointer == Pointer)
				return;

			Release();

			if (RawPointer == NULL)
				return;

			Pointer = RawPointer;
		}

		void Copy(ZEPointer<Type>& OtherPointer)
		{
			if (OtherPointer.Pointer == NULL)
				Release();
			else
			{
				Pointer = OtherPointer.Pointer;
				OtherPointer.Pointer = NULL;
			}
		}

		bool IsNull() const
		{
			return Pointer == NULL;
		}

		void Release()
		{
			if (Pointer != NULL)
				delete Pointer;

			Pointer = NULL;
		}

		Type* GetPointer() const
		{
			return Pointer;
		}

		Type* Transfer()
		{
			Type* Temp = Pointer;
			Pointer = NULL;
			return Temp;
		}

		operator Type*()
		{
			return Pointer;
		}

		Type& operator*()
		{
			zeDebugCheck(Pointer == NULL, "ZEPointer does not points any data structure.");
			return *Pointer;
		}

		Type* operator->()
		{
			zeDebugCheck(Pointer == NULL, "ZEPointer does not points any data structure.");
			return Pointer;
		}

		ZEPointer<Type>& operator=(Type* RawPointer)
		{
			Create(RawPointer);
			return *this;
		}

		ZEPointer<Type>& operator=(ZEPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
			return *this;
		}

		ZEPointer()
		{
			Pointer = NULL;
		}

		ZEPointer(Type* RawPointer)
		{
			Pointer = NULL;
			Create(RawPointer);
		}

		explicit ZEPointer(ZEPointer<Type>& OtherPointer)
		{
			Pointer = NULL;
			Copy(OtherPointer);
		}

		~ZEPointer()
		{
			Release();
		}
};

#endif
