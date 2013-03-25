//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVector.cpp
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
#include "ZEMatrix.h"
#include "ZEMath.h"
#include "ZEMath.h"
#include "ZEAngle.h"

#define		ZE_VECTOR4_ELEMENT_COUNT	(ZESize)4

//ZE_OBJECT_IMPL(ZEVector2)

// ZEVector2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEVector2 ZEVector2::Zero = ZEVector2(0.0f, 0.0f);
const ZEVector2 ZEVector2::One = ZEVector2(1.0f, 1.0f);
const ZEVector2 ZEVector2::UnitX = ZEVector2(1.0f, 0.0f);
const ZEVector2 ZEVector2::UnitY = ZEVector2(0.0f, 1.0f);
const ZEVector2 ZEVector2::MinValue	= ZEVector2(ZE_FLOAT_MIN, ZE_FLOAT_MIN);
const ZEVector2 ZEVector2::MaxValue	= ZEVector2(ZE_FLOAT_MAX, ZE_FLOAT_MAX);

inline void ZEVector2::Create(float x, float y)
{
	this->x = x;
	this->y = y;
}

inline void ZEVector2::Create(const ZEVector2 &Start, const ZEVector2 &End)
{
	this->x = End.x - Start.x;
	this->y = End.y - Start.y;
}

void ZEVector2::CreateFromPolar(float Radius, float Theta)
{
	x = Radius * ZEAngle::Cos(Theta);
	y = Radius * ZEAngle::Sin(Theta);
}

void ZEVector2::ConvertToPolar(float& Radius, float& Theta)
{
	Radius = ZEMath::Sqrt(x * x + y * y);
	Theta = ZEAngle::ArcTan2(y, x);
}

void ZEVector2::Add(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
}

void ZEVector2::Substution(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
}

void ZEVector2::Scale(ZEVector2 &Out, const ZEVector2 &A, float s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
}

void ZEVector2::Multiply(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
}

void ZEVector2::Divide(ZEVector2 &Out, const ZEVector2 &A, float s)
{
	Out.x = A.x / s;
	Out.y = A.y / s;
}

bool ZEVector2::IsValid() const
{
	if (x != x || y != y)
		return false;
	else
		return true;
}

bool ZEVector2::IsNormalized() const
{
	return ZEMath::Abs(Length() - 1.0f) < 0.001;
}

float ZEVector2::Length(const ZEVector2 &A)
{
	return ZEMath::Sqrt(ZEVector2::DotProduct(A, A));
}

float ZEVector2::LengthSquare(const ZEVector2 &A)
{
	return ZEVector2::DotProduct(A, A);
}

float ZEVector2::Distance(const ZEVector2 &A, const ZEVector2 &B)
{
	return ZEMath::Sqrt(ZEVector2::DistanceSquare(A, B));
}

void ZEVector2::Lerp(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B, float Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
}

ZEVector2 ZEVector2::Normalize() const
{
	ZEVector2 Temp;
	ZEVector2::Normalize(Temp, *this);
	return Temp;	
}

void ZEVector2::NormalizeSelf()
{
	ZEVector2::Normalize(*this, *this);
}

inline float ZEVector2::Length() const
{
	return ZEMath::Sqrt(ZEVector2::DotProduct(*this, *this));
}

float ZEVector2::LengthSquare() const
{
	return (ZEVector2::DotProduct(*this, *this));
}


float ZEVector2::SignedAngle(const ZEVector2& A, const ZEVector2& B)
{
	float PerpDot = A.x * B.y - A.y * B.x;
	return (float)ZEAngle::ArcTan2(PerpDot, ZEVector2::DotProduct(A, B));
}

ZEVector2 ZEVector2::Clamp(float MinValue, float MaxValue) const
{
	ZEVector2 Temp;
	ZEVector2::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

ZEVector2 ZEVector2::Clamp(const ZEVector2& MinValue, const ZEVector2& MaxValue) const
{
	ZEVector2 Temp;
	ZEVector2::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

void ZEVector2::ClampSelf(float MinValue, float MaxValue) 
{
	ZEVector2::Clamp(*this, *this, MinValue, MaxValue);
}

void ZEVector2::ClampSelf(const ZEVector2& MinValue, const ZEVector2& MaxValue) 
{
	ZEVector2::Clamp(*this, *this, MinValue, MaxValue);
}

ZEVector2 ZEVector2::Saturate() const
{
	ZEVector2 Temp;
	ZEVector2::Saturate(Temp, *this);
	return Temp;
}
	
void ZEVector2::SaturateSelf()
{
	ZEVector2::Saturate(*this, *this);
}

float ZEVector2::Min() const
{
	if (x < y)
		return x;
	else
		return y;
}

float ZEVector2::Max() const
{
	if (x > y)
		return x;
	else
		return y;
}

bool ZEVector2::Equals(const ZEVector2& Vector) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < ZE_ZERO_THRESHOLD) && 
		(ZEMath::Abs(this->y - Vector.y) < ZE_ZERO_THRESHOLD));
}

bool ZEVector2::Equals(const ZEVector2& Vector, float Threshold) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < Threshold) && 
		(ZEMath::Abs(this->y - Vector.y) < Threshold));
}

