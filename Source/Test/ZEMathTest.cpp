//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMathTest.cpp
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

#include <stdio.h>
#include <math.h>
#include <d3dx9.h>
#include "ZEMath/ZEMath.h"

#define MATHLIB_FLOATTRESHOLD 0.00001f

void PrintVector(float x, float y, float z)
{
	printf("V[%lf %lf, %lf]", x, y, z);
}

void PrintVector(const ZEVector3& Vect)
{
	printf("V[%lf %lf, %lf]", Vect.x, Vect.y, Vect.z);
}

void PrintMatrix(const ZEMatrix4x4& Matrix)
{
	printf("\tM");
	for (int I = 0; I < 4; I++)
	{
		printf("%s[ ", (I != 0 ? "\t ": ""));	
		for (int N = 0; N < 4; N++)
			printf("%lf ", Matrix.M[I][N]);
		printf("]\n");
	}
}

void PrintD3DXMatrix(const D3DXMATRIX& Matrix)
{
	printf("\tM");
	for (int I = 0; I < 4; I++)
	{
		printf("%s[ ", (I != 0 ? "\t ": ""));
		for (int N = 0; N < 4; N++)
			printf("%lf ", Matrix.m[I][N]);
		printf("]\n");
	}
}

void PrintQuaternion(const ZEQuaternion& Quot)
{
	printf("Q[%lf %lf %lf, %lf]", Quot.w, Quot.x, Quot.y, Quot.z);
}
void PrintQuaternion(float w, float x, float y, float z)
{
	printf("Q[%lf %lf %lf, %lf]", w, x, y, z);
}

bool CheckVector(const ZEVector3& Vect, float x, float y, float z)
{
	if (fabs(Vect.x - x) < MATHLIB_FLOATTRESHOLD)
		if (fabs(Vect.y - y) < MATHLIB_FLOATTRESHOLD)
			if (fabs(Vect.z - z) < MATHLIB_FLOATTRESHOLD)
			{
				printf("PASSED.\n  Referance : V[%lf %lf %lf]\n  Output    : ", x, y, z);
				PrintVector(Vect);
				printf("\n");
				return true;
			}
	
	printf("FAILED !!!\n  Referance : V[%lf %lf %lf]\n  Output    : ", x, y, z);
	PrintVector(Vect);
	printf("\n");
	return false;
}

bool CheckVector(const ZEVector3& Vect, const D3DXVECTOR3& RefVect)
{
	if (fabs(Vect.x - RefVect.x) < MATHLIB_FLOATTRESHOLD)
		if (fabs(Vect.y - RefVect.y) < MATHLIB_FLOATTRESHOLD)
			if (fabs(Vect.z - RefVect.z) < MATHLIB_FLOATTRESHOLD)
			{
				printf("PASSED.\n  Referance : V[%lf %lf %lf]\n  Output    : ", RefVect.x, RefVect.y, RefVect.z);
				PrintVector(Vect);
				printf("\n");
				return true;
			}
	
	printf("FAILED !!!\n  Referance : V[%lf %lf %lf]\n  Output    : ", RefVect.x, RefVect.y, RefVect.z);
	PrintVector(Vect);
	printf("\n");
	return false;
}

bool CheckMatrix(const ZEMatrix4x4& Matrix, const D3DXMATRIX& RefMatrix)
{
	for (int I = 0; I < 4; I++)
		for (int N = 0; N < 4; N++)
			if (fabs(Matrix.M[I][N] - RefMatrix.m[I][N]) > MATHLIB_FLOATTRESHOLD)
			{
					printf("FAILED !!!\n  Referance :\n");
					PrintD3DXMatrix(RefMatrix);
					printf("  Output    :\n");
					PrintMatrix(Matrix);		
					printf("\n");
					return false;
			}
	printf("PASSED.\n  Referance :\n");
	PrintD3DXMatrix(RefMatrix);
	printf("  Output    :\n");
	PrintMatrix(Matrix);		
	printf("\n");
	return true;
}

