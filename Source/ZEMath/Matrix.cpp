//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Matrix.cpp
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

#include "matrix.h"
#include "vector.h"
#include "quaternion.h"
#include <memory.h>
#include <math.h>
#include <d3dx9.h>

#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3  ZEMatrix3x3 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEMatrix3x3 ZEMatrix3x3::Zero = ZEMatrix3x3(0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f);

const ZEMatrix3x3 ZEMatrix3x3::Identity = ZEMatrix3x3(1.0f, 0.0f, 0.0f,
													  0.0f, 1.0f, 0.0f,
													  0.0f, 0.0f, 1.0f);

inline void ZEMatrix3x3::Create(ZEMatrix3x3 &Matrix, float M11, float M12, float M13,
														float M21, float M22, float M23,
														float M31, float M32, float M33)
{
	Matrix.M11 = M11; Matrix.M12 = M12; Matrix.M13 = M13;
	Matrix.M21 = M21; Matrix.M22 = M22; Matrix.M23 = M23;
	Matrix.M31 = M31; Matrix.M32 = M32; Matrix.M33 = M33;
}

inline void ZEMatrix3x3::Create(ZEMatrix3x3& Matrix, float M[9])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix3x3));
}

void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, const ZEQuaternion& Rotation)
{
	Matrix.M11 = 1.0f	-	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M12 =			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z;
	Matrix.M13 =			2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y;


	Matrix.M21 =			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z;
	Matrix.M22 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z;
	;


	Matrix.M31 =			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y;
	Matrix.M32 =			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x;
	Matrix.M33 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y;
}

void ZEMatrix3x3::CreateRotationX(ZEMatrix3x3& Matrix, float Pitch)
{
	float Cos = cosf(Pitch);
	float Sin = sinf(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = Sin;

	Matrix.M31 = 0.0f;
	Matrix.M32 = -Sin;
	Matrix.M33 = Cos;

}

void ZEMatrix3x3::CreateRotationY(ZEMatrix3x3& Matrix, float Yawn)
{
	float Cos = cosf(Yawn);
	float Sin = sinf(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = -Sin;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;

	Matrix.M31 = Sin;
	Matrix.M32 = 0.0f;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3::CreateRotationZ(ZEMatrix3x3& Matrix, float Roll)
{
	float Cos = cosf(Roll);
	float Sin = sinf(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = Sin;
	Matrix.M13 = 0.0f;

	Matrix.M21 = -Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
}

void ZEMatrix3x3::CreateIdentity(ZEMatrix3x3& Matrix)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f);
}
void ZEMatrix3x3::CreateZero(ZEMatrix3x3& Matrix)
{
	Create(Matrix,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f);
}

void ZEMatrix3x3::CreateScale(ZEMatrix3x3& Matrix, float x, float y, float z)
{
	Create(Matrix,
			x, 0.0f, 0.0f,
			0.0f, y, 0.0f,
			0.0f, 0.0f, z);
}

inline void ZEMatrix3x3::Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	Out.M11 = A.M11 + B.M11;
	Out.M12 = A.M12 + B.M12;
	Out.M13 = A.M13 + B.M13;
	
	Out.M21 = A.M11 + B.M11;
	Out.M22 = A.M12 + B.M12;
	Out.M23 = A.M13 + B.M13;
	
	Out.M31 = A.M31 + B.M31;
	Out.M32 = A.M32 + B.M32;
	Out.M33 = A.M33 + B.M33;
}


inline void ZEMatrix3x3::Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	Out.M11 = A.M11 - B.M11;
	Out.M12 = A.M12 - B.M12;
	Out.M13 = A.M13 - B.M13;
	
	Out.M21 = A.M21 - B.M21;
	Out.M22 = A.M22 - B.M22;
	Out.M23 = A.M23 - B.M23;
	
	Out.M31 = A.M31 - B.M31;
	Out.M32 = A.M32 - B.M32;
	Out.M33 = A.M33 - B.M33;
}

inline void ZEMatrix3x3::Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	for (int I = 0; I < 3; I++)
		for (int J = 0; J < 3; J++)
		{
			Out.M[J][I] = 0.0f;
			for (int R = 0; R <3; R++)
				Out.M[J][I] += A.M[R][I] * B.M[J][R];
		}
}

