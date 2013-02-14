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

static ZEInt EdgeLength;

static ZESize CreateQuad(ZETerrainVertex* Vertices, float x, float y)
{
	Vertices[0].Position = ZEVector3(x, 0.0f, y);
	Vertices[1].Position = ZEVector3(x, 0.0f, y + 1);
	Vertices[2].Position = ZEVector3(x + 1, 0.0f, y + 1);

	Vertices[3].Position = Vertices[0].Position;
	Vertices[4].Position = Vertices[2].Position;
	Vertices[5].Position = ZEVector3(x + 1, 0.0f, y);	

	return 6;
}

static ZESize CreateVerticalBlock(ZETerrainVertex* Vertices, ZEInt Width = -1)
{
	if (Width == -1)
	{
		Width = EdgeLength;
	}

	ZESize Index = 0;
	for (ZEInt y = -1; y < EdgeLength + 1; y++)
	{
		for (ZEInt x = 0; x < Width; x++)
		{
			Index += CreateQuad(Vertices + Index, (float)x, (float)y);
		}
	}
	
	return Index;
}

static ZESize CreateHorizontalBlock(ZETerrainVertex* Vertices, ZEInt Height = -1)
{
	if (Height == -1)
		Height = EdgeLength;

	ZESize Index = 0;
	for (ZEInt x = -1; x < EdgeLength + 1; x++)
	{
		for (ZEInt y = 0; y < Height; y++)
		{
			Index += CreateQuad(Vertices + Index, (float)x, (float)y);
		}
	}

	return Index;
}

bool ZETerrainPrimitivesGenerator::Generate(ZEVertexBuffer** VertexBuffer, ZETerrainPrimitiveIndices* Indices, ZEInt EdgeLength)
{
	::EdgeLength = EdgeLength;
	
	ZESize VertexBufferSize = 
		2 * (ZESize)EdgeLength * ((ZESize)EdgeLength + 2) +
		1 * ((ZESize)EdgeLength - 1) * ((ZESize)EdgeLength + 2) +
		3 * ((ZESize)EdgeLength + 2);
	
	VertexBufferSize *= 6 * sizeof(ZETerrainVertex);

	if (*VertexBuffer == 0)
		*VertexBuffer = ZEVertexBuffer::CreateInstance();

	if (!(*VertexBuffer)->CreateDynamic((ZEUInt)(VertexBufferSize / sizeof(ZETerrainVertex)), sizeof(ZETerrainVertex), NULL))
		return false;

	ZETerrainVertex* Vertices = NULL;
	if (!(*VertexBuffer)->Lock((void**)VertexBuffer))
		return false;

	// Primitives
	ZEInt HalfLength = EdgeLength / 2;
	ZEInt Negative = -2 * EdgeLength;
	ZEInt Positivie = -2 * EdgeLength;

	ZESize VertexIndex = 0;
	Indices->Index[ZE_TP_HORIZONTAL] = VertexIndex;
	VertexIndex += CreateHorizontalBlock(Vertices + VertexIndex);

	Indices->Index[ZE_TP_VERTICAL] = VertexIndex;
	VertexIndex += CreateVerticalBlock(Vertices + VertexIndex);

	Indices->Index[ZE_TP_CORNER] = VertexIndex;
	VertexIndex += CreateVerticalBlock(Vertices + VertexIndex, EdgeLength - 1);

	Indices->Index[ZE_TP_CORNER_1] = VertexIndex;
	VertexIndex += CreateVerticalBlock(Vertices + VertexIndex, 1);

	Indices->Index[ZE_TP_CORNER_2] = VertexIndex;
	VertexIndex += CreateVerticalBlock(Vertices + VertexIndex, 2);

	zeDebugCheck(VertexIndex * sizeof(ZETerrainVertex) != VertexBufferSize, "Vertex counts does not match.");
	(*VertexBuffer)->Unlock();

	return true;
}
