//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrain.cpp
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

#include <crtdbg.h>

#include "ZETerrain.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZETerrainMaterial.h"
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZEGraphics/ZEBitmap.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEFile/ZEFile.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

ZEDrawFlags ZETerrain::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

ZERayCastFlags ZETerrain::GetRayCastFlags() const
{
	return ZE_RCF_INTERNAL;
}

void ZETerrain::SetChunkSize(ZEUInt Size)
{
	ChunkSize = Size;
	if (GetInitialized())
		ZETerrainPrimitivesGenerator::Generate(&VertexBuffer, &Indices, ChunkSize);
}

ZEUInt ZETerrain::GetChunkSize()
{
	return ChunkSize;
}

void ZETerrain::SetUnitLength(float Length)
{
	UnitLength = Length;
}

float ZETerrain::GetUnitLength()
{
	return UnitLength;
}

void ZETerrain::SetMaxLevel(ZEUInt MaxLevel)
{
	this->MaxLevel = MaxLevel;
}

ZEUInt ZETerrain::GetMaxLevel()
{
	return MaxLevel;
}

void ZETerrain::SetHeightOffset(float Offset)
{
	HeightOffset = Offset;
}

float ZETerrain::GetHeightOffset()
{
	return HeightOffset;
}

void ZETerrain::SetHeightScale(float Scale)
{
	HeightScale = Scale;
}

float ZETerrain::GetHeightScale()
{
	return HeightScale;
}

ZETerrain::ZETerrain()
{
	UnitLength = 1.0f;
	ChunkSize = 16;
	MaxLevel = 10;
	VertexDeclaration = NULL;
	VertexBuffer = NULL;
	HeightScale = 10.0f;
	HeightOffset = 0.0f;
	ChunkPositionX = 2147483647;
	ChunkPositionY = 2147483647;
}

ZETerrain::~ZETerrain()
{
	Deinitialize();
}

bool ZETerrain::Initialize()
{
	if (GetInitialized())
		return false;

	if (!CreateVertexBuffer())
		return false;

	if (!LoadLevelData())
		return false;

	if (!CreateLevels())
		return false;

	return ZEEntity::Initialize();
}

void ZETerrain::Deinitialize()
{
	if (!GetInitialized())
		return;

	DestroyLevels();
	UnloadLevelData();
	DestroyVertexBuffer();

	ZEEntity::Deinitialize();
}

bool ZETerrain::CreateVertexBuffer()
{
	ZETerrainPrimitivesGenerator::Generate(&VertexBuffer, &Indices, ChunkSize);

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();
	ZEVertexElement Elements[] = 
	{
		{ZE_VES_POSITION, ZE_VET_FLOAT3, 0}
	};

	if (!VertexDeclaration->Create(Elements, 1))
	{
		zeError("Can not create vertex elements.");
		return false;
	}
}

void ZETerrain::DestroyVertexBuffer()
{
	VertexDeclaration->Destroy();
	VertexDeclaration = NULL;

	VertexBuffer->Destroy();
	VertexBuffer = NULL;
}

bool ZETerrain::CreateLevels()
{
	DestroyLevels();

	Levels.SetCount(MaxLevel);
	for (ZESize I = 0; I < Levels.GetCount(); I++)
	{
		ZETerrainLevel* CurrentLevel = &Levels[I];
		CurrentLevel->HeightTexture = ZETexture2D::CreateInstance();
		if (!CurrentLevel->HeightTexture->Create(ChunkSize * 4 + 1, ChunkSize * 4 + 1, ZE_TPF_F32_2, false, 1))
			return false;

		CurrentLevel->Material = ZETerrainMaterial::CreateInstance();
		CurrentLevel->Material->SetHeightTexture(CurrentLevel->HeightTexture);
		CurrentLevel->Material->SetColorTexture(CurrentLevel->HeightTexture);
		CurrentLevel->Material->SetBlendTreshold(0.75f);
		CurrentLevel->Material->SetChunkSize(ChunkSize);
	}

	return true;
}

void ZETerrain::DestroyLevels()
{	
	for (ZESize I = 0; I < Levels.GetCount(); I++)
	{
		Levels[I].Material->Destroy();
		Levels[I].HeightTexture->Destroy();
	}
	Levels.Clear();
}

