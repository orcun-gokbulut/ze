//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectorSSE4.cpp
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

#include <smmintrin.h>
#include "ZEVector.h"

// MMX Register Loads
inline __m128 LoadVector2(const ZEVector2 &A)
{
	return _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)A.M);
}

inline __m128 LoadVector3(const ZEVector3 &A)
{
	// High 8bytes and Low 8bytes
	__m128 mmxL, mmxH;

	mmxL = _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)A.M);
	mmxH = _mm_load_ss(&A.z);
	return _mm_movelh_ps(mmxL, mmxH);
}

inline __m128 LoadVector4(const ZEVector4 &A)
{
	return _mm_loadu_ps(A.M);
}

// MMX Register Stores
inline void StoreVector2(ZEVector2 &Out, const __m128 &Reg)
{
	_mm_storel_pi((__m64*) &Out, Reg);
}

inline void StoreVector3(ZEVector3 &Out, __m128 &Reg)
{
	float *f = Out.M;
	_mm_storel_pi((__m64*)f, Reg);
	Reg = _mm_shuffle_ps(Reg, Reg, 0xAA);
	f += 2;
	_mm_store_ss(f, Reg);
}

inline void StoreVector4(ZEVector4 &Out, const __m128 &Reg)
{
	_mm_storeu_ps(Out.M, Reg);
}

// ZEVector2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector2::Divide(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	__m128 mmxA = LoadVector2(A);
	__m128 mmxB = LoadVector2(B);

	// Math 
	mmxA = _mm_div_ps(mmxA, mmxB);
	StoreVector2(Out, mmxA);
}

float ZEVector2::DotProduct(const ZEVector2 &A, const ZEVector2 &B)
{
	__m128 mmxA = LoadVector2(A);
	__m128 mmxB = LoadVector2(B);
	
	mmxA = _mm_dp_ps(mmxA, mmxB, 0x31);
	return mmxA.m128_f32[0];
}

float ZEVector2::DistanceSquare(const ZEVector2 &A, const ZEVector2 &B)
{
	__m128 mmxA = LoadVector2(A);
	__m128 mmxB = LoadVector2(B);
	
	mmxA = _mm_sub_ps(mmxA, mmxB);
	mmxA = _mm_dp_ps(mmxA, mmxA, 0x31);
	return mmxA.m128_f32[0];
}

void ZEVector2::Normalize(ZEVector2 &Out, const ZEVector2 &A)
{	
	__m128 mmxA = LoadVector2(A);
	__m128 mmxLength = _mm_dp_ps(mmxA, mmxA, 0x33);
	
	mmxLength = _mm_sqrt_ps(mmxLength);
	mmxA = _mm_div_ps(mmxA, mmxLength);
	StoreVector2(Out, mmxA);
}

void ZEVector2::Max(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)
{
	__m128 mmxA = LoadVector2(A);
	__m128 mmxB = LoadVector2(B);

	mmxA = _mm_max_ps(mmxA, mmxB);
	StoreVector2(Out, mmxA);
}

void ZEVector2::Min(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)
{
	__m128 mmxA = LoadVector2(A);
	__m128 mmxB = LoadVector2(B);

	mmxA = _mm_min_ps(mmxA, mmxB);
	StoreVector2(Out, mmxA);
}

void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, float MinValue, float MaxValue)
{
	__m128 mmxA = LoadVector2(Vector);
	__m128 mmxMin = _mm_load1_ps(&MinValue);
	__m128 mmxMax = _mm_load1_ps(&MaxValue);
	
	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector2(Out, mmxA);
}

void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, const ZEVector2& MinValue, const ZEVector2& MaxValue)
{
	__m128 mmxA = LoadVector2(Vector);
	__m128 mmxMin = LoadVector2(MinValue);
	__m128 mmxMax = LoadVector2(MaxValue);
	
	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector2(Out, mmxA);
}

