//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrixTests.cpp
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

#include "ZETest.h"
#include <d3dx9.h>
#include <math.h>
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMathDefinitions.h"

       
ZETestSuiteAdd(ZEMatrix3x3)
{
	ZETestItemAdd(MTRX3Constructor)
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		CHECK_EQUAL(A, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));

		float M[9]={1.0f, 3.0f, 13.0f, 1.0f, 5.0f, 21.0f, 2.0f,  8.0f,  34.0f};
		ZEMatrix3x3 B(M);
		CHECK_EQUAL(B, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZETestItemAdd(MTRX3Create)
	{
		ZEMatrix3x3 A, B;
		ZEMatrix3x3::Create(A, 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		CHECK_EQUAL(A, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));

		float M[9]={34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f};
		ZEMatrix3x3::Create(B, M);
		CHECK_EQUAL(B, ZEMatrix3x3(34.0f, 8.0f, 2.0f ,21.0f, 5.0f, 1.0f ,13.0f, 3.0f, 1.0f));
	}

	ZETestItemAdd(MTRX3CreateIdentity)
	{
		ZEMatrix3x3 A;
		ZEMatrix3x3::CreateIdentity(A);

		CHECK_EQUAL(A, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZETestItemAdd(MTRX3CreateRotation)
	{
		
	}

	ZETestItemAdd(MTRX3CreateRotationX)
	{
		

	}

	ZETestItemAdd(MTRX3CreateRotationY)
	{

	}

	ZETestItemAdd(MTRX3CreateRotationZ)
	{

	}

	ZETestItemAdd(MTRX3CreateRotationXYZ)
	{

	}

	ZETestItemAdd(MTRX3CreateScale)
	{
		ZEMatrix3x3 A;
		ZEMatrix3x3::CreateScale(A, 1.0f, 2.0f, 3.0f);
		CHECK_EQUAL(A, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 3.0f));
	}

	ZETestItemAdd(MTRX3CreateZero)
	{
		ZEMatrix3x3 A;
		ZEMatrix3x3::CreateZero(A);

		CHECK_EQUAL(A, ZEMatrix3x3(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	}

	ZETestItemAdd(MTRX3Add)
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f);
		ZEMatrix3x3 B( 1.0f,  2.0f,  3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		ZEMatrix3x3 C;

		ZEMatrix3x3::Add(C, A, B);

		CHECK_EQUAL(C, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));
	}

	ZETestItemAdd(MTRX3Sub)
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f);
		ZEMatrix3x3 B( 1.0f,  2.0f,  3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		ZEMatrix3x3 C;

		ZEMatrix3x3::Sub(C, A, B);

		CHECK_EQUAL(C, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}

	ZETestItemAdd(MTRX3Multiply)
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f);
		ZEMatrix3x3 B( 1.0f,  1.0f,  2.0f,  3.0f,  5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		ZEMatrix3x3 C;
		ZEMatrix3x3 D;
		ZEMatrix3x3 I;

		ZEMatrix3x3::CreateIdentity(I);

		ZEMatrix3x3::Multiply(C, A, B);

		CHECK_EQUAL(C, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.f));

		ZEMatrix3x3::Multiply(D, A, I);

		CHECK_EQUAL(D, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f));
	}

	ZETestItemAdd(MTRX3Scale)
	{
		ZEMatrix3x3 A(1.0f,  1.0f,  2.0f,  3.0f,  5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		ZEMatrix3x3 B;
		float c = 2.0f;

		ZEMatrix3x3::Scale(B, A, c);

		CHECK_EQUAL(B, ZEMatrix3x3(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f ));
		
	}

	ZETestItemAdd(MATRX3Transpose)
	{
		ZEMatrix3x3 A(1.2f, 1.2f, 2.4f, 3.6f, 6.0f, 9.6f, 15.6f, 25.2f, 40.8f);
		ZEMatrix3x3 B;

		ZEMatrix3x3::Transpose(B, A);

		CHECK_EQUAL(B, ZEMatrix3x3(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f));
	}

	ZETestItemAdd(MTRX3Transform)
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f);
		ZEVector3 U;
		ZEVector3 V(1.0f, 2.0f, 3.0f);

		ZEMatrix3x3::Transform(U, A, V);

		CHECK_EQUAL(U, ZEVector3(86.0f, 70.0f, 64.0f));
	}
	ZETestItemAdd(MTRX3Determinant)
	{
		ZEMatrix3x3 A(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f);
		float d = ZEMatrix3x3::Determinant(A);
		CHECK_EQUAL(d, 0.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, -5.0f, 4.0f, 2.0f, 1.0f, 3.0f, 2.0f, 9.0f);
		float dd = ZEMatrix3x3::Determinant(B);
		CHECK_EQUAL(dd, -60.0f);
		
		ZEMatrix3x3 I;
		ZEMatrix3x3::CreateIdentity(I);
		float i = ZEMatrix3x3::Determinant(I);
		CHECK_EQUAL(i, 1.0f);
	}

	ZETestItemAdd(MTRX3Inverse)
	{
		ZEMatrix3x3 A(2.0f, 1.0f, 1.0f, 3.0f, 2.0f, 1.0f, 4.0f, -3.0f, 5.0f);
		ZEMatrix3x3 B; 
		ZEMatrix3x3::Inverse(B, A);

		ZEMatrix3x3 C(-6.5f,  4.0f, 0.5f, 5.5f, -3.0f, -0.5f, 8.5f, -5.0f, -0.5f);
		CHECK_EQUAL(B, C);

		ZEMatrix3x3 I;
		ZEMatrix3x3::CreateIdentity(I);
		ZEMatrix3x3 II;
		ZEMatrix3x3::Inverse(II, I);
		CHECK_EQUAL(II, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}
}
ZETestSuiteAdd(ZEMatrix4x4)
{
	
	ZETestItemAdd(MTRX4Constructor)
	{
		ZEMatrix4x4 A(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f);

		CHECK_EQUAL(A, ZEMatrix4x4( 1.0f, 1.0f,  2.0f,  3.0f,
								    5.0f, 8.0f, 13.0f, 21.0f, 
								   34.0f, 55.0f, 1.0f,  1.0f, 
								    3.0f,  5.0f, 8.0f, 13.0f));

		float M[16] = {1.0f, 5.0f, 34.0f, 3.0f, 1.0f, 8.0f, 55.0f, 5.0f, 2.0f, 13.0f, 1.0f, 8.0f, 3.0f, 21.0f, 1.0f, 13.0f};
		ZEMatrix4x4 B(M);

		CHECK_EQUAL(B, ZEMatrix4x4( 1.0f,  1.0f,  2.0f,  3.0f, 
								    5.0f,  8.0f, 13.0f, 21.0f, 
								   34.0f, 55.0f,  1.0f,  1.0f, 
								    3.0f,  5.0f,  8.0f, 13.0f));
	}

	ZETestItemAdd(MTRX4Create)
	{
		ZEMatrix4x4 A, B;
		ZEMatrix4x4::Create(A, 1.0f,  1.0f,  2.0f,  3.0f, 
							   5.0f,  8.0f, 13.0f, 21.0f, 
							  34.0f, 55.0f,  1.0f,  1.0f, 
							   3.0f,  5.0f,  8.0f, 13.0f);
		
		CHECK_EQUAL(A, ZEMatrix4x4(1.0f,  1.0f,  2.0f,  3.0f,
								   5.0f,  8.0f, 13.0f, 21.0f, 
								  34.0f, 55.0f,  1.0f,  1.0f,
								   3.0f,  5.0f,  8.0f, 13.0f));

		float M[16]={13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f, 34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f};
		ZEMatrix4x4::Create(B, M);
		CHECK_EQUAL(B, ZEMatrix4x4(13.0f,  2.0f, 21.0f, 3.0f, 
									8.0f,  1.0f, 13.0f, 2.0f, 
									5.0f,  1.0f,  8.0f, 1.0f, 
									3.0f, 34.0f,  5.0f, 1.0f));
	}

	ZETestItemAdd(MTRX4CreateIdentity)
	{
		ZEMatrix4x4 A;
		ZEMatrix4x4::CreateIdentity(A);

		CHECK_EQUAL(A, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 
								   0.0f, 1.0f, 0.0f, 0.0f, 
								   0.0f, 0.0f, 1.0f, 0.0f, 
								   0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZETestItemAdd(MTRX4CreateRotation)
	{
		//ZEMatrix3x3  A(34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f);
		//ZEQuaternion B;
		//ZEMatrix3x3::CreateRotation();
	}

	ZETestItemAdd(MTRX4CreateRotationX)
	{
		ZEMatrix4x4 M;
		ZEMatrix4x4 A( 1.0f,  0.0f,        0.0f,       0.0f,
					   0.0f, -0.44807363,  0.89399666, 0.0f,
					   0.0f, -0.89399666, -0.44807363, 0.0f,
					   0.0f,  0.0f,        0.0f,       1.0f);
		
		ZEMatrix4x4::CreateRotationX(M, 90);

		CHECK_EQUAL(M, A);
		
	}

	ZETestItemAdd(MTRX4CreateRotationY)
	{
		ZEMatrix4x4 M;
		ZEMatrix4x4 A(-0.44807363f, 0.0f, -0.89399666f, 0.0f,
					   0.0f,        1.0f,  0.0f,        0.0f,
					   0.89399666f, 0.0f, -0.44807363f, 0.0f,
					   0.0f,        0.0f,  0.0f,        1.0f);

		ZEMatrix4x4::CreateRotationY(M, 90);

		CHECK_EQUAL(M, A);
	}
	ZETestItemAdd(MTRX4CreateRotationZ)
	{
		ZEMatrix4x4 M;
		ZEMatrix4x4 A(-0.44807363f,  0.89399666f, 0.0f,   0.0f,
					  -0.89399666f, -0.44807363f, 0.0f,   0.0f,
					   0.0f,         0.0f,        1.0f,   0.0f,
					   0.0f,         0.0f,        0.0f,   1.0f);

		ZEMatrix4x4::CreateRotationZ(M, 90);

		CHECK_EQUAL(M, A);
	}

	ZETestItemAdd(MTRX4CreateRotationXYZ)
	{

	}
	ZETestItemAdd(MTRX4CreateOffset)
	{
		ZEMatrix4x4 M;
		ZEVector3 V(1.0f, 2.0f, 3.0f);
		ZEQuaternion Q(1.0f, 1.0f, 2.0f, 3.0f);
		ZEMatrix4x4::CreateOffset(M, V, Q);
	}
	ZETestItemAdd(MTRX4CreateScale)
	{
		ZEMatrix4x4 A;
		ZEMatrix4x4::CreateScale(A, 1.0f, 2.0f, 3.0f);
		CHECK_EQUAL(A, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 
								   0.0f, 2.0f, 0.0f, 0.0f, 
								   0.0f, 0.0f, 3.0f, 0.0f, 
								   0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZETestItemAdd(MTRX4CreateZero)
	{
		ZEMatrix4x4 A;
		ZEMatrix4x4::CreateZero(A);

		CHECK_EQUAL(A, ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 0.0f, 0.0f, 
								   0.0f, 0.0f, 0.0f, 0.0f, 
								   0.0f, 0.0f, 0.0f, 0.0f));
	}

	ZETestItemAdd(MTRX4Add)
	{
		ZEMatrix4x4 A( 13.0f,  8.0f, 5.0f,  3.0f,
					    2.0f,  1.0f, 1.0f, 34.0f,
					   21.0f, 13.0f, 8.0f,  5.0f,  
					    3.0f,  2.0f, 1.0f,  1.0f);

		ZEMatrix4x4 B( 1.0f,  2.0f,  3.0f,  4.0f,
					   5.0f,  6.0f,  7.0f,  8.0f, 
					   9.0f, 10.0f, 11.0f, 12.0f, 
					  13.0f, 14.0f, 15.0f, 16.0f);
		ZEMatrix4x4 C;

		ZEMatrix4x4::Add(C, A, B);

		CHECK_EQUAL(C, ZEMatrix4x4(14.0f, 10.0f,  8.0f,  7.0f,
									7.0f,  7.0f,  8.0f, 42.0f, 
								   30.0f, 23.0f, 19.0f, 17.0f, 
								   16.0f, 16.0f, 16.0f, 17.0f));
	}

	ZETestItemAdd(MTRX4Sub)
	{
		ZEMatrix4x4 A(13.0f,  8.0f, 5.0f,  3.0f,
					   2.0f,  1.0f, 1.0f, 34.0f, 
					  21.0f, 13.0f, 8.0f,  5.0f,  
					   3.0f,  2.0f, 1.0f,  1.0f);

		ZEMatrix4x4 B( 1.0f,  2.0f,  3.0f,  4.0f,
					   5.0f,  6.0f,  7.0f,  8.0f,
					   9.0f, 10.0f, 11.0f, 12.0f, 
					  13.0f, 14.0f, 15.0f, 16.0f);
		ZEMatrix4x4 C;

		ZEMatrix4x4::Sub(C, A, B);

		CHECK_EQUAL(C, ZEMatrix4x4( 12.0f,   6.0f,   2.0f,  -1.0f,
								    -3.0f,  -5.0f,  -6.0f,  26.0f, 
								    12.0f,   3.0f,  -3.0f,  -7.0f, 
								   -10.0f, -12.0f, -14.0f, -15.0f));
	}

	ZETestItemAdd(MTRX4Multiply)
	{
		ZEMatrix4x4 A( 35.0f, 23.0f, 16.0f, 12.0f,
					   10.0f,  9.0f,  9.0f,  9.0f, 
					   10.0f,  1.0f,  2.0f,  3.0f, 
					    4.0f,  5.0f,  6.0f,  7.0f);
		
		ZEMatrix4x4 B(  1.0f,  1.0f,  2.0f,  3.0f,  
					    5.0f,  8.0f, 13.0f, 21.0f,
					   34.0f, 55.0f,  1.0f,  1.0f, 
					    2.0f,  3.0f,  5.0f,  8.0f);
		ZEMatrix4x4 C;
		ZEMatrix4x4 D;
		ZEMatrix4x4 I;

		ZEMatrix4x4::CreateIdentity(I);

		ZEMatrix4x4::Multiply(C, A, B);

		CHECK_EQUAL(C, ZEMatrix4x4(718.0f, 1135.0f,  445.0f,  700.0f,
						           379.0f,  604.0f,  191.0f,  300.0f,
						            89.0f,  137.0f,   50.0f,   77.0f,
						           247.0f,  395.0f,  114.0f,  179.0f));

		ZEMatrix4x4::Multiply(D, A, I);

		CHECK_EQUAL(D, ZEMatrix4x4( 35.0f, 23.0f, 16.0f, 12.0f,
									10.0f,  9.0f,  9.0f,  9.0f, 
									10.0f,  1.0f,  2.0f,  3.0f, 
									 4.0f,  5.0f,  6.0f,  7.0f));
	}
	ZETestItemAdd(MTRX4Scale)
	{
		ZEMatrix4x4 A( 1.0f,  1.0f,  2.0f,  3.0f,  
					   5.0f,  8.0f, 13.0f, 21.0f, 
					  34.0f, 55.0f,  1.0f,  1.0f, 
					   2.0f,  3.0f,  5.0f,  8.0f);
		ZEMatrix4x4 B;
		
		float c = 2.0f;

		ZEMatrix4x4::Scale(B, A, c);

		CHECK_EQUAL(B, ZEMatrix4x4( 2.0f,   2.0f,  4.0f,  6.0f,
								   10.0f,  16.0f, 26.0f, 42.0f,
								   68.0f, 110.0f,  2.0f,  2.0f, 
								    4.0f,   6.0f, 10.0f, 16.0f));
		
	}

	ZETestItemAdd(MATRX4Transpose)
	{
		ZEMatrix4x4 A( 1.2f, 1.2f,  2.4f,  3.6f, 
					   6.0f, 9.6f, 15.6f, 25.2f, 
					  40.8f, 1.0f,  2.0f,  3.0f, 
					   4.0f, 5.0f,  6.0f,  7.0f);
		ZEMatrix4x4 B;

		ZEMatrix4x4::Transpose(B, A);

		CHECK_EQUAL(B, ZEMatrix4x4(	1.2f,  6.0f, 40.8f,  4.0f,
									1.2f,  9.6f,  1.0f,  5.0f,
									2.4f, 15.6f,  2.0f,  6.0f,
									3.6f, 25.2f,  3.0f,  7.0f));
	}

	ZETestItemAdd(MTRX4Transform)
	{
		ZEMatrix4x4 A( 35.0f, 23.0f, 16.0f, 12.0f,
					   10.0f,  9.0f,  9.0f,  9.0f, 
					   10.0f,  1.0f,  2.0f,  3.0f, 
					    4.0f,  5.0f,  6.0f,  7.0f);
		ZEVector3 U;
		ZEVector3 V(1.0f, 2.0f, 3.0f);

		ZEMatrix4x4::Transform(U, A, V);

		CHECK_EQUAL(U, ZEVector3(89.0f, 49.0f, 46.0f));
	}
	ZETestItemAdd(MTRX4Determinant)
	{
		ZEMatrix4x4 A(1.2f, 3.6f, 15.6f, 0.0f, 
			          1.2f, 6.0f, 25.2f, 0.0f, 
					  2.4f, 9.6f, 40.8f, 0.0f, 
					  0.0f, 0.0f,  0.0f, 1.0f);
		float d = ZEMatrix4x4::Determinant(A);
		
		CHECK_CLOSE(d, 0.0f, 0.00004);
		
		ZEMatrix4x4 B( 35.0f, 23.0f, 16.0f, 12.0f,
					   10.0f,  9.0f,  9.0f,  9.0f, 
					   10.0f,  1.0f,  2.0f,  3.0f, 
					    4.0f,  5.0f,  6.0f,  7.0f);

		float dd = ZEMatrix4x4::Determinant(B);
		
		CHECK_EQUAL(dd, 282.0f);

		ZEMatrix4x4 I;
		
		ZEMatrix4x4::CreateIdentity(I);
		float i = ZEMatrix4x4::Determinant(I);
		
		CHECK_EQUAL(i, 1.0f);

	}

	ZETestItemAdd(MTRX4Inverse)
	{
		ZEMatrix4x4 A( 35.0f, 23.0f, 16.0f, 12.0f,
					   10.0f,  9.0f,  9.0f,  9.0f, 
					   10.0f,  1.0f,  2.0f,  3.0f, 
					    4.0f,  5.0f,  6.0f,  7.0f);
		ZEMatrix4x4 B; 
		ZEMatrix4x4::Inverse(B, A);

		ZEMatrix4x4 C( 0.000000000,  0.042553191,  0.095744681, -0.095744681,
					   0.333333333, -1.574468085, -0.209219858,  1.542553191,
					  -0.666666667,  3.765957447,  0.056737589, -3.723404255,
					   0.333333333, -2.127659574,  0.046099291,  2.287234043);
		CHECK_EQUAL(B, C);

		ZEMatrix4x4 I;
		ZEMatrix4x4::CreateIdentity(I);
		ZEMatrix4x4 II;
		ZEMatrix4x4::Inverse(II, I);
		CHECK_EQUAL(II, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f, 
									0.0f, 0.0f, 0.0f, 1.0f));
	}
}
