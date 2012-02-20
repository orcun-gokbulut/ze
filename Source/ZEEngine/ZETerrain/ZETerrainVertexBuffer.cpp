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
	/*if (((int)x + (int)y) % 2 == 0)	
	{
		Vertices[0].Position = ZEVector3(x, 0.0f, y);
		Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
		Vertices[2].Position = ZEVector3(x + 1, 0.0f, y - 1);

		Vertices[3].Position = Vertices[0].Position;
		Vertices[4].Position = Vertices[2].Position;
		Vertices[5].Position = ZEVector3(x, 0.0f, y - 1);	
}
	else
	{*/
		Vertices[0].Position = ZEVector3(x, 0.0f, y);
		Vertices[1].Position = ZEVector3(x + 1, 0.0f, y);
		Vertices[2].Position = ZEVector3(x, 0.0f, y - 1);

		Vertices[3].Position = Vertices[2].Position;
		Vertices[4].Position = Vertices[1].Position;
		Vertices[5].Position = ZEVector3(x + 1, 0.0f, y - 1);
	/*}*/

	return 6;
}

static ZESize CreateVertical(ZETerrainVertex* Vertices, ZEInt PosY, ZEInt Direction)
{
	ZESize Index = 0;
	ZESize PosX = -EdgeLength;
	for (ZEInt x = 0; x < EdgeLength + 2; x++)
	{
		for (ZEInt y = 0; y < EdgeLength; y++)
		{
			Index += CreateQuad(Vertices + Index, PosX, PosY);
			PosY ++;
		}
		PosX += Direction;
	}
	
	return Index;
}

static ZESize CreateTop(ZETerrainVertex* Vertices)
{
	ZESize Index = 0;
	for (int y = EdgeLength + 1; y <= 2 * EdgeLength + 2; y++)
		for (int x = -EdgeLength; x < EdgeLength; x++)
			Index += CreateQuad(Vertices + Index, x, y);

	return Index;
}

static ZESize CreateBottom(ZETerrainVertex* Vertices)
{
	ZESize Index = 0;
	for (int y = -EdgeLength; y >= -2 * EdgeLength - 1; y--)
		for (int x = -EdgeLength; x < EdgeLength; x++)
			Index += CreateQuad(Vertices + Index, x, y);

	return Index;
}

static ZESize CreateRight(ZETerrainVertex* Vertices)
{
	ZESize Index = 0;
	for (int x = EdgeLength; x <= 2 * EdgeLength + 1; x++)
		for (int y = -EdgeLength + 1; y <= EdgeLength; y++)
			Index += CreateQuad(Vertices + Index, x, y);

	return Index;
}

static ZESize CreateLeft(ZETerrainVertex* Vertices)
{
	ZESize Index = 0;
	for (int x = -EdgeLength - 1; x >= -2 * EdgeLength - 2; x--)
		for (int y = -EdgeLength + 1; y <= EdgeLength; y++)
			Index += CreateQuad(Vertices + Index, x, y);

	return Index;
}

static ZESize CreateCorner(ZETerrainVertex* Vertices, ZEInt PosX,ZEInt PosY, ZEInt DirectionX, ZEInt DirectionY)
{
	ZESize Index = 0;
	ZEInt TempPosY = PosY;
	for (ZEInt y = 0; y < EdgeLength; y++)
	{
		ZEInt TempPosX = PosX;
		for (ZEInt x = 0; x < EdgeLength; x++)
		{
			Index += CreateQuad(Vertices + Index, TempPosX, TempPosY);
			TempPosX += DirectionX;
		}
		TempPosY += DirectionY;
	}

	TempPosY = PosY;
	for (ZEInt y = 0; y < EdgeLength + 2; y++)
	{
		ZEInt TempPosX = PosX;
		for (ZEInt x = 0; x < EdgeLength + 2; x++)
		{
			if (x >= EdgeLength || y >= EdgeLength)
				Index += CreateQuad(Vertices + Index, TempPosX, TempPosY);

			TempPosX += DirectionX;
		}
		TempPosY += DirectionY;
	}

	return Index;
}