bool CheckQuaternion(const ZEQuaternion& Quat, float w, float x, float y, float z)
{
	if (fabs(Quat.x - x) < MATHLIB_FLOATTRESHOLD)
		if (fabs(Quat.y - y) < MATHLIB_FLOATTRESHOLD)
			if (fabs(Quat.z - z) < MATHLIB_FLOATTRESHOLD)
				if (fabs(Quat.w - w) < MATHLIB_FLOATTRESHOLD)
				{
					printf("PASSED.\n  Referance : ");
					PrintQuaternion(w, x, y, z);
					printf("\n  Output    : ");
					PrintQuaternion(Quat);
					printf("\n");
					return true;
				}

	printf("FAILED !!!\n  Referance : ");
	PrintQuaternion(w, x, y, z);
	printf("\n  Output    : ");
	PrintQuaternion(Quat);
	printf("\n");

	return false;
}

bool CheckQuaternion(const ZEQuaternion& Quat, const D3DXQUATERNION& RefQuat)
{
	if (fabs(Quat.x - RefQuat.x) < MATHLIB_FLOATTRESHOLD)
		if (fabs(Quat.y - RefQuat.y) < MATHLIB_FLOATTRESHOLD)
			if (fabs(Quat.z - RefQuat.z) < MATHLIB_FLOATTRESHOLD)
				if (fabs(Quat.w - RefQuat.w) < MATHLIB_FLOATTRESHOLD)
				{
					printf("PASSED.\n  Referance : ");
					PrintQuaternion(RefQuat.w, RefQuat.x, RefQuat.y, RefQuat.z);
					printf("\n  Output    : ");
					PrintQuaternion(Quat);
					printf("\n");
					return true;
				}

	printf("FAILED !!!\n  Referance : ");
	PrintQuaternion(RefQuat.w, RefQuat.x, RefQuat.y, RefQuat.z);
	printf("\n  Output    : ");
	PrintQuaternion(Quat);
	printf("\n");

	return false;
}

