//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Canvas.cpp
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

#include "Canvas.h"
#include "GraphicsModule.h"
#include "Core/Error.h"
#include <stdio.h>

#define ZECANVAS_ADDVERTEX(Vertex, Matrix, Pos, Nor, Texcrd)\
	ZEMatrix4x4::Transform((Vertex).Position, (Matrix), (Pos));\
	ZEMatrix4x4::Transform3x3((Vertex).Normal, (Matrix), (Nor));\
	(Vertex).Texcoord = (Texcrd)

#define ZECANVAS_ADDWIREVERTEX(Vertex, Matrix, Pos)\
	ZEMatrix4x4::Transform((Vertex).Position, (Matrix), (Pos));\
	(Vertex).Normal = ZEVector3(0.0f, 0.0f, 0.0f);\
	(Vertex).Texcoord = ZEVector2(0.0f, 0.0f)

void ZECanvas::PushTransformation()
{
	TransformationStack.Push(Transformation);
}

void ZECanvas::PopTransformation()
{
	Transformation = TransformationStack.Pop();
}

void ZECanvas::SetTransfomation(const ZEMatrix4x4& Matrix)
{
	Transformation = Matrix;
}

const ZEMatrix4x4& ZECanvas::GetTransformation()
{
	return Transformation;
}

void ZECanvas::AddWireframePlane(float Width, float Length, unsigned int VSegment, unsigned int HSegment)
{
	float XPosDiff = Width / (HSegment + 1);
	float ZPosDiff = Length / (VSegment + 1);
	Length *= 0.5f;
	Width *= 0.5f;
	
	ZESimpleVertex* Verts = Vertices.MassAdd((VSegment + 2) * 2 + (HSegment + 2)* 2);

	int N = 0;
	for (int I = 0; I <= VSegment + 1; I++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, Length));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, -Length));
		N += 2;
	}

	for (int I = 0; I <= HSegment + 1; I++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(-Width, 0.0f, I * ZPosDiff - Length));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(Width, 0.0f, I * ZPosDiff - Length));
		N += 2;
	}
}

