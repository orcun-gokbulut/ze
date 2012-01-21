//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDelegateTests.cpp
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

#include "ZETest.h"
#include "ZEDelegate.h"

#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

void CascadeStack()
{
	char Buffer[1024];

	for (ZEInt I = 0; I < 1024; I++)
		Buffer[I] = rand() % 0xFFFFFFFF;
}

bool CallCheck(ZEInt Arg0)
{
	bool Result = (Arg0 == 0x1234ABCD);
	return Result;
}

const char* GlobalFunction(ZEInt Arg0)
{
	return (CallCheck(Arg0) ? __FUNCTION__ : "");
}

class ZETestBase1
{
	public:
				const char*		Function					(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
				const char*		ConstFunction				(ZEInt Arg0) const	{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		BaseVirtualFunction			(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		VirtualFunction				(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		VirtualConstFunction		(ZEInt Arg0) const	{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		static	const char*		StaticFunction				(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
};

class ZETestBase2 : public ZETestBase1
{
	public:
		virtual	const char*		AbstractVirtualFunction		(ZEInt Arg0) = 0;
		virtual	const char*		AbstractVirtualConstFunction(ZEInt Arg0) const = 0;
};

class ZETestBase3 : public ZETestBase2
{
	public:
		const char*				Function					(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		const char*				ConstFunction				(ZEInt Arg0) const	{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		VirtualFunction				(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		VirtualConstFunction		(ZEInt Arg0) const	{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		AbstractVirtualFunction		(ZEInt Arg0)			{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
		virtual	const char*		AbstractVirtualConstFunction(ZEInt Arg0) const	{return (CallCheck(Arg0) ? __FUNCTION__ : "");}
};

void VoidSimpleFunction0() { }
void VoidSimpleFunction1(ZEInt) { }

const char* SimpleFunction0() { return __FUNCTION__;}
const char* SimpleFunction1(ZEInt) { return __FUNCTION__;}
const char* SimpleFunction2(ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction3(ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction4(ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction5(ZEInt, ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction6(ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction7(ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction8(ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}
const char* SimpleFunction10(ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt, ZEInt) { return __FUNCTION__;}

#define SYNTAX const char* (ZEInt)

ZETestSuite(ZEDelegate)
{
	ZETest("NULLDelegate")
	{
		ZEDelegate<SYNTAX> Delegate;
		ZETestCheck(Delegate.IsNull());
		ZETestCheck(Delegate == false);
		ZETestCheck(!Delegate == true);
	}

	ZETest("IsNull")
	{
		ZEDelegate<SYNTAX> Delegate = ZEDelegate<SYNTAX>::Create<&GlobalFunction>();
		ZETestCheck(!Delegate.IsNull());
		ZETestCheck(Delegate == true);
		ZETestCheck(!Delegate == false);	
	}

	ZETest("Clean")
	{
		ZEDelegate<SYNTAX> Delegate = ZEDelegate<SYNTAX>::Create<&GlobalFunction>();
		Delegate.Clear();
		ZETestCheck(Delegate.IsNull());
		ZETestCheck(Delegate == false);
		ZETestCheck(!Delegate == true);
	}

	ZETest("GlobalFunctions")
	{
		ZEDelegate<SYNTAX> Delegate = ZEDelegate<SYNTAX>::Create<&GlobalFunction>();
		ZETestCheckString(Delegate(0x1234ABCD), "GlobalFunction");
	}

	ZETest("NonInheritedClassFunctions")
	{
		ZETestBase1 Test;

		ZEDelegate<SYNTAX> Delegate;
		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase1, &ZETestBase1::Function>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::Function");

		Delegate = ZEDelegate<SYNTAX>::CreateConst<ZETestBase1, &ZETestBase1::ConstFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::ConstFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase1, &ZETestBase1::VirtualFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::VirtualFunction");

		Delegate = ZEDelegate<SYNTAX>::CreateConst<ZETestBase1, &ZETestBase1::VirtualConstFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::VirtualConstFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase1::StaticFunction>();
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::StaticFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase1, &ZETestBase1::BaseVirtualFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::BaseVirtualFunction");
	}

	ZETest("InheritedClassFunctions")
	{
		ZETestBase3 Test;

		ZEDelegate<SYNTAX> Delegate;
		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase3, &ZETestBase3::Function>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::Function");

		Delegate = ZEDelegate<SYNTAX>::CreateConst<ZETestBase3, &ZETestBase3::ConstFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::ConstFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase3, &ZETestBase3::VirtualFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::VirtualFunction");

		Delegate = ZEDelegate<SYNTAX>::CreateConst<ZETestBase3, &ZETestBase3::VirtualConstFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::VirtualConstFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase3, &ZETestBase3::AbstractVirtualFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::AbstractVirtualFunction");

		Delegate = ZEDelegate<SYNTAX>::CreateConst<ZETestBase3, &ZETestBase3::AbstractVirtualConstFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase3::AbstractVirtualConstFunction");

		Delegate = ZEDelegate<SYNTAX>::Create<ZETestBase1, &ZETestBase3::BaseVirtualFunction>(&Test);
		ZETestCheckString(Delegate(0x1234ABCD), "ZETestBase1::BaseVirtualFunction");
	}

	ZETest("Versions")
	{
		ZEDelegate<void ()> VoidDelegate0 = ZEDelegate<void ()>::Create<&VoidSimpleFunction0>();
		VoidDelegate0();

		ZEDelegate<void (ZEInt)> VoidDelegate1 = ZEDelegate<void (ZEInt)>::Create<&VoidSimpleFunction1>();
		VoidDelegate1(1);
		
		ZEDelegate<const char* ()> Delegate0 = ZEDelegate<const char* ()>::Create<&SimpleFunction0>();		
		ZETestCheckString(Delegate0(), "SimpleFunction0");
		ZEDelegate<const char* (ZEInt)> Delegate1 = ZEDelegate<const char* (ZEInt)>::Create<&SimpleFunction1>();
		ZETestCheckString(Delegate1(0), "SimpleFunction1");
		ZEDelegate<const char* (ZEInt, ZEInt)> Delegate2 = ZEDelegate<const char* (ZEInt, ZEInt)>::Create<&SimpleFunction2>();
		ZETestCheckString(Delegate2(0, 1), "SimpleFunction2");
		ZEDelegate<const char* (ZEInt, ZEInt, ZEInt)> Delegate3 = ZEDelegate<const char* (ZEInt, ZEInt, ZEInt)>::Create<&SimpleFunction3>();
		ZETestCheckString(Delegate3(0, 1, 2), "SimpleFunction3");
		ZEDelegate<const char* (ZEInt, ZEInt, ZEInt, ZEInt)> Delegate4 = ZEDelegate<const char* (ZEInt, ZEInt, ZEInt, ZEInt)>::Create<&SimpleFunction4>();
		ZETestCheckString(Delegate4(0, 1, 2, 3), "SimpleFunction4");
		ZEDelegate<const char* (ZEInt, ZEInt, ZEInt, ZEInt, ZEInt)> Delegate5 = ZEDelegate<const char* (ZEInt, ZEInt, ZEInt, ZEInt, ZEInt)>::Create<&SimpleFunction5>();
		ZETestCheckString(Delegate5(0, 1, 2, 3, 4), "SimpleFunction5");
	}
}

#define DELEGATE(CLASS, FUNCTION)
