//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainVertexBuffer.cpp
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

#include "ZETerrainVertexBuffer.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEGraphics/ZEVertexBuffer.h"

struct ZETerrainVertex
{
	ZEVector3 Position;
};

static int CreateQuad(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x, 0.0f, y);
	Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
	Vertices[2].Position = ZEVector3(x, 0.0f, y - 1);

	Vertices[3].Position = Vertices[2].Position;
	Vertices[4].Position = Vertices[1].Position;
	Vertices[5].Position = ZEVector3(x + 1, 0.0f, y - 1);

	return 6;
}

static int CreateQuad(ZETerrainVertex* Vertices, int EdgeLength)
{
	size_t Index = 0;
	for (int y = EdgeLength / 2; y > -EdgeLength / 2; y--)
		for (int x = -EdgeLength / 2; x < EdgeLength / 2; x++)
			Index += CreateQuad(Vertices + Index, x, y);


	return Index;
}

static int CreateLeftEdge(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x,	0.0f, y);
	Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
	Vertices[2].Position = ZEVector3(x + 1,	0.0f, y - 1);

	Vertices[3].Position = Vertices[0].Position;
	Vertices[4].Position = Vertices[2].Position;
	Vertices[5].Position = ZEVector3(x, 0.0f, y - 2);

	Vertices[6].Position = Vertices[5].Position;
	Vertices[7].Position = Vertices[2].Position;
	Vertices[8].Position = ZEVector3(x + 1, 0.0f, y - 2);

	return 9;
}

static int CreateBottomEdge(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x,	0.0f, y);
	Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
	Vertices[2].Position = ZEVector3(x,	0.0f, y - 1);

	Vertices[3].Position = Vertices[2].Position;
	Vertices[4].Position = Vertices[1].Position;
	Vertices[5].Position = ZEVector3(x + 2, 0.0f, y - 1);

	Vertices[6].Position = Vertices[5].Position;
	Vertices[7].Position = Vertices[1].Position;
	Vertices[8].Position = ZEVector3(x + 2, 0.0f, y);

	return 9;
}

static int CreateLeftEdge(ZETerrainVertex* Vertices, int EdgeLength)
{	
	size_t Index = 0;

	for (int y = EdgeLength / 2; y > -EdgeLength / 2; y--)
		for (int x = -EdgeLength / 2 + 1; x < EdgeLength / 2; x++)
			Index += CreateQuad(Vertices + Index, x, y);

	int x =  -EdgeLength / 2;
	for (int y = EdgeLength / 2; y > -EdgeLength / 2; y -= 2)
		Index += CreateLeftEdge(Vertices + Index, x, y);

	return Index;
}

static int CreateTopEdge(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x,	0.0f, y);
	Vertices[1].Position = ZEVector3(x + 1, 0.0f, y - 1);
	Vertices[2].Position = ZEVector3(x,	0.0f, y - 1);

	Vertices[3].Position = Vertices[1].Position;
	Vertices[4].Position = Vertices[0].Position;
	Vertices[5].Position = ZEVector3(x + 2, 0.0f, y);

	Vertices[6].Position = Vertices[3].Position;
	Vertices[7].Position = Vertices[5].Position;
	Vertices[8].Position = ZEVector3(x + 2, 0.0f, y - 1);

	return 9;
}

static int CreateTopEdge(ZETerrainVertex* Vertices, int EdgeLength)
{	
	size_t Index = 0;

	int y =  EdgeLength / 2;
	for (int x = -EdgeLength / 2; x < EdgeLength / 2; x += 2)
		Index += CreateTopEdge(Vertices + Index, x, y);

	for (int y = EdgeLength / 2 - 1; y > -EdgeLength / 2; y--)
		for (int x = -EdgeLength / 2; x < EdgeLength / 2; x++)
			Index += CreateQuad(Vertices + Index, x, y);

	return Index;
}

static int CreateLeftBottomCorner(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x, 0.0f, y);
	Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
	Vertices[2].Position = ZEVector3(x + 1, 0.0f, y - 1);

	Vertices[3].Position = Vertices[0].Position;
	Vertices[4].Position = Vertices[2].Position;
	Vertices[5].Position = ZEVector3(x, 0.0f, y - 2);

	Vertices[6].Position = Vertices[5].Position;
	Vertices[7].Position = Vertices[2].Position;
	Vertices[8].Position = ZEVector3(x + 2, 0.0f, y - 2);

	Vertices[9].Position = Vertices[8].Position;
	Vertices[10].Position = Vertices[2].Position;
	Vertices[11].Position = ZEVector3(x + 2, 0.0f, y - 1);

	CreateQuad(Vertices + 12, x + 1, y);

	return 12 + 6;
}

