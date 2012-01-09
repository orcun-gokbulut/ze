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
#include "ZETest.h"
#include "ZEString.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZEVector.h"
#include "ZETypedVariant.h"
#include "ZEFile\ZEFile.h"

ZETestSuite(ZEVariant)
{
	ZETest("ZEVariant::~ZEVariant()")
	{

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
	ZETest("int ZEVariant::GetInteger() const")
	{
		ZEVariant Variant;
		int Value = 6;
		Variant.SetInteger(Value);

		int Result = Variant.GetInteger();
		ZETestCheckEqual(Result, 6);
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
	ZETest("void* ZEVariant::GetPointer() const")
	{
		ZETestCheck(false);
	}
	ZETest("ZEQuaternion& ZEVariant::GetQuaternion() const")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_4, ZEVector3::UnitY);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = Variant.GetQuaternion();
		ZETestCheckEqual(Result, ZEQuaternion(0.92387950f, 0.0f, 0.38268346f, 0.0f));
	}
	ZETest("char* ZEVariant::GetString() const")
	{
		ZEVariant Variant;
		ZEString String = "String";
		Variant.SetString(String);

		char* Result;
		Result = Variant.GetString();
		ZETestCheck(String == Result);
	}
	ZETest("ZEVariantType ZEVariant::GetType() const")
	{
		ZEVariant Variant;
		int i = 6;
		Variant.SetInteger(i);

		ZEVariantType Result = Variant.GetType();
		ZETestCheck(Result == ZE_VRT_INTEGER);
	}
	ZETest("ZEVariant::ZEVariantValue ZEVariant::GetValue()	const")
	{
		ZEVariant Variant;
		int i = 6;
		Variant.SetInteger(i);

		ZEVariant::ZEVariantValue Result = Variant.GetValue();
		ZETestCheckEqual(Result.Integer, 6);
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
	ZETest("ZEVariant::ZEVariantValue& ZEVariant::ImplicitAcesss()")
	{
		ZEVariant Variant;
		int i = 6;
		Variant.SetInteger(i);

		ZEVariant::ZEVariantValue Result = Variant.ImplicitAcesss();
		ZETestCheckEqual(Result.Integer, 6);
	}
	ZETest("bool ZEVariant::IsNull() const")
	{
		ZEVariant Variant;
		Variant.SetNull();

		bool Result = Variant.IsNull();
		ZETestCheck(Result == true);

		ZETestCase("for type not null")
		{
			Variant.SetInteger(5);

			Result = Variant.IsNull();
			ZETestCheck(Result == false);
		}	
	}
	ZETest("void ZEVariant::operator=(bool NewValue)")
	{
		ZEVariant Variant;
		bool NewValue = false;

		Variant = NewValue;
		ZETestCheck(Variant.GetType() == ZE_VRT_BOOLEAN);
		ZETestCheck(Variant.GetBoolean() == false);
	}
	ZETest("void ZEVariant::operator=(const char* NewValue)")
	{
		ZEVariant Variant;
		const char* NewValue = "String";

		Variant = NewValue;
		ZETestCheck(Variant.GetType() == ZE_VRT_STRING);

	}
	ZETest("void ZEVariant::operator=(const ZEMatrix3x3& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix( 1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant = Matrix;
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX3X3);
		ZETestCheck(Variant.GetMatrix3x3() == Matrix);

	}
	ZETest("void ZEVariant::operator=(const ZEMatrix4x4& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant = Matrix;
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX4X4);
		ZETestCheck(Variant.GetMatrix4x4() == Matrix);
	}
	ZETest("void ZEVariant::operator=(const ZEQuaternion& Quaternion)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

		Variant = Quaternion;
		ZETestCheck(Variant.GetType() == ZE_VRT_QUATERNION);
		ZETestCheck(Variant.GetQuaternion() == Quaternion);
	}
	ZETest("void ZEVariant::operator=(const ZEVariant& NewValue)")
	{
		ZEVariant Variant;
		ZEVariant NewValue;
		NewValue.SetInteger(5);

		Variant = NewValue;
		ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
		ZETestCheck(Variant.GetInteger() == 5);
	}
	ZETest("void ZEVariant::operator=(const ZEVector2& Vector)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant = Vector;
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR2);
		ZETestCheck(Variant.GetVector2() == Vector);
	}
	ZETest("void ZEVariant::operator=(const ZEVector3& Vector)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant = Vector;
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR3);
		ZETestCheck(Variant.GetVector3() == Vector);
	}
	ZETest("void ZEVariant::operator=(const ZEVector4& Vector)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant = Vector;
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR4);
		ZETestCheck(Variant.GetVector4() == Vector);
	}
	ZETest("void ZEVariant::operator=(float NewValue)")
	{
		ZEVariant Variant;
		float NewValue = 3.8f;

		Variant = NewValue;
		ZETestCheck(Variant.GetType() == ZE_VRT_FLOAT);
		ZETestCheck(Variant.GetFloat() == NewValue);
	}
	ZETest("void ZEVariant::operator=(int NewValue)")
	{
		ZEVariant Variant;
		int NewValue = 7;

		Variant = NewValue;
		ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
		ZETestCheck(Variant.GetInteger() == NewValue);
	}
	ZETest("ZEVariant::operator bool()")
	{
		ZEVariant Variant;
		Variant.SetBoolean(true);

		bool Result = (bool)Variant;
		ZETestCheck(Result == true);
	}
	ZETest("ZEVariant::operator const char*()")
	{
		ZEVariant Variant;
		Variant.SetString("String");

		const char* Result = (const char*)Variant;
		ZETestCheck(Result == Variant.GetString());
	}
	ZETest("ZEVariant::operator float()")
	{
		ZEVariant Variant;
		Variant.SetFloat(5.55f);

		float Result =(float)Variant;
		ZETestCheck(Result == Variant.GetFloat());

		ZETestCase("for integer")
		{
			Variant.SetInteger(5);

			Result = (float)Variant;
			ZETestCheckClose(Result, 5.0f);
		}
	}
	ZETest("ZEVariant::operator int()")
	{
		ZEVariant Variant;
		Variant.SetInteger(5);

		int Result = (int)Variant;
		ZETestCheck(Result == Variant.GetInteger());

		ZETestCase("for float")
		{
			Variant.SetFloat(5.5f);

			Result = (int)Variant;
			ZETestCheckEqual(Result, 5);
		}
	}
	ZETest("ZEVariant::operator void*()")
	{
		// ZEVariant has been changed pointer type became ZEClass type.
		ZETestCheck(false);
	}
	ZETest("ZEVariant::operator ZEMatrix3x3()")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);

		ZEMatrix3x3 Result = (ZEMatrix3x3)Variant;
		ZETestCheck(Result == Variant.GetMatrix3x3());
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
		ZETestCheck(Result == Variant.GetMatrix4x4());
	}
	ZETest("ZEVariant::operator ZEQuaternion()")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);
		Variant.SetQuaternion(Quaternion);

		ZEQuaternion Result = (ZEQuaternion)Variant;
		ZETestCheck(Result == Variant.GetQuaternion());
	}
	ZETest("ZEVariant::operator ZEVector2()")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);
		Variant.SetVector2(Vector);

		ZEVector2 Result = (ZEVector2)Variant;
		ZETestCheck(Result == Variant.GetVector2());
	}
	ZETest("ZEVariant::operator ZEVector3()")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		Variant.SetVector3(Vector);

		ZEVector3 Result = (ZEVector3)Variant;
		ZETestCheck(Result == Variant.GetVector3());
	}
	ZETest("ZEVariant::operator ZEVector4()")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		Variant.SetVector4(Vector);

		ZEVector4 Result = (ZEVector4)Variant;
		ZETestCheck(Result == Variant.GetVector4());
	}
	ZETest("bool ZEVariant::Serialize(ZESerializer* Serializer)")
	{
		ZEFile File;

		File.Open("ZEVariantTests.txt", ZE_FM_READ_WRITE, true);

		ZEVariant Variant;

		ZETestCase("for Integer")
		{
			Variant.SetInteger(255);

			bool Result = Variant.Serialize((ZESerializer*)&File);
			ZETestCheck(Result == true);
		}

		ZETestCase("for float")
		{
			float Float = 9.0f;

			Variant = Float;
			size_t LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}

		ZETestCase("for Matrix3x3")
		{
			size_t LastItem;

			ZEMatrix3x3 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Variant = Matrix;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}
		ZETestCase("for Matrix4x4")
		{
			size_t LastItem;

			ZEMatrix4x4 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Matrix.M14 = 4;
			Variant = Matrix;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}
		ZETestCase("for Vector2")
		{
			size_t LastItem;

			ZEVector2 Vector(1.0f, 2.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}
		ZETestCase("for Vector3")
		{
			size_t LastItem;

			ZEVector3 Vector(1.0f, 2.0f, 3.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}
		ZETestCase("for Vector4")
		{
			size_t LastItem;

			ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
			Variant = Vector;
			LastItem = File.Tell();

			bool Result = Variant.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZETestCheck(Result == true);
		}
	}

	ZETest("void ZEVariant::SetBoolean(bool Value)")
	{
		ZEVariant Variant;
		bool Value = false;

		Variant.SetBoolean(Value);
		ZETestCheck(Variant.GetType() == ZE_VRT_BOOLEAN);
		ZETestCheck(Variant.GetBoolean() == false);
	}
	ZETest("void ZEVariant::SetFloat(float Value)")
	{
		ZEVariant Variant;
		float Value = 0.8f;

		Variant.SetFloat(Value);
		ZETestCheck(Variant.GetType() == ZE_VRT_FLOAT);
		ZETestCheck(Variant.GetFloat() == Value);
	}
	ZETest("void ZEVariant::SetInteger(int Value)")
	{
		ZEVariant Variant;
		int Value = 6;

		Variant.SetInteger(Value);
		ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
		ZETestCheck(Variant.GetInteger() == Value);
	}
	ZETest("void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		Variant.SetMatrix3x3(Matrix);
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX3X3);
		ZETestCheck(Variant.GetMatrix3x3() == Matrix);
	}
	ZETest("void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Matrix)")
	{
		ZEVariant Variant;
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,   
			5.0f, 8.0f, 6.0f, 9.0f,   
			13.0f, 3.0f, 4.0f, 5.0f,   
			12.0f, 2.0f, 8.0f, 6.0f);

		Variant.SetMatrix4x4(Matrix);
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX4X4);
		ZETestCheck(Variant.GetMatrix4x4() == Matrix);
	}
	ZETest("void ZEVariant::SetNull()")
	{
		ZEVariant Variant;

		Variant.SetNull();
		ZETestCheck(Variant.GetType() == ZE_VRT_NULL);
	}
	ZETest("void ZEVariant::SetPointer(void* Pointer)")
	{
		// ZEVariant has been changed pointer type became ZEClass type.
		ZETestCheck(false);
	}
	ZETest("void ZEVariant::SetQuaternion(const ZEQuaternion& Quaternion)")
	{
		ZEVariant Variant;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		Variant.SetQuaternion(Quaternion);
		ZETestCheck(Variant.GetType() == ZE_VRT_QUATERNION);
		ZETestCheck(Variant.GetQuaternion() == Quaternion);
	}
	ZETest("void ZEVariant::SetString(const char *NewValue)")
	{
		ZEVariant Variant;
		const char *NewValue = "String";

		Variant.SetString(NewValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_STRING);
		ZETestCheckString(Variant.GetString(), "String");
	}
	ZETest("void ZEVariant::SetType(ZEVariantType NewType)")
	{
		ZEVariant Variant;

		Variant.SetType(ZE_VRT_FLOAT);
		ZETestCheck(Variant.GetType() == ZE_VRT_FLOAT);

		Variant.SetType(ZE_VRT_MATRIX4X4);
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX4X4);
	}
	ZETest("void ZEVariant::SetVariant(const ZEVariant& NewValue)")
	{
		ZEVariant NewValue;
		ZEVariant Variant;

		ZETestCase("ZE_VRT_UNDEFINED")
		{
			NewValue.SetType(ZE_VRT_UNDEFINED);

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_UNDEFINED);
		}
		ZETestCase("ZE_VRT_NULL")
		{
			NewValue.SetType(ZE_VRT_NULL);

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_NULL);
		}
		ZETestCase("ZE_VRT_STRING")
		{
			NewValue.SetString("String");

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_STRING);
			ZETestCheckString(Variant.GetString(), NewValue.GetString());
		}
		ZETestCase("ZE_VRT_INTEGER")
		{
			NewValue.SetInteger(255);

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
			ZETestCheck(Variant.GetInteger() == NewValue.GetInteger());
		}
		ZETestCase("ZE_VRT_BOOLEAN")
		{
			NewValue.SetBoolean(false);

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_BOOLEAN);
			ZETestCheck(Variant.GetBoolean() == NewValue.GetBoolean());
		}
		ZETestCase("ZE_VRT_FLOAT")
		{
			NewValue.SetFloat(3.2f);

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_FLOAT);
			ZETestCheck(Variant.GetFloat() == NewValue.GetFloat());
		}
		ZETestCase("ZE_VRT_VECTOR2")
		{
			NewValue.SetVector2(ZEVector2(3.0f, 5.0f));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR2);
			ZETestCheck(Variant.GetVector2() == NewValue.GetVector2());
		}
		ZETestCase("ZE_VRT_VECTOR3")
		{
			NewValue.SetVector3(ZEVector3(3.0f, 5.0f, 8.0f));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR3);
			ZETestCheck(Variant.GetVector3() == NewValue.GetVector3());
		}
		ZETestCase("ZE_VRT_VECTOR4")
		{
			NewValue.SetVector4(ZEVector4(3.0f, 5.0f, 8.0f, 11.0f));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR4);
			ZETestCheck(Variant.GetVector4() == NewValue.GetVector4());
		}
		ZETestCase("ZE_VRT_QUATERNION")
		{
			NewValue.SetQuaternion(ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_QUATERNION);
			ZETestCheck(Variant.GetQuaternion() == NewValue.GetQuaternion());
		}
		ZETestCase("ZE_VRT_MARTIX3X3")
		{
			NewValue.SetMatrix3x3(ZEMatrix3x3(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX3X3);
			ZETestCheck(Variant.GetMatrix3x3() == NewValue.GetMatrix3x3());
		}
		ZETestCase("ZE_VRT_MARTIX4X4")
		{
			NewValue.SetMatrix4x4(ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f,   
				5.0f, 8.0f, 6.0f, 9.0f,   
				13.0f, 3.0f, 4.0f, 5.0f,   
				12.0f, 2.0f, 8.0f, 6.0f));

			Variant.SetVariant(NewValue);
			ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX4X4);
			ZETestCheck(Variant.GetMatrix4x4() == NewValue.GetMatrix4x4());
		}
	}
	ZETest("void ZEVariant::SetVector2(const ZEVector2& Vector)")
	{
		ZEVariant Variant;
		ZEVector2 Vector(1.0f, 2.0f);

		Variant.SetVector2(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR2);
		ZETestCheck(Variant.GetVector2() == Vector);
	}
	ZETest("void ZEVariant::SetVector3(const ZEVector3& Vector)")
	{
		ZEVariant Variant;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Variant.SetVector3(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR3);
		ZETestCheck(Variant.GetVector3() == Vector);
	}
	ZETest("void ZEVariant::SetVector4(const ZEVector4& Vector)")
	{
		ZEVariant Variant;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		Variant.SetVector4(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR4);
		ZETestCheck(Variant.GetVector4() == Vector);
	}
	ZETest("size_t ZEVariant::SizeOf() const")
	{
		ZEVariant Variant;
		int Size;

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
		ZETestCase("ZE_VRT_STRING")
		{
			Variant = "String";

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 7);
		}
		ZETestCase("ZE_VRT_INTEGER")
		{
			Variant.SetType(ZE_VRT_INTEGER);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}
		ZETestCase("ZE_VRT_BOOLEAN")
		{
			Variant.SetType(ZE_VRT_BOOLEAN);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 1);
		}
		ZETestCase("ZE_VRT_FLOAT")
		{
			Variant.SetType(ZE_VRT_FLOAT);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
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
		ZETestCase("ZE_VRT_QUATERNION")
		{
			Variant.SetType(ZE_VRT_QUATERNION);

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
		ZETestCase("ZE_VRT_POINTER")
		{
			Variant.SetType(ZE_VRT_CLASS);

			Size = Variant.SizeOf();
			ZETestCheckEqual(Size, 4);
		}
	}
	ZETest("bool ZEVariant::Unserialize(ZEUnserializer* Unserializer)")
	{
		ZEFile File;
		File.Open("ZEVariantTests.txt", ZE_FM_READ_WRITE, true);

		ZETestCase("for Integer")
		{
			ZEVariant Variant;
			Variant.SetInteger(255);

			bool Result = Variant.Unserialize((ZEUnserializer*)&File);
			ZETestCheck(Result == true);
		}	
		ZETestCase("for Float")
		{
			ZEVariant Variant1;
			float Float = 9.0f;

			Variant1 = Float;
			size_t LastItem = File.Tell();

			Variant1.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheckEqual(Variant2.GetFloat(), Variant1.GetFloat());
			ZETestCheck(Result == true);
		}
		ZETestCase("for Matrix3x3")
		{
			size_t LastItem;

			ZEVariant Variant1;
			ZEMatrix3x3 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Variant1 = Matrix;
			LastItem = File.Tell();
			Variant1.Serialize(&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize(&File);
			ZETestCheckEqual(Variant1.GetMatrix3x3(), Variant2.GetMatrix3x3());
			ZETestCheck(Result == true);
		}	
		ZETestCase("for Matrix4x4")
		{
			size_t LastItem;

			ZEVariant Variant1;
			ZEMatrix4x4 Matrix;
			Matrix.M11 = 1;
			Matrix.M12 = 2;
			Matrix.M13 = 3;
			Matrix.M14 = 4;
			Variant1 = Matrix;
			LastItem = File.Tell();

			Variant1.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheckEqual(Variant2.GetMatrix4x4(), Variant1.GetMatrix4x4());
			ZETestCheck(Result == true);
		}		
		ZETestCase("for Vector2")
		{
			size_t LastItem;

			ZEVariant Variant1;
			ZEVector2 Vector(1.0f, 2.0f);
			Variant1 = Vector;
			LastItem = File.Tell();

			Variant1.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheckEqual(Variant2.GetVector2(), Variant1.GetVector2());
			ZETestCheck(Result == true);
		}
		ZETestCase("for Vector3")
		{
			size_t LastItem;

			ZEVariant Variant1;
			ZEVector3 Vector(1.0f, 2.0f, 3.0f);
			Variant1 = Vector;
			LastItem = File.Tell();

			Variant1.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheckEqual(Variant2.GetVector3(), Variant1.GetVector3());
			ZETestCheck(Result == true);
		}
		ZETestCase("for Vector4")
		{
			size_t LastItem;

			ZEVariant Variant1;
			ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
			Variant1 = Vector;
			LastItem = File.Tell();

			Variant1.Serialize((ZESerializer*)&File);
			File.Seek(LastItem, ZE_SF_BEGINING);
			ZEVariant Variant2;

			bool Result = Variant2.Unserialize((ZEUnserializer*)&File);
			ZETestCheckEqual(Variant2.GetVector4(), Variant1.GetVector4());
			ZETestCheck(Result == true);
		}
	}
	ZETest("ZEVariant::ZEVariant()")
	{
		ZEVariant Variant;
		ZETestCheck(Variant.GetType() == ZE_VRT_UNDEFINED);
	}
	ZETest("ZEVariant::ZEVariant(bool InitialValue)")
	{
		bool InitialValue = true;

		ZEVariant Variant(InitialValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_BOOLEAN);
		ZETestCheck(Variant.GetBoolean() == true);
	}
	ZETest("ZEVariant::ZEVariant(const char* InitialValue)")
	{
		const char* InitialValue = "String";

		ZEVariant Variant(InitialValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_STRING);

	}
	ZETest("ZEVariant::ZEVariant(const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEVariant Variant(Matrix);
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX3X3);
		ZETestCheck(Variant.GetMatrix3x3() == Matrix);
	}
	ZETest("ZEVariant::ZEVariant(const ZEMatrix4x4& Matrix)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 6.0f, 9.0f,
			13.0f, 3.0f, 4.0f, 5.0f,
			12.0f, 2.0f, 8.0f, 6.0f);

		ZEVariant Variant(Matrix);
		ZETestCheck(Variant.GetType() == ZE_VRT_MATRIX4X4);
		ZETestCheck(Variant.GetMatrix4x4() == Matrix);
	}
	ZETest("ZEVariant::ZEVariant(const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitZ);

		ZEVariant Variant(Quaternion);
		ZETestCheck(Variant.GetType() == ZE_VRT_QUATERNION);
		ZETestCheck(Variant.GetQuaternion() == Quaternion);
	}
	ZETest("ZEVariant::ZEVariant(const ZEVariant& InitialValue)")
	{
		ZEVariant InitialValue;
		InitialValue.SetInteger(255);

		ZEVariant Variant(InitialValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
		ZETestCheck(Variant.GetInteger() == InitialValue.GetInteger());
	}
	ZETest("ZEVariant::ZEVariant(const ZEVector2& Vector)")
	{
		ZEVector2 Vector(1.0f, 2.0f);

		ZEVariant Variant(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR2);
		ZETestCheck(Variant.GetVector2() == Vector);
	}
	ZETest("ZEVariant::ZEVariant(const ZEVector3& Vector)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEVariant Variant(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR3);
		ZETestCheck(Variant.GetVector3() == Vector);
	}
	ZETest("ZEVariant::ZEVariant(const ZEVector4& Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEVariant Variant(Vector);
		ZETestCheck(Variant.GetType() == ZE_VRT_VECTOR4);
		ZETestCheck(Variant.GetVector4() == Vector);
	}
	ZETest("ZEVariant::ZEVariant(float InitialValue)")
	{
		float InitialValue = 255.349f;

		ZEVariant Variant(InitialValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_FLOAT);
		ZETestCheck(Variant.GetFloat() == InitialValue);
	}
	ZETest("ZEVariant::ZEVariant(int InitialValue)")
	{
		int InitialValue = 255;

		ZEVariant Variant(InitialValue);
		ZETestCheck(Variant.GetType() == ZE_VRT_INTEGER);
		ZETestCheck(Variant.GetInteger() == InitialValue);
	}
	ZETest("ZEVariant::ZEVariant(void* Pointer)")
	{
		void* Pointer;

		ZEVariant Variant(&Pointer);
		ZETestCheck(Variant.GetType() == ZE_VRT_CLASS);
	}
}
