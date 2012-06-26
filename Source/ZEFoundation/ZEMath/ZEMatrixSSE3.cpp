//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrixSSE3.cpp
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

extern "C"
{
#include <xmmintrin.h>
#include <pmmintrin.h>
};

// MMX Register Loads
inline __m128 LoadFloat3(const float *F)
{
	__m128 mmxL, mmxH;

	mmxL = _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)F);
	mmxH = _mm_load_ss(F+2);
	return _mm_movelh_ps(mmxL, mmxH);
}

inline void LoadMatrix3x3(__m128 *Reg, const ZEMatrix3x3 &In)
{	
	Reg[0] = LoadFloat3(In.MA);
	Reg[1] = LoadFloat3(In.MA + 3);
	Reg[2] = LoadFloat3(In.MA + 6);
}

inline void LoadMatrix3x3Transpose(__m128 *Reg, const ZEMatrix3x3 &In)
{	
	// Order is Reversed, Register Holds them in reverse order (z-x)
	Reg[0] = _mm_set_ps(In.M13 , In.M13 , In.M12 , In.M11 );
	Reg[1] = _mm_set_ps(In.M23 , In.M23 , In.M22 , In.M21 );
	Reg[2] = _mm_set_ps(In.M33 , In.M33 , In.M32 , In.M31 );
}

inline void LoadMatrix4x4(__m128 *Reg, const ZEMatrix4x4 &In)
{	
	Reg[0] = _mm_loadu_ps(In.MA);
	Reg[1] = _mm_loadu_ps(In.MA + 4);
	Reg[2] = _mm_loadu_ps(In.MA + 8);
	Reg[3] = _mm_loadu_ps(In.MA + 12);
}

inline void LoadMatrix4x4Transpose(__m128 *Reg, const ZEMatrix4x4 &In)
{	
	// Order is Reversed, Register Holds them in reverse order (z-x)
	Reg[0] = _mm_set_ps(In.M14 , In.M13 , In.M12 , In.M11 );
	Reg[1] = _mm_set_ps(In.M24 , In.M23 , In.M22 , In.M21 );
	Reg[2] = _mm_set_ps(In.M34 , In.M33 , In.M32 , In.M31 );
	Reg[3] = _mm_set_ps(In.M44 , In.M43 , In.M42 , In.M41 );
}

// MMX Register Stores
inline void StoreFloat3(float *F, __m128 &Reg)
{
	_mm_storel_pi((__m64*)F, Reg);
	Reg = _mm_shuffle_ps(Reg, Reg, 0xAA);
	F += 2;
	_mm_store_ss(F, Reg);
}

inline void StoreMatrix3x3(ZEMatrix3x3 &Out, __m128 *Reg)
{	
	StoreFloat3(Out.MA, Reg[0]);
	StoreFloat3(Out.MA + 3, Reg[1]);
	StoreFloat3(Out.MA + 6, Reg[2]);
}

inline void StoreMatrix4x4(ZEMatrix4x4 &Out, const __m128 *Reg)
{	
	_mm_storeu_ps(Out.MA, Reg[0]);
	_mm_storeu_ps(Out.MA + 4, Reg[1] );
	_mm_storeu_ps(Out.MA + 8, Reg[2] );
	_mm_storeu_ps(Out.MA + 12, Reg[3]);
}

// ZEMatrix3x3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEMatrix3x3::Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	__m128 mmxBLines;
	__m128 mmxMatrixA[3];
	LoadMatrix3x3(mmxMatrixA, A);

	for(int i = 0; i < 3; i++)
	{
		mmxBLines = _mm_loadu_ps(B.MA + 3 * i);
		mmxMatrixA[i] = _mm_add_ps(mmxMatrixA[i], mmxBLines);
	}
	StoreMatrix3x3(Out, mmxMatrixA);
}

void ZEMatrix3x3::Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	__m128 mmxBLines;
	__m128 mmxMatrixA[3];
	LoadMatrix3x3(mmxMatrixA, A);

	for(int i = 0; i < 3; i++)
	{
		mmxBLines = _mm_loadu_ps(B.MA + 3 * i);
		mmxMatrixA[i] = _mm_sub_ps(mmxMatrixA[i], mmxBLines);
	}
	StoreMatrix3x3(Out, mmxMatrixA);
}

