//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrain.h
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

#pragma once
#ifndef __ZE_TERRAIN_H__
#define __ZE_TERRAIN_H__

#include "ZETypes.h"
#include "ZEGame/ZEEntity.h"
#include "ZETerrainVertexBuffer.h"

class ZETexture2D;
class ZEStaticVertexBuffer;
class ZEMaterial;
class ZETerrainMaterial;
class ZERenderer;
class ZEVertexDeclaration;

struct ZETerrainData
{
	size_t Width;
	size_t Height;
	float* HeightData;
	ZEUINT32* ColorData;

	ZETerrainData()
	{
		Width = 0;
		Height = 0;
		HeightData = NULL;
		ColorData = NULL;
	}
};

struct ZETerrainLOD
{
	ZETexture2D* HeightTexture;
	ZETexture2D* NormalTexture;
	ZETexture2D* ColorTexture;
};

ZE_META_ENTITY_DESCRIPTION(ZETerrain)
class ZETerrain : public ZEEntity
{
	ZE_META_ENTITY(ZETerrain)
	friend class ZEPortalMapDoor;
	private:
		ZETerrainMaterial*						Material;
		ZEStaticVertexBuffer*					VertexBuffer;
		ZEVertexDeclaration*					VertexDeclaration;
		ZETerrainPrimitiveIndices				Indices;
		
		ZEArray<ZETerrainData>					TerrainData;
		ZEArray<ZETerrainLOD>					TerrainLODs;

		float									UnitLength;
		ZEUInt									ChunkSize;
		ZEUInt									MaxLevel;

		float									HeightOffset;
		float									HeightScale;
		
		ZEString								TerrainFileName;
		ZETexture2D*							HeightTexture;
		ZETexture2D*							ColorTexture;
		ZETexture2D*							NormalTexture;

		bool									LoadTerrain();
		void									UnloadTerrain();

		bool									DrawPrimtive(ZERenderer* Renderer, int PrimitiveType, const ZEVector3& Offset, const ZEVector3& Position, float Scale, bool Rotate = false);

												ZETerrain();
												~ZETerrain();

	public:	
		virtual ZEDrawFlags						GetDrawFlags() const;
		virtual ZERayCastFlags					GetRayCastFlags() const;

		void									SetUnitLength(float Length);
		float									GetUnitLength();

		void									SetChunkSize(ZEUInt Size);
		ZEUInt									GetChunkSize();

		void									SetMaxLevel(ZEUInt MaxLevel);
		ZEUInt									GetMaxLevel();

		void									SetHeightOffset(float Offset);
		float									GetHeightOffset();

		void									SetHeightScale(float Scale);
		float									GetHeightScale();
		
		void									Stream();

		virtual bool							Initialize();
		virtual void							Deinitialize();
	
		virtual void							Draw(ZEDrawParameters* DrawParameters);
	
		virtual void							SetTerrainFile(const ZEString& FileName);
		virtual const ZEString&					GetTerrainFile();

		static ZETerrain*						CreateInstance();

};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZETerrain"	parent="ZEEntity"	description="Terrain" />
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif
