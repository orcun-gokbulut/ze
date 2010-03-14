//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - OBoundingBox.cpp
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

#include "BoundingSphere.h"
#include "AABoundingBox.h"
#include "OBoundingBox.h"
#include "Definitions.h"
#include "LineSegment.h"
#include "Ray.h"

#include <math.h>

ZEVector3 ZEOBoundingBox::GetCenter() const
{
	return U * 0.5f + V * 0.5 + N * 0.5f + Position;
}


ZEVector3 ZEOBoundingBox::GetVertex(unsigned char Index) const
{
	switch(Index)
	{
		case 0:
			return Position;
		case 1:
			return Position + U;
		case 2:
			return Position + V;
		case 3:
			return Position + U + V;
		case 4:
			return Position + N;
		case 5:
			return Position + N + U;
		case 6:
			return Position + N + V;
		case 7:
			return Position + N + U + V;
	}
}

ZEHalfSpace ZEOBoundingBox::PlaneHalfSpaceTest(const ZEOBoundingBox& BoundingBox, const ZEPlane& Plane)
{
	ZEHalfSpace HS1 = ZEPlane::TestHalfSpace(Plane, BoundingBox.GetVertex(0));
	for (int I = 1; I < 8; I++)
	{
		ZEHalfSpace HS2 = ZEPlane::TestHalfSpace(Plane, BoundingBox.GetVertex(I));
		if (HS1 != HS2)
			return ZEHALFSPACE_INTERSECTS;
	}
	return HS1;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZEVector3& Point)
{
	float factor;

	factor = (ZEVector3::DotProduct(BoundingBox.U,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.U)) / ZEVector3::DotProduct(BoundingBox.U,BoundingBox.U);
	if (factor < 0 || factor > 1) return 0;

	factor = (ZEVector3::DotProduct(BoundingBox.V,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.V)) / ZEVector3::DotProduct(BoundingBox.V,BoundingBox.V);
	if (factor < 0 || factor > 1) return 0;

	factor = (ZEVector3::DotProduct(BoundingBox.N,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.N)) / ZEVector3::DotProduct(BoundingBox.N,BoundingBox.N);
	if (factor < 0 || factor > 1) return 0;

	return true;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELine& Line, float& T0, float& T1)
{
//	zeAssert(true, "Not implamented");
/*template <class T> bool Math3D<T>::IntersectLineOBB(
    const Vector3<T>& O, // Line origin
    const Vector3<T>& D, // Line direction
    const Vector3<T>& C, // Box center
    const Vector3<T> A[], // Box axes
    const Vector3<T>& e, // Box extents
    T t[], // On output, parametric points of intersection
    T epsilon) // Tolerance for parallel test*/

    int Parallel = 0;
    bool Found = false;
    float DA[3];
    float dA[3];

	//Vector3<T> d = C - O;
	ZEVector3 d;
	ZEVector3::Sub(d, BoundingBox.GetCenter(), Line.p);
	
	float e[3];
	float t[2];
	e[0] = ZEVector3::Length(BoundingBox.U);
	e[1] = ZEVector3::Length(BoundingBox.V);
	e[2] = ZEVector3::Length(BoundingBox.N);
	
	ZEVector3 A[3];
	ZEVector3::Scale(A[0], BoundingBox.U, 1.0f / e[0]);
	ZEVector3::Scale(A[1], BoundingBox.V, 1.0f / e[1]);
	ZEVector3::Scale(A[2], BoundingBox.N, 1.0f / e[2]);

    for (int I = 0; I < 3; I++)
    {
        //DA[i] = D.Dot(A[i]);
		DA[I] = ZEVector3::DotProduct(Line.v, A[I]);

        //dA[i] = d.Dot(A[i]);
        dA[I] = ZEVector3::DotProduct(d, A[I]);

		if (fabs(DA[I]) < ZE_ZEROTRESHOLD)
            Parallel |= 1 << I;
        else
        {
            float es = (DA[I] > 0.0f) ? e[I] : -e[I];
            float invDA = 1.0f / DA[I];

            if (!Found)
            {
                t[0] = (dA[I] - es) * invDA;
                t[1] = (dA[I] + es) * invDA;
                Found = true;
            }
            else
            {
                float s = (dA[I] - es) * invDA;
                if (s > t[0])
                    t[0] = s;
                s = (dA[I] + es) * invDA;
                if (s < t[1])
                    t[1] = s;
                if (t[0] > t[1])
                    return false;
            }
        }
    }
    
    if (Parallel)
        for (int I = 0; I < 3; I++)
            if (Parallel & (1 << I))
                if (fabs(dA[I] - t[0] * DA[I]) > e[I] || fabs(dA[I] - t[1] * DA[I]) > e[I])
                    return false;

/*	if (t[0] > t[1])
	{
		TMin = t[1];
		TMax = t[0];
	}
	else
	{
		TMin = t[0];
		TMax = t[1];
	}*/
    return true;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax)
{
	float T0, T1;
	if (IntersectionTest(BoundingBox, Ray.p))
	{
		if (IntersectionTest(BoundingBox, ZELine(Ray.v, Ray.p), T0, T1))
		{
			TMin = 0.0f;
			TMax = T0 > T1 ? T0 : T1;
			return true;
		}
	}
	else
	{
		if (IntersectionTest(BoundingBox, ZELine(Ray.v, Ray.p), T0, T1))
		{
			if (T0 > T1)
			{
				TMin = T1;
				TMax = T0;
			}
			else
			{
				TMin = T0;
				TMax = T1;
			}

			if (TMin > 0.0f)
				return true;
			else 
				return false;
		}
		else
			return false;
	}
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)
{
	float T0, T1;
	if (IntersectionTest(BoundingBox, ZELine(LineSegment.v, LineSegment.p), TMin, TMax))
		if (TMin > 0.0f && TMax < 1.0f)
			return true;
		else 
			return false;
	else
		return false;
}

