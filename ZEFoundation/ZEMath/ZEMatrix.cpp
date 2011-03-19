//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrix.cpp
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

#include "ZEMatrix.h"
#include "ZEVector.h"
#include "ZEQuaternion.h"
#include "ZEMathDefinitions.h"
#include <memory.h>
#include <math.h>

#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp

// ZEMatrix3x3
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
	Matrix.M23 =			2.0f * Rotation.y * Rotation.z	-	2.0f * Rotation.w * Rotation.x;

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
	
	Out.M21 = A.M21 + B.M21;
	Out.M22 = A.M22 + B.M22;
	Out.M23 = A.M23 + B.M23;
	
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

void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix3x3::Determinant(float &det,const ZEMatrix3x3 &Matrix)
{
	float a, b, c, d, e, f, det1, det2;
	a= Matrix.M11 * Matrix.M22 * Matrix.M33;
	b= Matrix.M12 * Matrix.M23 * Matrix.M31;
	c= Matrix.M13 * Matrix.M21 * Matrix.M32;
	d= Matrix.M13 * Matrix.M22 * Matrix.M31;
	e= Matrix.M12 * Matrix.M21 * Matrix.M33;
	f= Matrix.M11 * Matrix.M23 * Matrix.M32;
	det1 =  a + b + c ;
	det2 = - d - e - f;
	det = det1 + det2;
} 

bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)
{
	float a;
	float detx;
	ZEMatrix3x3::Determinant(a, Matrix);
	if (a == 0)
		return false;

	ZEMatrix3x3 X;
	ZEMatrix3x3::Create(X,1,Matrix.M12,Matrix.M13,0,Matrix.M22,Matrix.M23,0,Matrix.M32,Matrix.M33);
	ZEMatrix3x3::Determinant(detx,X);
	float x=detx/a;

	float dety;
	ZEMatrix3x3 Y;
	ZEMatrix3x3::Create(Y,Matrix.M11,1,Matrix.M13,Matrix.M21,0,Matrix.M23,Matrix.M31,0,Matrix.M33);
	ZEMatrix3x3::Determinant(dety,Y);
	float y=dety/a;
	
	float detz;
	ZEMatrix3x3 Z;
	ZEMatrix3x3::Create(Z,Matrix.M11,Matrix.M12,1,Matrix.M21,Matrix.M22,0,Matrix.M31,Matrix.M32,0);
	ZEMatrix3x3::Determinant(detz,Z);
	float z=detz/a;


	float detk;
	ZEMatrix3x3 K;
	ZEMatrix3x3::Create(K,0,Matrix.M12,Matrix.M13,1,Matrix.M22,Matrix.M23,0,Matrix.M32,Matrix.M33);
	ZEMatrix3x3::Determinant(detk,K);
	float k=detk/a;

	float detl;
	ZEMatrix3x3 L;
	ZEMatrix3x3::Create(L,Matrix.M11,0,Matrix.M13,Matrix.M21,1,Matrix.M23,Matrix.M31,0,Matrix.M33);
	ZEMatrix3x3::Determinant(detl,L);
	float l=detl/a;

	float detm;
	ZEMatrix3x3 M;
	ZEMatrix3x3::Create(M,Matrix.M11,Matrix.M12,0,Matrix.M21,Matrix.M22,1,Matrix.M31,Matrix.M32,0);
	ZEMatrix3x3::Determinant(detm,M);
	float m=detm/a;


    float deto;
	ZEMatrix3x3 O;
	ZEMatrix3x3::Create(O,0,Matrix.M12,Matrix.M13,0,Matrix.M22,Matrix.M23,1,Matrix.M32,Matrix.M33);
	ZEMatrix3x3::Determinant(deto,O);
	float o=deto/a;

	float detp;
	ZEMatrix3x3 P;
	ZEMatrix3x3::Create(P,Matrix.M11,0,Matrix.M13,Matrix.M21,0,Matrix.M23,Matrix.M31,1,Matrix.M33);
	ZEMatrix3x3::Determinant(detp,P);
	float p=detp/a;

	float detq;
	ZEMatrix3x3 Q;
	ZEMatrix3x3::Create(Q,Matrix.M11,Matrix.M12,0,Matrix.M21,Matrix.M22,0,Matrix.M31,Matrix.M32,1);
	ZEMatrix3x3::Determinant(detq,Q);
	float q=detq/a;

	Out.M11=x; Out.M12=k; Out.M13=o;
	Out.M21=y; Out.M22=l; Out.M23=p;
	Out.M31=z; Out.M32=m; Out.M33=q;

	return true;
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
	for (size_t I = 0; I < 9; I++)
		if (fabs(MA[I] - M.MA[I]) > ZE_ZERO_TRESHOLD)
			return false;

	return true;
}

bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const 
{
	for (size_t I = 0; I < 9; I++)
		if (fabs(MA[I] - M.MA[I]) > ZE_ZERO_TRESHOLD)
			return true;

	return true;
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


// ZEMatrix4x4
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
			x   ,    y,    z, 1.0f );
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{	
	ZEMatrix4x4 A, B, C, D;

	ZEMatrix4x4::CreateScale(A, Scale);
	ZEMatrix4x4::CreateRotation(B, Rotation);
	ZEMatrix4x4::CreateTranslation(C, Position);
	ZEMatrix4x4::Multiply(D, A, B);
	ZEMatrix4x4::Multiply(Matrix, D, C);
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
	ZEMatrix4x4 A, B;

	ZEMatrix4x4::CreateRotation(A, Rotation);
	ZEMatrix4x4::CreateTranslation(B, Position);
	ZEMatrix4x4::Multiply(Matrix, A, B);
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, const ZEVector3& Position)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			Position.x, Position.y, Position.z, 1.0f );
}

void ZEMatrix4x4::CreateOffset(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
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

void ZEMatrix4x4::CreateViewTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
	ZEMatrix4x4 ViewPositionTransform, ViewRotationTransform;
	ZEQuaternion ViewRotation;
	ZEQuaternion::Conjugate(ViewRotation, Rotation);
	ZEMatrix4x4::CreateTranslation(ViewPositionTransform, -Position.x, -Position.y, -Position.z);
	ZEMatrix4x4::CreateRotation(ViewRotationTransform, ViewRotation);
	ZEMatrix4x4::Multiply(Matrix, ViewPositionTransform, ViewRotationTransform);
	
}

void ZEMatrix4x4::CreateLookAtTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEVector3& Target, const ZEVector3& Up)
{
	ZEVector3 XAxis;
	ZEVector3 YAxis;
	ZEVector3 ZAxis;

	ZEVector3::Normalize(ZAxis, Target - Position);
	ZEVector3::CrossProduct(XAxis, Up, ZAxis);
	ZEVector3::Normalize(XAxis, XAxis);
	ZEVector3::CrossProduct(YAxis, ZAxis, XAxis);

	ZEMatrix4x4::Create(Matrix, XAxis.x, YAxis.x, ZAxis.x, 0.0f,
		XAxis.y, YAxis.y, ZAxis.y, 0.0f,
		XAxis.z, YAxis.z, ZAxis.z, 0.0f,
		-ZEVector3::DotProduct(XAxis, Position), 
		-ZEVector3::DotProduct(YAxis, Position), 
		-ZEVector3::DotProduct(ZAxis, Position), 1.0f);
}