void ZEMatrix3x3::Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

	// Registers Used
	__m128 mmx3, mmx4, mmx5;		// All except 6, 7
	__m128 mmxMatrixA[3];			// mmx0, mmx1, mmx2 

	LoadMatrix3x3Transpose(mmxMatrixA, A);

	float *OutColumnIndex = Out.MA;
	for(int i = 0; i < 3; i++)
	{
		// Load Column of B to mmx5
		mmx5 = LoadFloat3(B.MA + 3 * i);		

		// 1st Couple
		mmx3 = _mm_mul_ps(mmxMatrixA[0], mmx5);
		mmx4 = _mm_mul_ps(mmxMatrixA[1], mmx5);
		mmx3 = _mm_hadd_ps(mmx3, mmx4);
		mmx3 = _mm_hadd_ps(mmx3, mmx3);					// -, -, Ry, Rx
		
		// Z Only
		mmx5 = _mm_mul_ps(mmxMatrixA[2], mmx5);
		mmx5 = _mm_hadd_ps(mmx5, mmx5);
		mmx5 = _mm_hadd_ps(mmx5, mmx5);					// -, -, Rz, Rz

		// Join
		mmx5 = _mm_shuffle_ps(mmx3, mmx5, 0x44);		// Rz, Rz, Ry, Rx

		// Storing the Column to "Out"
		StoreFloat3(OutColumnIndex + 3 * i, mmx5);
	}
}

void ZEMatrix3x3::Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3& A, float s)
{
	__m128 mmxScaleLine = _mm_load1_ps(&s);
	__m128 mmxMatrixA[3];
	LoadMatrix3x3(mmxMatrixA, A);

	for(int i = 0; i < 3; i++)
		mmxMatrixA[i] = _mm_mul_ps(mmxMatrixA[i], mmxScaleLine);
	StoreMatrix3x3(Out, mmxMatrixA);
}

void ZEMatrix3x3::Transpose(ZEMatrix3x3& Out, const ZEMatrix3x3& Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxMatrix[3];
	LoadMatrix3x3Transpose(mmxMatrix, Matrix);
	StoreMatrix3x3(Out, mmxMatrix);
}

float ZEMatrix3x3::Determinant(const ZEMatrix3x3 &Matrix)
{
	__m128 mmxMatrix[3];
	__m128 mmxRow1Modified, mmxRow2Modified;
	__m128 mmxABC, mmxDEF;
	LoadMatrix3x3Transpose(mmxMatrix, Matrix);
	
	// Calculating A + B + C
	mmxRow1Modified = _mm_shuffle_ps(mmxMatrix[1], mmxMatrix[1], 0xC9); //Shift Left
	mmxRow2Modified = _mm_shuffle_ps(mmxMatrix[2], mmxMatrix[2], 0xD2); //Shift Right

	mmxRow1Modified = _mm_mul_ps(mmxRow1Modified, mmxMatrix[0]);
	mmxABC = _mm_mul_ps(mmxRow1Modified, mmxRow2Modified);

	// Calculating D + E + F
	mmxRow1Modified = _mm_shuffle_ps(mmxMatrix[1], mmxMatrix[1], 0xD2); //Shift Right
	mmxRow2Modified = _mm_shuffle_ps(mmxMatrix[2], mmxMatrix[2], 0xC9); //Shift Left

	mmxRow1Modified = _mm_mul_ps(mmxRow1Modified, mmxMatrix[0]);
	mmxDEF = _mm_mul_ps(mmxRow1Modified, mmxRow2Modified);

	// Adding all together
	mmxABC = _mm_hadd_ps(mmxABC, mmxDEF);
	mmxABC = _mm_hadd_ps(mmxABC, mmxABC);
	mmxABC = _mm_hsub_ps(mmxABC, mmxABC);

	float Temp;
	_mm_store_ss(&Temp, mmxABC);
	return Temp;
} 

bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxDeterminants;
	__m128 mmxMainDeterminant;

	float Determinant = ZEMatrix3x3::Determinant(Matrix);
	mmxMainDeterminant = _mm_load1_ps(&Determinant);

	if (Determinant == 0)
		return false;

	// 1st Column
	ZEMatrix3x3 X;
	ZEMatrix3x3::Create(X,
		1.0f, Matrix.M12, Matrix.M13, 
		0.0f, Matrix.M22, Matrix.M23,
		0.0f, Matrix.M32, Matrix.M33);

	ZEMatrix3x3 Y;
	ZEMatrix3x3::Create(Y,
		Matrix.M11, 1.0f, Matrix.M13,
		Matrix.M21, 0.0f, Matrix.M23,
		Matrix.M31, 0.0f, Matrix.M33);
	
	ZEMatrix3x3 Z;
	ZEMatrix3x3::Create(Z,
		Matrix.M11, Matrix.M12, 1.0f, 
		Matrix.M21, Matrix.M22, 0.0f,
		Matrix.M31, Matrix.M32, 0.0f);

	mmxDeterminants = _mm_set_ps(0.0f, Z.Determinant(), Y.Determinant(), X.Determinant());
	__m128 Temp = _mm_div_ps(mmxDeterminants, mmxMainDeterminant);
	StoreFloat3(Out.MA, Temp);

	// 2nd Column
	ZEMatrix3x3 K;
	ZEMatrix3x3::Create(K, 
		0.0f, Matrix.M12, Matrix.M13,
		1.0f, Matrix.M22, Matrix.M23,
		0.0f, Matrix.M32, Matrix.M33);

	ZEMatrix3x3 L;
	ZEMatrix3x3::Create(L, 
		Matrix.M11, 0.0f, Matrix.M13,
		Matrix.M21, 1.0f, Matrix.M23,
		Matrix.M31, 0.0f, Matrix.M33);

	ZEMatrix3x3 M;
	ZEMatrix3x3::Create(M,
		Matrix.M11, Matrix.M12, 0.0f, 
		Matrix.M21,	Matrix.M22, 1.0f,
		Matrix.M31, Matrix.M32, 0.0f);

	mmxDeterminants = _mm_set_ps(0.0f, M.Determinant(), L.Determinant(), K.Determinant()); 
	Temp = _mm_div_ps(mmxDeterminants, mmxMainDeterminant);
	StoreFloat3(Out.MA + 3, Temp);

	// 3rd Column
	ZEMatrix3x3 O;
	ZEMatrix3x3::Create(O, 
		0.0f, Matrix.M12, Matrix.M13,
		0.0f, Matrix.M22, Matrix.M23,
		1.0f, Matrix.M32, Matrix.M33);

	ZEMatrix3x3 P;
	ZEMatrix3x3::Create(P,
		Matrix.M11, 0.0f, Matrix.M13,
		Matrix.M21, 0.0f, Matrix.M23,
		Matrix.M31, 1.0f, Matrix.M33);

	ZEMatrix3x3 Q;
	ZEMatrix3x3::Create(Q,
		Matrix.M11, Matrix.M12, 0.0f, 
		Matrix.M21, Matrix.M22, 0.0f,
		Matrix.M31, Matrix.M32, 1.0f);
	
	mmxDeterminants = _mm_set_ps(0.0f, Q.Determinant(), P.Determinant(), O.Determinant()); 
	Temp = _mm_div_ps(mmxDeterminants, mmxMainDeterminant);
	StoreFloat3(Out.MA + 6, Temp);

	return true;
}

void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEMatrix3x3& Matrix, const ZEVector2& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	__m128 mmxMatrix[3];
	__m128 mmxVectorX = _mm_load1_ps(Vector.M);
	__m128 mmxVectorY = _mm_load1_ps(Vector.M + 1);
	LoadMatrix3x3(mmxMatrix, Matrix);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxMatrix[2]);

	_mm_storel_pi((__m64*) &Out, mmxVectorY);
}

