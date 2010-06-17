//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECanvas.cpp
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

#include "ZEMath\ZEDefinitions.h"
#include "ZECanvas.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEResourceFile.h"
#include "ZEGraphics\ZEVertexDeclaration.h"

#include <stdio.h>

#define ZECANVAS_ADDVERTEX(Vertex, Matrix, Pos, Nor, Texcrd)\
	ZEMatrix4x4::Transform((Vertex).Position, (Matrix), (Pos));\
	ZEMatrix4x4::Transform3x3((Vertex).Normal, (Matrix), (Nor));\
	(Vertex).Texcoord = (Texcrd);\
	(Vertex).Color = Color

#define ZECANVAS_ADDWIREVERTEX(Vertex, Matrix, Pos)\
	ZEMatrix4x4::Transform((Vertex).Position, (Matrix), (Pos));\
	(Vertex).Normal = ZEVector3(0.0f, 0.0f, 0.0f);\
	(Vertex).Texcoord = ZEVector2(0.0f, 0.0f);\
	(Vertex).Color = Color

ZEVertexDeclaration* ZECanvasVertex::VertexDeclaration = NULL;
ZEVertexDeclaration* ZECanvasVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT3, 0},
										{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT4, 1},
									};

	if (!VertexDeclaration->Create(ElementArray, 4))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	zeAssert(VertexDeclaration->GetVertexSize() != sizeof(ZECanvasVertex), "Vertex decleration size does not matches.");
	return VertexDeclaration;
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
void ZECanvas::SetTranslation(const ZEVector3& Translation)
{
	this->Translation = Translation;
	ZEMatrix4x4::CreateOrientation(Transformation, Translation, Rotation, Scale);
}

const ZEVector3& ZECanvas::GetTranslation()
{
	return this->Translation;
}

void ZECanvas::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
	ZEMatrix4x4::CreateOrientation(Transformation, Translation, Rotation, Scale);
}

const ZEQuaternion& ZECanvas::GetRotation()
{
	return Rotation;
}

void ZECanvas::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
	ZEMatrix4x4::CreateOrientation(Transformation, Translation, Rotation, Scale);
}

const ZEVector3& ZECanvas::GetScale()
{
	return Scale;
}

void ZECanvas::SetColor(const ZEVector4& Color)
{
	this->Color = Color;
}

const ZEVector4& ZECanvas::GetColor()
{
	return Color;
}

void ZECanvas::ApplyTranslation(const ZEVector3& Translation)
{
	ZEMatrix4x4 TranslationMatrix;
	ZEMatrix4x4::CreateTranslation(TranslationMatrix, Translation);
	ZEMatrix4x4::Multiply(Transformation, ZEMatrix4x4(Transformation), TranslationMatrix);
}

void ZECanvas::ApplyRotation(const ZEQuaternion& Rotation)
{
	ZEMatrix4x4 RotationMatrix;
	ZEMatrix4x4::CreateRotation(RotationMatrix, Rotation);
	ZEMatrix4x4::Multiply(Transformation, ZEMatrix4x4(Transformation), RotationMatrix);
}

void ZECanvas::ApplyScale(const ZEVector3& Scale)
{
	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateTranslation(ScaleMatrix, Scale);
	ZEMatrix4x4::Multiply(Transformation, ZEMatrix4x4(Transformation), ScaleMatrix);
}

void ZECanvas::AddWireframePlane(float Width, float Length, unsigned int VSegment, unsigned int HSegment)
{
	float XPosDiff = Width / (HSegment + 1);
	float ZPosDiff = Length / (VSegment + 1);
	Length *= 0.5f;
	Width *= 0.5f;
	
	ZECanvasVertex* Verts = Vertices.MassAdd((VSegment + 2) * 2 + (HSegment + 2)* 2);

	size_t N = 0;
	for (size_t I = 0; I <= VSegment + 1; I++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, Length));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(I * XPosDiff - Width, 0.0f, -Length));
		N += 2;
	}

	for (size_t I = 0; I <= HSegment + 1; I++)
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

	ZECanvasVertex* Verts = Vertices.MassAdd(24);

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

