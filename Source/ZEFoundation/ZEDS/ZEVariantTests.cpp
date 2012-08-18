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
#include "ZETypedVariant.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAngle.h"
#include "ZEFile/ZEFile.h"
#include "ZEMeta/ZEObject.h"
#include "ZEModel/ZEModel.h"

ZETestSuite(ZEVariant)
{
	ZETest("ZEVariant::~ZEVariant()")
	{
		const char* Value = "String";
		ZEVariant Variant(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
		ZETestCheckString(Variant.GetString(), Value);

		Variant.~ZEVariant();
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNDEFINED);
		ZEString Res = Variant.GetString(); //(Bad Ptr) error Variant type mismatched
	}

	ZETest("bool ZEVariant::GetBoolean() const")
	{
		ZEVariant Variant;
		bool Value = false;
		Variant.SetBoolean(Value);

		bool Result = Variant.GetBoolean();
		ZETestCheckEqual(Result, false);
	}

	ZETest("float ZEVariant::GetFloat() const")
	{
		ZEVariant Variant;
		float Value = 0.8f;
		Variant.SetFloat(Value);

		float Result = Variant.GetFloat();
		ZETestCheckClose(Result, 0.8f);
	}

	ZETest("double ZEVariant::GetDouble() const")
	{
		ZEVariant Variant;
		double Value = 1.3;
		Variant.SetDouble(Value);

		double Result = Variant.GetDouble();
		ZETestCheckClose(Result, 1.3);
	}

	ZETest("ZEInt8 ZEVariant::GetInt8() const")
	{
		ZEVariant Variant;
		ZEInt8 Value = 100;
		Variant.SetInt8(Value);

		ZEInt8 Result = Variant.GetInt8();
		ZETestCheckEqual(Result, 100);

		ZETestCase("for value bigger than upper limit of ZEInt8")
		{
			Value = 130;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, -126);

			Value = 127;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, 127);

			Value = 128;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, -128);
		}

		ZETestCase("for value lower than lower limit of ZEInt8")
		{
			Value = -130;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, 126);

			Value = -128.9;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, -128);

			Value = -129;
			Variant.SetInt8(Value);

			Result = Variant.GetInt8();
			ZETestCheckEqual(Result, 127);
		}
	}

	ZETest("ZEInt16 ZEVariant::GetInt16() const")
	{
		ZEVariant Variant;
		ZEInt16 Value = 30600;
		Variant.SetInt16(Value);

		ZEInt16 Result = Variant.GetInt16();
		ZETestCheckEqual(Result, 30600);

		Value = 32767;
		Variant.SetInt16(Value);
		Result = Variant.GetInt16();
		ZETestCheckEqual(Result, 32767);

		Variant.SetInt16(32767.9);
		Result = Variant.GetInt16();
		ZETestCheckEqual(Result, 32767);

		Variant.SetInt16(-32768);
		Result = Variant.GetInt16();
		ZETestCheckEqual(Result, -32768);

		ZETestCase("for value bigger than upper limit of ZEInt16")
		{
			Value = 34780;
			Variant.SetInt16(Value);

			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, -30756);

			Variant.SetInt16(32768);
			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, -32768);

			Variant.SetInt16(65535);
			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, -1);

			Variant.SetInt16(65536);
			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, 0);
		}

		ZETestCase("for value lower than lower limit of ZEInt16")
		{
			Value = -33780;
			Variant.SetInt16(Value);

			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, 31756);

			Variant.SetInt16(-32769);
			Result = Variant.GetInt16();
			ZETestCheckEqual(Result, 32767);
		}
	}

	ZETest("ZEInt32 ZEVariant::GetInt32() const")
	{
		ZEVariant Variant;
		ZEInt32 Value = 5;
		Variant.SetInt32(Value);

		ZEInt32 Result = Variant.GetInt32();
		ZETestCheckEqual(Result, 5);

		Variant.SetInt32(2147483647);
		Result = Variant.GetInt32();
		ZETestCheckEqual(Result, 2147483647);

		ZETestCase("for value bigger than upper limit of ZEInt32")
		{
			Value = 2547483647;
			Variant.SetInt32(Value);

			Result = Variant.GetInt32();
			ZETestCheckEqual(Result, -1747483649);

			Variant.SetInt32(2147483648);
			Result = Variant.GetInt32();
			ZETestCheckEqual(Result, -2147483648);
		}

		ZETestCase("for value lower than lower limit of ZEInt32")
		{
			Value = -2547483647;
			Variant.SetInt32(Value);

			Result = Variant.GetInt32();
			ZETestCheckEqual(Result, 1747483649);

			Value = -2147483649;
			Variant.SetInt32(Value);
			Result = Variant.GetInt32();
			ZETestCheckEqual(Result, 2147483647);
		}
	}

	ZETest("ZEInt64 ZEVariant::GetInt64() const")
	{
		ZEVariant Variant;
		ZEInt64 Value = 500;
		Variant.SetInt64(Value);

		ZEInt64 Result = Variant.GetInt64();
		ZETestCheckEqual(Result, 500);

		Value = -9223372036854775808;
		Variant.SetInt64(Value);
		Result = Variant.GetInt64();
		ZETestCheckEqual(Result, -9223372036854775808);

		ZETestCase("for value bigger than upper limit of ZEInt64")
		{
			Value = 9323372036854775807;
			Variant.SetInt64(Value);

			Result = Variant.GetInt64();
			ZETestCheckEqual(Result, -9123372036854775809);
		}

		ZETestCase("for value lower than lower limit of ZEInt64")
		{
			Value = -9223372036854775809;
			Variant.SetInt64(Value);

			Result = Variant.GetInt64();
			ZETestCheckEqual(Result, 9223372036854775807);
		}
	}

	ZETest("ZEUInt8 ZEVariant::GetUInt8() const")
	{
		ZEVariant Variant;
		ZEUInt8 Value = 200;
		Variant.SetUInt8(Value);

		ZEUInt8 Result = Variant.GetUInt8();
		ZETestCheckEqual(Result, 200);

		ZETestCase("for value bigger than upper limit of ZEUInt8")
		{
			Value = 256;
			Variant.SetUInt8(Value);

			Result = Variant.GetUInt8();
			ZETestCheckEqual(Result, 0);

			Variant.SetUInt8(3468);
			Result = Variant.GetUInt8();
			ZETestCheckEqual(Result, 140);
		}

		ZETestCase("for value lower than lower limit of ZEUInt8")
		{
			Value = -3;
			Variant.SetUInt8(Value);

			Result = Variant.GetUInt8();
			ZETestCheckEqual(Result, 253);
		}
	}

	ZETest("ZEUInt16 ZEVariant::GetUInt16() const")
	{
		ZEVariant Variant;
		ZEUInt16 Value = 500;
		Variant.SetUInt16(Value);

		ZEUInt16 Result = Variant.GetUInt16();
		ZETestCheckEqual(Result, 500);

		ZETestCase("for value bigger than upper limit of ZEUInt16")
		{
			Value = 66000;
			Variant.SetUInt16(Value);

			Result = Variant.GetUInt16();
			ZETestCheckEqual(Result, 464);
		}

		ZETestCase("for value lower than lower limit of ZEUInt16")
		{
			Value = -10;
			Variant.SetUInt16(Value);

			Result = Variant.GetUInt16();
			ZETestCheckEqual(Result, 65526);
		}
	}

	ZETest("ZEUInt32 ZEVariant::GetUInt32() const")
	{
		ZEVariant Variant;
		ZEUInt32 Value = 555;
		Variant.SetUInt32(Value);

		ZEUInt32 Result = Variant.GetUInt32();
		ZETestCheckEqual(Result, 555);

		ZETestCase("for value bigger than upper limit of ZEUInt32")
		{
			Value = 4594967295;
			Variant.SetUInt32(Value);

			Result = Variant.GetUInt32();
			ZETestCheckEqual(Result, 299999999);
		}

		ZETestCase("for value lower than lower limit of ZEUInt32")
		{
			Value = -10;
			Variant.SetUInt32(Value);

			Result = Variant.GetUInt32();
			ZETestCheckEqual(Result, 4294967286);
		}
	}

	ZETest("ZEUInt64 ZEVariant::GetUInt64() const")
	{
		ZEVariant Variant;
		ZEUInt64 Value = 1300;
		Variant.SetUInt64(Value);

		ZEUInt64 Result = Variant.GetUInt64();
		ZETestCheckEqual(Result, 1300);

		ZETestCase("for value bigger than upper limit of ZEUIn64")
		{
			Value = 18446744073709551615.9;
			Variant.SetUInt64(Value);

			Result = Variant.GetUInt64();
			ZETestCheckEqual(Result, 384);
		}

		ZETestCase("for value lower than lower limit of ZEUInt64")
		{
			Value = -10;
			Variant.SetUInt64(Value);

			Result = Variant.GetUInt64();
			ZETestCheckEqual(Result, 18446744073709551606);
		}
	}

	ZETest("ZEMatrix3x3& ZEVariant::GetMatrix3x3() const")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		Variant.SetMatrix3x3(Matrix);

		ZEMatrix3x3 Result = Variant.GetMatrix3x3();
		ZETestCheckEqual(Result, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZETest("ZEMatrix4x4& ZEVariant::GetMatrix4x4() const")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix( 1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);

		ZEMatrix4x4 Result = Variant.GetMatrix4x4();
		ZETestCheckEqual(Result, ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 6.0f, 9.0f, 13.0f, 3.0f, 4.0f, 5.0f, 12.0f, 2.0f, 8.0f, 6.0f));
	}

	ZETest("ZEQuaternion& ZEVariant::GetQuaternion() const")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_4, ZEVector3::UnitY);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = Variant.GetQuaternion();
		ZETestCheckEqual(Result, ZEQuaternion(0.92387956f, 0.0f, 0.38268349f, 0.0f));
	}

	ZETest("const ZEString& ZEVariant::GetString() const")
	{
		ZEVariant Variant;
		ZEString String = "String";
		Variant.SetString(String);

		ZEString Result;
		Result = Variant.GetString();
		ZETestCheckString(Result, String);
	}

	ZETest("ZEVariantType ZEVariant::GetType() const")
	{
		ZEVariant Variant;
		ZEInt32 i = 6;
		Variant.SetInt32(i);

		ZEVariantType Result = Variant.GetType();
		ZETestCheckEqual(Result, ZE_VRT_INTEGER_32);
	}

	ZETest("ZEVariant::ZEVariantValue ZEVariant::GetValue()	const")
	{
		ZEVariant Variant;
		ZEInt32 i = 6;
		Variant.SetInt32(i);

		ZEVariant::ZEVariantValue Result = Variant.GetValue();
		ZETestCheckEqual(Result.Int32, 6);

		ZETestCase("for string")
		{
			Variant.SetString("VariantTest");

			Result = Variant.GetValue();
			ZETestCheckString(Result.String, "VariantTest");
		}

		ZETestCase("for ZEVariant(const char*)")
		{
			ZEVariant Variant1("Test");

			Result = Variant1.GetValue();
			ZETestCheckString(Result.String, "Test");
		}
	}

	ZETest("ZEVector2& ZEVariant::GetVector2() const")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);
		Variant.SetVector2(Vector);

		ZEVector2 Result = Variant.GetVector2();
		ZETestCheckEqual(Result, ZEVector2(1.0f, 2.0f));
	}

	ZETest("ZEVector3& ZEVariant::GetVector3() const")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		Variant.SetVector3(Vector);

		ZEVector3 Result = Variant.GetVector3();
		ZETestCheckEqual(Result, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZETest("ZEVector4& ZEVariant::GetVector4() const")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		Variant.SetVector4(Vector);

		ZEVector4 Result = Variant.GetVector4();
		ZETestCheckEqual(Result, ZEVector4(1.0f, 2.0f, 3.0f, 4.0f));
	}

	ZETest("ZEObject* ZEVariant::GetClass() const")
	{
		ZEVariant Variant;

		ZEObject* Class;
		

	}

	ZETest("ZEVariant::ZEVariantValue& ZEVariant::ImplicitAcesss()")
	{
		ZEVariant Variant;
		ZEInt32 i = 6;
		Variant.SetInt32(i);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);

		ZEVariant::ZEVariantValue Result = Variant.ImplicitAcesss();
		ZETestCheckEqual(Result.Int32, 6);

		ZETestCase("for ZE_VRT_STRING")
		{
			Variant = "TestString";
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);

			Result = Variant.ImplicitAcesss();
			ZETestCheckString(Result.String, "TestString");
			ZETestCheckString(Variant.GetString(), "TestString");
		}
 	}

	ZETest("bool ZEVariant::IsNull() const")
	{
		ZEVariant Variant;
		Variant.SetNull();

		bool Result = Variant.IsNull();
		ZETestCheck(Result);

		ZETestCase("for type not null")
		{
			Variant.SetInt32(5);

			Result = Variant.IsNull();
			ZETestCheck(!Result);
 		}	
	}

	ZETest("void ZEVariant::operator=(bool NewValue)")
	{
		ZEVariant Variant;
		bool NewValue = false;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZETestCheck(!Variant.GetBoolean());
	}

	ZETest("void ZEVariant::operator=(const char* NewValue)")
	{
		ZEVariant Variant;
		const char* NewValue = "String";

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
		ZETestCheckString(Variant.GetString(), "String");

		ZETestCase("for ZEString")
		{
			Variant.SetType(ZE_VRT_VECTOR4);
			ZEString String = "TestString";
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);

			Variant = String;
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), String);
			ZETestCheckString(Variant.GetString(), "TestString");
		}

		ZETestCase("for char* ")
		{
			Variant.SetType(ZE_VRT_BOOLEAN);
			char* String = "NewString";
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);

			Variant = String;
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), String);
			ZETestCheckString(Variant.GetString(), "NewString");
		}
	}

	ZETest("void ZEVariant::operator=(const ZEMatrix3x3& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix( 1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant = Matrix;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZETestCheckEqual(Variant.GetMatrix3x3(), Matrix);
	}

	ZETest("void ZEVariant::operator=(const ZEMatrix4x4& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant = Matrix;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZETestCheckEqual(Variant.GetMatrix4x4(), Matrix);
	}

	ZETest("void ZEVariant::operator=(const ZEQuaternion& Quaternion)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

		Variant = Quaternion;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
		ZETestCheckEqual(Variant.GetQuaternion(), Quaternion);
	}

	ZETest("void ZEVariant::operator=(const ZEVariant& NewValue)")
	{
		ZEVariant Variant;
		ZEVariant NewValue;
		NewValue.SetInt32(5);

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZETestCheckEqual(Variant.GetInt32(), 5);
	}

	ZETest("void ZEVariant::operator=(const ZEVector2& Vector)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant = Vector;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR2);
		ZETestCheckEqual(Variant.GetVector2(), Vector);
	}

	ZETest("void ZEVariant::operator=(const ZEVector3& Vector)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant = Vector;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR3);
		ZETestCheckEqual(Variant.GetVector3(), Vector);
	}

	ZETest("void ZEVariant::operator=(const ZEVector4& Vector)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant = Vector;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);
		ZETestCheckEqual(Variant.GetVector4(), Vector);
	}

	ZETest("void ZEVariant::operator=(float NewValue)")
	{
		ZEVariant Variant;
		float NewValue = 3.8f;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);
		ZETestCheckEqual(Variant.GetFloat(), NewValue);
	}

	ZETest("void ZEVariant::operator=(double NewValue)")
	{
		ZEVariant Variant;
		double NewValue = 3.8;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
		ZETestCheckEqual(Variant.GetDouble(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEInt8 NewValue)")
	{
		ZEVariant Variant;
		ZEInt8 NewValue = 110;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZETestCheckEqual(Variant.GetInt8(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEInt16 NewValue)")
	{
		ZEVariant Variant;
		ZEInt16 NewValue = 1000;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZETestCheckEqual(Variant.GetInt16(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEInt32 NewValue)")
	{
		ZEVariant Variant;
		ZEInt32 NewValue = 1000000000;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZETestCheckEqual(Variant.GetInt32(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEInt64 NewValue)")
	{
		ZEVariant Variant;
		ZEInt64 NewValue = 700;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZETestCheckEqual(Variant.GetInt64(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEUInt8 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt8 NewValue = 210;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZETestCheckEqual(Variant.GetUInt8(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEUInt16 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt16 NewValue = 210;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZETestCheckEqual(Variant.GetUInt16(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEUInt32 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt32 NewValue = 550;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZETestCheckEqual(Variant.GetUInt32(), NewValue);
	}

	ZETest("void ZEVariant::operator=(ZEUInt64 NewValue)")
	{
		ZEVariant Variant;
		ZEUInt64 NewValue = 1550;

		Variant = NewValue;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZETestCheckEqual(Variant.GetUInt64(), NewValue);
	}

	ZETest("ZEVariant::operator bool()")
	{
		ZEVariant Variant;
		Variant.SetBoolean(true);

		bool Result = (bool)Variant;
		ZETestCheck(Result);
	}

	ZETest("ZEVariant::operator const char*()")
	{
		ZEVariant Variant;
		Variant.SetString("String");

		const char* Result = (const char*)Variant;
		ZETestCheckString(Result, Variant.GetString());
		ZETestCheckString(Result, "String");

		ZETestCase("for ZEString")
		{
			ZEString String = "TestString";
			Variant.SetString(String);

			Result = (const char*)Variant;
			ZETestCheckString(Result, String);
			ZETestCheckString(Result, "TestString");
			ZETestCheckString(Result, Variant.GetString());
		}

		ZETestCase("for char* ")
		{
			char* String = "NewString";
			Variant.SetString(String);

			Result = (const char*)Variant;
			ZETestCheckString(Result, String);
			ZETestCheckString(Result, "NewString");
			ZETestCheckString(Result, Variant.GetString());
			ZETestCheckString(Variant.GetString(), String);
		}
	}

	ZETest("ZEVariant::operator float()")
	{
		ZEVariant Variant;
		Variant.SetFloat(5.55f);

		float Result =(float)Variant;
		ZETestCheckEqual(Result, Variant.GetFloat());

		ZETestCase("for ZEInt32")
		{
			Variant.SetInt32(5);

			Result = (float)Variant;
			ZETestCheckClose(Result, 5.0f);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZEVariant::ZEVariantValue Value = Variant.GetValue();
			ZETestCheckEqual(Value.Int32, 5);
		}
	}

	ZETest("ZEVariant::operator double()")
	{
		ZEVariant Variant;
		Variant.SetDouble(5.55);

		double Result = (double)Variant;
		ZETestCheckClose(Result, Variant.GetDouble());

		ZETestCase("for ZEInt32")
		{
			Variant.SetInt32(3);

			Result = (double)Variant;
			ZETestCheckClose(Result, 3.0);
			ZEVariant::ZEVariantValue Value = Variant.GetValue();
			ZETestCheckClose(Value.Double, 5.55);
			ZETestCheckEqual(Value.Int32, 3);
		}
	}

	ZETest("ZEVariant::operator ZEInt8()")
	{
		ZEVariant Variant;
		Variant.SetInt8(10);

		ZEInt8 Result = (ZEInt8)Variant;
		ZETestCheckEqual(Result, Variant.GetInt8());

		ZETestCase("for float")
		{
			Variant.SetFloat(13.2f);

			Result = (ZEInt8)Variant;
			ZETestCheckEqual(Result, 13);
			ZETestCheckEqual(Variant.GetFloat(), 13.2f);
		}
	}

	ZETest("ZEVariant::operator ZEInt16()")
	{
		ZEVariant Variant;
		Variant.SetInt16(150);

		ZEInt16 Result = (ZEInt16)Variant;
		ZETestCheckEqual(Result, Variant.GetInt16());

		ZETestCase("for double")
		{
			Variant.SetDouble(120.2);

			Result = (ZEInt16)Variant;
			ZETestCheckEqual(Result, 120);
			ZETestCheckClose(Variant.GetDouble(), 120.2);
		}
	}

	ZETest("ZEVariant::operator ZEInt32()")
	{
		ZEVariant Variant;
		Variant.SetInt32(560);

		ZEInt32 Result = (ZEInt32)Variant;
		ZETestCheckEqual(Result, Variant.GetInt32());

		ZETestCase("for float")
		{
			Variant.SetFloat(510.3f);

			Result = (ZEInt32)Variant;
			ZETestCheckEqual(Result, 510);
			ZETestCheckClose(Variant.GetFloat(), 510.3f);
		}
	}

	ZETest("ZEVariant::operator ZEInt64()")
	{
		ZEVariant Variant;
		Variant.SetInt64(10);

		ZEInt64 Result = (ZEInt64)Variant;
		ZETestCheckEqual(Result, Variant.GetInt64());

		ZETestCase("for double")
		{
			Variant.SetDouble(11.1);

			Result = (ZEInt64)Variant;
			ZETestCheckEqual(Result, 11);
			ZETestCheckClose(Variant.GetDouble(), 11.1);
		}
	}

	ZETest("ZEVariant::operator ZEUInt8()")
	{
		ZEVariant Variant;
		Variant.SetUInt8(3);

		ZEUInt8 Result = (ZEUInt8)Variant;
		ZETestCheckEqual(Result, Variant.GetUInt8());

		ZETestCase("for ZEInt8")
		{
			Variant.SetInt8(-120);

			Result = (ZEUInt8)Variant;
			ZETestCheckEqual(Result, 136);
			ZETestCheckEqual(Variant.GetInt8(), -120);
		}
	}

	ZETest("ZEVariant::operator ZEUInt16()")
	{
		ZEVariant Variant;
		Variant.SetUInt16(5);

		ZEUInt16 Result = (ZEUInt16)Variant;
		ZETestCheckEqual(Result, Variant.GetUInt16());

		ZETestCase("for ZEInt8")
		{
			Variant.SetInt8(120);

			Result = (ZEUInt16)Variant;
			ZETestCheckEqual(Result, 120);
			ZETestCheckEqual(Variant.GetInt8(), 120);
		}
	}

	ZETest("ZEVariant::operator ZEUInt32()")
	{
		ZEVariant Variant;
		Variant.SetUInt32(20);

		ZEUInt32 Result = (ZEUInt32)Variant;
		ZETestCheckEqual(Result, Variant.GetUInt32());

		ZETestCase("for double")
		{
			Variant.SetDouble(26.1);

			Result = (ZEUInt32)Variant;
			ZETestCheckEqual(Result, 26);
			ZETestCheckClose(Variant.GetDouble(), 26.1);
		}
	}

	ZETest("ZEVariant::operator ZEUInt64()")
	{
		ZEVariant Variant;
		Variant.SetUInt64(160);

		ZEUInt64 Result = (ZEUInt64)Variant;
		ZETestCheckEqual(Result, Variant.GetUInt64());

		ZETestCase("for ZEInt64")
		{
			Variant.SetInt64(-10);

			Result = (ZEUInt64)Variant;
			ZETestCheckEqual(Result, 18446744073709551606);
			ZETestCheckEqual(Variant.GetInt64(), -10);
		}
	}

	ZETest("ZEVariant::operator ZEMatrix3x3()")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);

		ZEMatrix3x3 Result = (ZEMatrix3x3)Variant;
		ZETestCheckEqual(Result, Variant.GetMatrix3x3());
	}

	ZETest("ZEVariant::operator ZEMatrix4x4()")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
			5.0f, 8.0f, 6.0f, 9.0f,   
			13.0f, 3.0f, 4.0f, 5.0f,   
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);

		ZEMatrix4x4 Result = (ZEMatrix4x4)Variant;
		ZETestCheckEqual(Result, Variant.GetMatrix4x4());
	}

	ZETest("ZEVariant::operator ZEQuaternion()")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = (ZEQuaternion)Variant;
		ZETestCheckEqual(Result, Variant.GetQuaternion());
	}

	ZETest("ZEVariant::operator ZEVector2()")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);
		Variant.SetVector2(Vector);

		ZEVector2 Result = (ZEVector2)Variant;
		ZETestCheckEqual(Result, Variant.GetVector2());
	}

	ZETest("ZEVariant::operator ZEVector3()")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		Variant.SetVector3(Vector);

		ZEVector3 Result = (ZEVector3)Variant;
		ZETestCheckEqual(Result, Variant.GetVector3());
	}

	ZETest("ZEVariant::operator ZEVector4()")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		Variant.SetVector4(Vector);

		ZEVector4 Result = (ZEVector4)Variant;
		ZETestCheckEqual(Result, Variant.GetVector4());
	}

	ZETest("ZEVariant::operator ZEObject*()")
	{
		ZEVariant Variant;

	}

	ZETest("bool ZEVariant::Serialize(ZESerializer* Serializer)")
	{
		ZEFile File;
		File.Open("ZEVariantTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		ZEVariant Variant;

		ZETestCase("for ZEInt8")
		{
			Variant.SetInt8(16);

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZETestCheck(Result);	
		}

		ZETestCase("for ZEInt16")
		{
			ZEInt16 Value = 30;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEInt32")
		{
			ZEInt32 Value = 0;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEInt64")
		{
			ZEInt64 Value = 40;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEUInt8")
		{
			ZEUInt8 Value = 200;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEUInt16")
		{
			ZEUInt16 Value = 640;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEUInt32")
		{
			ZEUInt32 Value = 43000;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEUInt64")
		{
			ZEUInt64 Value = 2000000000;

			Variant = Value;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for float")
		{
			float Float = 9.0f;

			Variant = Float;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for double")
		{
			double Double = 3.80;

			Variant = Double;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEMatrix3x3")
		{
			ZEInt64 LastItem;

			ZEMatrix3x3 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Variant = Matrix;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX3X3);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEMatrix4x4")
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
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEVector2")
		{
			ZEInt64 LastItem;

			ZEVector2 Vector(1.0f, 2.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR2);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEVector3")
		{
			ZEInt64 LastItem;

			ZEVector3 Vector(1.0f, 2.0f, 3.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR3);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEVector4")
		{
			ZEInt64 LastItem;

			ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for Boolean")
		{
			bool Boolean = true;
			Variant = Boolean;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEString")
		{
			ZEString String = "String";
			Variant = String;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for ZEQuaternion")
		{
			ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
			Variant = Quaternion;
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
			ZETestCheck(Result);
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for undefined")
		{
			Variant.SetType(ZE_VRT_UNDEFINED);
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNDEFINED);
			ZETestCheck(Result);
			ZEInt64 Last = File.Tell();
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		ZETestCase("for null")
		{
			Variant.SetType(ZE_VRT_NULL);
			ZEInt64 LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_NULL);
			ZETestCheck(Result);
			ZEInt64 Last = File.Tell();
			File.Seek(LastItem, ZE_SF_BEGINING);
		}

		File.Close();
 		remove("ZEVariantTests.txt");
	}

	ZETest("void ZEVariant::SetBoolean(bool Value)")
	{
		ZEVariant Variant;
		bool Value = false;

		Variant.SetBoolean(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZETestCheck(!Variant.GetBoolean());
	}

	ZETest("void ZEVariant::SetFloat(float Value)")
	{
		ZEVariant Variant;
		float Value = 0.8f;

		Variant.SetFloat(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);
		ZETestCheckEqual(Variant.GetFloat(), Value);
	}

	ZETest("void ZEVariant::SetDouble(double Value)")
	{
		ZEVariant Variant;
		double Value = 5.8;

		Variant.SetDouble(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
		ZETestCheckClose(Variant.GetDouble(), Value);
	}

	ZETest("void ZEVariant::SetInt8(ZEInt8 Value)")
	{
		ZEVariant Variant;
		ZEInt8 Value = 16;

		Variant.SetInt8(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZETestCheckEqual(Variant.GetInt8(), Value);
	}

	ZETest("void ZEVariant::SetInt16(ZEInt16 Value)")
	{
		ZEVariant Variant;
		ZEInt16 Value = 30;

		Variant.SetInt16(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZETestCheckEqual(Variant.GetInt16(), Value);
	}

	ZETest("void ZEVariant::SetInt32(ZEInt32 Value)")
	{
		ZEVariant Variant;
		ZEInt32 Value = 0;

		Variant.SetInt32(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZETestCheckEqual(Variant.GetInt32(), Value);
	}

	ZETest("void ZEVariant::SetInt64(ZEInt64 Value)")
	{
		ZEVariant Variant;
		ZEInt64 Value = 40;

		Variant.SetInt64(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZETestCheckEqual(Variant.GetInt64(), Value);
	}

	ZETest("void ZEVariant::SetUInt8(ZEUInt8 Value)")
	{
		ZEVariant Variant;
		ZEUInt8 Value = 200;

		Variant.SetUInt8(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZETestCheckEqual(Variant.GetUInt8(), Value);
	}

	ZETest("void ZEVariant::SetUInt16(ZEUInt16 Value)")
	{
		ZEVariant Variant;
		ZEUInt16 Value = 640;

		Variant.SetUInt16(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZETestCheckEqual(Variant.GetUInt16(), Value);
	}

	ZETest("void ZEVariant::SetUInt32(ZEUInt32 Value)")
	{
		ZEVariant Variant;
		ZEUInt32 Value = 43000;

		Variant.SetUInt32(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZETestCheckEqual(Variant.GetUInt32(), Value);
	}

	ZETest("void ZEVariant::SetUInt64(ZEUInt64 Value)")
	{
		ZEVariant Variant;
		ZEUInt64 Value = 2000000000;

		Variant.SetUInt64(Value);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZETestCheckEqual(Variant.GetUInt64(), Value);
	}

	ZETest("void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZETestCheckEqual(Variant.GetMatrix3x3(), Matrix);
	}

	ZETest("void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
			5.0f, 8.0f, 6.0f, 9.0f,   
			13.0f, 3.0f, 4.0f, 5.0f,   
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZETestCheckEqual(Variant.GetMatrix4x4(), Matrix);
	}

	ZETest("void ZEVariant::SetNull()")
	{
		ZEVariant Variant;

		Variant.SetNull();
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_NULL);
	}

	ZETest("void ZEVariant::SetClass(ZEObject* Class)")
	{
// 		ZEVariant Variant;
// 		ZEObjectDescription* Desc;
// 		ZEObject* Class = Desc->CreateInstance();

// 
// 		Variant.SetClass(Class);
// 		ZETestCheckEqual(Variant.GetType(), ZE_VRT_CLASS);
	}

	ZETest("void ZEVariant::SetQuaternion(const ZEQuaternion& Quaternion)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		Variant.SetQuaternion(Quaternion);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
		ZETestCheckEqual(Variant.GetQuaternion(), Quaternion);
	}

	ZETest("void ZEVariant::SetString(const char *NewValue)")
	{
		ZEVariant Variant;
		const char* NewValue = "String";

		Variant.SetString(NewValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
		ZETestCheckString(Variant.GetString(), "String");

		ZETestCase("set string to "" ")
		{
			NewValue = "";

			Variant.SetString(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), "");
		}

		ZETestCase("call SetString with different parameters respectively")
		{
			Variant.SetString("String");
			Variant.SetString("Test");

			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), "Test");
		}

		ZETestCase("first set variant to boolean then set same variant to string")
		{
			ZEVariant TestVariant(false);
			ZETestCheckEqual(TestVariant.GetType(), ZE_VRT_BOOLEAN);
			ZETestCheck(!TestVariant.GetBoolean());
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZETestCheck(!Result.Boolean);

			TestVariant = "Test";
			ZETestCheckEqual(TestVariant.GetType(),ZE_VRT_STRING);
			ZETestCheckString(TestVariant.GetString(), "Test");
			Result = TestVariant.GetValue();
			ZETestCheckString(Result.String, "Test");
		}

		ZETestCase("first create a variant as undefined then set string to same variant")
		{
			ZEVariant TestVariant;
			ZETestCheckEqual(TestVariant.GetType(), ZE_VRT_UNDEFINED);
			ZEString String = TestVariant.GetString(); //error Variant type mismatched

			const char* Value = "TestString";
			TestVariant.SetString(Value);

			ZETestCheckEqual(TestVariant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(TestVariant.GetString(), Value);
			ZETestCheckString(TestVariant.GetString(), "TestString");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZETestCheckString(Result.String, Value);
			ZETestCheckString(Result.String, "TestString");
		}

		ZETestCase("first create a variant as undefined then set ZEString to same variant")
		{
			ZEVariant TestVariant;
			ZETestCheckEqual(TestVariant.GetType(), ZE_VRT_UNDEFINED);

			ZEString Value = "Test";
			TestVariant.SetString(Value);

			ZETestCheckEqual(TestVariant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(TestVariant.GetString(), Value);
			ZETestCheckString(TestVariant.GetString(), "Test");
			ZEVariant::ZEVariantValue Result = TestVariant.GetValue();
			ZETestCheckString(Result.String, Value);
			ZETestCheckString(Result.String, "Test");
		}

		ZETestCase("set variant to different variant types respectively")
		{
			ZEVariant NewVariant;
			NewVariant.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewVariant);
			ZETestCheckEqual(Variant.GetType(), NewVariant.GetType());
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_NULL);

			Variant.SetString("TestValue");
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), "TestValue");
			ZEVariant::ZEVariantValue Result = Variant.GetValue();
			ZETestCheckString(Result.String, "TestValue");

			Variant.SetDouble(1.20);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
			ZETestCheckClose(Variant.GetDouble(), 1.20);
			ZEString String = Variant.GetString(); //(Bad Ptr) error Variant type mismatched
			Result = Variant.GetValue();
			ZETestCheckClose(Result.Double, 1.20);
			String = Result.String; //(Bad Ptr)

			NewVariant.SetType(ZE_VRT_STRING);
			NewVariant = "Test";
			Variant.SetVariant(NewVariant);
			ZETestCheckEqual(Variant.GetType(), NewVariant.GetType());
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), NewVariant.GetString());
			ZETestCheckString(Variant.GetString(), "Test");
			ZETestCheckString(NewVariant.GetString(), "Test");

			Variant = "String";	
			ZETestCheckString(Variant.GetString(), "String");
			ZETestCheckString(NewVariant.GetString(), "Test");

			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			ZETestCheckEqual(Variant.GetUInt32(), 0);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			const char* NewString = "NewTestString";
			Variant = NewString;
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), NewString);
			ZETestCheckString(Variant.GetString(), "NewTestString");

			NewVariant.SetType(ZE_VRT_INTEGER_8);
			ZETestCheckEqual(NewVariant.GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(NewVariant.GetInt8(), 0);
			Variant.SetVariant(NewVariant);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Variant.GetInt8(), 0);
			Result = Variant.GetValue();
			ZETestCheckEqual(Result.Int32, 0);
		}

		ZETestCase("for dynamicly created const char *")
		{
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);

			const char* String = new char;

			Variant.SetString(String);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), String);
		}

		ZETestCase("assign ZEString to const char* ")
		{
			ZEString String = "New";
			NewValue = String;

			Variant.SetType(ZE_VRT_NULL);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_NULL);

			Variant.SetString(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), NewValue);
			ZETestCheckString(Variant.GetString(), "New");
		}

		ZETestCase("set string to NULL")
		{
// 			NewValue = NULL;
// 
// 			Variant.SetString(NewValue);
// 			char* Result = Variant.GetString();
		}
	}

	ZETest("void ZEVariant::SetType(ZEVariantType NewType)")
	{
		ZEVariant Variant;

		Variant.SetType(ZE_VRT_FLOAT);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);

		Variant.SetType(ZE_VRT_MATRIX4X4);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
	}

	ZETest("void ZEVariant::SetVariant(const ZEVariant& NewValue)")
	{
		ZEVariant NewValue;
		ZEVariant Variant;

		ZETestCase("ZE_VRT_UNDEFINED")
		{
			NewValue.SetType(ZE_VRT_UNDEFINED);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNDEFINED);
		}

		ZETestCase("ZE_VRT_NULL")
		{
			NewValue.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_NULL);
		}

		ZETestCase("ZE_VRT_FLOAT")
		{
			NewValue.SetFloat(3.2f);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);
			ZETestCheckEqual(Variant.GetFloat(), NewValue.GetFloat());
		}

		ZETestCase("ZE_VRT_DOUBLE")
		{
			NewValue.SetDouble(2.8);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
			ZETestCheckClose(Variant.GetDouble(), NewValue.GetDouble());
		}

		ZETestCase("ZE_VRT_INTEGER_8")
		{
			NewValue.SetInt8(16);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
			ZETestCheckEqual(Variant.GetInt8(), NewValue.GetInt8());
		}

		ZETestCase("ZE_VRT_INTEGER_16")
		{
			NewValue.SetInt16(30);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
			ZETestCheckEqual(Variant.GetInt16(), NewValue.GetInt16());
		}

		ZETestCase("ZE_VRT_INTEGER_32")
		{
			NewValue.SetInt32(0);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
			ZETestCheckEqual(Variant.GetInt32(), NewValue.GetInt32());
		}

		ZETestCase("ZE_VRT_INTEGER_64")
		{
			NewValue.SetInt64(40);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZETestCheckEqual(Variant.GetInt64(), NewValue.GetInt64());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			NewValue.SetUInt8(200);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
			ZETestCheckEqual(Variant.GetUInt8(), NewValue.GetUInt8());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			NewValue.SetUInt16(640);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
			ZETestCheckEqual(Variant.GetUInt16(), NewValue.GetUInt16());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			NewValue.SetUInt32(43000);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
			ZETestCheckEqual(Variant.GetUInt32(), NewValue.GetUInt32());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			NewValue.SetUInt64(2000000000);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
			ZETestCheckEqual(Variant.GetUInt64(), NewValue.GetUInt64());
		}

		ZETestCase("ZE_VRT_BOOLEAN")
		{
			NewValue.SetBoolean(false);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);
			ZETestCheckEqual(Variant.GetBoolean(), NewValue.GetBoolean());
		}

		ZETestCase("ZE_VRT_STRING")
		{
			NewValue.SetString("String");

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), NewValue.GetString());
		}

		ZETestCase("ZE_VRT_QUATERNION")
		{
			NewValue.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
			ZETestCheckEqual(Variant.GetQuaternion(), NewValue.GetQuaternion());
		}

		ZETestCase("ZE_VRT_VECTOR2")
		{
			NewValue.SetVector2(ZEVector2(3.0f, 5.0f));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR2);
			ZETestCheckEqual(Variant.GetVector2(), NewValue.GetVector2());
		}

		ZETestCase("ZE_VRT_VECTOR3")
		{
			NewValue.SetVector3(ZEVector3(3.0f, 5.0f, 8.0f));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR3);
			ZETestCheckEqual(Variant.GetVector3(), NewValue.GetVector3());
		}

		ZETestCase("ZE_VRT_VECTOR4")
		{
			NewValue.SetVector4(ZEVector4(3.0f, 5.0f, 8.0f, 11.0f));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);
			ZETestCheckEqual(Variant.GetVector4(), NewValue.GetVector4());
		}

		ZETestCase("ZE_VRT_MARTIX3X3")
		{
			NewValue.SetMatrix3x3(ZEMatrix3x3(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX3X3);
			ZETestCheckEqual(Variant.GetMatrix3x3(), NewValue.GetMatrix3x3());
		}

		ZETestCase("ZE_VRT_MARTIX4X4")
		{
			NewValue.SetMatrix4x4(ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f,   
				5.0f, 8.0f, 6.0f, 9.0f,   
				13.0f, 3.0f, 4.0f, 5.0f,   
				12.0f, 2.0f, 8.0f, 6.0f));

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
			ZETestCheckEqual(Variant.GetMatrix4x4(), NewValue.GetMatrix4x4());
		}

		ZETestCase("ZE_VRT_CLASS")
		{

		}

		ZETestCase("set type without set value")
		{
			NewValue.SetType(ZE_VRT_QUATERNION);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
			ZETestCheckEqual(Variant.GetQuaternion(), ZEQuaternion(0.0f, 5.0f, 8.0f, 11.0f));

			NewValue.SetType(ZE_VRT_INTEGER_64);

			Variant.SetVariant(NewValue);
			ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
			ZEInt64 IntValue = Variant.GetInt64();
			ZETestCheckEqual(IntValue, 4656722014701092864);
		}

		ZETestCase("set type without set value for new variants")
		{
			ZEVariant Variant1;
			ZEVariant NewValue1;

			NewValue1.SetType(ZE_VRT_QUATERNION);
			Variant1.SetVariant(NewValue1);
			ZEQuaternion Quaternion = Variant1.GetQuaternion();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_QUATERNION);

			NewValue1.SetType(ZE_VRT_BOOLEAN);
			Variant1.SetVariant(NewValue1);
			bool Boolean = Variant1.GetBoolean();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_BOOLEAN);

			NewValue1.SetType(ZE_VRT_DOUBLE);
			Variant1.SetVariant(NewValue1);
			double Double = Variant1.GetDouble();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_DOUBLE);

			NewValue1.SetType(ZE_VRT_FLOAT);
			Variant1.SetVariant(NewValue1);
			float Float = Variant1.GetFloat();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_FLOAT);

			NewValue1.SetType(ZE_VRT_INTEGER_16);
			Variant1.SetVariant(NewValue1);
			ZEInt16 I16 = Variant1.GetInt16();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_INTEGER_16);

			NewValue1.SetType(ZE_VRT_INTEGER_32);
			Variant1.SetVariant(NewValue1);
			ZEInt32 I32 = Variant1.GetInt32();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_INTEGER_32);

			NewValue1.SetType(ZE_VRT_INTEGER_64);
			Variant1.SetVariant(NewValue1);
			ZEInt64 I64 = Variant1.GetInt64();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_INTEGER_64);

			NewValue1.SetType(ZE_VRT_INTEGER_8);
			Variant1.SetVariant(NewValue1);
			ZEInt8 I8 = Variant1.GetInt8();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_INTEGER_8);

			NewValue1.SetType(ZE_VRT_MATRIX3X3);
			Variant1.SetVariant(NewValue1);
			ZEMatrix3x3 Matrix3 = Variant1.GetMatrix3x3();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_MATRIX3X3);

			NewValue1.SetType(ZE_VRT_MATRIX4X4);
			Variant1.SetVariant(NewValue1);
			ZEMatrix4x4 Matrix4 = Variant1.GetMatrix4x4();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_MATRIX4X4);