void ZEVector2::Saturate(ZEVector2& Out, const ZEVector2& Vector)
{
	__m128 mmxA = LoadVector2(Vector);
	__m128 mmxMin = _mm_setzero_ps();	// 0.0f
	__m128 mmxMax =	_mm_set1_ps(1.0f);	// 1.0f

	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector2(Out, mmxA);
}

// ZEVector3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector3::Divide(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	__m128 mmxA = LoadVector3(A);
	__m128 mmxB = LoadVector3(B);
	
	mmxA = _mm_div_ps(mmxA, mmxB);
	StoreVector3(Out, mmxA);
}

float ZEVector3::DotProduct(const ZEVector3& A, const ZEVector3& B) 
{
	__m128 mmxA = LoadVector3(A);
	__m128 mmxB = LoadVector3(B);

	mmxA = _mm_dp_ps(mmxA, mmxB, 0x71);
	return mmxA.m128_f32[0];
}

//void ZEVector3::CrossProduct(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
//{
//	__m128 mmxA = LoadVector3(A);
//	__m128 mmxB = LoadVector3(B);
//
//	__m128 mmxAShift = _mm_shuffle_ps(mmxA, mmxA, 0xC9);
//	__m128 mmxBShift = _mm_shuffle_ps(mmxB, mmxB, 0xC9);
//
//	__m128 mmxLow = _mm_mul_ps(mmxAShift, mmxB);
//	__m128 mmxHigh = _mm_mul_ps(mmxBShift, mmxA);
//	mmxHigh = _mm_sub_ps(mmxHigh, mmxLow);
//	mmxHigh = _mm_shuffle_ps(mmxHigh, mmxHigh, 0xC9);
//	StoreVector3(Out, mmxHigh);
//}

float ZEVector3::DistanceSquare(const ZEVector3& A, const ZEVector3& B)
{
	__m128 mmxA = LoadVector3(A);
	__m128 mmxB = LoadVector3(B);
	
	mmxA = _mm_sub_ps(mmxA, mmxB);
	mmxA = _mm_dp_ps(mmxA, mmxA, 0x71);
	return mmxA.m128_f32[0];
}

void ZEVector3::Normalize(ZEVector3& Out, const ZEVector3& Vector)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxLength = _mm_dp_ps(mmxA, mmxA, 0x77);
	
	mmxLength = _mm_sqrt_ps(mmxLength);
	mmxA = _mm_div_ps(mmxA, mmxLength);
	StoreVector3(Out, mmxA);
}

void ZEVector3::Max(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	__m128 mmxA = LoadVector3(A);
	__m128 mmxB = LoadVector3(B);

	mmxA = _mm_max_ps(mmxA, mmxB);
	StoreVector3(Out, mmxA);
}

void ZEVector3::Min(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	__m128 mmxA = LoadVector3(A);
	__m128 mmxB = LoadVector3(B);

	mmxA = _mm_min_ps(mmxA, mmxB);
	StoreVector3(Out, mmxA);
}

void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, float MinValue, float MaxValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMin = _mm_load1_ps(&MinValue);
	__m128 mmxMax = _mm_load1_ps(&MaxValue);

	mmxA = _mm_min_ps(mmxA, mmxMax);
	mmxA = _mm_max_ps(mmxA, mmxMin);
	StoreVector3(Out, mmxA);
}

void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue, const ZEVector3& MaxValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMin = LoadVector3(MinValue);
	__m128 mmxMax = LoadVector3(MaxValue);

	mmxA = _mm_min_ps(mmxA, mmxMax);
	mmxA = _mm_max_ps(mmxA, mmxMin);
	StoreVector3(Out, mmxA);
}

void ZEVector3::ClampLower(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMin = LoadVector3(MinValue);

	mmxA = _mm_max_ps(mmxA, mmxMin);
	StoreVector3(Out, mmxA);
}

