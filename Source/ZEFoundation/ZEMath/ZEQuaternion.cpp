//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuaternion.cpp
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

#include "ZEQuaternion.h"
#include "ZEMath.h"
#include "ZEAngle.h"
#include "ZEError.h"
#include "ZEMath/ZEMath.h"

#include <stdlib.h>

const ZEQuaternion ZEQuaternion::Zero = ZEQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
const ZEQuaternion ZEQuaternion::Identity = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

void ZEQuaternion::Create(ZEQuaternion& Output, float w, float x, float y, float z)
{
	Output.w = w;
	Output.x = x;
	Output.y = y;
	Output.z = z;
}

void ZEQuaternion::CreateFromEuler(ZEQuaternion& Output, float x, float y, float z)
{
   float SinPitch		= ZEAngle::Sin(x * 0.5f);
   float CosPitch		= ZEAngle::Cos(x * 0.5f);
   float SinYaw			= ZEAngle::Sin(y * 0.5f);
   float CosYaw			= ZEAngle::Cos(y * 0.5f);
   float SinRoll		= ZEAngle::Sin(z * 0.5f);
   float CosRoll		= ZEAngle::Cos(z * 0.5f);
   float CosPitchCosYaw	= CosPitch * CosYaw;
   float SinPitchSinYaw	= SinPitch * SinYaw;
  
   Output.w = CosRoll * CosPitchCosYaw + SinRoll * SinPitchSinYaw;
   Output.x = CosRoll * SinPitch * CosYaw + SinRoll * CosPitch * SinYaw;
   Output.y = CosRoll * CosPitch * SinYaw - SinRoll * SinPitch * CosYaw;
   Output.z = SinRoll * CosPitchCosYaw - CosRoll * SinPitchSinYaw;

   // zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
   // zeAssert(!Output.IsNormalized(), "Output quaternion is not normalized.");

}

void ZEQuaternion::CreateFromEuler(ZEQuaternion& Output, const ZEVector3& Rotation)
{
	CreateFromEuler(Output, Rotation.x, Rotation.y, Rotation.z);
}

void ZEQuaternion::CreateFromAngleAxis(ZEQuaternion& Output, float Angle, const ZEVector3& Axis)
{
	//zeAssert(!Axis.IsNormalized(), "Input axis is not normalized.");
	zeAssert(!Axis.IsValid(), "Input axis is not a valid vector.");

	float SinAngle = ZEAngle::Sin(Angle / 2.0f);
	if (!ZEMath::IsValid(SinAngle))
	{
		Output = ZEQuaternion::Identity;
		return;
	}

	Output.w = ZEAngle::Cos(Angle / 2.0f);
	Output.x = Axis.x * SinAngle;
	Output.y = Axis.y * SinAngle;
	Output.z = Axis.z * SinAngle;

	Output.NormalizeSelf();

	zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
	zeAssert(!Output.IsNormalized(), "Output quaternion is not normalized.");
}

void ZEQuaternion::CreateIdentity(ZEQuaternion& Output)
{
	Output.w = 1.0f;
	Output.x = 0.0f;
	Output.y = 0.0f;
	Output.z = 0.0f;
}

void ZEQuaternion::CreateFromDirection(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up)
{
	ZEVector3 NewRight, NewUp, NewDirection;

	ZEVector3::Normalize(NewDirection, Direction);
	//float Dot = ZEVector3::DotProduct(NewDirection, ZEVector3::UnitY);

	ZEVector3::CrossProduct(NewRight, Up, NewDirection);
	ZEVector3::CrossProduct(NewUp, NewDirection, NewRight);

	/*	if (Dot < 0.99f)
	{	
		ZEVector3::CrossProduct(NewRight, Up, NewDirection);
		ZEVector3::CrossProduct(NewUp, NewDirection, NewRight);
	}
	else
	{
		ZEVector3::CrossProduct(NewUp, ZEVector3::UnitX, NewDirection);
		ZEVector3::CrossProduct(NewRight, NewUp, NewDirection);
	}*/

	ZEVector3::Normalize(NewRight, NewRight);
	ZEVector3::Normalize(NewUp, NewUp);

	ZEMatrix3x3 Basis(
	NewRight.x, NewUp.x, NewDirection.x,                                        
	NewRight.y, NewUp.y, NewDirection.y,                          
	NewRight.z, NewUp.z, NewDirection.z);

	Output.w = ZEMath::Sqrt(1.0f + Basis.M11 + Basis.M22 + Basis.M33) / 2.0f;
	
	float Scale = Output.w * 4.0f;
	Output.x = (Basis.M32 - Basis.M23) / Scale;
	Output.y = (Basis.M13 - Basis.M31) / Scale;
	Output.z = (Basis.M21 - Basis.M12) / Scale;
	
/*
	ZEQuaternion Quat;
	Quat.w = ZEMath::Sqrt(1.0f + Right.x + NewUp.y + Direction.z) / 2.0f;
	float Scale = Quat.w * 4.0f;
	Quat.x = (Direction.y - NewUp.z) / Scale;
	Quat.y = (Right.z - Direction.x) / Scale;
	Quat.z = (NewUp.x - Right.y) / Scale;*/

	ZEQuaternion::Normalize(Output, Output);

	//zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
}