void ZECanvas::AddWireframeSphere(float Radius, unsigned int HSegments, unsigned int VSegments)
{
	float VAngle = ZE_PI / (VSegments + 1);
	float HAngle = 2.0f * ZE_PI / HSegments;

	size_t N = 0;
	ZECanvasVertex* Verts =  Vertices.MassAdd(VSegments * HSegments * 4 + HSegments * 2);

	float YPos = Radius;
	float PrevRadius = 0.0f;
	float PrevYPos = YPos;

	float CurrentRadius;
	for (size_t Y = 1; Y <= VSegments; Y++)
	{
		YPos = Radius * cosf(VAngle * Y);
		CurrentRadius = Radius * sinf(VAngle * Y);
		for (size_t X = 0; X < HSegments; X++)
		{
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation,		ZEVector3(CurrentRadius * sinf(HAngle * X), YPos, CurrentRadius * cosf(HAngle * X)));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation,	ZEVector3(CurrentRadius * sinf(HAngle * (X + 1)), YPos, CurrentRadius * cosf(HAngle * (X + 1))));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 2], Transformation,	ZEVector3(PrevRadius * sinf(HAngle * X), PrevYPos, PrevRadius * cosf(HAngle * X)));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 3], Transformation,	ZEVector3(CurrentRadius * sinf(HAngle * X), YPos, CurrentRadius * cosf(HAngle * X)));
			N += 4;
		}
		PrevYPos = YPos;
		PrevRadius = CurrentRadius;
	}
	
	YPos = -Radius;
	for (size_t X = 0; X < HSegments; X++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(0, YPos, 0));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(PrevRadius * sinf(HAngle * X), PrevYPos, PrevRadius * cosf(HAngle * X)));
		N += 2;
	}
}

void ZECanvas::AddWireframeTriangle(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(6);
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, P0);
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, P1);
	
	ZECANVAS_ADDWIREVERTEX(Verts[2], Transformation, P1);
	ZECANVAS_ADDWIREVERTEX(Verts[3], Transformation, P2);

	ZECANVAS_ADDWIREVERTEX(Verts[4], Transformation, P2);
	ZECANVAS_ADDWIREVERTEX(Verts[5], Transformation, P0);
}

void ZECanvas::AddWireframeQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(8);
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, P0);
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, P1);
	
	ZECANVAS_ADDWIREVERTEX(Verts[2], Transformation, P1);
	ZECANVAS_ADDWIREVERTEX(Verts[3], Transformation, P2);

	ZECANVAS_ADDWIREVERTEX(Verts[4], Transformation, P2);
	ZECANVAS_ADDWIREVERTEX(Verts[5], Transformation, P3);

	ZECANVAS_ADDWIREVERTEX(Verts[6], Transformation, P3);
	ZECANVAS_ADDWIREVERTEX(Verts[7], Transformation, P0);
}

void ZECanvas::AddWireframeCylinder(float Radius, float Height, unsigned int HSegments, unsigned int VSegments, bool Caps)
{
	float HAngle = ((2.0f * ZE_PI) / HSegments);
	float YPos;
	float YPosDiff = (Height / (VSegments + 1));

	size_t N = 0;
	ZECanvasVertex* Verts =  Vertices.MassAdd((VSegments + 2) * HSegments * 2 + (VSegments + 1) * HSegments * 2 + (Caps ? 4 * HSegments : 0));
	
	for (size_t Y = 0; Y < VSegments + 2; Y++)
	{
		YPos = YPosDiff * Y ;
		for (size_t X = 0; X < HSegments; X++)
		{
			float sinfHAngle = sinf(X * HAngle);
			float cosfHAngle = cosf(X * HAngle);

			if (Y != VSegments + 1)
			{
				ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(Radius * sinfHAngle, YPos + YPosDiff, Radius * cosfHAngle));
				ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(Radius * sinfHAngle, YPos, Radius * cosfHAngle));
				N += 2;
			}
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(Radius * sinfHAngle, YPos, Radius * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(Radius * sinf((X + 1) * HAngle), YPos, Radius * cosf((X + 1) * HAngle)));
			N += 2;
		}
	}

	if (Caps)
		for (size_t X = 0; X < HSegments; X++)
		{
			float sinfHAngle = sinf(X * HAngle);
			float cosfHAngle = cosf(X * HAngle);
			ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(Radius * sinfHAngle, 0.0f, Radius * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(0.0f, 0.0f, 0.0f));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 2], Transformation, ZEVector3(Radius * sinfHAngle, Height, Radius * cosfHAngle));
			ZECANVAS_ADDWIREVERTEX(Verts[N + 3], Transformation, ZEVector3(0.0f, Height, 0.0f));
			N += 4;
		}
}

void ZECanvas::AddWireframePyramid(float Width, float Height, float Length)
{
		
	ZECanvasVertex* Verts = Vertices.MassAdd(16);
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
	ZECanvasVertex* Vertex = Vertices.Add();
	ZECANVAS_ADDWIREVERTEX(*Vertex, Transformation, Point);
}