/*
bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox1, const ZEOBoundingBox& BoundingBox2)
{
	//ZEVector3 Temp;
	//float UDotU = ZEVector3::DotProduct(BoundingBox.U,BoundingBox.U);
	//float VDotV = ZEVector3::DotProduct(BoundingBox.V,BoundingBox.V);
	//float NDotN;
	//float factor;
	//ZEVector3 PointMinusBox;
	//ZEVector3::Sub(PointMinusBox,PointMinusBox,BoundingBox);

	//factor = (ZEVector3::DotProduct(BoundingBox.U,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.U)) / UDotU;
	//ZEVector3::Scale(Temp,BoundingBox.U,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on YZ Plane of the oobb

	//float proj2d = ZEVector3::DotProduct(BoundingBox.V,Temp) / VDotV;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//NDotN = ZEVector3::DotProduct(BoundingBox.N,BoundingBox.N);
	//proj2d = ZEVector3::DotProduct(BoundingBox.N,Temp) / NDotN;
	//if (proj2d < 0 || proj2d > 1) return 0;

	////-----------------

	//factor = (ZEVector3::DotProduct(BoundingBox.V,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.V)) / VDotV;
	//ZEVector3::Scale(Temp,BoundingBox.V,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on XZ Plane of the oobb

	//proj2d = ZEVector3::DotProduct(BoundingBox.U,Temp) / UDotU;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//proj2d = ZEVector3::DotProduct(BoundingBox.N,Temp) / NDotN;
	//if (proj2d < 0 || proj2d > 1) return 0;

	////------------------

	//factor = (ZEVector3::DotProduct(BoundingBox.N,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.N)) / NDotN;
	//ZEVector3::Scale(Temp,BoundingBox.N,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on XY Plane of the oobb

	//proj2d = ZEVector3::DotProduct(BoundingBox.U,Temp) / UDotU;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//proj2d = ZEVector3::DotProduct(BoundingBox.V,Temp) / VDotV;
	//if (proj2d < 0 || proj2d > 1) return 0;

	zeAssert(true "Not implamented");
	return false;
}
bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox1, const ZEAABoundingBox& BoundingBox2)
{
	zeAssert(true "Not implamented");
	return false;
}

bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox, const ZEBoundingSphere& BoundingSphere)
{
	zeAssert(true "Not implamented");
	return false;
}*/