void ZEMatrix4x4::CreateOrthographicProjection(ZEMatrix4x4& Matrix, float Width, float Height, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/Width, 0.0f, 0.0f, 0.0f, 
			0.0f, 2.0f/Height, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), 0.0f, 
			0.0f, 0.0f, NearZ/(NearZ - FarZ), 1.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjectionLinearZ(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float Ys = 1.0f / tanf(FOV * 0.5f);
	float Xs = Ys / AspectRatio;

	Create(Matrix, 
			(FarZ - NearZ) * Xs, 0.0f, 0.0f, 0.0f,
			0.0f, (FarZ - NearZ) * Ys, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, (FarZ - NearZ),
			0.0f, 0.0f, -NearZ, 0.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjection(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float Ys = 1.0f / tanf(FOV * 0.5f);
	float Xs = Ys / AspectRatio;

	Create(Matrix, 
		Xs, 0.0f, 0.0f, 0.0f,
		0.0f, Ys, 0.0f, 0.0f,
		0.0f, 0.0f, FarZ / (FarZ - NearZ), 1.0f,
		0.0f, 0.0f, -NearZ * FarZ / (FarZ - NearZ), 0.0f);
}

void ZEMatrix4x4::CreateViewPortTransform(ZEMatrix4x4& Matrix, float Left, float Right, float Top, float Bottom, float NearZ, float FarZ)
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
	
	Out.M21 = A.M21 + B.M21;
	Out.M22 = A.M22 + B.M22;
	Out.M23 = A.M23 + B.M23;
	Out.M24 = A.M24 + B.M24;
	
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

void ZEMatrix4x4::Transform(ZEVector3& Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z + Matrix.M41;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z + Matrix.M42;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z + Matrix.M43;
}

void ZEMatrix4x4::Transform3x3(ZEVector3 &Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix4x4::Transform(ZEVector4& Out, const ZEMatrix4x4& Matrix, const ZEVector4& Vector)
{
	Out.x = Matrix.M11 * Vector.x + Matrix.M21 * Vector.y + Matrix.M31 * Vector.z + Matrix.M41;
	Out.y = Matrix.M12 * Vector.x + Matrix.M22 * Vector.y + Matrix.M32 * Vector.z + Matrix.M42;
	Out.z = Matrix.M13 * Vector.x + Matrix.M23 * Vector.y + Matrix.M33 * Vector.z + Matrix.M43;
	Out.w = Matrix.M14 * Vector.x + Matrix.M24 * Vector.y + Matrix.M34 * Vector.z + Matrix.M44;
}

void ZEMatrix4x4::Determinant(float &det,const ZEMatrix4x4 &Matrix)
{
	float d1,d2,d3,d4;
	ZEMatrix3x3 A,B,C,D;
	ZEMatrix3x3::Create(A,Matrix.M22,Matrix.M23,Matrix.M24,Matrix.M32,Matrix.M33,Matrix.M34,Matrix.M42,Matrix.M43,Matrix.M44);
	ZEMatrix3x3::Create(B,Matrix.M21,Matrix.M23,Matrix.M24,Matrix.M31,Matrix.M33,Matrix.M34,Matrix.M41,Matrix.M43,Matrix.M44);
    ZEMatrix3x3::Create(C,Matrix.M21,Matrix.M22,Matrix.M24,Matrix.M31,Matrix.M32,Matrix.M34,Matrix.M41,Matrix.M42,Matrix.M44);
	ZEMatrix3x3::Create(D,Matrix.M21,Matrix.M22,Matrix.M23,Matrix.M31,Matrix.M32,Matrix.M33,Matrix.M41,Matrix.M42,Matrix.M43);

	ZEMatrix3x3::Determinant(d1,A);
	ZEMatrix3x3::Determinant(d2,B);
	ZEMatrix3x3::Determinant(d3,C);
	ZEMatrix3x3::Determinant(d4,D);

	det = Matrix.M11 * d1 + Matrix.M13 * d3 - Matrix.M12 * d2 - Matrix.M14 * d4;
	
	
}

bool ZEMatrix4x4::Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix)
{
	/*D3DXMATRIX D3DMatrix(Matrix.M11, Matrix.M12, Matrix.M13, Matrix.M14,
						Matrix.M21, Matrix.M22, Matrix.M23, Matrix.M24,
						Matrix.M31, Matrix.M32, Matrix.M33, Matrix.M34,
						Matrix.M41, Matrix.M42, Matrix.M43, Matrix.M44);
	D3DXMATRIX D3DOut;
	float Det;
	D3DXMatrixInverse(&D3DOut, &Det, &D3DMatrix);

	ZEMatrix4x4::Create(Out, D3DOut._11, D3DOut._12, D3DOut._13, D3DOut._14,
			D3DOut._21, D3DOut._22, D3DOut._23, D3DOut._24,
			D3DOut._31, D3DOut._32, D3DOut._33, D3DOut._34,
			D3DOut._41, D3DOut._42, D3DOut._43, D3DOut._44);

	return true;*/

	float a;
	ZEMatrix4x4::Determinant(a, Matrix);
	if(a == 0)
		return false;

	float detx;
	ZEMatrix4x4 X;
	ZEMatrix4x4::Create(X, 1, Matrix.M12, Matrix.M13, Matrix.M14, 
						   0, Matrix.M22, Matrix.M23, Matrix.M24,
						   0, Matrix.M32, Matrix.M33, Matrix.M34, 
						   0, Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix4x4::Determinant(detx, X);
	float x = detx/a;

	float dety;
	ZEMatrix4x4 Y;
	ZEMatrix4x4::Create(Y,Matrix.M11,1,Matrix.M13,Matrix.M14,
		                  Matrix.M21,0,Matrix.M23,Matrix.M24,
						  Matrix.M31,0,Matrix.M33,Matrix.M34,
						  Matrix.M41,0,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(dety,Y);
	float y= dety/a;

	float detz;
	ZEMatrix4x4 Z;
	ZEMatrix4x4::Create(Z,Matrix.M11,Matrix.M12,1,Matrix.M14,
		                  Matrix.M21,Matrix.M22,0,Matrix.M24,
						  Matrix.M31,Matrix.M32,0,Matrix.M34,
						  Matrix.M41,Matrix.M42,0,Matrix.M44);
	ZEMatrix4x4::Determinant(detz,Z);
	float z= detz/a;

	float detw;
	ZEMatrix4x4 W;
	ZEMatrix4x4::Create(W,Matrix.M11,Matrix.M12,Matrix.M13,1,
		                  Matrix.M21,Matrix.M22,Matrix.M23,0,
						  Matrix.M31,Matrix.M32,Matrix.M33,0,
						  Matrix.M41,Matrix.M42,Matrix.M43,0);
	ZEMatrix4x4::Determinant(detw,W);
	float w= detw/a;


	float detk;
	ZEMatrix4x4 K;
	ZEMatrix4x4::Create(K,0,Matrix.M12,Matrix.M13,Matrix.M14,1,Matrix.M22,Matrix.M23,Matrix.M24,0,Matrix.M32,Matrix.M33,Matrix.M34,0,Matrix.M42,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(detk,K);
	float k=detk/a;

	float detl;
	ZEMatrix4x4 L;
	ZEMatrix4x4::Create(L,Matrix.M11,0,Matrix.M13,Matrix.M14,Matrix.M21,1,Matrix.M23,Matrix.M24,Matrix.M31,0,Matrix.M33,Matrix.M34,Matrix.M41,0,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(detl,L);
	float l=detl/a;

	float detm;
	ZEMatrix4x4 M;
	ZEMatrix4x4::Create(M,Matrix.M11,Matrix.M12,0,Matrix.M14,Matrix.M21,Matrix.M22,1,Matrix.M24,Matrix.M31,Matrix.M32,0,Matrix.M34,Matrix.M41,Matrix.M42,0,Matrix.M44);
	ZEMatrix4x4::Determinant(detm,M);
	float m=detm/a;

	float detn;
	ZEMatrix4x4 N;
	ZEMatrix4x4::Create(N,Matrix.M11,Matrix.M12,Matrix.M13,0,Matrix.M21,Matrix.M22,Matrix.M23,1,Matrix.M31,Matrix.M32,Matrix.M33,0,Matrix.M41,Matrix.M42,Matrix.M43,0);
	ZEMatrix4x4::Determinant(detn,N);
	float n=detn/a;

	
	float deto;
	ZEMatrix4x4 O;
	ZEMatrix4x4::Create(O,0,Matrix.M12,Matrix.M13,Matrix.M14,0,Matrix.M22,Matrix.M23,Matrix.M24,1,Matrix.M32,Matrix.M33,Matrix.M34,0,Matrix.M42,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(deto,O);
	float o=deto/a;

	float detp;
	ZEMatrix4x4 P;
	ZEMatrix4x4::Create(P,Matrix.M11,0,Matrix.M13,Matrix.M14,Matrix.M21,0,Matrix.M23,Matrix.M24,Matrix.M31,1,Matrix.M33,Matrix.M34,Matrix.M41,0,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(detp,P);
	float p=detp/a;

	float detq;
	ZEMatrix4x4 Q;
	ZEMatrix4x4::Create(Q,Matrix.M11,Matrix.M12,0,Matrix.M14,Matrix.M21,Matrix.M22,0,Matrix.M24,Matrix.M31,Matrix.M32,1,Matrix.M34,Matrix.M41,Matrix.M42,0,Matrix.M44);
	ZEMatrix4x4::Determinant(detq,Q);
	float q=detq/a;

	float detr;
	ZEMatrix4x4 R;
	ZEMatrix4x4::Create(R,Matrix.M11,Matrix.M12,Matrix.M13,0,Matrix.M21,Matrix.M22,Matrix.M23,0,Matrix.M31,Matrix.M32,Matrix.M33,1,Matrix.M41,Matrix.M42,Matrix.M43,0);
	ZEMatrix4x4::Determinant(detr,R);
	float r=detr/a;


	float dets;
	ZEMatrix4x4 S;
	ZEMatrix4x4::Create(S,0,Matrix.M12,Matrix.M13,Matrix.M14,0,Matrix.M22,Matrix.M23,Matrix.M24,0,Matrix.M32,Matrix.M33,Matrix.M34,1,Matrix.M42,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(dets,S);
	float s=dets/a;

	float dett;
	ZEMatrix4x4 T;
	ZEMatrix4x4::Create(T,Matrix.M11,0,Matrix.M13,Matrix.M14,Matrix.M21,0,Matrix.M23,Matrix.M24,Matrix.M31,0,Matrix.M33,Matrix.M34,Matrix.M41,1,Matrix.M43,Matrix.M44);
	ZEMatrix4x4::Determinant(dett,T);
	float t=dett/a;

	float detu;
	ZEMatrix4x4 U;
	ZEMatrix4x4::Create(U,Matrix.M11,Matrix.M12,0,Matrix.M14,Matrix.M21,Matrix.M22,0,Matrix.M24,Matrix.M31,Matrix.M32,0,Matrix.M34,Matrix.M41,Matrix.M42,1,Matrix.M44);
	ZEMatrix4x4::Determinant(detu,U);
	float u=detu/a;

	float detv;
	ZEMatrix4x4 V;
	ZEMatrix4x4::Create(V,Matrix.M11,Matrix.M12,Matrix.M13,0,Matrix.M21,Matrix.M22,Matrix.M23,0,Matrix.M31,Matrix.M32,Matrix.M33,0,Matrix.M41,Matrix.M42,Matrix.M43,1);
	ZEMatrix4x4::Determinant(detv,V);
	float v=detv/a;
	ZEMatrix4x4::Create(Out,x,k,o,s,y,l,p,t,z,m,q,u,w,n,r,v);

	return true;
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

ZEVector3 ZEMatrix4x4::operator*(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEMatrix4x4::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEMatrix4x4::operator*(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEMatrix4x4::Transform(Temp, *this, RightOperand);
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
	for (size_t I = 0; I < 16; I++)
		if (fabs(MA[I] - M.MA[I]) > ZE_ZERO_TRESHOLD)
			return false;

	return true;
}

bool ZEMatrix4x4::operator != (const ZEMatrix4x4 &M) const 
{
	for (size_t I = 0; I < 16; I++)
		if (fabs(MA[I] - M.MA[I]) > ZE_ZERO_TRESHOLD)
			return true;

	return false;
}

float ZEMatrix4x4::operator[](size_t Index) const
{
	return MA[Index];
}

float& ZEMatrix4x4::operator[](size_t Index)
{
	return MA[Index];
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