void ZECanvas::AddLine(const ZEVector3& Point0, const ZEVector3& Point1)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(2);
	ZECANVAS_ADDWIREVERTEX(Verts[0], Transformation, Point0);
	ZECANVAS_ADDWIREVERTEX(Verts[1], Transformation, Point1);
}

void ZECanvas::AddCircle(float Radius, unsigned int Segments)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(Segments * 2);
	float HAngle = ((2.0f * ZE_PI) / Segments);
	size_t N = 0;
	for (size_t X = 0; X < Segments; X++)
	{
		ZECANVAS_ADDWIREVERTEX(Verts[N], Transformation, ZEVector3(Radius * sinf(X * HAngle), 0.0f, Radius * cosf(X * HAngle)));
		ZECANVAS_ADDWIREVERTEX(Verts[N + 1], Transformation, ZEVector3(Radius * sinf((X + 1) * HAngle), 0.0f, Radius * cosf((X + 1) * HAngle)));
		N += 2;
	}
}

void ZECanvas::AddPlane(float Width, float Length)
{
	Width *= 0.5f;
	Length *= 0.5f;

	ZECanvasVertex* Verts = Vertices.MassAdd(6);
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

	ZECanvasVertex* Verts = Vertices.MassAdd(36);

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

void ZECanvas::AddSphere(float Radius, unsigned int HSegments, unsigned int VSegments)
{
	float VAngle = ZE_PI / (VSegments + 1);
	float HAngle = 2.0f * ZE_PI / HSegments;
	float UDiff = 1.0f / HSegments;
	float VDiff = 1.0f / VSegments;

	int N = 0;
	ZECanvasVertex* Verts =  Vertices.MassAdd((VSegments) * HSegments * 6 + HSegments * 6);

	float YPos;
	float PrevRadius = Radius * sinf(VAngle);
	float PrevYPos = Radius * cosf(VAngle);
	float PrevcosfVAngle = cosf(VAngle);
	float cosfVAngle;
	float cosfHAngle;
	float CurrentRadius;

	ZEVector3 Position, Normal;
	
	for (size_t Y = 1; Y <= VSegments; Y++)
	{
		cosfVAngle = cosf(VAngle * Y);
		YPos = Radius * cosfVAngle;
		CurrentRadius = Radius * sinf(VAngle * Y);
		for (size_t X = 0; X < HSegments; X++)
		{
			float sinfHAngle;
		
			sinfHAngle = sinf(HAngle * X);
			cosfHAngle = cosf(HAngle * X);
			Position = ZEVector3(PrevRadius * sinfHAngle , PrevYPos, PrevRadius * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(X * UDiff, Y * VDiff));

			Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2(X * UDiff, (Y + 1) * VDiff));

			sinfHAngle = sinf(HAngle * (X + 1));
			cosfHAngle = cosf(HAngle * (X + 1));
			Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2((X + 1) * UDiff, (Y + 1) * VDiff));

			Verts[N + 3] = Verts[N + 2];

			Position = ZEVector3(PrevRadius * sinfHAngle, PrevYPos, PrevRadius * cosfHAngle);
			ZEVector3::Normalize(Normal, Position);
			ZECANVAS_ADDVERTEX(Verts[N + 4], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, Y * VDiff));

			Verts[N + 5] = Verts[N];

			N += 6;
		}
		PrevYPos = YPos;
		PrevRadius = CurrentRadius;
		PrevcosfVAngle = cosfVAngle;
	}
	
	cosfVAngle = cosf(VAngle);
	YPos = Radius * cosfVAngle;
	CurrentRadius = Radius * sinf(VAngle);
	for (size_t X = 0; X < HSegments; X++)
	{
		float sinfHAngle = sinf(HAngle * X);
		Position = ZEVector3(0.0f, Radius,  0.0f);
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(0.5f, 1.0f));

		Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, 1.0f - VDiff));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2((X) * UDiff, 1.0f - VDiff));
		N += 3;
	}
	cosfVAngle = cosf(ZE_PI - VAngle);
	YPos = Radius * cosfVAngle;
	for (size_t X = 0; X < HSegments; X++)
	{
		float sinfHAngle = sinf(HAngle * X);
		Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosf(HAngle * X));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, Position, Normal, ZEVector2(0.5f, 1.0f));

		Position = ZEVector3(0.0f, -Radius,  0.0f);
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, Position, Normal, ZEVector2((X + 1) * UDiff, 1.0f - VDiff));

		sinfHAngle = sinf(HAngle * (X + 1));
		Position = ZEVector3(CurrentRadius * sinfHAngle, YPos, CurrentRadius * cosf(HAngle * (X + 1)));
		ZEVector3::Normalize(Normal, Position);
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, Position, Normal,	ZEVector2(X * UDiff, 1.0f - VDiff));
		N += 3;
	}
}