ZEVector2 ZEVector2::operator +(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEVector2 ZEVector2::operator -(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Substution(Temp, *this, RightOperand);
	return Temp;
}

ZEVector2 ZEVector2::operator *(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEVector2 ZEVector2::operator *(float s) const
{
	ZEVector2 Temp;
	ZEVector2::Scale(Temp, *this, s);
	return Temp;
}

ZEVector2 ZEVector2::operator /(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Divide(Temp, *this, RightOperand);
	return Temp;
}

ZEVector2 ZEVector2::operator /(float s) const
{
	ZEVector2 Temp;
	ZEVector2::Divide(Temp, *this, s);
	return Temp;
}

ZEVector2 ZEVector2::operator-() const
{
	ZEVector2 Temp(-x, -y);
	return Temp;
}

ZEVector2& ZEVector2::operator +=(const ZEVector2 &RightOperand)
{

	ZEVector2::Add(*this, *this, RightOperand);
	return *this;
}

ZEVector2& ZEVector2::operator -=(const ZEVector2 &RightOperand)
{
	ZEVector2::Substution(*this, *this, RightOperand);
	return *this;
}

ZEVector2& ZEVector2::operator *=(const ZEVector2 &RightOperand)
{
	ZEVector2::Multiply(*this, *this, RightOperand);
	return *this;
}

ZEVector2& ZEVector2::operator *=(float s)
{
	ZEVector2::Scale(*this, *this, s);
	return *this;
}

ZEVector2& ZEVector2::operator /=(const ZEVector2 &RightOperand)
{
	ZEVector2::Divide(*this, *this, RightOperand);
	return *this;
}

ZEVector2& ZEVector2::operator /=(float s)
{
	ZEVector2::Divide(*this, *this, s);
	return *this;
}

bool ZEVector2::operator ==(const ZEVector2 &RightOperand) const
{
	return Equals(RightOperand);
}

bool ZEVector2::operator !=(const ZEVector2 &RightOperand) const
{
	return !Equals(RightOperand);
}

float ZEVector2::operator[](ZESize Index) const
{
	return M[Index];
}

float& ZEVector2::operator[](ZESize Index)
{
	return M[Index];
}

ZEVector2::ZEVector2(float x, float y)
{
	Create(x, y);
}

ZEVector2::ZEVector2(const ZEVector2& Start, const ZEVector2& End)
{
	Create(Start, End);
}

ZEVector2::ZEVector2()
{

}

ZEVector2 operator*(float S, const ZEVector2& RightOperand)
{
	return RightOperand * S;
}

//ZE_OBJECT_IMPL(ZEVector3)

// ZEVector3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEVector3 ZEVector3::Zero = ZEVector3(0.0f, 0.0f, 0.0f);
const ZEVector3 ZEVector3::One = ZEVector3(1.0f, 1.0f, 1.0f);
const ZEVector3 ZEVector3::UnitX = ZEVector3(1.0f, 0.0f, 0.0f);
const ZEVector3 ZEVector3::UnitY = ZEVector3(0.0f, 1.0f, 0.0f);
const ZEVector3 ZEVector3::UnitZ = ZEVector3(0.0f, 0.0f, 1.0f);
const ZEVector3 ZEVector3::MinValue	= ZEVector3(ZE_FLOAT_MIN, ZE_FLOAT_MIN, ZE_FLOAT_MIN);
const ZEVector3 ZEVector3::MaxValue	= ZEVector3(ZE_FLOAT_MAX, ZE_FLOAT_MAX, ZE_FLOAT_MAX);

inline void ZEVector3::Create(ZEVector3& Out, float x, float y, float z)
{
	Out.x = x;
	Out.y = y;
	Out.z = z;
}

inline void ZEVector3::Create(ZEVector3& Out, const ZEVector3 &Start, const ZEVector3 &End)
{
	Out.x = End.x - Start.x;
	Out.y = End.y - Start.y;
	Out.z = End.z - Start.z;
}

void ZEVector3::CreateFromSpherical(ZEVector3& Out, float Radius, float Theta, float Phi)
{
// 	float SinTheta = ZEAngle::Sin(Theta);
// 	Out.x = Radius * SinTheta * ZEAngle::Cos(Phi);
// 	Out.y = Radius * ZEAngle::Cos(Theta);
// 	Out.z = Radius * SinTheta * ZEAngle::Sin(Phi);

	float SinPhi = ZEAngle::Sin(-Phi);
	Out.x = Radius * ZEAngle::Cos(-Theta) * SinPhi;
	Out.y = Radius * ZEAngle::Cos(-Phi);
	Out.z = Radius * ZEAngle::Sin(-Theta) * SinPhi;
}

void ZEVector3::CreateFromCylindirical(ZEVector3& Out, float Radius, float Theta, float Height)
{
	Out.x = Radius * ZEAngle::Cos(Theta);
	Out.y = Height;
	Out.z = Radius * ZEAngle::Sin(Theta);
}

void ZEVector3::ConvertToSpherical(const ZEVector3& In, float& Radius, float& Theta, float& Phi)
{
// 	Radius = ZEMath::Sqrt(In.x * In.x + In.y * In.y + In.z * In.z);
// 	Theta =  ZEAngle::ArcTan(In.y / ZEMath::Sqrt(In.x * In.x + In.z * In.z));
// 	Phi = ZEAngle::ArcTan(In.z / In.x);

	Radius = ZEMath::Sqrt(In.x * In.x + In.y * In.y + In.z * In.z);
	Phi = -ZEAngle::ArcCos(In.y / Radius);

	if (In.x >= 0.0f)
		Theta = -ZEAngle::ArcSin(In.z / ZEMath::Sqrt(In.x * In.x + In.z * In.z));
	else
		Theta = -(ZE_PI - ZEAngle::ArcSin(In.z / ZEMath::Sqrt(In.x * In.x + In.z * In.z)));

	//ZEAngle::ArcTan(In.z / In.x);
}

void ZEVector3::ConvertToCylindirical(const ZEVector3& In, float& Radius, float& Theta, float& Height)
{
	Radius = ZEMath::Sqrt(In.x * In.x + In.z * In.z);
	Theta = ZEAngle::ArcTan(In.z / In.x);
	Height = In.y;
}

void ZEVector3::Add(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
	Out.z = A.z + B.z;
}

void ZEVector3::Sub(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
	Out.z = A.z - B.z;
}

void ZEVector3::Scale(ZEVector3 &Out, const ZEVector3 &A, float s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
	Out.z = A.z * s;
}

void ZEVector3::Multiply(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
	Out.z = A.z * B.z;
}

void ZEVector3::Divide(ZEVector3& Out, const ZEVector3& A, float s)
{
	Out.x = A.x / s;
	Out.y = A.y / s;
	Out.z = A.z / s;
}

void ZEVector3::CrossProduct(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.y * B.z - A.z * B.y;
	Out.y = A.z * B.x - A.x * B.z;
	Out.z = A.x * B.y - A.y * B.x;
}

bool ZEVector3::IsValid() const
{
	if (x != x || y != y || z != z)
		return false;
	else
		return true;
}

bool ZEVector3::IsNormalized() const
{
	return ZEMath::Abs(Length() - 1.0f) < 0.001;
}

float ZEVector3::Length(const ZEVector3& Vector)
{
	return ZEMath::Sqrt(ZEVector3::DotProduct(Vector, Vector));
}

float ZEVector3::LengthSquare(const ZEVector3& Vector)
{
	return ZEVector3::DotProduct(Vector, Vector);
}

float ZEVector3::Distance(const ZEVector3& A, const ZEVector3& B)
{
	return ZEMath::Sqrt(ZEVector3::DistanceSquare(A, B));
}

void ZEVector3::Lerp(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B, float Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
	Out.z = A.z + (B.z - A.z) * Factor;
}

inline float ZEVector3::Length() const
{
	return ZEMath::Sqrt(ZEVector3::DotProduct(*this, *this));
}

float ZEVector3::LengthSquare() const
{
	return ZEVector3::DotProduct(*this, *this);
}

ZEVector3 ZEVector3::Normalize() const
{
	ZEVector3 Temp;
	ZEVector3::Normalize(Temp, *this);
	return Temp;
}

void ZEVector3::NormalizeSelf()
{
	ZEVector3::Normalize(*this, *this);
}

ZEVector3 ZEVector3::Clamp(float MinValue, float MaxValue) const
{
	ZEVector3 Temp;
	ZEVector3::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

ZEVector3 ZEVector3::Clamp(const ZEVector3& MinValue, const ZEVector3& MaxValue) const
{
	ZEVector3 Temp;
	ZEVector3::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

ZEVector3 ZEVector3::ClampLower(float MinValue) const
{
	ZEVector3 Temp;
	ZEVector3::ClampLower(Temp, *this, MinValue);
	return Temp;
}

ZEVector3 ZEVector3::ClampLower(const ZEVector3& MinValue) const
{
	ZEVector3 Temp;
	ZEVector3::ClampLower(Temp, *this, MinValue);
	return Temp;

}

ZEVector3 ZEVector3::ClampUpper(float MaxValue) const
{
	ZEVector3 Temp;
	ZEVector3::ClampUpper(Temp, *this, MaxValue);
	return Temp;
}

ZEVector3 ZEVector3::ClampUpper(const ZEVector3& MaxValue) const
{
	ZEVector3 Temp;
	ZEVector3::ClampUpper(Temp, *this, MaxValue);
	return Temp;
}

void ZEVector3::ClampSelf(float MinValue, float MaxValue)
{
	ZEVector3::Clamp(*this, *this, MinValue, MaxValue);
}

void ZEVector3::ClampSelf(const ZEVector3& MinValue, const ZEVector3& MaxValue)
{
	ZEVector3::Clamp(*this, *this, MinValue, MaxValue);
}

void ZEVector3::ClampLowerSelf(float MinValue)
{
	ZEVector3::ClampLower(*this, *this, MinValue);
}

void ZEVector3::ClampLowerSelf(const ZEVector3& MinValue)
{
	ZEVector3::ClampLower(*this, *this, MinValue);
}

void ZEVector3::ClampUpperSelf(float MaxValue)
{
	ZEVector3::ClampUpper(*this, *this, MaxValue);
}

void ZEVector3::ClampUpperSelf(const ZEVector3& MaxValue)
{
	ZEVector3::ClampUpper(*this, *this, MaxValue);

}

ZEVector3 ZEVector3::Saturate() const
{
	ZEVector3 Temp;
	ZEVector3::Saturate(Temp, *this);
	return Temp;
}

ZEVector2 ZEVector3::ToVector2(const ZEVector3& Vector)
{
	return ZEVector2(Vector.x, Vector.y);
}

void ZEVector3::SaturateSelf()
{
	ZEVector3::Saturate(*this, *this);
}

float ZEVector3::Min() const
{
	float MinComponent = x;

	if(y < MinComponent)
		MinComponent = y;

	if(z < MinComponent)
		MinComponent = z;

	return MinComponent;
}

float ZEVector3::Max() const
{
	float MaxComponent = x;

	if(y > MaxComponent)
		MaxComponent = y;

	if(z > MaxComponent)
		MaxComponent = z;

	return MaxComponent;
}

bool ZEVector3::Equals(const ZEVector3& Vector) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < ZE_ZERO_THRESHOLD) && 
		(ZEMath::Abs(this->y - Vector.y) < ZE_ZERO_THRESHOLD) &&
		(ZEMath::Abs(this->z - Vector.z) < ZE_ZERO_THRESHOLD));
}

bool ZEVector3::Equals(const ZEVector3& Vector, float Threshold) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < Threshold) && 
		(ZEMath::Abs(this->y - Vector.y) < Threshold) &&
		(ZEMath::Abs(this->z - Vector.z) < Threshold));
}

