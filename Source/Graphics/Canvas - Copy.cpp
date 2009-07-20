//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Canvas - Copy.cpp
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
#include "Core/ResourceFile.h"
#include "Core/Error.h"
#include <stdio.h>

bool ZECanvas::IsEmpty()
{
	return (Vertices.GetSize() == 0);
}

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
		AddWireframeVertex(Verts[N], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, Length));
		AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, -Length));
		N += 2;
	}

	for (int I = 0; I <= HSegment + 1; I++)
	{
		AddWireframeVertex(Verts[N], Transformation, ZEVector3(-Width, 0.0f, I * ZPosDiff - Length));
		AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(Width, 0.0f, I * ZPosDiff - Length));
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
	AddWireframeVertex(Verts[0], Transformation, ZEVector3(-Width, Height, Length));
	AddWireframeVertex(Verts[1], Transformation, ZEVector3(-Width, Height, -Length));

	AddWireframeVertex(Verts[2], Transformation, ZEVector3(-Width, Height, -Length));
	AddWireframeVertex(Verts[3], Transformation, ZEVector3(Width, Height, -Length));

	AddWireframeVertex(Verts[4], Transformation, ZEVector3(Width, Height, -Length));
	AddWireframeVertex(Verts[5], Transformation, ZEVector3(Width, Height, Length));

	AddWireframeVertex(Verts[6], Transformation, ZEVector3(Width, Height, Length));
	AddWireframeVertex(Verts[7], Transformation, ZEVector3(-Width, Height, Length));

	// Bottom
	AddWireframeVertex(Verts[8], Transformation, ZEVector3(-Width, -Height, Length));
	AddWireframeVertex(Verts[9], Transformation, ZEVector3(-Width, -Height, -Length));

	AddWireframeVertex(Verts[10], Transformation, ZEVector3(-Width, -Height, -Length));
	AddWireframeVertex(Verts[11], Transformation, ZEVector3(Width, -Height, -Length));

	AddWireframeVertex(Verts[12], Transformation, ZEVector3(Width, -Height, -Length));
	AddWireframeVertex(Verts[13], Transformation, ZEVector3(Width, -Height, Length));

	AddWireframeVertex(Verts[14], Transformation, ZEVector3(Width, -Height, Length));
	AddWireframeVertex(Verts[15], Transformation, ZEVector3(-Width, -Height, Length));

	// Sides
	AddWireframeVertex(Verts[16], Transformation, ZEVector3(-Width, Height, -Length));
	AddWireframeVertex(Verts[17], Transformation, ZEVector3(-Width, -Height, -Length));

	AddWireframeVertex(Verts[18], Transformation, ZEVector3(-Width, Height, Length));
	AddWireframeVertex(Verts[19], Transformation, ZEVector3(-Width, -Height, Length));

	AddWireframeVertex(Verts[20], Transformation, ZEVector3(Width, Height, -Length));
	AddWireframeVertex(Verts[21], Transformation, ZEVector3(Width, -Height, -Length));

	AddWireframeVertex(Verts[22], Transformation, ZEVector3(Width, Height, Length));
	AddWireframeVertex(Verts[23], Transformation, ZEVector3(Width, -Height, Length));
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
			AddWireframeVertex(Verts[N], Transformation,		ZEVector3(CurrentRadious * sinf(HAngle * X), YPos, CurrentRadious * cosf(HAngle * X)));
			AddWireframeVertex(Verts[N + 1], Transformation,	ZEVector3(CurrentRadious * sinf(HAngle * (X + 1)), YPos, CurrentRadious * cosf(HAngle * (X + 1))));
			AddWireframeVertex(Verts[N + 2], Transformation,	ZEVector3(PrevRadious * sinf(HAngle * X), PrevYPos, PrevRadious * cosf(HAngle * X)));
			AddWireframeVertex(Verts[N + 3], Transformation,	ZEVector3(CurrentRadious * sinf(HAngle * X), YPos, CurrentRadious * cosf(HAngle * X)));
			N += 4;
		}
		PrevYPos = YPos;
		PrevRadious = CurrentRadious;
	}
	
	YPos = -HalfRadious;
	for (int X = 0; X < HSegments; X++)
	{
		AddWireframeVertex(Verts[N], Transformation, ZEVector3(0, YPos, 0));
		AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(PrevRadious * sinf(HAngle * X), PrevYPos, PrevRadious * cosf(HAngle * X)));
		N += 2;
	}
}

