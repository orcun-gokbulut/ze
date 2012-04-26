//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVector4Tests.cpp
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
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMath.h"

bool ZETestInternalCheckClose(const ZEVector4& Actual, const ZEVector4& Expected, const ZEVector4& Threshold = ZEVector4(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD))
{
	return (ZETestInternalCheckClose(Actual.x, Expected.x, Threshold.x) && ZETestInternalCheckClose(Actual.y, Expected.y, Threshold.y) && ZETestInternalCheckClose(Actual.z, Expected.z, Threshold.z) && ZETestInternalCheckClose(Actual.w, Expected.w, Threshold.w));
}

ZETestSuite(ZEVector4)
{
	ZETest("ZEVector4::ZEVector4(float x, float y, float z, float w)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 2.0f, 3.0f);

		ZETestCheckEqual(Vector, ZEVector4(1.0f, 2.0f, 2.0f, 3.0f));	
	}

	ZETest("ZEVector4::ZEVector4(const ZEVector2 & Base, float z, float w)")
	{
		ZEVector2 Vector2(2.1f, 3.4f);

		ZEVector4 Vector4(Vector2, 5.3f, 2.0f);

		ZETestCheckEqual(Vector4, ZEVector4(2.1f, 3.4f, 5.3f, 2.0f));
	}

	ZETest("ZEVector4::ZEVector4(const ZEVector3 & Base, float w)")
	{
		ZEVector3 Vector3(2.1f, 3.4f, 1.5f);

		ZEVector4 Vector4(Vector3, 2.0f);

		ZETestCheckEqual(Vector4, ZEVector4(2.1f, 3.4f, 1.5f, 2.0f));
	}

	ZETest("ZEVector4::ZEVector4(const ZEVector4 & Start, const ZEVector4 & End);")
	{
		ZEVector4 StartVector(1.0f, 2.0f, 3.0f, 5.0f);
		ZEVector4 EndVector(3.0f, 2.0f, 4.0f, 1.0f);

		ZEVector4 Result(StartVector, EndVector);

		ZETestCheckClose(Result, ZEVector4(2.0f, 0.0f, 1.0f, -4.0f));
	}

	ZETest("ZEVector4::Create(float x, float y, float z, float w)")
	{
		ZEVector4 Vector;

		Vector.Create(1.0f, 2.0f, 3.0f, 4.0f);

		ZETestCheckEqual(Vector, ZEVector4(1.0f, 2.0f, 3.0f, 4.0f));
	}

	ZETest("ZEVector4::Create(const ZEVector4 & Start, const ZEVector4 & End)")
	{
		ZEVector4 Result;

		Result.Create(ZEVector4(1.0f, 2.0f, 3.0f, 4.0f), ZEVector4(4.0f, 6.0f, 8.0f, 10.0f));

		ZETestCheckClose(Result, ZEVector4(3.0f, 4.0f, 5.0f, 6.0f));
	}

	ZETest("ZEVector4::Add(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 8.0f);
		ZEVector4 Result;

		ZEVector4::Add(Result, A, B);

		ZETestCheckClose(Result, ZEVector4(3.0f, 6.0f, 9.0f, 12.0f));
	}

	ZETest("ZEVector4::Sub(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 8.0f);
		ZEVector4 Result;

		ZEVector4::Sub(Result, A, B);

		ZETestCheckClose(Result, ZEVector4(-1.0f, -2.0f, -3.0f, -4.0f));
	}

	ZETest("ZEVector4::Scale(ZEVector4 & Out, const ZEVector4 & A, float s)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 Result;

		ZEVector4::Scale(Result, Vector, 10.5f);

		ZETestCheckClose(Result, ZEVector4(10.5f, 21.0f, 31.5f, 42.0f));

		ZETestCase("Multiplication with s = 0")
		{
			ZEVector4::Scale(Result, Vector, 0.0f);

			ZETestCheckClose(Result, ZEVector4::Zero);
		}

	}

	ZETest("ZEVector4::Multiply(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 10.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 11.0f);
		ZEVector4 Result;

		ZEVector4::Multiply(Result, A, B);
		ZETestCheckClose(Result, ZEVector4(2.0f, 8.0f, 18.0f, 110.0f));

		ZETestCase("Multiplication with ZEVector4::Zero")
		{
			ZEVector4 A(1.0f, 2.0f, 3.0f, 10.0f);
			ZEVector4 B = ZEVector4::Zero;
			ZEVector4 Result;

			ZEVector4::Multiply(Result, A, B);
			ZETestCheckClose(Result, ZEVector4::Zero);
		}	
	}

	ZETest("ZEVector4::DotProduct(const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 3.0f, 4.0f, 5.0f);

		float Result = ZEVector4::DotProduct(A, B);

		ZETestCheckClose(Result, 40.0f);

		ZETestCase("DotProduction with ZEVector4::Zero")
		{
			ZEVector4 Vector(1.0f, 2.0f, 3.0f, 13.0f);

			float Result = ZEVector4::DotProduct(Vector, ZEVector4::Zero);

			ZETestCheckClose(Result, 0.0f);
		}
	}
	
	ZETest("ZEVector4::Normalize(ZEVector4 & Out, const ZEVector4 & Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 NormalizedVector;

		ZEVector4::Normalize(NormalizedVector, Vector);

		ZETestCheckClose(NormalizedVector, ZEVector4(1.0f/ZEMath::Sqrt(30.0f), 2.0f/ZEMath::Sqrt(30.0f), 3.0f/ZEMath::Sqrt(30.0f), 4.0f/ZEMath::Sqrt(30.0f)));	
	}

	ZETest("ZEVector4 ZEVector4::Normalize() const")
	{
		ZEVector4 Vector(2.0f, 2.0f, 2.0f, 2.0f);
		ZEVector4 NormalizedVector;

		NormalizedVector = Vector.Normalize();

		ZETestCheckClose(NormalizedVector, ZEVector4(0.5f, 0.5f, 0.5f, 0.5f));
	}

	ZETest("void ZEVector2::NormalizeSelf()")
	{
		ZEVector4 Vector(2.0f, 2.0f, 2.0f, 2.0f);

		Vector.NormalizeSelf();

		ZETestCheckClose(Vector, ZEVector4(0.5f, 0.5f, 0.5f, 0.5f));
	}

	ZETest("ZEVector4::IsNormalized() const")
	{
		ZEVector4 Vector(2.0f, 2.0f, 2.0f, 2.0f);
		ZEVector4 NormalizedVector;

		NormalizedVector = Vector.Normalize();

		ZETestCheck(NormalizedVector.IsNormalized());
	}

	ZETest("bool ZEVector4::IsValid() const")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		bool Result = Vector.IsValid();

		ZETestCheckEqual(Result, true);

		ZETestCase("A non-valid vector is given as argument.")
		{
			//Insert non-valid vector.
		}
	}

	ZETest("ZEVector4::Length(const ZEVector4 & Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 4.0f, 7.0f);

		float Result = ZEVector4::Length(Vector);

		ZETestCheckClose(Result, ZEMath::Sqrt(70.0f));
	}

	ZETest("ZEVector4::Length() const")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		float Result = ZEVector4::Length(Vector);

		ZETestCheckClose(Result, ZEMath::Sqrt(30.0f));
	}

	ZETest("ZEVector4::LengthSquare(const ZEVector4 & Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 5.0f);
		float Result = ZEVector4::LengthSquare(Vector);

		ZETestCheckClose(Result, 39.0f);
	}

	ZETest("float ZEVector4::LengthSquare() const")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 5.0f);
		float Result = Vector.LengthSquare();

		ZETestCheckClose(Result, 39.0f);
	}

	ZETest("ZEVector4::Distance(const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 5.0f);
		ZEVector4 B(4.0f, 5.0f, 6.0f, 7.0f);

		float Result = ZEVector4::Distance(A, B);

		ZETestCheckClose(Result, ZEMath::Sqrt(31.0f));
	}

	ZETest("ZEVector4::DistanceSquare(const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 2.0f);
		ZEVector4 B(4.0f, 5.0f, 6.0f, 7.0f);

		float Result = ZEVector4::DistanceSquare(A, B);

		ZETestCheckClose(Result, 52.0f);
	}

	ZETest("ZEVector4::Lerp(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B, float Factor)")
	{
		ZEVector4 A(10.0f, 10.0f, 10.0f, 10.0f);
		ZEVector4 B(0.0f, 0.0f, 0.0f, 0.0f);
		ZEVector4 Result;
		float Factor = 0.5f;

		ZEVector4::Lerp(Result, A, B, Factor);

		ZETestCheckClose(Result, ZEVector4(5.0f, 5.0f, 5.0f, 5.0f));
	}

	ZETest("ZEVector4::Max(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(5.0f, 2.0f, 4.0f, 9.0f);
		ZEVector4 B(3.0f, 7.0f, 6.0f, 5.0f);
		ZEVector4 Result;

		ZEVector4::Max(Result, A, B);
		ZETestCheckEqual(Result, ZEVector4(5.0f, 7.0f, 6.0f, 9.0f));
	}

	ZETest("ZEVector4::Max() const")
	{
		ZEVector4 Vector(1.0f, 4.0f, 5.0f, 2.0f);

		float Result = Vector.Max();
		ZETestCheckEqual(Result, 5.0f);
	}

	ZETest("ZEVector4::Min(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(5.0f, 2.0f, 4.0f, 9.0f);
		ZEVector4 B(3.0f, 7.0f, 6.0f, 5.0f);
		ZEVector4 Result;

		ZEVector4::Min(Result, A, B);
		ZETestCheckEqual(Result, ZEVector4(3.0f, 2.0f, 4.0f, 5.0f));
	}

	ZETest("ZEVector4::Min() const")
	{
		ZEVector4 Vector(1.0f, 4.0f, 5.0f, 2.0f);

		float Result = Vector.Min();
		ZETestCheckEqual(Result, 1.0f);
	}

	ZETest("ZEVector4::Saturate(ZEVector4 & Out, const ZEVector4 & Vector)")
	{
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 6.0f);
		ZEVector4 Result;

		ZEVector4::Saturate(Result, Vector);

		ZETestCheckClose(Result, ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));

		ZETestCase("Saturating a ZEVector4 containing negative signed components")
		{
			ZEVector4 AnotherVector(-2.0f, 0.0f, 3.0f, -1.0f);
			ZEVector4 Result;

			ZEVector4::Saturate(Result, AnotherVector);

			ZETestCheckEqual(Result, ZEVector4(0.0f, 0.0f, 1.0f, 0.0f));
		}
	}

	ZETest("ZEVector4::Saturate() const")
	{
		ZEVector4 Vector(2.0f, -4.0f, 3.0f, -2.0f);

		ZEVector4 Result = Vector.Saturate();

		ZETestCheckEqual(Result, ZEVector4(1.0f, 0.0f, 1.0f, 0.0f));
	}

	ZETest("ZEVector4::SaturateSelf()")
	{
		ZEVector4 Vector(1.0f, -1.0f, 3.0f, 6.0f);

		Vector.SaturateSelf();

		ZETestCheckEqual(Vector, ZEVector4(1.0f, 0.0f, 1.0f, 1.0f));
	}

	ZETest("ZEVector4::Clamp(float MinValue, float MaxValue) const")
	{
		ZEVector4 Vector(10.0f, 5.0f, 3.0f, 1.0f);
		float MinValue = 2.0f;
		float MaxValue = 5.0f;

		ZEVector4 Result = Vector.Clamp(MinValue, MaxValue);

		ZETestCheckEqual(Result, ZEVector4(5.0f, 5.0f, 3.0f, 2.0f));
	}

	ZETest("ZEVector4::Clamp(const ZEVector4 & MinValue, const ZEVector4 & MaxValue) const")
	{
		ZEVector4 Vector(10.0f, 3.0f, 2.0f, 1.0f);
		ZEVector4 MinValue(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 MaxValue(3.0f, 5.0f, 6.0f, 7.0f);

		ZEVector4 Result = Vector.Clamp(MinValue, MaxValue);

		ZETestCheckEqual(Result, ZEVector4(3.0f, 3.0f, 3.0f, 4.0f));
	}

	ZETest("ZEVector4::Clamp(ZEVector4 & Out, const ZEVector4 & Vector, float MinValue, float MaxValue)")
	{
		ZEVector4 Vector(10.0f, 5.0f, 3.0f, 1.0f);
		ZEVector4 Result;
		float MinValue = 2.0f;
		float MaxValue = 5.0f;

		ZEVector4::Clamp(Result, Vector, MinValue, MaxValue);

		ZETestCheckEqual(Result, ZEVector4(5.0f, 5.0f, 3.0f, 2.0f));
	}

	ZETest("ZEVector4::Clamp(ZEVector4 & Out, const ZEVector4 & Vector, const ZEVector4 & MinValue, const ZEVector4 & MaxValue)")
	{
		ZEVector4 Vector(10.0f, 3.0f, 2.0f, 1.0f);
		ZEVector4 Result;
		ZEVector4 MinValue(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 MaxValue(3.0f, 5.0f, 6.0f, 7.0f);

		ZEVector4::Clamp(Result, Vector, MinValue, MaxValue);

		ZETestCheckEqual(Result, ZEVector4(3.0f, 3.0f, 3.0f, 4.0f));
	}

	ZETest("ZEVector4::ClampSelf(float MinValue, float MaxValue)")
	{
		ZEVector4 Vector(10.0f, 5.0f, 3.0f, 1.0f);
		float MinValue = 2.0f;
		float MaxValue = 5.0f;

		Vector.ClampSelf(MinValue, MaxValue);

		ZETestCheckEqual(Vector, ZEVector4(5.0f, 5.0f, 3.0f, 2.0f));
	}

	ZETest("ZEVector4::ClampSelf(const ZEVector4 & MinValue, const ZEVector4 & MaxValue)")
	{
		ZEVector4 Vector(10.0f, 3.0f, 2.0f, 1.0f);
		ZEVector4 MinValue(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 MaxValue(3.0f, 5.0f, 6.0f, 7.0f);

		Vector.ClampSelf(MinValue, MaxValue);

		ZETestCheckEqual(Vector, ZEVector4(3.0f, 3.0f, 3.0f, 4.0f));
	}

	ZETest("ZEVector4::Divide(ZEVector4 & Out, const ZEVector4 & A, const ZEVector4 & B)")
	{
		ZEVector4 A(2.0f, 8.0f, 18.0f, 4.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 2.0f);
		ZEVector4 Result;

		ZEVector4::Divide(Result, A, B);

		ZETestCheckClose(Result, ZEVector4(1.0f, 2.0f, 3.0f, 2.0f));
	}

	ZETest("ZEVector4::Divide(ZEVector4 & Out, const ZEVector4 & A, float s)")
	{
		ZEVector4 Vector(10.5f, 21.0f, 31.5f, 21.0f);
		ZEVector4 Result;
		float ScalarValue = 10.5f;

		ZEVector4::Divide(Result, Vector, ScalarValue);

		ZETestCheckClose(Result, ZEVector4(1.0f, 2.0f, 3.0f, 2.0f));
	}

	ZETest("ZEVector4::operator+(const ZEVector4 & RightOperand) const")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		ZEVector4 Result = A + B;

		ZETestCheckClose(Result, ZEVector4(13.0f, 6.0f, 5.5f, 8.0f));
	}

	ZETest("ZEVector4::operator+=(const ZEVector4 & RightOperand)")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		A += B;

		ZETestCheckClose(A, ZEVector4(13.0f, 6.0f, 5.5f, 8.0f));
	}

	ZETest("ZEVector4::operator-(const ZEVector4 & RightOperand) const")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		ZEVector4 Result = A - B;

		ZETestCheckClose(Result, ZEVector4(7.0f, -4.0f, 0.5f, 0.0f));
	}

	ZETest("ZEVector4 ZEVector4::operator-() const")
	{
		ZEVector4 Vector(10.0f, 1.0f, 3.0f, 4.0f);

		ZEVector4 Result = -Vector ;
		ZETestCheckEqual(Result, ZEVector4(-10.0f, -1.0f, -3.0f, -4.0f));
	}

	ZETest("ZEVector4::operator-=(const ZEVector4 & RightOperand)")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		A -= B;
		ZETestCheckClose(A, ZEVector4(7.0f, -4.0f, 0.5f, 0.0f));
	}

	ZETest("ZEVector4::operator*(const ZEVector4 & RightOperand) const")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		ZEVector4 Result = A * B;

		ZETestCheckClose(Result, ZEVector4(30.0f, 5.0f, 7.5f, 16.0f));
	}

	ZETest("ZEVector4::operator*(float S) const")
	{
		ZEVector4 Vector(10.0f, 1.0f, 3.0f, 4.0f);
		float ScalarValue = 1.5f;

		ZEVector4 Result = Vector * ScalarValue;

		ZETestCheckClose(Result, ZEVector4(15.0f, 1.5f, 4.5f, 6.0f));
	}

	ZETest("ZEVector4& ZEVector4::operator *= (const ZEVector4 &RightOperand)")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(3.0f, 5.0f, 2.5f, 4.0f);

		A *= B;

		ZETestCheckClose(A, ZEVector4(30.0f, 5.0f, 7.5f, 16.0f));
	}

	ZETest("ZEVector4& ZEVector4::operator *= (float s)")
	{
		ZEVector4 Vector(10.0f, 1.0f, 3.0f, 4.0f);
		float ScalarValue = 1.5f;

		Vector *= ScalarValue;

		ZETestCheckClose(Vector, ZEVector4(15.0f, 1.5f, 4.5f, 6.0f));
	}

	ZETest("ZEVector4::operator/(float S) const")
	{
		ZEVector4 Vector(10.0f, 1.0f, 3.0f, 4.0f);
		float ScalarValue = 2.0f;

		ZEVector4 Result = Vector / ScalarValue;

		ZETestCheckClose(Result, ZEVector4(5.0f, 0.5f, 1.5f, 2.0f));
	}

	ZETest("ZEVector4::operator/(const ZEVector4 & RightOperand) const")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 5.0f, 2.5f, 4.0f);

		ZEVector4 Result = A / B;

		ZETestCheckClose(Result, ZEVector4(5.0f, 0.2f, 1.2f, 1.0f));
	}

	ZETest("ZEVector4::operator/=(const ZEVector4 & RightOperand)")
	{
		ZEVector4 A(10.0f, 1.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 5.0f, 2.5f, 4.0f);

		A /= B;

		ZETestCheckClose(A, ZEVector4(5.0f, 0.2f, 1.2f, 1.0f));
	}

	ZETest("ZEVector4::operator/=(float S)")
	{
		ZEVector4 Vector(10.0f, 1.0f, 3.0f, 4.0f);
		float ScalarValue = 2.0f;

		Vector /= ScalarValue;

		ZETestCheckClose(Vector, ZEVector4(5.0f, 0.5f, 1.5f, 2.0f));
	}

	ZETest("ZEVector4::operator[](ZESize Index)")
	{
		ZEVector4 Vector(2.0f, 4.0f, 6.0f, 3.0f);
		ZEVector4 Result;

		Result[1] = Vector[0];
		Result[2] = Vector[1];
		Result[3] = Vector[2];
		Result[0] = Vector[3];

		ZETestCheckEqual(Result, ZEVector4(3.0f, 2.0f, 4.0f, 6.0f));
	}
	
	ZETest("ZEVector4::operator[](ZESize Index) const")
	{
		const ZEVector4 Vector(2.0f, 4.0f, 6.0f, 3.0f);
		ZEVector4 Result;

		Result[1] = Vector[0];
		Result[2] = Vector[1];
		Result[3] = Vector[2];
		Result[0] = Vector[3];

		ZETestCheckEqual(Result, ZEVector4(3.0f, 2.0f, 4.0f, 6.0f));
	}

	ZETest("bool ZEVector4::operator==(const ZEVector4 & RightOperand) const")
	{
		ZEVector4 A(3.0f, 2.0f, 1.0f, 2.5f);
		ZEVector4 B(3.0f, 2.0f, 1.0f, 2.5f);

		bool Result = (A == B);

		ZETestCheck(Result);

		ZETestCase("Checking two different vectors for falsifying")
		{
			ZEVector4 C(2.0f, 8.0f, 5.0f, 2.0f);

			bool Result2 = (A == C);

			ZETestCheckEqual(Result2, false);
		}
	}

	ZETest("bool ZEVector4::operator != (const ZEVector4 &RightOperand) const")
	{
		ZEVector4 A(3.0f, 2.0f, 1.0f, 2.5f);
		ZEVector4 B(3.0f, 2.0f, 1.0f, 2.5f);

		bool Result = (A != B);

		ZETestCheckEqual(Result, false);

		ZETestCase("Checking two different vectors for falsifying")
		{
			ZEVector4 C(2.0f, 8.0f, 5.0f, 2.0f);

			bool Result2 = (A != C);

			ZETestCheck(Result2);
		}
	}
}