void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	__m128 mmxMatrix[3];
	__m128 mmxVectorX = _mm_load1_ps(Vector.M);
	__m128 mmxVectorY = _mm_load1_ps(Vector.M + 1);
	__m128 mmxVectorZ = _mm_load1_ps(Vector.M + 2);
	LoadMatrix3x3(mmxMatrix, Matrix);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	mmxVectorZ = _mm_mul_ps(mmxVectorZ, mmxMatrix[2]);

	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorZ);

	StoreFloat3(Out.M, mmxVectorY);
}

void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEMatrix3x3& Matrix, const ZEVector4& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Vector can not be the same variable.");

	__m128 mmxMatrix[3];
	__m128 mmxVectorX = _mm_load1_ps(Vector.M);
	__m128 mmxVectorY = _mm_load1_ps(Vector.M + 1);
	__m128 mmxVectorZ = _mm_load1_ps(Vector.M + 2);
	LoadMatrix3x3(mmxMatrix, Matrix);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	mmxVectorZ = _mm_mul_ps(mmxVectorZ, mmxMatrix[2]);

	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorZ);

	StoreFloat3(Out.M, mmxVectorY);
	Out.w = 1.0f;
}

// ZEMatrix4x4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZEMatrix4x4::Add(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	__m128 mmxBLines;
	__m128 mmxMatrixA[4];
	LoadMatrix4x4(mmxMatrixA, A);

	for(int i = 0; i < 4; i++)
	{
		mmxBLines = _mm_loadu_ps(B.MA + 4 * i);
		mmxMatrixA[i] = _mm_add_ps(mmxMatrixA[i], mmxBLines);
	}
	StoreMatrix4x4(Out, mmxMatrixA);
}

void ZEMatrix4x4::Sub(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	__m128 mmxBLines;
	__m128 mmxMatrixA[4];
	LoadMatrix4x4(mmxMatrixA, A);

	for(int i = 0; i < 4; i++)
	{
		mmxBLines = _mm_loadu_ps(B.MA + 4 * i);
		mmxMatrixA[i] = _mm_sub_ps(mmxMatrixA[i], mmxBLines);
	}
	StoreMatrix4x4(Out, mmxMatrixA);
}

void ZEMatrix4x4::Multiply(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B)
{
	zeDebugCheck(&Out == &A, "Parameters Out and A can not be the same variable.");
	zeDebugCheck(&Out == &B, "Parameters Out and B can not be the same variable.");

	// All Registers Allocated
	__m128 mmx4, mmx5, mmx6, mmx7;	// Rest
	__m128 mmxMatrixA[4];			// mmx0, mmx1, mmx2, mmx3

	LoadMatrix4x4Transpose(mmxMatrixA, A);

	float *OutColumnIndex = Out.MA;
	for(int i = 0; i < 4; i++)
	{
		// Load Column of B to mmx7
		mmx7 = _mm_loadu_ps(B.MA + 4 * i);
		
		// 1st Couple
		mmx4 = _mm_mul_ps(mmxMatrixA[0], mmx7);
		mmx5 = _mm_mul_ps(mmxMatrixA[1], mmx7);
		mmx4 = _mm_hadd_ps(mmx4, mmx5);
		mmx4 = _mm_hadd_ps(mmx4, mmx4);					// -, -, Ry, Rx

		// 2nd Couple
		mmx6 = _mm_mul_ps(mmxMatrixA[2], mmx7);
		mmx7 = _mm_mul_ps(mmxMatrixA[3], mmx7);
		mmx6 = _mm_hadd_ps(mmx6, mmx7);
		mmx6 = _mm_hadd_ps(mmx6, mmx6);					// -, -, Rw, Rz
		
		// Joining Couples
		mmx7 = _mm_shuffle_ps(mmx4, mmx6, 0x44);		// Rw, Rz, Ry, Rx

		// Storing the Column to "Out"
		_mm_storeu_ps(OutColumnIndex + 4 * i, mmx7);
	}
}

void ZEMatrix4x4::Scale(ZEMatrix4x4 &Out, const ZEMatrix4x4& A, float s)
{	
	
	__m128 mmxScaleValue = _mm_load1_ps(&s);
	__m128 mmxMatrixA[4];
	LoadMatrix4x4(mmxMatrixA, A);

	for(int i = 0; i < 4; i++)	
		mmxMatrixA[i] = _mm_mul_ps(mmxMatrixA[i], mmxScaleValue);

	StoreMatrix4x4(Out, mmxMatrixA);
}