inline void ZEMatrix3x3::Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3& A, float s)
{
	Out.M11 = A.M11 * s;
	Out.M12 = A.M12 * s;
	Out.M13 = A.M13 * s;

	Out.M21 = A.M21 * s;
	Out.M22 = A.M22 * s;
	Out.M23 = A.M23 * s;

	Out.M31 = A.M31 * s;
	Out.M32 = A.M32 * s;
	Out.M33 = A.M33 * s;
}

void ZEMatrix3x3::Transpose(ZEMatrix3x3& Out, const ZEMatrix3x3& Matrix)
{
	Out.M11 = Matrix.M11;
	Out.M12 = Matrix.M21;
	Out.M13 = Matrix.M31;
	
	Out.M21 = Matrix.M12;
	Out.M22 = Matrix.M22;
	Out.M23 = Matrix.M32;
	
	Out.M31 = Matrix.M13;
	Out.M32 = Matrix.M23;
	Out.M33 = Matrix.M33;
}

bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)
{
	float Det;
	return D3DXMatrixInverse((D3DXMATRIX*)&Out, &Det, (D3DXMATRIX*)&Matrix) != NULL;
}

void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEVector2& Vector, const ZEMatrix3x3 &Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32;
}

void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEVector3& Vector, const ZEMatrix3x3 &Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEVector4& Vector, const ZEMatrix3x3 &Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z;
	Out.w = 1.0f;
}