void ZECanvas::AddWireframeBox(float Width, float Height, float Length)
{
	Width /= 2;
	Height /= 2;
	Length /= 2;

	ZESimpleVertex* Verts = Vertices.MassAdd(24);

	// Top
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, ZEVector3(-Width, Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, ZEVector3(-Width, Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[2], Transformation, ZEVector3(-Width, Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[3], Transformation, ZEVector3(Width, Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[4], Transformation, ZEVector3(Width, Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[5], Transformation, ZEVector3(Width, Height, Length));

	ZECANVAS_ADDWIREVERTEX(Verts[6], Transformation, ZEVector3(Width, Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[7], Transformation, ZEVector3(-Width, Height, Length));

	// Bottom
	ZECANVAS_ADDWIREVERTEX(Verts[8], Transformation, ZEVector3(-Width, -Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[9], Transformation, ZEVector3(-Width, -Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[10], Transformation, ZEVector3(-Width, -Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[11], Transformation, ZEVector3(Width, -Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[12], Transformation, ZEVector3(Width, -Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[13], Transformation, ZEVector3(Width, -Height, Length));

	ZECANVAS_ADDWIREVERTEX(Verts[14], Transformation, ZEVector3(Width, -Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[15], Transformation, ZEVector3(-Width, -Height, Length));

	// Sides
	ZECANVAS_ADDWIREVERTEX(Verts[16], Transformation, ZEVector3(-Width, Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[17], Transformation, ZEVector3(-Width, -Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[18], Transformation, ZEVector3(-Width, Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[19], Transformation, ZEVector3(-Width, -Height, Length));

	ZECANVAS_ADDWIREVERTEX(Verts[20], Transformation, ZEVector3(Width, Height, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[21], Transformation, ZEVector3(Width, -Height, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[22], Transformation, ZEVector3(Width, Height, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[23], Transformation, ZEVector3(Width, -Height, Length));
}

void ZECanvas::AddWireframeSphere(float Radious, unsigned int HSegments, unsigned int VSegments)
{
	float VAngle = ZE_PI / (VSegments + 1);
	float HAngle = 2.0f * ZE_PI / HSegments;

	int N = 0;
	ZESimpleVertex* Verts =  Vertices.MassAdd(VSegments * HSegments * 4 + HSegments * 2);

	float HalfRadious = Radious * 0.5f;	

	float YPos = HalfRadious;
	float PrevRadious = 0.0f;
	float PrevYPos = YPos;

	float CurrentRadious;
	for (int Y = 1; Y <= VSegments; Y++)
	{
		YPos = HalfRadious * cosf(VAngle * Y);
		CurrentRadious = HalfRadious * sinf(VAngle * Y);
		for (int X = 0; X < HSegments; X++)
		{
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation,		ZEVector3(CurrentRadious * sinf(HAngle * X), YPos, CurrentRadious * cosf(HAngle * X)));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation,	ZEVector3(CurrentRadious * sinf(HAngle * (X + 1)), YPos, CurrentRadious * cosf(HAngle * (X + 1))));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 2], Transformation,	ZEVector3(PrevRadious * sinf(HAngle * X), PrevYPos, PrevRadious * cosf(HAngle * X)));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 3], Transformation,	ZEVector3(CurrentRadious * sinf(HAngle * X), YPos, CurrentRadious * cosf(HAngle * X)));
			N += 4;
		}
		PrevYPos = YPos;
		PrevRadious = CurrentRadious;
	}
	
	YPos = -HalfRadious;
	for (int X = 0; X < HSegments; X++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(0, YPos, 0));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(PrevRadious * sinf(HAngle * X), PrevYPos, PrevRadious * cosf(HAngle * X)));
		N += 2;
	}
}

void ZECanvas::AddWireframeQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(8);
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, P0);
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, P1);
	
	ZECANVAS_ADDWIREVERTEX(Verts[2], Transformation, P1);
	ZECANVAS_ADDWIREVERTEX(Verts[3], Transformation, P2);

	ZECANVAS_ADDWIREVERTEX(Verts[4], Transformation, P2);
	ZECANVAS_ADDWIREVERTEX(Verts[5], Transformation, P3);

	ZECANVAS_ADDWIREVERTEX(Verts[6], Transformation, P3);
	ZECANVAS_ADDWIREVERTEX(Verts[7], Transformation, P0);
}

void ZECanvas::AddWireframeCylinder(float Radious, float Height, unsigned int HSegments, unsigned int VSegments, bool Caps)
{
	float HalfRadious = Radious * 0.5;
	float HAngle = ((2.0f * ZE_PI) / HSegments);
	float YPos;
	float YPosDiff = (Height / (VSegments + 1));

	int N = 0;
	ZESimpleVertex* Verts =  Vertices.MassAdd((VSegments + 2) * HSegments * 2 + (VSegments + 1) * HSegments * 2 + (Caps ? 4 * HSegments : 0));
	
	for (int Y = 0; Y < VSegments + 2; Y++)
	{
		YPos = YPosDiff * Y ;
		for (int X = 0; X < HSegments; X++)
		{
			float sinfHAngle = sinf(X * HAngle);
			float cosfHAngle = cosf(X * HAngle);

			if (Y != VSegments + 1)
			{
				ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos + YPosDiff, HalfRadious * cosfHAngle));
				ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos, HalfRadious * cosfHAngle));
				N += 2;
			}
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos, HalfRadious * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinf((X + 1) * HAngle), YPos, HalfRadious * cosf((X + 1) * HAngle)));
			N += 2;
		}
	}

	if (Caps)
		for (int X = 0; X < HSegments; X++)
		{
			float sinfHAngle = sinf(X * HAngle);
			float cosfHAngle = cosf(X * HAngle);
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, 0.0f, HalfRadious * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(0.0f, 0.0f, 0.0f));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 2], Transformation, ZEVector3(HalfRadious * sinfHAngle, Height, HalfRadious * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 3], Transformation, ZEVector3(0.0f, Height, 0.0f));
			N += 4;
		}
}

