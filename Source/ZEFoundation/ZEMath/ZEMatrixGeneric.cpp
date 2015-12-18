//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrixGeneric.cpp
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
#include "ZEError.h"

#define ZE_MATRIX4X4_ROW_ELEMENT_COUNT		(ZESize)4
#define ZE_MATRIX4X4_COL_ELEMENT_COUNT		(ZESize)4

// ZEMatrix3x3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEMatrix3x3::Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
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

void ZEMatrix3x3::Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
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

void ZEMatrix3x3::Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

	for (ZEInt RowIndex = 0; RowIndex < 3; RowIndex++) // Row
		for (ZEInt ColumnIndex = 0; ColumnIndex < 3; ColumnIndex++) // Column
			Out.M[ColumnIndex][RowIndex] = 
			A.M[0][RowIndex] * B.M[ColumnIndex][0] + 
			A.M[1][RowIndex] * B.M[ColumnIndex][1] + 
			A.M[2][RowIndex] * B.M[ColumnIndex][2];
}

void ZEMatrix3x3::Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3& A, float s)
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
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEMatrix3x3& Matrix, const ZEVector2& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23;
}

void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEMatrix3x3& Matrix, const ZEVector4& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
	Out.w = 1.0f;
}

// ZEMatrix3x3d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEMatrix3x3d::Add(ZEMatrix3x3d &Out, const ZEMatrix3x3d &A, const ZEMatrix3x3d &B)
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

void ZEMatrix3x3d::Sub(ZEMatrix3x3d &Out, const ZEMatrix3x3d &A, const ZEMatrix3x3d &B)
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

void ZEMatrix3x3d::Multiply(ZEMatrix3x3d &Out, const ZEMatrix3x3d &A, const ZEMatrix3x3d &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

	for (ZEInt RowIndex = 0; RowIndex < 3; RowIndex++) // Row
		for (ZEInt ColumnIndex = 0; ColumnIndex < 3; ColumnIndex++) // Column
			Out.M[ColumnIndex][RowIndex] = 
			A.M[0][RowIndex] * B.M[ColumnIndex][0] + 
			A.M[1][RowIndex] * B.M[ColumnIndex][1] + 
			A.M[2][RowIndex] * B.M[ColumnIndex][2];
}

void ZEMatrix3x3d::Scale(ZEMatrix3x3d &Out, const ZEMatrix3x3d& A, double s)
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