// 			NewValue1.SetType(ZE_VRT_STRING);
// 			Variant1.SetVariant(NewValue1); //access violation reading location 0x00000000
// 			ZEString String = Variant1.GetString();
// 			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
// 			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_STRING);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_16);
			Variant1.SetVariant(NewValue1);
			ZEUInt16 UI16 = Variant1.GetUInt16();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			Variant1.SetVariant(NewValue1);
			ZEUInt32 UI32 = Variant1.GetUInt32();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_64);
			Variant1.SetVariant(NewValue1);
			ZEUInt64 UI64 = Variant1.GetUInt64();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);

			NewValue1.SetType(ZE_VRT_UNSIGNED_INTEGER_8);
			Variant1.SetVariant(NewValue1);
			ZEUInt8 UI8 = Variant1.GetUInt8();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);

			NewValue1.SetType(ZE_VRT_VECTOR2);
			Variant1.SetVariant(NewValue1);
			ZEVector2 Vector2 = Variant1.GetVector2();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_VECTOR2);

			NewValue1.SetType(ZE_VRT_VECTOR3);
			Variant1.SetVariant(NewValue1);
			ZEVector3 Vector3 = Variant1.GetVector3();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_VECTOR3);

			NewValue1.SetType(ZE_VRT_VECTOR4);
			Variant1.SetVariant(NewValue1);
			ZEVector4 Vector4 = Variant1.GetVector4();
			ZETestCheckEqual(Variant1.GetType(), NewValue1.GetType());
			ZETestCheckEqual(Variant1.GetType(), ZE_VRT_VECTOR4);
		}
	}

	ZETest("void ZEVariant::SetVector2(const ZEVector2& Vector)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant.SetVector2(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR2);
		ZETestCheckEqual(Variant.GetVector2(), Vector);
	}

	ZETest("void ZEVariant::SetVector3(const ZEVector3& Vector)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant.SetVector3(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR3);
		ZETestCheckEqual(Variant.GetVector3(), Vector);
	}

	ZETest("void ZEVariant::SetVector4(const ZEVector4& Vector)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant.SetVector4(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);
		ZETestCheckEqual(Variant.GetVector4(), Vector);
	}

	ZETest("ZESize ZEVariant::SizeOf() const")
	{
		ZEVariant Variant;
		ZEInt Size;

		ZETestCase("ZE_VRT_UNDEFINED")
		{
			Variant.SetType(ZE_VRT_UNDEFINED);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 0);
		}

		ZETestCase("ZE_VRT_NULL")
		{
			Variant.SetType(ZE_VRT_NULL);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 0);
		}

		ZETestCase("ZE_VRT_FLOAT")
		{
			Variant.SetType(ZE_VRT_FLOAT);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}

		ZETestCase("ZE_VRT_DOUBLE")
		{
			Variant.SetType(ZE_VRT_DOUBLE);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 8);
		}

		ZETestCase("ZE_VRT_INTEGER_8")
		{
			Variant.SetType(ZE_VRT_INTEGER_8);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 1);
		}

		ZETestCase("ZE_VRT_INTEGER_16")
		{
			Variant.SetType(ZE_VRT_INTEGER_16);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 2);
		}

		ZETestCase("ZE_VRT_INTEGER_32")
		{
			Variant.SetType(ZE_VRT_INTEGER_32);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}

		ZETestCase("ZE_VRT_INTEGER_64")
		{
			Variant.SetType(ZE_VRT_INTEGER_64);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 8);
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_8")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_8);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 1);
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_16")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_16);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 2);
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_32")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_32);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_64")
		{
			Variant.SetType(ZE_VRT_UNSIGNED_INTEGER_64);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 8);
		}

		ZETestCase("ZE_VRT_BOOLEAN")
		{
			Variant.SetType(ZE_VRT_BOOLEAN);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 1);
		}

		ZETestCase("ZE_VRT_STRING")
		{
			Variant = "String";

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 7);

			Variant = "TestString";

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 11);
		}

		ZETestCase("ZE_VRT_QUATERNION")
		{
			Variant.SetType(ZE_VRT_QUATERNION);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 16);
		}

		ZETestCase("ZE_VRT_VECTOR2")
		{
			Variant.SetType(ZE_VRT_VECTOR2);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 8);
		}

		ZETestCase("ZE_VRT_VECTOR3")
		{
			Variant.SetType(ZE_VRT_VECTOR3);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 12);
		}

		ZETestCase("ZE_VRT_VECTOR4")
		{
			Variant.SetType(ZE_VRT_VECTOR4);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 16);
		}

		ZETestCase("ZE_VRT_MATRIX3X3")
		{
			Variant.SetType(ZE_VRT_MATRIX3X3);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 36);
		}

		ZETestCase("ZE_VRT_MATRIX4X4")
		{
			Variant.SetType(ZE_VRT_MATRIX4X4);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 64);
		}

		ZETestCase("ZE_VRT_CLASS")
		{
			Variant.SetType(ZE_VRT_CLASS);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}
	}

	ZETest("bool ZEVariant::Unserialize(ZEUnserializer* Unserializer)")
	{
		ZEFile File;
		bool Opened = File.Open("VariantTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		bool Serialized;
		bool Unserialized;
		ZEInt64 LastItem;
		ZEInt Seek;

		ZETestCase("ZE_VRT_FLOAT")
		{
			ZEVariant Variant1(3.8f);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheck(Unserialized);
  			ZETestCheckEqual(Variant2.GetFloat(), Variant1.GetFloat());
		}

		ZETestCase("ZE_VRT_DOUBLE")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1(1.2);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheck(Unserialized);
			ZETestCheckClose(Variant2.GetDouble(), Variant1.GetDouble());
		}

		ZETestCase("ZE_VRT_INTEGER_8")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetInt8(), Variant1.GetInt8());
		}

		ZETestCase("ZE_VRT_INTEGER_16")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetInt16(), Variant1.GetInt16());
		}

		ZETestCase("ZE_VRT_INTEGER_32")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetInt32(), Variant1.GetInt32());
		}

		ZETestCase("ZE_VRT_INTEGER_64")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetInt64(), Variant1.GetInt64());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_8")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetUInt8(), Variant1.GetUInt8());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_16")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetUInt16(), Variant1.GetUInt16());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_32")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetUInt32(), Variant1.GetUInt32());
		}

		ZETestCase("ZE_VRT_UNSIGNED_INTEGER_64")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetUInt64(), Variant1.GetUInt64());
		}

		ZETestCase("ZE_VRT_BOOLEAN")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1(false);

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetBoolean(), Variant1.GetBoolean());
		}

		ZETestCase("ZE_VRT_STRING")
		{
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant1("Test");

			Serialized = Variant1.Serialize((ZESerializer*)&File);
			LastItem = File.Tell();
			Seek = File.Seek(-LastItem, ZE_SF_CURRENT);

			ZEVariant Variant2;
			Unserialized = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheck(Unserialized);
			ZETestCheckString(Variant2.GetString(), Variant1.GetString());
		}

		ZETestCase("ZE_VRT_QUATERNION")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetQuaternion(), Variant1.GetQuaternion());
		}

		ZETestCase("ZE_VRT_VECTOR2")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetVector2(), Variant1.GetVector2());
		}

		ZETestCase("ZE_VRT_VECTOR3")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetVector3(), Variant1.GetVector3());
		}

		ZETestCase("ZE_VRT_VECTOR4")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetVector4(), Variant1.GetVector4());
		}

		ZETestCase("ZE_VRT_MATRIX3X3")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetMatrix3x3(), Variant1.GetMatrix3x3());
		}

		ZETestCase("ZE_VRT_MATRIX4X4")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetMatrix4x4(), Variant1.GetMatrix4x4());
		}

		ZETestCase("ZE_VRT_CLASS")
		{

		}

		ZETestCase("ZE_VRT_NULL")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetType(), Variant1.GetType());
		}

		ZETestCase("ZE_VRT_UNDEFINED")
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
			ZETestCheck(Unserialized);
			ZETestCheckEqual(Variant2.GetType(), Variant1.GetType());
		}

		File.Close();
		remove("VariantTests.txt");
	}

	ZETest("ZEVariant::ZEVariant()")
	{
		ZEVariant Variant;
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNDEFINED);
	}

	ZETest("ZEVariant::ZEVariant(bool InitialValue)")
	{
		bool InitialValue = true;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_BOOLEAN);
		ZETestCheck(Variant.GetBoolean());
	}

	ZETest("ZEVariant::ZEVariant(const char* InitialValue)")
	{
		const char* InitialValue = "String";

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_STRING);
		ZETestCheckString(Variant.GetString(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEVariant Variant(Matrix);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX3X3);
		ZETestCheckEqual(Variant.GetMatrix3x3(), Matrix);
	}

	ZETest("ZEVariant::ZEVariant(const ZEMatrix4x4& Matrix)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		ZEVariant Variant(Matrix);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_MATRIX4X4);
		ZETestCheckEqual(Variant.GetMatrix4x4(), Matrix);
	}

	ZETest("ZEVariant::ZEVariant(const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		ZEVariant Variant(Quaternion);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_QUATERNION);
		ZETestCheckEqual(Variant.GetQuaternion(), Quaternion);
	}

	ZETest("ZEVariant::ZEVariant(const ZEVariant& InitialValue)")
	{
		ZEVariant InitialValue;
		InitialValue.SetInt16(255);

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
 		ZETestCheckEqual(Variant.GetInt16(), InitialValue.GetInt16());
	}

	ZETest("ZEVariant::ZEVariant(const ZEVector2& Vector)")
	{
		ZEVector2 Vector(1.0f, 2.0f);

		ZEVariant Variant(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR2);
		ZETestCheckEqual(Variant.GetVector2(), Vector);
	}

	ZETest("ZEVariant::ZEVariant(const ZEVector3& Vector)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEVariant Variant(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR3);
		ZETestCheckEqual(Variant.GetVector3(), Vector);
	}

	ZETest("ZEVariant::ZEVariant(const ZEVector4& Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEVariant Variant(Vector);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_VECTOR4);
		ZETestCheckEqual(Variant.GetVector4(), Vector);
	}

	ZETest("ZEVariant::ZEVariant(float InitialValue)")
	{
		float InitialValue = 255.349f;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_FLOAT);
		ZETestCheckEqual(Variant.GetFloat(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(double InitialValue)")
	{
		double InitialValue = 12.56;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_DOUBLE);
		ZETestCheckClose(Variant.GetDouble(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEInt8 InitialValue)")
	{
		ZEInt8 InitialValue = 16;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_8);
		ZETestCheckEqual(Variant.GetInt8(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEInt16 InitialValue)")
	{
		ZEInt16 InitialValue = 30;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_16);
		ZETestCheckEqual(Variant.GetInt16(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEInt32 InitialValue)")
	{
		ZEInt32 InitialValue = 0;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_32);
		ZETestCheckEqual(Variant.GetInt32(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEInt64 InitialValue)")
	{
		ZEInt64 InitialValue = 40;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_INTEGER_64);
		ZETestCheckEqual(Variant.GetInt64(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEUInt8 InitialValue)")
	{
		ZEUInt8 InitialValue = 200;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_8);
		ZETestCheckEqual(Variant.GetUInt8(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEUInt16 InitialValue)")
	{
		ZEUInt16 InitialValue = 640;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_16);
		ZETestCheckEqual(Variant.GetUInt16(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEUInt32 InitialValue)")
	{
		ZEUInt32 InitialValue = 43000;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_32);
		ZETestCheckEqual(Variant.GetUInt32(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEUInt64 InitialValue)")
	{
		ZEUInt64 InitialValue = 2000000000;

		ZEVariant Variant(InitialValue);
		ZETestCheckEqual(Variant.GetType(), ZE_VRT_UNSIGNED_INTEGER_64);
		ZETestCheckEqual(Variant.GetUInt64(), InitialValue);
	}

	ZETest("ZEVariant::ZEVariant(ZEObject* Class)")
	{
		ZEObject* Class;

		ZEVariant Variant(&Class);
		//ZE_VRT_BOOLEAN
		//ZETestCheckEqual(Variant.GetType(), ZE_VRT_CLASS);
	}
}
