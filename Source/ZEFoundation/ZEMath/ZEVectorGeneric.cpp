//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectorGeneric.cpp
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

#include "ZEVector.h"

// ZEVector2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector2::Divide(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
}

float ZEVector2::DotProduct(const ZEVector2 &A, const ZEVector2 &B)
{
	return A.x * B.x + A.y * B.y ;
}

float ZEVector2::DistanceSquare(const ZEVector2 &A, const ZEVector2 &B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

void ZEVector2::Normalize(ZEVector2 &Out, const ZEVector2 &A)
{
	float L = A.Length();
	Out.x = A.x / L;
	Out.y = A.y / L;
}

void ZEVector2::Max(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
}

void ZEVector2::Min(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
}

void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, float MinValue, float MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;
}

void ZEVector2::Clamp(ZEVector2& Out, const ZEVector2& Vector, const ZEVector2& MinValue, const ZEVector2& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;
}

void ZEVector2::Saturate(ZEVector2& Out, const ZEVector2& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;
}

// ZEVector2d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector2d::Divide(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
}

double ZEVector2d::DotProduct(const ZEVector2d& A, const ZEVector2d& B)
{
	return A.x * B.x + A.y * B.y ;
}

double ZEVector2d::DistanceSquare(const ZEVector2d& A, const ZEVector2d& B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

void ZEVector2d::Normalize(ZEVector2d &Out, const ZEVector2d &A)
{
	double L = A.Length();
	Out.x = A.x / L;
	Out.y = A.y / L;
}

void ZEVector2d::Max(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
}

void ZEVector2d::Min(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
}

void ZEVector2d::Clamp(ZEVector2d& Out, const ZEVector2d& Vector, double MinValue, double MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;
}

void ZEVector2d::Clamp(ZEVector2d& Out, const ZEVector2d& Vector, const ZEVector2d& MinValue, const ZEVector2d& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;
}

void ZEVector2d::Saturate(ZEVector2d& Out, const ZEVector2d& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;
}

// ZEVector3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector3::Divide(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
	Out.z = A.z / B.z;
}

float ZEVector3::DotProduct(const ZEVector3& A, const ZEVector3& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

float ZEVector3::DistanceSquare(const ZEVector3& A, const ZEVector3& B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z);
}

void ZEVector3::Normalize(ZEVector3& Out, const ZEVector3& Vector)
{
	float L = Vector.Length();
	Out.x = Vector.x / L;
	Out.y = Vector.y / L;
	Out.z = Vector.z / L;
}

void ZEVector3::Max(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
	Out.z = A.z > B.z ? A.z : B.z;
}

void ZEVector3::Min(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
	Out.z = A.z < B.z ? A.z : B.z;
}

void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, float MinValue, float MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;
}

