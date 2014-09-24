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

class ZETerrainDataLevel : public ZEObject
{
	ZE_OBJECT
	public:
		ZESize		ElevationWidth;
		ZESize		ElevationHeight;
		float*		ElevationData;
		ZESize		ColorWidth;
		ZESize		ColorHeight;
		ZEUInt32*	ColorData;
};

class ZETerrainLevel : public ZEObject
{
	ZE_OBJECT
	public:
		ZETerrainMaterial*	Material;
		ZETexture2D*		ElevationTexture;
		ZETexture2D*		ColorTexture;
		float				MinHeight;
		float				MaxHeight;
};

class ZETerrain : public ZEEntity
{
	friend class ZEInteriorDoor;

	ZE_OBJECT

	public:
		ZEStaticVertexBuffer*					VertexBuffer;
		ZEVertexDeclaration*					VertexDeclaration;
		ZETerrainPrimitiveIndices				Indices;
		
		ZEArray<ZETerrainDataLevel>				DataLevels;
		ZEArray<ZETerrainLevel>					Levels;

		const ZETexture2D*						ColorTexture;
		const ZETexture2D*						DetailNormalTexture;

		float									UnitLength;
		ZEUInt									ChunkSize;
		ZEUInt									MaxLevel;

		float									HeightOffset;
		float									HeightScale;
		
		ZEString								TerrainFileName;

		ZESSize									OldPositionX;
		static ZESSize							OldPositionY;

		static ZEArray<int>						Test;
		ZEArray<int>						TestNonStatic;

		bool									CreateVertexBuffer();
		void									DestroyVertexBuffer();

		bool									CreateLevels();
		void									DestroyLevels();

		bool									LoadLevelData();
		void									UnloadLevelData();

		void									Stream(ZEDrawParameters* DrawParameters, ZEInt PositionX, ZEInt PositionY);
		bool									DrawPrimtive(ZERenderer* Renderer, ZEInt PrimitiveType, ZEInt PositionX, ZEInt PositionY, ZEInt LocalPositionX, ZEInt LocalPositionY, ZEInt Mode, ZESize Level);

		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();
		private:
												ZETerrain();
												~ZETerrain();

	public:	
		bool									Locked;
		bool									Wireframe;
		virtual ZEDrawFlags						GetDrawFlags() const;

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

		float									GetHeight(float X, float Z);
	
		virtual void							Draw(ZEDrawParameters* DrawParameters);
	
		virtual void							SetTerrainFile(const ZEString& FileName);
		virtual const ZEString&					GetTerrainFile();

		static ZETerrain*						CreateInstance();

};
#endif
