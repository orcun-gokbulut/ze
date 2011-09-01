//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectorTest.cpp
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
#include <math.h>
#include "ZEIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEDefinitions.h"

bool operator>=(const ZEVector2& A, const ZEVector2& B)
{
	return (A.x >= B.x && A.y >= B.y);
}

bool operator>=(const ZEVector3& A, const ZEVector3& B)
{
	return (A.x >= B.x && A.y >= B.y && A.z >= B.z);
}

bool operator>=(const ZEVector4& A, const ZEVector4& B)
{
	return (A.x >= B.x && A.y >= B.y && A.z >= B.z && A.w >= B.w);
}

bool operator<=(const ZEVector2& A, const ZEVector2& B)
{
	return (A.x <= B.x && A.y <= B.y);
}

bool operator<=(const ZEVector3& A, const ZEVector3& B)
{
	return (A.x <= B.x && A.y <= B.y && A.z <= B.z);
}

bool operator<=(const ZEVector4& A, const ZEVector4& B)
{
	return (A.x <= B.x && A.y <= B.y && A.z <= B.z && A.w <= B.w);
}

SUITE(ZEVector2)
{
	TEST(Constructor2)
	{
		ZEVector2 A(1.0f, 2.0f);
		CHECK_EQUAL(A, ZEVector2(1.0f, 2.0f));	
		
		ZEVector2 B(3.0f, 2.0f);
		ZEVector2 C(A,B);
		CHECK_EQUAL(C, ZEVector2(2.0f, 0.0f));
	}
	TEST(Create2)
	{
		ZEVector2 A;
		A.Create(1.0f,2.0f);
		CHECK_EQUAL(A, ZEVector2(1.0f, 2.0f));

		ZEVector2 X;
		X.Create(ZEVector2(1.0f, 2.0f), ZEVector2(4.0f, 6.0f));
		CHECK_EQUAL(X, ZEVector2(3.0f, 4.0f));
	}

	TEST(Add2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(2.0f, 4.0f);
		ZEVector2 C;

		ZEVector2::Add(C,A,B);
		CHECK_EQUAL(C, ZEVector2(3.0f, 6.0f));

	}
	
	TEST(Substution2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(2.0f, 3.0f);
		ZEVector2 C;
		
		ZEVector2::Substution(C,A,B);
		CHECK_EQUAL(C,ZEVector2(-1.0f, -1.0f));
	}
	
	TEST(Scale2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B;

		ZEVector2::Scale(B, A, 10.5f);
		CHECK_EQUAL(B, ZEVector2(10.5f, 21.0f));
	}
	TEST(Multiply2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(2.0f, 4.0f);
		ZEVector2 C;

		ZEVector2::Multiply(C, A, B);
		CHECK_EQUAL(C, ZEVector2(2.0f, 8.0f));
	}
	
	TEST(DotProduction2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(2.0f, 3.0f);

		CHECK_EQUAL(8.0f, ZEVector2::DotProduction(A,B));
		
	}
	
	TEST(Normalize2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 N;
		ZEVector2::Normalize(N, A);

		CHECK_EQUAL(N, ZEVector2(1.0f/sqrt(5.0f), 2.0f/sqrt(5.0f)));		
	}
	TEST(Length2)
	{
		ZEVector2 A(1.0f, 2.0f);
		float l = ZEVector2::Length(A);

		CHECK_EQUAL(l, sqrt(5.0f));
	}

	TEST(LengthSquare2)
	{
		ZEVector2 A(1.0f, 2.0f);
		float l = ZEVector2::LengthSquare(A);

		CHECK_EQUAL(l, 5.0f);
	}
	TEST(Distance2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(4.0f, 5.0f);

		float d = ZEVector2::Distance(A, B);
		CHECK_EQUAL(d, sqrt(18.0f));
	}
	TEST(DistanceSquare2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(4.0f, 5.0f);

		float ds = ZEVector2::DistanceSquare(A, B);

		CHECK_EQUAL(ds, 18.0f);
	}
	TEST(Lerp2)
	{
		ZEVector2 A(1.0f, 2.0f);
		ZEVector2 B(6.0f, 9.0f);
		ZEVector2 C;
		float t= 0.65f;

		ZEVector2::Lerp(C, A, B, t);

		CHECK_CLOSE(C, ZEVector2(4.25f, 6.55f), ZEVector2(0.0001f, 0.0001f));
	}
}
SUITE(ZEVector3)
{
	TEST(Constructor3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		CHECK_EQUAL(A, ZEVector3(1.0f, 2.0f, 3.0f));	
		
		ZEVector3 B(3.0f, 2.0f, 4.0f);
		ZEVector3 C(A,B);
		CHECK_EQUAL(C, ZEVector3(2.0f, 0.0f, 1.0f));

		ZEVector3 D(ZEVector2(2.1f, 3.4f), 5.3f);
		CHECK_EQUAL(D, ZEVector3(2.1f, 3.4f, 5.3f));
	}
	TEST(Create3)
	{
		ZEVector3 A;
		ZEVector3::Create(A,1.0f,2.0f,3.0f);
		CHECK_EQUAL(A, ZEVector3(1.0f, 2.0f, 3.0f));

		ZEVector3 X(ZEVector3(1.0f, 2.0f, 3.0f), ZEVector3(4.0f, 6.0f, 8.0f));
		CHECK_EQUAL(X, ZEVector3(3.0f, 4.0f, 5.0f));
	}

	TEST(Add3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 4.0f, 6.0f);
		ZEVector3 C;

		ZEVector3::Add(C,A,B);
		CHECK_EQUAL(C, ZEVector3(3.0f, 6.0f, 9.0f));

	}
	
	TEST(Sub3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);
		ZEVector3 C;
		
		ZEVector3::Sub(C,A,B);
		CHECK_EQUAL(C,ZEVector3(-1.0f, -1.0f, -1.0f));
	}
	
	TEST(Scale3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B;

		ZEVector3::Scale(B, A, 10.5f);
		CHECK_EQUAL(B, ZEVector3(10.5f, 21.0f, 31.5f));
	}
	TEST(Multiply3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 4.0f, 6.0f);
		ZEVector3 C;

		ZEVector3::Multiply(C, A, B);
		CHECK_EQUAL(C, ZEVector3(2.0f, 8.0f, 18.0f));
	}
	
	TEST(DotProduct3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);

		CHECK_EQUAL(20.0f, ZEVector3::DotProduct(A,B));
		
	}
	
	TEST(CrossProduct3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(2.0f, 3.0f, 4.0f);
		ZEVector3 C;
		ZEVector3::CrossProduct(C, A ,B);

		CHECK_EQUAL(C, ZEVector3(-1.0f, 2.0f, -1.0f));
		
	}

	TEST(Normalize3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 N;
		ZEVector3::Normalize(N, A);

		CHECK_EQUAL(N, ZEVector3(1.0f/sqrt(14.0f), 2.0f/sqrt(14.0f), 3.0f/sqrt(14.0f)));		
	}
	TEST(Length3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		float l = ZEVector3::Length(A);

		CHECK_EQUAL(l, sqrt(14.0f));
	}
	TEST(LengthSquare3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		float l = ZEVector3::LengthSquare(A);

		CHECK_EQUAL(l, 14.0f);
	}
	TEST(Distance3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(4.0f, 5.0f, 6.0f);

		float d = ZEVector3::Distance(A, B);
		CHECK_EQUAL(d, sqrt(27.0f));
	}
	TEST(DistanceSquare3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(4.0f, 5.0f, 6.0f);

		float ds = ZEVector3::DistanceSquare(A, B);

		CHECK_EQUAL(ds, 27.0f);
	}
	TEST(Lerp3)
	{
		ZEVector3 A(1.0f, 2.0f, 3.0f);
		ZEVector3 B(6.0f, 9.0f, 5.0f);
		ZEVector3 C;
		float t= 0.65f;

		ZEVector3::Lerp(C, A, B, t);

		CHECK_CLOSE(C, ZEVector3(4.25f, 6.55f, 4.30), ZEVector3(0.001f, 0.001f, 0.001f));
	}
}
SUITE(ZEVector4)
{
	TEST(Constructor4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		CHECK_EQUAL(A, ZEVector4(1.0f, 2.0f, 3.0f, 4.0f));	
		
		ZEVector4 B(3.0f, 2.0f, 4.0f, 5.0f);
		ZEVector4 C(A,B);
		CHECK_EQUAL(C, ZEVector4(2.0f, 0.0f, 1.0f, 1.0f));

		ZEVector4 D(ZEVector2(2.1f, 3.4f), 5.3f, 6.7f);
		CHECK_EQUAL(D, ZEVector4(2.1f, 3.4f, 5.3f, 6.7f));

		ZEVector4 E(ZEVector3(21.11f, 3.54f, 8.01f), 4.9f);
		CHECK_EQUAL(E, ZEVector4(21.11f, 3.54f, 8.01f, 4.9f));
	}
	TEST(Create4)
	{
		ZEVector4 A;
		A.Create(1.0f, 2.0f, 3.0f, 4.0f);
		CHECK_EQUAL(A, ZEVector4(1.0f, 2.0f, 3.0f, 4.0f));

		ZEVector4 X;
		X.Create(ZEVector4(1.0f, 2.0f, 3.0f, 4.0f), ZEVector4(4.0f, 6.0f, 8.0f, 10.0f));
		CHECK_EQUAL(X, ZEVector4(3.0f, 4.0f, 5.0f, 6.0f));
	}

	TEST(Add4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 8.0f);
		ZEVector4 C;

		ZEVector4::Add(C,A,B);
		CHECK_EQUAL(C, ZEVector4(3.0f, 6.0f, 9.0f, 12.0f));

	}
	
	TEST(Sub4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 8.0f);
		ZEVector4 C;
		
		ZEVector4::Sub(C,A,B);
		CHECK_EQUAL(C,ZEVector4(-1.0f, -2.0f, -3.0f, -4.0f));
	}
	
	TEST(Scale4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 B;

		ZEVector4::Scale(B, A, 10.5f);
		CHECK_EQUAL(B, ZEVector4(10.5f, 21.0f, 31.5f, 42.0f));
	}
	TEST(Multiply4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 10.0f);
		ZEVector4 B(2.0f, 4.0f, 6.0f, 11.0f);
		ZEVector4 C;

		ZEVector4::Multiply(C, A, B);
		CHECK_EQUAL(C, ZEVector4(2.0f, 8.0f, 18.0f, 110.f));
	}
	
	TEST(DotProduct4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 13.0f);
		ZEVector4 B(2.0f, 3.0f, 4.0f, 10.0f);

		CHECK_EQUAL(150.0f, ZEVector4::DotProduct(A,B));
		
	}

	TEST(Normalize4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 4.0f);
		ZEVector4 N;
		ZEVector4::Normalize(N, A);

		CHECK_EQUAL(N, ZEVector4(1.0f/sqrt(30.0f), 2.0f/sqrt(30.0f), 3.0f/sqrt(30.0f), 4.0f/sqrt(30.0f)));		
	}
	TEST(Length4)
	{
		ZEVector4 A(1.0f, 2.0f, 4.0f, 7.0f);
		float l = ZEVector4::Length(A);

		CHECK_EQUAL(l, sqrt(70.0f));
	}
	TEST(LengthSquare4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 5.0f);
		float l = ZEVector4::LengthSquare(A);

		CHECK_EQUAL(l, 39.0f);
	}
	TEST(Distance4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 5.0f);
		ZEVector4 B(4.0f, 5.0f, 6.0f, 7.0f);

		float d = ZEVector4::Distance(A, B);
		CHECK_EQUAL(d, sqrt(31.0f));
	}
	TEST(DistanceSquare4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 2.0f);
		ZEVector4 B(4.0f, 5.0f, 6.0f, 7.0f);

		float ds = ZEVector4::DistanceSquare(A, B);

		CHECK_EQUAL(ds, 52.0f);
	}
	TEST(Lerp4)
	{
		ZEVector4 A(1.0f, 2.0f, 3.0f, 6.0f);
		ZEVector4 B(6.0f, 9.0f, 5.0f, 3.0f);
		ZEVector4 C;
		float t= 0.65f;

		ZEVector4::Lerp(C, A, B, t);

		CHECK_CLOSE(C, ZEVector4(4.25f, 6.55f, 4.30, 4.05), ZEVector4(0.0001f, 0.0001f, 0.0001f, 0.0001f));
	}
}