static int CreateLeftBottomCorner(ZETerrainVertex* Vertices, int EdgeLength)
{	
	size_t Index = 0;

	for (int y = EdgeLength / 2; y > -EdgeLength / 2; y -= 2)
		for (int x = -EdgeLength / 2; x < EdgeLength / 2; x += 2)
			if (x == -EdgeLength / 2)
			{
				if (y == -EdgeLength / 2 + 2)
					Index += CreateLeftBottomCorner(Vertices + Index, x, y);
				else
				{
					Index += CreateLeftEdge(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x + 1, y);
					Index += CreateQuad(Vertices + Index, x + 1, y - 1);
				}
			}
			else
			{
				if (y == -EdgeLength / 2 + 2)
				{
					Index += CreateBottomEdge(Vertices + Index, x, y - 1);
					Index += CreateQuad(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x + 1, y);
				}
				else
				{
					Index += CreateQuad(Vertices + Index, x, y - 1);
					Index += CreateQuad(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x + 1, y - 1);
					Index += CreateQuad(Vertices + Index, x + 1, y);
				}
			}

	return Index;
}


static int CreateLeftTopCorner(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x, 0.0f, y);
	Vertices[1].Position = ZEVector3(x + 2, 0.0f, y);
	Vertices[2].Position = ZEVector3(x + 1, 0.0f, y - 1);

	Vertices[3].Position = Vertices[2].Position;
	Vertices[4].Position = Vertices[1].Position;
	Vertices[5].Position = ZEVector3(x + 2, 0.0f, y - 1);

	Vertices[6].Position = Vertices[0].Position;
	Vertices[7].Position = Vertices[2].Position;
	Vertices[8].Position = ZEVector3(x, 0.0f, y - 2);

	Vertices[9].Position = Vertices[8].Position;
	Vertices[10].Position = Vertices[7].Position;
	Vertices[11].Position = ZEVector3(x + 1, 0.0f, y - 2);

	CreateQuad(Vertices + 12, x + 1, y - 1);

	return 12 + 6;
}

static int CreateLeftTopCorner(ZETerrainVertex* Vertices, int EdgeLength)
{	
	size_t Index = 0;

	for (int y = EdgeLength / 2; y > -EdgeLength / 2; y -= 2)
		for (int x = -EdgeLength / 2; x < EdgeLength / 2; x += 2)
			if (x == -EdgeLength / 2)
			{
				if (y == EdgeLength / 2)
					Index += CreateLeftTopCorner(Vertices + Index, x, y);
				else
				{
					Index += CreateLeftEdge(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x + 1, y);
					Index += CreateQuad(Vertices + Index, x + 1, y - 1);
				}
			}
			else
			{
				if (y == EdgeLength / 2)
				{
					Index += CreateTopEdge(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x, y - 1);
					Index += CreateQuad(Vertices + Index, x + 1, y - 1);
				}
				else
				{
					Index += CreateQuad(Vertices + Index, x, y - 1);
					Index += CreateQuad(Vertices + Index, x, y);
					Index += CreateQuad(Vertices + Index, x + 1, y - 1);
					Index += CreateQuad(Vertices + Index, x + 1, y);
				}
			}

	return Index;
}

void ApplyTransform(ZETerrainVertex* Vertices, size_t Start, size_t End, const ZEMatrix4x4& Transform)
{
	for (int I = Start; I < End; I++)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Transform, Vertices[I].Position);
		Vertices[I].Position = Temp;
	}
}

int CreateCenter(ZETerrainVertex* Vertices, int EdgeLength, float Scale)
{	
	size_t TotalIndex = 0;
	int Index;
	ZEMatrix4x4 Transform;

	Index = CreateQuad(Vertices + TotalIndex, EdgeLength);
	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-EdgeLength + EdgeLength / 2, 0.0f, EdgeLength - EdgeLength / 2));
	ApplyTransform(Vertices, TotalIndex, TotalIndex + Index, Transform);
	TotalIndex += Index;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-EdgeLength + EdgeLength / 2, 0.0f, 0 - EdgeLength / 2));
	Index = CreateQuad(Vertices + TotalIndex, EdgeLength);
	ApplyTransform(Vertices, TotalIndex, TotalIndex + Index, Transform);
	TotalIndex += Index;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(0 + EdgeLength / 2, 0.0f, EdgeLength - EdgeLength / 2));
	Index = CreateQuad(Vertices + TotalIndex, EdgeLength);
	ApplyTransform(Vertices, TotalIndex, TotalIndex + Index, Transform);
	TotalIndex += Index;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(0 + EdgeLength / 2, 0.0f, 0 - EdgeLength / 2));
	Index = CreateQuad(Vertices + TotalIndex, EdgeLength);
	ApplyTransform(Vertices, TotalIndex, TotalIndex + Index, Transform);
	TotalIndex += Index;

	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateScale(ScaleMatrix, Scale, Scale, Scale);
	ApplyTransform(Vertices, TotalIndex, TotalIndex + Index, Transform);

	return TotalIndex;
}


