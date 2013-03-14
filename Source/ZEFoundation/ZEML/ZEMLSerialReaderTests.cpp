//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLSerialReaderTests.cpp
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

#include "ZEML/ZEMLSerialReader.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEMLSerialWriter.h"
#include "ZEMLNode.h"
#include "ZEMLDataProperty.h"
#include "ZEMLProperty.h"

ZETestSuite(ZEMLSerialReader)
{
	ZETest("ZEMLSerialReader::ZEMLSerialReader(ZEFile* File)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

		ZEMLSerialReader Reader(File);
		//Reader.File: File
		//Reader.DataItemDataPointer: 0
		ZEMLSerialPointer MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 0);

		File->Close();
		remove("SerialReaderTests.txt");
	}

	ZETest("bool ZEMLSerialReader::Read()")
	{
		ZEFile* File = new ZEFile();
		bool ResultForRead;

		ZETestCase("for an empty file")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialReader Reader(File);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheck(Reader.GetItemName() == "");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);
			//Reader.DataItemDataPointer: 0

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEFile")
		{
			unsigned char* Buffer = new unsigned char[1024];
			for ( ZEInt I = 0; I < 1024; I++)
			{
				Buffer[I] = I % 256;
			}

			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			File->Write(Buffer, sizeof(unsigned char), 3);
			File->Flush();

			ZEMLSerialReader Reader(File);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheck(Reader.GetItemName() == "");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 3);
			//Reader.DataItemDataPointer : 0

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZML file with serial writer for bool typed variant")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			Node.WriteProperty("Prop", true);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 41);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 17);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 28);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "Prop");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_BOOLEAN);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_BOOLEAN);
			ZETestCheck(Reader.GetItemValue().GetBoolean());
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);
	
			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheck(Reader.GetItemName() == NULL);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 69);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with string values")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEVariant Value;
			ZEString String1 = "TestString";
			Value.SetString(String1);
			Node.WriteProperty("Prop1", Value);
			ZEString String2 = "";
			Node.WriteProperty("Prop2", String2);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 69);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 2);
			ZETestCheckEqual(Reader.GetDataSize(), 45);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 28);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "Prop1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheckString(Reader.GetItemValue().GetString(), "TestString");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "Prop2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheck(Reader.GetItemValue().GetString() == "");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 80);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheck(Reader.GetItemName() == NULL);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 97);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with float")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			float Value = 2.0f;
			Node.WriteProperty("PropFloat", Value);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropFloat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetFloat(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZML file with serial writer with double")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			double Value = 2.0;
			Node.WriteProperty("PropDouble", Value);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropDouble");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_DOUBLE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_DOUBLE);
			ZETestCheckEqual(Reader.GetItemValue().GetDouble(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with ZEInt8")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEInt8 Value = 'z';
			Node.WriteProperty("PropInt8", Value);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheck(ResultForRead);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetInt8(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with ZEInt16")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEInt16 Value = 30;
			Node.WriteProperty("PropInt16", Value);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetInt16(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with ZEInt32, ZEInt64, ZEUInt8, ZEUInt16, ZEUInt32, ZEUInt64 respectively")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEInt32 Value32 = 500;
			Node.WriteProperty("PropInt32", Value32);
			ZEInt64 Value64 = 500;
			Node.WriteProperty("PropInt64", Value64);
			ZEUInt8 ValueU8 = 'z';
			Node.WriteProperty("PropUInt8", ValueU8);
			ZEUInt16 ValueU16 = 500;
			Node.WriteProperty("PropUInt16", ValueU16);
			ZEUInt32 ValueU32 = 500;
			Node.WriteProperty("PropUInt32", ValueU32);
			ZEUInt64 ValueU64 = 500;
			Node.WriteProperty("PropUInt64", ValueU64);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 6);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt32");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT32);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_32);
			ZETestCheckEqual(Reader.GetItemValue().GetInt32(), Value32);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt64");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT64);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_64);
			ZETestCheckEqual(Reader.GetItemValue().GetInt64(), Value64);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 77);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropUInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt8(), ValueU8);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 106);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropUInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt16(), ValueU16);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 128);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropUInt32");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT32);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt32(), ValueU32);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 152);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropUInt64");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT64);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt64(), ValueU64);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 178);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 208);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file with serial writer with ZEQuaternion, ZEVector2, ZEVector3, ZEVector4, ZEMatrix3x3, ZEMatrix4x4 respectively")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEQuaternion Quat = ZEQuaternion::Identity;
			Node.WriteProperty("PropQuat", Quat);
			ZEVector2 Vec2 = ZEVector2::One;
			Node.WriteProperty("PropVec2", Vec2);
			ZEVector3 Vec3 = ZEVector3::Zero;
			Node.WriteProperty("PropVec3", Vec3);
			ZEVector4 Vec4 = ZEVector4::Zero;
			Node.WriteProperty("PropVec4", Vec4);
			ZEMatrix3x3 Mat3x3 = ZEMatrix3x3::Identity;
			Node.WriteProperty("PropMat3x3", Mat3x3);
			ZEMatrix4x4 Mat4x4 = ZEMatrix4x4::Zero;
			Node.WriteProperty("PropMat4x4", Mat4x4);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 6);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropQuat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_QUATERNION);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_QUATERNION);
			ZETestCheckEqual(Reader.GetItemValue().GetQuaternion(), Quat);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR2);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR2);
			ZETestCheckEqual(Reader.GetItemValue().GetVector2(), Vec2);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 88);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec3");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR3);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR3);
			ZETestCheckEqual(Reader.GetItemValue().GetVector3(), Vec3);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 116);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec4");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR4);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR4);
			ZETestCheckEqual(Reader.GetItemValue().GetVector4(), Vec4);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 148);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropMat3x3");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_MATRIX3X3);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_MATRIX3X3);
			ZETestCheckEqual(Reader.GetItemValue().GetMatrix3x3(), Mat3x3);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 184);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropMat4x4");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_MATRIX4X4);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_MATRIX4X4);
			ZETestCheckEqual(Reader.GetItemValue().GetMatrix4x4(), Mat4x4);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 242);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(Reader.GetCurrentPointer(), File->GetSize());

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZML file with ZEMLNode Write")
		{
			ZEVariant Value;
			Value.SetFloat(2.0f);
			ZEMLNode Node("ParentNode");
			Node.AddProperty("PropFloat", Value);
			Value.SetInt8('z');
			Node.AddProperty("PropInt8", Value);
			Value.SetInt16(300);
			Node.AddProperty("PropInt16", Value);
			ZEString String = "String";
			Value.SetString(String);
			Node.AddProperty("PropString", Value);
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZETestCheckEqual(Node.GetTotalSize(), 98);
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "ParentNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 4);
			ZETestCheckEqual(Reader.GetDataSize(), 98);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropFloat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetFloat(), 2.0f);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 30);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetInt8(), 'z');
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 55);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetInt16(), 300);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 76);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "PropString");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheckString(Reader.GetItemValue().GetString(), "String");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 99);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 128);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZEML file ZEMLDataProperty with ZEMLNode Write ")
		{
			ZESize DataCount = 10;
			void* Data = new unsigned char[DataCount];
			ZEMLNode Node("ParentNode");
			Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char) * 10, false);
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZETestCheckEqual(Node.GetTotalSize(), 31);
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "ParentNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 31);
			ZETestCheck(!Reader.GetData(Data, sizeof(unsigned char), 0));
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), sizeof(unsigned char) * 10);
			ZETestCheck(Reader.GetData(Data, Reader.GetDataSize(), 0));
			ZETestCheckEqual(Reader.GetCurrentPointer(), 30);
			//Reader.DataItemDataPointer : 51

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 61);
			//Reader.DataItemDataPointer : 51

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}

		ZETestCase("write ZEMLDataProperty to ZEML file with serial writer")
		{
			void* Data = new char[sizeof(unsigned char)];
			void* Buffer = new char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			Node.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 28);
			ZETestCheck(!Reader.GetData(Buffer, sizeof(unsigned char), 0));

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "DataProp");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetDataSize(), 1);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);
			//Reader.DataItemDataPointer : 72
			ZETestCheck(Reader.GetData(Buffer, sizeof(unsigned char), 0));
			ZEInt Compare = memcmp(Data, Buffer, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
			delete Buffer;
		}

		ZETestCase("write data properties to ZEML file with serial writer")
		{
			void* Data = new char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			Root.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));
			Root.WriteDataProperty("DataProp2", Data, sizeof(unsigned char));
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 0);
			//Reader.DataItemDataPointer : 0

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 28);
			//Reader.DataItemDataPointer : 49

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckString(Reader.GetItemName(), "DataProp2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 50);
			//Reader.DataItemDataPointer : 71

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 72);
			//Reader.DataItemDataPointer : 71

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}
	}

	ZETest("bool ZEMLSerialReader::SkipNodeAndRead()")
	{
		ZEFile* File = new ZEFile();

		ZETestCase("open nodes over root node and skip the first node, then skip the second one")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);

			ZEMLSerialNode Node1 = Root.OpenNode("Node1");
			Node1.WriteProperty("Prop1", true);
			Node1.WriteProperty("Prop2", "test");
			Node1.WriteProperty("Prop3", 5.1f);

			File->Flush();
			ZEInt64 PositionForNode2 = File->GetSize();

			ZEMLSerialNode Node2 = Root.OpenNode("Node2");
			void* Data1 = new unsigned char[5];
			void* Data2 = new unsigned char[3];
			Node2.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * 5);
			Node2.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char) * 3);

			File->Flush();
			ZEInt64 PositionForProp4 = File->GetSize();

			Root.WriteProperty("Prop4", false);
			Root.WriteProperty("Prop5", "test");

			Node2.CloseNode();
			Node1.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);

			bool SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(SkipAndRead);
			ZETestCheckString(Reader.GetItemName(), "Root");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			
			bool ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Node1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(SkipAndRead);
			ZETestCheckString(Reader.GetItemName(), "Node2");
			ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForNode2);

			SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(SkipAndRead);
			ZETestCheckString(Reader.GetItemName(), "Prop4");
			ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForProp4);

			SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(SkipAndRead);
			ZETestCheckString(Reader.GetItemName(), "Prop5");

			SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(!SkipAndRead);
			ZETestCheck(Reader.GetItemName() == "");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("for root node includes only one node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);

			bool SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(SkipAndRead);
			ZETestCheckEqual(Reader.GetItemName(), "Root");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			SkipAndRead = Reader.SkipNodeAndRead();
			ZETestCheck(!SkipAndRead);
			ZETestCheckEqual(Reader.GetCurrentPointer(), File->GetSize());
			ZETestCheck(Reader.GetItemName() == "");

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("open a serial node over a serial node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);

			ZEMLSerialNode Node1 = Root.OpenNode("Node1");

			Node1.WriteProperty("Prop1", 3.0f);
			Node1.WriteProperty("Prop2", true);

			ZEMLSerialNode Node2 = Node1.OpenNode("Node2");

			void* Data = new char[2];
			Node2.WriteDataProperty("DataProp1", Data, sizeof(char) * 2);
			Node2.WriteProperty("Prop3", "test");

			Node2.CloseNode();
			Node1.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);
			bool ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			bool Result = Reader.SkipNodeAndRead();
			ZETestCheck(!Result);
			ZETestCheckEqual(Reader.GetCurrentPointer(), File->GetSize());

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}

		ZETestCase("for three root nodes")
		{
			void* Data = new char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root1("RootNode1", File);

			ZEMLSerialNode Node1 = Root1.OpenNode("Node1");

			Node1.WriteProperty("Prop1", 2.0f);
			Node1.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));

			Node1.CloseNode();
			Root1.CloseNode();

			ZEMLSerialRootNode Root2("RootNode2", File);

			ZEMLSerialNode Node2 = Root2.OpenNode("Node2");

			Node2.WriteProperty("Prop2", true);
			Node2.WriteDataProperty("DataProp2", Data, sizeof(unsigned char));

			Node2.CloseNode();
			Root2.CloseNode();

			ZEMLSerialRootNode Root3("RootNode3", File);

			ZEMLSerialNode Node3 = Root3.OpenNode("Node3");

			Node3.WriteProperty("Prop3", "test");
			Node3.WriteDataProperty("DataProp3", Data, sizeof(unsigned char));

			Node3.CloseNode();
			Root3.CloseNode();

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);

			bool ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "RootNode1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZEMLSerialPointer MaxPointer = Reader.GetCurrentPointer();
			ZETestCheckEqual(MaxPointer, 0);
			ZEUInt64 CurrentItemDataSize = Reader.GetDataSize();
			ZETestCheckEqual(CurrentItemDataSize, 68);
			ZEInt64 FileCur = File->Tell();
			ZETestCheckEqual(FileCur, 29);

			bool Result = Reader.SkipNodeAndRead();
			ZETestCheck(Result);
			ZETestCheckString(Reader.GetItemName(), "RootNode2");
			MaxPointer = Reader.GetCurrentPointer();
			ZETestCheckEqual(MaxPointer, 97);
			CurrentItemDataSize = Reader.GetDataSize();
			ZETestCheckEqual(CurrentItemDataSize, 65);
			FileCur = File->Tell();
			ZETestCheckEqual(FileCur, 126);

			Result = Reader.SkipNodeAndRead();
			ZETestCheck(Result);
			ZETestCheckString(Reader.GetItemName(), "RootNode3");
			MaxPointer = Reader.GetCurrentPointer();
			ZETestCheckEqual(MaxPointer, 191);
			CurrentItemDataSize = Reader.GetDataSize();
			ZETestCheckEqual(CurrentItemDataSize, 69);
			FileCur = File->Tell();
			ZETestCheckEqual(FileCur, 220);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Node3");

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}
	}

	ZETest("ZEMLItemType ZEMLSerialReader::GetItemType()")
	{
		ZEFile* File = new ZEFile();
		bool ResultForRead;
		ZEMLItemType ItemType;

		ZETestCase("ZEML_IT_UNDEFINED")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialReader Reader(File);
			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);

			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_UNDEFINED);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("different from ZEML_IT_UNDEFINED")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEString Value = "String";
			RootNode.WriteProperty("Prop", Value);
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);

			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			ResultForRead = Reader.Read();
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_STRING);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write data property to node")
		{
			void* Data = new char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			Node.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);

			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			ResultForRead = Reader.Read();
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			ResultForRead = Reader.Read();
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_INLINE_DATA);

			ResultForRead = Reader.Read();
			ZETestCheck(!ResultForRead);
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_UNDEFINED);

			File->Close();
			remove("SeriaReaderTests.txt");
			delete Data;
		}
	}

	ZETest("ZEString ZEMLSerialReader::GetItemName()")
	{
		ZEFile* File = new ZEFile();
		bool ResultForRead;
		ZEString ItemName;

		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialReader Reader(File);
		ResultForRead = Reader.Read();
		ZETestCheck(!ResultForRead);

		ItemName = Reader.GetItemName();
		ZETestCheck(ItemName == "");

		File->Close();
		remove("SerialReaderTests.txt");

		ZETestCase("for name not null")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEInt8 Value = 'a';
			RootNode.WriteProperty("Prop", Value);
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);

			ItemName = Reader.GetItemName();
			ZETestCheckString(ItemName, "RootNode");

			ResultForRead = Reader.Read();
			ZETestCheck(ResultForRead);

			ItemName = Reader.GetItemName();
			ZETestCheckString(ItemName, "Prop");

			File->Close();
			remove("SerialReaderTests.txt");
		}
	}

	ZETest("ZEUInt64 ZEMLSerialReader::GetSubItemCount()")
	{
		ZEFile* File = new ZEFile();
		bool ResultForRead;
		ZEUInt64 SubItemCount;

		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialReader Reader(File);
		ResultForRead = Reader.Read();
		ZETestCheck(!ResultForRead);

		SubItemCount = Reader.GetSubItemCount();
		ZETestCheckEqual(SubItemCount, 0);

		File->Close();
		remove("SerialReaderTests.txt");

		ZETestCase("open 2 nodes over root node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			ZEMLSerialNode Node1 = Root.OpenNode("Node1");
			ZEMLSerialNode Node2 = Root.OpenNode("Node2");
			Node1.WriteProperty("Prop1", true);
			ZEVariant Value;
			Value.SetInt8('z');
			Node1.WriteProperty("Prop2", Value);
			Node1.WriteProperty("Prop3", 2.1f);
			void* Data = new char[sizeof(unsigned char)];
			Node2.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));
			Node2.WriteDataProperty("DataProp2", Data, sizeof(unsigned char));
			Node2.CloseNode();
			Node1.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 2);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Node1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 3);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Node2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 2);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Prop1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_BOOLEAN);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Prop2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Prop3");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "DataProp2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			ResultForRead = Reader.Read();
			ZETestCheck(Reader.GetItemName() == "");
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}
	}

	ZETest("ZEVariant ZEMLSerialReader::GetItemValue()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEInt8 Value = 'a';
		RootNode.WriteProperty("Prop", Value);
		RootNode.CloseNode();
		ZEMLSerialReader Reader(File);
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		bool ResultForRead = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
		ZEVariant ItemValue = Reader.GetItemValue();
		ZETestCheckEqual(ItemValue.GetType(), ZE_VRT_UNDEFINED);

		ResultForRead = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
		ItemValue = Reader.GetItemValue();
		ZETestCheckEqual(ItemValue.GetType(), ZE_VRT_INTEGER_8);
		ZETestCheckEqual(ItemValue.GetInt8(), 'a');

		File->Close();
		remove("SerialReaderTests.txt");
	}

	ZETest("ZEUInt64 ZEMLSerialReader::GetDataSize()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEInt8 Value = 'a';
		RootNode.WriteProperty("Prop", Value);
		RootNode.CloseNode();
		ZEMLSerialReader Reader(File);
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		bool ResultForRead = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
		ZEUInt64 DataSize = Reader.GetDataSize();
		ZETestCheckEqual(DataSize, 17);

		ResultForRead = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
		DataSize = Reader.GetDataSize();
		ZETestCheckEqual(DataSize, 0);

		File->Close();
		remove("SerialReaderTests.txt");

		ZETestCase("write ZEInt8 value to ZEMLNode")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLNode Node("ParentNode");
			Node.AddProperty("Prop", Value);
			Node.Write(File);
			ZEMLSerialReader SerialReader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = SerialReader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(SerialReader.GetItemType(), ZEML_IT_NODE);
			DataSize = SerialReader.GetDataSize();
			ZETestCheckEqual(DataSize, 17);

			ResultForRead = SerialReader.Read();
			ZETestCheck(ResultForRead);
			ZETestCheckEqual(SerialReader.GetItemType(), ZEML_IT_INT8);
			DataSize = SerialReader.GetDataSize();
			ZETestCheckEqual(DataSize, 0);

			ResultForRead = SerialReader.Read();
			ZETestCheck(!ResultForRead);
			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("for ItemType ZEML_IT_INLINE_DATA")
		{
			ZESize DataCount = 10;
			void* Data = new unsigned char[DataCount];

			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLNode Node("ParentNode");
			Node.AddDataProperty("DataProp", Data, sizeof(unsigned char) * DataCount, false);
			Node.Write(File);
			ZEMLSerialReader TestReader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = TestReader.Read();
			ZETestCheckEqual(TestReader.GetItemType(), ZEML_IT_NODE);
			DataSize = TestReader.GetDataSize();
			ZETestCheckEqual(DataSize, 30);

			ResultForRead = TestReader.Read();
			ZETestCheckEqual(TestReader.GetItemType(), ZEML_IT_INLINE_DATA);
			DataSize = TestReader.GetDataSize();
			ZETestCheckEqual(DataSize, sizeof(unsigned char) * DataCount);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}

		ZETestCase("write data property and property")
		{
			ZESize DataCount = 10;
			void* Data1 = new unsigned char[DataCount];
			void* Data2 = new unsigned char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * DataCount);
			Node.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char));
			Node.WriteProperty("Prop", true);
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader SReader(File);

			ResultForRead = SReader.Read();
			ZETestCheckString(SReader.GetItemName(), "RootNode");
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_NODE);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 94);

			ResultForRead = SReader.Read();
			ZETestCheckString(SReader.GetItemName(), "Node");
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_NODE);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 70);

			ResultForRead = SReader.Read();
			ZETestCheckString(SReader.GetItemName(), "DataProp1");
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_INLINE_DATA);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 10);

			ResultForRead = SReader.Read();
			ZETestCheckString(SReader.GetItemName(), "DataProp2");
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_INLINE_DATA);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 1);

			ResultForRead = SReader.Read();
			ZETestCheckString(SReader.GetItemName(), "Prop");
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_BOOLEAN);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 0);

			ResultForRead = SReader.Read();
			ZETestCheck(!ResultForRead);
			ZETestCheckEqual(SReader.GetItemType(), ZEML_IT_UNDEFINED);
			DataSize = SReader.GetDataSize();
			ZETestCheckEqual(DataSize, 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data1;
			delete Data2;
		}
	}

	ZETest("bool ZEMLSerialReader::GetData(void* Buffer, ZEUInt64 BufferSize, ZEUInt64 Offset)")
	{
		ZEFile* File = new ZEFile();
		unsigned char* Buffer = new unsigned char[1024];
		bool ResultForRead;
		bool Get;

		ZETestCase("for CurrentItemType of ZEMLSerialReader != ZEML_IT_INLINE_DATA")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEInt8 Value = 'a';
			RootNode.WriteProperty("Prop", Value);
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(!Get);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(!Get);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write data properties to ZEMLNode")
		{
			void* Data1 = new unsigned char[3];
			void* Data2 = new unsigned char[4];
			ZEMLNode Node("ParentNode");
			Node.AddDataProperty("DataProp1", Data1, sizeof(unsigned char) * 3, false);
			Node.AddDataProperty("DataProp2", Data2, sizeof(unsigned char) * 4, false);
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 3, 0);
			ZETestCheck(Get);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 4, 0);
 			ZETestCheck(Get);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data1;
			delete Data2;
		}

		ZETestCase("write data properties to serial node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			void* Data1 = new unsigned char[3];
			void* Data2 = new unsigned char[4];
			ZEMLSerialRootNode Root("RootNode", File);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * 3);
			Node.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char) * 4);
			Node.CloseNode();
			Root.CloseNode();
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheck(!Reader.GetData(Buffer, sizeof(unsigned char), 0));
			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheck(!Reader.GetData(Buffer, sizeof(unsigned char), 0));

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 3, 0);
			ZETestCheck(Get);
			ZETestCheckEqual(memcmp(Data1, Buffer, sizeof(unsigned char) * 3), 0);

			ResultForRead = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 4, 0);
			ZETestCheck(Get);
			ZETestCheckEqual(memcmp(Data2, Buffer, sizeof(unsigned char) * 4), 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data1;
			delete Data2;
		}

		ZETestCase("for Offset != 0 and offset exceed the data size")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			void* Data1 = new unsigned char[3];
			void* Data2 = new unsigned char[4];
			ZEMLSerialRootNode Root("RootNode", File);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * 3);
			File->Flush();
			ZEInt64 PositionForDataProp2 = File->GetSize();
			Node.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char) * 4);
			Node.CloseNode();
			Root.CloseNode();
			ZEMLSerialReader Reader(File);
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			Reader.SeekPointer(52);
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);

			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 3, 2);
			ZETestCheck(Get);
			ZEInt Comp = memcmp(Data1, Buffer, sizeof(unsigned char) * 3);
			ZETestCheck(Comp != 0);

			Reader.SeekPointer(PositionForDataProp2);
			ZETestCheckString(Reader.GetItemName(), "DataProp2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);

			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 4, 5);
			//error Can not read ZEMLDataProperty data with offset : 5
			ZETestCheck(!Get);
			Comp = memcmp(Data2, Buffer, sizeof(unsigned char) * 4);
			ZETestCheck(Comp != 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data1;
			delete Data2;
		}

		ZETestCase("Offset != 0")
		{
			void* Data1 = new unsigned char[3];
			void* Data2 = new unsigned char[4];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * 3);
			Node.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char) * 4);
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			Reader.SeekPointer(52);
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetDataSize(), 3);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 52);
			//Reader.DataItemDataPointer : 73

			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 3, 2);
			ZETestCheck(Get);
			ZEInt Comp = memcmp(Data1, Buffer, sizeof(unsigned char) * 3);
			ZETestCheck(Comp != 0);

			Reader.SeekPointer(76);
			ZETestCheckString(Reader.GetItemName(), "DataProp2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetDataSize(), 4);
			ZETestCheckEqual(Reader.GetCurrentPointer(), 76);
			//Reader.DataItemDataPointer : 97

			Get = Reader.GetData(Buffer, sizeof(unsigned char) * 4, 0);
			ZETestCheck(Get);
			Comp = memcmp(Data2, Buffer, sizeof(unsigned char) * 4);
			ZETestCheck(Comp == 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data2;
			delete Data1;
		}
	}

	ZETest("bool ZEMLSerialReader::GetData(ZEPartialFile& File)")
	{
		ZESize DataCount1 = 5;
		ZESize DataCount2 = 3;
		ZESize DataCount3 = 10;
		void* Data1 = new unsigned char[DataCount1];
		void* Data2 = new unsigned char[DataCount2];
		void* Data3 = new unsigned char[DataCount3];

		ZEFile* File = new ZEFile;

		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

		ZEMLSerialRootNode Root("Root", File);
		File->Flush();			ZEInt64 PosForDataProp1 = File->Tell();
		ZETestCheckEqual(PosForDataProp1, 24);

		Root.WriteDataProperty("DataProp1", Data1, sizeof(unsigned char) * DataCount1);
		File->Flush();
		ZEInt64 PosForDataProp2 = File->Tell();
		ZETestCheckEqual(PosForDataProp2, 50);

		Root.WriteDataProperty("DataProp2", Data2, sizeof(unsigned char) * DataCount2);
		File->Flush();
		ZEInt64 PosForDataProp3 = File->Tell();
		ZETestCheckEqual(PosForDataProp3, 74);

		Root.WriteDataProperty("DataProp3", Data3, sizeof(unsigned char) * DataCount3);
		File->Flush();
		ZEInt64 PosForNull = File->GetSize();
		ZETestCheckEqual(PosForNull, 105);

		Root.CloseNode();
		File->Seek(-PosForNull * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZEMLSerialReader Reader(File);

		Reader.SeekPointer(PosForDataProp1);
		ZETestCheckString(Reader.GetItemName(), "DataProp1");
		//Reader.DataItemDataPointer : 45
		ZETestCheckEqual(Reader.GetDataSize(), DataCount1);

		ZEPartialFile PartialFile;

		bool Get = Reader.GetData(PartialFile);
		ZETestCheck(Get);
		ZETestCheckEqual(PartialFile.GetSize(), DataCount1);
		ZEInt64 CurFile = File->Tell();
		ZETestCheckEqual(CurFile, 50);
		ZEMLSerialPointer MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 24);

		void* Buffer = new unsigned char[DataCount3];
		ZESize ReadCount = PartialFile.Read(Buffer, sizeof(unsigned char), DataCount1);
		ZETestCheckEqual(ReadCount, DataCount1);
		ZEInt Compare = memcmp(Buffer, Data1, sizeof(unsigned char) * DataCount1);
		ZETestCheck(Compare == 0);
		PartialFile.Close();

		Reader.SeekPointer(PosForDataProp2);
		ZETestCheckString(Reader.GetItemName(), "DataProp2");
		ZETestCheckEqual(Reader.GetDataSize(), 3);
		//Reader.DataItemDataPointer : 71

		Get = Reader.GetData(PartialFile);
		ZETestCheck(Get);
		ZETestCheckEqual(PartialFile.GetSize(), Reader.GetDataSize());

		ReadCount = PartialFile.Read(Buffer, sizeof(unsigned char), DataCount2);
		ZETestCheckEqual(ReadCount, DataCount2);
		Compare = memcmp(Buffer, Data2, sizeof(unsigned char) * DataCount2);
		ZETestCheck(Compare == 0);
		PartialFile.Close();

		bool ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "DataProp3");
		ZETestCheckEqual(Reader.GetDataSize(), DataCount3);
		//Reader.DataItemDataPointer : 95

		Get = Reader.GetData(PartialFile);
		ZETestCheck(Get);
		ZETestCheckEqual(PartialFile.GetSize(), DataCount3);

		ReadCount = PartialFile.Read(Buffer, sizeof(unsigned char), DataCount3);
		ZETestCheckEqual(ReadCount, DataCount3);
		Compare = memcmp(Buffer, Data3, sizeof(unsigned char) * DataCount3);
		ZETestCheck(Compare == 0);

		PartialFile.Close();
		File->Close();
		remove("SerialReaderTests.txt");
		delete Data3;
		delete Data2;
		delete Data1;
	}

	ZETest("void ZEMLSerialReader::SeekPointer(ZEMLSerialPointer Pointer)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode Root("RootNode", File);
		File->Flush();
		ZEInt64 PositionForPropBool = File->GetSize();
		ZETestCheckEqual(PositionForPropBool, 28);
		Root.WriteProperty("PropBool", true);
		File->Flush();
		ZEInt64 PositionForPropFloat = File->GetSize();
		ZETestCheckEqual(PositionForPropFloat, 49);
		Root.WriteProperty("PropFloat", 3.0f);
		File->Flush();
		ZEInt64 PositionForDataProp = File->GetSize();
		ZETestCheckEqual(PositionForDataProp, 74);
		void* Data = new char[sizeof(unsigned char)];
		Root.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
		File->Flush();
		ZEInt64 PositionForNull = File->GetSize();
		ZETestCheckEqual(PositionForNull, 95);
		Root.CloseNode();
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
		ZEMLSerialReader Reader(File);

		ZEMLSerialPointer Pointer = 17;
		Reader.SeekPointer(Pointer);
		//error Identifier mismatch. Corrupted ZEML file.
		ZETestCheckEqual(Reader.GetCurrentPointer(), 17);
		ZETestCheck(Reader.GetItemName() == "");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);

		Pointer = PositionForPropBool;
		Reader.SeekPointer(Pointer);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 28);
		ZETestCheckString(Reader.GetItemName(), "PropBool");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_BOOLEAN);
		ZETestCheck(Reader.GetItemValue().GetBoolean());

		Reader.SeekPointer(PositionForPropFloat);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 49);
		ZETestCheckString(Reader.GetItemName(), "PropFloat");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
		ZETestCheckEqual(Reader.GetItemValue().GetFloat(), 3.0f);

		Reader.SeekPointer(PositionForDataProp);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 74);
		ZETestCheckString(Reader.GetItemName(), "DataProp");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
		ZETestCheckEqual(Reader.GetDataSize(), 1);

		Reader.SeekPointer(PositionForNull);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 95);
		ZETestCheck(Reader.GetItemName() == "");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);

		Reader.SeekPointer(17);
		//error Identifier mismatch. Corrupted ZEML file.
		ZETestCheckEqual(Reader.GetCurrentPointer(), 95);
		ZETestCheck(Reader.GetItemName() == "");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);

		Reader.SeekPointer(PositionForPropBool);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 95);
		ZETestCheckString(Reader.GetItemName(), "PropBool");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_BOOLEAN);

		Reader.SeekPointer(PositionForDataProp);
		ZETestCheckEqual(Reader.GetCurrentPointer(), 95);
		ZETestCheckString(Reader.GetItemName(), "DataProp");
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);

		File->Close();
		remove("SerialReaderTests.txt");		
		delete Data;
	}

	ZETest("ZEMLSerialPointer ZEMLSerialReader::GetCurrentPointer()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode Root("RootNode", File);
		Root.WriteProperty("PropBool", true);
		Root.WriteProperty("PropFloat", 3.0f);
		void* Data = new char[sizeof(unsigned char)];
		Root.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
		Root.CloseNode();
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
		
		ZEMLSerialReader Reader(File);
		ZETestCheck(Reader.GetItemName() == "");
		ZEMLSerialPointer MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 0);

		bool ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "RootNode");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 0);

		ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "PropBool");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 28);

		Reader.SeekPointer(13);
		//error Identifier mismatch. Corrupted ZEML file.
		ZETestCheck(Reader.GetItemName() == "");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 28);
		Reader.GoToCurrentPointer();
		ZETestCheckString(Reader.GetItemName(), "PropBool");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 28);

		ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "PropFloat");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 49);

		ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "DataProp");
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 74);

		ResultForRead = Reader.Read();
		ZETestCheck(!ResultForRead);
		MaxPointer = Reader.GetCurrentPointer();
		ZETestCheckEqual(MaxPointer, 95);

		File->Close();
		remove("SerialReaderTests.txt");
		delete Data;
	}

	ZETest("void ZEMLSerialReader::GoToCurrentPointer()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode Root("Root", File);
		File->Flush();
		ZEInt64 PositionForProp1 = File->GetSize();

		Root.WriteProperty("Prop1", true);
		File->Flush();
		ZEInt64 PositionForProp2 = File->GetSize();

		Root.WriteProperty("Prop2", false);
		File->Flush();
		ZEInt64 PositionForNode1 = File->GetSize();

		ZEMLSerialNode Node1 = Root.OpenNode("Node1");
		File->Flush();
		ZEInt64 PositionForProp3 = File->GetSize();

		Node1.WriteProperty("Prop3", 3.4f);
		File->Flush();
		ZEInt64 PositionForNode2 = File->GetSize();

		ZEMLSerialNode Node2 = Root.OpenNode("Node2");
		File->Flush();
		ZEInt64 PositionForProp4 = File->GetSize();

		Node2.WriteProperty("Prop4", "test");
		Node2.CloseNode();
		Node1.CloseNode();
		Root.CloseNode();
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZEMLSerialReader Reader(File);

		ZEMLSerialPointer Node1Pointer, Node2Pointer;
		ZEVariant Prop1, Prop2, Prop3;

		ZEMLSerialListItem RootItemList[4] = {
			ZEML_LIST_PROPERTY("Prop1", Prop1, ZE_VRT_BOOLEAN, true),
			ZEML_LIST_PROPERTY("Prop2", Prop2, ZE_VRT_BOOLEAN, true),
			ZEML_LIST_NODE("Node1", Node1Pointer, true),
			ZEML_LIST_NODE("Node2", Node2Pointer, true)
		};

		ZEMLSerialListItem Node1ItemList[1] = ZEML_LIST_PROPERTY("Prop3", Prop3, ZE_VRT_FLOAT, true);

		bool ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "Root");

		bool ReadPropList = Reader.ReadPropertyList(RootItemList, 4);
		ZETestCheck(ReadPropList);
		ZETestCheckString(Reader.GetItemName(), "Node2");
		ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForNode2);

		Reader.SeekPointer(PositionForNode1);
		ZETestCheckString(Reader.GetItemName(), "Node1");
		ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForNode2);

		ReadPropList = Reader.ReadPropertyList(Node1ItemList, 1);
		ZETestCheck(ReadPropList);
		ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForNode2);
		ZETestCheckString(Reader.GetItemName(), "Prop3");
		
		Reader.GoToCurrentPointer();
		ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForNode2);
		ZETestCheckString(Reader.GetItemName(), "Node2");

		ResultForRead = Reader.Read();
		ZETestCheckString(Reader.GetItemName(), "Prop4");
		ZETestCheckEqual(Reader.GetCurrentPointer(), PositionForProp4);

		File->Close();
		remove("SerialReaderTests.txt");
	}

	ZETest("bool ZEMLSerialReader::ReadPropertyList(ZEMLSerialListItem* List, ZESize ItemCount)")
	{
		ZEFile* File = new ZEFile();
		void* Data = new char[sizeof(unsigned char)];

		bool ResultForRead;
		bool ReadPropList;

		ZETestCase("for a serial root node which includes properties, data properties and nodes")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);
			Root.WriteProperty("PropFloat", 7.4f);
			ZEMLSerialNode Node = Root.OpenNode("Node");
			Root.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
			Root.WriteProperty("PropBool", true);
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);

			ZEMLSerialPointer NodePointer, DataPointer;
			ZEVariant PropFloat, PropBool;

			ZEMLSerialListItem ItemList[4] = {
				ZEML_LIST_PROPERTY("PropFloat", PropFloat,		ZE_VRT_FLOAT,	true),
				ZEML_LIST_NODE("Node",			NodePointer,					true),
				ZEML_LIST_DATA("DataProp",		DataPointer,					true),
				ZEML_LIST_PROPERTY("PropBool",	PropBool,		ZE_VRT_BOOLEAN, true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root");

			ReadPropList = Reader.ReadPropertyList(ItemList, 4);
			ZETestCheck(ReadPropList);
			ZETestCheck(ItemList[0].IsFound);
			ZETestCheckString(ItemList[0].Name, "PropFloat");
			ZETestCheck(ItemList[1].IsFound);
			ZETestCheckString(ItemList[1].Name, "Node");
			ZETestCheckEqual(*ItemList[1].Pointer, 49);
			ZETestCheck(ItemList[2].IsFound);
			ZETestCheckString(ItemList[2].Name, "DataProp");
			ZETestCheckEqual(*ItemList[2].Pointer, 73);
			ZETestCheck(ItemList[3].IsFound);
			ZETestCheckString(ItemList[3].Name, "PropBool");

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("List1 for serial root node, List2 for serial node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			Root.WriteProperty("Prop1", true);
			Root.WriteProperty("Prop2", 3.0f);
			Root.WriteProperty("Prop3", "test");

			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteProperty("Prop4", "string");
			Node.WriteProperty("Prop5", false);
			Node.WriteProperty("Prop6", 4.0f);
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ZEVariant Prop1, Prop2, Prop3, Prop4, Prop5, Prop6;

			ZEMLSerialListItem List1[3] = {
				ZEML_LIST_PROPERTY("Prop1", Prop1, ZE_VRT_BOOLEAN,	true),
				ZEML_LIST_PROPERTY("Prop2", Prop2, ZE_VRT_FLOAT,	true),
				ZEML_LIST_PROPERTY("Prop3", Prop3, ZE_VRT_STRING,	true)
			};

			ZEMLSerialListItem List2[3] = {
				ZEML_LIST_PROPERTY("Prop4", Prop4, ZE_VRT_STRING,	true),
				ZEML_LIST_PROPERTY("Prop5", Prop5, ZE_VRT_BOOLEAN,	true),
				ZEML_LIST_PROPERTY("Prop6", Prop6, ZE_VRT_FLOAT,	true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "RootNode");

			ReadPropList = Reader.ReadPropertyList(List1, 3);
			ZETestCheck(ReadPropList);
			ZETestCheck(List1[0].IsFound);
			ZETestCheck(List1[0].Value->GetBoolean());
			ZETestCheck(List1[1].IsFound);
			ZETestCheckEqual(List1[1].Value->GetFloat(), 3.0f);
			ZETestCheck(List1[2].IsFound);
			ZETestCheckString(List1[2].Value->GetString(), "test");

			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetCurrentPointer(), 89);

			ReadPropList = Reader.ReadPropertyList(List2, 3);
			ZETestCheck(ReadPropList);
			ZETestCheck(List2[0].IsFound);
			ZETestCheckString(List2[0].Value->GetString(), "string");
			ZETestCheck(List2[1].IsFound);
			ZETestCheck(!List2[1].Value->GetBoolean());
			ZETestCheck(List2[2].IsFound);
			ZETestCheckEqual(List2[2].Value->GetFloat(), 4.0f);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("List1 for serial root node and includes Node, List2 for serial node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("RootNode", File);
			Root.WriteProperty("Prop1", true);
			Root.WriteProperty("Prop2", 3.0f);
			Root.WriteProperty("Prop3", "test");

			ZEMLSerialNode Node = Root.OpenNode("Node");
			Node.WriteProperty("Prop4", "string");
			Node.WriteProperty("Prop5", false);
			Node.WriteProperty("Prop6", 4.0f);
			Node.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ZEMLSerialReader Reader(File);

			ZEMLSerialPointer NodePointer;
			ZEVariant Prop1, Prop2, Prop3, Prop4, Prop5, Prop6;

			ZEMLSerialListItem List1[4] = {
				ZEML_LIST_PROPERTY("Prop1", Prop1,		ZE_VRT_BOOLEAN, true),
				ZEML_LIST_PROPERTY("Prop2", Prop2,		ZE_VRT_FLOAT,	true),
				ZEML_LIST_PROPERTY("Prop3", Prop3,		ZE_VRT_STRING,	true),
				ZEML_LIST_NODE("Node",		NodePointer,				true)
			};

			ZEMLSerialListItem List2[3] = {
				ZEML_LIST_PROPERTY("Prop4", Prop4, ZE_VRT_STRING,	true),
				ZEML_LIST_PROPERTY("Prop5", Prop5, ZE_VRT_BOOLEAN,	true),
				ZEML_LIST_PROPERTY("Prop6", Prop6, ZE_VRT_FLOAT,	true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "RootNode");

			ReadPropList = Reader.ReadPropertyList(List1, 4);

			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetCurrentPointer(), 89);

			ReadPropList = Reader.ReadPropertyList(List2, 3);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("for more than 1 root")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root1("Root1", File);
			Root1.WriteProperty("Prop1", true);
			Root1.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));
			Root1.CloseNode();

			ZEMLSerialRootNode Root2("Root2", File);
			Root2.WriteProperty("Prop2", "test");
			Root2.WriteDataProperty("DataProp2", Data, sizeof(unsigned char));
			Root2.CloseNode();

			ZEMLSerialRootNode Root3("Root3", File);
			Root3.WriteProperty("Prop3", 3.0f);
			Root3.WriteDataProperty("DataProp3", Data, sizeof(unsigned char));
			Root3.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ZEVariant Prop1, Prop2, Prop3;
			ZEMLSerialPointer DataPointer1, DataPointer2, DataPointer3;

			ZEMLSerialListItem List1[2] = {
				ZEML_LIST_PROPERTY("Prop1", Prop1,			ZE_VRT_BOOLEAN, true),
				ZEML_LIST_DATA("DataProp1", DataPointer1,					true)
			};

			ZEMLSerialListItem List2[2] = {
				ZEML_LIST_PROPERTY("Prop2", Prop2,			ZE_VRT_STRING, true),
				ZEML_LIST_DATA("DataProp2", DataPointer2,				   true)
			};

			ZEMLSerialListItem List3[2] = {
				ZEML_LIST_PROPERTY("Prop3", Prop3,			ZE_VRT_FLOAT,   true),
				ZEML_LIST_DATA("DataProp3", DataPointer3,					true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root1");

			ReadPropList = Reader.ReadPropertyList(List1, 2);
			ZETestCheck(ReadPropList);
			ZETestCheck(List1[0].IsFound);
			ZETestCheck(List1[0].Value->GetBoolean());
			ZETestCheck(List1[1].IsFound);
			ZETestCheckEqual(*List1[1].Pointer, 43);

			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root2");

			ReadPropList = Reader.ReadPropertyList(List2, 2);
			ZETestCheck(ReadPropList);
			ZETestCheck(List2[0].IsFound);
			ZETestCheckString(List2[0].Value->GetString(), "test");
			ZETestCheck(List2[1].IsFound);
			ZETestCheckEqual(*List2[1].Pointer, 112);

			ZETestCheckString(Reader.GetItemName(), "DataProp2");
			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root3");

			ReadPropList = Reader.ReadPropertyList(List3, 2);
			ZETestCheck(ReadPropList);
			ZETestCheck(List3[0].IsFound);
			ZETestCheckEqual(List3[0].Value->GetFloat(), 3.0f);
			ZETestCheck(List3[1].IsFound);
			ZETestCheckEqual(*List3[1].Pointer, 180);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("open nodes over root node and write properties to child nodes")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);
			Root.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));
			ZEMLSerialNode Node1 = Root.OpenNode("Node1");
			Node1.WriteProperty("Prop1", true);
			Node1.WriteProperty("Prop2", false);
			ZEMLSerialNode Node2 = Root.OpenNode("Node2");
			Node2.WriteProperty("Prop3", "test");
			Node2.WriteProperty("Prop4", 5.5f);
			Node2.CloseNode();
			Node1.CloseNode();
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ZEMLSerialPointer RootPointer, Data1Pointer, Node1Pointer, Node2Pointer;
			ZEVariant Prop1, Prop2, Prop3, Prop4;

			ZEMLSerialListItem RootItemList[4] = {
				ZEML_LIST_NODE("Root", RootPointer, false),
				ZEML_LIST_DATA("DataProp1", Data1Pointer, true),
				ZEML_LIST_NODE("Node1", Node1Pointer, true),
				ZEML_LIST_NODE("Node2", Node2Pointer, true)
			};

			ZEMLSerialListItem Node1ItemList[3] = {
				ZEML_LIST_NODE("Node1", Node1Pointer, false),
				ZEML_LIST_PROPERTY("Prop1", Prop1, ZE_VRT_BOOLEAN, true),
				ZEML_LIST_PROPERTY("Prop2", Prop2, ZE_VRT_BOOLEAN, true)
			};

			ZEMLSerialListItem Node2ItemList[2] = {
				ZEML_LIST_PROPERTY("Prop3", Prop3, ZE_VRT_STRING, true),
				ZEML_LIST_PROPERTY("Prop4", Prop4, ZE_VRT_FLOAT, true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root");

			ReadPropList = Reader.ReadPropertyList(RootItemList, 4);
			ZETestCheck(ReadPropList);
			ZETestCheck(!RootItemList[0].IsFound);
			ZETestCheckString(RootItemList[0].Name, "Root");
			ZETestCheck(RootItemList[1].IsFound);
			ZETestCheckString(RootItemList[1].Name, "DataProp1");
			ZETestCheckEqual(*RootItemList[1].Pointer, 24);
			ZETestCheck(RootItemList[2].IsFound);
			ZETestCheckString(RootItemList[2].Name, "Node1");
			ZETestCheckEqual(*RootItemList[2].Pointer, 46);
			ZETestCheck(RootItemList[3].IsFound);
			ZETestCheckString(RootItemList[3].Name, "Node2");
			ZETestCheckEqual(*RootItemList[3].Pointer, 107);

			ZETestCheckEqual(Reader.GetCurrentPointer(), 107);
			ZETestCheckString(Reader.GetItemName(), "Node2");

			Reader.SeekPointer(46);
			ZETestCheckString(Reader.GetItemName(), "Node1");

			ReadPropList = Reader.ReadPropertyList(Node1ItemList, 3);
			ZETestCheck(ReadPropList);
			ZETestCheck(!Node1ItemList[0].IsFound);
			ZETestCheckString(Node1ItemList[0].Name, "Node1");
			ZETestCheck(Node1ItemList[1].IsFound);
			ZETestCheckString(Node1ItemList[1].Name, "Prop1");
			ZETestCheck(Node1ItemList[1].Value->GetBoolean());
			ZETestCheck(Node1ItemList[2].IsFound);
			ZETestCheckString(Node1ItemList[2].Name, "Prop2");
			ZETestCheck(!Node1ItemList[2].Value->GetBoolean());

			ZETestCheckEqual(Reader.GetCurrentPointer(), 107);
			ZETestCheckString(Reader.GetItemName(), "Prop2");
			Reader.GoToCurrentPointer();
			ZETestCheckEqual(Reader.GetCurrentPointer(), 107);
			ZETestCheckString(Reader.GetItemName(), "Node2");

			ReadPropList = Reader.ReadPropertyList(Node2ItemList, 2);
			ZETestCheck(ReadPropList);
			ZETestCheck(Node2ItemList[0].IsFound);
			ZETestCheckString(Node2ItemList[0].Name, "Prop3");
			ZETestCheckString(Node2ItemList[0].Value->GetString(), "test");
			ZETestCheck(Node2ItemList[1].IsFound);
			ZETestCheckString(Node2ItemList[1].Name, "Prop4");
			ZETestCheckEqual(Node2ItemList[1].Value->GetFloat(), 5.5f);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("the list includes a data property which doesn't exist in the properties of current node")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);
			Root.WriteDataProperty("DataProp1", Data, sizeof(unsigned char));
			Root.WriteProperty("Prop1", true);
			Root.WriteProperty("Prop2", 3.2f);
			Root.WriteProperty("Prop3", "test");
			Root.CloseNode();
			File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEMLSerialReader Reader(File);

			ZEMLSerialPointer Data1Pointer, Data2Pointer;
			ZEVariant Prop1, Prop2, Prop3;

			ZEMLSerialListItem ItemList[5] = {
				ZEML_LIST_DATA("DataProp1", Data1Pointer, true),
				ZEML_LIST_PROPERTY("Prop1", Prop1, ZE_VRT_BOOLEAN, true),
				ZEML_LIST_PROPERTY("Prop2", Prop2, ZE_VRT_FLOAT, true),
				ZEML_LIST_PROPERTY("Prop3", Prop3, ZE_VRT_STRING, true),
				ZEML_LIST_DATA("DataProp2", Data2Pointer, true)
			};

			ResultForRead = Reader.Read();
			ZETestCheckString(Reader.GetItemName(), "Root");

			ReadPropList = Reader.ReadPropertyList(ItemList, 5);
			//error List item not found. Item name : DataProp2
			ZETestCheck(!ReadPropList);
			ZETestCheck(ItemList[0].IsFound);
			ZETestCheckString(ItemList[0].Name, "DataProp1");
			ZETestCheckEqual(*ItemList[0].Pointer, 24);
			ZETestCheck(ItemList[1].IsFound);
			ZETestCheckString(ItemList[1].Name, "Prop1");
			ZETestCheck(ItemList[1].Value->GetBoolean());
			ZETestCheck(ItemList[2].IsFound);
			ZETestCheckString(ItemList[2].Name, "Prop2");
			ZETestCheckEqual(ItemList[2].Value->GetFloat(), 3.2f);
			ZETestCheck(ItemList[3].IsFound);
			ZETestCheckString(ItemList[3].Name, "Prop3");
			ZETestCheckString(ItemList[3].Value->GetString(), "test");
			ZETestCheck(!ItemList[4].IsFound);
			ZETestCheckString(ItemList[4].Name, "DataProp2");
			ZETestCheckEqual(*ItemList[4].Pointer, -1);

			File->Close();
			remove("SerialReaderTests.txt");
		}
		delete Data;
	}
}