void ZECanvas::AddWireframePyramid(float Width, float Height, float Length)
{
		
	ZESimpleVertex* Verts = Vertices.MassAdd(16);
	Width *= 0.5f;
	Length *= 0.5f;

	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, ZEVector3(Width, 0.0f, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, ZEVector3(0.0f, Height, 0.0f));

	ZECANVAS_ADDWIREVERTEX(Verts[2], Transformation, ZEVector3(Width, 0.0f, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[3], Transformation, ZEVector3(0.0f, Height, 0.0f));

	ZECANVAS_ADDWIREVERTEX(Verts[4], Transformation, ZEVector3(-Width, 0.0f, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[5], Transformation, ZEVector3(0.0f, Height, 0.0f));

	ZECANVAS_ADDWIREVERTEX(Verts[6], Transformation, ZEVector3(-Width, 0.0f, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[7], Transformation, ZEVector3(0.0f, Height, 0.0f));

	ZECANVAS_ADDWIREVERTEX(Verts[8], Transformation, ZEVector3(Width, 0.0f, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[9], Transformation, ZEVector3(Width, 0.0f, -Length));
	
	ZECANVAS_ADDWIREVERTEX(Verts[10], Transformation, ZEVector3(Width, 0.0f, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[11], Transformation, ZEVector3(-Width, 0.0f, -Length));

	ZECANVAS_ADDWIREVERTEX(Verts[12], Transformation, ZEVector3(-Width, 0.0f, -Length));
	ZECANVAS_ADDWIREVERTEX(Verts[13], Transformation, ZEVector3(-Width, 0.0f, Length));

	ZECANVAS_ADDWIREVERTEX(Verts[14], Transformation, ZEVector3(-Width, 0.0f, Length));
	ZECANVAS_ADDWIREVERTEX(Verts[15], Transformation, ZEVector3(Width, 0.0f, Length));
}

void ZECanvas::AddPoint(const ZEVector3& Point)
{
	ZESimpleVertex* Vertex = Vertices.Add();
	ZECANVAS_ADDWIREVERTEX(*Vertex, Transformation, Point);
}

void ZECanvas::AddLine(const ZEVector3& Point0, const ZEVector3& Point1)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(2);
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, Point0);
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, Point1);
}

void ZECanvas::AddCircle(float Radious, unsigned int Segments)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(Segments * 2);
	float HAngle = ((2.0f * ZE_PI) / Segments);
	float HalfRadious = 0.5 * Radious;
	int N = 0;
	for (int X = 0; X < Segments; X++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(HalfRadious * sinf(X * HAngle), 0.0f, HalfRadious * cosf(X * HAngle)));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinf((X + 1) * HAngle), 0.0f, HalfRadious * cosf((X + 1) * HAngle)));
		N += 2;
	}
}

void ZECanvas::AddPlane(float Width, float Length)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(6);
	ZEVector3 Normal(0.0f, 1.0f, 0.0f);
	ZECANVAS_ADDVERTEX(Verts[0], Transformation, ZEVector3(-Width, 0.0f, Length), Normal, ZEVector2(0.0f ,1.0f));
	ZECANVAS_ADDVERTEX(Verts[1], Transformation, ZEVector3(Width, 0.0f, -Length), Normal, ZEVector2(1.0f ,0.0f));
	ZECANVAS_ADDVERTEX(Verts[2], Transformation, ZEVector3(-Width, 0.0f, -Length), Normal, ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[3], Transformation, ZEVector3(-Width, 0.0f, Length), Normal, ZEVector2(0.0f, 1.0f));
	ZECANVAS_ADDVERTEX(Verts[4], Transformation, ZEVector3(Width, 0.0f, Length), Normal, ZEVector2(1.0f, 1.0f));
	ZECANVAS_ADDVERTEX(Verts[5], Transformation, ZEVector3(Width, 0.0f, -Length), Normal, ZEVector2(1.0f, 0.0f));
}

