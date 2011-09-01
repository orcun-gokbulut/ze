//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuaternionTest.cpp
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

#include <UnitTest/UnitTest++.h>
#include <d3dx9.h>
#include <math.h>
#include "ZEIOStreamMapping.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEDefinitions.h"

SUITE(Quaternion)
{
	TEST(QTRN_Constructor)
	{

	}
	TEST(QTRN_Create1)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, 1.0f, 2.0f, 3.0f, 4.0f);
		CHECK_EQUAL(Q, ZEQuaternion(1.0f, 2.0f, 3.0f, 4.0f));
	}

	TEST(QTRN_Create2)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, ZE_PI, ZEVector3(1.0f, 2.0f, 3.0));
		CHECK_EQUAL(Q, ZEQuaternion(cos(ZE_PI_2), 1.0f, 2.0f, 3.0f));
	}

	TEST(QTRN_Create3)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, ZE_PI, ZE_PI_2, ZE_PI_2);
		CHECK_EQUAL(Q, ZEQuaternion(0.49999997f, 0.49999997f, -0.5f, -0.5f));
	}

	TEST(QTRN_Create4)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, ZEVector3(1.0f, 2.0f, 3.0f), ZEVector3(2.0f, 3.0f, 5.0f));
	}

	TEST(QTRN_CreateIdentity)
	{
		ZEQuaternion Q;
		ZEQuaternion::CreateIdentity(Q);
		CHECK_EQUAL(Q, ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
	}

	TEST(QTRN_Product)
	{
		ZEQuaternion Q;
		ZEQuaternion A(3.0f, 5.0f, 8.0f, 11.0f);
		ZEQuaternion B(1.0f, 1.0f, 2.0f,  3.0f);
		ZEQuaternion::Product(Q, A, B);

		CHECK_EQUAL(Q, ZEQuaternion(-51.0f, 10.0f, 10.0f, 22.0f));
	}

	TEST(QTRN_VectorProduct)
	{
		ZEVector3 Q;
		ZEQuaternion A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector3 V(1.0f, 2.0f, 3.0f);
		ZEQuaternion::VectorProduct(Q, A, V);
	}

	TEST(QTRN_ConvertToEulerAngles)
	{
		float yaw, pitch, roll;
		ZEQuaternion Q(0.5f, 0.5f, 0.5f, 0.5f);
		ZEQuaternion::ConvertToEulerAngles(yaw, pitch, roll, Q);
		

     	CHECK_EQUAL(yaw, ZE_PI_2);
		CHECK_EQUAL(pitch, ZE_PI_2);
		CHECK_EQUAL(roll, 0.0f);
	}

	TEST(QTRN_Conjugate1)
	{
		ZEQuaternion C, Q(1.0f, 2.0f, 3.0f, 4.0f);
		ZEQuaternion::Conjugate(C, Q);
		CHECK_EQUAL(C, ZEQuaternion(1.0f, -2.0f, -3.0f, -4.0f));
	}

	TEST(QTRN_Conjugate2)
	{
		ZEQuaternion A, C(1.0f, 2.0f, 3.0f, 4.0f);
		A = C.Conjugate();
		CHECK_EQUAL(A, ZEQuaternion(1.0f, -2.0f, -3.0f, -4.0f));
	}
	
	TEST(QTRN_Slerp)
	{
		ZEQuaternion P(0.1f, 0.2f, 0.3f, 0.4f);
		ZEQuaternion Q(0.1f, 0.0f, 0.0f, 0.3f);
		ZEQuaternion QOut;
		float t = 0.3;
		ZEQuaternion::Slerp(QOut, P, Q, t);

		CHECK_EQUAL(QOut, ZEQuaternion(0.12755547f, 0.17063320f, 0.25594980f, 0.46798304f));

		ZEQuaternion A(0.3f, 0.0f, 0.0f, 0.0f);
		ZEQuaternion B(0.1f, 0.1f, 0.0f, 0.3f);

		ZEQuaternion QOut2;
		ZEQuaternion::Slerp(QOut2, A, B, t);

		CHECK_EQUAL(QOut2, ZEQuaternion(0.30911696f, 0.044615276f, 0.0f, 0.13384584f));
	}

	TEST(QTRN_ConvertToRotationMatrix)
	{
		ZEQuaternion A(0.5f, 0.5f, 0.5f, 0.5f);
		ZEMatrix4x4 M;
		ZEQuaternion::ConvertToRotationMatrix(M, A);
		
		CHECK_EQUAL(M, ZEMatrix4x4( 0.0f, 1.0f,  0.0f, 0.0f, 
								    0.0f, 0.0f,  1.0f, 0.0f, 
								    1.0f, 0.0f,  0.0f, 0.0f, 
								    0.0f, 0.0f,  0.0f, 1.0f));
	}
	
	TEST(QTRN_Normalize1)
	{
		ZEQuaternion A, Q(0.1f, 0.2f, 0.3f, 0.4f);
		A = Q.Normalize();

		CHECK_EQUAL(A, ZEQuaternion(0.1f/sqrt(0.3f), 0.2f/sqrt(0.3f), 0.3f/sqrt(0.3f), 0.4f/sqrt(0.3f)));

	}

	TEST(QTRN_Normalize2)
	{
		ZEQuaternion P;
		ZEQuaternion Q(0.1f, 0.2f, 0.3f, 0.4f);
		ZEQuaternion::Normalize(P, Q);

		CHECK_EQUAL(P, ZEQuaternion(0.1f/sqrt(0.3f), 0.2f/sqrt(0.3f), 0.3f/sqrt(0.3f), 0.4f/sqrt(0.3f)));
	}

 }