bool TestVector()
{
	ZEVector3 A(1.0f, 2.0f, 3.0f), B(4.0f, 5.0f, 6.0f), C;
	D3DXVECTOR3 DA, DB, DC;

	bool Succeed = true;
	printf("\n Vector Test in progress\n----------------------------------------------------\n");
	// ZEVector3()
	{
		printf ("ZEVector3(float x, float y, float z) -> ");

		ZEVector3 X;
		
		Succeed = CheckVector(X, 0.0f, 0.0f, 0.0f)& Succeed;
	}

	// ZEVector3(float x, float y, float z);
	{	
		printf ("ZEVector3(float x, float y, float z) -> ");

		ZEVector3 X(1.0f, 2.0f, 3.0f);

		Succeed = CheckVector(X, 1.0f, 2.0f, 3.0f)&Succeed;
			
	}

	// ZEVector3(const ZEVector& Start, const ZEVector& End);
	{	
		printf ("ZEVector3(const ZEVector& Start, const ZEVector& End) -> ");

		ZEVector3 X(ZEVector3(1.0f, 2.0f, 3.0f), ZEVector3(4.0f, 6.0f, 8.0f));

		Succeed = CheckVector(X, 3.0f, 4.0f, 5.0f)&Succeed;
		
	}

	// void Create(float x, float y, float z);
	{	
		printf ("void Create(float x, float y, float z -> ");

		ZEVector3 X;
		ZEVector3::Create(X, 1.0f, 2.0f, 3.0f);

		Succeed = CheckVector(X, 1.0f, 2.0f, 3.0f)& Succeed; 
	}
	// void Create(const ZEVector &Start, const ZEVector &End);
	{	
		printf ("void Create(const ZEVector &Vstart, const ZEVector &Vend) -> ");
		ZEVector3 X(ZEVector3(1.0f, 2.0f, 3.0f), ZEVector3(4.0f, 6.0f, 8.0f));

		Succeed = CheckVector(X, 3.0f, 4.0f, 5.0f) &Succeed;
	}

	//static void Add(ZEVector& Out, const ZEVector& A, const ZEVector& B)
	{	
		printf("static void Add(ZEVector& Out, const ZEVector& A, const ZEVector& B) -> ");

		ZEVector3 A(1.0, 2.0, 3.0), B(2.0, 3.0, 4.0), C;
		ZEVector3::Add(C, A, B);

		Succeed = CheckVector(C, D3DXVECTOR3(1, 2, 3) + D3DXVECTOR3(2, 3, 4))&Succeed;
	}

	//static void Sub(ZEVector& Out, const ZEVector& A, const ZEVector& B)
	{	
		printf("static void Sub(ZEVector& Out, const ZEVector& A, const ZEVector& B) -> ");

		ZEVector3 A(1.0, 2.0, 3.0), B(2.0, 3.0, 4.0), C;
		ZEVector3::Sub(C, A, B);

		Succeed = CheckVector(C, D3DXVECTOR3(1, 2, 3) - D3DXVECTOR3(2, 3, 4))&Succeed;
	}

	// static void Scale(ZEVector& Out, const ZEVector& A, float s);
	{
		printf("static void Scale(ZEVector& Out, const ZEVector& A, float s); -> ");

		ZEVector3 A(1.0f, 2.0f, 3.0f), B;
		ZEVector3::Scale(B, A, 10.5);
		
		Succeed =CheckVector(B, D3DXVECTOR3(1, 2, 3) * 10.5)&Succeed;
	}

	// static float	DotProduct(const ZEVector& A, const ZEVector& B)
	{
		printf("static float DotProduct(const ZEVector& A, const ZEVector& B); -> ");
		
		if(ZEVector3::DotProduct(ZEVector3(1.0, 2.0, 3.0), ZEVector3(2.0, 3.0, 4.0)) == D3DXVec3Dot(&D3DXVECTOR3(1.0, 2.0, 3.0), &D3DXVECTOR3(2.0, 3.0, 4.0)))
			printf("PASS.\n");
		else
			Succeed = false;
	}

	//static void CrossProduct(ZEVector& Out, const ZEVector& A, const ZEVector& B);
	{
		printf("static void CrossProduct(ZEVector& Out, const ZEVector& A, const ZEVector& B) -> ");

		D3DXVECTOR3 DA(1.0, 2.0, 3.0), DB(4.0, 5.0, 6.0), DO;
		D3DXVec3Cross(&DO, &DA, &DB);

		ZEVector3 A(1.0, 2.0, 3.0), B(4.0, 5.0, 6.0), C;
		ZEVector3::CrossProduct(C, A, B);

		Succeed =CheckVector(C, DO)&Succeed;
	}
	

	// static void Normalize(ZEVector& Out, const ZEVector& Vector);	
	{
		printf("static void Normalize(ZEVector& Out, const ZEVector& Vector) -> ");

		D3DXVECTOR3 DO;
		D3DXVec3Normalize(&DO, &D3DXVECTOR3(1.0, 2.0, 3.0));

		ZEVector3(1.0, 2.0, 3.0), B;
		ZEVector3::Normalize(B, A);

		Succeed =CheckVector(B, DO)&Succeed;
	
	}

	// static float Length(const ZEVector& Vector);
	{
		printf("static float Length(const ZEVector& Vector) -> ");

		if (ZEVector3::Length(ZEVector3(1,2,3)) == D3DXVec3Length(&D3DXVECTOR3(1.0, 2.0, 3.0)))
			printf("PASS.\n");
		else
			Succeed = false;
	}

	// static float Distance(const ZEVector& A, const ZEVector& B);
	{
		printf("static float Distance(const ZEVector& A, const ZEVector& B) -> ");
		//if (ZEVector::Length(ZEVector3(1,2,3)) == D3DXVec3Length(&D3DXVECTOR3(1.0, 2.0, 3.0)))
			printf("NOT TESTED !!!\n");
		//else
		//	Succeed = false;
	}
	// static float DistanceSquare(const ZEVector& A, const ZEVector& B);	
	{
		printf("static float DistanceSquare(const ZEVector& A, const ZEVector& B) -> ");

		//if (ZEVector::Length(ZEVector3(1,2,3)) == D3DXVec3Length(&D3DXVECTOR3(1.0, 2.0, 3.0)))
			printf("NOT TESTED !!!\n");
		//else
		//	Succeed = false;
	}
	// static void LineerInterpolate(ZEVector& Out, const ZEVector& A, const ZEVector& B, float Factor);
	{
		printf("static void LineerInterpolate(ZEVector& Out, const ZEVector& A, const ZEVector& B, float Factor) -> ");

		//if (ZEVector::Length(ZEVector3(1,2,3)) == D3DXVec3Length(&D3DXVECTOR3(1.0, 2.0, 3.0)))
			printf("NOT TESTED !!!\n");
		//else
		//	Succeed = false;
	}
	if (Succeed == true)
		printf("\nVector test completed successfully.\n\n");
	else
		printf("\nVector test failed. Check failed functions.\n\n");

	return Succeed;
}