void ZEVector3::Clamp(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue, const ZEVector3& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3::ClampLower(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue)
{
	if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;


	if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3::ClampLower(ZEVector3& Out, const ZEVector3& Vector, float MinValue)
{
	if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;


	if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;
}

void ZEVector3::ClampUpper(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3::ClampUpper(ZEVector3& Out, const ZEVector3& Vector, float MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else
		Out.z = Vector.z;
}

void ZEVector3::Saturate(ZEVector3& Out, const ZEVector3& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;

	if (Vector.z > 1.0f)
		Out.z = 1.0f;
	else if (Vector.z < 0.0f)
		Out.z = 0.0f;
	else
		Out.z = Vector.z;
}

// ZEVector3d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector3d::Divide(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
	Out.z = A.z / B.z;
}

double ZEVector3d::DotProduct(const ZEVector3d& A, const ZEVector3d& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

double ZEVector3d::DistanceSquare(const ZEVector3d& A, const ZEVector3d& B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z);
}

void ZEVector3d::Normalize(ZEVector3d& Out, const ZEVector3d& Vector)
{
	double L = Vector.Length();
	Out.x = Vector.x / L;
	Out.y = Vector.y / L;
	Out.z = Vector.z / L;
}

void ZEVector3d::Max(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
	Out.z = A.z > B.z ? A.z : B.z;
}

void ZEVector3d::Min(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
	Out.z = A.z < B.z ? A.z : B.z;
}

void ZEVector3d::Clamp(ZEVector3d& Out, const ZEVector3d& Vector, double MinValue, double MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;
}

void ZEVector3d::Clamp(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MinValue, const ZEVector3d& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3d::ClampLower(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MinValue)
{
	if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;


	if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3d::ClampLower(ZEVector3d& Out, const ZEVector3d& Vector, double MinValue)
{
	if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;


	if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;
}

void ZEVector3d::ClampUpper(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else
		Out.z = Vector.z;
}

void ZEVector3d::ClampUpper(ZEVector3d& Out, const ZEVector3d& Vector, double MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else
		Out.z = Vector.z;
}

void ZEVector3d::Saturate(ZEVector3d& Out, const ZEVector3d& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;

	if (Vector.z > 1.0f)
		Out.z = 1.0f;
	else if (Vector.z < 0.0f)
		Out.z = 0.0f;
	else
		Out.z = Vector.z;
}


// ZEVector4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector4::Add(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
	Out.z = A.z + B.z;
	Out.w = A.w + B.w;
}

void ZEVector4::Sub(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
	Out.z = A.z - B.z;
	Out.w = A.w - B.w;
}

void ZEVector4::Scale(ZEVector4& Out, const ZEVector4& A, float s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
	Out.z = A.z * s;
	Out.w = A.w * s;
}

void ZEVector4::Multiply(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
	Out.z = A.z * B.z;
	Out.w = A.w * B.w;
}

void ZEVector4::Divide(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
	Out.z = A.z / B.z;
	Out.w = A.w / B.w;
}

void ZEVector4::Divide(ZEVector4& Out, const ZEVector4& A, float s)
{
	Out.x = A.x / s;
	Out.y = A.y / s;
	Out.z = A.z / s;
	Out.w = A.w / s;
}

float ZEVector4::DotProduct(const ZEVector4& A, const ZEVector4& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
}

float ZEVector4::DistanceSquare(const ZEVector4& A, const ZEVector4& B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z) + (A.w - B.w) * (A.w - B.w);
}

void ZEVector4::Normalize(ZEVector4& Out, const ZEVector4& Vector)
{
	float L = Vector.Length();
	Out.x = Vector.x / L;
	Out.y = Vector.y / L;
	Out.z = Vector.z / L;
	Out.w = Vector.w / L;
}

void ZEVector4::Lerp(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B, float Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
	Out.z = A.z + (B.z - A.z) * Factor;
	Out.w = A.w + (B.w - A.w) * Factor;
}

void ZEVector4::Max(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
	Out.z = A.z > B.z ? A.z : B.z;
	Out.w = A.w > B.w ? A.w : B.w;
}

void ZEVector4::Min(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
	Out.z = A.z < B.z ? A.z : B.z;
	Out.w = A.w < B.w ? A.w : B.w;
}

void ZEVector4::Clamp(ZEVector4& Out, const ZEVector4& Vector, float MinValue, float MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;

	if (Vector.w > MaxValue)
		Out.w = MaxValue;
	else if (Vector.w < MinValue)
		Out.w = MinValue;
	else
		Out.w = Vector.w;
}

void ZEVector4::Clamp(ZEVector4& Out, const ZEVector4& Vector, const ZEVector4& MinValue, const ZEVector4& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;

	if (Vector.w > MaxValue.w)
		Out.w = MaxValue.w;
	else if (Vector.w < MinValue.w)
		Out.w = MinValue.w;
	else
		Out.w = Vector.w;
}

void ZEVector4::Saturate(ZEVector4& Out, const ZEVector4& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;

	if (Vector.z > 1.0f)
		Out.z = 1.0f;
	else if (Vector.z < 0.0f)
		Out.z = 0.0f;
	else
		Out.z = Vector.z;

	if (Vector.w > 1.0f)
		Out.w = 1.0f;
	else if (Vector.w < 0.0f)
		Out.w = 0.0f;
	else
		Out.w = Vector.w;
}

// ZEVector4d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEVector4d::Add(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
	Out.z = A.z + B.z;
	Out.w = A.w + B.w;
}

void ZEVector4d::Sub(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
	Out.z = A.z - B.z;
	Out.w = A.w - B.w;
}

void ZEVector4d::Scale(ZEVector4d& Out, const ZEVector4d& A, double s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
	Out.z = A.z * s;
	Out.w = A.w * s;
}

void ZEVector4d::Multiply(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
	Out.z = A.z * B.z;
	Out.w = A.w * B.w;
}

void ZEVector4d::Divide(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x / B.x;
	Out.y = A.y / B.y;
	Out.z = A.z / B.z;
	Out.w = A.w / B.w;
}

void ZEVector4d::Divide(ZEVector4d& Out, const ZEVector4d& A, double s)
{
	Out.x = A.x / s;
	Out.y = A.y / s;
	Out.z = A.z / s;
	Out.w = A.w / s;
}

double ZEVector4d::DotProduct(const ZEVector4d& A, const ZEVector4d& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
}

double ZEVector4d::DistanceSquare(const ZEVector4d& A, const ZEVector4d& B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z) + (A.w - B.w) * (A.w - B.w);
}