bool ZETerrain::LoadLevelData()
{
	if (TerrainFileName.IsEmpty())
		return false;


	struct ZETerrainFileHeader
	{
		ZEUInt32	Width;
		ZEUInt32	Heigth;
	} Header;

	ZEFile File;

	bool Result;
	Result = File.Open(TerrainFileName, ZE_FM_READ_ONLY, true);
	if (!Result)
	{
		zeError("Can not open terrain file.");
		File.Close();
		return false;
	}

	ZEUInt32 Depth;
	File.Read(&Depth, sizeof(ZEUInt32), 1);
	LevelData.SetCount(Depth);
	for (size_t I = 0; I < Depth; I++)
	{
		File.Read(&Header, sizeof(ZETerrainFileHeader), 1);
		LevelData[I].ElevationHeight = Header.Heigth;
		LevelData[I].ElevationWidth = Header.Width;
		LevelData[I].ElevationData = new float[Header.Heigth * Header.Width];
		File.Read(LevelData[I].ElevationData, Header.Width * Header.Heigth * sizeof(float), 1);
	}

	File.Close();
	 
	return true;
}

void ZETerrain::UnloadLevelData()
{
	for (ZESize I = 0; I < 8; I++)
		delete[] LevelData[I].ElevationData;

	LevelData.Clear();
}

#include <FreeImage.h>

#define GetDestinationAddress(buffer, x, y, pitch) ((ZEUInt8*)(buffer) + (pitch) * (y) + (x) * 2 * sizeof(float))
#define GetSourceAddress(buffer, x, y, pitch) ((ZEUInt8*)(buffer) + (pitch) * (y) + (x) * sizeof(float))

float Sample(ZETerrainLevelData* Data, ZESize x, ZESize y)
{
	if (x < 0 || x >= Data->ElevationWidth)
		return 0.0f;

	if (y < 0 || y >= Data->ElevationHeight)
		return 0.0f;
	
	return Data->ElevationData[y * Data->ElevationWidth + x];
}

float* Write(void* Buffer, int Pitch, ZESize x, ZESize y)
{
	return (float*)((ZEUInt8*)Buffer + Pitch * y + x * 2 * sizeof(float));
}