bool TestMatrix()
{
	bool Succeed = true;
	printf("\n Matrix Test in progress\n----------------------------------------------------\n");
	
	//void Create(float M00, ... float M33)
	{
		printf("void Create(float M00, ... float M33) -> ");

		ZEMatrix4x4 A;
		D3DXMATRIX B(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
		
		ZEMatrix4x4::Create(A, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);

		Succeed = CheckMatrix(A, B)& Succeed;
	}

	//void Create(float M[4][4])
	{	
		printf("void Create(float M[4][4]) -> ");

		float Referance[]= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
		float Referance2[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
		D3DXMATRIX B(Referance2);

		ZEMatrix4x4 A;
		ZEMatrix4x4::Create(A, Referance);

		Succeed = CheckMatrix(A, B)& Succeed;
	}

	//static void Add(TMatrix &Out, const TMatrix &A, const TMatrix &B);
	{
		printf("static void Add(TMatrix &Out, const TMatrix &A, const TMatrix &B) -> ");
			
		ZEMatrix4x4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), B(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1), C;
		ZEMatrix4x4::Add(C, A, B);
			
		Succeed = (CheckMatrix(C, 
				D3DXMATRIX(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) + 
				D3DXMATRIX(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)))& Succeed;
	}


	//static void Sub(TMatrix &Out, const TMatrix &A, const TMatrix &B);
	{	printf("static void Sub(TMatrix &Out, const TMatrix &A, const TMatrix &B) -> ");
			
		ZEMatrix4x4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), B(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1),C;
		ZEMatrix4x4::Sub(C, A, B);
		 
				
			Succeed = (CheckMatrix(C, 
				D3DXMATRIX(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) - 
				D3DXMATRIX(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)))& Succeed;
	}

	//static void Product(TMatrix &Out, const TMatrix &A, const TMatrix &B);
	{	printf("static void Product(TMatrix &Out, const TMatrix &A, const TMatrix &B) -> ");
			
		ZEMatrix4x4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), B(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1),C;
		ZEMatrix4x4::Multiply(C, A, B);
						
		Succeed = (CheckMatrix(C, 
				D3DXMATRIX(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) *
				D3DXMATRIX(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)))& Succeed;
	}


	//static void Scale(TMatrix &Out, const TMatrix &A, float s);
	{
		printf("static void Scale(TMatrix &Out, const TMatrix &A, float s) -> ");
		ZEMatrix4x4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),C;
	
		ZEMatrix4x4::Scale(C, A, 10.5);
		
		Succeed = CheckMatrix(C,D3DXMATRIX(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) * 10.5)& Succeed;
	}

	//	static void Transpose(TMatrix &Out, const TMatrix &Matrix);
	{
		printf("void Transpose() -> ");
		ZEMatrix4x4 A(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), C;
		ZEMatrix4x4::Transpose(C, A);
		D3DXMATRIX DO;
		D3DXMatrixTranspose(&DO,&D3DXMATRIX(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16));
		Succeed = CheckMatrix(C,DO)& Succeed;
	}

	//static void Transform(ZEVector& Out, TMatrix &Matrix, ZEVector& Vector)
	{
		printf("static void Transform(ZEVector& Out, TMatrix &Matrix, ZEVector& Vector) -> ");
		ZEMatrix4x4 TA;
		ZEMatrix4x4::Create(TA, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

		D3DXMATRIX DT(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
		D3DXVECTOR3 DA(1.0, 2.0, 3.0);
		D3DXVECTOR4 DO;
		D3DXVec3Transform(&DO, &DA, &DT);

		ZEVector3 A(1.0, 2.0, 3.0),C;
		ZEMatrix4x4::Transform(C, A, TA);

		Succeed =CheckVector(C, DO.x, DO.y, DO.z)&Succeed;
	}

	//void LoadIdentity();
	{
		printf("void LoadIdentity() -> ");
		ZEMatrix4x4 A;
		ZEMatrix4x4::CreateIdentity(A);
		D3DXMATRIX DI;
		D3DXMatrixIdentity(&DI);
		Succeed = CheckMatrix(A,DI)& Succeed;	
	}

	//void LoadZero();
	{
		printf("void LoadZero() -> ");
		ZEMatrix4x4 A;
		ZEMatrix4x4::CreateZero(A);
		
		
		Succeed = CheckMatrix(A,D3DXMATRIX(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0))& Succeed;
	}

	if (Succeed == true)
		printf("\nMatrix test completed successfully.\n\n");
	else
		printf("\nMatrix test failed. Check failed functions.\n\n");

	return Succeed;
}

bool TestQuaternion()
{
	bool Succeed = true;
	
	printf("\n Quaternion Test in progress\n----------------------------------------------------\n");

	//void Create(float w, float x, float y, float z);
	{
		printf("void Create(float w, float x, float y, float z) -> ");
		
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, 5, 1, 2, 3);
		Succeed = CheckQuaternion(Q, 5, 1, 2, 3) & Succeed;
	}

	//void Create(float w, const ZEVector& Axis)
	{
		printf("void Create(float w, const ZEVector& Axis) -> ");
		
		ZEVector3 A;
		ZEVector3::Create(A, 1, 2, 3);
		ZEVector3::Normalize(A, A);
		ZEQuaternion Q(5, A);
		D3DXQUATERNION DQ;

		D3DXQuaternionRotationAxis(&DQ,  &D3DXVECTOR3(1,2,3), 5);
		Succeed = CheckQuaternion(Q, DQ) & Succeed;
	}

	//void Create(float Yaw, float Pitch, float Roll);
	{
		printf("void Create(float Yaw, float Pitch, float Roll) -> ");
		
		ZEQuaternion A;
		ZEQuaternion::Create(A, 1, 2, 3);

		D3DXQUATERNION DA;
		D3DXQuaternionRotationYawPitchRoll(&DA, 2, 1, 3);
		
		Succeed = CheckQuaternion(A, DA) & Succeed;
	}

	//void Normalize();
	{
		printf("void Normalize() -> ");
		
		ZEQuaternion A(1,2,3,4);
		A.Normalize();

		D3DXQUATERNION DA(2, 3, 4, 1), DO;
		D3DXQuaternionNormalize(&DO, &DA);
		
		Succeed = CheckQuaternion(A, DO) & Succeed;
	}

	//void Normalize(TQuaternion& Output) const;
	{
		printf("void Normalize(TQuaternion& Output) const -> ");
		
		ZEQuaternion A(1,2,3,4), B;
		ZEQuaternion::Normalize(B, A);

		D3DXQUATERNION DA(2, 3, 4, 1), DO;
		D3DXQuaternionNormalize(&DO, &DA);
		
		Succeed = CheckQuaternion(B, DO) & Succeed;
	}

	//void LoadIdentity()
	{
		printf("void LoadIdentity() -> ");
		D3DXQUATERNION Ref;
		D3DXQuaternionIdentity(&Ref);

		ZEQuaternion Q;
		ZEQuaternion::CreateIdentity(Q);

		Succeed = CheckQuaternion(Q, Ref) & Succeed;
	}

	//void Product(const TQuaternion& Other, TQuaternion& Output) const
	{
		printf("void Product(const TQuaternion& Other, TQuaternion& Output) const -> ");
		
		D3DXQUATERNION DA(2,3,4,1), DB(6, 7, 8, 5), DC;
		D3DXQuaternionNormalize(&DA, &DA);
		D3DXQuaternionNormalize(&DB, &DB);
		D3DXQuaternionMultiply(&DC, &DB, &DA);

		ZEQuaternion A(1, 2, 3, 4), B(5, 6, 7, 8), C;
		A.Normalize(); 
		B.Normalize();

		ZEQuaternion::Product(C, A, B);

		Succeed = CheckQuaternion(C,  DC) & Succeed;
	}

	//void VectorProduct(const ZEVector& Other, ZEVector& Output) const
	{
		printf("void VectorProduct(const ZEVector& Other, ZEVector& Output) const -> ");
		Succeed = false;
		ZEQuaternion A(1, 2, 3, 4);
		A.Normalize();

		ZEVector3 V(5, 6, 7), Out;
		ZEQuaternion::Transform(Out, V, A);

		D3DXQUATERNION DQ(2,3,4,1), DQP, DV(5,6,7,0), DO;
		D3DXQuaternionNormalize(&DQ, &DQ);
		D3DXQuaternionInverse(&DQP, &DQ);
		DO = DQP * DV * DQ;

		CheckVector(Out, D3DXVECTOR3(DO.x, DO.y, DO.z));
	}

	//void Inverse();
	{
		printf("void Inverse() -> ");
		
		ZEQuaternion A(1,2,3,4);
		A.Normalize();
		A.Conjugate();

		D3DXQUATERNION DA(2, 3, 4, 1), DO;
		D3DXQuaternionNormalize(&DA, &DA);
		D3DXQuaternionInverse(&DO, &DA);
		
		Succeed = CheckQuaternion(A, DO) & Succeed;
	}

	//void Inverse(TQuaternion& Output) const;
	{
		printf("void Inverse(TQuaternion& Output) const -> ");
		
		ZEQuaternion A(1,2,3,4), B;
		A.Normalize();
		ZEQuaternion::Conjugate(B, A);

		D3DXQUATERNION DA(2, 3, 4, 1), DO;
		D3DXQuaternionNormalize(&DA, &DA);
		D3DXQuaternionInverse(&DO, &DA);
		
		Succeed = CheckQuaternion(B, DO) & Succeed;
	}
	
	//void Interpolate(TQuaternion& Other, float I, TQuaternion& Output) const;
	{
		/*printf("void Normalize() -> ");
		
		TQuaternion A(1,2,3,4);
		A.Normalize();

		D3DXQUATERNION DA(2, 3, 4, 1), DO;
		D3DXQuaternionNormalize(&DO, &DA);
		
		Succeed = CheckQuaternion(A, DO) & Succeed;*/
	}

	//void Interpolate(TQuaternion& Other, float I);
	{
	}
	


	//void ConvertToRotationMatrix(TMatrix& Output) const;
	{
		printf("void ConvertToRotationMatrix(TMatrix& Output) const -> ");
		
		ZEQuaternion A(1,2,3,4);
		ZEMatrix4x4 M;
		A.Normalize();
		ZEQuaternion::ConvertToRotationMatrix(M, A);

		D3DXQUATERNION DA(2, 3, 4, 1);
		D3DXQuaternionNormalize(&DA, &DA);
		D3DXMATRIX DM;
		D3DXMatrixRotationQuaternion(&DM, &DA);
		
		Succeed = CheckMatrix(M, DM) & Succeed;
	}

	if (Succeed == true)
		printf("\nQuaternion test completed successfully.\n\n");
	else
		printf("\nQuaternion test failed. Check failed functions.\n\n");

	return Succeed;
}

bool TestMath()
{
	bool Succeed = true;
	Succeed = TestVector() & Succeed;
	Succeed = TestMatrix() & Succeed;
	Succeed = TestQuaternion() & Succeed;
	return Succeed;
}
