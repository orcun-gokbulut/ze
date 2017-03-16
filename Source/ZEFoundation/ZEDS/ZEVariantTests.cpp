//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariantTests.cpp
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


#include "ZEVariant.h"
#include "ZETest/ZETest.h"
#include "ZEString.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAngle.h"
#include "ZEFile/ZEFile.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEMTType.h"


ZE_TEST(ZEVariant)
{
	ZE_TEST_ITEM("Value Type")
	{
		ZEVariant Variant;
		ZEInt Value = 0x9F8FAFBF;
	
		Variant.SetInt32(Value);

		// Check Type
		ZEMTType ExpectedType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL);
		ZE_TEST_CHECK_ERRORMessage(Variant.GetType() == ExpectedType,
			"Variant type is incorrect.\n
			"  Value: %s"
			"  Expected: %s\n", Variant.GetType().ToString(), ExpectedType.ToString());

		// Check Values & Conversions
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), Value);
		//ZE_TEST_CHECK_EQUAL(Variant.GetInt32Const(), Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32Ref(), Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32ConstRef(), Value);
	}

	ZE_TEST_ITEM("Reference Type")
	{
		ZEVariant Variant;
		ZEInt Value = 0x9F8FAFBF;

		Variant.SetInt32(Value);

		// Check Type
		ZEMTType ExpectedType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL);
		ZE_TEST_CHECK_ERRORMessage(Variant.GetType() == ExpectedType,
			"Variant type is incorrect.\n
			"  Value: %s"
			"  Expected: %s\n", Variant.GetType().ToString(), ExpectedType.ToString());

		// Check Values & Conversions
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), Value);
		//ZE_TEST_CHECK_EQUAL(Variant.GetInt32Const(), Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32Ref(), Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32ConstRef(), Value);
	}


	ZE_TEST_ITEM("ZEVariant::~ZEVariant()")
	{
		const char* Value = "String";
		ZEVariant Variant(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), Value);

		Variant.~ZEVariant();
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNDEFINED);
	}

	ZE_TEST_ITEM("bool ZEVariant::GetBoolean() const")
	{
		ZEVariant Variant;
		bool Value = false;
		Variant.SetBoolean(Value);

		bool Result = Variant.GetBoolean();
		ZE_TEST_CHECK_EQUAL(Result, false);
	}

	ZE_TEST_ITEM("float ZEVariant::GetFloat() const")
	{
		ZEVariant Variant;
		float Value = 0.8f;
		Variant.SetFloat(Value);

		float Result = Variant.GetFloat();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 0.8f);
	}

	ZE_TEST_ITEM("double ZEVariant::GetDouble() const")
	{
		ZEVariant Variant;
		double Value = 1.3;
		Variant.SetDouble(Value);

		double Result = Variant.GetDouble();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 1.3);
	}

	ZE_TEST_ITEM("ZEInt8 ZEVariant::GetInt8() const")
	{
		ZEVariant Variant;
		ZEInt8 Value = 100;
		Variant.SetInt8(Value);

		ZEInt8 Result = Variant.GetInt8();
		ZE_TEST_CHECK_EQUAL(Result, 100);

		ZE_TEST_CASE("for value bigger than upper limit of ZEInt8")
		{
			Value = 130;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, -126);

			Value = 127;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, 127);

			Value = 128;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, -128);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEInt8")
		{
			Value = -130;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, 126);

			Value = -128.9;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, -128);

			Value = -129;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZE_TEST_CHECK_EQUAL(Result, 127);
		}
	}

	ZE_TEST_ITEM("ZEInt16 ZEVariant::GetInt16() const")
	{
		ZEVariant Variant;
		ZEInt16 Value = 30600;
		Variant.SetInt16(Value);

		ZEInt16 Result = Variant.GetInt16();
		ZE_TEST_CHECK_EQUAL(Result, 30600);

		Value = 32767;
		Variant.SetInt16(Value);
		Result = Variant.GetInt16();
		ZE_TEST_CHECK_EQUAL(Result, 32767);

		Variant.SetInt16(32767.9);
		Result = Variant.GetInt16();
		ZE_TEST_CHECK_EQUAL(Result, 32767);

		Variant.SetInt16(-32768);
		Result = Variant.GetInt16();
		ZE_TEST_CHECK_EQUAL(Result, -32768);

		ZE_TEST_CASE("for value bigger than upper limit of ZEInt16")
		{
			Value = 34780;
			Variant.SetInt16(Value);

			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, -30756);

			Variant.SetInt16(32768);
			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, -32768);

			Variant.SetInt16(65535);
			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, -1);

			Variant.SetInt16(65536);
			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, 0);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEInt16")
		{
			Value = -33780;
			Variant.SetInt16(Value);

			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, 31756);

			Variant.SetInt16(-32769);
			Result = Variant.GetInt16();
			ZE_TEST_CHECK_EQUAL(Result, 32767);
		}
	}

	ZE_TEST_ITEM("ZEInt32 ZEVariant::GetInt32() const")
	{
		ZEVariant Variant;
		ZEInt32 Value = 5;
		Variant.SetInt32(Value);

		ZEInt32 Result = Variant.GetInt32();
		ZE_TEST_CHECK_EQUAL(Result, 5);

		Variant.SetInt32(2147483647);
		Result = Variant.GetInt32();
		ZE_TEST_CHECK_EQUAL(Result, 2147483647);

		ZE_TEST_CASE("for value bigger than upper limit of ZEInt32")
		{
			Value = 2547483647;
			Variant.SetInt32(Value);

			Result = Variant.GetInt32();
			ZE_TEST_CHECK_EQUAL(Result, -1747483649);

			Variant.SetInt32(2147483648);
			Result = Variant.GetInt32();
			ZE_TEST_CHECK_EQUAL(Result, -2147483648);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEInt32")
		{
			Value = -2547483647;
			Variant.SetInt32(Value);

			Result = Variant.GetInt32();
			ZE_TEST_CHECK_EQUAL(Result, 1747483649);

			Value = -2147483649;
			Variant.SetInt32(Value);
			Result = Variant.GetInt32();
			ZE_TEST_CHECK_EQUAL(Result, 2147483647);
		}
	}

	ZE_TEST_ITEM("ZEInt64 ZEVariant::GetInt64() const")
	{
		ZEVariant Variant;
		ZEInt64 Value = 500;
		Variant.SetInt64(Value);

		ZEInt64 Result = Variant.GetInt64();
		ZE_TEST_CHECK_EQUAL(Result, 500);

		Value = -9223372036854775808;
		Variant.SetInt64(Value);
		Result = Variant.GetInt64();
		ZE_TEST_CHECK_EQUAL(Result, -9223372036854775808);

		ZE_TEST_CASE("for value bigger than upper limit of ZEInt64")
		{
			Value = 9323372036854775807;
			Variant.SetInt64(Value);

			Result = Variant.GetInt64();
			ZE_TEST_CHECK_EQUAL(Result, -9123372036854775809);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEInt64")
		{
			Value = -9223372036854775809;
			Variant.SetInt64(Value);

			Result = Variant.GetInt64();
			ZE_TEST_CHECK_EQUAL(Result, 9223372036854775807);
		}
	}

	ZE_TEST_ITEM("ZEUInt8 ZEVariant::GetUInt8() const")
	{
		ZEVariant Variant;
		ZEUInt8 Value = 200;
		Variant.SetUInt8(Value);

		ZEUInt8 Result = Variant.GetUInt8();
		ZE_TEST_CHECK_EQUAL(Result, 200);

		ZE_TEST_CASE("for value bigger than upper limit of ZEUInt8")
		{
			Value = 256;
			Variant.SetUInt8(Value);

			Result = Variant.GetUInt8();
			ZE_TEST_CHECK_EQUAL(Result, 0);

			Variant.SetUInt8(3468);
			Result = Variant.GetUInt8();
			ZE_TEST_CHECK_EQUAL(Result, 140);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEUInt8")
		{
			Value = -3;
			Variant.SetUInt8(Value);

			Result = Variant.GetUInt8();
			ZE_TEST_CHECK_EQUAL(Result, 253);
		}
	}

	ZE_TEST_ITEM("ZEUInt16 ZEVariant::GetUInt16() const")
	{
		ZEVariant Variant;
		ZEUInt16 Value = 500;
		Variant.SetUInt16(Value);

		ZEUInt16 Result = Variant.GetUInt16();
		ZE_TEST_CHECK_EQUAL(Result, 500);

		ZE_TEST_CASE("for value bigger than upper limit of ZEUInt16")
		{
			Value = 66000;
			Variant.SetUInt16(Value);

			Result = Variant.GetUInt16();
			ZE_TEST_CHECK_EQUAL(Result, 464);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEUInt16")
		{
			Value = -10;
			Variant.SetUInt16(Value);

			Result = Variant.GetUInt16();
			ZE_TEST_CHECK_EQUAL(Result, 65526);
		}
	}

	ZE_TEST_ITEM("ZEUInt32 ZEVariant::GetUInt32() const")
	{
		ZEVariant Variant;
		ZEUInt32 Value = 555;
		Variant.SetUInt32(Value);

		ZEUInt32 Result = Variant.GetUInt32();
		ZE_TEST_CHECK_EQUAL(Result, 555);

		ZE_TEST_CASE("for value bigger than upper limit of ZEUInt32")
		{
			Value = 4594967295;
			Variant.SetUInt32(Value);

			Result = Variant.GetUInt32();
			ZE_TEST_CHECK_EQUAL(Result, 299999999);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEUInt32")
		{
			Value = -10;
			Variant.SetUInt32(Value);

			Result = Variant.GetUInt32();
			ZE_TEST_CHECK_EQUAL(Result, 4294967286);
		}
	}

	ZE_TEST_ITEM("ZEUInt64 ZEVariant::GetUInt64() const")
	{
		ZEVariant Variant;
		ZEUInt64 Value = 1300;
		Variant.SetUInt64(Value);

		ZEUInt64 Result = Variant.GetUInt64();
		ZE_TEST_CHECK_EQUAL(Result, 1300);

		ZE_TEST_CASE("for value bigger than upper limit of ZEUIn64")
		{
			Value = 18446744073709551615.9;
			Variant.SetUInt64(Value);

			Result = Variant.GetUInt64();
			ZE_TEST_CHECK_EQUAL(Result, 384);
		}

		ZE_TEST_CASE("for value lower than lower limit of ZEUInt64")
		{
			Value = -10;
			Variant.SetUInt64(Value);

			Result = Variant.GetUInt64();
			ZE_TEST_CHECK_EQUAL(Result, 18446744073709551606);
		}
	}

	ZE_TEST_ITEM("ZEMatrix3x3& ZEVariant::GetMatrix3x3() const")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		Variant.SetMatrix3x3(Matrix);

		ZEMatrix3x3 Result = Variant.GetMatrix3x3();
		ZE_TEST_CHECK_EQUAL(Result, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZE_TEST_ITEM("ZEMatrix4x4& ZEVariant::GetMatrix4x4() const")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix( 1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);

		ZEMatrix4x4 Result = Variant.GetMatrix4x4();
		ZE_TEST_CHECK_EQUAL(Result, ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 6.0f, 9.0f, 13.0f, 3.0f, 4.0f, 5.0f, 12.0f, 2.0f, 8.0f, 6.0f));
	}

	ZE_TEST_ITEM("ZEQuaternion& ZEVariant::GetQuaternion() const")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_4, ZEVector3::UnitY);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = Variant.GetQuaternion();
		ZE_TEST_CHECK_EQUAL(Result, ZEQuaternion(0.92387956f, 0.0f, 0.38268349f, 0.0f));
	}

	ZE_TEST_ITEM("const ZEString& ZEVariant::GetString() const")
	{
		ZEVariant Variant;
		ZEString String = "String";
		Variant.SetString(String);

		ZEString Result;
		Result = Variant.GetString();
		ZE_TEST_CHECK_STRING_EQUAL(Result, String);
	}

	ZE_TEST_ITEM("ZEVariantType ZEVariant::GetType() const")
	{
		ZEVariant Variant;
		ZEInt32 i = 6;
		Variant.SetInt32(i);

		ZEVariantType Result = Variant.GetType();
		ZE_TEST_CHECK_EQUAL(Result, ZE_VRT_INTEGER_32);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariantValue ZEVariant::GetValue()	const")
	{
		ZEVariant Variant;
		ZEInt32 i = 6;
		Variant.SetInt32(i);

		ZEVariant::ZEVariantValue Result = Variant.GetValue();
		ZE_TEST_CHECK_EQUAL(Result.Int32, 6);

		ZE_TEST_CASE("for string")
		{
			Variant.SetString("VariantTest");

			Result = Variant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "VariantTest");
		}

		ZE_TEST_CASE("for ZEVariant(const char*)")
		{
			ZEVariant Variant1("Test");

			Result = Variant1.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "Test");
		}
	}

	ZE_TEST_ITEM("ZEVector2& ZEVariant::GetVector2() const")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);
		Variant.SetVector2(Vector);

		ZEVector2 Result = Variant.GetVector2();
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(1.0f, 2.0f));
	}

	ZE_TEST_ITEM("ZEVector3& ZEVariant::GetVector3() const")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		Variant.SetVector3(Vector);

		ZEVector3 Result = Variant.GetVector3();
		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector4& ZEVariant::GetVector4() const")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		Variant.SetVector4(Vector);

		ZEVector4 Result = Variant.GetVector4();
		ZE_TEST_CHECK_EQUAL(Result, ZEVector4(1.0f, 2.0f, 3.0f, 4.0f));
	}

	ZE_TEST_ITEM("ZEObject* ZEVariant::GetClass() const")
	{
		ZEVariant Variant;

		ZEObject* Class;
		

	}

	ZE_TEST_ITEM("ZEVariant::ZEVariantValue& ZEVariant::ImplicitAcesss()")
	{
		ZEVariant Variant;
		ZEInt32 Value32 = 0;
		Variant.SetInt32(Value32);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);

		ZEVariant::ZEVariantValue Result = Variant.ImplicitAcesss();
		ZE_TEST_CHECK_EQUAL(Result.Int32, 0);
		ZE_TEST_CHECK_EQUAL(Variant.ImplicitAcesss().Int32, 0);

		ZEInt16 Value16 = 35;
		Variant.SetInt16(Value16);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);

		ZEVariant::ZEVariantValue Result1 = Variant.ImplicitAcesss();
		ZE_TEST_CHECK_EQUAL(Result1.Int32, 35);
		ZE_TEST_CHECK_EQUAL(Variant.ImplicitAcesss().Int32, 35);

		ZE_TEST_CASE("for ZE_VRT_STRING")
		{
			Variant = "TestString";
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);

			Result = Variant.ImplicitAcesss();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestString");
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "TestString");
		}
 	}

	ZE_TEST_ITEM("bool ZEVariant::IsNull() const")
	{
		ZEVariant Variant;
		Variant.SetNull();

		bool Result = Variant.IsNull();
		ZE_TEST_CHECK_ERROR(Result);

		ZE_TEST_CASE("for type not null")
		{
			Variant.SetInt32(5);

			Result = Variant.IsNull();
			ZE_TEST_CHECK_ERROR(!Result);
 		}	
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(bool NewValue)")
	{
		ZEVariant Variant;
		bool NewValue = false;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZE_TEST_CHECK_ERROR(!Variant.GetBoolean());
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const char* NewValue)")
	{
		ZEVariant Variant;
		const char* NewValue = "String";

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");

		ZE_TEST_CASE("for ZEString")
		{
			Variant.SetType(ZE_VRT_VECTOR4);
			ZEString String = "TestString";
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);

			Variant = String;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), String);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "TestString");
		}

		ZE_TEST_CASE("for char* ")
		{
			Variant.SetType(ZE_VRT_BOOLEAN);
			char* String = "NewString";
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);

			Variant = String;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), String);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "NewString");
		}
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEString& NewValue)")
	{
		ZEVariant Variant;
		const ZEString NewValue = "String";

		ZEVariant NewVariant = Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_EQUAL(NewVariant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");
		ZE_TEST_CHECK_STRING_EQUAL(NewVariant.GetString(), "String");

		ZE_TEST_CASE("for an empty ZEString")
		{
			Variant.SetType(ZE_VRT_VECTOR2);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
			const ZEString Value = "";

			NewVariant = Variant = Value;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_EQUAL(NewVariant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_ERROR(Variant.GetString() == "");
			ZE_TEST_CHECK_ERROR(NewVariant.GetString() == "");
		}
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEMatrix3x3& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix( 1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant = Matrix;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix3x3(), Matrix);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEMatrix4x4& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant = Matrix;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix4x4(), Matrix);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEQuaternion& Quaternion)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

		Variant = Quaternion;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
		ZE_TEST_CHECK_EQUAL(Variant.GetQuaternion(), Quaternion);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEVariant& NewValue)")
	{
		ZEVariant Variant;
		ZEVariant NewValue;
		NewValue.SetInt32(5);

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), 5);

		ZE_TEST_CASE("for variant type string")
		{
			ZEVariant Variant1;
			ZEVariant NewValue1;
			NewValue1.SetString("Test");

			Variant1 = NewValue1;
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant1.GetString(), "Test");
		}
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEVector2& Vector)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant = Vector;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector2(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEVector3& Vector)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant = Vector;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR3);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector3(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(const ZEVector4& Vector)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant = Vector;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector4(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(float NewValue)")
	{
		ZEVariant Variant;
		float NewValue = 3.8f;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);
		ZE_TEST_CHECK_EQUAL(Variant.GetFloat(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(double NewValue)")
	{
		ZEVariant Variant;
		double NewValue = 3.8;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
		ZE_TEST_CHECK_EQUAL(Variant.GetDouble(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEInt8 NewValue)")
	{
		ZEVariant Variant;
		ZEInt8 NewValue = 110;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEInt16 NewValue)")
	{
		ZEVariant Variant;
		ZEInt16 NewValue = 1000;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt16(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEInt32 NewValue)")
	{
		ZEVariant Variant;
		ZEInt32 NewValue = 1000000000;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEInt64 NewValue)")
	{
		ZEVariant Variant;
		ZEInt64 NewValue = 700;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt64(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEUInt8 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt8 NewValue = 210;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt8(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEUInt16 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt16 NewValue = 210;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt16(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEUInt32 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt32 NewValue = 550;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEUInt64 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt64 NewValue = 1550;

		Variant = NewValue;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt64(), NewValue);
	}

	ZE_TEST_ITEM("ZEVariant& ZEVariant::operator=(ZEObject* NewValue)")
	{

	}

	ZE_TEST_ITEM("ZEVariant::operator bool()")
	{
		ZEVariant Variant;
		Variant.SetBoolean(true);

		bool Result = (bool)Variant;
		ZE_TEST_CHECK_ERROR(Result);
	}

	ZE_TEST_ITEM("ZEVariant::operator const char*()")
	{
		ZEVariant Variant;
		Variant.SetString("String");

		const char* Result = (const char*)Variant;
		ZE_TEST_CHECK_STRING_EQUAL(Result, Variant.GetString());
		ZE_TEST_CHECK_STRING_EQUAL(Result, "String");

		ZE_TEST_CASE("for ZEString")
		{
			ZEString String = "TestString";
			Variant.SetString(String);

			Result = (const char*)Variant;
			ZE_TEST_CHECK_STRING_EQUAL(Result, String);
			ZE_TEST_CHECK_STRING_EQUAL(Result, "TestString");
			ZE_TEST_CHECK_STRING_EQUAL(Result, Variant.GetString());
		}

		ZE_TEST_CASE("for char* ")
		{
			char* String = "NewString";
			Variant.SetString(String);

			Result = (const char*)Variant;
			ZE_TEST_CHECK_STRING_EQUAL(Result, String);
			ZE_TEST_CHECK_STRING_EQUAL(Result, "NewString");
			ZE_TEST_CHECK_STRING_EQUAL(Result, Variant.GetString());
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), String);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator float()")
	{
		ZEVariant Variant;
		Variant.SetFloat(5.55f);

		float Result =(float)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetFloat());

		ZE_TEST_CASE("for ZEInt32")
		{
			Variant.SetInt32(5);

			Result = (float)Variant;
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 5.0f);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZEVariant::ZEVariantValue Value = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL(Value.Int32, 5);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator double()")
	{
		ZEVariant Variant;
		Variant.SetDouble(5.55);

		double Result = (double)Variant;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Variant.GetDouble());

		ZE_TEST_CASE("for ZEInt32")
		{
			Variant.SetInt32(3);

			Result = (double)Variant;
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 3.0);
			ZEVariant::ZEVariantValue Value = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Value.Double, 5.55);
			ZE_TEST_CHECK_EQUAL(Value.Int32, 3);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEInt8()")
	{
		ZEVariant Variant;
		Variant.SetInt8(10);

		ZEInt8 Result = (ZEInt8)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetInt8());

		ZE_TEST_CASE("for float")
		{
			Variant.SetFloat(13.2f);

			Result = (ZEInt8)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 13);
			ZE_TEST_CHECK_EQUAL(Variant.GetFloat(), 13.2f);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEInt16()")
	{
		ZEVariant Variant;
		Variant.SetInt16(150);

		ZEInt16 Result = (ZEInt16)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetInt16());

		ZE_TEST_CASE("for double")
		{
			Variant.SetDouble(120.2);

			Result = (ZEInt16)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 120);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), 120.2);
		}

		ZE_TEST_CASE("for ZEVector2")
		{
			Variant.SetVector2(ZEVector2::One);

			Result = (ZEInt16)Variant;
			//error Integer conversion operation failed. Variant type mismatched.
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(Variant.GetVector2(), ZEVector2::One);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEInt32()")
	{
		ZEVariant Variant;
		Variant.SetInt32(560);

		ZEInt32 Result = (ZEInt32)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetInt32());

		ZE_TEST_CASE("for float")
		{
			Variant.SetFloat(510.3f);

			Result = (ZEInt32)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 510);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetFloat(), 510.3f);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEInt64()")
	{
		ZEVariant Variant;
		Variant.SetInt64(10);

		ZEInt64 Result = (ZEInt64)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetInt64());

		ZE_TEST_CASE("for double")
		{
			Variant.SetDouble(11.1);

			Result = (ZEInt64)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 11);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), 11.1);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEUInt8()")
	{
		ZEVariant Variant;
		Variant.SetUInt8(3);

		ZEUInt8 Result = (ZEUInt8)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetUInt8());

		ZE_TEST_CASE("for ZEInt8")
		{
			Variant.SetInt8(-120);

			Result = (ZEUInt8)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 136);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), -120);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEUInt16()")
	{
		ZEVariant Variant;
		Variant.SetUInt16(5);

		ZEUInt16 Result = (ZEUInt16)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetUInt16());

		ZE_TEST_CASE("for ZEInt8")
		{
			Variant.SetInt8(120);

			Result = (ZEUInt16)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 120);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), 120);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEUInt32()")
	{
		ZEVariant Variant;
		Variant.SetUInt32(20);

		ZEUInt32 Result = (ZEUInt32)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetUInt32());

		ZE_TEST_CASE("for double")
		{
			Variant.SetDouble(26.1);

			Result = (ZEUInt32)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 26);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), 26.1);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEUInt64()")
	{
		ZEVariant Variant;
		Variant.SetUInt64(160);

		ZEUInt64 Result = (ZEUInt64)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetUInt64());

		ZE_TEST_CASE("for ZEInt64")
		{
			Variant.SetInt64(-10);

			Result = (ZEUInt64)Variant;
			ZE_TEST_CHECK_EQUAL(Result, 18446744073709551606);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt64(), -10);
		}
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEMatrix3x3()")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);

		ZEMatrix3x3 Result = (ZEMatrix3x3)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetMatrix3x3());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEMatrix4x4()")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
			5.0f, 8.0f, 6.0f, 9.0f,   
			13.0f, 3.0f, 4.0f, 5.0f,   
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);

		ZEMatrix4x4 Result = (ZEMatrix4x4)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetMatrix4x4());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEQuaternion()")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = (ZEQuaternion)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetQuaternion());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEVector2()")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);
		Variant.SetVector2(Vector);

		ZEVector2 Result = (ZEVector2)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetVector2());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEVector3()")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		Variant.SetVector3(Vector);

		ZEVector3 Result = (ZEVector3)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetVector3());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEVector4()")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		Variant.SetVector4(Vector);

		ZEVector4 Result = (ZEVector4)Variant;
		ZE_TEST_CHECK_EQUAL(Result, Variant.GetVector4());
	}

	ZE_TEST_ITEM("ZEVariant::operator ZEObject*()")
	{
		ZEVariant Variant;

	}

	ZE_TEST_ITEM("bool ZEVariant::Serialize(ZESerializer* Serializer)")
	{
		ZEFile File;
		File.Open("ZEVariantTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		ZEVariant Variant;

		ZE_TEST_CASE("for ZEInt8")
		{
			Variant.SetInt8(16);

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_ERROR(Result);	
		}

		ZE_TEST_CASE("for ZEInt16")
		{
			ZEInt16 Value = 30;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEInt32")
		{
			ZEInt32 Value = 0;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEInt64")
		{
			ZEInt64 Value = 40;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEUInt8")
		{
			ZEUInt8 Value = 200;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEUInt16")
		{
			ZEUInt16 Value = 640;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEUInt32")
		{
			ZEUInt32 Value = 43000;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEUInt64")
		{
			ZEUInt64 Value = 2000000000;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for float")
		{
			float Float = 9.0f;

			Variant = Float;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for double")
		{
			double Double = 3.80;

			Variant = Double;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEMatrix3x3")
		{
			ZEInt64 LastItem;

			ZEMatrix3x3 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Variant = Matrix;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEMatrix4x4")
		{
			ZEInt64 LastItem;

			ZEMatrix4x4 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Matrix.M14 = 4;
			Variant = Matrix;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEVector2")
		{
			ZEInt64 LastItem;

			ZEVector2 Vector(1.0f, 2.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEVector3")
		{
			ZEInt64 LastItem;

			ZEVector3 Vector(1.0f, 2.0f, 3.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR3);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEVector4")
		{
			ZEInt64 LastItem;

			ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for Boolean")
		{
			bool Boolean = true;
			Variant = Boolean;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEString")
		{
			ZEString String = "String";
			Variant = String;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for ZEQuaternion")
		{
			ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
			Variant = Quaternion;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
			ZE_TEST_CHECK_ERROR(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for undefined")
		{
			Variant.SetType(ZE_VRT_UNDEFINED);
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNDEFINED);
			ZE_TEST_CHECK_ERROR(Result);
			ZEInt64 Last = File.Tell();
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZE_TEST_CASE("for null")
		{
			Variant.SetType(ZE_VRT_NULL);
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);
			ZE_TEST_CHECK_ERROR(Result);
			ZEInt64 Last = File.Tell();
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		File.Close();
 		remove("ZEVariantTests.txt");
	}

	ZE_TEST_ITEM("void ZEVariant::SetBoolean(bool Value)")
	{
		ZEVariant Variant;
		bool Value = false;

		Variant.SetBoolean(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZE_TEST_CHECK_ERROR(!Variant.GetBoolean());
	}

	ZE_TEST_ITEM("void ZEVariant::SetFloat(float Value)")
	{
		ZEVariant Variant;
		float Value = 0.8f;

		Variant.SetFloat(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);
		ZE_TEST_CHECK_EQUAL(Variant.GetFloat(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetDouble(double Value)")
	{
		ZEVariant Variant;
		double Value = 5.8;

		Variant.SetDouble(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetInt8(ZEInt8 Value)")
	{
		ZEVariant Variant;
		ZEInt8 Value = 16;

		Variant.SetInt8(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetInt16(ZEInt16 Value)")
	{
		ZEVariant Variant;
		ZEInt16 Value = 30;

		Variant.SetInt16(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt16(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetInt32(ZEInt32 Value)")
	{
		ZEVariant Variant;
		ZEInt32 Value = 0;

		Variant.SetInt32(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetInt64(ZEInt64 Value)")
	{
		ZEVariant Variant;
		ZEInt64 Value = 40;

		Variant.SetInt64(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt64(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetUInt8(ZEUInt8 Value)")
	{
		ZEVariant Variant;
		ZEUInt8 Value = 200;

		Variant.SetUInt8(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt8(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetUInt16(ZEUInt16 Value)")
	{
		ZEVariant Variant;
		ZEUInt16 Value = 640;

		Variant.SetUInt16(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt16(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetUInt32(ZEUInt32 Value)")
	{
		ZEVariant Variant;
		ZEUInt32 Value = 43000;

		Variant.SetUInt32(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetUInt64(ZEUInt64 Value)")
	{
		ZEVariant Variant;
		ZEUInt64 Value = 2000000000;

		Variant.SetUInt64(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt64(), Value);
	}

	ZE_TEST_ITEM("void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Value)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix3x3(), Matrix);
	}

	ZE_TEST_ITEM("void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Value)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
			5.0f, 8.0f, 6.0f, 9.0f,   
			13.0f, 3.0f, 4.0f, 5.0f,   
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix4x4(), Matrix);
	}

	ZE_TEST_ITEM("void ZEVariant::SetNull()")
	{
		ZEVariant Variant;

		Variant.SetNull();
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);
	}

	ZE_TEST_ITEM("void ZEVariant::SetClass(ZEObject* Value)")
	{
//		ZEVariant Variant;

//		ZEObject* Class;
//		Variant.SetClass(Class);
//  	ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_CLASS);
	}

	ZE_TEST_ITEM("void ZEVariant::SetQuaternion(const ZEQuaternion& Value)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		Variant.SetQuaternion(Quaternion);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
		ZE_TEST_CHECK_EQUAL(Variant.GetQuaternion(), Quaternion);
	}

	ZE_TEST_ITEM("void ZEVariant::SetString(const char *Value)")
	{
		ZEVariant Variant;
		const char* NewValue = "String";

		Variant.SetString(NewValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");

		ZE_TEST_CASE("set string to "" ")
		{
			NewValue = "";

			Variant.SetString(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_ERROR(Variant.GetString() == "");
		}

		ZE_TEST_CASE("call SetString with different parameters respectively")
		{
			Variant.SetString("String");
			Variant.SetString("Test");

			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "Test");
		}

		ZE_TEST_CASE("first set variant to boolean then set same variant to string")
		{
			ZEVariant TestVariant(false);
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_BOOLEAN);
			ZE_TEST_CHECK_ERROR(!TestVariant.GetBoolean());
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_ERROR(!Result.Boolean);

			TestVariant = "Test";
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(),ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "Test");
			Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "Test");
		}

		ZE_TEST_CASE("first create a variant as undefined then set string to same variant")
		{
			ZEVariant TestVariant;
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_UNDEFINED);
			ZEString String = TestVariant.GetString(); //error Variant type mismatched

			const char* Value = "TestString";
			TestVariant.SetString(Value);

			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), Value);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "TestString");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, Value);
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestString");
		}

		ZE_TEST_CASE("first create a variant as undefined then set ZEString to same variant")
		{
			ZEVariant TestVariant;
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_UNDEFINED);

			ZEString Value = "Test";
			TestVariant.SetString(Value);

			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), Value);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "Test");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, Value);
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "Test");
		}

		ZE_TEST_CASE("set variant to different variant types respectively")
		{
			ZEVariant NewVariant;
			NewVariant.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), NewVariant.GetType());
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);

			Variant.SetString("TestValue");
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "TestValue");
			ZEVariant::ZEVariantValue Result = Variant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestValue");

			Variant.SetDouble(1.20);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), 1.20);
			ZEString String = Variant.GetString(); //(Bad Ptr) error Variant type mismatched
			Result = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.Double, 1.20);
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestValue");

			NewVariant.SetType(ZE_VRT_STRING);
			NewVariant = "Test";
			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), NewVariant.GetType());
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewVariant.GetString());
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "Test");
			ZE_TEST_CHECK_STRING_EQUAL(NewVariant.GetString(), "Test");

			Variant = "String";	
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");
			ZE_TEST_CHECK_STRING_EQUAL(NewVariant.GetString(), "Test");

			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), 0);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			const char* NewString = "NewTestString";
			Variant = NewString;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewString);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "NewTestString");

			NewVariant.SetType(ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(NewVariant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(NewVariant.GetInt8(), 0);
			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), 0);
			Result = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL(Result.Int32, 0);
		}

		ZE_TEST_CASE("for dynamicly created const char *")
		{
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);

			const char* String = new char;

			Variant.SetString(String);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), String);
		}

		ZE_TEST_CASE("assign ZEString to const char* ")
		{
			ZEString String = "New";
			NewValue = String;

			Variant.SetType(ZE_VRT_NULL);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);

			Variant.SetString(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewValue);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "New");
		}

		ZE_TEST_CASE("set string to NULL")
		{
 			NewValue = NULL;
 
 			Variant.SetString(NewValue);
 			const char* Result = Variant.GetString();
			ZE_TEST_CHECK_ERROR(Result == NULL);
		}
	}

	ZE_TEST_ITEM("void ZEVariant::SetString(const ZEString& Value)")
	{
		ZEVariant Variant;
		ZEString NewValue = "String";

		Variant.SetString(NewValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");

		ZE_TEST_CASE("for an empty string")
		{
			const ZEString String;

			Variant.SetString(String);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_ERROR(Variant.GetString() == "");
		}

		ZE_TEST_CASE("set string to "" ")
		{
			const ZEString NewValue1 = "";

			Variant.SetString(NewValue1);
 			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_ERROR(Variant.GetString() == "");
		}

		ZE_TEST_CASE("call SetString with different parameters respectively")
		{
			ZEString String = "String";
			ZEString Test = "Test";

			Variant.SetString(String);
			Variant.SetString(Test);

			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "Test");
		}

		ZE_TEST_CASE("first set variant to boolean then set same variant to string")
		{
			ZEVariant TestVariant(false);
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_BOOLEAN);
			ZE_TEST_CHECK_ERROR(!TestVariant.GetBoolean());
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_ERROR(!Result.Boolean);

			ZEString Test = "Test";
			TestVariant = Test;
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(),ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "Test");
			Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "Test");
		}

		ZE_TEST_CASE("first create a variant as undefined then set string to same variant")
		{
			ZEVariant TestVariant;
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_UNDEFINED);
			ZE_TEST_CHECK_ERROR(TestVariant.GetString() == ""); //error Variant type mismatched

			ZEString Value = "TestString";
			TestVariant.SetString(Value);

			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), Value);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "TestString");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, Value);
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestString");
		}

		ZE_TEST_CASE("first create a variant as undefined then set ZEString to same variant")
		{
			ZEVariant TestVariant;
			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_UNDEFINED);

			ZEString Value = "Test";
			TestVariant.SetString(Value);

			ZE_TEST_CHECK_EQUAL(TestVariant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), Value);
			ZE_TEST_CHECK_STRING_EQUAL(TestVariant.GetString(), "Test");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, Value);
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "Test");
		}

		ZE_TEST_CASE("set variant to different variant types respectively")
		{
			ZEVariant NewVariant;
			NewVariant.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), NewVariant.GetType());
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);

			ZEString TestValue = "TestValue";
			Variant = TestValue;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "TestValue");
			ZEVariant::ZEVariantValue Result = Variant.GetValue();
			ZE_TEST_CHECK_STRING_EQUAL(Result.String, "TestValue");

			Variant.SetDouble(1.20);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), 1.20);
			ZEString String = Variant.GetString(); //(Bad Ptr) error Variant type mismatched
			Result = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.Double, 1.20);
			String = Result.String;
			ZE_TEST_CHECK_STRING_EQUAL(String, "TestValue");

			NewVariant.SetType(ZE_VRT_STRING);
			ZEString Test = "Test";
			NewVariant = Test;
			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), NewVariant.GetType());
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewVariant.GetString());
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "Test");
			ZE_TEST_CHECK_STRING_EQUAL(NewVariant.GetString(), "Test");

			Variant = "String";	
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");
			ZE_TEST_CHECK_STRING_EQUAL(NewVariant.GetString(), "Test");

			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), 0);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZEString NewString = "NewTestString";
			Variant = NewString;
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewString);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "NewTestString");

			NewVariant.SetType(ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(NewVariant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(NewVariant.GetInt8(), 0);
			Variant.SetVariant(NewVariant);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), 0);
			Result = Variant.GetValue();
			ZE_TEST_CHECK_EQUAL(Result.Int32, 0);
		}

		ZE_TEST_CASE("assign const char* to ZEString ")
		{
			const char* String = "New";
			NewValue = String;

			Variant.SetType(ZE_VRT_NULL);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);

			Variant.SetString(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewValue);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "New");
		}

		ZE_TEST_CASE("for dynamicly created ZEString")
		{
			ZEString String = new char;

			Variant.SetString(String);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), String);
		}
	}

	ZE_TEST_ITEM("void ZEVariant::SetType(ZEVariantType NewType)")
	{
		ZEVariant Variant;

		Variant.SetType(ZE_VRT_FLOAT);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);

		Variant.SetType(ZE_VRT_MATRIX4X4);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);

		Variant.SetString("String");
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");

		Variant.SetType(ZE_VRT_STRING);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");
		
		Variant.SetType(ZE_VRT_MATRIX3X3);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), "String");
		//error ZEVariant::GetString operation failed. Variant type mismatched. 
	}

	ZE_TEST_ITEM("void ZEVariant::SetVariant(const ZEVariant& NewValue)")
	{
		ZEVariant NewValue;
		ZEVariant Variant;

		ZE_TEST_CASE("ZE_VRT_UNDEFINED")
		{
			NewValue.SetType(ZE_VRT_UNDEFINED);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNDEFINED);
		}

		ZE_TEST_CASE("ZE_VRT_NULL")
		{
			NewValue.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_NULL);
		}

		ZE_TEST_CASE("ZE_VRT_FLOAT")
		{
			NewValue.SetFloat(3.2f);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);
			ZE_TEST_CHECK_EQUAL(Variant.GetFloat(), NewValue.GetFloat());
		}

		ZE_TEST_CASE("ZE_VRT_DOUBLE")
		{
			NewValue.SetDouble(2.8);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), NewValue.GetDouble());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_8")
		{
			NewValue.SetInt8(16);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), NewValue.GetInt8());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_16")
		{
			NewValue.SetInt16(30);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt16(), NewValue.GetInt16());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_32")
		{
			NewValue.SetInt32(0);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), NewValue.GetInt32());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_64")
		{
			NewValue.SetInt64(40);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZE_TEST_CHECK_EQUAL(Variant.GetInt64(), NewValue.GetInt64());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			NewValue.SetUInt8(200);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt8(), NewValue.GetUInt8());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			NewValue.SetUInt16(640);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt16(), NewValue.GetUInt16());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			NewValue.SetUInt32(43000);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), NewValue.GetUInt32());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			NewValue.SetUInt64(2000000000);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZE_TEST_CHECK_EQUAL(Variant.GetUInt64(), NewValue.GetUInt64());
		}

		ZE_TEST_CASE("ZE_VRT_BOOLEAN")
		{
			NewValue.SetBoolean(false);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
			ZE_TEST_CHECK_EQUAL(Variant.GetBoolean(), NewValue.GetBoolean());
		}

		ZE_TEST_CASE("ZE_VRT_STRING")
		{
			NewValue.SetString("String");

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
			ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), NewValue.GetString());
		}

		ZE_TEST_CASE("ZE_VRT_QUATERNION")
		{
			NewValue.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
			ZE_TEST_CHECK_EQUAL(Variant.GetQuaternion(), NewValue.GetQuaternion());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR2")
		{
			NewValue.SetVector2(ZEVector2(3.0f, 5.0f));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
			ZE_TEST_CHECK_EQUAL(Variant.GetVector2(), NewValue.GetVector2());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR3")
		{
			NewValue.SetVector3(ZEVector3(3.0f, 5.0f, 8.0f));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR3);
			ZE_TEST_CHECK_EQUAL(Variant.GetVector3(), NewValue.GetVector3());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR4")
		{
			NewValue.SetVector4(ZEVector4(3.0f, 5.0f, 8.0f, 11.0f));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);
			ZE_TEST_CHECK_EQUAL(Variant.GetVector4(), NewValue.GetVector4());
		}

		ZE_TEST_CASE("ZE_VRT_MARTIX3X3")
		{
			NewValue.SetMatrix3x3(ZEMatrix3x3(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
			ZE_TEST_CHECK_EQUAL(Variant.GetMatrix3x3(), NewValue.GetMatrix3x3());
		}

		ZE_TEST_CASE("ZE_VRT_MARTIX4X4")
		{
			NewValue.SetMatrix4x4(ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f,   
				5.0f, 8.0f, 6.0f, 9.0f,   
				13.0f, 3.0f, 4.0f, 5.0f,   
				12.0f, 2.0f, 8.0f, 6.0f));

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);
			ZE_TEST_CHECK_EQUAL(Variant.GetMatrix4x4(), NewValue.GetMatrix4x4());
		}

		ZE_TEST_CASE("ZE_VRT_CLASS")
		{

		}

		ZE_TEST_CASE("set type without set value")
		{
			NewValue.SetType(ZE_VRT_QUATERNION);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
			ZE_TEST_CHECK_EQUAL(Variant.GetQuaternion(), ZEQuaternion(0.0f, 5.0f, 8.0f, 11.0f));

			NewValue.SetType(ZE_VRT_INTEGER_64);

			Variant.SetVariant(NewValue);
			ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZEInt64 IntValue = Variant.GetInt64();
			ZE_TEST_CHECK_EQUAL(IntValue, 4656722014701092864);
		}

		ZE_TEST_CASE("set type without set value for new variants")
		{
			ZEVariant Variant1;
			ZEVariant NewValue1;

			NewValue1.SetType(ZE_VRT_QUATERNION);
			Variant1.SetVariant(NewValue1);
			ZEQuaternion Quaternion = Variant1.GetQuaternion();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_QUATERNION);

			NewValue1.SetType(ZE_VRT_BOOLEAN);
			Variant1.SetVariant(NewValue1);
			bool Boolean = Variant1.GetBoolean();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_BOOLEAN);

			NewValue1.SetType(ZE_VRT_DOUBLE);
			Variant1.SetVariant(NewValue1);
			double Double = Variant1.GetDouble();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_DOUBLE);

			NewValue1.SetType(ZE_VRT_FLOAT);
			Variant1.SetVariant(NewValue1);
			float Float = Variant1.GetFloat();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_FLOAT);

			NewValue1.SetType(ZE_VRT_INTEGER_16);
			Variant1.SetVariant(NewValue1);
			ZEInt16 I16 = Variant1.GetInt16();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_INTEGER_16);

			NewValue1.SetType(ZE_VRT_INTEGER_32);
			Variant1.SetVariant(NewValue1);
			ZEInt32 I32 = Variant1.GetInt32();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_INTEGER_32);

			NewValue1.SetType(ZE_VRT_INTEGER_64);
			Variant1.SetVariant(NewValue1);
			ZEInt64 I64 = Variant1.GetInt64();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_INTEGER_64);

			NewValue1.SetType(ZE_VRT_INTEGER_8);
			Variant1.SetVariant(NewValue1);
			ZEInt8 I8 = Variant1.GetInt8();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_INTEGER_8);

			NewValue1.SetType(ZE_VRT_MATRIX3X3);
			Variant1.SetVariant(NewValue1);
			ZEMatrix3x3 Matrix3 = Variant1.GetMatrix3x3();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_MATRIX3X3);

			NewValue1.SetType(ZE_VRT_MATRIX4X4);
			Variant1.SetVariant(NewValue1);
			ZEMatrix4x4 Matrix4 = Variant1.GetMatrix4x4();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_MATRIX4X4);

 			NewValue1.SetType(ZE_VRT_STRING);
 			Variant1.SetVariant(NewValue1);
 			ZEString String = Variant1.GetString();
 			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
 			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_STRING);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_16);
			Variant1.SetVariant(NewValue1);
			ZEUInt16 UI16 = Variant1.GetUInt16();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			Variant1.SetVariant(NewValue1);
			ZEUInt32 UI32 = Variant1.GetUInt32();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_64);
			Variant1.SetVariant(NewValue1);
			ZEUInt64 UI64 = Variant1.GetUInt64();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_8);
			Variant1.SetVariant(NewValue1);
			ZEUInt8 UI8 = Variant1.GetUInt8();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);

			NewValue1.SetType(ZE_VRT_VECTOR2);
			Variant1.SetVariant(NewValue1);
			ZEVector2 Vector2 = Variant1.GetVector2();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_VECTOR2);

			NewValue1.SetType(ZE_VRT_VECTOR3);
			Variant1.SetVariant(NewValue1);
			ZEVector3 Vector3 = Variant1.GetVector3();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_VECTOR3);

			NewValue1.SetType(ZE_VRT_VECTOR4);
			Variant1.SetVariant(NewValue1);
			ZEVector4 Vector4 = Variant1.GetVector4();
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), NewValue1.GetType());
			ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_VECTOR4);
		}
	}

	ZE_TEST_ITEM("void ZEVariant::SetVector2(const ZEVector2& Value)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant.SetVector2(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector2(), Vector);
	}

	ZE_TEST_ITEM("void ZEVariant::SetVector3(const ZEVector3& Value)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant.SetVector3(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR3);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector3(), Vector);
	}

	ZE_TEST_ITEM("void ZEVariant::SetVector4(const ZEVector4& Value)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant.SetVector4(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector4(), Vector);
	}

	ZE_TEST_ITEM("ZESize ZEVariant::SizeOf() const")
	{
		ZEVariant Variant;
		ZEInt Size;

		ZE_TEST_CASE("ZE_VRT_UNDEFINED")
		{
			Variant.SetType(ZE_VRT_UNDEFINED);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 0);
		}

		ZE_TEST_CASE("ZE_VRT_NULL")
		{
			Variant.SetType(ZE_VRT_NULL);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 0);
		}

		ZE_TEST_CASE("ZE_VRT_FLOAT")
		{
			Variant.SetType(ZE_VRT_FLOAT);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 4);
		}

		ZE_TEST_CASE("ZE_VRT_DOUBLE")
		{
			Variant.SetType(ZE_VRT_DOUBLE);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 8);
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_8")
		{
			Variant.SetType(ZE_VRT_INTEGER_8);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 1);
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_16")
		{
			Variant.SetType(ZE_VRT_INTEGER_16);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 2);
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_32")
		{
			Variant.SetType(ZE_VRT_INTEGER_32);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 4);
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_64")
		{
			Variant.SetType(ZE_VRT_INTEGER_64);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 8);
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_8);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 1);
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_16);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 2);
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_32);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 4);
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_64);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 8);
		}

		ZE_TEST_CASE("ZE_VRT_BOOLEAN")
		{
			Variant.SetType(ZE_VRT_BOOLEAN);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 1);
		}

		ZE_TEST_CASE("ZE_VRT_STRING")
		{
			Variant = "String";

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 7);

			Variant = "TestString";

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 11);
		}

		ZE_TEST_CASE("ZE_VRT_QUATERNION")
		{
			Variant.SetType(ZE_VRT_QUATERNION);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 16);
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR2")
		{
			Variant.SetType(ZE_VRT_VECTOR2);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 8);
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR3")
		{
			Variant.SetType(ZE_VRT_VECTOR3);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 12);
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR4")
		{
			Variant.SetType(ZE_VRT_VECTOR4);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 16);
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX3X3")
		{
			Variant.SetType(ZE_VRT_MATRIX3X3);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 36);
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX4X4")
		{
			Variant.SetType(ZE_VRT_MATRIX4X4);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 64);
		}

		ZE_TEST_CASE("ZE_VRT_CLASS")
		{
			Variant.SetType(ZE_VRT_CLASS);

			Size = Variant.SizeOf();
			ZE_TEST_CHECK_EQUAL(Size, 4);
		}
	}

	ZE_TEST_ITEM("bool ZEVariant::Unserialize(ZEUnserializer* Unserializer)")
	{
		ZEFile File;
		bool Opened = File.Open("VariantTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		bool Serialized;
		bool Unserialized;
		ZEInt64 LastItem;
		ZEInt Seek;

		ZE_TEST_CASE("ZE_VRT_FLOAT")
		{
			ZEVariant Variant1(3.8f);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
  			ZE_TEST_CHECK_EQUAL(Variant2.GetFloat(), Variant1.GetFloat());
		}

		ZE_TEST_CASE("ZE_VRT_DOUBLE")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1(1.2);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant2.GetDouble(), Variant1.GetDouble());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_8")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEInt8 Value = 16;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetInt8(), Variant1.GetInt8());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_16")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEInt16 Value = 30;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetInt16(), Variant1.GetInt16());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_32")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEInt32 Value = 0;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetInt32(), Variant1.GetInt32());
		}

		ZE_TEST_CASE("ZE_VRT_INTEGER_64")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEInt64 Value = 40;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetInt64(), Variant1.GetInt64());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEUInt8 Value = 200;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetUInt8(), Variant1.GetUInt8());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEUInt16 Value = 640;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetUInt16(), Variant1.GetUInt16());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEUInt32 Value = 4300;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetUInt32(), Variant1.GetUInt32());
		}

		ZE_TEST_CASE("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEUInt64 Value = 2000000000;
			ZEVariant Variant1 = Value;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetUInt64(), Variant1.GetUInt64());
		}

		ZE_TEST_CASE("ZE_VRT_BOOLEAN")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1(false);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetBoolean(), Variant1.GetBoolean());
		}

		ZE_TEST_CASE("ZE_VRT_STRING")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1("Test");

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_STRING_EQUAL(Variant2.GetString(), Variant1.GetString());
		}

		ZE_TEST_CASE("ZE_VRT_QUATERNION")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
			ZEVariant Variant1(Quaternion);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetQuaternion(), Variant1.GetQuaternion());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR2")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVector2 Vector(3.0f, 0.0f);
			ZEVariant Variant1(Vector);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetVector2(), Variant1.GetVector2());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR3")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVector3 Vector(3.0f, 0.0f, 1.0f);
			ZEVariant Variant1(Vector);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetVector3(), Variant1.GetVector3());
		}

		ZE_TEST_CASE("ZE_VRT_VECTOR4")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVector4 Vector(3.0f, 0.0f, 1.0f, 0.0f);
			ZEVariant Variant1(Vector);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetVector4(), Variant1.GetVector4());
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX3X3")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEMatrix3x3 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;

			ZEVariant Variant1 = Matrix;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetMatrix3x3(), Variant1.GetMatrix3x3());
		}

		ZE_TEST_CASE("ZE_VRT_MATRIX4X4")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEMatrix4x4 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Matrix.M14 = 4;

			ZEVariant Variant1 = Matrix;

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetMatrix4x4(), Variant1.GetMatrix4x4());
		}

		ZE_TEST_CASE("ZE_VRT_CLASS")
		{

		}

		ZE_TEST_CASE("ZE_VRT_NULL")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1;
			Variant1.SetType(ZE_VRT_NULL);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetType(), Variant1.GetType());
		}

		ZE_TEST_CASE("ZE_VRT_UNDEFINED")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1;
			Variant1.SetType(ZE_VRT_UNDEFINED);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZE_TEST_CHECK_ERROR(Unserialized);
			ZE_TEST_CHECK_EQUAL(Variant2.GetType(), Variant1.GetType());
		}

		File.Close();
		remove("VariantTests.txt");
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant()")
	{
		ZEVariant Variant;
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNDEFINED);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEVariant& Value)")
	{
		ZEVariant Value;
		Value.SetBoolean(true);

		ZEVariant Variant(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), Value.GetType());
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZE_TEST_CHECK_ERROR(Variant.GetBoolean(), Value.GetBoolean());
		ZE_TEST_CHECK_ERROR(Variant.GetBoolean());

		Value.SetString("TestString");
		ZEVariant Variant1(Value);
		ZE_TEST_CHECK_EQUAL(Variant1.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant1.GetString(), "TestString");
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(bool Value)")
	{
		bool InitialValue = true;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZE_TEST_CHECK_ERROR(Variant.GetBoolean());
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const char* Value)")
	{
		const char* InitialValue = "String";

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_STRING_EQUAL(Variant.GetString(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant(const ZEString& Value)")
	{
		const ZEString Value = "";

		ZEVariant Variant(Value);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_STRING);
		ZE_TEST_CHECK_ERROR(Variant.GetString() == "");
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEMatrix3x3& Value)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEVariant Variant(Matrix);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix3x3(), Matrix);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEMatrix4x4& Value)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		ZEVariant Variant(Matrix);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZE_TEST_CHECK_EQUAL(Variant.GetMatrix4x4(), Matrix);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		ZEVariant Variant(Quaternion);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_QUATERNION);
		ZE_TEST_CHECK_EQUAL(Variant.GetQuaternion(), Quaternion);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEVariant& Value)")
	{
		ZEVariant InitialValue;
		InitialValue.SetInt16(255);

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
 		ZE_TEST_CHECK_EQUAL(Variant.GetInt16(), InitialValue.GetInt16());
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEVector2& Value)")
	{
		ZEVector2 Vector(1.0f, 2.0f);

		ZEVariant Variant(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR2);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector2(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEVector3& Value)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEVariant Variant(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR3);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector3(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(const ZEVector4& Value)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEVariant Variant(Vector);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_VECTOR4);
		ZE_TEST_CHECK_EQUAL(Variant.GetVector4(), Vector);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(float Value)")
	{
		float InitialValue = 255.349f;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_FLOAT);
		ZE_TEST_CHECK_EQUAL(Variant.GetFloat(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(double Value)")
	{
		double InitialValue = 12.56;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_DOUBLE);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Variant.GetDouble(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEInt8 Value)")
	{
		ZEInt8 InitialValue = 16;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt8(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEInt16 Value)")
	{
		ZEInt16 InitialValue = 30;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt16(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEInt32 Value)")
	{
		ZEInt32 InitialValue = 0;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt32(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEInt64 Value)")
	{
		ZEInt64 InitialValue = 40;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetInt64(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEUInt8 Value)")
	{
		ZEUInt8 InitialValue = 200;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt8(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEUInt16 Value)")
	{
		ZEUInt16 InitialValue = 640;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt16(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEUInt32 Value)")
	{
		ZEUInt32 InitialValue = 43000;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt32(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEUInt64 Value)")
	{
		ZEUInt64 InitialValue = 2000000000;

		ZEVariant Variant(InitialValue);
		ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZE_TEST_CHECK_EQUAL(Variant.GetUInt64(), InitialValue);
	}

	ZE_TEST_ITEM("ZEVariant::ZEVariant(ZEObject* Value)")
	{
		ZEObject* Class;

		ZEVariant Variant(&Class);
		//ZE_VRT_BOOLEAN
		//ZE_TEST_CHECK_EQUAL(Variant.GetType(), ZE_VRT_CLASS);
	}
}