void ZEQuaternion::CreateFromMatrix(ZEQuaternion& Output, const ZEMatrix4x4& Matrix)
{
	Output.w = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f + Matrix.M11 + Matrix.M22 + Matrix.M33)) / 2.0f;
	Output.x = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f + Matrix.M11 - Matrix.M22 - Matrix.M33)) / 2.0f;
	Output.y = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f - Matrix.M11 + Matrix.M22 - Matrix.M33)) / 2.0f;
	Output.z = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f - Matrix.M11 - Matrix.M22 + Matrix.M33)) / 2.0f;
	Output.x = ZEMath::CopySign(Output.x, Matrix.M32 - Matrix.M23);
	Output.y = ZEMath::CopySign(Output.y, Matrix.M13 - Matrix.M31);
	Output.z = ZEMath::CopySign(Output.z, Matrix.M21 - Matrix.M12);

	ZEQuaternion::Normalize(Output, Output);

	zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
}

static inline void Product_NOCHECK(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B)
{
	Output.w = A.w * B.w - A.x * B.x - A.y * B.y - A.z * B.z;
	Output.x = A.w * B.x + A.x * B.w + A.y * B.z - A.z * B.y;
	Output.y = A.w * B.y - A.x * B.z + A.y * B.w + A.z * B.x;
	Output.z = A.w * B.z + A.x * B.y - A.y * B.x + A.z * B.w;
}


void ZEQuaternion::Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B)
{
	zeAssert(!A.IsValid(), "Input A quaternion is not valid.");
	zeAssert(!A.IsNormalized(), "Input A quaternion is no normalized.");
	
	zeAssert(!B.IsValid(), "Input B quaternion is not valid.");
	zeAssert(!B.IsNormalized(), "Input B quaternion is not normalized.");

	Product_NOCHECK(Output, A, B);

	zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
	zeAssert(!Output.IsNormalized(), "Output quaternion is not normalized.");
}

void ZEQuaternion::VectorProduct(ZEVector3& Output, const ZEQuaternion& Quaternion, const ZEVector3& Vector)
{
	zeAssert(!Quaternion.IsValid(), "Input Quaternion is not valid.");
	zeAssert(!Quaternion.IsNormalized(), "Input Quaternion is not normalized.");

	ZEQuaternion Vect(0, Vector.x, Vector.y, Vector.z), Temp, InvQ;
	
	Product_NOCHECK(Temp, Quaternion, Vect);	
	Conjugate(InvQ, Quaternion);
	Product_NOCHECK(Vect, Temp, InvQ);

	Output.x = Vect.x;
	Output.y = Vect.y;
	Output.z = Vect.z;
}

void ZEQuaternion::ConvertToEulerAngles(float &x, float &y, float &z, const ZEQuaternion& Quaternion)
{
	zeAssert(!Quaternion.IsValid(), "Input Quaternion is not valid.");
	zeAssert(!Quaternion.IsNormalized(), "Input Quaternion is not normalized.");

	float test = Quaternion.x * Quaternion.y + Quaternion.z * Quaternion.w;
	if (test > 0.499f) 
	{ 
		// Singularity at north pole
		y = 2.0f * ZEAngle::ArcTan2(Quaternion.x, Quaternion.w);
		z = ZE_PI_2;
		x = 0.0f;
		return;
	}	

	if (test < -0.499f) 
	{ 
		// Singularity at south pole
		y = -2.0f * ZEAngle::ArcTan2(Quaternion.x, Quaternion.w);
		z = - ZE_PI_2;
		x = 0.0f;
		return;
	}  

	float sqx = Quaternion.x * Quaternion.x;    
	float sqy = Quaternion.y * Quaternion.y;    
	float sqz = Quaternion.z * Quaternion.z;
    y = ZEAngle::ArcTan2(2.0f * Quaternion.y * Quaternion.w - 2.0f * Quaternion.x * Quaternion.z, 1.0f - 2.0f * sqy - 2.0f * sqz);
	z = ZEAngle::ArcSin(2.0f * test);
	x = ZEAngle::ArcTan2(2.0f * Quaternion.x * Quaternion.w - 2.0f * Quaternion.y * Quaternion.z, 1.0f - 2.0f * sqx - 2.0f * sqz);
}

