//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEClassOverloads.cpp
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

#include "ZEClass.h"

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue)
{
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, NULL, 0);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0)
{
	const ZEReference* ParameterArray[1] = { &Parameter0 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 1);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1)
{
	const ZEReference* ParameterArray[2] =	{ &Parameter0, &Parameter1 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 2);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2)
{
	const ZEReference* ParameterArray[3] = { &Parameter0, &Parameter1, &Parameter2 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 3);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3)
{
	const ZEReference* ParameterArray[4] =	{ &Parameter0, &Parameter1,	&Parameter2, &Parameter3 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 4);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4)
{
	const ZEReference* ParameterArray[5] =	{ &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4	};
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 5);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5)
{
	const ZEReference* ParameterArray[6] =	{ &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 6);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6)
{
	const ZEReference* ParameterArray[7] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 7);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7)
{
	const ZEReference* ParameterArray[8] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 8);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8)
{
	const ZEReference* ParameterArray[9] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 9);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9)
{
	const ZEReference* ParameterArray[10] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 10);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10)
{
	const ZEReference* ParameterArray[11] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 11);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11)
{
	const ZEReference* ParameterArray[12] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 12);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12)
{
	const ZEReference* ParameterArray[13] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 13);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13)
{
	const ZEReference* ParameterArray[14] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 14);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14)
{
	const ZEReference* ParameterArray[15] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 15);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15)
{
	const ZEReference* ParameterArray[16] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 16);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16)
{
	const ZEReference* ParameterArray[17] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 17);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17)
{
	const ZEReference* ParameterArray[18] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 18);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18)
{
	const ZEReference* ParameterArray[19] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 19);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19)
{
	const ZEReference* ParameterArray[20] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 20);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20)
{
	const ZEReference* ParameterArray[21] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 21);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21)
{
	const ZEReference* ParameterArray[22] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 22);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22)
{
	const ZEReference* ParameterArray[23] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 23);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22, const ZEReference& Parameter23)
{
	const ZEReference* ParameterArray[24] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22, &Parameter23};
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 24);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22, const ZEReference& Parameter23, const ZEReference& Parameter24)
{
	const ZEReference* ParameterArray[25] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22, &Parameter23, &Parameter24 };
	return ZEClass::CallMethod(Object, MethodId, ReturnValue, ParameterArray, 25);
}

//////////////////////////////////////////////////////////////////////////

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue)
{
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, NULL, 0);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0)
{
	const ZEReference* ParameterArray[1] = { &Parameter0 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 1);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1)
{
	const ZEReference* ParameterArray[2] =	{ &Parameter0, &Parameter1 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 2);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2)
{
	const ZEReference* ParameterArray[3] = { &Parameter0, &Parameter1, &Parameter2 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 3);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3)
{
	const ZEReference* ParameterArray[4] =	{ &Parameter0, &Parameter1,	&Parameter2, &Parameter3 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 4);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4)
{
	const ZEReference* ParameterArray[5] =	{ &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4	};
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 5);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5)
{
	const ZEReference* ParameterArray[6] =	{ &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 6);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6)
{
	const ZEReference* ParameterArray[7] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 7);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7)
{
	const ZEReference* ParameterArray[8] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 8);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8)
{
	const ZEReference* ParameterArray[9] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 9);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9)
{
	const ZEReference* ParameterArray[10] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 10);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10)
{
	const ZEReference* ParameterArray[11] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 11);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11)
{
	const ZEReference* ParameterArray[12] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 12);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12)
{
	const ZEReference* ParameterArray[13] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 13);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13)
{
	const ZEReference* ParameterArray[14] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 14);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14)
{
	const ZEReference* ParameterArray[15] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 15);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15)
{
	const ZEReference* ParameterArray[16] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 16);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16)
{
	const ZEReference* ParameterArray[17] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 17);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17)
{
	const ZEReference* ParameterArray[18] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 18);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18)
{
	const ZEReference* ParameterArray[19] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 19);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19)
{
	const ZEReference* ParameterArray[20] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 20);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20)
{
	const ZEReference* ParameterArray[21] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 21);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21)
{
	const ZEReference* ParameterArray[22] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 22);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22)
{
	const ZEReference* ParameterArray[23] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 23);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22, const ZEReference& Parameter23)
{
	const ZEReference* ParameterArray[24] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22, &Parameter23};
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 24);
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference& Parameter0, const ZEReference& Parameter1, const ZEReference& Parameter2, const ZEReference& Parameter3, const ZEReference& Parameter4, const ZEReference& Parameter5, const ZEReference& Parameter6, const ZEReference& Parameter7, const ZEReference& Parameter8, const ZEReference& Parameter9, const ZEReference& Parameter10, const ZEReference& Parameter11, const ZEReference& Parameter12, const ZEReference& Parameter13, const ZEReference& Parameter14, const ZEReference& Parameter15, const ZEReference& Parameter16, const ZEReference& Parameter17, const ZEReference& Parameter18, const ZEReference& Parameter19, const ZEReference& Parameter20, const ZEReference& Parameter21, const ZEReference& Parameter22, const ZEReference& Parameter23, const ZEReference& Parameter24)
{
	const ZEReference* ParameterArray[25] = { &Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4, &Parameter5, &Parameter6, &Parameter7, &Parameter8, &Parameter9, &Parameter10, &Parameter11, &Parameter12, &Parameter13, &Parameter14, &Parameter15, &Parameter16, &Parameter17, &Parameter18, &Parameter19, &Parameter20, &Parameter21, &Parameter22, &Parameter23, &Parameter24 };
	return ZEClass::CallMethod(Object, MethodName, ReturnValue, ParameterArray, 25);
}