void ZEVector3::ClampLower(ZEVector3& Out, const ZEVector3& Vector, float MinValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMin = _mm_load1_ps(&MinValue);

	mmxA = _mm_max_ps(mmxA, mmxMin);
	StoreVector3(Out, mmxA);
}

void ZEVector3::ClampUpper(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MaxValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMax = LoadVector3(MaxValue);

	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector3(Out, mmxA);
}

void ZEVector3::ClampUpper(ZEVector3& Out, const ZEVector3& Vector, float MaxValue)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMax = _mm_load1_ps(&MaxValue);

	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector3(Out, mmxA);
}

void ZEVector3::Saturate(ZEVector3& Out, const ZEVector3& Vector)
{
	__m128 mmxA = LoadVector3(Vector);
	__m128 mmxMin = _mm_setzero_ps();
	__m128 mmxMax = _mm_set1_ps(1.0f);

	mmxA = _mm_min_ps(mmxA, mmxMax);
	mmxA = _mm_max_ps(mmxA, mmxMin);
	StoreVector3(Out, mmxA);
}

// ZEVector4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector4::Add(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_add_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Sub(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_sub_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Scale(ZEVector4& Out, const ZEVector4& A, float s)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = _mm_load1_ps(&s);
	mmxA = _mm_mul_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Multiply(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_mul_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Divide(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_div_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Divide(ZEVector4& Out, const ZEVector4& A, float s)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = _mm_load1_ps(&s);
	mmxA = _mm_div_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

float ZEVector4::DotProduct(const ZEVector4& A, const ZEVector4& B) 
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_dp_ps(mmxA, mmxB, 0xF1);
	return mmxA.m128_f32[0];
}

float ZEVector4::DistanceSquare(const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	mmxA = _mm_sub_ps(mmxA, mmxB);
	mmxA = _mm_dp_ps(mmxA, mmxA, 0xF1);
	return mmxA.m128_f32[0];
}

void ZEVector4::Normalize(ZEVector4& Out, const ZEVector4& Vector)
{
	__m128 mmxA = LoadVector4(Vector);
	__m128 mmxLength = _mm_dp_ps(mmxA, mmxA, 0xFF);
	
	mmxLength = _mm_sqrt_ps(mmxLength);
	mmxA = _mm_div_ps(mmxA, mmxLength);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Lerp(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B, float Factor)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	__m128 mmxFactor = _mm_load1_ps(&Factor);

	mmxB = _mm_sub_ps(mmxB, mmxA);
	mmxB = _mm_mul_ps(mmxB, mmxFactor);
	mmxA = _mm_add_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Max(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);

	mmxA = _mm_max_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Min(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	__m128 mmxA = LoadVector4(A);
	__m128 mmxB = LoadVector4(B);
	
	mmxA = _mm_min_ps(mmxA, mmxB);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Clamp(ZEVector4& Out, const ZEVector4& Vector, float MinValue, float MaxValue)
{
	__m128 mmxA = LoadVector4(Vector);
	__m128 mmxMin = _mm_load1_ps(&MinValue);
	__m128 mmxMax = _mm_load1_ps(&MaxValue);

	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Clamp(ZEVector4& Out, const ZEVector4& Vector, const ZEVector4& MinValue, const ZEVector4& MaxValue)
{
	__m128 mmxA = LoadVector4(Vector);
	__m128 mmxMin = LoadVector4(MinValue);
	__m128 mmxMax = LoadVector4(MaxValue);

	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector4(Out, mmxA);
}

void ZEVector4::Saturate(ZEVector4& Out, const ZEVector4& Vector)
{
	__m128 mmxA = LoadVector4(Vector);
	__m128 mmxMin = _mm_setzero_ps();
	__m128 mmxMax = _mm_set1_ps(1.0f);

	mmxA = _mm_max_ps(mmxA, mmxMin);
	mmxA = _mm_min_ps(mmxA, mmxMax);
	StoreVector4(Out, mmxA);
}