void ZEMatrix3x3d::Transpose(ZEMatrix3x3d& Out, const ZEMatrix3x3d& Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

double ZEMatrix3x3d::Determinant(const ZEMatrix3x3d &Matrix)
{
	double a, b, c, d, e, f;

	a = Matrix.M11 * Matrix.M22 * Matrix.M33;
	b = Matrix.M12 * Matrix.M23 * Matrix.M31;
	c = Matrix.M13 * Matrix.M21 * Matrix.M32;
	d = Matrix.M13 * Matrix.M22 * Matrix.M31;
	e = Matrix.M12 * Matrix.M21 * Matrix.M33;
	f = Matrix.M11 * Matrix.M23 * Matrix.M32;

	return a + b + c - d - e - f;
}

bool ZEMatrix3x3d::Inverse(ZEMatrix3x3d &Out, const ZEMatrix3x3d &Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	double Determinant = ZEMatrix3x3d::Determinant(Matrix);

	if (Determinant == 0)
		return false;

	ZEMatrix3x3d X;
	ZEMatrix3x3d::Create(X,
		1.0, Matrix.M12, Matrix.M13, 
		0.0, Matrix.M22, Matrix.M23,
		0.0, Matrix.M32, Matrix.M33);
	Out.M11 = X.Determinant() / Determinant;

	ZEMatrix3x3d Y;
	ZEMatrix3x3d::Create(Y,
		Matrix.M11, 1.0, Matrix.M13,
		Matrix.M21, 0.0, Matrix.M23,
		Matrix.M31, 0.0, Matrix.M33);
	Out.M21 = Y.Determinant() / Determinant;

	ZEMatrix3x3d Z;
	ZEMatrix3x3d::Create(Z,
		Matrix.M11, Matrix.M12, 1.0, 
		Matrix.M21, Matrix.M22, 0.0,
		Matrix.M31, Matrix.M32, 0.0);
	Out.M31 = Z.Determinant() / Determinant;

	ZEMatrix3x3d K;
	ZEMatrix3x3d::Create(K, 
		0.0, Matrix.M12, Matrix.M13,
		1.0, Matrix.M22, Matrix.M23,
		0.0, Matrix.M32, Matrix.M33);
	Out.M12 = K.Determinant() / Determinant;

	ZEMatrix3x3d L;
	ZEMatrix3x3d::Create(L, 
		Matrix.M11, 0.0, Matrix.M13,
		Matrix.M21, 1.0, Matrix.M23,
		Matrix.M31, 0.0, Matrix.M33);
	Out.M22 = L.Determinant() / Determinant;

	ZEMatrix3x3d M;
	ZEMatrix3x3d::Create(M,
		Matrix.M11, Matrix.M12, 0.0, 
		Matrix.M21,	Matrix.M22, 1.0,
		Matrix.M31, Matrix.M32, 0.0);
	Out.M32 = M.Determinant() / Determinant;

	ZEMatrix3x3d O;
	ZEMatrix3x3d::Create(O, 
		0.0, Matrix.M12, Matrix.M13,
		0.0, Matrix.M22, Matrix.M23,
		1.0, Matrix.M32, Matrix.M33);
	Out.M13 = O.Determinant() / Determinant;

	ZEMatrix3x3d P;
	ZEMatrix3x3d::Create(P,
		Matrix.M11, 0.0, Matrix.M13,
		Matrix.M21, 0.0, Matrix.M23,
		Matrix.M31, 1.0, Matrix.M33);
	Out.M23 = P.Determinant() / Determinant;

	ZEMatrix3x3d Q;
	ZEMatrix3x3d::Create(Q,
		Matrix.M11, Matrix.M12, 0.0, 
		Matrix.M21, Matrix.M22, 0.0,
		Matrix.M31, Matrix.M32, 1.0);
	Out.M33 = Q.Determinant() / Determinant;

	return true;
}

void ZEMatrix3x3d::Transform(ZEVector2d& Out, const ZEMatrix3x3d& Matrix, const ZEVector2d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23;
}

void ZEMatrix3x3d::Transform(ZEVector3d& Out, const ZEMatrix3x3d& Matrix, const ZEVector3d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix3x3d::Transform(ZEVector4d& Out, const ZEMatrix3x3d& Matrix, const ZEVector4d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
	Out.w = 1.0f;
}

// ZEMatrix4x4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ZEMatrix4x4::Add(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
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

void ZEMatrix4x4::Sub(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
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

void ZEMatrix4x4::Multiply(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

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

void ZEMatrix4x4::Scale(ZEMatrix4x4 &Out, const ZEMatrix4x4& A, float s)
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
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

bool ZEMatrix4x4::Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

void ZEMatrix4x4::Transform(ZEVector3& Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34;
}

void ZEMatrix4x4::Transform3x3(ZEVector3 &Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix4x4::Transform(ZEVector4& Out, const ZEMatrix4x4& Matrix, const ZEVector4& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14 * Vector.w;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24 * Vector.w;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34 * Vector.w;
	Out.w = Matrix.M41 * Vector.x + Matrix.M42 * Vector.y + Matrix.M43 * Vector.z + Matrix.M44 * Vector.w;
}

// ZEMatrix4x4d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ZEMatrix4x4d::Add(ZEMatrix4x4d &Out, const ZEMatrix4x4d &A, const ZEMatrix4x4d &B)
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

void ZEMatrix4x4d::Sub(ZEMatrix4x4d &Out, const ZEMatrix4x4d &A, const ZEMatrix4x4d &B)
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

void ZEMatrix4x4d::Multiply(ZEMatrix4x4d &Out, const ZEMatrix4x4d &A, const ZEMatrix4x4d &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

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

void ZEMatrix4x4d::Scale(ZEMatrix4x4d &Out, const ZEMatrix4x4d& A, double s)
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

void ZEMatrix4x4d::Transpose(ZEMatrix4x4d& Out, const ZEMatrix4x4d& Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

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

bool ZEMatrix4x4d::Inverse(ZEMatrix4x4d &Out, const ZEMatrix4x4d &Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	double Determinant = Matrix.Determinant();

	if(Determinant == 0)
		return false;

	ZEMatrix4x4d X;
	ZEMatrix4x4d::Create(X, 
		1.0, Matrix.M12, Matrix.M13, Matrix.M14, 
		0.0, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0, Matrix.M32, Matrix.M33, Matrix.M34, 
		0.0, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M11 = X.Determinant() / Determinant;

	ZEMatrix4x4d Y;
	ZEMatrix4x4d::Create(Y,
		Matrix.M11, 1.0, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0, Matrix.M43, Matrix.M44);
	Out.M21 = Y.Determinant() / Determinant;

	ZEMatrix4x4d Z;
	ZEMatrix4x4d::Create(Z,
		Matrix.M11, Matrix.M12, 1.0, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0, Matrix.M44);
	Out.M31 = Z.Determinant() / Determinant;

	ZEMatrix4x4d W;
	ZEMatrix4x4d::Create(W,
		Matrix.M11, Matrix.M12, Matrix.M13, 1.0,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0);
	Out.M41 = W.Determinant() / Determinant;

	ZEMatrix4x4d K;
	ZEMatrix4x4d::Create(K,
		0.0, Matrix.M12, Matrix.M13, Matrix.M14,
		1.0, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M12= K.Determinant() / Determinant;

	ZEMatrix4x4d L;
	ZEMatrix4x4d::Create(L,
		Matrix.M11, 0.0, Matrix.M13, Matrix.M14,
		Matrix.M21, 1.0, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0, Matrix.M43, Matrix.M44);
	Out.M22 = L.Determinant() / Determinant;

	ZEMatrix4x4d M;
	ZEMatrix4x4d::Create(M,
		Matrix.M11, Matrix.M12, 0.0, Matrix.M14,
		Matrix.M21, Matrix.M22, 1.0, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0, Matrix.M44);
	Out.M32= M.Determinant() / Determinant;

	ZEMatrix4x4d N;
	ZEMatrix4x4d::Create(N,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0,
		Matrix.M21, Matrix.M22, Matrix.M23, 1.0,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0);
	Out.M42 = N.Determinant() / Determinant;

	ZEMatrix4x4d O;
	ZEMatrix4x4d::Create(O,
		0.0, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0, Matrix.M22, Matrix.M23, Matrix.M24,
		1.0, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M13 = O.Determinant() / Determinant;

	ZEMatrix4x4d P;
	ZEMatrix4x4d::Create(P,
		Matrix.M11, 0.0, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0, Matrix.M23, Matrix.M24,
		Matrix.M31, 1.0, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0, Matrix.M43, Matrix.M44);
	Out.M23 = P.Determinant() / Determinant;

	ZEMatrix4x4d Q;
	ZEMatrix4x4d::Create(Q,
		Matrix.M11, Matrix.M12, 0.0, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0, Matrix.M24,
		Matrix.M31, Matrix.M32, 1.0, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0, Matrix.M44);
	Out.M33 = Q.Determinant() / Determinant;

	ZEMatrix4x4d R;
	ZEMatrix4x4d::Create(R,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0,
		Matrix.M31, Matrix.M32, Matrix.M33, 1.0,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0);
	Out.M43 = R.Determinant() / Determinant;


	ZEMatrix4x4d S;
	ZEMatrix4x4d::Create(S,
		0.0, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0, Matrix.M32, Matrix.M33, Matrix.M34,
		1.0, Matrix.M42, Matrix.M43, Matrix.M44);
	Out.M14 = S.Determinant() /Determinant;

	ZEMatrix4x4d T;
	ZEMatrix4x4d::Create(T,
		Matrix.M11, 0.0, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0, Matrix.M33, Matrix.M34,
		Matrix.M41, 1.0, Matrix.M43, Matrix.M44);
	Out.M24 = T.Determinant() / Determinant;

	ZEMatrix4x4d U;
	ZEMatrix4x4d::Create(U,
		Matrix.M11, Matrix.M12, 0.0, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0, Matrix.M34,
		Matrix.M41, Matrix.M42, 1.0, Matrix.M44);
	Out.M34 = U.Determinant() / Determinant;

	ZEMatrix4x4d V;
	ZEMatrix4x4d::Create(V,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0,
		Matrix.M41, Matrix.M42, Matrix.M43, 1.0);
	Out.M44 = V.Determinant() / Determinant;

	return true;
}

void ZEMatrix4x4d::Transform(ZEVector3d& Out, const ZEMatrix4x4d& Matrix, const ZEVector3d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34;
}

void ZEMatrix4x4d::Transform3x3(ZEVector3d &Out, const ZEMatrix4x4d& Matrix, const ZEVector3d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z;
}

void ZEMatrix4x4d::Transform(ZEVector4d& Out, const ZEMatrix4x4d& Matrix, const ZEVector4d& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	Out.x = Matrix.M11 * Vector.x + Matrix.M12 * Vector.y + Matrix.M13 * Vector.z + Matrix.M14 * Vector.w;
	Out.y = Matrix.M21 * Vector.x + Matrix.M22 * Vector.y + Matrix.M23 * Vector.z + Matrix.M24 * Vector.w;
	Out.z = Matrix.M31 * Vector.x + Matrix.M32 * Vector.y + Matrix.M33 * Vector.z + Matrix.M34 * Vector.w;
	Out.w = Matrix.M41 * Vector.x + Matrix.M42 * Vector.y + Matrix.M43 * Vector.z + Matrix.M44 * Vector.w;
}
