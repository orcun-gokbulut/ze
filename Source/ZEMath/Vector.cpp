//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Vector.cpp
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

#include "vector.h"
#include <math.h>
#include "matrix.h"

const ZEVector2 ZEVector2::Zero = ZEVector2(0.0f, 0.0f);
const ZEVector2 ZEVector2::One = ZEVector2(1.0f, 1.0f);
const ZEVector2 ZEVector2::UnitX = ZEVector2(1.0f, 0.0f);
const ZEVector2 ZEVector2::UnitY = ZEVector2(0.0f, 1.0f);

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

inline void ZEVector2::Add(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
}

inline void ZEVector2::Substution(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
}

void ZEVector2::Multiply(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
}

inline float ZEVector2::DotProduction(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B)
{
	return A.x * B.x + A.y * B.y ;
}

void ZEVector2::Scale(ZEVector2 &Out, const ZEVector2 &A, float s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
}

float ZEVector2::Length(const ZEVector2 &A)
{
	return sqrt(A.x * A.x + A.y * A.y);
}

inline float ZEVector2::LengthSquare(const ZEVector2 &A)
{
	return A.x * A.x + A.y * A.y;
}

float ZEVector2::Distance(const ZEVector2 &A, const ZEVector2 &B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

inline float ZEVector2::DistanceSquare(const ZEVector2 &A, const ZEVector2 &B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

void ZEVector2::Normalize(ZEVector2 &Out, const ZEVector2 &A)
{
	float L = A.Length();
	Out.x = A.x / L;
	Out.y = A.y / L;
}

void ZEVector2::Lerp(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B, float Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
}

inline float ZEVector2::Length() const
{
	return sqrt(x * x + y * y);
}

ZEVector2 ZEVector2::operator +(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Add(Temp, RightOperand, *this);
	return Temp;
}

ZEVector2 ZEVector2::operator -(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Substution(Temp, RightOperand, *this);
	return Temp;
}

ZEVector2 ZEVector2::operator *(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEVector2::Multiply(Temp, RightOperand, *this);
	return Temp;
}

ZEVector2 ZEVector2::operator *(float s) const
{
	ZEVector2 Temp;
	ZEVector2::Scale(Temp, *this, s);
	return Temp;
}

ZEVector2 ZEVector2::operator-() const
{
	ZEVector2 Temp(-x, -y);
	return Temp;
}

ZEVector2& ZEVector2::operator +=(const ZEVector2 &RightOperand)
{

	ZEVector2::Add(*this, RightOperand, *this);
	return *this;
}

ZEVector2& ZEVector2::operator -=(const ZEVector2 &RightOperand)
{
	ZEVector2::Substution(*this, RightOperand, *this);
	return *this;
}

ZEVector2& ZEVector2::operator *=(const ZEVector2 &RightOperand)
{
	ZEVector2::Multiply(*this, RightOperand, *this);
	return *this;
}

ZEVector2& ZEVector2::operator *=(float s)
{
	ZEVector2::Scale(*this, *this, s);
	return *this;
}

bool ZEVector2::operator ==(const ZEVector2 &RightOperand) const
{
	return (this->x == RightOperand.x) && (this->y == RightOperand.y);
}

bool ZEVector2::operator !=(const ZEVector2 &RightOperand) const
{
	return (this->x != RightOperand.x) || (this->y != RightOperand.y);

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


// ZEVector3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEVector3 ZEVector3::Zero	= ZEVector3(0.0f, 0.0f, 0.0f);
const ZEVector3 ZEVector3::One	= ZEVector3(1.0f, 1.0f, 1.0f);
const ZEVector3 ZEVector3::UnitX = ZEVector3(1.0f, 0.0f, 0.0f);
const ZEVector3 ZEVector3::UnitY = ZEVector3(0.0f, 1.0f, 0.0f);
const ZEVector3 ZEVector3::UnitZ = ZEVector3(0.0f, 0.0f, 1.0f);

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

inline void ZEVector3::Add(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
	Out.z = A.z + B.z;
}


inline void ZEVector3::Sub(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
	Out.z = A.z - B.z;
}

inline void ZEVector3::Scale(ZEVector3& Out, const ZEVector3& A, float s)
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

float ZEVector3::DotProduct(const ZEVector3& A, const ZEVector3& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

inline void ZEVector3::CrossProduct(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B)
{
	Out.x = A.y * B.z - A.z * B.y;
	Out.y = A.z * B.x - A.x * B.z;
	Out.z = A.x * B.y - A.y * B.x;
}

float ZEVector3::Length(const ZEVector3& Vector)
{
	return sqrt(Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z);
}

float ZEVector3::LengthSquare(const ZEVector3& Vector)
{
	return Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z;
}

float ZEVector3::Distance(const ZEVector3& A, const ZEVector3& B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z));
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

void ZEVector3::Lerp(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B, float Factor)
{
	Out.x = A.x + (B.x - A.x) * Factor;
	Out.y = A.y + (B.y - A.y) * Factor;
	Out.z = A.z + (B.z - A.z) * Factor;
}

inline float ZEVector3::Length() const
{
	return sqrt(x * x + y * y + z * z);
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
	ZEVector3::CrossProduct(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEVector3::operator*(float s) const
{
	ZEVector3 Temp;
	ZEVector3::Scale(Temp, *this, s);
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
	ZEVector3::Add(*this, *this, RightOperand);
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

bool ZEVector3::operator == (const ZEVector3 &RightOperand) const
{
	return ((this->x == RightOperand.x) && (this->y == RightOperand.y) && (this->z == RightOperand.z));
}

bool ZEVector3::operator != (const ZEVector3 &RightOperand) const
{
	return ((this->x != RightOperand.x) || (this->y != RightOperand.y) || (this->z != RightOperand.z));
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4  ZEVector4 //
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

inline void ZEVector4::Add(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x + B.x;
	Out.y = A.y + B.y;
	Out.z = A.z + B.z;
	Out.w = A.w + B.w;
}


inline void ZEVector4::Sub(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x - B.x;
	Out.y = A.y - B.y;
	Out.z = A.z - B.z;
	Out.w = A.w - B.w;
}

inline void ZEVector4::Scale(ZEVector4& Out, const ZEVector4& A, float s)
{
	Out.x = A.x * s;
	Out.y = A.y * s;
	Out.z = A.z * s;
	Out.z = A.w * s;
}

void ZEVector4::Multiply(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B)
{
	Out.x = A.x * B.x;
	Out.y = A.y * B.y;
	Out.z = A.z * B.z;
	Out.w = A.w * B.w;
}

float ZEVector4::DotProduct(const ZEVector4& A, const ZEVector4& B) 
{
	return A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
}

float ZEVector4::Length(const ZEVector4& Vector)
{
	return sqrt(Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z + Vector.w * Vector.w);
}

inline float ZEVector4::LengthSquare(const ZEVector4& Vector)
{
	return (Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z + Vector.w * Vector.w);
}

float ZEVector4::Distance(const ZEVector4& A, const ZEVector4& B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z) + (A.w - B.w) * (A.w - B.w));
}

inline float ZEVector4::DistanceSquare(const ZEVector4& A, const ZEVector4& B)
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

inline float ZEVector4::Length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
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


ZEVector4& ZEVector4::operator += (const ZEVector4 &RightOperand)
{
	ZEVector4::Add(*this, *this, RightOperand);
	return *this;
}

ZEVector4& ZEVector4::operator -= (const ZEVector4 &RightOperand)
{
	ZEVector4::Add(*this, *this, RightOperand);
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


ZEVector4 ZEVector4::operator-() const
{
	ZEVector4 Temp(-x, -y, -z, -w);
	return Temp;
}


bool ZEVector4::operator == (const ZEVector4 &RightOperand) const
{
	return ((this->x == RightOperand.x) && (this->y == RightOperand.y) && (this->z == RightOperand.z) && (this->w == RightOperand.w));
}

bool ZEVector4::operator != (const ZEVector4 &RightOperand) const
{
	return ((this->x != RightOperand.x) || (this->y != RightOperand.y) || (this->z != RightOperand.z) || (this->w != RightOperand.w));
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
