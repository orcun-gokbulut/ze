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
		ZEReferenceCount* ReferenceCount;

	public:
		bool IsNull()
		{
			if (Pointer == NULL || ReferenceCount == NULL || ReferenceCount->Strong == 0)
				return true;
			else
				return false;
		}

		void Copy(const ZEWeakPointer<Type>& OtherPointer)
		{
			if (OtherPointer.Pointer == Pointer)
				return;

			Release();

			if (OtherPointer.Pointer != NULL)
			{
				Pointer = OtherPointer.Pointer;
				ReferenceCount = OtherPointer.ReferenceCount;
				ReferenceCount->Weak++;			
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
				ReferenceCount = OtherPointer.ReferanceCount;
				ReferenceCount->Weak++;			
			}
		}

		ZESize GetReferenceCount()
		{
			if (ReferenceCount != NULL)
				return ReferenceCount->Strong;
			else
				return 0;
		}

		ZESize GetWeakReferenceCount()
		{
			if (ReferenceCount != NULL)
				return ReferenceCount->Weak;
			else
				return 0;
		}

		Type* GetPointer()
		{
			return Pointer;
		}

		ZESharedPointer<Type> GetInstance()
		{
			ZESharedPointer<Type> SharedPointer;

			if (!IsNull())
			{
				SharedPointer.Pointer = Pointer;
				SharedPointer.ReferanceCount = ReferenceCount;
				SharedPointer.ReferanceCount->Strong++;
			}

			return SharedPointer;
		}

		void Release()
		{
			if (ReferenceCount != NULL)
			{
				Pointer = NULL;
				ReferenceCount->Weak--;
				if (ReferenceCount->Strong == 0 && ReferenceCount->Weak == 0)
					delete ReferenceCount;

				ReferenceCount = NULL;
			}
		}

		Type& operator*()
		{
			zeAssert(Pointer == NULL, "ZEWeakPointer does not points any data structure.");
			zeAssert(ReferenceCount == 0 || ReferenceCount->Strong == 0, "ZEWeakPointer is ilvalid. Pointer is deleted.");

			return *Pointer;
		}

		Type* operator->()
		{
			zeAssert(Pointer == NULL, "ZEWeakPointer does not points any data structure.");
			zeAssert(ReferenceCount == 0 || ReferenceCount->Strong == 0, "ZEWeakPointer is ilvalid. Pointer is deleted.");

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
			ReferenceCount = NULL;
		}

		ZEWeakPointer(const ZESharedPointer<Type>& OtherPointer)
		{
			Pointer = NULL;
			ReferenceCount = NULL;
			Copy(OtherPointer);
		}

		ZEWeakPointer(const ZEWeakPointer<Type>& OtherPointer)
		{
			Pointer = NULL;
			ReferenceCount = NULL;
			Copy(OtherPointer);
		}

		~ZEWeakPointer()
		{
			Release();
		}

		static void Swap(const ZEWeakPointer<Type>& A, const ZEWeakPointer<Type>& B)
		{
			Type* TempP = A.Pointer;
			ZESize TempRC = A.ReferenceCount;

			A.Pointer = B.Pointer;
			A.ReferenceCount = B.ReferenceCount;

			B.Pointer = TempP;
			B.ReferenceCount = TempRC;
		}
};

#endif
