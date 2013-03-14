//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLNodeTests.cpp
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

#include "ZEML/ZEMLNode.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEMLProperty.h"
#include "ZEMLDataProperty.h"
#include "ZEDS/ZEList.h"
#include "ZEFile/ZEFile.h"
#include "ZEMath/ZEAngle.h"


ZETestSuite(ZEMLNode)
{
	ZETest("ZEMLNode::~ZEMLNode()")
	{

	}

	ZETest("ZEMLNode::ZEMLNode()")
	{
		ZEMLNode Node;

		ZETestCheckEqual(Node.GetType(), ZEML_IT_NODE);
	}

	ZETest("ZEMLNode::ZEMLNode(const ZEString& Name)")
	{
		ZEMLNode Node("TestNode");

		ZETestCheckEqual(Node.GetType(), ZEML_IT_NODE);
		ZETestCheckString(Node.GetName(), "TestNode");
	}

	ZETest("ZEUInt64 ZEMLNode::GetTotalSize()")
	{
		ZEUInt64 TotalSize;

		ZETestCase("for ZEMLNode::ZEMLNode()")
		{
			ZEMLNode Node;

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 0);
		}

		ZETestCase("for ZEMLNode::ZEMLNode(const ZEString& Name)")
		{
			ZEMLNode Node("TestNode");

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 0);
		}

		ZETestCase("add Property then call function")
		{
			ZEMLNode Node;
			Node.AddProperty("PropFloat", 3.0f);

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 25);
 		}

		ZETestCase("add DataProperty then call function")
		{
			ZEMLNode Node;
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty");

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 24);
		}

		ZETestCase("add SubNode then call function")
		{
			ZEMLNode Node;
			ZEMLNode* SubNode = Node.AddSubNode("SubNode");

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 27);
 		}

		ZETestCase("add Property, DataProperty and SubNode then call function")
		{
			ZEMLNode Node;
			Node.AddProperty("PropFloat", 3.0f);
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty");
			ZEMLNode* SubNode = Node.AddSubNode("SubNode");

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 76);
		}
	}

	ZETest("bool ZEMLNode::AddProperty(ZEMLProperty* Property)")
	{
		ZEMLProperty* Property = new ZEMLProperty();
		ZEMLNode Node;

		bool Result = Node.AddProperty(Property);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		ZEMLProperty* Property2 = new ZEMLProperty("Property2");

		Result = Node.AddProperty(Property2);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckString(Node.GetProperties()[1]->GetName(), "Property2");

		ZEVariant Value;
		Value.SetFloat(1.3f);
		ZEMLProperty* Property3 = new ZEMLProperty("Property3", Value);

		Result = Node.AddProperty(Property3);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
		ZETestCheckString(Node.GetProperties()[2]->GetName(), "Property3");

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value.SetBoolean(true);
			ZEMLProperty* Property4 = new ZEMLProperty("Property3", Value);

			Result = Node.AddProperty(Property4);
			//error ZEML node can not contain properties with duplicate name : Property3.
			ZETestCheck(!Result);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVariant& Value)")
	{
		ZEVariant Value;
		Value.SetBoolean(false);
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("TestProperty", Value);
		ZETestCheckString(Property->GetName(), "TestProperty");
		ZETestCheck(!Property->GetValue().GetBoolean());
		ZETestCheckString(Node.GetName(), "ParentNode");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value.SetInt8('Z');
			Property = Node.AddProperty("TestProperty", Value);
			//error ZEML node can not contain properties with duplicate name : TestProperty.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
			ZEMLProperty* Prop = (ZEMLProperty*)Node.GetProperties()[0];
			ZETestCheckEqual(Prop->GetType(), ZEML_IT_BOOLEAN);
			ZETestCheck(!Prop->GetValue().GetBoolean());
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("TestProperty");
		ZETestCheckString(Property->GetName(), "TestProperty");
		ZETestCheckString(Node.GetName(), "ParentNode");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Property = Node.AddProperty("TestProperty2");
		ZETestCheckString(Property->GetName(), "TestProperty2");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("TestProperty");
			//error ZEML node can not contain properties with duplicate name : TestProperty.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty()")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty();
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Property = Node.AddProperty();
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, float Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropFloat", 1.3f);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_FLOAT);
		ZETestCheckEqual(Property->GetValue().GetFloat(), 1.3f);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropFloat", 5.0f);
			//error ZEML node can not contain properties with duplicate name : PropFloat.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
			Property = (ZEMLProperty*)Node.GetProperties()[0];
			ZETestCheckEqual(Property->GetType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Property->GetValue().GetFloat(), 1.3f);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, double Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropDouble", 5.0);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_DOUBLE);
		ZETestCheckClose(Property->GetValue().GetDouble(), 5.0);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropDouble", 3.0);
			//error ZEML node can not contain properties with duplicate name : PropDouble.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			Property = Node.AddProperty("Double", 3.0);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
			Property = (ZEMLProperty*)Node.GetProperties()[0];
			ZETestCheckClose(Property->GetValue().GetDouble(), 5.0);
			Property = (ZEMLProperty*)Node.GetProperties()[1];
			ZETestCheckClose(Property->GetValue().GetDouble(), 3.0);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt8 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt8 Value = 'z';

		ZEMLProperty* Property = Node.AddProperty("PropInt8", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT8);
		ZETestCheckEqual(Property->GetValue().GetInt8(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 'e';
			Property = Node.AddProperty("PropInt8", Value);
			//error ZEML node can not contain properties with duplicate name : PropInt8.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt16 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt16 Value = 30;

		ZEMLProperty* Property = Node.AddProperty("PropInt16", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT16);
		ZETestCheckEqual(Property->GetValue().GetInt16(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 53;
			Property = Node.AddProperty("PropInt16", Value);
			//error ZEML node can not contain properties with duplicate name : PropInt16.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt32 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt32 Value = 3200;

		ZEMLProperty* Property = Node.AddProperty("PropInt32", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT32);
		ZETestCheckEqual(Property->GetValue().GetInt32(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 3590;
			Property = Node.AddProperty("PropInt32", Value);
			//error ZEML node can not contain properties with duplicate name : PropInt32.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt64 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt64 Value = 3000000;

		ZEMLProperty* Property = Node.AddProperty("PropInt64", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT64);
		ZETestCheckEqual(Property->GetValue().GetInt64(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 0;
			Property = Node.AddProperty("PropInt64", Value);
			//error ZEML node can not contain properties with duplicate name : PropInt64
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt8 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt8 Value = 'Z';

		ZEMLProperty* Property = Node.AddProperty("PropUInt8", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT8);
		ZETestCheckEqual(Property->GetValue().GetUInt8(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 'E';
			Property = Node.AddProperty("PropUInt8", Value);
			//error ZEML node can not contain properties with duplicate name : PropUInt8
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt16 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt16 Value = 1600;

		ZEMLProperty* Property = Node.AddProperty("PropUInt16", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT16);
		ZETestCheckEqual(Property->GetValue().GetUInt16(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 5638;
			Property = Node.AddProperty("PropUInt16", Value);
			//error ZEML node can not contain properties with duplicate name : PropUInt16.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt32 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt32 Value = 58000;

		ZEMLProperty* Property = Node.AddProperty("PropUInt32", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT32);
		ZETestCheckEqual(Property->GetValue().GetUInt32(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 37360;
			Property = Node.AddProperty("PropUInt32", Value);
			//error ZEML node can not contain properties with duplicate name : PropUInt32.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt64 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt64 Value = 90000000;

		ZEMLProperty* Property = Node.AddProperty("PropUInt64", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT64);
		ZETestCheckEqual(Property->GetValue().GetUInt64(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = 332494034;
			Property = Node.AddProperty("PropUInt64", Value);
			//error ZEML node can not contain properties with duplicate name : PropUInt64.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, bool Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropBool", true);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_BOOLEAN);
		ZETestCheck(Property->GetValue().GetBoolean());

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropBool", false);
			//error ZEML node can not contain properties with duplicate name : PropBool.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEString& Value)")
	{
		ZEMLNode Node("ParentNode");
		const ZEString Value = "TestString";

		ZEMLProperty* Property = Node.AddProperty("PropString", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_STRING);
		ZETestCheckString(Property->GetValue().GetString(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			const ZEString Value1 = "String";
			Property = Node.AddProperty("PropString", Value1);
			//error ZEML node can not contain properties with duplicate name : PropString.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const char* Value)")
	{
		ZEMLNode Node("ParentNode");
		const char* Value = "TestString";

		ZEMLProperty* Property = Node.AddProperty("PropChar", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_STRING);
		ZETestCheckString(Property->GetValue().GetString(), Value);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Value = "String";
			Property = Node.AddProperty("PropChar", Value);
			//error ZEML node can not contain properties with duplicate name : PropChar.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEQuaternion& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropQuat", ZEQuaternion::Identity);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_QUATERNION);
		ZETestCheckEqual(Property->GetValue().GetQuaternion(), ZEQuaternion::Identity);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropQuat", ZEQuaternion(ZE_PI_2, ZEVector3::UnitY));
			//error ZEML node can not contain properties with duplicate name : PropQuat.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector2& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropVec2", ZEVector2::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR2);
		ZETestCheckEqual(Property->GetValue().GetVector2(), ZEVector2::Zero);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropVec2", ZEVector2::One);
			//error ZEML node can not contain properties with duplicate name : PropVec2.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector3& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropVec3", ZEVector3::One);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR3);
		ZETestCheckEqual(Property->GetValue().GetVector3(), ZEVector3::One);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropVec3", ZEVector3::Zero);
			//error ZEML node can not contain properties with duplicate name : PropVec3.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector4& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropVec4", ZEVector4::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR4);
		ZETestCheckEqual(Property->GetValue().GetVector4(), ZEVector4::Zero);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropVec4", ZEVector4::One);
			//error ZEML node can not contain properties with duplicate name : PropVec4.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix3x3& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropMatrix3x3", ZEMatrix3x3::Identity);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_MATRIX3X3);
		ZETestCheckEqual(Property->GetValue().GetMatrix3x3(), ZEMatrix3x3::Identity);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropMatrix3x3", ZEMatrix3x3::Zero);
			//error ZEML node can not contain properties with duplicate name : PropMatrix3x3.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix4x4& Value)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("PropMatrix4x4", ZEMatrix4x4::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Property->GetType(), ZEML_IT_MATRIX4X4);
		ZETestCheckEqual(Property->GetValue().GetMatrix4x4(), ZEMatrix4x4::Zero);

		ZETestCase("try to add a new property with an already existing property name")
		{
			Property = Node.AddProperty("PropMatrix4x4", ZEMatrix4x4::Identity);
			//error ZEML node can not contain properties with duplicate name : PropMatrix4x4.
			ZETestCheck(Property == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("bool ZEMLNode::AddDataProperty(ZEMLDataProperty* Property)")
	{
		ZEMLNode Node("ParentNode");
		ZEMLDataProperty* DataProperty = new ZEMLDataProperty();

		bool Result = Node.AddDataProperty(DataProperty);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Node.GetProperties()[0]->GetType(), ZEML_IT_INLINE_DATA);

		ZEMLDataProperty* DataProperty2 = new ZEMLDataProperty("DataProperty2");

		Result = Node.AddDataProperty(DataProperty2);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckEqual(Node.GetProperties()[1]->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(Node.GetProperties()[1]->GetName(), "DataProperty2");

		void* Data = new char[sizeof(unsigned char)];
		ZEMLDataProperty* DataProperty3 = new ZEMLDataProperty("DataProperty3", Data, sizeof(unsigned char), false);

		Result = Node.AddDataProperty(DataProperty3);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
		ZETestCheckEqual(Node.GetProperties()[2]->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(Node.GetProperties()[2]->GetName(), "DataProperty3");

		ZETestCase("try to add a new data property with an already existing data property name")
		{
			ZEMLProperty* Property = Node.AddProperty("Property");
			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);

			ZEMLDataProperty* DataProperty4 = new ZEMLDataProperty("Property");

			Result = Node.AddDataProperty(DataProperty4);
			//error ZEML node can not contain data properties with duplicate name : Property.
			ZETestCheck(!Result);

			ZEMLDataProperty* DataProperty5 = new ZEMLDataProperty("DataProperty2", Data, sizeof(unsigned char), false);
			 
			Result = Node.AddDataProperty(DataProperty5);
			//error ZEML node can not contain data properties with duplicate name : DataProperty2.
			ZETestCheck(!Result);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);
		}
		delete Data;
	}

	ZETest("ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)")
	{
		ZEMLNode Node("ParentNode");
		void* Data = new char[sizeof(unsigned char)];

		ZETestCase("for Cache false")
		{
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty", Data, sizeof(unsigned char), false);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
			ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(DataProperty->GetData(), Data);
			ZEMLDataProperty* Property = (ZEMLDataProperty*)Node.GetProperties()[0];
			ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Property->GetData(), Data);
		}

		ZETestCase("for Cache true")
		{
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty2", Data, sizeof(unsigned char), true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
			ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheck(DataProperty->GetData() != Data);
			ZEMLDataProperty* Property = (ZEMLDataProperty*)Node.GetProperties()[1];
			ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheck(Property->GetData() != Data);
			ZETestCheck(DataProperty->GetData() == Property->GetData());
		}

		ZETestCase("try to add a new data property with an already existing data property name")
		{
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty2", Data, sizeof(unsigned char), false);
			//error ZEML node can not contain data properties with duplicate name : DataProperty2.
			ZETestCheck(DataProperty == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		}
		delete Data;
	}

	ZETest("ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(DataProperty->GetData() == NULL);

		ZETestCase("try to add a new data property with an already existing property name")
		{
			DataProperty = Node.AddDataProperty("DataProperty");
			//error ZEML node can not contain data properties with duplicate name : DataProperty.
			ZETestCheck(DataProperty == NULL);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		}
	}

	ZETest("ZEMLDataProperty* ZEMLNode::AddDataProperty()")
	{
		ZEMLNode Node("ParentNode");

		ZEMLDataProperty* DataProperty = Node.AddDataProperty();
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(DataProperty->GetData() == NULL);
	}

	ZETest("void ZEMLNode::AddSubNode(ZEMLNode* Node)")
	{
		ZEMLNode Node("ParentNode");
 		ZEMLNode* SubNode1 = new ZEMLNode();

		Node.AddSubNode(SubNode1);
 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
	}

	ZETest("ZEMLNode* ZEMLNode::AddSubNode(const ZEString& Name)")
	{
		ZEMLNode Node("ParentNode");
		
		ZEMLNode* SubNode1 = Node.AddSubNode("SubNode1");
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
		ZETestCheckString(Node.GetName(), "ParentNode");
		ZETestCheckString(SubNode1->GetName(), "SubNode1");

		ZEMLNode* SubNode2 = Node.AddSubNode("SubNode2");
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
		ZETestCheckString(Node.GetName(), "ParentNode");
 		ZETestCheckString(SubNode2->GetName(), "SubNode2");
	}

	ZETest("ZEMLNode* ZEMLNode::AddSubNode()")
	{
		ZEMLNode Node("ParentNode");

		ZEMLNode* SubNode = Node.AddSubNode();
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);

		SubNode = Node.AddSubNode();
 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
	}

	ZETest("bool ZEMLNode::InsertSubNode(ZEMLNode* Node, ZESize Index)")
	{
		ZEMLNode Node("ParentNode");
		ZEMLNode SubNode1("SubNode1");
		ZEMLNode SubNode2("SubNode2");
		
		bool Result = Node.InsertSubNode(&SubNode1, 0);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
		ZETestCheckString(Node.GetSubNodes()[0]->GetName(), "SubNode1");

		Result = Node.InsertSubNode(&SubNode2, 0);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
		ZETestCheckString(Node.GetSubNodes()[0]->GetName(), "SubNode2");
		ZETestCheckString(Node.GetSubNodes()[1]->GetName(), "SubNode1");

		ZETestCase("try to add a sub node with an invalid index value")
		{
			ZEMLNode SubNode3("SubNode3");

			Result = Node.InsertSubNode(&SubNode3, 3);
			//error Can not insert sub node index is out of range. Node name : SubNode3, index : 3
			ZETestCheck(!Result);
			ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
		}

		ZETestCase("try to add a null node as sub node")
		{

		}
	}

	ZETest("ZEMLNode* ZEMLNode::InsertSubNode(const ZEString & Name, ZESize Index)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLNode* SubNode = Node.InsertSubNode("SubNode1", 0);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
		ZETestCheckString(Node.GetSubNodes()[0]->GetName(), SubNode->GetName());
		ZETestCheckEqual(SubNode->GetParent(), &Node);

		SubNode = Node.InsertSubNode("SubNode2", 1);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
		ZETestCheckString(Node.GetSubNodes()[1]->GetName(), SubNode->GetName());
		ZETestCheckEqual(SubNode->GetParent(), &Node);
	}


	ZETest("const ZEList<ZEMLNode>& ZEMLNode::GetSubNodes() const")
	{
		ZEMLNode Node("ParentNode");

		ZEMLNode* SubNode1 = Node.AddSubNode("SubNode1");
		ZEMLNode* SubNode2 = Node.InsertSubNode("SubNode2", 1);
		ZEMLNode* SubNode3 = Node.AddSubNode("SubNode3");

		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 3);
		ZETestCheckString(Node.GetSubNodes()[0]->GetName(), "SubNode1");
		ZETestCheckString(Node.GetSubNodes()[1]->GetName(), "SubNode2");
		ZETestCheckString(Node.GetSubNodes()[2]->GetName(), "SubNode3");
	}

	ZETest("const ZEArray<ZEMLNode*> ZEMLNode::GetSubNodes(const ZEString& NodeName)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLNode* SubNode = Node.AddSubNode("SubNode1");
		ZEArray<ZEMLNode*> SubNodes = Node.GetSubNodes("SubNode1");
 		ZETestCheckEqual(SubNodes.GetCount(), 1);
		ZETestCheckString(SubNodes[0]->GetName(), "SubNode1");
		SubNodes = Node.GetSubNodes("ParentNode");
		ZETestCheckEqual(SubNodes.GetCount(), 0);

		SubNode = Node.AddSubNode("SubNode2");
		SubNodes = Node.GetSubNodes(Node.GetName());
 		ZETestCheckEqual(SubNodes.GetCount(), 0);
		SubNodes = Node.GetSubNodes("SubNode2");
		ZETestCheckEqual(SubNodes.GetCount(), 1);
		ZETestCheckString(SubNodes[0]->GetName(), "SubNode2");

		ZETestCase("insert more than 1 sub nodes with the same name")
		{
			Node.AddSubNode("SubNode");
			Node.InsertSubNode("SubNode", 0);

			SubNodes = Node.GetSubNodes("SubNode");
			ZETestCheckEqual(SubNodes.GetCount(), 2);
		}
	}

	ZETest("const ZEArray<ZEMLItem*>& ZEMLNode::GetProperties() const")
	{
		ZEMLNode Node("ParentNode");
		ZEVariant Value;
		Value.SetBoolean(true);

		ZEMLProperty* Property = Node.AddProperty("Prop1", Value);
		ZEArray<ZEMLItem*> Properties = Node.GetProperties();
		ZETestCheckEqual(Properties.GetCount(), 1);
		ZETestCheckString(Properties[0]->GetName(), "Prop1");

		Value.SetString("Test");
		Property = Node.AddProperty("Prop2", Value);
		Properties = Node.GetProperties();
		ZETestCheckEqual(Properties.GetCount(), 2);
		ZETestCheckString(Properties[1]->GetName(), "Prop2");

		void* Data = new char[sizeof(unsigned char)];
		ZEMLDataProperty* DataProperty = Node.AddDataProperty("Prop3", Data, sizeof(unsigned char), false);
		Properties = Node.GetProperties();
		ZETestCheckEqual(Properties.GetCount(), 3);
		ZETestCheckString(Properties[2]->GetName(), "Prop3");

		ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheck(Item1->GetValue().GetBoolean());
		ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
		ZETestCheckString(Item2->GetValue().GetString(), "Test");
		ZEMLDataProperty* Item3 = (ZEMLDataProperty*)Node.GetProperties()[2];
		ZETestCheck(Item3->GetData() == Data);

		delete Data;
	}

	ZETest("const ZEMLItem* ZEMLNode::GetProperty(const ZEString& PropertyName)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Prop1 = Node.AddProperty("Prop1", true);
		ZEMLProperty* Prop2 = Node.AddProperty("Prop2", 3.0f);

		void* Data = new char[sizeof(unsigned char)];
		ZEMLDataProperty* DataProp1 = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
		ZEMLDataProperty* DataProp2 = Node.AddDataProperty("DataProp2");

		const ZEMLItem* Property = Node.GetProperty("DataProp2");
		ZETestCheckString(Property->GetName(), "DataProp2");
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);

		Property = Node.GetProperty("DataProp1");
		ZETestCheckString(Property->GetName(), "DataProp1");
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);

		Property = Node.GetProperty("Prop2");
		ZETestCheckString(Property->GetName(), "Prop2");
		ZETestCheckEqual(Property->GetType(), ZEML_IT_FLOAT);

		Property = Node.GetProperty("Prop1");
		ZETestCheckString(Property->GetName(), "Prop1");
		ZETestCheckEqual(Property->GetType(), ZEML_IT_BOOLEAN);

		Property = Node.GetProperty("Prop");
		ZETestCheck(Property ==  NULL);

		delete Data;
	}

	ZETest("const ZEMLNode* ZEMLNode::GetParent()")
	{
		ZEMLNode Node("Node");
		ZEMLNode* SubNode1 = Node.AddSubNode("SubNode01");
		ZEMLNode* SubNode2 = SubNode1->AddSubNode("SubNode001");
		ZEMLNode* SubNode3 = SubNode2->AddSubNode("SubNode0001");

		const ZEMLNode* Parent = Node.GetParent();
		ZETestCheck(Parent == NULL);

		Parent = SubNode1->GetParent();
		ZETestCheckString(Parent->GetName(), "Node");

		Parent = SubNode2->GetParent();
		ZETestCheckString(Parent->GetName(), "SubNode01");

		Parent = SubNode3->GetParent();
		ZETestCheckString(Parent->GetName(), "SubNode001");
	}

	ZETest("bool ZEMLNode::WriteSelf(ZEFile* File)")
	{

	}

	ZETest("bool ZEMLNode::Write(ZEFile* File)")
	{
		ZEFile* File = new ZEFile();		
		ZEVariant Value;
		ZEMLProperty* Prop;
		ZEMLDataProperty* DataProp;
		bool ResultForWrite;
		
		ZETestCase("node includes Property")
		{
			ZEMLNode Node("ParentNode");
			Value.SetBoolean(true);
			Prop = Node.AddProperty("Prop1", Value);
			Value.SetString("Test");
			Prop = Node.AddProperty("Prop2", Value);
			Value.SetInt32(32);
			Prop = Node.AddProperty("Prop3", Value);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("node includes DataProperty created by ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache) and Property")
		{
			ZEMLNode Node("ParentNode");
			Value.SetBoolean(true);
			Prop = Node.AddProperty("Prop1", Value);
			Value.SetString("TestString");
			Prop = Node.AddProperty("Prop2", Value);
			void* Data = new char[sizeof(unsigned char)];
			DataProp = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);

			File->Close();
			remove("NodeTests.txt");

			delete Data;
		}

		ZETestCase("node includes Property, DataProperty and SubNodes")
		{
			ZEMLNode* SubNode;
			ZEMLNode Node("ParentNode");
			Value.SetBoolean(true);
			Prop = Node.AddProperty("Prop1", Value);
			Value.SetString("String");
			Prop = Node.AddProperty("Prop2", Value);
			void* Data = new char[sizeof(unsigned char)];
			DataProp = Node.AddDataProperty("DataProp", Data, sizeof(unsigned char), false);
			SubNode = Node.AddSubNode("SubNode1");
			SubNode = Node.AddSubNode("SubNode2");

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);

			File->Close();
			remove("NodeTests.txt");

			delete Data;
		}

		ZETestCase("DataProperty with size 0") 
		{
// 			ZEMLNode Node("ParentNode");
// 			bool Added = Node.AddDataProperty(new ZEMLDataProperty("DataProp1"));
// 			//DataProp = Node.AddDataProperty("DataProp2"); Locked in void ZELog::Log(const char* Module, ZELogType Type, const char* Format, ...)
// 
// 			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
// 
// 			ResultForWrite = Node.Write(File);
// 			ZETestCheck(ResultForWrite);
// 			//warning ZEMLDataProperty DataProp1 data size is : 0
// 			//warning ZEMLDataProperty DataProp2 data size is : 0
// 
// 			File->Close();
// 			remove("NodeTests.txt");
		}
	}

	ZETest("bool ZEMLNode::ReadSelf(ZEFile* File, bool DeferredDataReading)")
	{

	}

	ZETest("bool ZEMLNode::Read(ZEFile* File, bool DeferredDataReading)")
	{
		ZEFile* File = new ZEFile();
		ZEVariant Value;
		ZEMLProperty* Prop;
		ZEMLDataProperty* DataProp;		
		bool ResultForWrite;
		bool ResultForRead;

		ZETestCase("add property and data property, then call Read for DeferredDataReading false")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			
			Prop = Node.AddProperty("Prop1", true);
			Prop = Node.AddProperty("Prop2", "Test");
			DataProp = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
			DataProp = Node.AddDataProperty("DataProp2", Data, sizeof(unsigned char), true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);
			ZETestCheckEqual(Node.GetTotalSize(), 84);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, false);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 8);
			ZETestCheckEqual(Node.GetTotalSize(), 168);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			bool Value1 = Item1->GetValue().GetBoolean();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZEMLDataProperty* Item3 = (ZEMLDataProperty*)Node.GetProperties()[2];
			const void* Value3 = Item3->GetData();
			ZEMLDataProperty* Item4 = (ZEMLDataProperty*)Node.GetProperties()[3];
			const void* Value4 = Item4->GetData();
			ZEMLProperty* Item5 = (ZEMLProperty*)Node.GetProperties()[4];
			bool Value5 = Item5->GetValue().GetBoolean();
			ZEMLProperty* Item6 = (ZEMLProperty*)Node.GetProperties()[5];
			const ZEString Value6 = Item6->GetValue().GetString();
			ZEMLDataProperty* Item7 = (ZEMLDataProperty*)Node.GetProperties()[6];
			const void* Value7 = Item7->GetData();
			ZEMLDataProperty* Item8 = (ZEMLDataProperty*)Node.GetProperties()[7];
			const void* Value8 = Item8->GetData();
			ZETestCheck(Value1 == Value5);
			ZETestCheck(Value2 == Value6);
			ZETestCheck(Value3 != Value7);
			ZETestCheck(Value3 == Data);
			ZETestCheck(Value4 != Value8);
			ZETestCheck(Value4 != Data);
			ZEInt Compare = memcmp(Value3, Value7, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);
			Compare = memcmp(Value4, Value8, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);
			Compare = memcmp(Data, Value4, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("add property and data property, then call Read for DeferredDataReading true")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			Value.SetBoolean(true);
			Node.AddProperty("Prop1", Value);
			Value.SetString("Test");
			Node.AddProperty("Prop2", Value);
			DataProp = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
			DataProp = Node.AddDataProperty("DataProp2", Data, sizeof(unsigned char), true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);
			ZETestCheckEqual(Node.GetTotalSize(), 84);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 8);
			ZETestCheckEqual(Node.GetTotalSize(), 168);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			bool Value1 = Item1->GetValue().GetBoolean();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZEMLDataProperty* Item3 = (ZEMLDataProperty*)Node.GetProperties()[2];
			const void* Value3 = Item3->GetData();
			ZEMLDataProperty* Item4 = (ZEMLDataProperty*)Node.GetProperties()[3];
			const void* Value4 = Item4->GetData();
			ZEMLProperty* Item5 = (ZEMLProperty*)Node.GetProperties()[4];
			bool Value5 = Item5->GetValue().GetBoolean();
			ZEMLProperty* Item6 = (ZEMLProperty*)Node.GetProperties()[5];
			const ZEString Value6 = Item6->GetValue().GetString();
			ZEMLDataProperty* Item7 = (ZEMLDataProperty*)Node.GetProperties()[6];
			const void* Value7 = Item7->GetData();
			ZEMLDataProperty* Item8 = (ZEMLDataProperty*)Node.GetProperties()[7];
			const void* Value8 = Item8->GetData();
			ZETestCheck(Value1 == Value5);
			ZETestCheck(Value2 == Value6);
			ZETestCheck(Value3 != Value7);
			ZETestCheck(Value3 == Data);
			ZETestCheck(Value7 == NULL);
			ZETestCheck(Value4 != Value8);
			ZETestCheck(Value4 != Data);
			ZETestCheck(Value8 == NULL);
			ZEInt Compare = memcmp(Data, Value4, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("add property with type ZEML_IT_STRING by setting variant to ZEString")
		{
			ZEMLNode Node("ParentNode");
			ZEString String = "TestString";
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheckString(Value1, Value2);
			ZETestCheck(Value1 == String);
			
			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with type ZEML_IT_STRING by setting variant to const char*")
		{
			ZEMLNode Node("ParentNode");
			const char* String = "TestString";
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheckString(Value1, Value2);
			ZETestCheck(Value1 == String);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with ZEString """)
		{
			ZEMLNode Node("ParentNode");
			ZEString String = "";
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheck(Value1 == Value2);
			ZETestCheck(Value1 == String);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with ZEString which is not initialized")
		{
			ZEMLNode Node("ParentNode");
			ZEString String;
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheck(Value1 == Value2);
			ZETestCheck(Value1 == String);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with setting variant type as ZE_VRT_STRING")
		{
			ZEMLNode Node("ParentNode");
			ZEVariant Variant;
			Variant.SetType(ZE_VRT_STRING);
			Node.AddProperty("PropString", Variant);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheck(Value1 == Value2);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with const char* """)
		{
			ZEMLNode Node("ParentNode");
			const char* String = "";
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheck(Value1 == Value2);
			ZETestCheck(Value1 == String);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("for dynamically created ZEString")
		{
			ZEMLNode Node("ParentNode");
			ZEString* String = new ZEString();
			Value.SetString(*String);
			Node.AddProperty("PropString", Value);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			const ZEString Value1 = Item1->GetValue().GetString();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			const ZEString Value2 = Item2->GetValue().GetString();
			ZETestCheck(Value1 == Value2);
			ZETestCheck(Value1 == *String);

			File->Close();
			remove("NodeTests.txt");
			delete String;
		}

		ZETestCase("add property with float")
		{
			ZEMLNode Node("ParentNode");
			float Float = 1.3f;
			Node.AddProperty("PropFloat", Float);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			float Value1 = Item1->GetValue().GetFloat();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			float Value2 = Item2->GetValue().GetFloat();
			ZETestCheckEqual(Value1, Value2);
			ZETestCheckEqual(Value1, Float);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add property with double")
		{
			ZEMLNode Node("ParentNode");
			double Double = 1.3;
			Node.AddProperty("PropDouble", Double);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			double Value1 = Item1->GetValue().GetDouble();
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			double Value2 = Item2->GetValue().GetDouble();
			ZETestCheckClose(Value1, Value2);
			ZETestCheckClose(Value1, Double);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add properties with ZEInt8, ZEInt16, ZEInt32, ZEInt64, ZEUInt8, ZEUInt16, ZEUInt32, ZEUInt64 respectively")
		{
			ZEMLNode Node("ParentNode");
			ZEInt8 Int8 = 0;
			Node.AddProperty("PropInt8", Int8);
			ZEInt16 Int16 = 0;
			Node.AddProperty("PropInt16", Int16);
			ZEInt32 Int32 = 0;
			Node.AddProperty("PropInt32", Int32);
			ZEInt64 Int64 = 0;
			Node.AddProperty("PropInt64", Int64);
			ZEUInt8 UInt8 = 0;
			Node.AddProperty("PropUInt8", UInt8);
			ZEUInt16 UInt16 = 0;
			Node.AddProperty("PropUInt16", UInt16);
			ZEUInt32 UInt32 = 0;
			Node.AddProperty("PropUInt32", UInt32);
			ZEUInt64 UInt64 = 0;
			Node.AddProperty("PropUInt64", UInt64);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 8);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			ZEInt8 V1 = Item1->GetValue().GetInt8();
			ZETestCheckEqual(Item1->GetValue().GetInt8(), 0);
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			ZETestCheckEqual(Item2->GetValue().GetInt16(), 0);
			ZEMLProperty* Item3 = (ZEMLProperty*)Node.GetProperties()[2];
			ZETestCheckEqual(Item3->GetValue().GetInt32(), 0);
			ZEMLProperty* Item4 = (ZEMLProperty*)Node.GetProperties()[3];
			ZETestCheckEqual(Item4->GetValue().GetInt64(), 0);
			ZEMLProperty* Item5 = (ZEMLProperty*)Node.GetProperties()[4];
			ZETestCheckEqual(Item5->GetValue().GetUInt8(), 0);
			ZEMLProperty* Item6 = (ZEMLProperty*)Node.GetProperties()[5];
			ZETestCheckEqual(Item6->GetValue().GetUInt16(), 0);
			ZEMLProperty* Item7 = (ZEMLProperty*)Node.GetProperties()[6];
			ZETestCheckEqual(Item7->GetValue().GetUInt32(), 0);
			ZEMLProperty* Item8 = (ZEMLProperty*)Node.GetProperties()[7];
			ZETestCheckEqual(Item8->GetValue().GetUInt64(), 0);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 16);

			File->Seek(-File->Tell() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ResultForWrite = Node.Write(File);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add properties with ZEQuaternion, ZEVector2, ZEVector3, ZEVector4, ZEMatrix3x3, ZEMatrix4x4 respectively")
		{
			ZEMLNode Node("ParentNode");
			ZEQuaternion Quat(ZEQuaternion::Identity);
			Node.AddProperty("PropQuat", Quat);
			ZEVector2 Vec2(ZEVector2::One);
			Node.AddProperty("PropVec2", Vec2);
			ZEVector3 Vec3(ZEVector3::UnitX);
			Node.AddProperty("PropVec3", Vec3);
			ZEVector4 Vec4(ZEVector4::One);
			Node.AddProperty("PropVec4", Vec4);
			ZEMatrix3x3 Mat3x3(ZEMatrix3x3::Identity);
			Node.AddProperty("PropMat3x3", Mat3x3);
			ZEMatrix4x4 Mat4x4(ZEMatrix4x4::Zero);
			Node.AddProperty("PropMat4x4", Mat4x4);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 6);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 12);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			ZEMLProperty* Item3 = (ZEMLProperty*)Node.GetProperties()[2];
			ZEMLProperty* Item4 = (ZEMLProperty*)Node.GetProperties()[3];
			ZEMLProperty* Item5 = (ZEMLProperty*)Node.GetProperties()[4];
			ZEMLProperty* Item6 = (ZEMLProperty*)Node.GetProperties()[5];
			ZEMLProperty* Item7 = (ZEMLProperty*)Node.GetProperties()[6];
			ZEMLProperty* Item8 = (ZEMLProperty*)Node.GetProperties()[7];			
			ZEMLProperty* Item9 = (ZEMLProperty*)Node.GetProperties()[8];
			ZEMLProperty* Item10 = (ZEMLProperty*)Node.GetProperties()[9];
			ZEMLProperty* Item11 = (ZEMLProperty*)Node.GetProperties()[10];
			ZEMLProperty* Item12 = (ZEMLProperty*)Node.GetProperties()[11];
			ZETestCheckEqual(Item7->GetValue().GetQuaternion(), Item1->GetValue().GetQuaternion());
			ZETestCheckEqual(Item8->GetValue().GetVector2(), Item2->GetValue().GetVector2());
			ZETestCheckEqual(Item9->GetValue().GetVector3(), Item3->GetValue().GetVector3());
			ZETestCheckEqual(Item10->GetValue().GetVector4(), Item4->GetValue().GetVector4());
			ZETestCheckEqual(Item11->GetValue().GetMatrix3x3(), Item5->GetValue().GetMatrix3x3());
			ZETestCheckEqual(Item12->GetValue().GetMatrix4x4(), Item6->GetValue().GetMatrix4x4());

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("add data property with cache true, and call Read with deferred data reading true")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProp", Data, sizeof(unsigned char), true);
			ZETestCheck(DataProperty->GetData() != Data);
			Node.AddDataProperty(DataProperty);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);
			ZETestCheck(Data2 == NULL);
			ZEInt Compare = memcmp(Data, Data1, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("add data property with cache false, and call Read with deferred data reading false")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProp", Data, sizeof(unsigned char), false);
			ZETestCheck(DataProperty->GetData() == Data);
			Node.AddDataProperty(DataProperty);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, false);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);
			ZEInt Compare = memcmp(Data, Data1, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);
			Compare = memcmp(Data2, Data1, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Seek(-File->Tell() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("add data property with cache true, and call Read with deferred data reading false")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProp", Data, sizeof(unsigned char), true);
			ZETestCheck(DataProperty->GetData() != Data);
			Node.AddDataProperty(DataProperty);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, false);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
			
			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);

			File->Seek(-File->Tell() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("add data property with cache false, and call Read with deferred data reading true")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProp", Data, sizeof(unsigned char), false);
			ZETestCheck(DataProperty->GetData() == Data);
			Node.AddDataProperty(DataProperty);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, true);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
	
			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);
			ZETestCheck(Data2 == NULL);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}

		ZETestCase("for node includes SubNode and DeferredDataReading false")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			ZEMLNode* SubNode;
			Node.AddProperty("Prop1", true);
			Node.AddProperty("Prop2", "String");
			DataProp = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
			DataProp = Node.AddDataProperty("DataProp2", Data, sizeof(unsigned char), true);
			SubNode = Node.AddSubNode("SubNode1");
			SubNode = Node.AddSubNode("SubNode2");
			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);
			ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
			ZETestCheckEqual(Node.GetTotalSize(), 142);

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ResultForWrite = Node.Write(File);
			ZETestCheck(ResultForWrite);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Node.Read(File, false);
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 8);
			ZETestCheckEqual(Node.GetSubNodes().GetCount(), 4);
			ZETestCheckEqual(Node.GetTotalSize(), 284);

			File->Close();
			remove("NodeTests.txt");
			delete Data;
		}
	}

	ZETest("bool ZEMLNode::AddItem(ZEMLItem* Item)")
	{
		ZEMLNode Node("Node");

		ZEMLProperty* Property = new ZEMLProperty("Property", "String");
		ZEMLNode* SubNode = new ZEMLNode("SubNode");
		ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProperty");

		bool Result = Node.AddItem(Property);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Result = Node.AddItem(SubNode);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);

		Result = Node.AddItem(DataProperty);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

		Result = Node.AddItem(Property);
		//error ZEML node can not contain properties with duplicate name : Property.
		ZETestCheck(!Result);

		ZEMLItem* Item = new ZEMLProperty("Prop");
		Result = Node.AddItem(Item);
		ZETestCheck(Result);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
	}

	ZETest("bool ZEMLNode::RemoveItem(ZEMLItem* Item)")
	{
		ZEMLNode Node("Node");
		bool Added = Node.AddItem(new ZEMLNode("SubNode"));
		ZETestCheck(Added);
		Added = Node.AddItem(new ZEMLProperty("Prop", true));
		ZETestCheck(Added);
		ZEMLDataProperty* DataProperty = new ZEMLDataProperty("DataProp");
		Added = Node.AddItem(DataProperty);
		ZETestCheck(Added);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);

		bool Removed = Node.RemoveItem(DataProperty);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Removed = Node.RemoveItem(Node.GetProperties()[0]);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 0);

		Removed = Node.RemoveItem((ZEMLItem*)Node.GetSubNodes()[0]);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 0);

		ZEMLItem* Item = new ZEMLNode("Node1");
		Removed = Node.RemoveItem(Item);
		ZETestCheck(!Removed);
	}
}
