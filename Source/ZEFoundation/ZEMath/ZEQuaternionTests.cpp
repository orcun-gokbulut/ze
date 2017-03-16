//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuaternionTests.cpp
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
#include "ZEMath/ZEMath.h"
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMath.h"
#include "ZEOBBox.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

bool ZETestInternalCheckClose(const ZEQuaternion& Actual, const ZEQuaternion& Expected, const ZEQuaternion& Threshold = ZEQuaternion(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD))
{
	return (ZETestInternalCheckClose(Actual.w, Expected.w, Threshold.w) && ZETestInternalCheckClose(Actual.x, Expected.x, Threshold.x) && ZETestInternalCheckClose(Actual.y, Expected.y, Threshold.y) && ZETestInternalCheckClose(Actual.z, Expected.z, Threshold.z));
}

ZE_TEST(ZEQuaternion)
{
	ZE_TEST_ITEM("ZEQuaternion ZEQuaternion::Conjugate() const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;

		Result = Quaternion.Conjugate();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.w, Quaternion.w);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.x, -(Quaternion.x));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.y, -(Quaternion.y));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.z, -(Quaternion.z));
	}

	ZE_TEST_ITEM("void ZEQuaternion::Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;

		ZEQuaternion::Conjugate(Result, Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.w, Quaternion.w);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.x, -(Quaternion.x));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.y, -(Quaternion.y));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.z, -(Quaternion.z));
	}

	ZE_TEST_ITEM("void ZEQuaternion::ConjugateSelf()")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result = Quaternion;

		Result.ConjugateSelf();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.w, Quaternion.w);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.x, -(Quaternion.x));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.y, -(Quaternion.y));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.z, -(Quaternion.z));
	}

	ZE_TEST_ITEM("void ZEQuaternion::ConvertToAngleAxis(float& Angle, ZEVector3& Axis, const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(0.70710683f, 0.70710683f, 0.00000000f, 0.00000000f);

		float Angle;
		ZEVector3 Axis;

		ZEQuaternion::ConvertToAngleAxis(Angle, Axis, Quaternion);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Angle, 1.57079632f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.x, 1.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.y, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.z, 0.0f);
	}

	ZE_TEST_ITEM("void ZEQuaternion::ConvertToEulerAngles(float &x, float &y, float &z, const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		ZEQuaternion::ConvertToEulerAngles(x, y, z, Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(x, ZE_PI_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(y, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(z, 0.0f);

		/*ZE_TEST_CASE("Suspected Rotation Bug")
		{
			ZEOBBox OBBox(ZEVector3::Zero, ZEQuaternion::Identity, ZEVector3::One);
			ZEOBBox OBBox2(ZEVector3::Zero, ZEQuaternion(ZE_PI_2, ZEVector3::UnitX), ZEVector3::One);
			ZEOBBox Result1;
			ZEOBBox Result2;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			ZEVector3 Axis = ZEVector3(1.0f, 1.0f, -1.0f);
			Axis.NormalizeSelf();
			ZEQuaternion Quaternion(ZE_PI_2, Axis);
			ZEQuaternion Quaternion2;

			ZEQuaternion::ConvertToEulerAngles(x, y, z, Quaternion);

			ZEQuaternion::CreateFromEuler(Quaternion2, x, y, z);

			ZEMatrix4x4 Rotation1;
			ZEMatrix4x4::CreateOrientation(Rotation1, ZEVector3::Zero, Quaternion, ZEVector3::One);
			ZEOBBox::Transform(Result1,Rotation1, OBBox);

			ZEMatrix4x4 Rotation2;
			ZEMatrix4x4::CreateOrientation(Rotation2, ZEVector3::Zero, Quaternion2, ZEVector3::One);
			ZEOBBox::Transform(Result2, Rotation2, OBBox);

			ZEInt asd = 0;
			asd += 1;
		}*/

	}

	ZE_TEST_ITEM("void ZEQuaternion::ConvertToEulerAngles(ZEVector3& Rotation, const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEVector3 Result;

		ZEQuaternion::ConvertToEulerAngles(Result, Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEVector3(ZE_PI_2, 0.0f, 0.0f));
	}

	ZE_TEST_ITEM("void ZEQuaternion::ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		Quaternion.NormalizeSelf();
		ZEVector3 Look;
		ZEVector3 Up;

		ZEQuaternion::ConvertToLookAndUp(Look, Up, Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Look, ZEVector3(0.0f, -1.0f, 0.0f));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Up, ZEVector3(0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateFromAngleAxis(ZEQuaternion& Output, float Angle, const ZEVector3& Axis)")
	{
		ZEQuaternion Quaternion;

		ZEQuaternion::CreateFromAngleAxis(Quaternion, ZE_PI_2, ZEVector3::UnitX);
		
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.w, ZE_SQRT1_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.x, ZE_SQRT1_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.y, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.z, 0.0f);
	}

	ZE_TEST_ITEM("void ZEQuaternion::Create(ZEQuaternion& Output, float w, float x, float y, float z)")
	{
		ZEQuaternion Quaternion;
		ZEQuaternion Expected(ZE_PI_2, ZEVector3::UnitX);

		ZEQuaternion::Create(Quaternion, ZE_SQRT1_2, ZE_SQRT1_2, 0.0f, 0.0f);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion, Expected);
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateFromDirection(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up)")
	{
		ZEVector3 Up = ZEVector3::UnitZ;
		ZEVector3 Look(0.0f, -1.0f, 0.0f);
		ZEQuaternion Quaternion;

		ZEQuaternion::CreateFromDirection(Quaternion, Look, Up);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion, ZEQuaternion(ZE_PI_2, ZEVector3::UnitX));
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateFromEuler(ZEQuaternion& Output, const ZEVector3& Rotation)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;
		ZEVector3 Rotation;

		ZEQuaternion::ConvertToEulerAngles(Rotation, Quaternion);

		ZEQuaternion::CreateFromEuler(Result, Rotation);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Quaternion);
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateFromEuler(ZEQuaternion& Output, float x, float y, float z)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		ZEQuaternion::ConvertToEulerAngles(x, y, z, Quaternion);

		ZEQuaternion::CreateFromEuler(Result, x, y, z);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Quaternion);
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateFromMatrix(ZEQuaternion& Output, const ZEMatrix4x4& Matrix)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;
		ZEMatrix4x4 RotationMatrix;

		ZEMatrix4x4::CreateRotation(RotationMatrix, Quaternion);
		ZEQuaternion::CreateFromMatrix(Result, RotationMatrix);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Quaternion);
	}

	ZE_TEST_ITEM("void ZEQuaternion::CreateIdentity(ZEQuaternion& Output)")
	{
		ZEQuaternion IdentityQuaternion;
		ZEQuaternion::CreateIdentity(IdentityQuaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(IdentityQuaternion, ZEQuaternion::Identity);
	}

	ZE_TEST_ITEM("bool ZEQuaternion::Equals(const ZEQuaternion& Quaternion) const")
	{
		ZEQuaternion QuaternionA(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion QuaternionB(0.70710683f, 0.70710683f, 0.00000000f, 0.00000000f);
		ZEQuaternion QuaternionC(ZE_PI_2, ZEVector3::UnitY);

		bool Equal = QuaternionA.Equals(QuaternionB);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = QuaternionA.Equals(QuaternionC);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("bool ZEQuaternion::Equals(const ZEQuaternion& Quaternion, float Threshold) const")
	{
		float Threshold = 0.0001f;
		ZEQuaternion QuaternionA(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion QuaternionB(0.70710690f, 0.70710683f, 0.00000000f, 0.00000000f);
		ZEQuaternion QuaternionC(0.70610683f, 0.70710683f, 0.00000000f, 0.00000000f);

		bool Equal = QuaternionA.Equals(QuaternionB, Threshold);
		ZE_TEST_CHECK_ERROR(Equal);
			
		Equal = QuaternionA.Equals(QuaternionC, Threshold);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("bool ZEQuaternion::IsNormalized() const")
	{
		bool Result;
		ZEQuaternion Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

		Result = Quaternion.IsNormalized();

		ZE_TEST_CHECK_EQUAL(Result, false);

		ZE_TEST_CASE("ZEQuaternion is normalized")
		{
			Quaternion.NormalizeSelf();

			Result = Quaternion.IsNormalized();

			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("bool ZEQuaternion::IsValid() const")
	{
		bool Result;
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3(21.0f, 32.0f, 12.0f));

		Result = Quaternion.IsValid();

		ZE_TEST_CHECK_EQUAL(Result, false);

		ZE_TEST_CASE("ZEQuaternion is valid")
		{
			Quaternion = ZEQuaternion(ZE_PI_2, ZEVector3::UnitX);

			Result = Quaternion.IsValid();

			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("float ZEQuaternion::Length() const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		float Length = Quaternion.Length();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Length, 1.0f);
	}

	ZE_TEST_ITEM("float ZEQuaternion::Length(const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		float Length = ZEQuaternion::Length(Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Length, 1.0f);
	}

	ZE_TEST_ITEM("float ZEQuaternion::LengthSquare() const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		float LengthSquare = Quaternion.LengthSquare();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(LengthSquare, 1.0f);
	}

	ZE_TEST_ITEM("float ZEQuaternion::LengthSquare(const ZEQuaternion& Quaternion)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		float LengthSquare = ZEQuaternion::LengthSquare(Quaternion);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(LengthSquare, 1.0f);
	}
	
	ZE_TEST_ITEM("ZEQuaternion ZEQuaternion::Normalize() const")
	{
		bool Result;
		ZEQuaternion Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
		float Length = Quaternion.Length();

		ZEQuaternion NormalizedQuaternion = Quaternion.Normalize();

		float NormalizedLength = NormalizedQuaternion.Length();
		Result = NormalizedQuaternion.IsNormalized();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Length, 5.4772258f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(NormalizedLength, 1.0f);
		ZE_TEST_CHECK_ERROR(Result);
	}

	ZE_TEST_ITEM("void ZEQuaternion::Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion)")
	{
		bool Result;
		ZEQuaternion Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
		float Length = Quaternion.Length();
		ZEQuaternion NormalizedQuaternion;

		ZEQuaternion::Normalize(NormalizedQuaternion, Quaternion);

		float NormalizedLength = NormalizedQuaternion.Length();
		Result = NormalizedQuaternion.IsNormalized();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Length, 5.4772258f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(NormalizedLength, 1.0f);
		ZE_TEST_CHECK_ERROR(Result);
	}

	ZE_TEST_ITEM("void ZEQuaternion::NormalizeSelf()")
	{
		bool Result;
		ZEQuaternion Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
		float Length = Quaternion.Length();

		Quaternion.NormalizeSelf();

		float NormalizedLength = Quaternion.Length();
		Result = Quaternion.IsNormalized();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Length, 5.4772258f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(NormalizedLength, 1.0f);
		ZE_TEST_CHECK_ERROR(Result);
	}

	ZE_TEST_ITEM("bool ZEQuaternion::operator!=(const ZEQuaternion& RightOperand) const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion ManualQuaternion(ZE_SQRT1_2, ZE_SQRT1_2, 0.0f, 0.0f);
		bool Result;

		Result = (Quaternion != ManualQuaternion);

		ZE_TEST_CHECK_ERROR(!Result);

		ZE_TEST_CASE("ZEQuaternions are not equal")
		{
			ZEQuaternion AnotherQuaternion(ZE_PI_2, ZEVector3::UnitY);

			Result = (Quaternion != AnotherQuaternion);

			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("ZEQuaternion ZEQuaternion::operator*(const ZEQuaternion& Other) const")
	{
		ZEQuaternion Quaternion1(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Quaternion2(ZE_PI_2, ZEVector3::UnitY);
		
		ZEQuaternion Result = Quaternion1 * Quaternion2;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.w, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.x, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.y, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.z, 0.49999997f);
	}

	ZE_TEST_ITEM("ZEVector3 ZEQuaternion::operator*(const ZEVector3& Vector) const")
	{
		ZEVector3 Look(0.0f, 0.0f, 1.0f);
		ZEVector3 Up(0.0f, 1.0f, 0.0f);
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		ZEVector3 ResultLook = Quaternion * Look;
		ZEVector3 ResultUp = Quaternion * Up;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultLook, ZEVector3(0.0f, -1.0f, 0.0f));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultUp, ZEVector3(0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEQuaternion& ZEQuaternion::operator*=(const ZEQuaternion& Other)")
	{
		ZEQuaternion Quaternion1(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Quaternion2(ZE_PI_2, ZEVector3::UnitY);

		Quaternion1 *= Quaternion2;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion1.w, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion1.x, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion1.y, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion1.z, 0.49999997f);
	}

	ZE_TEST_ITEM("float& ZEQuaternion::operator[](ZESize Index)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;

		Result[0] = Quaternion[0];
		Result[1] = Quaternion[2];
		Result[2] = Quaternion[3];
		Result[3] = Quaternion[1];

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));
	}

	ZE_TEST_ITEM("float& ZEQuaternion::operator[](ZESize Index) const")
	{
		const ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Result;

		Result[0] = Quaternion[0];
		Result[1] = Quaternion[2];
		Result[2] = Quaternion[3];
		Result[3] = Quaternion[1];

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));
	}
	
	ZE_TEST_ITEM("bool ZEQuaternion::operator==(const ZEQuaternion& RightOperand) const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion ManualQuaternion(ZE_SQRT1_2, ZE_SQRT1_2, 0.0f, 0.0f);
		bool Result;

		Result = (Quaternion == ManualQuaternion);

		ZE_TEST_CHECK_ERROR(Result);

		ZE_TEST_CASE("ZEQuaternions are not equal")
		{
			ZEQuaternion AnotherQuaternion(ZE_PI_2, ZEVector3::UnitY);

			Result = (Quaternion == AnotherQuaternion);

			ZE_TEST_CHECK_ERROR(!Result);
		}
	}

	ZE_TEST_ITEM("void ZEQuaternion::Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B)")
	{
		ZEQuaternion Quaternion1(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Quaternion2(ZE_PI_2, ZEVector3::UnitY);
		ZEQuaternion Result;

		ZEQuaternion::Product(Result, Quaternion1, Quaternion2);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.w, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.x, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.y, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result.z, 0.49999997f);
	}

	ZE_TEST_ITEM("float ZEQuaternion::RotationAngle() const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		float Angle = Quaternion.RotationAngle();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Angle, 1.57079632f);
	}

	ZE_TEST_ITEM("ZEVector3 ZEQuaternion::RotationAxis() const")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		ZEVector3 Axis = Quaternion.RotationAxis();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.x, 1.0000000f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.y, 0.0000000f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Axis.z, 0.0000000f);
	}

	ZE_TEST_ITEM("void ZEQuaternion::ScaleRotation(ZEQuaternion& Output, const ZEQuaternion& Input, float Scale)")
	{
		ZEQuaternion Input(0.5f, 0.5f, 0.5f, 0.5f);
		float Scale = 2.0f;
		ZEQuaternion Output;

		ZEQuaternion::ScaleRotation(Output, Input, Scale);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Output.w, -0.50000006f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Output.x, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Output.y, 0.49999997f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Output.z, 0.49999997f);
	}

	ZE_TEST_ITEM("void ZEQuaternion::Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor)")
	{
		ZEQuaternion Quaternion1(ZE_PI_2, ZEVector3::UnitX);
		ZEQuaternion Quaternion2(ZE_PI_2 * 3, ZEVector3::UnitX);
		ZEQuaternion Result;

		ZEQuaternion::Slerp(Result, Quaternion1, Quaternion2, 0.5f);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEQuaternion(ZE_PI, ZEVector3::UnitX));

		ZE_TEST_CASE("A factor value of 0 is given as argument.")
		{
			ZEQuaternion::Slerp(Result, Quaternion1, Quaternion2, 0.0f);

			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Quaternion1);
		}
		ZE_TEST_CASE("A factor value of 1 is given as argument.")
		{
			ZEQuaternion::Slerp(Result, Quaternion1, Quaternion2, 1.0f);

			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Quaternion2);
		}
	}

	ZE_TEST_ITEM("void ZEQuaternion::VectorProduct(ZEVector3& Output, const ZEQuaternion& Quaternion, const ZEVector3& Vector)")
	{
		ZEVector3 Look(0.0f, 0.0f, 1.0f);
		ZEVector3 Up(0.0f, 1.0f, 0.0f);
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);
		ZEVector3 ResultLook;
		ZEVector3 ResultUp;

		ZEQuaternion::VectorProduct(ResultLook, Quaternion, Look);
		ZEQuaternion::VectorProduct(ResultUp, Quaternion, Up);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultLook, ZEVector3(0.0f, -1.0f, 0.0f));
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultUp, ZEVector3(0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEQuaternion::ZEQuaternion(float Angle, const ZEVector3& Axis)")
	{
		ZEQuaternion Quaternion(ZE_PI_2, ZEVector3::UnitX);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.w, ZE_SQRT1_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.x, ZE_SQRT1_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.y, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Quaternion.z, 0.0f);
	}

	ZE_TEST_ITEM("ZEQuaternion::ZEQuaternion(float w, float x, float y, float z)")
	{
		ZEQuaternion Quaternion;
		ZEQuaternion Expected(ZE_PI_2, ZEVector3::UnitX);

		ZEQuaternion Result(ZE_SQRT1_2, ZE_SQRT1_2, 0.0f, 0.0f);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Expected);
	}

 }
