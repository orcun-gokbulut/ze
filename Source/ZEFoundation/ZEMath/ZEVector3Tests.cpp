//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVector3Tests.cpp
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
#include "ZEError.h"
#include "ZEMath/ZEAngle.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD))
{
	return (ZETestInternalCheckClose(Actual.x, Expected.x, Threshold.x) && ZETestInternalCheckClose(Actual.y, Expected.y, Threshold.y) && ZETestInternalCheckClose(Actual.z, Expected.z, Threshold.z));
}

ZE_TEST(ZEVector3)
{
	/*ZE_TEST_ITEM("ZEVector3::ZEVector3()")
	{
		ZEVector3 Vector();
	}*/

	ZE_TEST_ITEM("ZEVector3::ZEVector3(const ZEVector3& Start, const ZEVector3& End)")
	{
		ZEVector3 StartVector(1.0f, 2.0f, 3.0f);
		ZEVector3 EndVector(3.0f, 2.0f, 4.0f);

		ZEVector3 Result(StartVector, EndVector);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEVector3::ZEVector3(float x, float y, float z)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZE_TEST_CHECK_EQUAL(Vector, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector3::ZEVector3(ZEVector2& Base, float z)")
	{
		ZEVector2 Vector(2.1f, 3.4f);
		float ZValue = 5.3f;

		ZEVector3 Result(Vector, ZValue);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.1f, 3.4f, 5.3f));
	}

	ZE_TEST_ITEM("inline void ZEVector3::Create(ZEVector3& Out, float x, float y, float z)")
	{
		ZEVector3 Result;

		ZEVector3::Create(Result, 1.0f, 2.0f, 3.0f);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("inline void ZEVector3::Create(ZEVector3& Out, const ZEVector3 &Start, const ZEVector3 &End)")
	{
		ZEVector3 StartVector(1.0f, 2.0f, 3.0f);
		ZEVector3 EndVector(4.0f, 6.0f, 8.0f);
		ZEVector3 Result;

		ZEVector3::Create(Result, StartVector, EndVector);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(3.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::CreateFromSpherical(ZEVector3& Out, float Radius, float Theta, float Phi)")
	{
		ZEVector3 Out;
		float Radius = 2.0f;
		float Theta = ZE_PI_4;
		float Phi = ZE_PI_12;

		ZEVector3::CreateFromSpherical(Out, Radius, Theta, Phi);

		ZE_TEST_CHECK_EQUAL(Out, ZEVector3(0.366025403f, 1.931851652f, 0.366025403f));
	}

	ZE_TEST_ITEM("void ZEVector3::ConvertToSpherical(const ZEVector3& In, float& Radius, float& Theta, float& Phi)")
	{
		ZEVector3 Input(0.366025403f, 1.931851652f, 0.366025403f);
		float Radius = 0.0f;
		float Theta = 0.0f;
		float Phi = 0.0f;

		ZEVector3::ConvertToSpherical(Input, Radius, Theta, Phi);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Radius, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Theta, ZE_PI_4);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Phi, ZE_PI_12);
	}

	ZE_TEST_ITEM("void ZEVector3::CreateFromCylindirical(ZEVector3& Out, float Radius, float Theta, float Height)")
	{
		ZEVector3 Out;
		float Radius = 2.0f;
		float Theta = ZE_PI_4;
		float Height = 5.0f;

		ZEVector3::CreateFromCylindirical(Out, Radius, Theta, Height);

		ZE_TEST_CHECK_EQUAL(Out, ZEVector3(1.414213562f, 5.000000000f, 1.414213562f));
	}

	ZE_TEST_ITEM("void ZEVector3::ConvertToCylindirical(const ZEVector3& In, float& Radius, float& Theta, float& Height)")
	{
		ZEVector3 Input(1.414213562f, 5.000000000f, 1.414213562f);
		float Radius = 0.0f;
		float Theta = 0.0f;
		float Height = 0.0f;

		ZEVector3::ConvertToCylindirical(Input, Radius, Theta, Height);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Radius, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Theta, ZE_PI_4);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Height, 5.0f);
	}

	ZE_TEST_ITEM("bool ZEVector3::IsValid() const")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		bool Result = Vector.IsValid();

		ZE_TEST_CHECK_EQUAL(Result, true);

		ZE_TEST_CASE("A non-valid vector is given as argument.")
		{
			//Insert non-valid vector.
		}
	}

	ZE_TEST_ITEM("inline void ZEVector3::Add(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 4.0f, 6.0f);
		ZEVector3 Result;

		ZEVector3::Add(Result, A, B);
		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(3.0f, 6.0f, 9.0f));
	}

	ZE_TEST_ITEM("inline void ZEVector3::Sub(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);
		ZEVector3 Result;

		ZEVector3::Sub(Result, A, B);
		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(-1.0f, -1.0f, -1.0f));
	}

	ZE_TEST_ITEM("inline void ZEVector3::Scale(ZEVector3& Out, const ZEVector3& A, float s)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		ZEVector3 Result;

		ZEVector3::Scale(Result, Vector, 10.5f);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(10.5f, 21.0f, 31.5f));
	}

	ZE_TEST_ITEM("void ZEVector3::Multiply(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 4.0f, 6.0f);
		ZEVector3 Result;

		ZEVector3::Multiply(Result, A, B);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 8.0f, 18.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Divide(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(2.0f, 8.0f, 18.0f);
		ZEVector3 B(2.0f, 4.0f, 6.0f);
		ZEVector3 Result;

		ZEVector3::Divide(Result, A, B);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Divide(ZEVector3& Out, const ZEVector3& A, float s)")
	{
		ZEVector3 A(10.5f, 21.0f, 31.5f);
		ZEVector3 Result;
		float Value = 10.5f;

		ZEVector3::Divide(Result, A, Value);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("float ZEVector3::DotProduct(const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);

		float Result = ZEVector3::DotProduct(A, B);
		
		ZE_TEST_CHECK_EQUAL(Result, 20.0f);
	}

	ZE_TEST_ITEM("inline void ZEVector3::CrossProduct(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);
		ZEVector3 Result;
		ZEVector3::CrossProduct(Result, A ,B);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(-1.0f, 2.0f, -1.0f));
	}

	ZE_TEST_ITEM("bool ZEVector3::IsNormalized() const")
	{
		ZEVector3 Vector(0.0f, 1.0f, 0.0f);
		
		bool Result = Vector.IsNormalized();

		ZE_TEST_CHECK_EQUAL(Result, true);
		
		ZE_TEST_CASE("A not normalized vector is given as argument.")
		{
			ZEVector3 Vector(2.0f, 1.0f, 8.0f);

			bool Result = Vector.IsNormalized();

			ZE_TEST_CHECK_EQUAL(Result, false);
		}
	}

	ZE_TEST_ITEM("float ZEVector3::LengthSquare(const ZEVector3& Vector)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		float LengthSquare = ZEVector3::LengthSquare(Vector);

		ZE_TEST_CHECK_EQUAL(LengthSquare, 14.0f);
	}

	ZE_TEST_ITEM("float ZEVector3::LengthSquare() const")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		float LengthSquare = Vector.LengthSquare();

		ZE_TEST_CHECK_EQUAL(LengthSquare, 14.0f);
	}

	ZE_TEST_ITEM("float ZEVector3::Length(const ZEVector3& Vector)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		float Length = ZEVector3::Length(Vector);

		ZE_TEST_CHECK_EQUAL(Length, ZEMath::Sqrt(14.0f));
	}

	ZE_TEST_ITEM("inline float ZEVector3::Length() const")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		float Length = Vector.Length();

		ZE_TEST_CHECK_EQUAL(Length, ZEMath::Sqrt(14.0f));
	}

	ZE_TEST_ITEM("float ZEVector3::DistanceSquare(const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(4.0f, 5.0f, 6.0f);

		float DistanceSquare = ZEVector3::DistanceSquare(A, B);

		ZE_TEST_CHECK_EQUAL(DistanceSquare, 27.0f);
	}

	ZE_TEST_ITEM("float ZEVector3::Distance(const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(4.0f, 5.0f, 6.0f);

		float Distance = ZEVector3::Distance(A, B);

		ZE_TEST_CHECK_EQUAL(Distance, ZEMath::Sqrt(27.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Normalize(ZEVector3& Out, const ZEVector3& Vector)")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		ZEVector3 Result;

		ZEVector3::Normalize(Result, Vector);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f/ZEMath::Sqrt(14.0f), 2.0f/ZEMath::Sqrt(14.0f), 3.0f/ZEMath::Sqrt(14.0f)));	
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::Normalize() const")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		ZEVector3 Result;

		Result = Vector.Normalize();

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f/ZEMath::Sqrt(14.0f), 2.0f/ZEMath::Sqrt(14.0f), 3.0f/ZEMath::Sqrt(14.0f)));	
	}

	ZE_TEST_ITEM("void ZEVector3::NormalizeSelf()")
	{
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		Vector.NormalizeSelf();

		ZE_TEST_CHECK_EQUAL(Vector, ZEVector3(1.0f/ZEMath::Sqrt(14.0f), 2.0f/ZEMath::Sqrt(14.0f), 3.0f/ZEMath::Sqrt(14.0f)));	
	}

	ZE_TEST_ITEM("void ZEVector3::Lerp(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B, float Factor)")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(6.0f, 9.0f, 5.0f);
		ZEVector3 Result;
		float Factor= 0.65f;

		ZEVector3::Lerp(Result, A, B, Factor);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEVector3(4.25f, 6.55f, 4.30f));

		ZE_TEST_CASE("A factor value of 0 is given as argument.")
		{
			Factor = 0.0f;

			ZEVector3::Lerp(Result, A, B, Factor);

			ZE_TEST_CHECK_EQUAL(Result, A);
		}

		ZE_TEST_CASE("A factor value of 1 is given as argument.")
		{
			Factor = 1.0f;

			ZEVector3::Lerp(Result, A, B, Factor);

			ZE_TEST_CHECK_EQUAL(Result, B);
		}
	}

	ZE_TEST_ITEM("void ZEVector3::Max(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 4.0f, 5.0f);
		ZEVector3 B(2.0f, 3.0f, 6.0f);
		ZEVector3 Result;

		ZEVector3::Max(Result, A, B);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 6.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Min(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)")
	{
		ZEVector3 A(1.0f, 4.0f, 5.0f);
		ZEVector3 B(2.0f, 3.0f, 6.0f);
		ZEVector3 Result;

		ZEVector3::Min(Result, A, B);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 3.0f, 5.0f));
	}

	ZE_TEST_ITEM("float ZEVector3::Max() const")
	{
		ZEVector3 A(1.0f, 4.0f, 5.0f);
		float Result;

		Result = A.Max();

		ZE_TEST_CHECK_EQUAL(Result, 5.0f);
	}

	ZE_TEST_ITEM("float ZEVector3::Min() const")
	{
		ZEVector3 A(1.0f, 4.0f, 5.0f);
		float Result;

		Result = A.Min();

		ZE_TEST_CHECK_EQUAL(Result, 1.0f);
	}

	ZE_TEST_ITEM("void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, float MinValue, float MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Min = 2.0f;
		float Max = 5.0f;

		ZEVector3::Clamp(Result, A, Min, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::Clamp(float MinValue, float MaxValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Min = 2.0f;
		float Max = 5.0f;

		Result = A.Clamp(Min, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue, const ZEVector3& MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(5.0f, 5.0f, 5.0f);
		ZEVector3 Min(2.0f, 2.0f, 2.0f);
		ZEVector3 Result;

		ZEVector3::Clamp(Result, A, Min, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::Clamp(const ZEVector3 & MinValue, const ZEVector3 & MaxValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(5.0f, 5.0f, 5.0f);
		ZEVector3 Min(2.0f, 2.0f, 2.0f);
		ZEVector3 Result;

		Result = A.Clamp(Min, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampSelf(float MinValue, float MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		float Min = 2.0f;
		float Max = 5.0f;

		A.ClampSelf(Min, Max);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampSelf(const ZEVector3 & MinValue, const ZEVector3 & MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(5.0f, 5.0f, 5.0f);
		ZEVector3 Min(2.0f, 2.0f, 2.0f);

		A.ClampSelf(Min, Max);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(2.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampLower(ZEVector3 & Out, const ZEVector3 & Vector, float MinValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Min = 2.0f;

		ZEVector3::ClampLower(Result, A, Min);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 6.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::ClampLower(float MinValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Min = 2.0f;

		Result = A.ClampLower(Min);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 4.0f, 6.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampLower(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Min(2.0f, 5.0f, 2.0f);
		ZEVector3 Result;

		ZEVector3::ClampLower(Result, A, Min);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 5.0f, 6.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::ClampLower(const ZEVector3& MinValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Min(2.0f, 5.0f, 2.0f);
		ZEVector3 Result;

		Result = A.ClampLower(Min);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(2.0f, 5.0f, 6.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampLowerSelf(float MinValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		float Min = 2.0f;

		A.ClampLowerSelf(Min);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(2.0f, 4.0f, 6.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampLowerSelf(const ZEVector3 & MinValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Min(2.0f, 5.0f, 2.0f);

		A.ClampLowerSelf(Min);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(2.0f, 5.0f, 6.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampUpper(ZEVector3 & Out, const ZEVector3 & Vector, float MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Max = 5.0f;

		ZEVector3::ClampUpper(Result, A, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::ClampUpper(float MaxValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Result;
		float Max = 5.0f;

		Result = A.ClampUpper(Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampUpper(ZEVector3 & Out, const ZEVector3 & Vector, const ZEVector3 & MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(2.0f, 3.0f, 5.0f);
		ZEVector3 Result;

		ZEVector3::ClampUpper(Result, A, Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 3.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::ClampUpper(const ZEVector3 & MaxValue) const")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(2.0f, 3.0f, 5.0f);
		ZEVector3 Result;

		Result = A.ClampUpper(Max);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 3.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampUpperSelf(float MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);

		float Max = 5.0f;

		A.ClampUpperSelf(Max);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(1.0f, 4.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::ClampUpperSelf(const ZEVector3 & MaxValue)")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 Max(2.0f, 3.0f, 5.0f);

		A.ClampUpperSelf(Max);

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(1.0f, 3.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::Saturate(ZEVector3 & Out, const ZEVector3 & Vector)")
	{
		ZEVector3 A(2.0f, 0.4f, -1.2f);
		ZEVector3 Result;

		ZEVector3::Saturate(Result, A);

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 0.4f, 0.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::Saturate() const")
	{
		const ZEVector3 A(2.0f, 0.4f, -1.2f);
		ZEVector3 Result;

		Result = A.Saturate();

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(1.0f, 0.4f, 0.0f));
	}

	ZE_TEST_ITEM("void ZEVector3::SaturateSelf()")
	{
		ZEVector3 A(2.0f, 0.4f, -1.2f);
		ZEVector3 Result;

		A.SaturateSelf();

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(1.0f, 0.4f, 0.0f));
	}

	ZE_TEST_ITEM("bool ZEVector3::Equals(const ZEVector3& Vector) const")
	{
		ZEVector3 VectorA(1.0f, 2.0f, 3.0f);
		ZEVector3 VectorB(0.0f, 2.0f, 3.0f);
		ZEVector3 VectorC(1.00000001f, 2.0f, 3.0f);

		bool Equal = VectorA.Equals(VectorB);
		ZE_TEST_CHECK_ERROR(!Equal);

		Equal = VectorA.Equals(VectorC);
		ZE_TEST_CHECK_ERROR(Equal);
	}

	ZE_TEST_ITEM("bool ZEVector3::Equals(const ZEVector3& Vector, float Threshold) const")
	{
		float Threshold = 0.1f;
		ZEVector3 VectorA(1.11f, 2.11f, 3.11f);
		ZEVector3 VectorB(1.1f, 2.11f, 3.11f);
		ZEVector3 VectorC(1.0f, 2.11f, 3.11f);

		bool Equal = VectorA.Equals(VectorB, Threshold);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = VectorA.Equals(VectorC, Threshold);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator-() const")
	{
		ZEVector3 A(10.0f, 1.0f, 4.0f);

		ZEVector3 Result = -A;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(-10.0f, -1.0f, -4.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator+(const ZEVector3 & RightOperand) const")
	{
		ZEVector3 A(10.0f, 1.0f, 2.0f);
		ZEVector3 B(3.0f, 5.0f, 6.0f);

		ZEVector3 Result = A + B;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(13.0f, 6.0f, 8.0f));
	}

	ZE_TEST_ITEM("ZEVector3 & ZEVector3::operator+=(const ZEVector3 & RightOperand)")
	{
		ZEVector3 A(10.0f, 1.0f, 2.0f);
		ZEVector3 B(3.0f, 5.0f, 6.0f);

		A += B;

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(13.0f, 6.0f, 8.0f));

	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator-(const ZEVector3 & RightOperand) const")
	{
		ZEVector3 A(10.0f, 5.0f, 3.0f);
		ZEVector3 B(3.0f, 2.0f, 2.0f);

		ZEVector3 Result = A - B;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(7.0f, 3.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEVector3& ZEVector3::operator-=(const ZEVector3 &RightOperand)")
	{
		ZEVector3 A(10.0f, 5.0f, 3.0f);
		ZEVector3 B(3.0f, 2.0f, 2.0f);

		A -= B;

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(7.0f, 3.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator*(const ZEVector3 & RightOperand) const")
	{
		ZEVector3 A(10.0f, 1.0f, 6.0f);
		ZEVector3 B(3.0f, 2.0f, 3.0f);

		ZEVector3 Result = A * B;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(30.0f, 2.0f, 18.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator*(float s) const")
	{

		ZEVector3 A(3.0f, 2.0f, 1.0f);
		float ScalarValue = 2.5f;

		ZEVector3 Result = A * ScalarValue;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(7.5f, 5.0f, 2.5f));
	}

	ZE_TEST_ITEM("ZEVector3 & ZEVector3::operator*=(const ZEVector3 & RightOperand)")
	{
		ZEVector3 A(10.0f, 1.0f, 6.0f);
		ZEVector3 B(3.0f, 2.0f, 3.0f);

		A *= B;

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(30.0f, 2.0f, 18.0f));
	}

	ZE_TEST_ITEM("ZEVector3 & ZEVector3::operator*=(float S)")
	{
		ZEVector3 A(2.1f, 3.0f, 4.0f);
		float ScalarValue = 7.0f;

		A *= ScalarValue;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(A, ZEVector3(14.7f, 21.0f, 28.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator/(const ZEVector3 & RightOperand) const")
	{
		ZEVector3 A(10.0f, 6.0f, 8.0f);
		ZEVector3 B(2.0f, 3.0f, 2.0f);

		ZEVector3 Result = A / B;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(5.0f, 2.0f, 4.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEVector3::operator/(float s) const")
	{
		ZEVector3 A(10.0f, 4.0f, 6.0f);
		float ScalarValue = 2.0f;

		ZEVector3 Result = A / ScalarValue;

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(5.0f, 2.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector3 & ZEVector3::operator/=(const ZEVector3 & RightOperand)")
	{
		ZEVector3 A(10.0f, 4.0f, 6.0f);
		ZEVector3 B(2.0f, 1.0f, 2.0f);

		A /= B;

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(5.0f, 4.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector3 & ZEVector3::operator/=(float S)")
	{
		ZEVector3 A(10.0f, 1.0f, 20.0f);
		float ScalarValue = 5.0f;

		A /= ScalarValue;

		ZE_TEST_CHECK_EQUAL(A, ZEVector3(2.0f, 0.2f, 4.0f));
	}

	ZE_TEST_ITEM("float ZEVector3::operator[](ZESize Index) const")
	{
		const ZEVector3 A(24.0f, 12.0f, 32.0f);
		ZEVector3 Result;

		Result[0] = A[1];
		Result[1] = A[2];
		Result[2] = A[0];

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(12.0f, 32.0f, 24.0f));
	}

	ZE_TEST_ITEM("float & ZEVector3::operator[](ZESize Index)")
	{
		ZEVector3 A(24.0f, 12.0f, 32.0f);
		ZEVector3 Result;

		Result[0] = A[1];
		Result[1] = A[2];
		Result[2] = A[0];

		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(12.0f, 32.0f, 24.0f));
	}

	ZE_TEST_ITEM("bool ZEVector3::operator==(const ZEVector3 &RightOperand) const")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(1.0f, 2.0f, 3.0f);

		bool Result = (A == B);

		ZE_TEST_CHECK_EQUAL(Result, true);

		ZE_TEST_CASE("Checking two different vectors for falsifying")
		{
			ZEVector3 C(1.0f, 3.0f, 5.0f);

			bool Result = (A == C);

			ZE_TEST_CHECK_EQUAL(Result, false);
		}
	}

	ZE_TEST_ITEM("bool ZEVector3::operator!=(const ZEVector3 &RightOperand) const")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(1.0f, 3.0f, 5.0f);

		bool Result = (A != B);

		ZE_TEST_CHECK_EQUAL(Result, true);

		ZE_TEST_CASE("Checking two identical vectors for falsifying")
		{
			ZEVector3 C(1.0f, 2.0f, 3.0f);

			bool Result = (A != C);

			ZE_TEST_CHECK_EQUAL(Result, false);
		}
	}
}