void ZETerrain::Stream(ZEDrawParameters* DrawParameters, const ZEVector3& Position)
{
	// Calculate Positions
	ZEVector3 CameraPosition = DrawParameters->View->Camera->GetWorldPosition();
	CameraPosition.y = 0;

	ZESSize CurrentChunkPositionX = Position.x;
	ZESSize CurrentChunkPositionY = Position.z;

	if (CurrentChunkPositionX == ChunkPositionX && CurrentChunkPositionY == ChunkPositionY)
		return;

	ChunkPositionX = CurrentChunkPositionX;
	ChunkPositionY = CurrentChunkPositionY;

	// Stream Data
	ZESize LevelCount = Levels.GetCount();
	if (LevelData.GetCount() < LevelCount)
		LevelCount = LevelData.GetCount();
	if (MaxLevel < LevelCount)
		LevelCount = MaxLevel;
	
	ZESize TextureSize = (ChunkSize * 4 + 1);
	float TexelSize = 1.0f / TextureSize;
	for (size_t CurrentLevelIndex = 0; CurrentLevelIndex < LevelCount - 1; CurrentLevelIndex++)
	{
		ZETerrainLevelData* CurrentLevelData = &LevelData[CurrentLevelIndex];
		ZETerrainLevelData* NextLevelData = &LevelData[CurrentLevelIndex + 1];
		ZETerrainLevel* CurrenLevel = &Levels[CurrentLevelIndex];
		
		void* Buffer;
		ZESize Pitch;
		CurrenLevel->HeightTexture->Lock(&Buffer, &Pitch);

		ZESSize DataX = (ChunkPositionX >> CurrentLevelIndex) - (ZESSize)ChunkSize * 2 + (ZESSize)CurrentLevelData->ElevationWidth / 2;
		ZESSize DataY = (-ChunkPositionY >> CurrentLevelIndex) - (ZESSize)ChunkSize * 2 + (ZESSize)CurrentLevelData->ElevationHeight / 2;

		ZESSize NextDataX = (ChunkPositionX >> CurrentLevelIndex + 1) - (ZESSize)ChunkSize + (ZESSize)NextLevelData->ElevationWidth / 2;
		ZESSize NextDataY = (-ChunkPositionY >> CurrentLevelIndex + 1) - (ZESSize)ChunkSize + (ZESSize)NextLevelData->ElevationHeight / 2;

		Levels[CurrentLevelIndex].Material->SetHeightOffset(HeightOffset);
		Levels[CurrentLevelIndex].Material->SetHeightScale(HeightScale);
		Levels[CurrentLevelIndex].Material->SetTextureScale(ZEVector2(TexelSize, TexelSize));
		Levels[CurrentLevelIndex].Material->SetTextureOffset(ZEVector2(2.0f * (float)ChunkSize * TexelSize + 0.5f * TexelSize, 2.0f * (float)ChunkSize * TexelSize + 0.5f * TexelSize));

		for (ZESSize BufferY = 0; BufferY < TextureSize; BufferY++)
			for (ZESSize BufferX = 0; BufferX < TextureSize; BufferX++)
			{
				Write(Buffer, Pitch, BufferX, BufferY)[0] = Sample(CurrentLevelData, DataX + BufferX, DataY + BufferY);
				Write(Buffer, Pitch, BufferX, BufferY)[1] = Sample(NextLevelData, NextDataX + BufferX / 2, NextDataY + BufferY / 2);
 
				/*if (DataCoordsY + BufferCoordsY < 0 || DataCoordsY + BufferCoordsY >= (ZESSize)CurrentLevelData->ElevationHeight)
				{
					memset((ZEUInt8*)Buffer + Pitch * BufferCoordsY, 0, sizeof(float) * TextureSize * 2);
					continue;
				}

				ZESize OffsetX = 0;
				if (DataCoordsX < 0)
				{
					OffsetX = -DataCoordsX;
					memset(GetDestinationAddress(Buffer, 0, BufferCoordsY, Pitch), 0 , OffsetX * sizeof(float) * 2);
				}

				ZESize LimitX = 0;
				if (DataCoordsX + (ZESSize)ChunkSize * 4 + 1 >= (ZESSize)CurrentLevelData->ElevationWidth)
				{
					LimitX = DataCoordsX + (ZESSize)TextureSize - (ZESSize)CurrentLevelData->ElevationWidth;
					memset(GetDestinationAddress(Buffer, (ZESSize)CurrentLevelData->ElevationWidth - DataCoordsX, BufferCoordsY, Pitch), 0, LimitX * sizeof(float) * 2);
				}

				// High Freq
				// Low Freq

				float* SourceBuffer = (float*)GetSourceAddress(CurrentLevelData->ElevationData, DataCoordsX + OffsetX, DataCoordsY + BufferCoordsY, CurrentLevelData->ElevationWidth * sizeof(float));
				float* DesitinationBuffer = (float*)GetDestinationAddress(Buffer, OffsetX, BufferCoordsY, Pitch);

				for (ZESize x = 0; x < (ZESSize)TextureSize - LimitX; x++)
					DesitinationBuffer[2 * x] = SourceBuffer[x];


				SourceBuffer = (float*)GetSourceAddress(NextLevelData->ElevationData, NextLevelDataCoordsX + OffsetX / 2, NextLevelDataCoordsY + BufferCoordsY / 2, NextLevelData->ElevationWidth * sizeof(float));
				DesitinationBuffer = (float*)GetDestinationAddress(Buffer, OffsetX, BufferCoordsY, Pitch);

				for (ZESize x = 0; x < (ZESSize)TextureSize - LimitX; x++)
					DesitinationBuffer[2 * x + 1] = SourceBuffer[x];

				/*memcpy(GetDestinationAddress(Buffer, OffsetX, BufferCoordsY, Pitch), 
					GetSourceAddress(CurrentLevelData->ElevationData, DataCoordsX + OffsetX, DataCoordsY + BufferCoordsY, CurrentLevelData->ElevationWidth * sizeof(float)),
					sizeof(float) * ((ZESSize)TextureSize - LimitX));*/
			}

		/*FIBITMAP* Dump = FreeImage_Allocate(CurrentLevelData->ElevationWidth, CurrentLevelData->ElevationHeight, 8);
		for (size_t x = 0; x < CurrentLevelData->ElevationWidth; x++)
			for (size_t y = 0; y < CurrentLevelData->ElevationHeight; y++)
				*(FreeImage_GetBits(Dump) + FreeImage_GetPitch(Dump) * y + x) = *(float*)GetAddress(CurrentLevelData->ElevationData, x, y, CurrentLevelData->ElevationWidth * sizeof(float)) * 256.0f;

		FIBITMAP* Dump = FreeImage_Allocate(ChunkSize * 4 + 1, ChunkSize * 4 + 1, 8);
		for (size_t x = 0; x < ChunkSize * 4 + 1; x++)
			for (size_t y = 0; y < ChunkSize * 4 + 1; y++)
				*(FreeImage_GetBits(Dump) + FreeImage_GetPitch(Dump) * y + x) = *(float*)GetAddress(Buffer, x, y, Pitch) * 256.0f;

		char Filename[256];
		sprintf(Filename, "TerrainDumpLevel%2d.bmp", CurrentLevelIndex);
		FreeImage_Save(FIF_BMP, Dump, Filename);

		zeLog("Lod %d Start Position : %d, %d", CurrentLevelIndex, DataCoordsX, DataCoordsY);*/
		CurrenLevel->HeightTexture->Unlock(0);
	}

}