void ZEMatrix4x4::Transpose(ZEMatrix4x4& Out, const ZEMatrix4x4& Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxMatrix[4];
	LoadMatrix4x4Transpose(mmxMatrix, Matrix);
	StoreMatrix4x4(Out, mmxMatrix);
}

bool ZEMatrix4x4::Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix)
{
	zeDebugCheck(&Out == &Matrix, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxDeterminants;
	__m128 mmxMainDeterminant;

	float Determinant = Matrix.Determinant();
	mmxMainDeterminant = _mm_load1_ps(&Determinant);

	if(Determinant == 0)
		return false;

	// Column1
	ZEMatrix4x4 X;
	ZEMatrix4x4::Create(X, 
		1.0f, Matrix.M12, Matrix.M13, Matrix.M14, 
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34, 
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	
	ZEMatrix4x4 Y;
	ZEMatrix4x4::Create(Y,
		Matrix.M11, 1.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0f, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 Z;
	ZEMatrix4x4::Create(Z,
		Matrix.M11, Matrix.M12, 1.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);
	
	ZEMatrix4x4 W;
	ZEMatrix4x4::Create(W,
		Matrix.M11, Matrix.M12, Matrix.M13, 1.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);

	mmxDeterminants = _mm_set_ps(W.Determinant(), Z.Determinant(), Y.Determinant(), X.Determinant()); 
	_mm_storeu_ps(Out.MA, _mm_div_ps(mmxDeterminants, mmxMainDeterminant));

	// Column 2
	ZEMatrix4x4 K;
	ZEMatrix4x4::Create(K,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		1.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);
	
	ZEMatrix4x4 L;
	ZEMatrix4x4::Create(L,
		Matrix.M11, 0.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 1.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 0.0f, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 M;
	ZEMatrix4x4::Create(M,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 1.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);

	ZEMatrix4x4 N;
	ZEMatrix4x4::Create(N,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 1.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);

	mmxDeterminants = _mm_set_ps(N.Determinant(), M.Determinant(), L.Determinant(), K.Determinant()); 
	_mm_storeu_ps(Out.MA + 4, _mm_div_ps(mmxDeterminants, mmxMainDeterminant));
	
	// Column3
	ZEMatrix4x4 O;
	ZEMatrix4x4::Create(O,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		1.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		0.0f, Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 P;
	ZEMatrix4x4::Create(P,
		Matrix.M11, 0, Matrix.M13, Matrix.M14,
		Matrix.M21, 0, Matrix.M23, Matrix.M24,
		Matrix.M31, 1, Matrix.M33, Matrix.M34,
		Matrix.M41, 0, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 Q;
	ZEMatrix4x4::Create(Q,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 1.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 0.0f, Matrix.M44);

	ZEMatrix4x4 R;
	ZEMatrix4x4::Create(R,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 1.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 0.0f);

	mmxDeterminants = _mm_set_ps(R.Determinant(), Q.Determinant(), P.Determinant(), O.Determinant()); 
	_mm_storeu_ps(Out.MA + 8, _mm_div_ps(mmxDeterminants, mmxMainDeterminant));

	// Column4
	ZEMatrix4x4 S;
	ZEMatrix4x4::Create(S,
		0.0f, Matrix.M12, Matrix.M13, Matrix.M14,
		0.0f, Matrix.M22, Matrix.M23, Matrix.M24,
		0.0f, Matrix.M32, Matrix.M33, Matrix.M34,
		1.0f, Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 T;
	ZEMatrix4x4::Create(T,
		Matrix.M11, 0.0f, Matrix.M13, Matrix.M14,
		Matrix.M21, 0.0f, Matrix.M23, Matrix.M24,
		Matrix.M31, 0.0f, Matrix.M33, Matrix.M34,
		Matrix.M41, 1.0f, Matrix.M43, Matrix.M44);

	ZEMatrix4x4 U;
	ZEMatrix4x4::Create(U,
		Matrix.M11, Matrix.M12, 0.0f, Matrix.M14,
		Matrix.M21, Matrix.M22, 0.0f, Matrix.M24,
		Matrix.M31, Matrix.M32, 0.0f, Matrix.M34,
		Matrix.M41, Matrix.M42, 1.0f, Matrix.M44);

	ZEMatrix4x4 V;
	ZEMatrix4x4::Create(V,
		Matrix.M11, Matrix.M12, Matrix.M13, 0.0f,
		Matrix.M21, Matrix.M22, Matrix.M23, 0.0f,
		Matrix.M31, Matrix.M32, Matrix.M33, 0.0f,
		Matrix.M41, Matrix.M42, Matrix.M43, 1.0f);

	mmxDeterminants = _mm_set_ps(V.Determinant(), U.Determinant(), T.Determinant(), S.Determinant()); 
	_mm_storeu_ps(Out.MA + 12, _mm_div_ps(mmxDeterminants, mmxMainDeterminant));

	return true;
}

void ZEMatrix4x4::Transform(ZEVector3& Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxMatrix[4];
	__m128 mmxVectorX = _mm_load1_ps(&Vector.x);
	__m128 mmxVectorY = _mm_load1_ps(&Vector.y);
	__m128 mmxVectorZ = _mm_load1_ps(&Vector.z);
	LoadMatrix4x4(mmxMatrix, Matrix);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	mmxVectorZ = _mm_mul_ps(mmxVectorZ, mmxMatrix[2]);
	
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorZ = _mm_add_ps(mmxVectorZ, mmxVectorY);
	mmxVectorZ = _mm_add_ps(mmxVectorZ, mmxMatrix[3]);

	StoreFloat3(Out.M, mmxVectorZ);
}

void ZEMatrix4x4::Transform3x3(ZEVector3 &Out, const ZEMatrix4x4& Matrix, const ZEVector3& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxMatrix[3];
	__m128 mmxVectorX = _mm_load1_ps(&Vector.x);
	__m128 mmxVectorY = _mm_load1_ps(&Vector.y);
	__m128 mmxVectorZ = _mm_load1_ps(&Vector.z);
	mmxMatrix[0] = _mm_loadu_ps(Matrix.MA);
	mmxMatrix[1] = _mm_loadu_ps(Matrix.MA + 4);
	mmxMatrix[2] = _mm_loadu_ps(Matrix.MA + 8);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	mmxVectorZ = _mm_mul_ps(mmxVectorZ, mmxMatrix[2]);
	
	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorZ = _mm_add_ps(mmxVectorZ, mmxVectorY);

	StoreFloat3(Out.M, mmxVectorZ);
}

void ZEMatrix4x4::Transform(ZEVector4& Out, const ZEMatrix4x4& Matrix, const ZEVector4& Vector)
{
	zeDebugCheck(&Out == &Vector, "Parameters Out and Matrix can not be the same variable.");

	__m128 mmxMatrix[4];
	__m128 mmxVectorX = _mm_load1_ps(&Vector.x);
	__m128 mmxVectorY = _mm_load1_ps(&Vector.y);
	__m128 mmxVectorZ = _mm_load1_ps(&Vector.z);
	__m128 mmxVectorW = _mm_load1_ps(&Vector.w);
	LoadMatrix4x4(mmxMatrix, Matrix);

	mmxVectorX = _mm_mul_ps(mmxVectorX, mmxMatrix[0]);
	mmxVectorY = _mm_mul_ps(mmxVectorY, mmxMatrix[1]);
	mmxVectorZ = _mm_mul_ps(mmxVectorZ, mmxMatrix[2]);
	mmxVectorW = _mm_mul_ps(mmxVectorW, mmxMatrix[3]);
	

	mmxVectorY = _mm_add_ps(mmxVectorY, mmxVectorX);
	mmxVectorZ = _mm_add_ps(mmxVectorZ, mmxVectorY);
	mmxVectorW = _mm_add_ps(mmxVectorZ, mmxVectorW);

	_mm_storeu_ps(Out.M, mmxVectorW);
}
