//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuaternionTests.cpp
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

ZETestSuiteAdd(Quaternion)
{
	ZETestItemAdd(QTRN_Constructor)
	{

	}
	ZETestItemAdd(QTRN_Create1)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, 1.0f, 2.0f, 3.0f, 4.0f);
		ZETestCheckEqual(Q, ZEQuaternion(1.0f, 2.0f, 3.0f, 4.0f));
	}

	ZETestItemAdd(QTRN_Create2)
	{
		ZEQuaternion Q;
		ZEQuaternion::Create(Q, ZE_PI, ZEVector3(1.0f, 2.0f, 3.0).Normalize());
		ZETestCheckEqual(Q, ZEQuaternion(ZE_PI, ZEVector3(1.0f, 2.0f, 3.0).Normalize()));
	}

	ZETestItemAdd(QTRN_Create3)
	{
		ZEQuaternion Q;
		ZEQuaternion::CreateFromEuler(Q, ZE_PI, ZE_PI_2, ZE_PI_2);
		ZETestCheckEqual(Q, ZEQuaternion(0.49999997f, 0.49999997f, -0.5f, -0.5f));
	}

	ZETestItemAdd(QTRN_CreateIdentity)
	{
		ZEQuaternion Q;
		ZEQuaternion::CreateIdentity(Q);
		ZETestCheckEqual(Q, ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
	}

	ZETestItemAdd(QTRN_Product)
	{
		ZEQuaternion Q;
		ZEQuaternion A(3.0f, 5.0f, 8.0f, 11.0f);
		A.NormalizeSelf();
		ZEQuaternion B(1.0f, 1.0f, 2.0f,  3.0f);
		B.NormalizeSelf();
		ZEQuaternion::Product(Q, A, B);

		ZETestCheckEqual(Q, ZEQuaternion(-0.88982064f, 0.17447460f, 0.17447464f, 0.38384420f));
	}

	ZETestItemAdd(QTRN_VectorProduct)
	{
		ZETestCheck(false);

		/*ZEVector3 Q;
		ZEQuaternion A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector3 V(1.0f, 2.0f, 3.0f);
		ZEQuaternion::VectorProduct(Q, A, V);*/
	}

	ZETestItemAdd(QTRN_ConvertToEulerAngles)
	{
		float yaw, pitch, roll;
		ZEQuaternion Q(0.5f, 0.5f, 0.5f, 0.5f);
		ZEQuaternion::ConvertToEulerAngles(yaw, pitch, roll, Q);
		

     	ZETestCheckEqual(yaw, ZE_PI_2);
		ZETestCheckEqual(pitch, ZE_PI_2);
		ZETestCheckEqual(roll, 0.0f);
	}

	ZETestItemAdd(QTRN_Conjugate1)
	{
		ZEQuaternion C, Q(1.0f, 2.0f, 3.0f, 4.0f);
		ZEQuaternion::Conjugate(C, Q);
		ZETestCheckEqual(C, ZEQuaternion(1.0f, -2.0f, -3.0f, -4.0f));
	}

	ZETestItemAdd(QTRN_Conjugate2)
	{
		ZEQuaternion A, C(1.0f, 2.0f, 3.0f, 4.0f);
		A = C.Conjugate();
		ZETestCheckEqual(A, ZEQuaternion(1.0f, -2.0f, -3.0f, -4.0f));
	}
	
	ZETestItemAdd(QTRN_Slerp)
	{
		ZEQuaternion P(0.1f, 0.2f, 0.3f, 0.4f);
		ZEQuaternion Q(0.1f, 0.0f, 0.0f, 0.3f);
		P.NormalizeSelf();

		ZEQuaternion QOut;
		float t = 0.3;
		ZEQuaternion::Slerp(QOut, P, Q, t);

		ZETestCheckEqual(QOut, ZEQuaternion(0.12755547f, 0.17063320f, 0.25594980f, 0.46798304f));

		ZEQuaternion A(0.3f, 0.0f, 0.0f, 0.0f);
		ZEQuaternion B(0.1f, 0.1f, 0.0f, 0.3f);

		ZEQuaternion QOut2;
		ZEQuaternion::Slerp(QOut2, A, B, t);

		ZETestCheckEqual(QOut2, ZEQuaternion(0.30911696f, 0.044615276f, 0.0f, 0.13384584f));
	}

	ZETestItemAdd(QTRN_ConvertToRotationMatrix)
	{
		ZEQuaternion A(0.5f, 0.5f, 0.5f, 0.5f);
		ZEMatrix4x4 M;
		ZEQuaternion::ConvertToRotationMatrix(M, A);
		
		ZETestCheckEqual(M, ZEMatrix4x4( 0.0f, 1.0f,  0.0f, 0.0f, 
								    0.0f, 0.0f,  1.0f, 0.0f, 
								    1.0f, 0.0f,  0.0f, 0.0f, 
								    0.0f, 0.0f,  0.0f, 1.0f));
	}
	
	ZETestItemAdd(QTRN_Normalize1)
	{
		ZEQuaternion A, Q(0.1f, 0.2f, 0.3f, 0.4f);
		A = Q.Normalize();

		ZETestCheckEqual(A, ZEQuaternion(0.1f/sqrt(0.3f), 0.2f/sqrt(0.3f), 0.3f/sqrt(0.3f), 0.4f/sqrt(0.3f)));

	}

	ZETestItemAdd(QTRN_Normalize2)
	{
		ZEQuaternion P;
		ZEQuaternion Q(0.1f, 0.2f, 0.3f, 0.4f);
		ZEQuaternion::Normalize(P, Q);

		ZETestCheckEqual(P, ZEQuaternion(0.1f/sqrt(0.3f), 0.2f/sqrt(0.3f), 0.3f/sqrt(0.3f), 0.4f/sqrt(0.3f)));
	}

 }
