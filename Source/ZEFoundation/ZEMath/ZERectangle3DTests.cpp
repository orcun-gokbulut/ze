//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERectangle3DTests.cpp
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

/*#include "ZETest.h"
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEPlane.h"
#include <math.h>


ZETestSuite(Rectangle3D)
{
	ZETest("RECT3D_Constructor")
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(4.0f, 5.0f, 6.0f);
		ZEVector3 C(7.0f, 8.0f, 9.0f);
		ZEVector3 D(10.0f, 11.0f, 12.0f);
		
		ZERectangle3D R(A, B, C, D);

		ZETestCheckEqual(R.P1, A);
		ZETestCheckEqual(R.P2, B);
		ZETestCheckEqual(R.P3, C);
		ZETestCheckEqual(R.P4, D);
	}

	ZETest("RECT3D_GetPlane")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 B(3.0f, 4.0f, 6.0f);
		ZEVector3 C(1.0f, 2.0f, 6.0f);
		ZEVector3 D(3.0f, 2.0f, 6.0f);
		
		ZERectangle3D R(A, B, C, D);

		ZEPlane P ;
		R.GetPlane(P);

		ZETestCheckEqual(P.n , ZEVector3(0.0f, 0.0f, -4.0f));
		ZETestCheckEqual(P.p , ZEVector3(1.0f, 2.0f, 6.0f));
	}

	ZETest("RECT3D_GetPoint")
	{
		ZEVector3 A(1.0f, 4.0f, 6.0f);
		ZEVector3 B(3.0f, 4.0f, 6.0f);
		ZEVector3 C(1.0f, 2.0f, 6.0f);
		ZEVector3 D(3.0f, 2.0f, 6.0f);
		
		ZERectangle3D R(A, B, C, D);

		ZEVector3 P1 = R.GetPoint(0);
		ZEVector3 P2 = R.GetPoint(1);
		ZEVector3 P3 = R.GetPoint(2);
		ZEVector3 P4 = R.GetPoint(3);

		ZETestCheckEqual(A, P1);
		ZETestCheckEqual(B, P2);
		ZETestCheckEqual(C, P3);
		ZETestCheckEqual(D, P4);
	}
}*/
