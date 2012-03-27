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
#include "ZEMath.h"
#include <memory.h>
#include "ZEMath/ZEAngle.h"
#include "ZEError.h"

#define ZE_MATRIX4X4_ROW_ELEMENT_COUNT		(ZESize)4
#define ZE_MATRIX4X4_COL_ELEMENT_COUNT		(ZESize)4

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
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;

	Matrix.M31 = 		2.0f * xz -	2.0f * yw;
	Matrix.M32 = 		2.0f * yz +	2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx -	2.0f * yy;
}

void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix3x3 XRotation, YRotation, ZRotation, Temp;

	ZEMatrix3x3::CreateRotationX(XRotation, Pitch);
	ZEMatrix3x3::CreateRotationY(YRotation, Yawn);
	ZEMatrix3x3::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
		case ZE_RO_XYZ:
			ZEMatrix3x3::Multiply(Temp, XRotation, YRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_XZY:
			ZEMatrix3x3::Multiply(Temp, XRotation, ZRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_YXZ:
			ZEMatrix3x3::Multiply(Temp, YRotation, XRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_YZX:
			ZEMatrix3x3::Multiply(Temp, YRotation, ZRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, XRotation);
			break;

		case ZE_RO_ZXY:
			ZEMatrix3x3::Multiply(Temp, ZRotation, XRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_ZYX:
			ZEMatrix3x3::Multiply(Temp, ZRotation, YRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, XRotation);
			break;
	}
}

void ZEMatrix3x3::CreateRotationX(ZEMatrix3x3& Matrix, float Pitch)
{
	float Cos = ZEAngle::Cos(Pitch);
	float Sin = ZEAngle::Sin(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;

	Matrix.M31 = 0.0f;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3::CreateRotationY(ZEMatrix3x3& Matrix, float Yawn)
{
	float Cos = ZEAngle::Cos(Yawn);
	float Sin = ZEAngle::Sin(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = Sin;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;

	Matrix.M31 = -Sin;
	Matrix.M32 = 0.0f;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3::CreateRotationZ(ZEMatrix3x3& Matrix, float Roll)
{
	float Cos = ZEAngle::Cos(Roll);
	float Sin = ZEAngle::Sin(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0f;

	Matrix.M21 = Sin;
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
	zeAssert(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeAssert(&Out == &B, "Parameters Out and B can not be the same variable.");

	for (ZEInt RowIndex = 0; RowIndex < 3; RowIndex++) // Row
		for (ZEInt ColumnIndex = 0; ColumnIndex < 3; ColumnIndex++) // Column
			Out.M[ColumnIndex][RowIndex] = 
			A.M[0][RowIndex] * B.M[ColumnIndex][0] + 
			A.M[1][RowIndex] * B.M[ColumnIndex][1] + 
			A.M[2][RowIndex] * B.M[ColumnIndex][2];
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
	zeAssert(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEMatrix3x3& Matrix, const ZEVector2& Vector)
{
	zeAssert(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23;
}

void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)
{
	zeAssert(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEMatrix3x3& Matrix, const ZEVector4& Vector)
{
	zeAssert(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
	Out.w = 1.0f;
}

float ZEMatrix3x3::Determinant(const ZEMatrix3x3 &Matrix)
{
	float a, b, c, d, e, f;

	a = Matrix.M11 * Matrix.M22 * Matrix.M33;
	b = Matrix.M12 * Matrix.M23 * Matrix.M31;
	c = Matrix.M13 * Matrix.M21 * Matrix.M32;
	d = Matrix.M13 * Matrix.M22 * Matrix.M31;
	e = Matrix.M12 * Matrix.M21 * Matrix.M33;
	f = Matrix.M11 * Matrix.M23 * Matrix.M32;

	return a + b + c - d - e - f;
} 

bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)
{
	zeAssert(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	float Determinant = ZEMatrix3x3::Determinant(Matrix);

	if (Determinant == 0)
		return false;

	ZEMatrix3x3 X;
	ZEMatrix3x3::Create(X,
		1.0f, Matrix.M12, Matrix.M13, 
		0.0f, Matrix.M22, Matrix.M23,
		0.0f, Matrix.M32, Matrix.M33);
	Out.M11 = X.Determinant() / Determinant;

	ZEMatrix3x3 Y;
	ZEMatrix3x3::Create(Y,
		Matrix.M11, 1.0f, Matrix.M13,
		Matrix.M21, 0.0f, Matrix.M23,
		Matrix.M31, 0.0f, Matrix.M33);
	Out.M21 = Y.Determinant() / Determinant;
	
	ZEMatrix3x3 Z;
	ZEMatrix3x3::Create(Z,
		Matrix.M11, Matrix.M12, 1.0f, 
		Matrix.M21, Matrix.M22, 0.0f,
		Matrix.M31, Matrix.M32, 0.0f);
	Out.M31 = Z.Determinant() / Determinant;

	ZEMatrix3x3 K;
	ZEMatrix3x3::Create(K, 
		0.0f, Matrix.M12, Matrix.M13,
		1.0f, Matrix.M22, Matrix.M23,
		0.0f, Matrix.M32, Matrix.M33);
	Out.M12 = K.Determinant() / Determinant;

	ZEMatrix3x3 L;
	ZEMatrix3x3::Create(L, 
		Matrix.M11, 0.0f, Matrix.M13,
		Matrix.M21, 1.0f, Matrix.M23,
		Matrix.M31, 0.0f, Matrix.M33);
	Out.M22 = L.Determinant() / Determinant;

	ZEMatrix3x3 M;
	ZEMatrix3x3::Create(M,
		Matrix.M11, Matrix.M12, 0.0f, 
		Matrix.M21,	Matrix.M22, 1.0f,
		Matrix.M31, Matrix.M32, 0.0f);
	Out.M32 = M.Determinant() / Determinant;

	ZEMatrix3x3 O;
	ZEMatrix3x3::Create(O, 
		0.0f, Matrix.M12, Matrix.M13,
		0.0f, Matrix.M22, Matrix.M23,
		1.0f, Matrix.M32, Matrix.M33);
	Out.M13 = O.Determinant() / Determinant;

	ZEMatrix3x3 P;
	ZEMatrix3x3::Create(P,
		Matrix.M11, 0.0f, Matrix.M13,
		Matrix.M21, 0.0f, Matrix.M23,
		Matrix.M31, 1.0f, Matrix.M33);
	Out.M23 = P.Determinant() / Determinant;

	ZEMatrix3x3 Q;
	ZEMatrix3x3::Create(Q,
		Matrix.M11, Matrix.M12, 0.0f, 
		Matrix.M21, Matrix.M22, 0.0f,
		Matrix.M31, Matrix.M32, 1.0f);
	Out.M33 = Q.Determinant() / Determinant;

	return true;
}

float ZEMatrix3x3::Determinant() const
{
	return ZEMatrix3x3::Determinant(*this);
}

ZEMatrix3x3 ZEMatrix3x3::Inverse() const
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::Transpose() const
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix3x3::InverseSelf()
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix3x3::TransposeSelf()
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Transpose(Temp, *this);
	*this = Temp;
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

ZEVector2 ZEMatrix3x3::operator*(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEMatrix3x3::operator*(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEMatrix3x3::operator*(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
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
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - M.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const 
{
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - M.MA[I]) > ZE_ZERO_THRESHOLD)
			return true;

	return false;
}

float ZEMatrix3x3::operator[](ZESize Index) const
{
	return MA[Index];
}

float& ZEMatrix3x3::operator[](ZESize Index)
{
	return MA[Index];
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

ZEMatrix3x3 operator*(float S, const ZEMatrix3x3& RightOperand)
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Scale(Temp, RightOperand, S);
	return Temp;
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
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 		2.0f * xz -	2.0f * yw;
	Matrix.M32 = 		2.0f * yz +	2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx -	2.0f * yy;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotation(ZEMatrix4x4& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix4x4 XRotation, YRotation, ZRotation, Temp;

	ZEMatrix4x4::CreateRotationX(XRotation, Pitch);
	ZEMatrix4x4::CreateRotationY(YRotation, Yawn);
	ZEMatrix4x4::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
	case ZE_RO_XYZ:
		ZEMatrix4x4::Multiply(Temp, XRotation, YRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, ZRotation);
		break;

	case ZE_RO_XZY:
		ZEMatrix4x4::Multiply(Temp, XRotation, ZRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, YRotation);
		break;

	case ZE_RO_YXZ:
		ZEMatrix4x4::Multiply(Temp, YRotation, XRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, ZRotation);
		break;

	case ZE_RO_YZX:
		ZEMatrix4x4::Multiply(Temp, YRotation, ZRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, XRotation);
		break;

	case ZE_RO_ZXY:
		ZEMatrix4x4::Multiply(Temp, ZRotation, XRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, YRotation);
		break;

	case ZE_RO_ZYX:
		ZEMatrix4x4::Multiply(Temp, ZRotation, YRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, XRotation);
		break;
	}
}

void ZEMatrix4x4::CreateRotationX(ZEMatrix4x4& Matrix, float Pitch)
{
	float Cos = ZEAngle::Cos(Pitch);
	float Sin = ZEAngle::Sin(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotationY(ZEMatrix4x4& Matrix, float Yawn)
{
	float Cos = ZEAngle::Cos(Yawn);
	float Sin = ZEAngle::Sin(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = Sin;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;
	Matrix.M24 = 0.0f;

	Matrix.M31 = -Sin;
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
	float Cos = ZEAngle::Cos(Roll);
	float Sin = ZEAngle::Sin(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = Sin;
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
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, float x, float y, float z)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = (1.0f - 2.0f * yy - 2.0f * zz) * Scale.x;
	Matrix.M12 = (		2.0f * xy - 2.0f * zw) * Scale.y;
	Matrix.M13 = (		2.0f * xz + 2.0f * yw) * Scale.z;
	Matrix.M14 = Position.x;				 
											 
	Matrix.M21 = (		2.0f * xy + 2.0f * zw) * Scale.x;
	Matrix.M22 = (1.0f - 2.0f * xx - 2.0f * zz) * Scale.y;
	Matrix.M23 = (		2.0f * yz - 2.0f * xw) * Scale.z;
	Matrix.M24 = Position.y;				 
											 
	Matrix.M31 = (		2.0f * xz -	2.0f * yw) * Scale.x;
	Matrix.M32 = (		2.0f * yz +	2.0f * xw) * Scale.y;
	Matrix.M33 = (1.0f - 2.0f * xx -	2.0f * yy) * Scale.z;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = Position.x;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = Position.y;

	Matrix.M31 = 		2.0f * xz -	2.0f * yw;
	Matrix.M32 = 		2.0f * yz +	2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx -	2.0f * yy;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, const ZEVector3& Position)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, Position.x,
			0.0f, 1.0f, 0.0f, Position.y,
			0.0f, 0.0f, 1.0f, Position.z,
			0.0f, 0.0f, 0.0f, 1.0f );
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
	ZEQuaternion RotationInv;
	ZEQuaternion::Conjugate(RotationInv, Rotation);

	float xx = RotationInv.x * RotationInv.x;
	float xy = RotationInv.x * RotationInv.y;
	float xz = RotationInv.x * RotationInv.z;
	float xw = RotationInv.x * RotationInv.w;
	float yy = RotationInv.y * RotationInv.y;
	float yz = RotationInv.y * RotationInv.z;
	float yw = RotationInv.y * RotationInv.w;
	float zz = RotationInv.z * RotationInv.z;
	float zw = RotationInv.z * RotationInv.w;
	float ww = RotationInv.w * RotationInv.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = -Position.x * Matrix.M11 + -Position.y * Matrix.M12 + -Position.z * Matrix.M13;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = -Position.x * Matrix.M21 + -Position.y * Matrix.M22 + -Position.z * Matrix.M23;

	Matrix.M31 = 		2.0f * xz -	2.0f * yw;
	Matrix.M32 = 		2.0f * yz +	2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx -	2.0f * yy;
	Matrix.M34 = -Position.x * Matrix.M31 + -Position.y * Matrix.M32 + -Position.z * Matrix.M33;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
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

	ZEMatrix4x4::Create(Matrix, 
		XAxis.x, XAxis.y, XAxis.z, -ZEVector3::DotProduct(XAxis, Position),
		YAxis.x, YAxis.y, YAxis.z, -ZEVector3::DotProduct(YAxis, Position),
		ZAxis.x, ZAxis.y, ZAxis.z, -ZEVector3::DotProduct(ZAxis, Position),
		0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateOrthographicProjection(ZEMatrix4x4& Matrix, float Width, float Height, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/Width, 0.0f, 0.0f, 0.0f, 
			0.0f, 2.0f/Height, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), NearZ/(NearZ - FarZ), 
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjection(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float Ys = 1.0f / ZEAngle::Tan(FOV * 0.5f);
	float Xs = Ys / AspectRatio;

	Create(Matrix, 
		Xs, 0.0f, 0.0f, 0.0f,
		0.0f, Ys, 0.0f, 0.0f,
		0.0f, 0.0f, FarZ / (FarZ - NearZ), -NearZ * FarZ / (FarZ - NearZ),
		0.0f, 0.0f, 1.0f, 0.0f);
}

void ZEMatrix4x4::CreateViewPortTransform(ZEMatrix4x4& Matrix, float Left, float Right, float Top, float Bottom, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/(Right - Left), 0.0f, 0.0f, (Left + Right)/(Left - Right), 
			0.0f, 2.0f/(Top - Bottom), 0.0f, (Top + Bottom)/(Bottom - Top), 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), NearZ/(NearZ - FarZ), 
			0.0f, 0.0f, 0.0f, 1.0f);
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
	zeAssert(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeAssert(&Out == &B, "Parameters Out and B can not be the same variable.");

	for (ZESize RowIndex = 0; RowIndex < ZE_MATRIX4X4_ROW_ELEMENT_COUNT; RowIndex++) // Row
	{
		for (ZESize ColumnIndex = 0; ColumnIndex < ZE_MATRIX4X4_COL_ELEMENT_COUNT; ColumnIndex++) // Column
		{
			Out.M[ColumnIndex][RowIndex] = 
				A.M[0][RowIndex] * B.M[ColumnIndex][0] + 
				A.M[1][RowIndex] * B.M[ColumnIndex][1] + 
				A.M[2][RowIndex] * B.M[ColumnIndex][2] + 
				A.M[3][RowIndex] * B.M[ColumnIndex][3];
		}
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
	zeAssert(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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
	zeAssert(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34;
}

void ZEMatrix4x4::Transform3x3(ZEVector3 &Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	zeAssert(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix4x4::Transform(ZEVector4& Out, const ZEMatrix4x4& Matrix, const ZEVector4& Vector)
{
	zeAssert(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14 * Vector.w;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24 * Vector.w;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34 * Vector.w;
	Out.w = Matrix.M41 * Vector.x + Matrix.M42 * Vector.y + Matrix.M43 * Vector.z + Matrix.M44 * Vector.w;
}
float ZEMatrix4x4::Determinant(const ZEMatrix4x4 &Matrix)
{
	ZEMatrix3x3 A, B, C, D;
	ZEMatrix3x3::Create(A, 
		Matrix.M22, Matrix.M23, Matrix.M24, 
		Matrix.M32, Matrix.M33, Matrix.M34, 
		Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix3x3::Create(B, 
		Matrix.M21, Matrix.M23, Matrix.M24, 
		Matrix.M31, Matrix.M33, Matrix.M34, 
		Matrix.M41, Matrix.M43, Matrix.M44);

    ZEMatrix3x3::Create(C, 
		Matrix.M21, Matrix.M22, Matrix.M24, 
		Matrix.M31, Matrix.M32, Matrix.M34, 
		Matrix.M41, Matrix.M42, Matrix.M44);

	ZEMatrix3x3::Create(D, 
		Matrix.M21, Matrix.M22, Matrix.M23, 
		Matrix.M31, Matrix.M32, Matrix.M33, 
		Matrix.M41, Matrix.M42, Matrix.M43);

	return Matrix.M11 * A.Determinant() + Matrix.M13 * C.Determinant() - Matrix.M12 * B.Determinant() - Matrix.M14 * D.Determinant();
}

bool ZEMatrix4x4::Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix)
{
	zeAssert(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	float Determinant = Matrix.Determinant();

	if(Determinant == 0)
		return false;

	ZEMatrix4x4 X;
	ZEMatrix4x4::Create(X, 
		1.0f, Matrix.M12, Matrix.M13, Matrix.M14, 
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34, 
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M11 = X.Determinant() / Determinant;

	ZEMatrix4x4 Y;
	ZEMatrix4x4::Create(Y,
		Matrix.M11, 1.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0f, Matrix.M43, Matrix.M44);
	Out.M21 = Y.Determinant() / Determinant;

	ZEMatrix4x4 Z;
	ZEMatrix4x4::Create(Z,
		Matrix.M11, Matrix.M12, 1.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);
	Out.M31 = Z.Determinant() / Determinant;

	ZEMatrix4x4 W;
	ZEMatrix4x4::Create(W,
		Matrix.M11, Matrix.M12, Matrix.M13, 1.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);
	Out.M41 = W.Determinant() / Determinant;

	ZEMatrix4x4 K;
	ZEMatrix4x4::Create(K,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		1.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M12= K.Determinant() / Determinant;

	ZEMatrix4x4 L;
	ZEMatrix4x4::Create(L,
		Matrix.M11, 0.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 1.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0f, Matrix.M43, Matrix.M44);
	Out.M22 = L.Determinant() / Determinant;

	ZEMatrix4x4 M;
	ZEMatrix4x4::Create(M,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 1.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);
	Out.M32= M.Determinant() / Determinant;

	ZEMatrix4x4 N;
	ZEMatrix4x4::Create(N,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 1.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);
	Out.M42 = N.Determinant() / Determinant;

	ZEMatrix4x4 O;
	ZEMatrix4x4::Create(O,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		1.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M13 = O.Determinant() / Determinant;

	ZEMatrix4x4 P;
	ZEMatrix4x4::Create(P,
		Matrix.M11, 0, Matrix.M13, Matrix.M14,
		Matrix.M21, 0, Matrix.M23, Matrix.M24,
		Matrix.M31, 1, Matrix.M33, Matrix.M34,
		Matrix.M41, 0, Matrix.M43, Matrix.M44);
	Out.M23 = P.Determinant() / Determinant;

	ZEMatrix4x4 Q;
	ZEMatrix4x4::Create(Q,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 1.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);
	Out.M33 = Q.Determinant() / Determinant;

	ZEMatrix4x4 R;
	ZEMatrix4x4::Create(R,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 1.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);
	Out.M43 = R.Determinant() / Determinant;


	ZEMatrix4x4 S;
	ZEMatrix4x4::Create(S,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		1.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M14 = S.Determinant() /Determinant;

	ZEMatrix4x4 T;
	ZEMatrix4x4::Create(T,
		Matrix.M11, 0.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 1.0f, Matrix.M43, Matrix.M44);
	Out.M24 = T.Determinant() / Determinant;

	ZEMatrix4x4 U;
	ZEMatrix4x4::Create(U,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 1.0f, Matrix.M44);
	Out.M34 = U.Determinant() / Determinant;

	ZEMatrix4x4 V;
	ZEMatrix4x4::Create(V,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 1.0f);
	Out.M44 = V.Determinant() / Determinant;

	return true;
}

float ZEMatrix4x4::Determinant() const
{
	return ZEMatrix4x4::Determinant(*this);
}

ZEMatrix4x4 ZEMatrix4x4::Inverse() const
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::Transpose() const
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix4x4::InverseSelf()
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix4x4::TransposeSelf()
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Transpose(Temp, *this);
	*this = Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator+(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator-(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator*(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Multiply(Temp, *this, RightOperand);
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
	ZEMatrix4x4::Scale(Temp, *this, S);
	return Temp;
}


ZEMatrix4x4& ZEMatrix4x4::operator += (const ZEMatrix4x4 &RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Add(Temp, *this, RightOperand);
	*this = Temp;
	return *this;
}

ZEMatrix4x4& ZEMatrix4x4::operator -= (const ZEMatrix4x4 &RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix4x4& ZEMatrix4x4::operator *= (const ZEMatrix4x4 &RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix4x4& ZEMatrix4x4::operator *= (float S)
{
	ZEMatrix4x4::Scale(*this, *this, S);
	return *this;
}

bool ZEMatrix4x4::operator == (const ZEMatrix4x4 &M) const 
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - M.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix4x4::operator != (const ZEMatrix4x4 &M) const 
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - M.MA[I]) > ZE_ZERO_THRESHOLD)
			return true;

	return false;
}

float ZEMatrix4x4::operator[](ZESize Index) const
{
	return MA[Index];
}

float& ZEMatrix4x4::operator[](ZESize Index)
{
	return MA[Index];
}

ZEMatrix4x4::ZEMatrix4x4(float M11, float M12, float M13, float M14,
						 float M21, float M22, float M23, float M24,
						 float M31, float M32, float M33, float M34,
						 float M41, float M42, float M43, float M44)
{
	ZEMatrix4x4::Create(*this, M11, M12, M13, M14, M21, M22, M23, M24, M31, M32, M33, M34, M41, M42, M43, M44);  
}

ZEMatrix4x4::ZEMatrix4x4(float M[16])
{
	ZEMatrix4x4::Create(*this, M);
}

ZEMatrix4x4::ZEMatrix4x4()
{

}

ZEMatrix4x4 operator*(float S, const ZEMatrix4x4& RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Scale(Temp, RightOperand, S);
	return Temp;
}
