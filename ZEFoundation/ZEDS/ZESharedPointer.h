//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESharedPointer.h
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
#ifndef __ZE_SHARED_POINTER_H__
#define __ZE_SHARED_POINTER_H__

struct ZEReferanceCount
{
	size_t Strong;
	size_t Weak;
};

template<typename Type>
class ZESharedPointer
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

		void Create(Type* RawPointer)
		{
			if (RawPointer == NULL)
				return;

			Release();

			Pointer = RawPointer;
			ReferanceCount = new ZEReferanceCount;
			ReferanceCount->Weak = 0;
			ReferanceCount->Strong = 1;
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
				ReferanceCount->Strong++;			
			}
		}

		size_t GetReferanceCount()
		{
			return ReferanceCount->Strong;
		}

		void Release()
		{
			if (ReferanceCount == NULL)
				return NULL;

			ReferanceCount->Strong--;
			if (ReferanceCount->Strong == 0)
			{
				delete Pointer;
				if (ReferanceCount->Weak == 0)
				{
					delete ReferanceCount;
				}
			}
		}

		Type& operator*()
		{
			zeAssert(Pointer == NULL, "ZEPointer does not points any data structure.");
			return *Pointer;
		}

		Type* operator->()
		{
			zeAssert(Pointer == NULL, "ZEPointer does not points any data structure.");
			return Pointer;
		}

		ZESharedPointer<Type>& operator=(Type* RawPointer)
		{
			Create(RawPointer);
			return *this;
		}

		ZESharedPointer<Type>& operator=(ZESharedPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
			return *this;
		}

		ZESharedPointer()
		{
			Pointer = NULL;
			ReferanceCount = NULL;
		}

		explicit ZESharedPointer(Type* RawPointer)
		{
			Create(RawPointer);
		}

		ZESharedPointer(const ZESharedPointer<Type>& OtherPointer)
		{
			Copy(OtherPointer);
		}

		~ZESharedPointer()
		{
			Release();
		}


		static void Swap(const ZESharedPointer<Type>& A, const ZESharedPointer<Type>& B)
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