void ZECanvas::AddWireframeQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(8);
	AddWireframeVertex(Verts[0], Transformation, P0);
	AddWireframeVertex(Verts[1], Transformation, P1);
	
	AddWireframeVertex(Verts[2], Transformation, P1);
	AddWireframeVertex(Verts[3], Transformation, P2);

	AddWireframeVertex(Verts[4], Transformation, P2);
	AddWireframeVertex(Verts[5], Transformation, P3);

	AddWireframeVertex(Verts[6], Transformation, P3);
	AddWireframeVertex(Verts[7], Transformation, P0);
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
				AddWireframeVertex(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos + YPosDiff, HalfRadious * cosfHAngle));
				AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos, HalfRadious * cosfHAngle));
				N += 2;
			}
			AddWireframeVertex(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, YPos, HalfRadious * cosfHAngle));
			AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinf((X + 1) * HAngle), YPos, HalfRadious * cosf((X + 1) * HAngle)));
			N += 2;
		}
	}

	if (Caps)
		for (int X = 0; X < HSegments; X++)
		{
			float sinfHAngle = sinf(X * HAngle);
			float cosfHAngle = cosf(X * HAngle);
			AddWireframeVertex(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, 0.0f, HalfRadious * cosfHAngle));
			AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(0.0f, 0.0f, 0.0f));
			AddWireframeVertex(Verts[N + 2], Transformation, ZEVector3(HalfRadious * sinfHAngle, Height, HalfRadious * cosfHAngle));
			AddWireframeVertex(Verts[N + 3], Transformation, ZEVector3(0.0f, Height, 0.0f));
			N += 4;
		}
}

void ZECanvas::AddWireframePyramid(float Width, float Height, float Length)
{
		
	ZESimpleVertex* Verts = Vertices.MassAdd(16);
	Width *= 0.5f;
	Length *= 0.5f;

	AddWireframeVertex(Verts[0], Transformation, ZEVector3(Width, 0.0f, Length));
	AddWireframeVertex(Verts[1], Transformation, ZEVector3(0.0f, Height, 0.0f));

	AddWireframeVertex(Verts[2], Transformation, ZEVector3(Width, 0.0f, -Length));
	AddWireframeVertex(Verts[3], Transformation, ZEVector3(0.0f, Height, 0.0f));

	AddWireframeVertex(Verts[4], Transformation, ZEVector3(-Width, 0.0f, -Length));
	AddWireframeVertex(Verts[5], Transformation, ZEVector3(0.0f, Height, 0.0f));

	AddWireframeVertex(Verts[6], Transformation, ZEVector3(-Width, 0.0f, Length));
	AddWireframeVertex(Verts[7], Transformation, ZEVector3(0.0f, Height, 0.0f));

	AddWireframeVertex(Verts[8], Transformation, ZEVector3(Width, 0.0f, Length));
	AddWireframeVertex(Verts[9], Transformation, ZEVector3(Width, 0.0f, -Length));
	
	AddWireframeVertex(Verts[10], Transformation, ZEVector3(Width, 0.0f, -Length));
	AddWireframeVertex(Verts[11], Transformation, ZEVector3(-Width, 0.0f, -Length));

	AddWireframeVertex(Verts[12], Transformation, ZEVector3(-Width, 0.0f, -Length));
	AddWireframeVertex(Verts[13], Transformation, ZEVector3(-Width, 0.0f, Length));

	AddWireframeVertex(Verts[14], Transformation, ZEVector3(-Width, 0.0f, Length));
	AddWireframeVertex(Verts[15], Transformation, ZEVector3(Width, 0.0f, Length));
}

void ZECanvas::AddPoint(const ZEVector3& Point)
{
	ZESimpleVertex* Vertex = Vertices.Add();
	AddWireframeVertex(*Vertex, Transformation, Point);
}

void ZECanvas::AddLine(const ZEVector3& Point0, const ZEVector3& Point1)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(2);
	AddWireframeVertex(Verts[0], Transformation, Point0);
	AddWireframeVertex(Verts[1], Transformation, Point1);
}

