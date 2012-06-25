//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDelegate.cpp
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

#include "ZEDelegate.h"

template <typename Signature>
class ZDelegate;
/*
template <typename TReturn, typename TParam0>
class ZDelegate< TReturn ( TParam0 ) >
{
	public:
		void* Object;
		TReturn (*Function)(void* Object, TParam0);
	
		// TReturn (TClass::*TFunction)(TParam0)
		template <typename TClass, TReturn (TClass::*TFunction)(TParam0)>
		static TReturn Wrapper(void* Object, TParam0 Param0)
		{
			TClass* CastedObject = (TClass*)Object;
			return (CastedObject->*TFunction)(Param0);
		}

	public:
		TReturn Call(TParam0 Parameter)
		{
			Function(Object, Parameter);
		}

		// TReturn (TClass::*TFunction)(TParam0)
		template <class TClass, TReturn (TClass::*TFunction)(TParam0)>
		static ZDelegate Create(TClass* Object)
		{
			ZDelegate Delegate;
			Delegate.Object = Object;
			Delegate.Function = &Wrapper<TClass, TFunction>;
			return Delegate;
		}

};

class B
{
	public:
		int funct(int a)
		{
			return a;
		}

};

//template<class TClass, typename void(TClass::*TMethod)()>
template<typename TClass, typename TFunction>
void Deneme(TClass* Object, TFunction Function)
{
	(Object->*Function)(33);
}

int dd(int a)
{
	return a;
}

void delegate()
{
	B ll;
	//Deneme(&ll, &dd);
	ZDelegate<int (int)> A = ZDelegate<int (int)>::Create<B, &B::funct>(&ll);

	//ZDelegate<int (int)> a = ZDelegate<int (int)>::Create(&ll, &B::funct);

	//Deneme<B, &B::funct>(&ll);
	
	ZDelegete<int (int)> D = ZDelegete<int (int)>::Create(&ll, B::funct);

	D.Call(4);
}
*/
