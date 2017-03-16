//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariantTest.cpp
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

#include "ZEMTTypeGenerator.h"
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
#include "ZEDS/ZEVariant.h"
#include "ZEMTType.h"
#include "ZEMTTestObject.h"
#include "ZEError.h"

#define ZEMT_TYPE_TEST_INVALID(Expression)

ZE_TEST(ZEVariant)
{
	ZE_TEST_ITEM("Initialization")
	{
		ZETestObject::Initialize();
	}

	#define ZEMT_TYPE_TEST_VALUE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue;\
			Variant.Set##FunctionPostfix(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
			ZE_TEST_CHECK_ERROR((BaseType)Variant.Get##FunctionPostfix() == InitialValue); \
			ZE_TEST_CHECK_ERROR((BaseType)Variant.Get##FunctionPostfix##Ref() == InitialValue); \
			ZE_TEST_CHECK_ERROR((BaseType)Variant.Get##FunctionPostfix##ConstRef() == InitialValue); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_REFERENCE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue; \
			Variant.Set##FunctionPostfix##Ref(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
			ZE_TEST_CHECK_ERROR(Variant.Get##FunctionPostfix() == InitialValue); \
			ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##Ref() == &InitialValue); \
			ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##ConstRef() == &InitialValue); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_CONST_REFERENCE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue; \
			Variant.Set##FunctionPostfix##ConstRef(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
				ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##ConstRef() == &InitialValue); \
				ZE_TEST_CHECK_ERROR(Variant.Get##FunctionPostfix() == InitialValue); \
			StartSuppressingLogging(); \
			StartExpectingError(ZE_ET_ERROR, "ZEVariant"); \
			Variant.Get##FunctionPostfix##Ref(); \
			ZE_TEST_CHECK_ERROR(DidExpectedErrorOccured(), "Invalid case get reference check failed."); \
			StopExpectingError(); \
			StopSuppressingLogging(); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_NO_IRREGULAR_TYPE
	#define ZEMT_TYPE_TEST_NO_COLLECTION_TYPE
	#include "ZEMTTypeTestTable.h"
	#undef ZEMT_TYPE_TEST_NO_IRREGULAR_TYPE
	#undef ZEMT_TYPE_TEST_NO_COLLECTION_TYPE
	#undef ZEMT_TYPE_TEST_VALUE
	#undef ZEMT_TYPE_TEST_REFERENCE
	#undef ZEMT_TYPE_TEST_CONST_REFERENCE

	ZE_TEST_ITEM("ZEVariant-ZEObject")
	{
		ZEVariant Variant;
		Variant.SetObject(ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());
		
		ZE_TEST_CHECK_ERROR(*(ZETestObject*)&Variant.GetObject() == ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(*(ZETestObject*)&Variant.GetObjectRef() == ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(*(const ZETestObject*)&Variant.GetObjectConstRef() == ZETestObject::Instance);
		
		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}

	ZE_TEST_ITEM("ZEVariant-ZEObject&")
	{
		ZEVariant Variant;
		Variant.SetObjectRef(ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
		
		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR(*(ZETestObject*)&Variant.GetObject() == ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(*(ZETestObject*)&Variant.GetObjectRef() == ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(*(const ZETestObject*)&Variant.GetObjectConstRef() == ZETestObject::Instance);
		
		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}

	ZE_TEST_ITEM("ZEVariant-const ZEObject&")
	{
		ZEVariant Variant;
		Variant.SetObjectConstRef(ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR(*(const ZETestObject*)&Variant.GetObjectConstRef() == ZETestObject::Instance);
		ZE_TEST_CHECK_ERROR(*(ZETestObject*)&Variant.GetObject() == ZETestObject::Instance);
		
		StartSuppressingLogging();
		StartExpectingError(ZE_ET_ERROR, "ZEVariant");
		Variant.GetObjectRef();
		ZE_TEST_CHECK_ERROR(DidExpectedErrorOccured(), "Invalid case get reference check failed.");
		StopExpectingError();
		StopSuppressingLogging();

		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}

	ZE_TEST_ITEM("ZEVariant-ZEHolder<ZEObject>")
	{
		ZETestObject* Instance = new ZETestObject();
		ZEHolder<ZETestObject> Holder = Instance;

		ZEVariant Variant;
		Variant.SetObjectHolder(Holder);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<ZETestObject>*>(&Variant.GetObjectHolder()))->GetPointer() == Instance);
		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<ZETestObject>*>(&Variant.GetObjectHolderRef()))->GetPointer() == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolder().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolder().GetObjectPtrConst()) == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtrConst()) == Instance);

		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}

	ZE_TEST_ITEM("ZEVariant-ZEHolder<const ZEObject>")
	{
		ZETestObject* Instance = new ZETestObject();
		ZEHolder<const ZETestObject> Holder = Instance;

		ZEVariant Variant;
		Variant.SetObjectHolder(Holder);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<const ZETestObject>*>(&Variant.GetObjectHolder()))->GetPointer() == Instance);
		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<const ZETestObject>*>(&Variant.GetObjectHolderRef()))->GetPointer() == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolder().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolder().GetObjectPtrConst()) == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtrConst()) == Instance);

		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}
	
	ZE_TEST_ITEM("ZEVariant-ZEHolder<ZEObject>&")
	{
		ZETestObject* Instance = new ZETestObject();
		ZEHolder<ZETestObject> Holder = Instance;

		ZEVariant Variant;
		Variant.SetObjectHolderRef(Holder);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<ZETestObject>*>(&Variant.GetObjectHolder()))->GetPointer() == Instance);
		ZE_TEST_CHECK_ERROR(&Variant.GetObjectHolderRef() == &Holder);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolder().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolder().GetObjectPtrConst()) == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtrConst()) == Instance);

		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}

	ZE_TEST_ITEM("ZEVariant-ZEHolder<const ZEObject>&")
	{
		ZETestObject* Instance = new ZETestObject();
		ZEHolder<const ZETestObject> Holder = Instance;

		ZEVariant Variant;
		Variant.SetObjectHolderRef(Holder);
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");

		ZEMTType Type = ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class());
		ZE_TEST_CHECK_ERROR(
			Variant.GetType() == Type,
			"Type mismatch.\n"
			" Result: {0}\n"
			" Expected: {1}\n",
			Variant.GetType().ToString(), Type.ToString());

		ZE_TEST_CHECK_ERROR((static_cast<ZEHolder<ZETestObject>*>(&Variant.GetObjectHolder()))->GetPointer() == Instance);
		ZE_TEST_CHECK_ERROR(&Variant.GetObjectHolderRef() == &Holder);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolder().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolder().GetObjectPtrConst()) == Instance);

		ZE_TEST_CHECK_ERROR(static_cast<ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtr()) == Instance);
		ZE_TEST_CHECK_ERROR(static_cast<const ZETestObject*>(Variant.GetObjectHolderRef().GetObjectPtrConst()) == Instance);

		Variant.SetUndefined();
		ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected.");
	}


	#define ZEMT_TYPE_TEST_VALUE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue;\
			Variant.Set##FunctionPostfix(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
				ZE_TEST_CHECK_ERROR(*(const BaseType*)&Variant.Get##FunctionPostfix() == InitialValue); \
				ZE_TEST_CHECK_ERROR(*(const BaseType*)&Variant.Get##FunctionPostfix##Ref() == InitialValue); \
				ZE_TEST_CHECK_ERROR(*(const BaseType*)&Variant.Get##FunctionPostfix##ConstRef() == InitialValue); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_REFERENCE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue; \
			Variant.Set##FunctionPostfix##Ref(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
			ZE_TEST_CHECK_ERROR(*(const BaseType*)&Variant.Get##FunctionPostfix() == InitialValue); \
			ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##Ref() == &InitialValue); \
			ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##ConstRef() == &InitialValue); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_CONST_REFERENCE(Declaration, BaseType, FunctionPostfix, TestValue, Type) \
		ZE_TEST_ITEM("ZEVariant-"#Declaration) \
		{ \
			ZEVariant Variant; \
			TestValue; \
			Variant.Set##FunctionPostfix##ConstRef(InitialValue); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
			ZE_TEST_CHECK_ERROR( \
				Variant.GetType() == Type, \
				"Type mismatch.\n" \
				" Result: {0}\n" \
				" Expected: {1}\n", \
				Variant.GetType().ToString(), Type.ToString()); \
				ZE_TEST_CHECK_ERROR(*(const BaseType*)&Variant.Get##FunctionPostfix() == InitialValue); \
				ZE_TEST_CHECK_ERROR(&Variant.Get##FunctionPostfix##ConstRef() == &InitialValue); \
			StartSuppressingLogging(); \
			StartExpectingError(ZE_ET_ERROR, "ZEVariant"); \
			Variant.Get##FunctionPostfix##Ref(); \
			ZE_TEST_CHECK_ERROR(DidExpectedErrorOccured(), "Invalid case get reference check failed."); \
			StopExpectingError(); \
			StopSuppressingLogging(); \
			Variant.SetUndefined(); \
			ZE_TEST_CHECK_ERROR(!ZE_MEMORY_CHECK_INTERNAL, "Memory corruption detected."); \
		}

	#define ZEMT_TYPE_TEST_NO_ORDINARY_TYPE
	#define ZEMT_TYPE_TEST_NO_IRREGULAR_TYPE
	#include "ZEMTTypeTestTable.h"
	#undef ZEMT_TYPE_TEST_NO_ORDINARY_TYPE
	#undef ZEMT_TYPE_TEST_NO_IRREGULAR_TYPE
	#undef ZEMT_TYPE_TEST_VALUE
	#undef ZEMT_TYPE_TEST_REFERENCE
	#undef ZEMT_TYPE_TEST_CONST_REFERENCE
}
