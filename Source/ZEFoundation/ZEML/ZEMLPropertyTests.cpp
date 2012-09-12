//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLPropertyTests.cpp
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

#include "ZEML/ZEMLProperty.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMLNode.h"
#include "ZEFile/ZEFile.h"


ZETestSuite(ZEMLProperty)
{
	ZETest("ZEMLProperty::ZEMLProperty()")
	{
		ZEMLProperty Property;

		ZETestCheckEqual(Property.GetType(), ZEML_IT_UNDEFINED);
		ZETestCheck(Property.GetName() == NULL);
	}

	ZETest("ZEMLProperty::ZEMLProperty(const ZEString& Name)")
	{
		ZEMLProperty Property("MLItem");

		ZETestCheckString(Property.GetName(), "MLItem");
		ZETestCheckEqual(Property.GetType(), ZEML_IT_UNDEFINED);
	}

	ZETest("ZEMLProperty::ZEMLProperty(const ZEString& Name, const ZEVariant& Value)")
	{
		ZEVariant Value;

		ZETestCase("variant type ZE_VRT_FLOAT")
		{
			Value.SetFloat(1.3f);
		
			ZEMLProperty PropertyFloat("ItemFloat", Value);
			ZETestCheckString(PropertyFloat.GetName(), "ItemFloat");
			ZETestCheckEqual(PropertyFloat.GetType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(PropertyFloat.GetDataSize(), 4);
			ZETestCheckEqual(PropertyFloat.GetValue().GetFloat(), 1.3f);
		}

		ZETestCase("variant type ZE_VRT_DOUBLE")
		{
			Value.SetDouble(1.3);

			ZEMLProperty PropertyDouble("ItemDouble", Value);
			ZETestCheckString(PropertyDouble.GetName(), "ItemDouble");
			ZETestCheckEqual(PropertyDouble.GetType(), ZEML_IT_DOUBLE);
			ZETestCheckEqual(PropertyDouble.GetDataSize(), 8);
			ZETestCheckClose(PropertyDouble.GetValue().GetDouble(), 1.3);
		}

		ZETestCase("variant type ZE_VRT_INTEGER_8")
		{
			Value.SetInt8(5);

			ZEMLProperty PropertyInt8("ItemInt8", Value);
			ZETestCheckString(PropertyInt8.GetName(), "ItemInt8");
			ZETestCheckEqual(PropertyInt8.GetType(), ZEML_IT_INT8);
			ZETestCheckEqual(PropertyInt8.GetDataSize(), 1);
			ZETestCheckEqual(PropertyInt8.GetValue().GetInt8(), 5);
		}

		ZETestCase("variant type ZE_VRT_INTEGER_16")
		{
			Value.SetInt16(30);

			ZEMLProperty PropertyInt16("ItemInt16", Value);
			ZETestCheckString(PropertyInt16.GetName(), "ItemInt16");
			ZETestCheckEqual(PropertyInt16.GetType(), ZEML_IT_INT16);
			ZETestCheckEqual(PropertyInt16.GetDataSize(), 2);
			ZETestCheckEqual(PropertyInt16.GetValue().GetInt16(), 30);
		}

		ZETestCase("variant type ZE_VRT_INTEGER_32")
		{
			Value.SetInt32(0);

			ZEMLProperty PropertyInt32("ItemInt32", Value);
			ZETestCheckString(PropertyInt32.GetName(), "ItemInt32");
			ZETestCheckEqual(PropertyInt32.GetType(), ZEML_IT_INT32);
			ZETestCheckEqual(PropertyInt32.GetDataSize(), 4);
			ZETestCheckEqual(PropertyInt32.GetValue().GetInt32(), 0);
		}

		ZETestCase("variant type ZE_VRT_INTEGER_64")
		{
			Value.SetInt64(40);

			ZEMLProperty PropertyInt64("ItemInt64", Value);
			ZETestCheckString(PropertyInt64.GetName(), "ItemInt64");
			ZETestCheckEqual(PropertyInt64.GetType(), ZEML_IT_INT64);
			ZETestCheckEqual(PropertyInt64.GetDataSize(), 8);
			ZETestCheckEqual(PropertyInt64.GetValue().GetInt64(), 40);
		}

		ZETestCase("variant type ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Value.SetUInt8(200);

			ZEMLProperty PropertyUInt8("ItemUInt8", Value);
			ZETestCheckString(PropertyUInt8.GetName(), "ItemUInt8");
			ZETestCheckEqual(PropertyUInt8.GetType(), ZEML_IT_UINT8);
			ZETestCheckEqual(PropertyUInt8.GetDataSize(), 1);
			ZETestCheckEqual(PropertyUInt8.GetValue().GetUInt8(), 200);
		}

		ZETestCase("variant type ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Value.SetUInt16(640);

			ZEMLProperty PropertyUInt16("ItemUInt16", Value);
			ZETestCheckString(PropertyUInt16.GetName(), "ItemUInt16");
			ZETestCheckEqual(PropertyUInt16.GetType(), ZEML_IT_UINT16);
			ZETestCheckEqual(PropertyUInt16.GetDataSize(), 2);
			ZETestCheckEqual(PropertyUInt16.GetValue().GetUInt16(), 640);
		}

		ZETestCase("variant type ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Value.SetUInt32(43000);

			ZEMLProperty PropertyUInt32("ItemUInt32", Value);
			ZETestCheckString(PropertyUInt32.GetName(), "ItemUInt32");
			ZETestCheckEqual(PropertyUInt32.GetType(), ZEML_IT_UINT32);
			ZETestCheckEqual(PropertyUInt32.GetDataSize(), 4);
			ZETestCheckEqual(PropertyUInt32.GetValue().GetUInt32(), 43000);
		}

		ZETestCase("variant type ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Value.SetUInt64(2000000000);

			ZEMLProperty PropertyUInt64("ItemUInt64", Value);
			ZETestCheckString(PropertyUInt64.GetName(), "ItemUInt64");
			ZETestCheckEqual(PropertyUInt64.GetType(), ZEML_IT_UINT64);
			ZETestCheckEqual(PropertyUInt64.GetDataSize(), 8);
			ZETestCheckEqual(PropertyUInt64.GetValue().GetUInt64(), 2000000000);
		}

		ZETestCase("variant type ZE_VRT_BOOLEAN")
		{
			Value.SetBoolean(false);

			ZEMLProperty PropertyBoolean("ItemBoolean", Value);
			ZETestCheckString(PropertyBoolean.GetName(), "ItemBoolean");
			ZETestCheckEqual(PropertyBoolean.GetType(), ZEML_IT_BOOLEAN);
			ZETestCheckEqual(PropertyBoolean.GetDataSize(), 1);
			ZETestCheck(!PropertyBoolean.GetValue().GetBoolean());
		}

		ZETestCase("variant type ZE_VRT_STRING")
		{
			Value.SetString("String");

			ZEMLProperty PropertyString("ItemString", Value);
			ZETestCheckString(PropertyString.GetName(), "ItemString");
			ZETestCheckEqual(PropertyString.GetType(), ZEML_IT_STRING);
			ZETestCheckEqual(PropertyString.GetDataSize(), 7);
			ZETestCheckString(PropertyString.GetValue().GetString(), "String");
		}

		ZETestCase("variant type ZE_VRT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			ZEMLProperty PropertyQuaternion("ItemQuaternion", Value);
			ZETestCheckString(PropertyQuaternion.GetName(), "ItemQuaternion");
			ZETestCheckEqual(PropertyQuaternion.GetType(), ZEML_IT_QUATERNION);
			ZETestCheckEqual(PropertyQuaternion.GetDataSize(), 16);
			ZETestCheckEqual(PropertyQuaternion.GetValue().GetQuaternion(), ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));
		}

		ZETestCase("variant type ZE_VRT_VECTOR2")
		{
			Value.SetVector2(ZEVector2(3.0f, 5.0f));

			ZEMLProperty PropertyVector2("ItemVector2", Value);
			ZETestCheckString(PropertyVector2.GetName(), "ItemVector2");
			ZETestCheckEqual(PropertyVector2.GetType(), ZEML_IT_VECTOR2);
			ZETestCheckEqual(PropertyVector2.GetDataSize(), 8);
			ZETestCheckEqual(PropertyVector2.GetValue().GetVector2(), ZEVector2(3.0f, 5.0f));
		}

		ZETestCase("variant type ZE_VRT_VECTOR3")
		{
			Value.SetVector3(ZEVector3(1.0f, 3.0f, 5.0f));

			ZEMLProperty PropertyVector3("ItemVector3", Value);
			ZETestCheckString(PropertyVector3.GetName(), "ItemVector3");
			ZETestCheckEqual(PropertyVector3.GetType(), ZEML_IT_VECTOR3);
			ZETestCheckEqual(PropertyVector3.GetDataSize(), 12);
			ZETestCheckEqual(PropertyVector3.GetValue().GetVector3(), ZEVector3(1.0f, 3.0f, 5.0f));
		}

		ZETestCase("variant type ZE_VRT_VECTOR4")
		{
			Value.SetVector4(ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));

			ZEMLProperty PropertyVector4("ItemVector4", Value);
			ZETestCheckString(PropertyVector4.GetName(), "ItemVector4");
			ZETestCheckEqual(PropertyVector4.GetType(), ZEML_IT_VECTOR4);
			ZETestCheckEqual(PropertyVector4.GetDataSize(), 16);
			ZETestCheckEqual(PropertyVector4.GetValue().GetVector4(), ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));
		}

		ZETestCase("variant type ZE_VRT_MATRIX3X3")
		{
			ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);
			Value.SetMatrix3x3(Matrix);

			ZEMLProperty PropertyMatrix3x3("ItemMatrix3x3", Value);
			ZETestCheckString(PropertyMatrix3x3.GetName(), "ItemMatrix3x3");
			ZETestCheckEqual(PropertyMatrix3x3.GetType(), ZEML_IT_MATRIX3X3);
			ZETestCheckEqual(PropertyMatrix3x3.GetDataSize(), 36);
			ZETestCheckEqual(PropertyMatrix3x3.GetValue().GetMatrix3x3(), Matrix);
		}

		ZETestCase("variant type ZE_VRT_MATRIX4X4")
		{
			ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
				5.0f, 8.0f, 6.0f, 9.0f,   
				13.0f, 3.0f, 4.0f, 5.0f,   
				12.0f, 2.0f, 8.0f, 6.0f);
			Value.SetMatrix4x4(Matrix);

			ZEMLProperty PropertyMatrix4x4("ItemMatrix4x4", Value);
			ZETestCheckString(PropertyMatrix4x4.GetName(), "ItemMatrix4x4");
			ZETestCheckEqual(PropertyMatrix4x4.GetType(), ZEML_IT_MATRIX4X4);
			ZETestCheckEqual(PropertyMatrix4x4.GetDataSize(), 64);
			ZETestCheckEqual(PropertyMatrix4x4.GetValue().GetMatrix4x4(), Matrix);
		}

		ZETestCase("variant type ZE_VRT_NULL")
		{
			Value.SetNull();

			ZEMLProperty PropertyNull("ItemNull", Value);
			ZETestCheckString(PropertyNull.GetName(), "ItemNull");
			ZETestCheckEqual(PropertyNull.GetDataSize(), 0);
			//error Unsupported ZEMLProperty type.
		}
	}

	ZETest("ZEUInt64 ZEMLProperty::GetTotalSize()")
	{
		ZEVariant Value;
		ZEUInt64 DataSize;
		ZEUInt64 TotalSize;

		ZETestCase("ZEML_IT_FLOAT")
		{
			Value.SetFloat(1.3f);

			ZEMLProperty PFloat("Float", Value);
			DataSize = PFloat.GetDataSize();
			ZETestCheckEqual(DataSize, 4);

			TotalSize = PFloat.GetTotalSize();
			ZETestCheckEqual(TotalSize, 21);
		}

		ZETestCase("ZEML_IT_DOUBLE")
		{
			Value.SetDouble(1.3);

			ZEMLProperty PDouble("Double", Value);
			DataSize = PDouble.GetDataSize();
			ZETestCheckEqual(DataSize, 8);

			TotalSize = PDouble.GetTotalSize();
			ZETestCheckEqual(TotalSize, 26);
		}

		ZETestCase("ZEML_IT_INT8")
		{
			Value.SetInt8(5);

			ZEMLProperty PInt8("Int8", Value);
			DataSize = PInt8.GetDataSize();
			ZETestCheckEqual(DataSize, 1);

			TotalSize = PInt8.GetTotalSize();
			ZETestCheckEqual(TotalSize, 17);
		}

		ZETestCase("ZEML_IT_INT16")
		{
			Value.SetInt16(30);

			ZEMLProperty PInt16("Int16", Value);
			DataSize = PInt16.GetDataSize();
			ZETestCheckEqual(DataSize, 2);

			TotalSize = PInt16.GetTotalSize();
			ZETestCheckEqual(TotalSize, 19);
		}

		ZETestCase("ZEML_IT_INT32")
		{
			Value.SetInt32(0);

			ZEMLProperty PInt32("Int32", Value);
			DataSize = PInt32.GetDataSize();
			ZETestCheckEqual(DataSize, 4);

			TotalSize = PInt32.GetTotalSize();
			ZETestCheckEqual(TotalSize, 21);
		}

		ZETestCase("ZEML_IT_INT64")
		{
			Value.SetInt64(40);

			ZEMLProperty PInt64("Int64", Value);
			DataSize = PInt64.GetDataSize();
			ZETestCheckEqual(DataSize, 8);

			TotalSize = PInt64.GetTotalSize();
			ZETestCheckEqual(TotalSize, 25);
		}

		ZETestCase("ZEML_IT_UINT8")
		{
			Value.SetUInt8(200);

			ZEMLProperty PUInt8("UInt8", Value);
			DataSize = PUInt8.GetDataSize();
			ZETestCheckEqual(DataSize, 1);

			TotalSize = PUInt8.GetTotalSize();
			ZETestCheckEqual(TotalSize, 18);
		}

		ZETestCase("ZEML_IT_UINT16")
		{
			Value.SetUInt16(6400);

			ZEMLProperty PUInt16("UInt16", Value);
			DataSize = PUInt16.GetDataSize();
			ZETestCheckEqual(DataSize, 2);

			TotalSize = PUInt16.GetTotalSize();
			ZETestCheckEqual(TotalSize, 20);
		}

		ZETestCase("ZEML_IT_UINT32")
		{
			Value.SetUInt32(43000);

			ZEMLProperty PUInt32("UInt32", Value);
			DataSize = PUInt32.GetDataSize();
			ZETestCheckEqual(DataSize, 4);

			TotalSize = PUInt32.GetTotalSize();
			ZETestCheckEqual(TotalSize, 22);
		}

		ZETestCase("ZEML_IT_UINT64")
		{
			Value.SetUInt64(2000000000);

			ZEMLProperty PUInt64("UInt64", Value);
			DataSize = PUInt64.GetDataSize();
			ZETestCheckEqual(DataSize, 8);

			TotalSize = PUInt64.GetTotalSize();
			ZETestCheckEqual(TotalSize, 26);
		}

		ZETestCase("ZEML_IT_BOOLEAN")
		{
			Value.SetBoolean(true);

			ZEMLProperty PBoolean("Boolean", Value);
			DataSize = PBoolean.GetDataSize();
			ZETestCheckEqual(DataSize, 1);

			TotalSize = PBoolean.GetTotalSize();
			ZETestCheckEqual(TotalSize, 20);
		}

		ZETestCase("ZEML_IT_STRING")
		{
			Value.SetString("String");

			ZEMLProperty PString("String", Value);
			DataSize = PString.GetDataSize();
			ZETestCheckEqual(DataSize, 7);

			TotalSize = PString.GetTotalSize();
			ZETestCheckEqual(TotalSize, 25);
		}

		ZETestCase("ZEML_IT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			ZEMLProperty PQuaternion("Quaternion", Value);
			DataSize = PQuaternion.GetDataSize();
			ZETestCheckEqual(DataSize, 16);

			TotalSize = PQuaternion.GetTotalSize();
			ZETestCheckEqual(TotalSize, 38);
		}

		ZETestCase("ZEML_IT_VECTOR2")
		{
			Value.SetVector2(ZEVector2(3.0f, 5.0f));

			ZEMLProperty PVector2("Vector2", Value);
			DataSize = PVector2.GetDataSize();
			ZETestCheckEqual(DataSize, 8);

			TotalSize = PVector2.GetTotalSize();
			ZETestCheckEqual(TotalSize, 27);
		}

		ZETestCase("ZEML_IT_VECTOR3")
		{
			Value.SetVector3(ZEVector3(1.0f, 3.0f, 5.0f));

			ZEMLProperty PVector3("Vector3", Value);
			DataSize = PVector3.GetDataSize();
			ZETestCheckEqual(DataSize, 12);

			TotalSize = PVector3.GetTotalSize();
			ZETestCheckEqual(TotalSize, 31);
		}

		ZETestCase("ZEML_IT_VECTOR4")
		{
			Value.SetVector4(ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));

			ZEMLProperty PVector4("Vector4", Value);
			DataSize = PVector4.GetDataSize();
			ZETestCheckEqual(DataSize, 16);

			TotalSize = PVector4.GetTotalSize();
			ZETestCheckEqual(TotalSize, 35);
		}

		ZETestCase("ZEML_IT_MATRIX3X3")
		{
			Value.SetMatrix3x3(ZEMatrix3x3::Identity);

			ZEMLProperty PMatrix3x3("Matrix3x3", Value);
			DataSize = PMatrix3x3.GetDataSize();
			ZETestCheckEqual(DataSize, 36);

			TotalSize = PMatrix3x3.GetTotalSize();
			ZETestCheckEqual(TotalSize, 57);
		}

		ZETestCase("ZEML_IT_MATRIX4X4")
		{
			Value.SetMatrix4x4(ZEMatrix4x4::Zero);

			ZEMLProperty PMatrix4x4("Matrix4x4", Value);
			DataSize = PMatrix4x4.GetDataSize();
			ZETestCheckEqual(DataSize, 64);

			TotalSize = PMatrix4x4.GetTotalSize();
			ZETestCheckEqual(TotalSize, 85);
		}
	}

	ZETest("void ZEMLProperty::SetValue(const ZEVariant& Value)")
	{
		ZEVariant Value;
		ZEMLProperty Property;
		ZETestCheckEqual(Property.GetType(), ZEML_IT_UNDEFINED);

		ZETestCase("ZE_VRT_FLOAT")
		{
			Value.SetFloat(1.3f);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_FLOAT);
			ZETestCheckEqual(Property.GetDataSize(), 4);
			ZETestCheckEqual(Property.GetValue().GetFloat(), Value.GetFloat());
		}

		ZETestCase("ZE_VRT_DOUBLE")
		{
			Value.SetDouble(1.3);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_DOUBLE);
			ZETestCheckEqual(Property.GetDataSize(), 8);
			ZETestCheckEqual(Property.GetValue().GetDouble(), Value.GetDouble());
		}

		ZETestCase("ZE_VRT_INTEGER_8")
		{
			Value.SetInt8(5);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_INT8);
			ZETestCheckEqual(Property.GetDataSize(), 1);
			ZETestCheckEqual(Property.GetValue().GetInt8(), Value.GetInt8());
		}

		ZETestCase("ZE_VRT_INTEGER_16")
		{
			Value.SetInt16(30);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_INT16);
			ZETestCheckEqual(Property.GetDataSize(), 2);
			ZETestCheckEqual(Property.GetValue().GetInt16(), Value.GetInt16());
		}

		ZETestCase("ZE_VRT_INTEGER_32")
		{
			Value.SetInt32(0);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_INT32);
			ZETestCheckEqual(Property.GetDataSize(), 4);
			ZETestCheckEqual(Property.GetValue().GetInt32(), Value.GetInt32());
		}

		ZETestCase("ZE_VRT_INTEGER_64")
		{
			Value.SetInt64(40);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_INT64);
			ZETestCheckEqual(Property.GetDataSize(), 8);
			ZETestCheckEqual(Property.GetValue().GetInt64(), Value.GetInt64());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Value.SetUInt8(200);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_UINT8);
			ZETestCheckEqual(Property.GetDataSize(), 1);
			ZETestCheckEqual(Property.GetValue().GetUInt8(), Value.GetUInt8());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Value.SetUInt16(6400);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_UINT16);
			ZETestCheckEqual(Property.GetDataSize(), 2);
			ZETestCheckEqual(Property.GetValue().GetUInt16(), Value.GetUInt16());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Value.SetUInt32(43000);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_UINT32);
			ZETestCheckEqual(Property.GetDataSize(), 4);
			ZETestCheckEqual(Property.GetValue().GetUInt32(), Value.GetUInt32());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Value.SetUInt64(2000000000);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_UINT64);
			ZETestCheckEqual(Property.GetDataSize(), 8);
			ZETestCheckEqual(Property.GetValue().GetUInt64(), Value.GetUInt64());
		}

		ZETestCase("ZE_VRT_BOOLEAN")
		{
			Value.SetBoolean(true);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_BOOLEAN);
			ZETestCheckEqual(Property.GetDataSize(), 1);
			ZETestCheckEqual(Property.GetValue().GetBoolean(), Value.GetBoolean());
		}

		ZETestCase("ZE_VRT_STRING")
		{
			Value.SetString("Test");

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_STRING);
			ZETestCheckEqual(Property.GetDataSize(), 5);
			ZETestCheckString(Property.GetValue().GetString(), Value.GetString());
		}

		ZETestCase("ZE_VRT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion::Identity);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_QUATERNION);
			ZETestCheckEqual(Property.GetDataSize(), 16);
			ZETestCheckEqual(Property.GetValue().GetQuaternion(), Value.GetQuaternion());
		}

		ZETestCase("ZE_VRT_VECTOR2")
		{
			Value.SetVector2(ZEVector2::Zero);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_VECTOR2);
			ZETestCheckEqual(Property.GetDataSize(), 8);
			ZETestCheckEqual(Property.GetValue().GetVector2(), Value.GetVector2());
		}

		ZETestCase("ZE_VRT_VECTOR3")
		{
			Value.SetVector3(ZEVector3::One);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_VECTOR3);
			ZETestCheckEqual(Property.GetDataSize(), 12);
			ZETestCheckEqual(Property.GetValue().GetVector3(), Value.GetVector3());
		}

		ZETestCase("ZE_VRT_VECTOR4")
		{
			Value.SetVector4(ZEVector4::Zero);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_VECTOR4);
			ZETestCheckEqual(Property.GetDataSize(), 16);
			ZETestCheckEqual(Property.GetValue().GetVector4(), Value.GetVector4());
		}

		ZETestCase("ZE_VRT_MATRIX3X3")
		{
			Value.SetMatrix3x3(ZEMatrix3x3::Identity);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_MATRIX3X3);
			ZETestCheckEqual(Property.GetDataSize(), 36);
			ZETestCheckEqual(Property.GetValue().GetMatrix3x3(), Value.GetMatrix3x3());
		}

		ZETestCase("ZE_VRT_MATRIX4X4")
		{
			Value.SetMatrix4x4(ZEMatrix4x4::Zero);

			Property.SetValue(Value);
			ZETestCheckEqual(Property.GetType(), ZEML_IT_MATRIX4X4);
			ZETestCheckEqual(Property.GetDataSize(), 64);
			ZETestCheckEqual(Property.GetValue().GetMatrix4x4(), Value.GetMatrix4x4());
		}
	}

	ZETest("const ZEVariant& ZEMLProperty::GetValue() const")
	{
		ZEVariant Variant;
		ZEVariant Value;
		ZEMLProperty Property;

		ZETestCase("for ZEMLProperty::ZEMLProperty()")
		{
			ZEMLProperty TestProperty;
			ZETestCheckEqual(TestProperty.GetType(), ZEML_IT_UNDEFINED);

			Value = TestProperty.GetValue();
			ZETestCheckEqual(Value.GetType(), ZE_VRT_UNDEFINED);
		}

		ZETestCase("for ZEMLProperty::ZEMLProperty(const ZEString& Name)")
		{
			ZEMLProperty TestProperty("TestProperty");
			ZETestCheckEqual(TestProperty.GetType(), ZEML_IT_UNDEFINED);
			ZETestCheckString(TestProperty.GetName(), "TestProperty");

			Value = TestProperty.GetValue();
			ZETestCheckEqual(Value.GetType(), ZE_VRT_UNDEFINED);
		}

		ZETestCase("for ZEMLProperty::ZEMLProperty(const ZEString& Name ,const ZEVariant& Value)")
		{	
			Variant.SetBoolean(false);
			ZEMLProperty TestProperty("TestProperty", Variant);
			ZETestCheckEqual(TestProperty.GetType(), ZEML_IT_BOOLEAN);

			Value = TestProperty.GetValue();
			ZETestCheckEqual(Value.GetBoolean(), Variant.GetBoolean());		
			ZETestCheckEqual(Value.GetType(), ZE_VRT_BOOLEAN);
		}

		ZETestCase("ZEML_IT_FLOAT")
		{
			Variant.SetFloat(5.0f);
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZETestCheckEqual(Value.GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Value.GetFloat(), Property.GetValue().GetFloat());
			ZETestCheckEqual(Value.GetFloat(), Variant.GetFloat());
		}

		ZETestCase("ZEML_IT_DOUBLE")
		{
			Variant.SetDouble(5.0);
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZETestCheckEqual(Value.GetType(), ZE_VRT_DOUBLE);
			ZETestCheckClose(Value.GetDouble(), Property.GetValue().GetDouble());
			ZETestCheckClose(Value.GetDouble(), Variant.GetDouble());
		}

		ZETestCase("ZEML_IT_STRING")
		{
			Variant.SetString("TestString");
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZETestCheckEqual(Value.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Value.GetString(), Property.GetValue().GetString());
			ZETestCheckString(Value.GetString(), Variant.GetString());
		}
	}
}