void ZECanvas::AddBox(float Width, float Height, float Length)
{
	Width /= 2;
	Height /= 2;
	Length /= 2;

	ZESimpleVertex* Verts = Vertices.MassAdd(36);

	ZECANVAS_ADDVERTEX(Verts[0],  Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[1],  Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[2],  Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[3],  Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[4],  Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[5],  Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[6],  Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[7],  Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[8],  Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[9],  Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[10], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[11], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[12], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[13], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[14], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[15], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[16], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[17], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[18], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[19], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[20], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[21], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[22], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[23], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[24], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[25], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[26], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[27], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[28], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[29], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[30], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[31], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[32], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[33], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	ZECANVAS_ADDVERTEX(Verts[34], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	ZECANVAS_ADDVERTEX(Verts[35], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
}

void ZECanvas::AddSphere(float Radious, unsigned int HSegments, unsigned int VSegments)
{
	float VAngle = ZE_PI / (VSegments + 1);
	float HAngle = 2.0f * ZE_PI / HSegments;
	float UDiff = 1.0f / HSegments;
	float VDiff = 1.0f / VSegments;

	int N = 0;
	ZESimpleVertex* Verts =  Vertices.MassAdd((VSegments) * HSegments * 6 + HSegments * 6);

	float HalfRadious = Radious * 0.5f;	

	float YPos;
	float PrevRadious = HalfRadious * sinf(VAngle);
	float PrevYPos = HalfRadious * cosf(VAngle);
	float PrevcosfVAngle = cosf(VAngle);
	float cosfVAngle;
	float cosfHAngle;
	float CurrentRadious;

	ZEVector3 Position, Normal;
	
	for (int Y = 1; Y <= VSegments; Y++)
	{
		cosfVAngle = cosf(VAngle * Y);
		YPos = HalfRadious * cosfVAngle;
		CurrentRadious = HalfRadious * sinf(VAngle * Y);
		for (int X = 0; X < HSegments; X++)
		{
			float sinfHAngle;
		
			sinfHAngle = sinf(HAngle * X);
			cosfHAngle = cosf(HAngle * X);
			Position = ZEVector3(PrevRadious * sinfHAngle , PrevYPos, PrevRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(X * UDiff, Y * VDiff));

			Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2(X * UDiff, (Y + 1) * VDiff));

			sinfHAngle = sinf(HAngle * (X + 1));
			cosfHAngle = cosf(HAngle * (X + 1));
			Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2((X + 1) * UDiff, (Y + 1) * VDiff));

			Verts[N + 3] = Verts[N + 2];

			Position = ZEVector3(PrevRadious * sinfHAngle, PrevYPos, PrevRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 4], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, Y * VDiff));

			Verts[N + 5] = Verts[N];

			N += 6;
		}
		PrevYPos = YPos;
		PrevRadious = CurrentRadious;
		PrevcosfVAngle = cosfVAngle;
	}
	
	cosfVAngle = cosf(VAngle);
	YPos = HalfRadious * cosfVAngle;
	CurrentRadious = HalfRadious * sinf(VAngle);
	for (int X = 0; X < HSegments; X++)
	{
		float sinfHAngle = sinf(HAngle * X);
		Position = ZEVector3(0.0f, HalfRadious,  0.0f);
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(0.5f, 1.0f));

		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, 1.0f - VDiff));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2((X) * UDiff, 1.0f - VDiff));
		N += 3;
	}
	cosfVAngle = cosf(ZE_PI - VAngle);
	YPos = HalfRadious * cosfVAngle;
	for (int X = 0; X < HSegments; X++)
	{
		float sinfHAngle = sinf(HAngle * X);
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(0.5f, 1.0f));

		Position = ZEVector3(0.0f, -HalfRadious,  0.0f);
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, 1.0f - VDiff));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2(X * UDiff, 1.0f - VDiff));
		N += 3;
	}
}

