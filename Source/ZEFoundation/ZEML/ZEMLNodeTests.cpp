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
// 			ZEMLNode Node;
// 			ZEMLNode* SubNode = Node.AddSubNode("SubNode");
// 
// 			TotalSize = Node.GetTotalSize();
// 			ZETestCheckEqual(TotalSize, 27);
 		}

		ZETestCase("add Property, DataProperty and SubNode then call function")
		{
			ZEMLNode Node;
			Node.AddProperty("PropFloat", 3.0f);
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty");
//			ZEMLNode* SubNode = Node.AddSubNode("SubNode");//total 76

			TotalSize = Node.GetTotalSize();
			ZETestCheckEqual(TotalSize, 49);
		}
	}

	ZETest("void ZEMLNode::AddProperty(ZEMLProperty* Property)")
	{
		ZEMLProperty* Property = new ZEMLProperty();
		ZEMLNode Node;

		Node.AddProperty(Property);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		ZEMLProperty* Property2 = new ZEMLProperty("Property2");

		Node.AddProperty(Property2);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckString(Node.GetProperties()[1]->GetName(), "Property2");

		ZEVariant Value;
		Value.SetFloat(1.3f);
		ZEMLProperty* Property3 = new ZEMLProperty("Property3", Value);

		Node.AddProperty(Property3);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
		ZETestCheckString(Node.GetProperties()[2]->GetName(), "Property3");
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
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty("TestProperty");
		ZETestCheckString(Property->GetName(), "TestProperty");
		ZETestCheckString(Node.GetName(), "ParentNode");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
	}

	ZETest("ZEMLProperty* ZEMLNode::AddProperty()")
	{
		ZEMLNode Node("ParentNode");

		ZEMLProperty* Property = Node.AddProperty();
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
	}

	ZETest("bool ZEMLNode::RemoveProperty(ZEMLProperty* Property)")
	{
		ZEMLNode Node("ParentNode");
		ZEMLProperty* Property1 = new ZEMLProperty();
		Property1->SetName("Property1");
		ZEMLProperty* Property2 = new ZEMLProperty();
		Property2->SetName("Property2");
		ZEMLProperty* Property3 = new ZEMLProperty();
		Property3->SetName("Property3");

		Node.AddProperty(Property1);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Node.AddProperty(Property2);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);

		bool Removed = Node.RemoveProperty(Property1);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Removed = Node.RemoveProperty(Property2);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 0);

		Removed = Node.RemoveProperty(Property3);
		ZETestCheck(!Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 0);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, float Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropFloat", 1.3f);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_FLOAT);
		ZETestCheckEqual(Property->GetValue().GetFloat(), 1.3f);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, double Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropDouble", 5.0);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_DOUBLE);
		ZETestCheckClose(Property->GetValue().GetDouble(), 5.0);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEInt8 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt8 Value = 'z';

		Node.AddProperty("PropInt8", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT8);
		ZETestCheckEqual(Property->GetValue().GetInt8(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEInt16 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt16 Value = 30;

		Node.AddProperty("PropInt16", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT16);
		ZETestCheckEqual(Property->GetValue().GetInt16(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEInt32 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt32 Value = 3200;

		Node.AddProperty("PropInt32", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT32);
		ZETestCheckEqual(Property->GetValue().GetInt32(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEInt64 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEInt64 Value = 3000000;

		Node.AddProperty("PropInt64", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INT64);
		ZETestCheckEqual(Property->GetValue().GetInt64(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt8 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt8 Value = 'Z';

		Node.AddProperty("PropUInt8", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT8);
		ZETestCheckEqual(Property->GetValue().GetUInt8(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt16 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt16 Value = 1600;

		Node.AddProperty("PropUInt16", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT16);
		ZETestCheckEqual(Property->GetValue().GetUInt16(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt32 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt32 Value = 58000;

		Node.AddProperty("PropUInt32", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT32);
		ZETestCheckEqual(Property->GetValue().GetUInt32(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt64 Value)")
	{
		ZEMLNode Node("ParentNode");
		ZEUInt64 Value = 90000000;

		Node.AddProperty("PropUInt64", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_UINT64);
		ZETestCheckEqual(Property->GetValue().GetUInt64(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, bool Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropBool", true);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_BOOLEAN);
		ZETestCheck(Property->GetValue().GetBoolean());
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEString& Value)")
	{
		ZEMLNode Node("ParentNode");
		const ZEString Value = "TestString";

		Node.AddProperty("PropString", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_STRING);
		ZETestCheckString(Property->GetValue().GetString(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const char* Value)")
	{
		ZEMLNode Node("ParentNode");
		const char* Value = "TestString";

		Node.AddProperty("PropChar", Value);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_STRING);
		ZETestCheckString(Property->GetValue().GetString(), Value);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEQuaternion& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropQuat", ZEQuaternion::Identity);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_QUATERNION);
		ZETestCheckEqual(Property->GetValue().GetQuaternion(), ZEQuaternion::Identity);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector2& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropVec2", ZEVector2::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR2);
		ZETestCheckEqual(Property->GetValue().GetVector2(), ZEVector2::Zero);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector3& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropVec3", ZEVector3::One);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR3);
		ZETestCheckEqual(Property->GetValue().GetVector3(), ZEVector3::One);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector4& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropVec4", ZEVector4::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_VECTOR4);
		ZETestCheckEqual(Property->GetValue().GetVector4(), ZEVector4::Zero);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix3x3& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropMatrix3x3", ZEMatrix3x3::Identity);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_MATRIX3X3);
		ZETestCheckEqual(Property->GetValue().GetMatrix3x3(), ZEMatrix3x3::Identity);
	}

	ZETest("void ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix4x4& Value)")
	{
		ZEMLNode Node("ParentNode");

		Node.AddProperty("PropMatrix4x4", ZEMatrix4x4::Zero);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZEMLProperty* Property = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_MATRIX4X4);
		ZETestCheckEqual(Property->GetValue().GetMatrix4x4(), ZEMatrix4x4::Zero);
	}

	ZETest("void ZEMLNode::AddDataProperty(ZEMLDataProperty* Property)")
	{
		ZEMLNode Node("ParentNode");
		ZEMLDataProperty* DataProperty = new ZEMLDataProperty();

		Node.AddDataProperty(DataProperty);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(Node.GetProperties()[0]->GetType(), ZEML_IT_INLINE_DATA);

		ZEMLDataProperty* DataProperty2 = new ZEMLDataProperty("DataProperty2");

		Node.AddDataProperty(DataProperty2);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckEqual(Node.GetProperties()[1]->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(Node.GetProperties()[1]->GetName(), "DataProperty2");

		void* Data = new char[sizeof(unsigned char)];
		ZEMLDataProperty* DataProperty3 = new ZEMLDataProperty("DataProperty3", Data, sizeof(unsigned char), false);

		Node.AddDataProperty(DataProperty3);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 3);
		ZETestCheckEqual(Node.GetProperties()[2]->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(Node.GetProperties()[2]->GetName(), "DataProperty3");
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
			ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty", Data, sizeof(unsigned char), true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
			ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheck(DataProperty->GetData() != Data);
			ZEMLDataProperty* Property = (ZEMLDataProperty*)Node.GetProperties()[1];
			ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheck(Property->GetData() != Data);
			ZETestCheck(DataProperty->GetData() == Property->GetData());
		}
	}

	ZETest("ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLDataProperty* DataProperty = Node.AddDataProperty("DataProperty");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(DataProperty->GetData() == NULL);
		ZEMLDataProperty* Property = (ZEMLDataProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(Property->GetData() == NULL);
	}

	ZETest("ZEMLDataProperty* ZEMLNode::AddDataProperty()")
	{
		ZEMLNode Node("ParentNode");

		ZEMLDataProperty* DataProperty = Node.AddDataProperty();
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckEqual(DataProperty->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(DataProperty->GetData() == NULL);
		ZEMLDataProperty* Property = (ZEMLDataProperty*)Node.GetProperties()[0];
		ZETestCheckEqual(Property->GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(Property->GetData() == NULL);
	}

	ZETest("bool ZEMLNode::RemoveDataProperty(ZEMLDataProperty* Property)")
	{
		ZEMLNode Node("ParentNode");

		ZEMLDataProperty* DataProperty1 = Node.AddDataProperty("DataProperty1");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);
		ZETestCheckString(DataProperty1->GetName(), "DataProperty1");

		ZEMLDataProperty* DataProperty2 = Node.AddDataProperty("DataProperty2");
		ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
		ZETestCheckString(DataProperty2->GetName(), "DataProperty2");

		bool Removed = Node.RemoveDataProperty(DataProperty1);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 1);

		Removed = Node.RemoveDataProperty(DataProperty2);
		ZETestCheck(Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 0);

		Removed = Node.RemoveDataProperty(DataProperty1);
		ZETestCheck(!Removed);
		ZETestCheckEqual(Node.GetProperties().GetCount(), 0);
	}

	ZETest("void ZEMLNode::AddSubNode(ZEMLNode* Node)")
	{
// 		ZEMLNode Node("ParentNode");
//  		ZEMLNode* SubNode1 = new ZEMLNode();
// 
// 		Node.AddSubNode(SubNode1);
//  		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
	}

	ZETest("ZEMLNode* ZEMLNode::AddSubNode(const ZEString& Name)")
	{
// 		ZEMLNode Node("ParentNode");
// 		
// 		ZEMLNode* SubNode1 = Node.AddSubNode("SubNode1");
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
// 		ZETestCheckString(Node.GetName(), "ParentNode");
// 		ZETestCheckString(SubNode1->GetName(), "SubNode1");
// 
// 		ZEMLNode* SubNode2 = Node.AddSubNode("SubNode2");
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
// 		ZETestCheckString(Node.GetName(), "ParentNode");
//  		ZETestCheckString(SubNode2->GetName(), "SubNode2");
	}

	ZETest("ZEMLNode* ZEMLNode::AddSubNode()")
	{
// 		ZEMLNode Node("ParentNode");
// 
// 		ZEMLNode* SubNode = Node.AddSubNode();
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
// 
// 		SubNode = Node.AddSubNode();
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
	}

	ZETest("bool ZEMLNode::RemoveSubNode(ZEMLNode* SubNode)")
	{
// 		ZEMLNode Node("ParentNode");
// 
// 		ZEMLNode* SubNode1 = Node.AddSubNode("SubNode1");
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
// 
// 		ZEMLNode* SubNode2 = Node.AddSubNode("SubNode2");
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
// 
// 		bool Removed = Node.RemoveSubNode(SubNode1);
// 		ZETestCheck(Removed);
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 1);
// 
// 		Removed = Node.RemoveSubNode(SubNode2);
// 		ZETestCheck(Removed);
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 0);
// 
// 		Removed = Node.RemoveSubNode(SubNode1);
// 		ZETestCheck(!Removed);
// 		ZETestCheckEqual(Node.GetSubNodes().GetCount(), 0);
	}

	ZETest("const ZEList<ZEMLNode>& ZEMLNode::GetSubNodes() const")
	{
// 		ZEMLNode Node("ParentNode");
// 
// 		ZEMLNode* SubNode = Node.AddSubNode("SubNode1");
		//ZEList<ZEMLNode> SubNodes = Node.GetSubNodes();
	}

	ZETest("const ZEArray<ZEMLNode*> ZEMLNode::GetSubNodes(const ZEString& NodeName)")
	{
// 		ZEMLNode Node("ParentNode");
// 
// 		ZEMLNode* SubNode = Node.AddSubNode("SubNode1");
// 		ZEArray<ZEMLNode*> SubNodes = Node.GetSubNodes("SubNode1");
//  		ZETestCheckEqual(SubNodes.GetCount(), 1);
// 		ZETestCheckString(SubNodes[0]->GetName(), "SubNode1");
// 		SubNodes = Node.GetSubNodes("ParentNode");
// 		ZETestCheckEqual(SubNodes.GetCount(), 0);
// 
// 		SubNode = Node.AddSubNode("SubNode2");
// 		SubNodes = Node.GetSubNodes(Node.GetName());
//  		ZETestCheckEqual(SubNodes.GetCount(), 0);
// 		SubNodes = Node.GetSubNodes("SubNode2");
// 		ZETestCheckEqual(SubNodes.GetCount(), 1);
// 		ZETestCheckString(SubNodes[0]->GetName(), "SubNode2");
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
	}

	ZETest("const ZEArray<ZEMLItem*> ZEMLNode::GetProperties(const ZEString& PropertyName)")
	{
		ZEMLNode Node("ParentNode");
		ZEVariant Value;
		Value.SetBoolean(true);

		ZEMLProperty* Property = Node.AddProperty("Prop1", Value);
		ZEArray<ZEMLItem*> Properties = Node.GetProperties("ParentNode");
		ZETestCheckEqual(Properties.GetCount(), 0);
		Properties = Node.GetProperties("Prop1");
		ZETestCheckEqual(Properties.GetCount(), 1);
		ZETestCheckString(Properties[0]->GetName(), "Prop1");

		Value.SetString("Test");
		Property = Node.AddProperty("Prop2", Value);
		Properties = Node.GetProperties("ParentNode");
		ZETestCheckEqual(Properties.GetCount(), 0);
		Properties = Node.GetProperties("Prop2");
		ZETestCheckEqual(Properties.GetCount(), 1);
		ZETestCheckString(Properties[0]->GetName(), "Prop2");

		ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
		ZETestCheck(Item1->GetValue().GetBoolean());
		ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
		ZETestCheckString(Item2->GetValue().GetString(), "Test");
	}

	ZETest("void ZEMLNode::WriteToFile(ZEFile* File)")
	{

	}

	ZETest("void ZEMLNode::Write(ZEFile* File)")
	{
		ZEFile* File = new ZEFile();		
		ZEVariant Value;
		ZEMLProperty* Prop;
		ZEMLDataProperty* DataProp;
		
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

			Node.Write(File);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("node includes Proprety, DataProperty")
		{
			ZEMLNode Node("ParentNode");
			Value.SetBoolean(false);
			Prop = Node.AddProperty("Prop1", Value);
			Value.SetString("String");
			Prop = Node.AddProperty("Prop2", Value);
			DataProp = Node.AddDataProperty("DataProp1");
			DataProp = Node.AddDataProperty("DataProp2");

			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			Node.Write(File);
			//error Cannot write ZEMLDataProperty data to file.
			//error Cannot write ZEMLDataProperty data to file.

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

			Node.Write(File);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("node includes Property, DataProperty and SubNodes")
		{
//			ZEMLNode* SubNode;
// 			ZEMLNode Node("ParentNode");
// 			Value.SetBoolean(true);
// 			Prop = Node.AddProperty("Prop1", Value);
// 			Value.SetString("String");
// 			Prop = Node.AddProperty("Prop2", Value);
// 			DataProp = Node.AddDataProperty("DataProp1");
// 			void* Data = new char[sizeof(unsigned char)];
// 			DataProp = Node.AddDataProperty("DataProp2", Data, sizeof(unsigned char), false);
// 			SubNode = Node.AddSubNode("SubNode1");
// 			SubNode = Node.AddSubNode("SubNode2");
// 			ZETestCheckEqual(Node.GetTotalSize(), 141);
// 
// 			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
// 
// 			Node.Write(File);
// 			//error Cannot write ZEMLDataProperty data to file.
// 
// 			File->Close();
// 			remove("NodeTests.txt");
		}
	}

	ZETest("void ZEMLNode::ReadFromFile(ZEFile* File, bool DeferredDataReading)")
	{

	}

	ZETest("void ZEMLNode::Read(ZEFile* File, bool DeferredDataReading)")
	{
		ZEFile* File = new ZEFile();
		ZEVariant Value;
		ZEMLDataProperty* DataProp;		
		
		ZETestCase("add property and data property, then call Read for DeferredDataReading false")
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, false);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
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

			Node.Read(File, true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 16);

			File->Seek(-File->Tell() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			Node.Write(File);

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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLProperty* Item1 = (ZEMLProperty*)Node.GetProperties()[0];
			ZETestCheckEqual(Item1->GetValue().GetQuaternion(), Quat);
			ZEMLProperty* Item2 = (ZEMLProperty*)Node.GetProperties()[1];
			ZETestCheckEqual(Item2->GetValue().GetVector2(), Vec2);
			ZEMLProperty* Item3 = (ZEMLProperty*)Node.GetProperties()[2];
			ZETestCheckEqual(Item3->GetValue().GetVector3(), Vec3);
			ZEMLProperty* Item4 = (ZEMLProperty*)Node.GetProperties()[3];
			ZETestCheckEqual(Item4->GetValue().GetVector4(), Vec4);
			ZEMLProperty* Item5 = (ZEMLProperty*)Node.GetProperties()[4];
			ZETestCheckEqual(Item5->GetValue().GetMatrix3x3(), Mat3x3);
			ZEMLProperty* Item6 = (ZEMLProperty*)Node.GetProperties()[5];
			ZETestCheckEqual(Item6->GetValue().GetMatrix4x4(), Mat4x4);

			Node.Read(File, true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 12);

			ZEMLProperty* Item7 = (ZEMLProperty*)Node.GetProperties()[6];
			ZETestCheckEqual(Item7->GetValue().GetQuaternion(), Item1->GetValue().GetQuaternion());
			ZEMLProperty* Item8 = (ZEMLProperty*)Node.GetProperties()[7];
			ZETestCheckEqual(Item8->GetValue().GetVector2(), Item2->GetValue().GetVector2());
			ZEMLProperty* Item9 = (ZEMLProperty*)Node.GetProperties()[8];
			ZETestCheckEqual(Item9->GetValue().GetVector3(), Item3->GetValue().GetVector3());
			ZEMLProperty* Item10 = (ZEMLProperty*)Node.GetProperties()[9];
			ZETestCheckEqual(Item10->GetValue().GetVector4(), Item4->GetValue().GetVector4());
			ZEMLProperty* Item11 = (ZEMLProperty*)Node.GetProperties()[10];
			ZETestCheckEqual(Item11->GetValue().GetMatrix3x3(), Item5->GetValue().GetMatrix3x3());
			ZEMLProperty* Item12 = (ZEMLProperty*)Node.GetProperties()[11];
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, false);
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
			Node.Write(File);

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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, false);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
			
			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);

			File->Seek(-File->Tell() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			Node.Write(File);

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
			Node.Write(File);
			File->Flush();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Node.Read(File, true);
			ZETestCheckEqual(Node.GetProperties().GetCount(), 2);
	
			ZEMLDataProperty* Item1 = (ZEMLDataProperty*)Node.GetProperties()[0];
			const void* Data1 = Item1->GetData();
			ZEMLDataProperty* Item2 = (ZEMLDataProperty*)Node.GetProperties()[1];
			const void* Data2 = Item2->GetData();
			ZETestCheck(Data1 != Data2);
			ZETestCheck(Data2 == NULL);

			File->Close();
			remove("NodeTests.txt");
		}

		ZETestCase("for node includes SubNode and DeferredDataReading false")
		{
// 			ZEMLNode Node("ParentNode");
// 			ZEMLNode* SubNode;
// 			Value.SetBoolean(true);
// 			Node.AddProperty("Prop1", Value);
// 			Value.SetString("String");
// 			Node.AddProperty("Prop2", Value);
// 			DataProp = Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
// 			DataProp = Node.AddDataProperty("DataProp2", Data, sizeof(unsigned char), true);
// 			SubNode = Node.AddSubNode("SubNode1");
// 			SubNode = Node.AddSubNode("SubNode2");
// 			ZETestCheckEqual(Node.GetProperties().GetCount(), 4);
// 			ZETestCheckEqual(Node.GetSubNodes().GetCount(), 2);
// 			ZETestCheckEqual(Node.GetTotalSize(), 142);
// 
// 			File->Open("NodeTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
// 			Node.Write(File);
// 			File->Flush();
// 			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
// 
// 			Node.Read(File, false);
// 			ZETestCheckEqual(Node.GetProperties().GetCount(), 8);
// 			ZETestCheckEqual(Node.GetSubNodes().GetCount(), 4);
// 			ZETestCheckEqual(Node.GetTotalSize(), 284);
// 
// 			File->Close();
// 			remove("NodeTests.txt");
		}
	}
}