bool ZETerrainPrimitivesGenerator::Generate(ZEStaticVertexBuffer** VertexBuffer, ZETerrainPrimitiveIndices* Indices, ZEInt EdgeLength)
{
	::EdgeLength = EdgeLength;
	
	ZESize VertexBufferSize = 
		4 * 4 * EdgeLength * (EdgeLength + 2) +
		8 * 2 * (EdgeLength + 2) * (EdgeLength + 2);
	VertexBufferSize *= 3 * sizeof(ZETerrainVertex);

	if (*VertexBuffer == 0)
		*VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (!(*VertexBuffer)->Create(VertexBufferSize))
		return false;

	ZETerrainVertex* Vertices = (ZETerrainVertex*)(*VertexBuffer)->Lock();
	if (Vertices == NULL)
		return false;

	// Primitives
	ZEInt HalfLength = EdgeLength / 2;
	ZEInt Negative = -2 * EdgeLength;
	ZEInt Positivie = -2 * EdgeLength;

	ZESize VertexIndex = 0;
	Indices->Size[ZE_TP_LEFT] =	4 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_LEFT] = 4 * EdgeLength * (EdgeLength + 2);
	Indices->Index[ZE_TP_LEFT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_LEFT] = Indices->Index[ZE_TP_LEFT] + 4 * EdgeLength;
	VertexIndex += CreateLeft(Vertices + VertexIndex);

	Indices->Size[ZE_TP_RIGHT] = 4 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_RIGHT] = 4 * EdgeLength * (EdgeLength + 2);
	Indices->Index[ZE_TP_RIGHT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_RIGHT] = Indices->Index[ZE_TP_RIGHT] + 4 * EdgeLength;
	VertexIndex += CreateRight(Vertices + VertexIndex);

	Indices->Size[ZE_TP_TOP] = 4 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_TOP] = 4 * EdgeLength * (EdgeLength + 2);
	Indices->Index[ZE_TP_TOP] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_TOP] = Indices->Index[ZE_TP_TOP] + 4 * EdgeLength;
	VertexIndex += CreateTop(Vertices + VertexIndex);

	Indices->Size[ZE_TP_BOTTOM] = 4 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_BOTTOM] = 4 * EdgeLength * (EdgeLength + 2);
	Indices->Index[ZE_TP_BOTTOM] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_BOTTOM] = Indices->Index[ZE_TP_BOTTOM] + 4 * EdgeLength;
	VertexIndex += CreateBottom(Vertices + VertexIndex);

	Indices->Size[ZE_TP_TOP_LEFT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_TOP_LEFT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_TOP_LEFT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_TOP_LEFT] = Indices->Index[ZE_TP_TOP_LEFT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, -EdgeLength - 1, EdgeLength + 1, -1, 1);	

	Indices->Size[ZE_TP_TOP_RIGHT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_TOP_RIGHT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_TOP_RIGHT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_TOP_RIGHT] = Indices->Index[ZE_TP_TOP_RIGHT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, EdgeLength, EdgeLength + 1, 1, 1);	

	Indices->Size[ZE_TP_BOTTOM_LEFT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_BOTTOM_LEFT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_BOTTOM_LEFT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_BOTTOM_LEFT] = Indices->Index[ZE_TP_BOTTOM_LEFT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, -EdgeLength - 1, -EdgeLength, -1, -1);	

	Indices->Size[ZE_TP_BOTTOM_RIGHT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_BOTTOM_RIGHT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_BOTTOM_RIGHT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_BOTTOM_RIGHT] = Indices->Index[ZE_TP_BOTTOM_RIGHT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, EdgeLength, -EdgeLength, 1, -1);	

	Indices->Size[ZE_TP_CENTER_TOP_LEFT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_CENTER_TOP_LEFT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_CENTER_TOP_LEFT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_CENTER_TOP_LEFT] = Indices->Index[ZE_TP_CENTER_TOP_LEFT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, 0 - 1, 0 + 1, -1, 1);	

	Indices->Size[ZE_TP_CENTER_TOP_RIGHT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_CENTER_TOP_RIGHT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_CENTER_TOP_RIGHT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_CENTER_TOP_RIGHT] = Indices->Index[ZE_TP_CENTER_TOP_RIGHT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, 0, 0 + 1, 1, 1);	

	Indices->Size[ZE_TP_CENTER_BOTTOM_LEFT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_CENTER_BOTTOM_LEFT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_CENTER_BOTTOM_LEFT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_CENTER_BOTTOM_LEFT] = Indices->Index[ZE_TP_CENTER_BOTTOM_LEFT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, -1, 0, -1, -1);	

	Indices->Size[ZE_TP_CENTER_BOTTOM_RIGHT] = 2 * EdgeLength * EdgeLength;
	Indices->SizeExtended[ZE_TP_CENTER_BOTTOM_RIGHT] = 2 * (EdgeLength + 2) * (EdgeLength + 2);
	Indices->Index[ZE_TP_CENTER_BOTTOM_RIGHT] = VertexIndex / 3;
	Indices->IndexShrinked[ZE_TP_CENTER_BOTTOM_RIGHT] = Indices->Index[ZE_TP_CENTER_BOTTOM_RIGHT] + 2;
	VertexIndex += CreateCorner(Vertices + VertexIndex, 0, 0, 1, -1);

	zeAssert(VertexIndex * sizeof(ZETerrainVertex) != VertexBufferSize, "Vertex counts does not match.");
	(*VertexBuffer)->Unlock();
}
