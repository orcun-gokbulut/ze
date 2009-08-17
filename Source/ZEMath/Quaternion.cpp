//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Quaternion.cpp
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

#include "quaternion.h"
#include <math.h>

const ZEQuaternion ZEQuaternion::Zero = ZEQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
const ZEQuaternion ZEQuaternion::Identity = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

void ZEQuaternion::Create(ZEQuaternion& Output, float w, float x, float y, float z)
{
	Output.w = w;
	Output.x = x;
	Output.y = y;
	Output.z = z;
}


void ZEQuaternion::Create(ZEQuaternion& Output, float Angle, const ZEVector3& Axis)
{
	float SinAngle = sinf(Angle/2);
	Output.x = Axis.x * SinAngle;
	Output.y = Axis.y * SinAngle;
	Output.z = Axis.z * SinAngle;
	Output.w = cos(Angle/2);
}

void ZEQuaternion::Create(ZEQuaternion& Output, float Pitch, float Yawn, float Roll)
{
       float SinPitch		= sinf(Pitch * 0.5f);
       float CosPitch		= cosf(Pitch * 0.5f);
       float SinYaw			= sinf(Yawn * 0.5f);
       float CosYaw			= cosf(Yawn * 0.5f);
       float SinRoll		= sinf(Roll * 0.5f);
       float CosRoll		= cosf(Roll * 0.5f);
       float CosPitchCosYaw	= CosPitch * CosYaw;
       float SinPitchSinYaw	= SinPitch * SinYaw;
      
	   Output.w = CosRoll * CosPitchCosYaw + SinRoll * SinPitchSinYaw;
       Output.x = CosRoll * SinPitch * CosYaw + SinRoll * CosPitch * SinYaw;
       Output.y = CosRoll * CosPitch * SinYaw - SinRoll * SinPitch * CosYaw;
       Output.z = SinRoll * CosPitchCosYaw - CosRoll * SinPitchSinYaw;
}

void ZEQuaternion::Create(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up)
{
	 ZEVector3 Right, NewUp;
	 ZEVector3::CrossProduct(Right, Up, Direction);   
	 ZEVector3::CrossProduct(NewUp, Direction, Right);

	 /*Matrix mBasis = new Matrix(
		 vRight.X, vRight.Y, vRight.Z, 0.0f,
		 vUp.X, vUp.Y, vUp.Z, 0.0f,
		 vDirection.X, vDirection.Y, vDirection.Z, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f); */

	 Output.w = sqrtf(1.0f + Right.x + NewUp.y + Direction.z) / 2.0f;
	 float Scale = Output.w * 4.0;

/*	 qrot.X = (mBasis.M32 - mBasis.M23) / Scale;
	 qrot.Y = (mBasis.M13 - mBasis.M31) / Scale;
	 qrot.Z = (mBasis.M21 - mBasis.M12) / Scale;*/

	 Output.x = (Direction.y - NewUp.z) / Scale;
	 Output.y = (Right.z - Direction.x) / Scale;
	 Output.z = (NewUp.x - Right.y) / Scale;
}


void ZEQuaternion::CreateIdentity(ZEQuaternion& Output)
{
	Output.w = 1.0f;
	Output.x = 0.0f;
	Output.y = 0.0f;
	Output.z = 0.0f;
}

void ZEQuaternion::Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B)
{
	Output.w = A.w * B.w - A.x * B.x - A.y * B.y - A.z * B.z;
	Output.x = A.w * B.x + A.x * B.w + A.y * B.z - A.z * B.y;
	Output.y = A.w * B.y - A.x * B.z + A.y * B.w + A.z * B.x;
	Output.z = A.w * B.z + A.x * B.y - A.y * B.x + A.z * B.w;
}

void ZEQuaternion::VectorProduct(ZEVector3& Output, const ZEQuaternion& Quaternion, const ZEVector3& Vector)
{
	ZEQuaternion Vect(0, Vector.x, Vector.y, Vector.z), Temp, InvQ;
	
	Product(Temp, Quaternion, Vect);
	
	Conjugate(InvQ, Quaternion);
	Product(Vect, Temp, InvQ);
	Output.x = Vect.x;
	Output.y = Vect.y;
	Output.z = Vect.z;
}

void ZEQuaternion::ConvertToEulerAngles(float &Pitch, float &Yaw, float &Roll, const ZEQuaternion& Quaternion)
{
	float test = Quaternion.x * Quaternion.y + Quaternion.z * Quaternion.w;
	if (test > 0.499) 
	{ // singularity at north pole
		Yaw = 2 * atan2(Quaternion.x, Quaternion.w);
		Pitch = ZE_PI_2;
		Roll = 0;
		return;
	}	

	if (test < -0.499) 
	{ // singularity at south pole
		Yaw = -2 * atan2(Quaternion.x, Quaternion.w);
		Pitch = - ZE_PI_2;
		Roll = 0;
		return;
	}  

	float sqx = Quaternion.x * Quaternion.x;    
	float sqy = Quaternion.y * Quaternion.y;    
	float sqz = Quaternion.z * Quaternion.z;
    Yaw = atan2(2 * Quaternion.y * Quaternion.w - 2 * Quaternion.x * Quaternion.z , 1 - 2 * sqy - 2 * sqz);
	Pitch = asin(2 * test);
	Roll = atan2(2 * Quaternion.x * Quaternion.w - 2 * Quaternion.y * Quaternion.z , 1 - 2 * sqx - 2 * sqz);
}

void ZEQuaternion::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
}

void ZEQuaternion::Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion)
{
	Output.x = -Quaternion.x;
	Output.y = -Quaternion.y;
	Output.z = -Quaternion.z;
	Output.w = Quaternion.w;
}