void ZECanvas::AddCircle(float Radious, unsigned int Segments)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(Segments * 2);
	float HAngle = ((2.0f * ZE_PI) / Segments);
	float HalfRadious = 0.5 * Radious;
	int N = 0;
	for (int X = 0; X < Segments; X++)
	{
		AddWireframeVertex(Verts[N], Transformation, ZEVector3(HalfRadious * sinf(X * HAngle), 0.0f, HalfRadious * cosf(X * HAngle)));
		AddWireframeVertex(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinf((X + 1) * HAngle), 0.0f, HalfRadious * cosf((X + 1) * HAngle)));
		N += 2;
	}
}

void ZECanvas::AddPlane(float Width, float Length)
{
	ZESimpleVertex* Verts = Vertices.MassAdd(6);
	ZEVector3 Normal(0.0f, 1.0f, 0.0f);
	AddVertex(Verts[0], Transformation, ZEVector3(-Width, 0.0f, Length), Normal, ZEVector2(0.0f ,1.0f));
	AddVertex(Verts[1], Transformation, ZEVector3(Width, 0.0f, -Length), Normal, ZEVector2(1.0f ,0.0f));
	AddVertex(Verts[2], Transformation, ZEVector3(-Width, 0.0f, -Length), Normal, ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[3], Transformation, ZEVector3(-Width, 0.0f, Length), Normal, ZEVector2(0.0f, 1.0f));
	AddVertex(Verts[4], Transformation, ZEVector3(Width, 0.0f, Length), Normal, ZEVector2(1.0f, 1.0f));
	AddVertex(Verts[5], Transformation, ZEVector3(Width, 0.0f, -Length), Normal, ZEVector2(1.0f, 0.0f));
}

