//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWeakPointer.h
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
#ifndef __ZE_WEAK_POINTER_H__
#define __ZE_WEAK_POINTER_H__

#include "ZESharedPointer.h"

template<typename Type>
class ZEWeakPointer
{
	private:
		Type* Pointer;
		ZEReferanceCount* ReferanceCount;

	public:
		void IsEmpty()
		{
			if (ReferanceCount == NULL || ReferanceCount->Strong == 0)
				return true;
		}

		void Copy(const ZEWeakPointer<Type>& OtherPointer)
		{
			if (OtherPointer.Pointer == Pointer)
				return;

			Release();

			if (OtherPointer.Pointer != NULL)
			{
				Pointer = OtherPointer.Pointer;
				ReferanceCount = OtherPointer.ReferanceCount;
				ReferanceCount->Weak++;			
			}
		}

		void Copy(const ZESharedPointer<Type>& OtherPointer)
		{
			if (OtherPointer.Pointer == Pointer)
				return;

			Release();

			if (OtherPointer.Pointer != NULL)
			{
				Pointer = OtherPointer.Pointer;
				ReferanceCount = OtherPointer.ReferanceCount;
				ReferanceCount->Weak++;			
			}
		}

		size_t GetReferanceCount()
		{
			return ReferanceCount->Strong;
		}

		ZESharedPointer<Type> GetInstance()
		{
			if (ReferanceCount->)
			ZESharedPointer<Type> SharedPointer;
			SharedPointer.Pointer = Pointer;
			SharedPointer.ReferanceCount = ReferanceCount;

		}

		void Release()
		{
			if (ReferanceCount != NULL)
			{
				Pointer = NULL;
				ReferanceCount->Weak--;
				if (ReferanceCount->Strong == 0 && ReferanceCount->Weak == 0)
				{
					delete ReferanceCount;
				}
				ReferanceCount = NULL;
			}
		}

		Type& operator*()
		{
			if (ReferanceCount->Strong == 0)
				Pointer = NULL;

			zeAssert(Pointer == NULL, "ZEPointer does not points any data structure.");
			return *Pointer;
		}

		Type* operator->()
		{
			if (ReferanceCount->Strong == 0)
				Pointer = NULL;

			zeAssert(Pointer == NULL, "ZEPointer does not points any data structure.");

			return Pointer;
		}

		ZEWeakPointer<Type>& operator=(ZESharedPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
			return *this;
		}

		ZEWeakPointer<Type>& operator=(ZEWeakPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
			return *this;
		}

		ZEWeakPointer()
		{
			Pointer = NULL;
			ReferanceCount = NULL;
		}

		ZEWeakPointer(const ZESharedPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
		}

		ZEWeakPointer(const ZEWeakPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
		}

		~ZEWeakPointer()
		{
			Release();
		}

		static void Swap(const ZEWeakPointer<Type>& A, const ZEWeakPointer<Type>& B)
		{
			Type* TempP = A.Pointer;
			size_t TempRC = A.ReferanceCount;

			A.Pointer = B.Pointer;
			A.ReferanceCount = B.ReferanceCount;

			B.Pointer = TempP;
			B.ReferanceCount = TempRC;
		}
};

#endif
