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
		//Reader.NextItemPosition: 0

		File->Close();
		remove("SerialReaderTests.txt");
	}

	ZETest("bool ZEMLSerialReader::Read()")
	{
		ZEFile* File = new ZEFile();
		bool Read;

		ZETestCase("for an empty file")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialReader Reader(File);

			Read = Reader.Read();
			ZETestCheck(!Read);
			ZETestCheck(Reader.GetItemName() == "");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UNDEFINED);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 0

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

			Read = Reader.Read();
			ZETestCheck(Read);
			//error Corrupted ZEML file. Corrupted ZEML file.
			//error Cannot read ZEMLItem name from file. Corrupted ZEML file.
			//error Cannot read ZEMLProperty value size from file. Corrupted ZEML file.
			//Cannot read ZEMLProperty value from file. Corrupted ZEML file.

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write to ZML file with serial writer for bool typed variant")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			ZEVariant Value;
			Value.SetBoolean(true);
			Node.WriteProperty("Prop", Value);
			Node.CloseNode();
			RootNode.CloseNode();
			ZEMLSerialReader Reader(File);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 41);
			//Reader.NextItemPosition: 28

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 17);
			//Reader.NextItemPosition: 52

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "Prop");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_BOOLEAN);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_BOOLEAN);
			ZETestCheck(Reader.GetItemValue().GetBoolean());
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 69
	
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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "RootNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 69);
			//Reader.NextItemPosition: 28

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "Node");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 2);
			ZETestCheckEqual(Reader.GetDataSize(), 45);
			//Reader.NextItemPosition: 52

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "Prop1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheckString(Reader.GetItemValue().GetString(), "TestString");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "Prop2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheck(Reader.GetItemValue().GetString() == "");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropFloat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetFloat(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropDouble");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_DOUBLE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_DOUBLE);
			ZETestCheckEqual(Reader.GetItemValue().GetDouble(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheck(Read);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetInt8(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetInt16(), Value);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 6);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt32");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT32);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_32);
			ZETestCheckEqual(Reader.GetItemValue().GetInt32(), Value32);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt64");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT64);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_64);
			ZETestCheckEqual(Reader.GetItemValue().GetInt64(), Value64);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropUInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt8(), ValueU8);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropUInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt16(), ValueU16);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropUInt32");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT32);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt32(), ValueU32);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropUInt64");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_UINT64);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZETestCheckEqual(Reader.GetItemValue().GetUInt64(), ValueU64);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(!Read);

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 6);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropQuat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_QUATERNION);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_QUATERNION);
			ZETestCheckEqual(Reader.GetItemValue().GetQuaternion(), Quat);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec2");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR2);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR2);
			ZETestCheckEqual(Reader.GetItemValue().GetVector2(), Vec2);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec3");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR3);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR3);
			ZETestCheckEqual(Reader.GetItemValue().GetVector3(), Vec3);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropVec4");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_VECTOR4);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_VECTOR4);
			ZETestCheckEqual(Reader.GetItemValue().GetVector4(), Vec4);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropMat3x3");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_MATRIX3X3);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_MATRIX3X3);
			ZETestCheckEqual(Reader.GetItemValue().GetMatrix3x3(), Mat3x3);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "PropMat4x4");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_MATRIX4X4);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_MATRIX4X4);
			ZETestCheckEqual(Reader.GetItemValue().GetMatrix4x4(), Mat4x4);
			ZETestCheckEqual(Reader.GetDataSize(), 0);

			Read = Reader.Read();
			ZETestCheck(!Read);

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
			ZEMLSerialReader Reader(File);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "ParentNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 4);
			ZETestCheckEqual(Reader.GetDataSize(), 98);
			//Reader.NextItemPosition: 30

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropFloat");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Reader.GetItemValue().GetFloat(), 2.0f);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 55

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt8");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Reader.GetItemValue().GetInt8(), 'z');
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 76

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropInt16");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT16);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_INTEGER_16);
			ZETestCheckEqual(Reader.GetItemValue().GetInt16(), 300);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 99

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "PropString");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_STRING);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_STRING);
			ZETestCheckString(Reader.GetItemValue().GetString(), "String");
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), 0);
			//Reader.NextItemPosition: 128

			Read = Reader.Read();
			ZETestCheck(!Read);

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
			ZEMLSerialReader Reader(File);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "ParentNode");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 1);
			ZETestCheckEqual(Reader.GetDataSize(), 31);
			ZETestCheck(!Reader.GetData(Data, sizeof(unsigned char), 0));
			//Reader.NextItemPosition: 30

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckString(Reader.GetItemName(), "DataProp1");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckEqual(Reader.GetDataSize(), sizeof(unsigned char) * 10);
			ZETestCheck(Reader.GetData(Data, Reader.GetDataSize(), 0));
			//Reader.NextItemPosition: 52

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

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheck(!Reader.GetData(Buffer, sizeof(unsigned char), 0));

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetSubItemCount(), 0);
			ZETestCheckString(Reader.GetItemName(), "DataProp");
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			ZETestCheckEqual(Reader.GetItemValue().GetType(), ZE_VRT_UNDEFINED);
			ZETestCheckEqual(Reader.GetDataSize(), 1);
			ZETestCheck(Reader.GetData(Buffer, sizeof(unsigned char), 0));
			ZEInt Compare = memcmp(Data, Buffer, sizeof(unsigned char));
			ZETestCheckEqual(Compare, 0);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
			delete Buffer;
		}
	}

	ZETest("ZEMLItemType ZEMLSerialReader::GetItemType()")
	{
		ZEFile* File = new ZEFile();
		bool Read;
		ZEMLItemType ItemType;

		ZETestCase("ZEML_IT_UNDEFINED")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(!Read);

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
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);

			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			Read = Reader.Read();
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
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);

			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			Read = Reader.Read();
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_NODE);

			Read = Reader.Read();
			ItemType = Reader.GetItemType();
			ZETestCheckEqual(ItemType, ZEML_IT_INLINE_DATA);

			Read = Reader.Read();
			ZETestCheck(!Read);
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
		bool Read;
		ZEString ItemName;

		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialReader Reader(File);
		Read = Reader.Read();
		ZETestCheck(!Read);

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
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);

			ItemName = Reader.GetItemName();
			ZETestCheckString(ItemName, "RootNode");

			File->Close();
			remove("SerialReaderTests.txt");
		}
	}

	ZETest("ZEUInt64 ZEMLSerialReader::GetSubItemCount()")
	{
		ZEFile* File = new ZEFile();
		bool Read;
		ZEUInt64 SubItemCount;

		File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialReader Reader(File);
		Read = Reader.Read();
		ZETestCheck(!Read);

		SubItemCount = Reader.GetSubItemCount();
		ZETestCheckEqual(SubItemCount, 0);

		File->Close();
		remove("SerialReaderTests.txt");

		ZETestCase("Reader has sub item")
		{
			ZEVariant Value;
			Value.SetFloat(2.0f);
			ZEMLNode Node("ParentNode");
			Node.AddProperty("PropFloat", Value);
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);

			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 1);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("write data and data property to serial node")
		{
			void* Data = new char[sizeof(unsigned char)];
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEMLSerialNode Node = RootNode.OpenNode("Node");
			Node.WriteProperty("Prop", 1.2f);
			Node.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);

			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(!Read);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}

		ZETestCase("write data and data property to node")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			Node.AddProperty("Prop", 1.2f);
			Node.AddDataProperty("DataProp", Data, sizeof(unsigned char));
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 2);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_FLOAT);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(Read);
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INLINE_DATA);
			SubItemCount = Reader.GetSubItemCount();
			ZETestCheckEqual(SubItemCount, 0);

			Read = Reader.Read();
			ZETestCheck(!Read);

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
		ZEMLSerialReader Reader(File);
		bool Read = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

		ZEVariant ItemValue = Reader.GetItemValue();
		ZETestCheckEqual(ItemValue.GetType(), ZE_VRT_UNDEFINED);

		Read = Reader.Read();
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
		ZEMLSerialReader Reader(File);
		bool Read = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

		ZEUInt64 DataSize = Reader.GetDataSize();
		ZETestCheckEqual(DataSize, 0);

		Read = Reader.Read();
		ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
		DataSize = Reader.GetDataSize();
		ZETestCheckEqual(DataSize, 0);

		File->Close();
		remove("SerialReaderTests.txt");

		ZETestCase("for ItemType ZEML_IT_INLINE_DATA")
		{
			ZESize DataCount = 10;
			void* Data = new unsigned char[DataCount];

			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLNode Node("ParentNode");
			Node.AddDataProperty("DataProp", Data, sizeof(unsigned char) * DataCount, false);
			Node.Write(File);
			ZEMLSerialReader TestReader(File);
			Read = TestReader.Read();
			ZETestCheckEqual(TestReader.GetItemType(), ZEML_IT_NODE);

			DataSize = TestReader.GetDataSize();
			ZETestCheckEqual(DataSize, 30);

			Read = TestReader.Read();
			ZETestCheckEqual(TestReader.GetItemType(), ZEML_IT_INLINE_DATA);
			DataSize = TestReader.GetDataSize();
			ZETestCheckEqual(DataSize, sizeof(unsigned char) * DataCount);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}
	}

	ZETest("bool ZEMLSerialReader::GetData(void* Buffer, ZEUInt64 BufferSize, ZEUInt64 Offset)")
	{
		ZEFile* File = new ZEFile();
		unsigned char* Buffer = new unsigned char[1024];
		bool Read;
		bool Get;

		ZETestCase("for CurrentItemType of ZEMLSerialReader != ZEML_IT_INLINE_DATA")
		{
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode RootNode("RootNode", File);
			ZEInt8 Value = 'a';
			RootNode.WriteProperty("Prop", Value);
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(!Get);

			Read = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_INT8);
			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(!Get);

			File->Close();
			remove("SerialReaderTests.txt");
		}

		ZETestCase("for CurrentItemType = ZEML_IT_INLINE_DATA")
		{
			void* Data = new char[sizeof(unsigned char)];
			ZEMLNode Node("ParentNode");
			Node.AddDataProperty("DataProp1", Data, sizeof(unsigned char), false);
			File->Open("SerialReaderTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			Node.Write(File);
			ZEMLSerialReader Reader(File);
			Read = Reader.Read();
			ZETestCheckEqual(Reader.GetItemType(), ZEML_IT_NODE);

			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(!Get);

			Read = Reader.Read();
			Get = Reader.GetData(Buffer, sizeof(unsigned char), 0);
			ZETestCheck(Get);

			File->Close();
			remove("SerialReaderTests.txt");
			delete Data;
		}
	}
}