void ZEQuaternion::ConvertToEulerAngles(ZEVector3& Rotation, const ZEQuaternion& Quaternion)
{
	ConvertToEulerAngles(Rotation.x, Rotation.y, Rotation.z, Quaternion);
}

void ZEQuaternion::ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion)
{
	ZEQuaternion::VectorProduct(Look, Quaternion, ZEVector3::UnitZ);
	ZEQuaternion::VectorProduct(Up, Quaternion, ZEVector3::UnitY);
}

void ZEQuaternion::ConvertToAngleAxis(float& Angle, ZEVector3& Axis, const ZEQuaternion& Quaternion)
{
	zeAssert(!Quaternion.IsValid(), "Input Quaternion is not valid.");
	zeAssert(!Quaternion.IsNormalized(), "Input Quaternion is not normalized.");

	Angle = 2.0f * ZEAngle::ArcCos(Quaternion.w);
	if (!ZEMath::IsValid(Angle) || Angle == 0.0f)
	{
		Axis = ZEVector3::UnitY;
		return;
	}

	float Temp = 1.0f / ZEAngle::Sin(Angle / 2.0f);
	Axis.x = Quaternion.x * Angle;
	Axis.y = Quaternion.y * Angle;
	Axis.z = Quaternion.z * Angle;

	zeAssert(!Axis.IsValid(), "Output parameter Axis is not a valid vector.");

	Axis.NormalizeSelf();
}

float ZEQuaternion::Length() const
{
	return Length(*this);
}

float ZEQuaternion::LengthSquare() const
{
	return LengthSquare(*this);
}

void ZEQuaternion::Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion)
{
	zeAssert(!Quaternion.IsValid(), "Parameter Quaternion is not valid.");

	Output.x = -Quaternion.x;
	Output.y = -Quaternion.y;
	Output.z = -Quaternion.z;
	Output.w = Quaternion.w;
}


void ZEQuaternion::Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor)
{
	zeAssert(!A.IsValid(), "Parameter A quaternion is not valid.");
	zeAssert(!A.IsNormalized(), "Parameter A quaternion is not normalized.");

	zeAssert(!B.IsValid(), "Parameter B quaternion is not valid.");
	zeAssert(!B.IsNormalized(), "Parameter A quaternion is not normalized.");

	float CosOmega = A.w * B.w + A.x * B.x + A.y * B.y + A.z * B.z;

	ZEQuaternion BTemp;
	if(CosOmega < 0.0f)             
	{                                
		BTemp.w = B.w * (-1.0f);                    
		BTemp.x = B.x * (-1.0f);                    
		BTemp.y = B.y * (-1.0f);                    
		BTemp.z = B.z * (-1.0f);                    
		CosOmega = CosOmega * (-1.0f);         
	}
	else
		BTemp = B;
	
	float RatioA;
	float RatioB;

	if (ZEMath::Abs(CosOmega) > 0.9999f)  
	{
		RatioA = 1.0f - Factor;
		RatioB = Factor;
	}
	else
	{
		// Compute the sin of the angle using the
		// trig identity sin^2(omega) + cos^2(omega) = 1
		float CosOmeag2 = CosOmega * CosOmega;
		float b = 1.0f - CosOmeag2 ;
		float SinOmega = ZEMath::Sqrt(b);

		// Compute the angle from its sin and cosine
		float Omega = ZEAngle::ArcTan2(SinOmega, CosOmega);

		// Compute inverse of denominator, so we only have
		// to divide once
		float OneOverSinOmega = 1.0f / SinOmega;

		// Compute interpolation parameters
		float a = 1.0f - Factor;
		float c = a * Omega;
		float d = Factor * Omega;

		RatioA = ZEAngle::Sin(c) * OneOverSinOmega;
		RatioB = ZEAngle::Sin(d) * OneOverSinOmega;
	}

	// Interpolate
	Output.w = (A.w * RatioA + BTemp.w * RatioB);
	Output.x = (A.x * RatioA + BTemp.x * RatioB);
	Output.y = (A.y * RatioA + BTemp.y * RatioB);
	Output.z = (A.z * RatioA + BTemp.z * RatioB);

	zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
	zeAssert(!Output.IsNormalized(), "Output quaternion is not normalized.");
}

void ZEQuaternion::Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion)
{
//	zeAssert(!Quaternion.IsValid(), "Parameter quaternion is not valid.");

	float L = Length(Quaternion);
	Output.x = Quaternion.x / L;
	Output.y = Quaternion.y / L;
	Output.z = Quaternion.z / L;
	Output.w = Quaternion.w / L;

	//zeAssert(!Output.IsValid(), "Output quaternion is not valid.");
}