ZEMatrix3x3 ZEMatrix3x3::operator+(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator-(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator*(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator*(float S) 
{
	ZEMatrix3x3 Temp;
	Scale(Temp, *this, S);
	return Temp;
}


ZEMatrix3x3& ZEMatrix3x3::operator += (const ZEMatrix3x3 &RightOperand)
{
	Add(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3& ZEMatrix3x3::operator -= (const ZEMatrix3x3 &RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3& ZEMatrix3x3::operator *= (const ZEMatrix3x3 &RightOperand)
{
	ZEMatrix3x3 Temp;
	Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix3x3& ZEMatrix3x3::operator *= (float S)
{
	Scale(*this, *this, S);
	return *this;
}


bool ZEMatrix3x3::operator == (const ZEMatrix3x3 &M) const 
{
	return false;
}

bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const 
{
	return false;
}

ZEMatrix3x3::ZEMatrix3x3(float M00, float M01, float M02,
			float M10, float M11, float M12,
			float M20, float M21, float M22)
{
	Create(*this, M00, M01, M02, M10, M11, M12, M20, M21, M22);  
}

ZEMatrix3x3::ZEMatrix3x3(float M[9])
{
	Create(*this, M);
}

ZEMatrix3x3::ZEMatrix3x3()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4  ZEMatrix4x4 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const ZEMatrix4x4 ZEMatrix4x4::Zero = ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f);

const ZEMatrix4x4 ZEMatrix4x4::Identity = ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
													  0.0f, 1.0f, 0.0f, 0.0f,
													  0.0f, 0.0f, 1.0f, 0.0f,
													  0.0f, 0.0f, 0.0f, 1.0f);

inline void ZEMatrix4x4::Create(ZEMatrix4x4& Matrix,
				float M11, float M12, float M13, float M14,
				float M21, float M22, float M23, float M24,
				float M31, float M32, float M33, float M34,
				float M41, float M42, float M43, float M44)
{
	Matrix.M11 = M11; Matrix.M21 = M21; Matrix.M31 = M31; Matrix.M41 = M41;
	Matrix.M12 = M12; Matrix.M22 = M22; Matrix.M32 = M32; Matrix.M42 = M42;
	Matrix.M13 = M13; Matrix.M23 = M23; Matrix.M33 = M33; Matrix.M43 = M43;
	Matrix.M14 = M14; Matrix.M24 = M24; Matrix.M34 = M34; Matrix.M44 = M44;
}

inline void ZEMatrix4x4::Create(ZEMatrix4x4& Matrix, float M[16])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix4x4));
}

void ZEMatrix4x4::CreateRotation(ZEMatrix4x4& Matrix, const ZEQuaternion& Rotation)
{
	Matrix.M11 = 1.0f	-	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M21 =			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z;
	Matrix.M31 =			2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y;
	Matrix.M41 = 0.0f;

	Matrix.M12 =			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z;
	Matrix.M22 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M32 =			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x;
	Matrix.M42 = 0.0f;

	Matrix.M13 =			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y;
	Matrix.M23 =			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x;
	Matrix.M33 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y;
	Matrix.M43 = 0.0f;

	Matrix.M14 = 0.0f;
	Matrix.M24 = 0.0f;
	Matrix.M34 = 0.0f;
	Matrix.M44 = 1.0f;	
}

void ZEMatrix4x4::CreateRotationX(ZEMatrix4x4& Matrix, float Pitch)
{
	float Cos = cosf(Pitch);
	float Sin = sinf(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = Sin;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = -Sin;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotationY(ZEMatrix4x4& Matrix, float Yawn)
{
	float Cos = cosf(Yawn);
	float Sin = sinf(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = -Sin;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;
	Matrix.M24 = 0.0f;

	Matrix.M31 = Sin;
	Matrix.M32 = 0.0f;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotationZ(ZEMatrix4x4& Matrix, float Roll)
{
	float Cos = cosf(Roll);
	float Sin = sinf(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = Sin;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = -Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0f;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateScale(ZEMatrix4x4& Matrix, float x, float y, float z)
{
	Create(Matrix,
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
}


void ZEMatrix4x4::CreateScale(ZEMatrix4x4& Matrix, const ZEVector3& Scale)
{
	Create(Matrix,
			Scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, Scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, Scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, float x, float y, float z)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			x, y, z, 1.0f );
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{/*
	Matrix.M11 = (1.0f -	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z) * Scale.x;
	Matrix.M12 = (			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z) * Scale.x;
	Matrix.M13 = (	 		2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y) * Scale.x;
	Matrix.M14 = 0.0f;

	Matrix.M21 = (			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z) * Scale.y;
	Matrix.M22 = (1.0f -	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z) * Scale.y;
	Matrix.M23 = (			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x) * Scale.y;
	Matrix.M24 = 0.0f;

	Matrix.M31 = (			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y) * Scale.z;
	Matrix.M32 = (			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x) * Scale.z;
	Matrix.M33 = (1.0f -	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y) * Scale.z;
	Matrix.M34 = 0.0f;
*/
/*	Matrix.M11 = (1.0f -	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z) * Scale.x;
	Matrix.M21 = (			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z) * Scale.x;
	Matrix.M31 = (	 		2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y) * Scale.x;
	Matrix.M41 = Position.x;

	Matrix.M12 = (			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z) * Scale.y;
	Matrix.M22 = (1.0f -	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z) * Scale.y;
	Matrix.M32 = (			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x) * Scale.y;
	Matrix.M42 = Position.y;

	Matrix.M13 = (			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y) * Scale.z;
	Matrix.M23 = (			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x) * Scale.z;
	Matrix.M33 = (1.0f -	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y) * Scale.z;
	Matrix.M43 = Position.z;

	Matrix.M14 = 0.0f;
	Matrix.M24 = 0.0f;
	Matrix.M34 = 0.0f;
	Matrix.M44 = 1.0f;	*/

	
	ZEMatrix4x4 A, B, C, D;

	ZEMatrix4x4::CreateScale(A, Scale);
	ZEMatrix4x4::CreateRotation(B, Rotation);
	ZEMatrix4x4::CreateTranslation(C, Position);
	ZEMatrix4x4::Multiply(D, A, B);
	ZEMatrix4x4::Multiply(Matrix, D, C);
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, const ZEPoint3& Position)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			Position.x, Position.y, Position.z, 1.0f );
}

void ZEMatrix4x4::CreateOffset(ZEMatrix4x4& Matrix, const ZEPoint3& Position, const ZEQuaternion& Rotation)
{
	/*
	Matrix.M11 = 1.0f	-	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M12 =			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z;
	Matrix.M13 =			2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y;
	Matrix.M14 = 0.0f;

	Matrix.M21 =			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z;
	Matrix.M22 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M23 =			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x;
	Matrix.M24 = 0.0f;

	Matrix.M31 =			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y;
	Matrix.M32 =			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x;
	Matrix.M33 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y;
	Matrix.M34 = 0.0f;
	*/

	Matrix.M11 = 1.0f	-	2.0f * Rotation.y * Rotation.y	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M21 =			2.0f * Rotation.x * Rotation.y	-	2.0f * Rotation.w * Rotation.z;
	Matrix.M31 =			2.0f * Rotation.x * Rotation.z	+	2.0f * Rotation.w * Rotation.y;
	Matrix.M41 = Position.x;

	Matrix.M12 =			2.0f * Rotation.x * Rotation.y	+	2.0f * Rotation.w * Rotation.z;
	Matrix.M22 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.z * Rotation.z;
	Matrix.M32 =			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x;
	Matrix.M42 = Position.y;

	Matrix.M13 =			2.0f * Rotation.x * Rotation.z	-	2.0f * Rotation.w * Rotation.y;
	Matrix.M23 =			2.0f * Rotation.y * Rotation.z	+	2.0f * Rotation.w * Rotation.x;
	Matrix.M33 = 1.0f	-	2.0f * Rotation.x * Rotation.x	-	2.0f * Rotation.y * Rotation.y;
	Matrix.M43 = Position.z;

	Matrix.M14 = 0.0f;
	Matrix.M24 = 0.0f;
	Matrix.M34 = 0.0f;
	Matrix.M44 = 1.0f;	
}

void ZEMatrix4x4::CreateIdentity(ZEMatrix4x4& Matrix)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
}
void ZEMatrix4x4::CreateZero(ZEMatrix4x4& Matrix)
{
	Create(Matrix,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f );
}
void ZEMatrix4x4::CreateOrthographicProjection(ZEMatrix4x4& Matrix, float Width, float Height, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/Width, 0.0f, 0.0f, 0.0f, 
			0.0f, 2.0f/Height, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), 0.0f, 
			0.0f, 0.0f, NearZ/(NearZ - FarZ), 1.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjection(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float Ys = 1 / tanf(FOV / 2.0f);
	float Xs = Ys / AspectRatio;

	Create(Matrix, 
			Xs, 0.0f, 0.0f, 0.0f,
			0.0f, Ys, 0.0f, 0.0f,
			0.0f, 0.0f, FarZ / (FarZ - NearZ), 1.0f,
			0.0f, 0.0f, -NearZ * FarZ / (FarZ - NearZ), 0.0f);
}

void ZEMatrix4x4::CreateViewPortTransform(ZEMatrix4x4& Matrix, float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/(Right - Left), 0.0f, 0.0f, 0.0f, 
			0.0f, 2.0f/(Top - Bottom), 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), 0.0f, 
			(Left + Right)/(Left - Right), (Top + Bottom)/(Bottom - Top), NearZ/(NearZ - FarZ), 1.0f);
}

inline void ZEMatrix4x4::Add(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	Out.M11 = A.M11 + B.M11;
	Out.M12 = A.M12 + B.M12;
	Out.M13 = A.M13 + B.M13;
	Out.M14 = A.M14 + B.M14;
	
	Out.M21 = A.M11 + B.M11;
	Out.M22 = A.M12 + B.M12;
	Out.M23 = A.M13 + B.M13;
	Out.M24 = A.M14 + B.M14;
	
	Out.M31 = A.M31 + B.M31;
	Out.M32 = A.M32 + B.M32;
	Out.M33 = A.M33 + B.M33;
	Out.M34 = A.M34 + B.M34;
	
	Out.M41 = A.M41 + B.M41;
	Out.M42 = A.M42 + B.M42;
	Out.M43 = A.M43 + B.M43;
	Out.M44 = A.M44 + B.M44;
}


inline void ZEMatrix4x4::Sub(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	Out.M11 = A.M11 - B.M11;
	Out.M12 = A.M12 - B.M12;
	Out.M13 = A.M13 - B.M13;
	Out.M14 = A.M14 - B.M14;
	
	Out.M21 = A.M21 - B.M21;
	Out.M22 = A.M22 - B.M22;
	Out.M23 = A.M23 - B.M23;
	Out.M24 = A.M24 - B.M24;
	
	Out.M31 = A.M31 - B.M31;
	Out.M32 = A.M32 - B.M32;
	Out.M33 = A.M33 - B.M33;
	Out.M34 = A.M34 - B.M34;
	
	Out.M41 = A.M41 - B.M41;
	Out.M42 = A.M42 - B.M42;
	Out.M43 = A.M43 - B.M43;
	Out.M44 = A.M44 - B.M44;
}

inline void ZEMatrix4x4::Multiply(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	for (int I = 0; I < 4; I++)
		for (int J = 0; J < 4; J++)
		{
			Out.M[J][I] = 0.0f;
			for (int R = 0; R <4; R++)
				Out.M[J][I] += A.M[R][I] * B.M[J][R];
		}
}

inline void ZEMatrix4x4::Scale(ZEMatrix4x4 &Out, const ZEMatrix4x4& A, float s)
{
	Out.M11 = A.M11 * s;
	Out.M12 = A.M12 * s;
	Out.M13 = A.M13 * s;
	Out.M14 = A.M14 * s;

	Out.M21 = A.M21 * s;
	Out.M22 = A.M22 * s;
	Out.M23 = A.M23 * s;
	Out.M24 = A.M24 * s;

	Out.M31 = A.M31 * s;
	Out.M32 = A.M32 * s;
	Out.M33 = A.M33 * s;
	Out.M34 = A.M34 * s; 

	Out.M41 = A.M41 * s;
	Out.M42 = A.M42 * s;
	Out.M43 = A.M43 * s;
	Out.M44 = A.M44 * s;
}

void ZEMatrix4x4::Transpose(ZEMatrix4x4& Out, const ZEMatrix4x4& Matrix)
{
	Out.M11 = Matrix.M11;
	Out.M12 = Matrix.M21;
	Out.M13 = Matrix.M31;
	Out.M14 = Matrix.M41;
	
	Out.M21 = Matrix.M12;
	Out.M22 = Matrix.M22;
	Out.M23 = Matrix.M32;
	Out.M24 = Matrix.M42;
	
	Out.M31 = Matrix.M13;
	Out.M32 = Matrix.M23;
	Out.M33 = Matrix.M33;
	Out.M34 = Matrix.M43;

	Out.M41 = Matrix.M14;
	Out.M42 = Matrix.M24;
	Out.M43 = Matrix.M34;
	Out.M44 = Matrix.M44;
}

void ZEMatrix4x4::Transform(ZEVector3& Out, const ZEVector3& Vector, const ZEMatrix4x4& Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z + Matrix.M41;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z + Matrix.M42;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z + Matrix.M43;
}

void ZEMatrix4x4::Transform3x3(ZEVector3 &Out, const ZEVector3& Vector, const ZEMatrix4x4& Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix4x4::Transform(ZEVector4 &Out, const ZEVector4& Vector, const ZEMatrix4x4& Matrix)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z + Matrix.M41 * Vector.w;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z + Matrix.M42 * Vector.w;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z + Matrix.M43 * Vector.w;
	Out.w = Matrix.M14 * Vector.x + Matrix.M24 * Vector.y + Matrix.M34 * Vector.z + Matrix.M44 * Vector.w;
}

bool ZEMatrix4x4::Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix)
{
	float Det;
	return D3DXMatrixInverse((D3DXMATRIX*)&Out, &Det, (D3DXMATRIX*)&Matrix) != NULL;
}
ZEMatrix4x4 ZEMatrix4x4::operator+(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator-(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator*(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator*(float S) 
{
	ZEMatrix4x4 Temp;
	Scale(Temp, *this, S);
	return Temp;
}


void ZEMatrix4x4::operator += (const ZEMatrix4x4 &RightOperand)
{
	ZEMatrix4x4 Temp;
	Add(Temp, *this, RightOperand);
	*this = Temp;
}

void ZEMatrix4x4::operator -= (const ZEMatrix4x4 &RightOperand)
{
	Sub(*this, *this, RightOperand);
}

void ZEMatrix4x4::operator *= (const ZEMatrix4x4 &RightOperand)
{
	Multiply(*this, *this, RightOperand);
}

void ZEMatrix4x4::operator *= (float S)
{
	Scale(*this, *this, S);
}


bool ZEMatrix4x4::operator == (const ZEMatrix4x4 &M) const 
{
	return false;
}

bool ZEMatrix4x4::operator != (const ZEMatrix4x4 &M) const 
{
	return false;
}

ZEMatrix4x4::ZEMatrix4x4(float M11, float M12, float M13, float M14,
				float M21, float M22, float M23, float M24,
				float M31, float M32, float M33, float M34,
				float M41, float M42, float M43, float M44)
{
	Create(*this, M11, M12, M13, M14, M21, M22, M23, M24, M31, M32, M33, M34, M41, M42, M43, M44);  
}

ZEMatrix4x4::ZEMatrix4x4(float M[16])
{
	Create(*this, M);
}

ZEMatrix4x4::ZEMatrix4x4()
{
}
