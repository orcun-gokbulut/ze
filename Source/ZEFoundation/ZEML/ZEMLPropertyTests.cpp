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


ZE_TEST(ZEMLProperty)
{
	ZE_TEST_ITEM("ZEMLProperty::ZEMLProperty()")
	{
		ZEMLProperty Property;

		ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UNDEFINED);
		ZE_TEST_CHECK_ERROR(Property.GetName() == NULL);
	}

	ZE_TEST_ITEM("ZEMLProperty::ZEMLProperty(const ZEString& Name)")
	{
		ZEMLProperty Property("MLItem");

		ZE_TEST_CHECK_STRING_EQUAL(Property.GetName(), "MLItem");
		ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UNDEFINED);
	}

	ZE_TEST_ITEM("ZEMLProperty::ZEMLProperty(const ZEString& Name, const ZEVariant& Value)")
	{
		ZEVariant Value;

		ZE_TEST_CASE("variant type ZE_VRT_FLOAT")
		{
			Value.SetFloat(1.3f);
		
			ZEMLProperty PropertyFloat("ItemFloat", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyFloat.GetName(), "ItemFloat");
			ZE_TEST_CHECK_EQUAL(PropertyFloat.GetType(), ZEML_IT_FLOAT);
			ZE_TEST_CHECK_EQUAL(PropertyFloat.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(PropertyFloat.GetValue().GetFloat(), 1.3f);
		}

		ZE_TEST_CASE("variant type ZE_VRT_DOUBLE")
		{
			Value.SetDouble(1.3);

			ZEMLProperty PropertyDouble("ItemDouble", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyDouble.GetName(), "ItemDouble");
			ZE_TEST_CHECK_EQUAL(PropertyDouble.GetType(), ZEML_IT_DOUBLE);
			ZE_TEST_CHECK_EQUAL(PropertyDouble.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(PropertyDouble.GetValue().GetDouble(), 1.3);
		}

		ZE_TEST_CASE("variant type ZE_VRT_INTEGER_8")
		{
			Value.SetInt8(5);

			ZEMLProperty PropertyInt8("ItemInt8", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyInt8.GetName(), "ItemInt8");
			ZE_TEST_CHECK_EQUAL(PropertyInt8.GetType(), ZEML_IT_INT8);
			ZE_TEST_CHECK_EQUAL(PropertyInt8.GetDataSize(), 1);
			ZE_TEST_CHECK_EQUAL(PropertyInt8.GetValue().GetInt8(), 5);
		}

		ZE_TEST_CASE("variant type ZE_VRT_INTEGER_16")
		{
			Value.SetInt16(30);

			ZEMLProperty PropertyInt16("ItemInt16", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyInt16.GetName(), "ItemInt16");
			ZE_TEST_CHECK_EQUAL(PropertyInt16.GetType(), ZEML_IT_INT16);
			ZE_TEST_CHECK_EQUAL(PropertyInt16.GetDataSize(), 2);
			ZE_TEST_CHECK_EQUAL(PropertyInt16.GetValue().GetInt16(), 30);
		}

		ZE_TEST_CASE("variant type ZE_VRT_INTEGER_32")
		{
			Value.SetInt32(0);

			ZEMLProperty PropertyInt32("ItemInt32", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyInt32.GetName(), "ItemInt32");
			ZE_TEST_CHECK_EQUAL(PropertyInt32.GetType(), ZEML_IT_INT32);
			ZE_TEST_CHECK_EQUAL(PropertyInt32.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(PropertyInt32.GetValue().GetInt32(), 0);
		}

		ZE_TEST_CASE("variant type ZE_VRT_INTEGER_64")
		{
			Value.SetInt64(40);

			ZEMLProperty PropertyInt64("ItemInt64", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyInt64.GetName(), "ItemInt64");
			ZE_TEST_CHECK_EQUAL(PropertyInt64.GetType(), ZEML_IT_INT64);
			ZE_TEST_CHECK_EQUAL(PropertyInt64.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(PropertyInt64.GetValue().GetInt64(), 40);
		}

		ZE_TEST_CASE("variant type ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Value.SetUInt8(200);

			ZEMLProperty PropertyUInt8("ItemUInt8", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyUInt8.GetName(), "ItemUInt8");
			ZE_TEST_CHECK_EQUAL(PropertyUInt8.GetType(), ZEML_IT_UINT8);
			ZE_TEST_CHECK_EQUAL(PropertyUInt8.GetDataSize(), 1);
			ZE_TEST_CHECK_EQUAL(PropertyUInt8.GetValue().GetUInt8(), 200);
		}

		ZE_TEST_CASE("variant type ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Value.SetUInt16(640);

			ZEMLProperty PropertyUInt16("ItemUInt16", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyUInt16.GetName(), "ItemUInt16");
			ZE_TEST_CHECK_EQUAL(PropertyUInt16.GetType(), ZEML_IT_UINT16);
			ZE_TEST_CHECK_EQUAL(PropertyUInt16.GetDataSize(), 2);
			ZE_TEST_CHECK_EQUAL(PropertyUInt16.GetValue().GetUInt16(), 640);
		}

		ZE_TEST_CASE("variant type ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Value.SetUInt32(43000);

			ZEMLProperty PropertyUInt32("ItemUInt32", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyUInt32.GetName(), "ItemUInt32");
			ZE_TEST_CHECK_EQUAL(PropertyUInt32.GetType(), ZEML_IT_UINT32);
			ZE_TEST_CHECK_EQUAL(PropertyUInt32.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(PropertyUInt32.GetValue().GetUInt32(), 43000);
		}

		ZE_TEST_CASE("variant type ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Value.SetUInt64(2000000000);

			ZEMLProperty PropertyUInt64("ItemUInt64", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyUInt64.GetName(), "ItemUInt64");
			ZE_TEST_CHECK_EQUAL(PropertyUInt64.GetType(), ZEML_IT_UINT64);
			ZE_TEST_CHECK_EQUAL(PropertyUInt64.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(PropertyUInt64.GetValue().GetUInt64(), 2000000000);
		}

		ZE_TEST_CASE("variant type ZE_VRT_BOOLEAN")
		{
			Value.SetBoolean(false);

			ZEMLProperty PropertyBoolean("ItemBoolean", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyBoolean.GetName(), "ItemBoolean");
			ZE_TEST_CHECK_EQUAL(PropertyBoolean.GetType(), ZEML_IT_BOOLEAN);
			ZE_TEST_CHECK_EQUAL(PropertyBoolean.GetDataSize(), 1);
			ZE_TEST_CHECK_ERROR(!PropertyBoolean.GetValue().GetBoolean());
		}

		ZE_TEST_CASE("variant type ZE_VRT_STRING")
		{
			Value.SetString("String");

			ZEMLProperty PropertyString("ItemString", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyString.GetName(), "ItemString");
			ZE_TEST_CHECK_EQUAL(PropertyString.GetType(), ZEML_IT_STRING);
			ZE_TEST_CHECK_EQUAL(PropertyString.GetDataSize(), 7);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyString.GetValue().GetString(), "String");
		}

		ZE_TEST_CASE("variant type ZE_VRT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			ZEMLProperty PropertyQuaternion("ItemQuaternion", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyQuaternion.GetName(), "ItemQuaternion");
			ZE_TEST_CHECK_EQUAL(PropertyQuaternion.GetType(), ZEML_IT_QUATERNION);
			ZE_TEST_CHECK_EQUAL(PropertyQuaternion.GetDataSize(), 16);
			ZE_TEST_CHECK_EQUAL(PropertyQuaternion.GetValue().GetQuaternion(), ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));
		}

		ZE_TEST_CASE("variant type ZE_VRT_VECTOR2")
		{
			Value.SetVector2(ZEVector2(3.0f, 5.0f));

			ZEMLProperty PropertyVector2("ItemVector2", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyVector2.GetName(), "ItemVector2");
			ZE_TEST_CHECK_EQUAL(PropertyVector2.GetType(), ZEML_IT_VECTOR2);
			ZE_TEST_CHECK_EQUAL(PropertyVector2.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(PropertyVector2.GetValue().GetVector2(), ZEVector2(3.0f, 5.0f));
		}

		ZE_TEST_CASE("variant type ZE_VRT_VECTOR3")
		{
			Value.SetVector3(ZEVector3(1.0f, 3.0f, 5.0f));

			ZEMLProperty PropertyVector3("ItemVector3", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyVector3.GetName(), "ItemVector3");
			ZE_TEST_CHECK_EQUAL(PropertyVector3.GetType(), ZEML_IT_VECTOR3);
			ZE_TEST_CHECK_EQUAL(PropertyVector3.GetDataSize(), 12);
			ZE_TEST_CHECK_EQUAL(PropertyVector3.GetValue().GetVector3(), ZEVector3(1.0f, 3.0f, 5.0f));
		}

		ZE_TEST_CASE("variant type ZE_VRT_VECTOR4")
		{
			Value.SetVector4(ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));

			ZEMLProperty PropertyVector4("ItemVector4", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyVector4.GetName(), "ItemVector4");
			ZE_TEST_CHECK_EQUAL(PropertyVector4.GetType(), ZEML_IT_VECTOR4);
			ZE_TEST_CHECK_EQUAL(PropertyVector4.GetDataSize(), 16);
			ZE_TEST_CHECK_EQUAL(PropertyVector4.GetValue().GetVector4(), ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));
		}

		ZE_TEST_CASE("variant type ZE_VRT_MATRIX3X3")
		{
			ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);
			Value.SetMatrix3x3(Matrix);

			ZEMLProperty PropertyMatrix3x3("ItemMatrix3x3", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyMatrix3x3.GetName(), "ItemMatrix3x3");
			ZE_TEST_CHECK_EQUAL(PropertyMatrix3x3.GetType(), ZEML_IT_MATRIX3X3);
			ZE_TEST_CHECK_EQUAL(PropertyMatrix3x3.GetDataSize(), 36);
			ZE_TEST_CHECK_EQUAL(PropertyMatrix3x3.GetValue().GetMatrix3x3(), Matrix);
		}

		ZE_TEST_CASE("variant type ZE_VRT_MATRIX4X4")
		{
			ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
				5.0f, 8.0f, 6.0f, 9.0f,   
				13.0f, 3.0f, 4.0f, 5.0f,   
				12.0f, 2.0f, 8.0f, 6.0f);
			Value.SetMatrix4x4(Matrix);

			ZEMLProperty PropertyMatrix4x4("ItemMatrix4x4", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyMatrix4x4.GetName(), "ItemMatrix4x4");
			ZE_TEST_CHECK_EQUAL(PropertyMatrix4x4.GetType(), ZEML_IT_MATRIX4X4);
			ZE_TEST_CHECK_EQUAL(PropertyMatrix4x4.GetDataSize(), 64);
			ZE_TEST_CHECK_EQUAL(PropertyMatrix4x4.GetValue().GetMatrix4x4(), Matrix);
		}

		ZE_TEST_CASE("variant type ZE_VRT_NULL")
		{
			Value.SetNull();

			ZEMLProperty PropertyNull("ItemNull", Value);
			ZE_TEST_CHECK_STRING_EQUAL(PropertyNull.GetName(), "ItemNull");
			ZE_TEST_CHECK_EQUAL(PropertyNull.GetDataSize(), 0);
			//error Unsupported ZEMLProperty type.
		}
	}

	ZE_TEST_ITEM("ZEUInt64 ZEMLProperty::GetTotalSize()")
	{
		ZEVariant Value;
		ZEUInt64 DataSize;
		ZEUInt64 TotalSize;

		ZE_TEST_CASE("ZEML_IT_FLOAT")
		{
			Value.SetFloat(1.3f);

			ZEMLProperty PFloat("Float", Value);
			DataSize = PFloat.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 4);

			TotalSize = PFloat.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 21);
		}

		ZE_TEST_CASE("ZEML_IT_DOUBLE")
		{
			Value.SetDouble(1.3);

			ZEMLProperty PDouble("Double", Value);
			DataSize = PDouble.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 8);

			TotalSize = PDouble.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 26);
		}

		ZE_TEST_CASE("ZEML_IT_INT8")
		{
			Value.SetInt8(5);

			ZEMLProperty PInt8("Int8", Value);
			DataSize = PInt8.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 1);

			TotalSize = PInt8.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 17);
		}

		ZE_TEST_CASE("ZEML_IT_INT16")
		{
			Value.SetInt16(30);

			ZEMLProperty PInt16("Int16", Value);
			DataSize = PInt16.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 2);

			TotalSize = PInt16.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 19);
		}

		ZE_TEST_CASE("ZEML_IT_INT32")
		{
			Value.SetInt32(0);

			ZEMLProperty PInt32("Int32", Value);
			DataSize = PInt32.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 4);

			TotalSize = PInt32.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 21);
		}

		ZE_TEST_CASE("ZEML_IT_INT64")
		{
			Value.SetInt64(40);

			ZEMLProperty PInt64("Int64", Value);
			DataSize = PInt64.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 8);

			TotalSize = PInt64.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 25);
		}

		ZE_TEST_CASE("ZEML_IT_UINT8")
		{
			Value.SetUInt8(200);

			ZEMLProperty PUInt8("UInt8", Value);
			DataSize = PUInt8.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 1);

			TotalSize = PUInt8.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 18);
		}

		ZE_TEST_CASE("ZEML_IT_UINT16")
		{
			Value.SetUInt16(6400);

			ZEMLProperty PUInt16("UInt16", Value);
			DataSize = PUInt16.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 2);

			TotalSize = PUInt16.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 20);
		}

		ZE_TEST_CASE("ZEML_IT_UINT32")
		{
			Value.SetUInt32(43000);

			ZEMLProperty PUInt32("UInt32", Value);
			DataSize = PUInt32.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 4);

			TotalSize = PUInt32.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 22);
		}

		ZE_TEST_CASE("ZEML_IT_UINT64")
		{
			Value.SetUInt64(2000000000);

			ZEMLProperty PUInt64("UInt64", Value);
			DataSize = PUInt64.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 8);

			TotalSize = PUInt64.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 26);
		}

		ZE_TEST_CASE("ZEML_IT_BOOLEAN")
		{
			Value.SetBoolean(true);

			ZEMLProperty PBoolean("Boolean", Value);
			DataSize = PBoolean.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 1);

			TotalSize = PBoolean.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 20);
		}

		ZE_TEST_CASE("ZEML_IT_STRING")
		{
			Value.SetString("String");

			ZEMLProperty PString("String", Value);
			DataSize = PString.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 7);

			TotalSize = PString.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 25);
		}

		ZE_TEST_CASE("ZEML_IT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			ZEMLProperty PQuaternion("Quaternion", Value);
			DataSize = PQuaternion.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 16);

			TotalSize = PQuaternion.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 38);
		}

		ZE_TEST_CASE("ZEML_IT_VECTOR2")
		{
			Value.SetVector2(ZEVector2(3.0f, 5.0f));

			ZEMLProperty PVector2("Vector2", Value);
			DataSize = PVector2.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 8);

			TotalSize = PVector2.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 27);
		}

		ZE_TEST_CASE("ZEML_IT_VECTOR3")
		{
			Value.SetVector3(ZEVector3(1.0f, 3.0f, 5.0f));

			ZEMLProperty PVector3("Vector3", Value);
			DataSize = PVector3.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 12);

			TotalSize = PVector3.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 31);
		}

		ZE_TEST_CASE("ZEML_IT_VECTOR4")
		{
			Value.SetVector4(ZEVector4(1.0f, 3.0f, 5.0f, 0.0f));

			ZEMLProperty PVector4("Vector4", Value);
			DataSize = PVector4.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 16);

			TotalSize = PVector4.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 35);
		}

		ZE_TEST_CASE("ZEML_IT_MATRIX3X3")
		{
			Value.SetMatrix3x3(ZEMatrix3x3::Identity);

			ZEMLProperty PMatrix3x3("Matrix3x3", Value);
			DataSize = PMatrix3x3.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 36);

			TotalSize = PMatrix3x3.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 57);
		}

		ZE_TEST_CASE("ZEML_IT_MATRIX4X4")
		{
			Value.SetMatrix4x4(ZEMatrix4x4::Zero);

			ZEMLProperty PMatrix4x4("Matrix4x4", Value);
			DataSize = PMatrix4x4.GetDataSize();
			ZE_TEST_CHECK_EQUAL(DataSize, 64);

			TotalSize = PMatrix4x4.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 85);
		}
	}

	ZE_TEST_ITEM("void ZEMLProperty::SetValue(const ZEVariant& Value)")
	{
		ZEVariant Value;
		ZEMLProperty Property;
		ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UNDEFINED);

		ZE_TEST_CASE("ZE_VRT_FLOAT")
		{
			Value.SetFloat(1.3f);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_FLOAT);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetFloat(), Value.GetFloat());
		}

		ZE_TEST_CASE("ZE_VRT_DOUBLE")
		{
			Value.SetDouble(1.3);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_DOUBLE);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetDouble(), Value.GetDouble());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_8")
		{
			Value.SetInt8(5);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_INT8);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 1);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetInt8(), Value.GetInt8());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_16")
		{
			Value.SetInt16(30);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_INT16);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 2);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetInt16(), Value.GetInt16());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_32")
		{
			Value.SetInt32(0);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_INT32);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetInt32(), Value.GetInt32());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_64")
		{
			Value.SetInt64(40);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_INT64);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetInt64(), Value.GetInt64());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Value.SetUInt8(200);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UINT8);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 1);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetUInt8(), Value.GetUInt8());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Value.SetUInt16(6400);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UINT16);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 2);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetUInt16(), Value.GetUInt16());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Value.SetUInt32(43000);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UINT32);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 4);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetUInt32(), Value.GetUInt32());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Value.SetUInt64(2000000000);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_UINT64);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetUInt64(), Value.GetUInt64());
		}

		ZE_TEST_CASE("ZE_VRT_BOOLEAN")
		{
			Value.SetBoolean(true);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_BOOLEAN);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 1);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetBoolean(), Value.GetBoolean());
		}

		ZE_TEST_CASE("ZE_VRT_STRING")
		{
			Value.SetString("Test");

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_STRING);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 5);
			ZE_TEST_CHECK_STRING_EQUAL(Property.GetValue().GetString(), Value.GetString());
		}

		ZE_TEST_CASE("ZE_VRT_QUATERNION")
		{
			Value.SetQuaternion(ZEQuaternion::Identity);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_QUATERNION);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 16);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetQuaternion(), Value.GetQuaternion());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR2")
		{
			Value.SetVector2(ZEVector2::Zero);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_VECTOR2);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 8);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetVector2(), Value.GetVector2());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR3")
		{
			Value.SetVector3(ZEVector3::One);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_VECTOR3);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 12);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetVector3(), Value.GetVector3());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR4")
		{
			Value.SetVector4(ZEVector4::Zero);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_VECTOR4);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 16);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetVector4(), Value.GetVector4());
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX3X3")
		{
			Value.SetMatrix3x3(ZEMatrix3x3::Identity);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_MATRIX3X3);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 36);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetMatrix3x3(), Value.GetMatrix3x3());
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX4X4")
		{
			Value.SetMatrix4x4(ZEMatrix4x4::Zero);

			Property.SetValue(Value);
			ZE_TEST_CHECK_EQUAL(Property.GetType(), ZEML_IT_MATRIX4X4);
			ZE_TEST_CHECK_EQUAL(Property.GetDataSize(), 64);
			ZE_TEST_CHECK_EQUAL(Property.GetValue().GetMatrix4x4(), Value.GetMatrix4x4());
		}
	}

	ZE_TEST_ITEM("const ZEVariant& ZEMLProperty::GetValue() const")
	{
		ZEVariant Variant;
		ZEVariant Value;
		ZEMLProperty Property;

		ZE_TEST_CASE("for ZEMLProperty::ZEMLProperty()")
		{
			ZEMLProperty TestProperty;
			ZE_TEST_CHECK_EQUAL(TestProperty.GetType(), ZEML_IT_UNDEFINED);

			Value = TestProperty.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_UNDEFINED);
		}

		ZE_TEST_CASE("for ZEMLProperty::ZEMLProperty(const ZEString& Name)")
		{
			ZEMLProperty TestProperty("TestProperty");
			ZE_TEST_CHECK_EQUAL(TestProperty.GetType(), ZEML_IT_UNDEFINED);
			ZE_TEST_CHECK_STRING_EQUAL(TestProperty.GetName(), "TestProperty");

			Value = TestProperty.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_UNDEFINED);
		}

		ZE_TEST_CASE("for ZEMLProperty::ZEMLProperty(const ZEString& Name ,const ZEVariant& Value)")
		{	
			Variant.SetBoolean(false);
			ZEMLProperty TestProperty("TestProperty", Variant);
			ZE_TEST_CHECK_EQUAL(TestProperty.GetType(), ZEML_IT_BOOLEAN);

			Value = TestProperty.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetBoolean(), Variant.GetBoolean());		
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_BOOLEAN);
		}

		ZE_TEST_CASE("ZEML_IT_FLOAT")
		{
			Variant.SetFloat(5.0f);
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_FLOAT);
			ZE_TEST_CHECK_EQUAL(Value.GetFloat(), Property.GetValue().GetFloat());
			ZE_TEST_CHECK_EQUAL(Value.GetFloat(), Variant.GetFloat());
		}

		ZE_TEST_CASE("ZEML_IT_DOUBLE")
		{
			Variant.SetDouble(5.0);
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_DOUBLE);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Value.GetDouble(), Property.GetValue().GetDouble());
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Value.GetDouble(), Variant.GetDouble());
		}

		ZE_TEST_CASE("ZEML_IT_STRING")
		{
			Variant.SetString("TestString");
			Property.SetValue(Variant);

			Value = Property.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Value.GetString(), Property.GetValue().GetString());
			ZE_TEST_CHECK_STRING_EQUAL(Value.GetString(), Variant.GetString());
		}
	}
}