void ZECanvas::AddQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(6);
	ZEVector3 Normal1;
	ZEVector3::CrossProduct(Normal1, P1 - P0, P3 - P0);
	ZEVector3::Normalize(Normal1, Normal1);

	ZEVector3 Normal2;
	ZEVector3::CrossProduct(Normal2, P1 - P2, P3 - P2);
	ZEVector3::Normalize(Normal2, Normal2);

	ZECANVAS_ADDVERTEX(Verts[0], Transformation, P0, Normal1, ZEVector2(0.0f ,0.0f));
	ZECANVAS_ADDVERTEX(Verts[1], Transformation, P1, Normal1, ZEVector2(1.0f ,0.0f));
	ZECANVAS_ADDVERTEX(Verts[2], Transformation, P2, Normal1, ZEVector2(1.0f, 1.0f));
	
	ZECANVAS_ADDVERTEX(Verts[3], Transformation, P2, Normal2, ZEVector2(1.0f, 1.0f));
	ZECANVAS_ADDVERTEX(Verts[4], Transformation, P3, Normal2, ZEVector2(0.0f, 1.0f));
	ZECANVAS_ADDVERTEX(Verts[5], Transformation, P0, Normal2, ZEVector2(0.0f, 0.0f));
}

void ZECanvas::AddDisc(float Radious, unsigned int Segments)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(Segments * 3);
	ZEVector3 Normal(0.0f, 1.0f, 0.0f);

	float HAngle = (2.0f * ZE_PI) / Segments;
	float UVDif = 1.0f / Segments;
	float HalfRadious = 0.5 * Radious;
	int N = 0;
	for (int X = 0; X < Segments; X++)
	{
		float sinfHAngle = sinf(X * HAngle);
		float sinfHAnglePP = sinf((X + 1) * HAngle);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, 0.0f, HalfRadious * cosf(X * HAngle)), Normal, ZEVector2(X * UVDif, 1.0f));
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinfHAnglePP , 0.0f, HalfRadious * cosf((X + 1) * HAngle)), Normal, ZEVector2((X + 1) * UVDif, 1.0f));
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, ZEVector3(0.0f, 0.0f, 0.0f), Normal, ZEVector2(X * UVDif, 0.0f));
		N += 3;
	}
}

void ZECanvas::AddPyramid(float Width, float Height, float Length)
{
	

	ZESimpleVertex* Verts = Vertices.MassAdd(18);
	Width *= 0.5f;
	Length *= 0.5f;

	float WidthNormal = Height*Height;
	float LengthNormal = sqrt(WidthNormal + Length*Length);
	WidthNormal = sqrt(WidthNormal + Width*Width);

	ZECANVAS_ADDVERTEX(Verts[0], Transformation, ZEVector3(Width, 0.0f, -Length), ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[2], Transformation, ZEVector3(0.0f, Height, 0.0f),  ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.5f,1.0f));
	ZECANVAS_ADDVERTEX(Verts[1], Transformation, ZEVector3(Width, 0.0f, Length),  ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(1.0f,0.0f));

	ZECANVAS_ADDVERTEX(Verts[3], Transformation, ZEVector3(Width, 0.0f, Length), ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[5], Transformation, ZEVector3(0.0f, Height, 0.0f), ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(0.5f,1.0f));
	ZECANVAS_ADDVERTEX(Verts[4], Transformation, ZEVector3(-Width, 0.0f, Length),  ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(1.0f,0.0f));

	ZECANVAS_ADDVERTEX(Verts[6], Transformation, ZEVector3(-Width, 0.0f, Length), ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[8], Transformation, ZEVector3(0.0f, Height, 0.0f),  ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.5f,1.0f));
	ZECANVAS_ADDVERTEX(Verts[7], Transformation, ZEVector3(-Width, 0.0f, -Length),  ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(1.0f,0.0f));

	ZECANVAS_ADDVERTEX(Verts[9], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[11], Transformation, ZEVector3(0.0f, Height, 0.0f), ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(0.5f,1.0f));
	ZECANVAS_ADDVERTEX(Verts[10], Transformation, ZEVector3(Width, 0.0f, -Length),  ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(1.0f,0.0f));

	ZECANVAS_ADDVERTEX(Verts[12], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[14], Transformation, ZEVector3(Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[13], Transformation, ZEVector3(Width, 0.0f, Length),  ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,1.0f));

	ZECANVAS_ADDVERTEX(Verts[15], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,0.0f));
	ZECANVAS_ADDVERTEX(Verts[17], Transformation, ZEVector3(Width, 0.0f, Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,1.0f));
	ZECANVAS_ADDVERTEX(Verts[16], Transformation, ZEVector3(-Width, 0.0f, Length),  ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,1.0f));
}