void ZETerrain::SetTerrainFile(const ZEString& FileName)
{
	TerrainFileName = FileName;
	if (GetInitialized())
		LoadLevelData();
}

const ZEString& ZETerrain::GetTerrainFile()
{
	return TerrainFileName;
}

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, ZEInt PrimitiveType, const ZEVector3& Position, ZESize Level)
{	
	ZERenderCommand RenderCommand;
	RenderCommand.SetZero();
	RenderCommand.Flags = ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration = VertexDeclaration;
	RenderCommand.Material = Levels[Level].Material;
	RenderCommand.Order = 0;
	RenderCommand.Pipeline = ZE_RORP_3D;
	RenderCommand.VertexBuffer = VertexBuffer;
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderCommand.Priority = 3;

	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateOrientation(ScaleMatrix, Position, ZEQuaternion::Identity, ZEVector3((float)(1 << Level), 1.0f, (float)(1 << Level)));
	ZEMatrix4x4::Multiply(RenderCommand.WorldMatrix, GetWorldTransform(), ScaleMatrix);

	switch(PrimitiveType)
	{
		case 0:
			RenderCommand.VertexBufferOffset = Indices.CenterIndex;
			RenderCommand.PrimitiveCount = Indices.CenterSize / 3;
			break;

		case 1:
			RenderCommand.VertexBufferOffset = Indices.RingIndex;
			RenderCommand.PrimitiveCount = Indices.RingSize / 3;
			break;
	}

	Renderer->AddToRenderList(&RenderCommand);

	return true;
}

void ZETerrain::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	if (Levels.GetCount() == 0)
		return;

	ZECamera* Camera = DrawParameters->View->Camera;
	ZEVector3 CameraPosition = Camera->GetWorldPosition();

	ZEVector3 ChunkPosition;
	ChunkPosition.x = (CameraPosition.x);// / 2 * 2;// (ZEInt)ChunkSize * (ZEInt)ChunkSize;
	ChunkPosition.y = 0.0f;
	ChunkPosition.z = (CameraPosition.z);//  / 2 * 2;//(ZEInt)ChunkSize * (ZEInt)ChunkSize;

	Stream(DrawParameters, ChunkPosition);
	
	int ActiveLevel = 0;
	for (ZESize CurrentLevelIndex = 0; CurrentLevelIndex < Levels.GetCount(); CurrentLevelIndex++)
	{
		if (CurrentLevelIndex == ActiveLevel)
			DrawPrimtive(DrawParameters->Renderer, 0, ChunkPosition, CurrentLevelIndex);

		DrawPrimtive(DrawParameters->Renderer, 1, ChunkPosition, CurrentLevelIndex);
	}
}

ZETerrain* ZETerrain::CreateInstance()
{
	return new ZETerrain();
}

ZEEntityRunAt ZETerrainDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
