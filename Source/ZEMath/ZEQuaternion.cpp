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
#include "ZEDefinitions.h"
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
	Output.w = cos(Angle/2);
	Output.x = Axis.x * SinAngle;
	Output.y = Axis.y * SinAngle;
	Output.z = Axis.z * SinAngle;
	
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
	 float Scale = Output.w * 4.0f;

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

void ZEQuaternion::ConvertToEulerAngles(float &Yaw, float &Pitch, float &Roll, const ZEQuaternion& Quaternion)
{
	float test = Quaternion.x * Quaternion.y + Quaternion.z * Quaternion.w;
	if (test > 0.499) 
	{ 
		// singularity at north pole
		Yaw = 2 * atan2(Quaternion.x, Quaternion.w);
		Pitch = ZE_PI_2;
		Roll = 0;
		return;
	}	

	if (test < -0.499) 
	{ 
		// singularity at south pole
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

void ZEQuaternion::ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion)
{
	ZEQuaternion::VectorProduct(Look, Quaternion, ZEVector3::UnitZ);
	ZEQuaternion::VectorProduct(Up, Quaternion, ZEVector3::UnitY);
}


ZEQuaternion ZEQuaternion::Conjugate() const
{
	ZEQuaternion Temp;
	
	Temp.x = -x;
	Temp.y = -y;
	Temp.z = -z;
	Temp.w = w;

	return Temp;
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
	float CosOmega = A.w * B.w + A.x * B.x + A.y * B.y + A.z * B.z;

	//***30.12.2009 tarihli ekleme***//
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

	if (fabs(CosOmega) > 0.9999f)  
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
		float SinOmega = sqrt(b);

		// Compute the angle from its sin and cosine
		float Omega = atan2(SinOmega, CosOmega);

		// Compute inverse of denominator, so we only have
		// to divide once
		float OneOverSinOmega = 1.0f / SinOmega;

		// Compute interpolation parameters
		float a = 1.0f - Factor;
		float c = a * Omega;
		float d = Factor * Omega;

		RatioA = sin(c) * OneOverSinOmega;
		RatioB = sin(d) * OneOverSinOmega;
	}

	// Interpolate
	Output.w = (A.w * RatioA + BTemp.w * RatioB);
	Output.x = (A.x * RatioA + BTemp.x * RatioB);
	Output.y = (A.y * RatioA + BTemp.y * RatioB);
	Output.z = (A.z * RatioA + BTemp.z * RatioB);
}

void ZEQuaternion::ConvertToRotationMatrix(ZEMatrix4x4& Output, const ZEQuaternion& Quaternion)
{
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

ZEQuaternion ZEQuaternion::Normalize() const
{
	ZEQuaternion Temp;
	
	float L = sqrt(x*x + y*y + z*z + w*w);
	
	Temp.x = x / L;
	Temp.y = y / L;
	Temp.z = z / L;
	Temp.w = w / L;
	
	return Temp;
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
	return ((fabs(this->x - RightOperand.x) < ZE_ZERO_TRESHOLD) && 
		(fabs(this->y - RightOperand.y) < ZE_ZERO_TRESHOLD) &&
		(fabs(this->z - RightOperand.z) < ZE_ZERO_TRESHOLD) &&
		(fabs(this->w - RightOperand.w) < ZE_ZERO_TRESHOLD));
}

bool ZEQuaternion::operator!=(const ZEQuaternion& RightOperand) const
{
	return ((fabs(this->x - RightOperand.x) > ZE_ZERO_TRESHOLD) || 
			(fabs(this->y - RightOperand.y) > ZE_ZERO_TRESHOLD) ||
			(fabs(this->z - RightOperand.z) > ZE_ZERO_TRESHOLD) ||
			(fabs(this->w - RightOperand.w) > ZE_ZERO_TRESHOLD));
}

float ZEQuaternion::operator[](size_t Index) const
{
	return M[Index];
}

float& ZEQuaternion::operator[](size_t Index)
{
	return M[Index];
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