void ZECanvas::AddVertex(const ZESimpleVertex& Vertex)
{
	Vertices.AddByRef(Vertex);
}

void ZECanvas::AddVertices(const ZESimpleVertex* Vertices, size_t Count)
{
	this->Vertices.MassAdd(Vertices, Count);
}

void ZECanvas::CalculateBoundingBox(ZEAABoundingBox& BoundingBox)
{
	if (Vertices.GetCount() == 0)
	{
		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		return;
	}

	BoundingBox.Min = BoundingBox.Max = Vertices[0].Position;
	ZEVector3& Min = BoundingBox.Min;
	ZEVector3& Max = BoundingBox.Max;

	ZEVector3* Position;
	for (size_t I = 0; I < Vertices.GetCount(); I++)
	{
		Position = &Vertices[I].Position;
		if (Min.x > Position->x) Min.x = Position->x;
		if (Min.y > Position->y) Min.y = Position->y;
		if (Min.z > Position->z) Min.z = Position->z;

		if (Max.x < Position->x) Max.x = Position->x;
		if (Max.y < Position->y) Max.y = Position->y;
		if (Max.z < Position->z) Max.z = Position->z;

	}
}

bool ZECanvas::IsEmpty()
{
	return Vertices.GetCount() == 0;
}

void ZECanvas::Clean()
{
	Vertices.Clear();
}

bool ZECanvas::LoadCanvasFile(const char* Filename)
{
	ZEResourceFile File;
	if (!File.Open(Filename))
	{
		zeError("Canvas", "Can not load canvas file. (Filename : \"%s\")", Filename);
		return false;
	}
	
	
	int VertexCount;
	fscanf((FILE*)File.GetFileHandle(), "%d", &VertexCount);

	ZESimpleVertex* Vertices = this->Vertices.MassAdd(VertexCount);
	size_t Index = 0;
	while (!File.Eof())
	{
		fscanf((FILE*)File.GetFileHandle(), "%f %f %f %f %f %f %f %f", 
			 &Vertices[Index].Position.x,  &Vertices[Index].Position.y, &Vertices[Index].Position.z,
			 &Vertices[Index].Normal.x, &Vertices[Index].Normal.y, &Vertices[Index].Normal.z,
			 &Vertices[Index].Texcoord.x, &Vertices[Index].Texcoord.y);
		Index++;

	/*	if (Index == VertexCount)
		{
			zeError("Canvas", "Corrupted canvas file. Vertex count does not match. (Filename : \"%s\")", Filename);
			return false;
		}*/
	}

	if (Index + 1 != VertexCount)
		zeWarning("Canvas", "Corrupted canvas file. Vertex count is less than verties in the file. (Filename : \"%s\")", Filename);

	return true;
}

ZEStaticVertexBuffer* ZECanvas::CreateStaticVertexBuffer()
{
	ZEStaticVertexBuffer* Buffer = zeGraphics->CreateStaticVertexBuffer();
	Buffer->Create(Vertices.GetCount() * sizeof(ZESimpleVertex));
	void* BufferPtr = Buffer->Lock();
	memcpy(BufferPtr, Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZESimpleVertex));
	Buffer->Unlock();
	return Buffer;
}

ZECanvas::ZECanvas()
{
	ZEMatrix4x4::CreateIdentity(Transformation);
}