void ZEQuaternion::Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor)
{
	float CosHalfTheta = A.w * B.w + A.x * B.x + A.y * B.y + A.z * B.z;

	if (fabs(CosHalfTheta) >= 1.0)
	{
		Output.w = A.w;
		Output.x = A.x;
		Output.y = A.y;
		Output.z = A.z;
	}
	else
	{
		float HalfTheta = acos(CosHalfTheta);
		float SinHalfTheta = sqrt(1.0f - CosHalfTheta * CosHalfTheta);

		if (fabs(SinHalfTheta) < 0.001)
		{ 
			Output.w = (A.w * 0.5f + B.w * 0.5f);
			Output.x = (A.x * 0.5f + B.x * 0.5f);
			Output.y = (A.y * 0.5f + B.y * 0.5f);
			Output.z = (A.z * 0.5f + B.z * 0.5f);
		}
		else
		{
			float RatioA = sin((1.0f - Factor) * HalfTheta) / SinHalfTheta;
			float RatioB = sin(Factor * HalfTheta) / SinHalfTheta; 

			Output.w = (A.w * RatioA + B.w * RatioB);
			Output.x = (A.x * RatioA + B.x * RatioB);
			Output.y = (A.y * RatioA + B.y * RatioB);
			Output.z = (A.z * RatioA + B.z * RatioB);
		}
	}
}

void ZEQuaternion::ConvertToRotationMatrix(ZEMatrix4x4& Output, const ZEQuaternion& Quaternion)
{
	/*
	Output.M11 = 1.0f	-	2.0f * Quaternion.y * Quaternion.y	-	2.0f * Quaternion.z * Quaternion.z;
	Output.M12 =			2.0f * Quaternion.x * Quaternion.y	-	2.0f * Quaternion.w * Quaternion.z;
	Output.M13 =			2.0f * Quaternion.x * Quaternion.z	+	2.0f * Quaternion.w * Quaternion.y;
	Output.M14 = 0.0f;

	Output.M21 =			2.0f * Quaternion.x * Quaternion.y	+	2.0f * Quaternion.w * Quaternion.z;
	Output.M22 = 1.0f	-	2.0f * Quaternion.x * Quaternion.x	-	2.0f * Quaternion.z * Quaternion.z;
	Output.M23 =			2.0f * Quaternion.y * Quaternion.z	-	2.0f * Quaternion.w * Quaternion.x;
	Output.M24 = 0.0f;

	Output.M31 =			2.0f * Quaternion.x * Quaternion.z	-	2.0f * Quaternion.w * Quaternion.y;
	Output.M32 =			2.0f * Quaternion.y * Quaternion.z	+	2.0f * Quaternion.w * Quaternion.x;
	Output.M33 = 1.0f	-	2.0f * Quaternion.x * Quaternion.x	-	2.0f * Quaternion.y * Quaternion.y;
	Output.M34 = 0.0f;

	Output.M41 = 0.0f;
	Output.M42 = 0.0f;
	Output.M43 = 0.0f;
	Output.M44 = 1.0f;
*/
	Output.M11 = 1.0f	-	2.0f * Quaternion.y * Quaternion.y	-	2.0f * Quaternion.z * Quaternion.z;
	Output.M21 =			2.0f * Quaternion.x * Quaternion.y	-	2.0f * Quaternion.w * Quaternion.z;
	Output.M31 =			2.0f * Quaternion.x * Quaternion.z	+	2.0f * Quaternion.w * Quaternion.y;
	Output.M41 = 0.0f;

	Output.M12 =			2.0f * Quaternion.x * Quaternion.y	+	2.0f * Quaternion.w * Quaternion.z;
	Output.M22 = 1.0f	-	2.0f * Quaternion.x * Quaternion.x	-	2.0f * Quaternion.z * Quaternion.z;
	Output.M32 =			2.0f * Quaternion.y * Quaternion.z	-	2.0f * Quaternion.w * Quaternion.x;
	Output.M42 = 0.0f;

	Output.M13 =			2.0f * Quaternion.x * Quaternion.z	-	2.0f * Quaternion.w * Quaternion.y;
	Output.M23 =			2.0f * Quaternion.y * Quaternion.z	+	2.0f * Quaternion.w * Quaternion.x;
	Output.M33 = 1.0f	-	2.0f * Quaternion.x * Quaternion.x	-	2.0f * Quaternion.y * Quaternion.y;
	Output.M43 = 0.0f;

	Output.M14 = 0.0f;
	Output.M24 = 0.0f;
	Output.M34 = 0.0f;
	Output.M44 = 1.0f;

}

void ZEQuaternion::Normalize()
{
	float L = sqrt(x*x + y*y + z*z + w*w);
	x = x / L;
	y = y / L;
	z = z / L;
	w = w / L;
	
}

void ZEQuaternion::Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion)
{
	float L = sqrt(Quaternion.x*Quaternion.x + Quaternion.y*Quaternion.y + Quaternion.z*Quaternion.z + Quaternion.w*Quaternion.w);
	Output.x = Quaternion.x / L;
	Output.y = Quaternion.y / L;
	Output.z = Quaternion.z / L;
	Output.w = Quaternion.w / L;	
}

ZEQuaternion ZEQuaternion::operator*(const ZEQuaternion& Other) const
{
	ZEQuaternion Temp;
	Product(Temp, *this, Temp);
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

ZEQuaternion::ZEQuaternion(float w, float x, float y, float z)
{
	Create(*this, w, x, y, z);
}

ZEQuaternion::ZEQuaternion(float Angle, const ZEVector3& Axis)
{
	Create(*this, Angle, Axis);
}

ZEQuaternion::ZEQuaternion()
{
}

