//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVector2Tests.cpp
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
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

#include "ZEMath/ZEMath.h"

bool ZETestInternalCheckClose(const ZEVector2& Actual, const ZEVector2& Expected, const ZEVector2& Threshold = ZEVector2(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD))
{	
	return(ZETestInternalCheckClose(Actual.x, Expected.x, Threshold.x) && ZETestInternalCheckClose(Actual.y, Expected.y, Threshold.y));
}

ZE_TEST(ZEVector2)
{
	ZE_TEST_ITEM("ZEVector2::ZEVector2()")
	{
		ZEVector2 Vector(1.0f, 0.0f);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(1.0f, 0.0f));
	}
		
	ZE_TEST_ITEM("ZEVector2::ZEVector2(const ZEVector2& Start, const ZEVector2& End)")
	{
		ZEVector2 Start(1.0f, 0.0f);
		ZEVector2 End(2.0f, 3.0f);
		
		ZEVector2 Vector(Start, End);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(1.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector2::ZEVector2(float x, float y)")
	{
		float x = 2.5f;
		float y = 3.0f;
			
		ZEVector2 Vector(x, y);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(2.5f, 3.0f));
	}
	
	ZE_TEST_ITEM("inline void ZEVector2::Create(float x, float y)")
	{
		float x = 1.0f;
		float y = 0.5f;
		ZEVector2 Vector;
			
		Vector.Create(x, y);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(1.0f, 0.5f));

	}

	ZE_TEST_ITEM("inline void ZEVector2::Create(const ZEVector2 &Start, const ZEVector2 &End)")
	{
		ZEVector2 Start(3.0f, 5.0f);
		ZEVector2 End(1.0f, 6.0f);
		ZEVector2 Vector;
			
		Vector.Create(Start, End);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(-2.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::CreateFromPolar(float Radius, float Theta)")
	{
		float Radius = 5.0f;
		float Theta = ZE_PI_4;
		ZEVector2 Vector;
			
		Vector.CreateFromPolar(Radius, Theta);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Vector, ZEVector2(3.53553391f, 3.53553389f));
	}

	ZE_TEST_ITEM("void ZEVector2::ConvertToPolar(float& Radius, float& Theta)")
	{
		ZEVector2 Vector(1.0f, 1.0f);
		float Radius;
		float Theta;
		
		Vector.ConvertToPolar(Radius, Theta);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Radius, 1.41421356f);
		ZE_TEST_CHECK_EQUAL(Theta, ZE_PI_4);	
	}

	ZE_TEST_ITEM("inline void ZEVector2::Add(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(3.0f, 5.0f);
		ZEVector2 VectorB(1.0f, 6.0f);
		ZEVector2 Out;
			
		ZEVector2::Add(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(4.0f, 11.0f));		
	}

	ZE_TEST_ITEM("inline void ZEVector2::Substution(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(3.0f, 5.0f);
		ZEVector2 VectorB(1.0f, 6.0f);
		ZEVector2 Out;
			
		ZEVector2::Substution(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(2.0f, -1.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Scale(ZEVector2 &Out, const ZEVector2 &A, float s)")
	{
		float s = 0.5f;
		ZEVector2 VectorA(2.0f, 4.0f);
		ZEVector2 Out;

		ZEVector2::Scale(Out, VectorA, s);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(1.0f, 2.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Multiply(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(3.0f, 5.0f);
		ZEVector2 VectorB(1.0f, 6.0f);
		ZEVector2 Out;

		ZEVector2::Multiply(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(3.0f, 30.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Divide(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(3.0f, 4.0f);
		ZEVector2 VectorB(1.0f, 2.0f);
		ZEVector2 Out;

		ZEVector2::Divide(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(3.0f, 2.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Divide(ZEVector2 &Out, const ZEVector2 &A, float s)")
	{
		float s = 3.0f;
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 Out;

		ZEVector2::Divide(Out, VectorA, s);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEVector2(0.33333333f, 0.66666666f));
	}

	ZE_TEST_ITEM("float ZEVector2::DotProduct(const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 VectorB(2.0f, 3.0f);
		float Result;

		Result = ZEVector2::DotProduct(VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Result, 8.0f);
	}

	ZE_TEST_ITEM("float ZEVector2::Distance(const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 VectorB(4.0f, 5.0f);
			
		float Result = ZEVector2::Distance(VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 4.24264068f);

	}

	ZE_TEST_ITEM("float ZEVector2::DistanceSquare(const ZEVector2 &A, const ZEVector2 &B)")
	{
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 VectorB(4.0f, 5.0f);

		float Result = ZEVector2::DistanceSquare(VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Result, 18.000000f);	
	}

	ZE_TEST_ITEM("bool ZEVector2::Equals(const ZEVector2& Vector) const")
	{
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 VectorB(4.0f, 5.0f);

		bool Equal = VectorA.Equals(VectorB);
		ZE_TEST_CHECK_ERROR(!Equal);

		ZEVector2 VectorC(VectorA);

		Equal = VectorA.Equals(VectorC);
		ZE_TEST_CHECK_ERROR(Equal);

		ZEVector2 VectorD(1.00000001f, 2.00000001f);
		Equal = VectorA.Equals(VectorD);
		ZE_TEST_CHECK_ERROR(Equal);
	}

	ZE_TEST_ITEM("bool ZEVector2::Equals(const ZEVector2& Vector, float Threshold) const")
	{
		float Threshold = 0.1f;
		ZEVector2 VectorA(1.11f, 2.11f);
		ZEVector2 VectorB(1.1f, 2.1f);
		ZEVector2 VectorC(1.0f, 2.0f);

		bool Equal = VectorA.Equals(VectorB, Threshold);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = VectorA.Equals(VectorC, Threshold);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("float ZEVector2::Length(const ZEVector2 &A)")
	{
		ZEVector2 VectorA(1.0f, 2.0f);

		float Result = ZEVector2::Length(VectorA);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 2.23606797f);
	}

	ZE_TEST_ITEM("inline float ZEVector2::Length() const")
	{
		ZEVector2 Vector(3.0f, 4.0f);

		float Result = Vector.Length();
		ZE_TEST_CHECK_EQUAL(Result, 5.000000f);
	}

	ZE_TEST_ITEM("float ZEVector2::LengthSquare(const ZEVector2 &A)")
	{
		ZEVector2 VectorA(1.0f, 2.0f);

		float Result = ZEVector2::LengthSquare(VectorA);
		ZE_TEST_CHECK_EQUAL(Result, 5.000000f);
	}

	ZE_TEST_ITEM("float ZEVector2::LengthSquare() const")
	{
		ZEVector2 Vector(3.0f, 4.0f);

		float Result = Vector.LengthSquare();
		ZE_TEST_CHECK_EQUAL(Result, 25.000000f);
	}

	ZE_TEST_ITEM("bool ZEVector2::IsValid() const")
	{
		ZEVector2 Vector(3.0f, 4.0f);
		bool Result = Vector.IsValid();

		ZE_TEST_CHECK_EQUAL(Result, true);
	}

	ZE_TEST_ITEM("bool ZEVector2::IsNormalized() const")
	{
		ZEVector2 Vector(3.0f, 4.0f);
		bool Result = Vector.IsNormalized();

		ZE_TEST_CHECK_EQUAL(Result, false);
	
		
		ZEVector2 Vector2(0.6f, 0.8f);
		bool Result2 = Vector2.IsNormalized();

		ZE_TEST_CHECK_EQUAL(Result2, true);
	}

	ZE_TEST_ITEM("void ZEVector2::Normalize(ZEVector2 &Out, const ZEVector2 &A)")
	{
		ZEVector2 VectorA(3.0f, 4.0f);
		ZEVector2 Out;
			
		ZEVector2::Normalize(Out, VectorA);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(0.6f, 0.8f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::Normalize() const")
	{
		ZEVector2 Vector(2.0f, 2.0f);
		ZEVector2 Result;

		Result = Vector.Normalize();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEVector2(0.70710678f, 0.70710678f));
	}

	ZE_TEST_ITEM("void ZEVector2::NormalizeSelf()")
	{
		ZEVector2 Vector(2.0f, 2.0f);
			
		Vector.NormalizeSelf();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Vector, ZEVector2(0.70710678f, 0.70710678f));
	}

	ZE_TEST_ITEM("void ZEVector2::Max(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)")
	{
		ZEVector2 VectorA(5.0f, 2.0f);
		ZEVector2 VectorB(3.0f, 7.0f);
		ZEVector2 Out;

		ZEVector2::Max(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(5.0f, 7.0f));
	}

	ZE_TEST_ITEM("float ZEVector2::Max() const")
	{
		ZEVector2 Vector(5.0f, 2.0f);
			
		float Result = Vector.Max();
		ZE_TEST_CHECK_EQUAL(Result, 5.0f);
	}

	ZE_TEST_ITEM("void ZEVector2::Min(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)")
	{
		ZEVector2 VectorA(5.0f, 2.0f);
		ZEVector2 VectorB(3.0f, 7.0f);
		ZEVector2 Out;

		ZEVector2::Min(Out, VectorA, VectorB);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(3.0f, 2.0f));
	}

	ZE_TEST_ITEM("float ZEVector2::Min() const")
	{
		ZEVector2 Vector(5.0f, 2.0f);

		float Result = Vector.Min();
		ZE_TEST_CHECK_EQUAL(Result, 2.0f);
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::Clamp(float MinValue, float MaxValue) const")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		float MinValue = 2.0f;
		float MaxValue = 5.0f;

		ZEVector2 Result = Vector.Clamp(MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(5.0f, 2.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::Clamp(const ZEVector2& MinValue, const ZEVector2& MaxValue) const")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		ZEVector2 MinValue(1.0f, 2.0f);
		ZEVector2 MaxValue(3.0f, 5.0f);
			
		ZEVector2 Result = Vector.Clamp(MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(3.0f, 2.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, float MinValue, float MaxValue)")
	{
		ZEVector2 Out;
		ZEVector2 Vector(3.0f, 5.0f);
		float MinValue = 3.0f;
		float MaxValue = 4.0f;

		ZEVector2::Clamp(Out, Vector, MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(3.0f, 4.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, const ZEVector2& MinValue, const ZEVector2& MaxValue)")
	{
		ZEVector2 Out;
		ZEVector2 Vector(1.0f, 5.0f);
		ZEVector2 MinValue(2.0f, 3.0f);
		ZEVector2 MaxValue(4.0f, 5.0f);

		ZEVector2::Clamp(Out, Vector, MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(2.0f, 5.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::ClampSelf(float MinValue, float MaxValue)")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		float MinValue = 2.0f;
		float MaxValue = 5.0f;

		Vector.ClampSelf(MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(5.0f, 2.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::ClampSelf(const ZEVector2& MinValue, const ZEVector2& MaxValue)")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		ZEVector2 MinValue(1.0f, 2.0f);
		ZEVector2 MaxValue(3.0f, 5.0f);

		Vector.ClampSelf(MinValue, MaxValue);
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(3.0f, 2.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::Lerp(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B, float Factor)")
	{
		ZEVector2 Out;
		ZEVector2 VectorA(1.0f, 2.0f);
		ZEVector2 VectorB(6.0f, 9.0f);
		float Factor = 0.65f;

		ZEVector2::Lerp(Out, VectorA, VectorB, Factor);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEVector2(4.25f, 6.55f));
	
		ZE_TEST_CASE("Lerp Two Vectors With Factor = 0 then Pass Result To Output")
		{
			Factor = 0.0f;

			ZEVector2::Lerp(Out, VectorA, VectorB, Factor);
			ZE_TEST_CHECK_EQUAL(Out, ZEVector2(1.0f, 2.0f));
		}

		ZE_TEST_CASE("Lerp Two Vectors With Factor = 1 then Pass Result To Output")
		{
			Factor = 1.0f;

			ZEVector2::Lerp(Out, VectorA, VectorB, Factor);
			ZE_TEST_CHECK_EQUAL(Out, ZEVector2(6.0f, 9.0f));
		}
	}

	ZE_TEST_ITEM("void ZEVector2::Saturate(ZEVector2& Out, const ZEVector2& Vector)")
	{
		ZEVector2 Out;
		ZEVector2 Vector(2.0f, -4.0f);
			
		ZEVector2::Saturate(Out, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(1.0f, 0.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::Saturate() const")
	{
		ZEVector2 Result;
		ZEVector2 Vector(2.0f, -4.0f);

		Result = Vector.Saturate();
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(1.0f, 0.0f));
	}

	ZE_TEST_ITEM("void ZEVector2::SaturateSelf()")
	{
		ZEVector2 Vector(2.0f, -4.0f);

		Vector.SaturateSelf();
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(1.0f, 0.0f));

	
		ZE_TEST_CASE("Saturate Zero Vector")
		{
			ZEVector2 Vector(0.0f, 0.0f);

			Vector.SaturateSelf();
			ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(0.0f, 0.0f));
		}
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator -(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(10.0f, 5.0f);
		ZEVector2 RightOperand(3.0f, 2.0f);
			
		ZEVector2 Result = Vector - RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(7.0f, 3.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator-() const")
	{
		ZEVector2 Vector(10.0f, 5.0f);
			
		ZEVector2 Result = -Vector;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(-10.0f, -5.0f));

	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator *(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(10.0f, 5.0f);
		ZEVector2 RightOperand(3.0f, 2.0f);

		ZEVector2 Result = Vector * RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(30.0f, 10.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator *(float s) const")
	{
		ZEVector2 Vector(2.5f, 3.0f);
		float s = 3.0f;

		ZEVector2 Result = Vector * s;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(7.5f, 9.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator *=(const ZEVector2 &RightOperand)")
	{
		ZEVector2 Vector(10.0f, 5.0f);
		ZEVector2 RightOperand(3.0f, 2.0f);

		Vector *= RightOperand;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(30.0f, 10.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator *=(float s)")
	{
		ZEVector2 Vector(2.5f, 3.0f);
		float s = 3.0f;

		Vector *= s;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(7.5f, 9.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator /(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(25.0f, 16.0f);
		ZEVector2 RightOperand(5.0f, 2.0f);

		ZEVector2 Result = Vector / RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(5.0f, 8.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator /(float s) const")
	{
		ZEVector2 Vector(9.0f, 15.0f);
		float s = 3.0f;

		ZEVector2 Result = Vector / s;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(3.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator /=(const ZEVector2 &RightOperand)")
	{
		ZEVector2 Vector(25.0f, 16.0f);
		ZEVector2 RightOperand(5.0f, 2.0f);

		Vector /= RightOperand;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(5.0f, 8.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator /=(float s)")
	{
		ZEVector2 Vector(9.0f, 15.0f);
		float s = 3.0f;

		Vector /= s;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(3.0f, 5.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEVector2::operator +(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		ZEVector2 RightOperand(3.0f, 5.0f);
			
		ZEVector2 Result = Vector + RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(13.0f, 6.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator +=(const ZEVector2 &RightOperand)")
	{
		ZEVector2 Vector(10.0f, 1.0f);
		ZEVector2 RightOperand(-3.0f, -5.0f);
			
		Vector += RightOperand;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(7.0f, -4.0f));
	}

	ZE_TEST_ITEM("ZEVector2& ZEVector2::operator -=(const ZEVector2 &RightOperand)")
	{
		ZEVector2 Vector(10.0f, 6.0f);
		ZEVector2 RightOperand(3.0f, 8.0f);
			
		Vector -= RightOperand;
		ZE_TEST_CHECK_EQUAL(Vector, ZEVector2(7.0f, -2.0f));
	}

	ZE_TEST_ITEM("bool ZEVector2::operator ==(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(3.0f, 2.0f);
		ZEVector2 RightOperand(3.0f, 2.0f);
			
		bool Result = (Vector == RightOperand);
		ZE_TEST_CHECK_EQUAL(Result, true);
	
		ZE_TEST_CASE("== WithVectorArg for Comparison for Different Vectors")
		{
			ZEVector2 Vector(3.0f, 8.0f);
			
			Result= (Vector == RightOperand);
			ZE_TEST_CHECK_EQUAL(Result, false);
		}
	}

	ZE_TEST_ITEM("bool ZEVector2::operator !=(const ZEVector2 &RightOperand) const")
	{
		ZEVector2 Vector(3.0f, 2.0f);
		ZEVector2 RightOperand(3.0f, 2.0f);

		bool Result = (Vector != RightOperand);
		ZE_TEST_CHECK_EQUAL(Result, false);
	
		ZE_TEST_CASE("!= WithVectorArg for Comparison for Different Vectors")
		{
			ZEVector2 Vector(3.0f, 8.0f);
			
			Result = (Vector != RightOperand);
			ZE_TEST_CHECK_EQUAL(Result, true);
		}
	}

	ZE_TEST_ITEM("float& ZEVector2::operator[](ZESize Index)")
	{
		ZEVector2 Vector(2.0f, 4.0f);

		float x = Vector[0];
		float y = Vector[1];
		ZE_TEST_CHECK_EQUAL(x, 2.0f);
		ZE_TEST_CHECK_EQUAL(y, 4.0f);
	}

	ZE_TEST_ITEM("float ZEVector2::operator[](ZESize Index) const")
	{
		ZEVector2 Vector(2.0f, 4.0f);

		float x = Vector[0];
		float y = Vector[1];
		ZE_TEST_CHECK_EQUAL(x, 2.0f);
		ZE_TEST_CHECK_EQUAL(y, 4.0f);
	}	
}
