//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTTypeGeneratorDynamicTests.cpp
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

#include "ZETest/ZETest.h"

#include "ZEMTTypeGeneratorDynamic.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEClass.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEPointer/ZEReferenceCounted.h"
#include "ZEDS/ZEFormat.h"
#include "ZEMTProvider.h"
#include "ZEMTTestObject.h"

class ZEDummyNonObject;
class ZEMTTestObject;

#define ZEMT_TYPE_TEST(Declaration, BaseType, FunctionPostfix, InitialValue, Type) \
	ZE_TEST_ITEM(ZEFormat::Format("ZEMT_TYPE_GENERATOR_DYNAMIC({0})", #Declaration)) \
	{ \
		ZEMTType GeneratedType = ZEMT_TYPE_GENERATOR_DYNAMIC(Declaration); \
		ZE_TEST_CHECK_ERROR(Type == GeneratedType, "Generated type is wrong. Generated Type: \"{0}\".", GeneratedType.ToString()); \
	}

#define ZEMT_TYPE_TEST_INVALID(Declaration) \
	ZE_TEST_ITEM(ZEFormat::Format("Invalid cases of ZEMT_TYPE_GENERATOR_DYNAMIC({0})", #Declaration)) \
	{ \
		ZEMTType GeneratedType = ZEMT_TYPE_GENERATOR_DYNAMIC(Declaration); \
		ZE_TEST_CHECK_ERROR(!GeneratedType.IsValid(), "Type is not valid but ZEMTTypeGenerator generated a valid type. Generated Type: \"{0}\".", GeneratedType.ToString()); \
	}

ZE_TEST(ZEMTTypeGenerator)
{
	ZE_TEST_ITEM("Initialization")
	{
		ZEMTProvider::GetInstance()->RegisterClass(ZETestObject::Class());
	}

	#include "ZEMTTypeTestTable.h"
}