ZEVector2 ZEVector3::ToVector2() const
{
	return ZEVector2(x, y);
}

ZEVector3 ZEVector3::operator+(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEVector3::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEVector3::operator-(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEVector3::operator*(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEVector3::Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEVector3::operator*(float s) const
{
	ZEVector3 Temp;
	ZEVector3::Scale(Temp, *this, s);
	return Temp;
}

ZEVector3 ZEVector3::operator/(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEVector3::Divide(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEVector3::operator/(float s) const
{
	ZEVector3 Temp;
	ZEVector3::Divide(Temp, *this, s);
	return Temp;
}

ZEVector3 ZEVector3::operator-() const
{
	ZEVector3 Temp(-x, -y, -z);
	return Temp;
}

ZEVector3& ZEVector3::operator += (const ZEVector3 &RightOperand)
{
	ZEVector3::Add(*this, *this, RightOperand);
	return *this;
}

ZEVector3& ZEVector3::operator -= (const ZEVector3 &RightOperand)
{
	ZEVector3::Sub(*this, *this, RightOperand);
	return *this;
}

ZEVector3& ZEVector3::operator *= (const ZEVector3 &RightOperand)
{
	ZEVector3::Multiply(*this, *this, RightOperand);
	return  *this;
}

ZEVector3& ZEVector3::operator *= (float s)
{
	ZEVector3::Scale(*this, *this, s);
	return *this;
}

ZEVector3& ZEVector3::operator /= (const ZEVector3 &RightOperand)
{
	ZEVector3::Divide(*this, *this, RightOperand);
	return *this;
}

ZEVector3& ZEVector3::operator /= (float s)
{
	ZEVector3::Divide(*this, *this, s);
	return *this;
}

bool ZEVector3::operator == (const ZEVector3 &RightOperand) const
{
	return Equals(RightOperand);
}

bool ZEVector3::operator != (const ZEVector3 &RightOperand) const
{
	return !Equals(RightOperand);
}

float ZEVector3::operator[](ZESize Index) const
{
	return M[Index];
}

float& ZEVector3::operator[](ZESize Index)
{
	return M[Index];
}

ZEVector3::ZEVector3(float x, float y, float z)
{
	Create(*this, x, y, z);
}

ZEVector3::ZEVector3(ZEVector2& Base, float z)
{
	x = Base.x;
	y = Base.y;
	this->z = z;
}

ZEVector3::ZEVector3(const ZEVector3& Start, const ZEVector3& End)
{
	Create(*this, Start, End);
}

ZEVector3::ZEVector3()
{

}

ZEVector3 operator*(float S, const ZEVector3& RightOperand)
{
	return RightOperand * S;
}

//ZE_OBJECT_IMPL(ZEVector4)

// ZEVector4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEVector4 ZEVector4::Zero	= ZEVector4(0.0f, 0.0f, 0.0f, 0.0f);
const ZEVector4 ZEVector4::One	= ZEVector4(1.0f, 1.0f, 1.0f, 1.0f);
const ZEVector4 ZEVector4::UnitX	= ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);
const ZEVector4 ZEVector4::UnitY	= ZEVector4(0.0f, 1.0f, 0.0f, 0.0f);
const ZEVector4 ZEVector4::UnitZ	= ZEVector4(0.0f, 0.0f, 1.0f, 0.0f);
const ZEVector4 ZEVector4::UnitW	= ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);

const ZEVector4 ZEVector4::HomogenousUnitX = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
const ZEVector4 ZEVector4::HomogenousUnitY = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
const ZEVector4 ZEVector4::HomogenousUnitZ = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);

const ZEVector4 ZEVector4::MinValue	= ZEVector4(ZE_FLOAT_MIN, ZE_FLOAT_MIN, ZE_FLOAT_MIN, ZE_FLOAT_MIN);
const ZEVector4 ZEVector4::MaxValue	= ZEVector4(ZE_FLOAT_MAX, ZE_FLOAT_MAX, ZE_FLOAT_MAX, ZE_FLOAT_MAX);

inline void ZEVector4::Create(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline void ZEVector4::Create(const ZEVector4 &Start, const ZEVector4 &End)
{
	x = End.x - Start.x;
	y = End.y - Start.y;
	z = End.z - Start.z;
	w = End.w - Start.w;
}

float ZEVector4::Length(const ZEVector4& Vector)
{
	return ZEMath::Sqrt(ZEVector4::DotProduct(Vector, Vector));
}

float ZEVector4::LengthSquare(const ZEVector4& Vector)
{
	return ZEVector4::DotProduct(Vector, Vector);
}

float ZEVector4::Distance(const ZEVector4& A, const ZEVector4& B)
{
	return ZEMath::Sqrt(ZEVector4::DistanceSquare(A, B));
}

bool ZEVector4::IsValid() const
{
	if (w != w || x != x || y != y || z != z)
		return false;
	else
		return true;
}

bool ZEVector4::IsNormalized() const
{
	return ZEMath::Abs(Length() - 1.0f) < 0.001;
}

inline float ZEVector4::Length() const
{
	return ZEMath::Sqrt(ZEVector4::DotProduct(*this, *this));
}

float ZEVector4::LengthSquare() const
{
	return ZEVector4::DotProduct(*this, *this);
}

ZEVector4 ZEVector4::Normalize() const
{
	ZEVector4 Temp;
	ZEVector4::Normalize(Temp, *this);

	return Temp;
}

void ZEVector4::NormalizeSelf()
{
	ZEVector4::Normalize(*this, *this);
}

ZEVector4 ZEVector4::Clamp(float MinValue, float MaxValue) const
{
	ZEVector4 Temp;
	ZEVector4::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

ZEVector4 ZEVector4::Clamp(const ZEVector4& MinValue, const ZEVector4& MaxValue) const
{
	ZEVector4 Temp;
	ZEVector4::Clamp(Temp, *this, MinValue, MaxValue);
	return Temp;
}

void ZEVector4::ClampSelf(float MinValue, float MaxValue)
{
	ZEVector4::Clamp(*this, *this, MinValue, MaxValue);
}

void ZEVector4::ClampSelf(const ZEVector4& MinValue, const ZEVector4& MaxValue)
{
	ZEVector4::Clamp(*this, *this, MinValue, MaxValue);
}

ZEVector4 ZEVector4::Saturate() const
{
	ZEVector4 Temp;
	ZEVector4::Saturate(Temp, *this);
	return Temp;
}

ZEVector2 ZEVector4::ToVector2(const ZEVector4& Vector)
{
	return ZEVector2(Vector.x, Vector.y);
}

ZEVector3 ZEVector4::ToVector3(const ZEVector4& Vector)
{
	return ZEVector3(Vector.x, Vector.y, Vector.z);
}

void ZEVector4::SaturateSelf()
{
	ZEVector4::Saturate(*this, *this);
}

float ZEVector4::Min() const
{
	float Min = x;
	for (ZESize I = 1; I < ZE_VECTOR4_ELEMENT_COUNT; I++)
	{
		if (M[I] < Min)
		{
			Min = M[I];
		}
	}

	return Min;
}

float ZEVector4::Max() const
{
	float Max = x;
	for (ZESize I = 1; I < ZE_VECTOR4_ELEMENT_COUNT; I++)
	{
		if (M[I] > Max)
		{
			Max = M[I];
		}
	}

	return Max;
}

bool ZEVector4::Equals(const ZEVector4& Vector) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < ZE_ZERO_THRESHOLD) && 
		(ZEMath::Abs(this->y - Vector.y) < ZE_ZERO_THRESHOLD) &&
		(ZEMath::Abs(this->z - Vector.z) < ZE_ZERO_THRESHOLD) &&
		(ZEMath::Abs(this->w - Vector.w) < ZE_ZERO_THRESHOLD));
}

bool ZEVector4::Equals(const ZEVector4& Vector, float Threshold) const
{
	return ((ZEMath::Abs(this->x - Vector.x) < Threshold) && 
		(ZEMath::Abs(this->y - Vector.y) < Threshold) &&
		(ZEMath::Abs(this->z - Vector.z) < Threshold) &&
		(ZEMath::Abs(this->w - Vector.w) < Threshold));
}

ZEVector2 ZEVector4::ToVector2() const
{
	return ZEVector2(x, y);
}

ZEVector3 ZEVector4::ToVector3() const
{
	return ZEVector3(x, y, z);
}

ZEVector4 ZEVector4::operator+(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEVector4::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEVector4::operator-(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEVector4::Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEVector4::operator*(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEVector4::Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEVector4::operator*(float s) const
{
	ZEVector4 Temp;
	ZEVector4::Scale(Temp, *this, s);
	return Temp;
}

ZEVector4 ZEVector4::operator/(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEVector4::Divide(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEVector4::operator/(float s) const
{
	ZEVector4 Temp;
	ZEVector4::Divide(Temp, *this, s);
	return Temp;
}

ZEVector4& ZEVector4::operator += (const ZEVector4 &RightOperand)
{
	ZEVector4::Add(*this, *this, RightOperand);
	return *this;
}

ZEVector4& ZEVector4::operator -= (const ZEVector4 &RightOperand)
{
	ZEVector4::Sub(*this, *this, RightOperand);
	return *this;
}

ZEVector4& ZEVector4::operator *= (const ZEVector4 &RightOperand)
{
	ZEVector4::Multiply(*this, *this, RightOperand);
	return *this;
}

ZEVector4& ZEVector4::operator *= (float s)
{
	ZEVector4::Scale(*this, *this, s);
	return *this;
}

ZEVector4& ZEVector4::operator /= (const ZEVector4 &RightOperand)
{
	ZEVector4::Divide(*this, *this, RightOperand);
	return *this;
}

ZEVector4& ZEVector4::operator /= (float s)
{
	ZEVector4::Divide(*this, *this, s);
	return *this;
}

ZEVector4 ZEVector4::operator-() const
{
	ZEVector4 Temp(-x, -y, -z, -w);
	return Temp;
}

bool ZEVector4::operator == (const ZEVector4 &RightOperand) const
{
	return Equals(RightOperand);
}

bool ZEVector4::operator != (const ZEVector4 &RightOperand) const
{
	return !Equals(RightOperand);
}

float ZEVector4::operator[](ZESize Index) const
{
	return M[Index];
}

float& ZEVector4::operator[](ZESize Index)
{
	return M[Index];
}

ZEVector4::ZEVector4(float x, float y, float z, float w)
{
	Create(x, y, z, w);
}

ZEVector4::ZEVector4(const ZEVector2& Base, float z, float w)
{
	Create(Base.x, Base.y, z, w);
}

ZEVector4::ZEVector4(const ZEVector3& Base, float w)
{
	Create(Base.x, Base.y, Base.z, w);
}

ZEVector4::ZEVector4(const ZEVector4& Start, const ZEVector4& End)
{
	Create(Start, End);
}

ZEVector4::ZEVector4()
{

}

ZEVector4 operator*(float S, const ZEVector4& RightOperand)
{
	return RightOperand * S;
}