void ZECanvas::AddTriangle(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(3);
	ZEVector3 Normal;
	ZEVector3::CrossProduct(Normal, P1 - P0, P2 - P0);
	ZEVector3::Normalize(Normal, Normal);


	ZECANVAS_ADDVERTEX(Verts[0], Transformation, P0, Normal, ZEVector2(0.0f ,0.0f));
	ZECANVAS_ADDVERTEX(Verts[1], Transformation, P1, Normal, ZEVector2(1.0f ,0.0f));
	ZECANVAS_ADDVERTEX(Verts[2], Transformation, P2, Normal, ZEVector2(1.0f, 1.0f));
}

void ZECanvas::AddQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(6);
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

void ZECanvas::AddDisc(float Radius, unsigned int Segments)
{
	ZECanvasVertex* Verts = Vertices.MassAdd(Segments * 3);
	ZEVector3 Normal(0.0f, 1.0f, 0.0f);

	float HAngle = (2.0f * ZE_PI) / Segments;
	float UVDif = 1.0f / Segments;
	int N = 0;
	for (int X = 0; X < Segments; X++)
	{
		float sinfHAngle = sinf(X * HAngle);
		float sinfHAnglePP = sinf((X + 1) * HAngle);
		ZECANVAS_ADDVERTEX(Verts[N], Transformation, ZEVector3(Radius * sinfHAngle, 0.0f, Radius * cosf(X * HAngle)), Normal, ZEVector2(X * UVDif, 1.0f));
		ZECANVAS_ADDVERTEX(Verts[N + 1], Transformation, ZEVector3(Radius * sinfHAnglePP , 0.0f, Radius * cosf((X + 1) * HAngle)), Normal, ZEVector2((X + 1) * UVDif, 1.0f));
		ZECANVAS_ADDVERTEX(Verts[N + 2], Transformation, ZEVector3(0.0f, 0.0f, 0.0f), Normal, ZEVector2(X * UVDif, 0.0f));
		N += 3;
	}
}

void ZECanvas::AddPyramid(float Width, float Height, float Length)
{
	

	ZECanvasVertex* Verts = Vertices.MassAdd(18);
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

void ZECanvas::AddVertex(const ZECanvasVertex& Vertex)
{
	Vertices.AddByRef(Vertex);
}

void ZECanvas::AddVertices(const ZECanvasVertex* Vertices, size_t Count)
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
	Translation = ZEVector3::Zero;
	Scale = ZEVector3::One;
	TransformationStack.Clear();
	Transformation = ZEMatrix4x4::Identity;

	Vertices.Clear();
}

bool ZECanvas::LoadFromFile(const char* FileName)
{
	ZEResourceFile File;
	if (!File.Open(FileName))
	{
		zeError("Canvas", "Can not load canvas file. (FileName : \"%s\")", FileName);
		return false;
	}
	
	
	int VertexCount;
	fscanf((FILE*)File.GetFileHandle(), "%d", &VertexCount);

	ZECanvasVertex* Vertices = this->Vertices.MassAdd(VertexCount);
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
			zeError("Canvas", "Corrupted canvas file. Vertex count does not match. (FileName : \"%s\")", FileName);
			return false;
		}*/
	}

	if (Index != VertexCount + 1 )
		zeWarning("Canvas", "Corrupted canvas file. Vertex count is less than verties in the file. (FileName : \"%s\")", FileName);

	return true;
}

ZEStaticVertexBuffer* ZECanvas::CreateStaticVertexBuffer()
{
	ZEStaticVertexBuffer* Buffer = ZEStaticVertexBuffer::CreateInstance();
	Buffer->Create(Vertices.GetCount() * sizeof(ZECanvasVertex));
	void* BufferPtr = Buffer->Lock();
	memcpy(BufferPtr, Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZECanvasVertex));
	Buffer->Unlock();
	return Buffer;
}

unsigned int ZECanvas::GetBufferSize()
{
	return Vertices.GetCount() * sizeof(ZECanvasVertex);
}

void* ZECanvas::GetVertexBuffer()
{
	return Vertices.GetCArray();
}
		
ZECanvas::ZECanvas()
{
	Transformation = ZEMatrix4x4::Identity;
	Translation = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}