void ZEVector4d::Normalize(ZEVector4d& Out, const ZEVector4d& Vector)
{
	double L = Vector.Length();
	Out.x = Vector.x / L;
	Out.y = Vector.y / L;
	Out.z = Vector.z / L;
	Out.w = Vector.w / L;
}

void ZEVector4d::Lerp(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B, double Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
	Out.z = A.z + (B.z - A.z) * Factor;
	Out.w = A.w + (B.w - A.w) * Factor;
}

void ZEVector4d::Max(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x > B.x ? A.x : B.x;
	Out.y = A.y > B.y ? A.y : B.y;
	Out.z = A.z > B.z ? A.z : B.z;
	Out.w = A.w > B.w ? A.w : B.w;
}

void ZEVector4d::Min(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B)
{
	Out.x = A.x < B.x ? A.x : B.x;
	Out.y = A.y < B.y ? A.y : B.y;
	Out.z = A.z < B.z ? A.z : B.z;
	Out.w = A.w < B.w ? A.w : B.w;
}

void ZEVector4d::Clamp(ZEVector4d& Out, const ZEVector4d& Vector, double MinValue, double MaxValue)
{
	if (Vector.x > MaxValue)
		Out.x = MaxValue;
	else if (Vector.x < MinValue)
		Out.x = MinValue;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue)
		Out.y = MaxValue;
	else if (Vector.y < MinValue)
		Out.y = MinValue;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue)
		Out.z = MaxValue;
	else if (Vector.z < MinValue)
		Out.z = MinValue;
	else
		Out.z = Vector.z;

	if (Vector.w > MaxValue)
		Out.w = MaxValue;
	else if (Vector.w < MinValue)
		Out.w = MinValue;
	else
		Out.w = Vector.w;
}

void ZEVector4d::Clamp(ZEVector4d& Out, const ZEVector4d& Vector, const ZEVector4d& MinValue, const ZEVector4d& MaxValue)
{
	if (Vector.x > MaxValue.x)
		Out.x = MaxValue.x;
	else if (Vector.x < MinValue.x)
		Out.x = MinValue.x;
	else
		Out.x = Vector.x;

	if (Vector.y > MaxValue.y)
		Out.y = MaxValue.y;
	else if (Vector.y < MinValue.y)
		Out.y = MinValue.y;
	else
		Out.y = Vector.y;

	if (Vector.z > MaxValue.z)
		Out.z = MaxValue.z;
	else if (Vector.z < MinValue.z)
		Out.z = MinValue.z;
	else
		Out.z = Vector.z;

	if (Vector.w > MaxValue.w)
		Out.w = MaxValue.w;
	else if (Vector.w < MinValue.w)
		Out.w = MinValue.w;
	else
		Out.w = Vector.w;
}

void ZEVector4d::Saturate(ZEVector4d& Out, const ZEVector4d& Vector)
{
	if (Vector.x > 1.0f)
		Out.x = 1.0f;
	else if (Vector.x < 0.0f)
		Out.x = 0.0f;
	else
		Out.x = Vector.x;

	if (Vector.y > 1.0f)
		Out.y = 1.0f;
	else if (Vector.y < 0.0f)
		Out.y = 0.0f;
	else
		Out.y = Vector.y;

	if (Vector.z > 1.0f)
		Out.z = 1.0f;
	else if (Vector.z < 0.0f)
		Out.z = 0.0f;
	else
		Out.z = Vector.z;

	if (Vector.w > 1.0f)
		Out.w = 1.0f;
	else if (Vector.w < 0.0f)
		Out.w = 0.0f;
	else
		Out.w = Vector.w;
}