int CreateRing(ZETerrainVertex* Vertices, int EdgeLength, float Scale)
{
	size_t Index = 0;
	int Size;
	ZEMatrix4x4 Transform;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-2 * EdgeLength + EdgeLength / 2, 0.0f, 2 * EdgeLength - EdgeLength / 2));
	Size = CreateLeftTopCorner(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-EdgeLength + EdgeLength / 2, 0.0f, 2 * EdgeLength - EdgeLength / 2));
	Size = CreateTopEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(0.0f + EdgeLength / 2, 0.0f, 2 * EdgeLength - EdgeLength / 2));
	Size = CreateTopEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-2 * EdgeLength + EdgeLength / 2, 0.0f, -EdgeLength - EdgeLength / 2));
	Size = CreateLeftBottomCorner(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	// Left
	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-2 * EdgeLength + EdgeLength / 2, 0.0f, EdgeLength - EdgeLength / 2));
	Size = CreateLeftEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateTranslation(Transform, ZEVector3(-2 * EdgeLength + EdgeLength / 2, 0.0f, 0 - EdgeLength / 2));
	Size = CreateLeftEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	// Right
	ZEMatrix4x4::CreateOrientation(Transform, ZEVector3(EdgeLength + EdgeLength / 2, 0.0f, EdgeLength - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateLeftEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateOrientation(Transform, ZEVector3(EdgeLength + EdgeLength / 2, 0.0f, 0 - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateLeftEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	// Bottom
	ZEMatrix4x4::CreateOrientation(Transform,ZEVector3(EdgeLength + EdgeLength / 2, 0.0f, 2 * EdgeLength - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateLeftBottomCorner(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateOrientation(Transform, ZEVector3(-EdgeLength + EdgeLength / 2, 0.0f, -EdgeLength - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateTopEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateOrientation(Transform, ZEVector3(0.0f + EdgeLength / 2, 0.0f, -EdgeLength - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateTopEdge(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4::CreateOrientation(Transform, ZEVector3(EdgeLength + EdgeLength / 2, 0.0f, -EdgeLength - EdgeLength / 2), ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	Size = CreateLeftTopCorner(Vertices + Index, EdgeLength);
	ApplyTransform(Vertices, Index, Index + Size, Transform);
	Index += Size;

	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateScale(ScaleMatrix, Scale, Scale, Scale);
	ApplyTransform(Vertices, 0, Index, ScaleMatrix);

	return Index;
}

bool ZETerrainPrimitivesGenerator::Generate(ZEStaticVertexBuffer** VertexBuffer, ZETerrainPrimitiveIndices* Indices, int EdgeLength)
{
	int QuadSize = 6 * EdgeLength * EdgeLength;
	Indices->LeftTopCornerSize = Indices->LeftBottomCornerSize = 3 * (2 * (EdgeLength - 1) * (EdgeLength - 1) + 3 * EdgeLength - 2);
	Indices->LeftEdgeSize = Indices->TopEdgeIndex = 3 * (2 * EdgeLength * EdgeLength - EdgeLength / 2);
	Indices->CenterQuadSize = 4 * QuadSize + 4 * Indices->LeftTopCornerSize + 8 * Indices->LeftEdgeSize;

	size_t VertexBufferSize = 
		2 * Indices->LeftTopCornerSize + 
		2 * Indices->LeftEdgeSize +
		Indices->CenterQuadSize;

	VertexBufferSize *= sizeof(ZETerrainVertex);

	if (*VertexBuffer == 0)
		*VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (!(*VertexBuffer)->Create(VertexBufferSize))
		return false;

	ZETerrainVertex* Vertices = (ZETerrainVertex*)(*VertexBuffer)->Lock();
	if (Vertices == NULL)
		return false;

	// Primitives
	size_t Index = 0;

	Indices->CenterQuadIndex = 0;
	Indices->CenterQuadSize = CreateQuad(Vertices, EdgeLength);
	//Indices->CenterQuadSize += CreateRing(Vertices + Indices->CenterQuadSize, EdgeLength, 1.0f);
	Index += Indices->CenterQuadSize;

	// Left
	Indices->LeftEdgeIndex = Index;
	Indices->LeftEdgeSize = CreateLeftEdge(Vertices + Index, EdgeLength);
	Index += Indices->LeftEdgeSize;

	// Top
	Indices->TopEdgeIndex = Index;
	Indices->TopEdgeSize = CreateTopEdge(Vertices + Index, EdgeLength);
	Index += Indices->TopEdgeSize;

	// LeftTopCorner
	Indices->LeftTopCornerIndex = Index;
	Indices->LeftTopCornerSize = CreateLeftTopCorner(Vertices + Index, EdgeLength);
	Index += Indices->LeftTopCornerSize;

	// LeftBottomCorner
	Indices->LeftBottomCornerIndex = Index;
	Indices->LeftBottomCornerSize = CreateLeftBottomCorner(Vertices + Index, EdgeLength);
	Index += Indices->LeftBottomCornerSize;

	(*VertexBuffer)->Unlock();
}