float ZEQuaternion::Length(const ZEQuaternion& Quaternion)
{
	return ZEMath::Sqrt(Quaternion.x * Quaternion.x + Quaternion.y * Quaternion.y + Quaternion.z * Quaternion.z + Quaternion.w * Quaternion.w);
}

float ZEQuaternion::LengthSquare(const ZEQuaternion& Quaternion)
{
	return Quaternion.x * Quaternion.x + Quaternion.y * Quaternion.y + Quaternion.z * Quaternion.z + Quaternion.w * Quaternion.w;
}

void ZEQuaternion::ScaleRotation(ZEQuaternion& Output, const ZEQuaternion& Input, float Scale)
{
	ZEVector3 RotationAxis;
	float RotationAngle;
	ZEQuaternion::ConvertToAngleAxis(RotationAngle, RotationAxis, Input);
	RotationAngle *= Scale;
	ZEQuaternion::CreateFromAngleAxis(Output, RotationAngle, RotationAxis);
}

bool ZEQuaternion::IsValid() const
{
	if (w != w || x != x || y != y || z != z)
		return false;

	if ((w * w) < 0.0f)
		return false;
	if ((x * x) < 0.0f)
		return false;
	if ((y * y) < 0.0f)
		return false;
	if ((z * z) < 0.0f)
		return false;

	return true;
}

bool ZEQuaternion::IsNormalized() const
{
	return ZEMath::Abs(1.0f - LengthSquare()) < 0.01;
}

ZEQuaternion ZEQuaternion::Conjugate() const
{
	ZEQuaternion Temp;
	ZEQuaternion::Conjugate(Temp, *this);

	return Temp;
}

ZEQuaternion ZEQuaternion::Normalize() const
{
	ZEQuaternion Temp;
	ZEQuaternion::Normalize(Temp, *this);

	return Temp;
}

void ZEQuaternion::ConjugateSelf()
{
	ZEQuaternion::Conjugate(*this, *this);
}

void ZEQuaternion::NormalizeSelf()
{
	ZEQuaternion::Normalize(*this, *this);
}

float ZEQuaternion::RotationAngle() const
{
	return 2.0f * ZEAngle::ArcCos(w);
}

ZEVector3 ZEQuaternion::RotationAxis() const
{
	float Angle = 2.0f * ZEAngle::ArcCos(w);
	float Temp = 1.0f / ZEAngle::Sin(Angle / 2.0f);
	
	ZEVector3 Axis;
	Axis.x = x * Temp;
	Axis.y = y * Temp;
	Axis.z = z * Temp;

	return Axis;
}

ZEQuaternion ZEQuaternion::operator*(const ZEQuaternion& Other) const
{
	ZEQuaternion Temp;
	Product(Temp, *this, Other);
	return Temp;
}

ZEQuaternion& ZEQuaternion::operator*=(const ZEQuaternion& Other)
{
	ZEQuaternion Temp;
	Product(Temp, *this, Other);
	*this = Temp;
	return *this;
}

ZEVector3 ZEQuaternion::operator*(const ZEVector3& Vector) const
{
	ZEVector3 Temp;
	VectorProduct(Temp, *this, Vector);
	return Temp;
}

bool ZEQuaternion::operator==(const ZEQuaternion& RightOperand) const
{
	return ((ZEMath::Abs(this->x - RightOperand.x) < ZE_ZERO_THRESHOLD) && 
		(ZEMath::Abs(this->y - RightOperand.y) < ZE_ZERO_THRESHOLD) &&
		(ZEMath::Abs(this->z - RightOperand.z) < ZE_ZERO_THRESHOLD) &&
		(ZEMath::Abs(this->w - RightOperand.w) < ZE_ZERO_THRESHOLD));
}

bool ZEQuaternion::operator!=(const ZEQuaternion& RightOperand) const
{
	return ((ZEMath::Abs(this->x - RightOperand.x) > ZE_ZERO_THRESHOLD) || 
			(ZEMath::Abs(this->y - RightOperand.y) > ZE_ZERO_THRESHOLD) ||
			(ZEMath::Abs(this->z - RightOperand.z) > ZE_ZERO_THRESHOLD) ||
			(ZEMath::Abs(this->w - RightOperand.w) > ZE_ZERO_THRESHOLD));
}

float ZEQuaternion::operator[](ZESize Index) const
{
	return M[Index];
}

float& ZEQuaternion::operator[](ZESize Index)
{
	return M[Index];
}

ZEQuaternion::ZEQuaternion(float w, float x, float y, float z)
{
	Create(*this, w, x, y, z);
}

ZEQuaternion::ZEQuaternion(float Angle, const ZEVector3& Axis)
{
	CreateFromAngleAxis(*this, Angle, Axis);
}

ZEQuaternion::ZEQuaternion()
{
}