void ZECanvas::AddBox(float Width, float Height, float Length)
{
	Width /= 2;
	Height /= 2;
	Length /= 2;

	ZESimpleVertex* Verts = Vertices.MassAdd(36);

	AddVertex(Verts[0],  Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[1],  Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[2],  Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[3],  Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[4],  Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[5],  Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, -1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[6],  Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[7],  Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[8],  Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[9],  Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[10], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[11], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 1.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[12], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[13], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[14], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[15], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[16], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[17], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(0.0f, 0.0f, -1.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[18], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[19], Transformation, ZEVector3(Width, Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[20], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[21], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[22], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[23], Transformation, ZEVector3(Width, -Height, -Length),	ZEVector3(1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[24], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[25], Transformation, ZEVector3(Width, Height, Length),		ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[26], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[27], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[28], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[29], Transformation, ZEVector3(Width, -Height, Length),	ZEVector3(0.0f, 0.0f, 1.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[30], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	AddVertex(Verts[31], Transformation, ZEVector3(-Width, Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, -1.0f));
	AddVertex(Verts[32], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[33], Transformation, ZEVector3(-Width, Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, -1.0f));
	AddVertex(Verts[34], Transformation, ZEVector3(-Width, -Height, -Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(1.0f, 0.0f));
	AddVertex(Verts[35], Transformation, ZEVector3(-Width, -Height, Length),	ZEVector3(-1.0f, 0.0f, 0.0f),	ZEVector2(0.0f, 0.0f));
	

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
			AddVertex(Verts[N], Transformation, Position, Normal, ZEVector2(X * UDiff, (Y - 1) * VDiff));

			Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			AddVertex(Verts[N + 1], Transformation, Position, Normal, ZEVector2(X * UDiff, Y * VDiff));

			sinfHAngle = sinf(HAngle * (X + 1));
			cosfHAngle = cosf(HAngle * (X + 1));
			Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			AddVertex(Verts[N + 2], Transformation, Position, Normal,	ZEVector2((X + 1) * UDiff, (Y + 1) * VDiff));

			Verts[N + 3] = Verts[N + 2];

			Position = ZEVector3(PrevRadious * sinfHAngle, PrevYPos, PrevRadious * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			AddVertex(Verts[N + 4], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, (Y - 1) * VDiff));

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
		AddVertex(Verts[N], Transformation, Position, Normal, ZEVector2(sinfHAngle, 0.0f));

		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		AddVertex(Verts[N + 1], Transformation, Position, Normal, ZEVector2(sinfHAngle, cosfVAngle));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		AddVertex(Verts[N + 2], Transformation, Position, Normal,	ZEVector2(sinfHAngle, cosfVAngle));
		N += 3;
	}
	cosfVAngle = cosf(ZE_PI - VAngle);
	YPos = HalfRadious * cosfVAngle;
	for (int X = 0; X < HSegments; X++)
	{
		float sinfHAngle = sinf(HAngle * X);
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		AddVertex(Verts[N], Transformation, Position, Normal, ZEVector2(sinfHAngle, cosfVAngle));

		Position = ZEVector3(0.0f, -HalfRadious,  0.0f);
		ZEVector3::Normalize(Normal, Position);
		AddVertex(Verts[N + 1], Transformation, Position, Normal, ZEVector2(sinfHAngle, 0.0f));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadious * sinfHAngle, YPos, CurrentRadious * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		AddVertex(Verts[N + 2], Transformation, Position, Normal,	ZEVector2(sinfHAngle, cosfVAngle));
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

	AddVertex(Verts[0], Transformation, P0, Normal1, ZEVector2(0.0f ,0.0f));
	AddVertex(Verts[1], Transformation, P1, Normal1, ZEVector2(1.0f ,0.0f));
	AddVertex(Verts[2], Transformation, P2, Normal1, ZEVector2(1.0f, 1.0f));
	
	AddVertex(Verts[3], Transformation, P2, Normal2, ZEVector2(1.0f, 1.0f));
	AddVertex(Verts[4], Transformation, P3, Normal2, ZEVector2(0.0f, 1.0f));
	AddVertex(Verts[5], Transformation, P0, Normal2, ZEVector2(0.0f, 0.0f));
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
		AddVertex(Verts[N], Transformation, ZEVector3(HalfRadious * sinfHAngle, 0.0f, HalfRadious * cosf(X * HAngle)), Normal, ZEVector2(X * UVDif, 1.0f));
		AddVertex(Verts[N + 1], Transformation, ZEVector3(HalfRadious * sinfHAnglePP , 0.0f, HalfRadious * cosf((X + 1) * HAngle)), Normal, ZEVector2((X + 1) * UVDif, 1.0f));
		AddVertex(Verts[N + 2], Transformation, ZEVector3(0.0f, 0.0f, 0.0f), Normal, ZEVector2(X * UVDif, 0.0f));
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

	AddVertex(Verts[0], Transformation, ZEVector3(Width, 0.0f, -Length), ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[2], Transformation, ZEVector3(0.0f, Height, 0.0f),  ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.5f,1.0f));
	AddVertex(Verts[1], Transformation, ZEVector3(Width, 0.0f, Length),  ZEVector3(Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(1.0f,0.0f));

	AddVertex(Verts[3], Transformation, ZEVector3(Width, 0.0f, Length), ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[5], Transformation, ZEVector3(0.0f, Height, 0.0f), ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(0.5f,1.0f));
	AddVertex(Verts[4], Transformation, ZEVector3(-Width, 0.0f, Length),  ZEVector3(0.0f,Height/LengthNormal,Length/LengthNormal), ZEVector2(1.0f,0.0f));

	AddVertex(Verts[6], Transformation, ZEVector3(-Width, 0.0f, Length), ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[8], Transformation, ZEVector3(0.0f, Height, 0.0f),  ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(0.5f,1.0f));
	AddVertex(Verts[7], Transformation, ZEVector3(-Width, 0.0f, -Length),  ZEVector3(-Width/WidthNormal,Height/WidthNormal,0.0f), ZEVector2(1.0f,0.0f));

	AddVertex(Verts[9], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[11], Transformation, ZEVector3(0.0f, Height, 0.0f), ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(0.5f,1.0f));
	AddVertex(Verts[10], Transformation, ZEVector3(Width, 0.0f, -Length),  ZEVector3(0.0f,Height/LengthNormal,-Length/LengthNormal), ZEVector2(1.0f,0.0f));

	AddVertex(Verts[12], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[14], Transformation, ZEVector3(Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,0.0f));
	AddVertex(Verts[13], Transformation, ZEVector3(Width, 0.0f, Length),  ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,1.0f));

	AddVertex(Verts[15], Transformation, ZEVector3(-Width, 0.0f, -Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,0.0f));
	AddVertex(Verts[17], Transformation, ZEVector3(Width, 0.0f, Length), ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(1.0f,1.0f));
	AddVertex(Verts[16], Transformation, ZEVector3(-Width, 0.0f, Length),  ZEVector3(0.0f,-1.0f,0.0f), ZEVector2(0.0f,1.0f));
}
		
ZESimpleVertex* ZECanvas::ReserverVertexSpace(size_t Count)
{
	Vertices.MassAdd(Count);
}

void ZECanvas::AddVertex(const ZESimpleVertex& Vertex, bool DoTransformation)
{
	if (DoTransformation)
	{
		ZESimpleVertex* NewVertex = this->Vertices.Add();
		ZEMatrix4x4::Transform(NewVertex.Position, Transformation, Vertex.Position);
		ZEMatrix4x4::Transform3x3(NewVertex.Normal, Transformation, Vertex.Normal);
		NewVertex.Texcoord = Vertex.Texcoord;
	}
	else
		Vertices.AddByRef(Vertex);
}

void ZECanvas::AddVertex(const ZEVector3& Position, const ZEVector3& Normal, const ZEVector2& Texcoord, bool DoTransformation = true)
{
	ZESimpleVertex* NewVertex = this->Vertices.Add();
	if (DoTransformation)
	{
		ZEMatrix4x4::Transform(NewVertex.Position, Transformation, Position);
		ZEMatrix4x4::Transform3x3(NewVertex.Normal, Transformation, Normal);
		NewVertex.Texcoord = Texcoord;
	}
	else
	{
		NewVertex.Position = Position;
		NewVertex.Normal = Normal;
		NewVertex.Texcoord = Texcoord;
	}
}

void ZECanvas::AddWireframeVertex(const ZESimpleVertex& Vertex, bool DoTransformation = true)
{
	if (DoTransformation)
	{
		ZESimpleVertex* NewVertex = this->Vertices.Add();
		ZEMatrix4x4::Transform(NewVertex.Position, Transformation, Vertex.Position);
/*		ZEMatrix4x4::Transform3x3(NewVertex.Normal, Transformation, Vertex.Normal);
		NewVertex.Texcoord = Vertex.Texcoord;*/
	}
	else
		Vertices.AddByRef(Vertex);
}

void ZECanvas::AddWireframeVertex(const ZEVector3& Position, bool DoTransformation = true)
{
	ZESimpleVertex* NewVertex = this->Vertices.Add();
	if (DoTransformation)
	{
		ZEMatrix4x4::Transform(NewVertex.Position, Transformation, Position);
		//ZEMatrix4x4::Transform3x3(NewVertex.Normal, Transformation, Normal);
		//NewVertex.Texcoord = Texcoord;
	}
	else
	{
		NewVertex.Position = Position;
		//NewVertex.Normal = Normal;
		//NewVertex.Texcoord = Texcoord;
	}
}

void ZECanvas::AddVertices(const ZESimpleVertex* Vertices, size_t Count, bool DoTransformation)
{
	if (DoTransformation)
	{
		ZESimpleVertex* NewVertices = this->Vertices.MassAdd(Count);
		for (size_t I = 0; I < Count; I++)
		{
			ZEMatrix4x4::Transform(NewVertices[I].Position, Transformation, Vertices[I].Position);
			ZEMatrix4x4::Transform3x3(NewVertices[I].Normal, Transformation, Vertices[I].Normal);
			NewVertices[I].Texcoord = Vertices[I].Texcoord;
		}
	}
	else
		this->Vertices.MassAdd(Vertices, Count);
}

void ZECanvas::AddWireframeVertices(const ZESimpleVertex* Vertices, size_t Count, bool DoTransformation)
{
	if (DoTransformation)
	{
		ZESimpleVertex* NewVertices = this->Vertices.MassAdd(Count);
		for (size_t I = 0; I < Count; I++)
		{
			ZEMatrix4x4::Transform(NewVertices[I].Position, Transformation, Vertices[I].Position);
			//ZEMatrix4x4::Transform3x3(NewVertices[I].Normal, Transformation, Vertices[I].Normal);
			//NewVertices[I].Texcoord = Vertices[I].Texcoord;
		}
	}
	else
		this->Vertices.MassAdd(Vertices, Count);
}

void ZECanvas::CalculateBoundingBox(ZEAABoundingBox& BoundingBox)
{
	if (Vertices.GetCount() == 0)
		return;

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


ZEVertexBuffer* ZECanvas::CreateVertexBuffer(bool Static)
{
	ZEVertexBuffer* Buffer = zeGraphics->CreateVertexBuffer();
	Buffer->Create(Vertices.GetCount(), ZEVERTEXTYPE_SIMPLEVERTEX, Static);
	void* BufferPtr = Buffer->Lock();
	memcpy(BufferPtr, Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZESimpleVertex));
	Buffer->Unlock();
	return Buffer;
}

ZECanvas::ZECanvas()
{
	ZEMatrix4x4::CreateIdentity(Transformation);
}
