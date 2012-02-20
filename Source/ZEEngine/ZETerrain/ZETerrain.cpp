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

#define ZE_TPM_NORMAL	0x0
#define ZE_TPM_SHRINKED 0x1
#define ZE_TPM_EXTENDED 0x2

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
	Locked = false;
	UnitLength = 1.0f;
	ChunkSize = 16;
	MaxLevel = 10;
	VertexDeclaration = NULL;
	VertexBuffer = NULL;
	HeightScale = 10.0f;
	HeightOffset = 0.0f;
	OldPositionX = 2147483647;
	OldPositionY = 2147483647;
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
ZEInt Align(ZEInt Value, ZEInt Lize)
{
	return (Value / Lize) * Lize;
}
void ZETerrain::Stream(ZEDrawParameters* DrawParameters, ZEInt PositionX, ZEInt PositionY)
{
	if (PositionX == OldPositionX && PositionY == OldPositionY)
		return;

	OldPositionX = PositionX;
	OldPositionY = PositionY;

	ZESize TextureSize = (ChunkSize * 4 + 1);
	float TexelSize = 1.0f / TextureSize;

	ZESize LevelCount = Levels.GetCount();
	if (LevelData.GetCount() < LevelCount)
		LevelCount = LevelData.GetCount();
	if (MaxLevel < LevelCount)
		LevelCount = MaxLevel;

	for (size_t CurrLevelIndex = 0; CurrLevelIndex < LevelCount - 1; CurrLevelIndex++)
	{
		ZETerrainLevelData* CurrentLevelData = &LevelData[CurrLevelIndex];
		ZETerrainLevelData* NextLevelData = &LevelData[CurrLevelIndex + 1];
		ZETerrainLevel* CurrLevel = &Levels[CurrLevelIndex];
		
		ZEInt CurrPositionX	= Align(PositionX, 1 << CurrLevelIndex);
		ZEInt CurrPositionY	= -Align(PositionY, 1 << CurrLevelIndex);
		ZESSize DataX = (CurrPositionX >> CurrLevelIndex) - (ZESSize)ChunkSize * 2 + (ZESSize)CurrentLevelData->ElevationWidth / 2;
		ZESSize DataY = (CurrPositionY >> CurrLevelIndex) - (ZESSize)ChunkSize * 2 + (ZESSize)CurrentLevelData->ElevationHeight / 2;

		ZEInt NextPositionX = Align(PositionX, 1 << (CurrLevelIndex + 1));
		ZEInt NextPositionY = -Align(PositionY, 1 << (CurrLevelIndex + 1));
		ZESSize NextDataX = (NextPositionX >> (CurrLevelIndex + 1)) - (ZESSize)ChunkSize + (ZESSize)NextLevelData->ElevationWidth / 2;
		ZESSize NextDataY = (NextPositionY >> (CurrLevelIndex + 1)) - (ZESSize)ChunkSize + (ZESSize)NextLevelData->ElevationHeight / 2;

		NextDataX = DataX / 2;
		NextDataY = DataY / 2;

		CurrLevel->Material->SetHeightOffset(HeightOffset);
		CurrLevel->Material->SetHeightScale(HeightScale);
		CurrLevel->Material->SetTextureScale(ZEVector2(TexelSize, TexelSize));
		CurrLevel->Material->SetTextureOffset(ZEVector2(2.0f * (float)ChunkSize * TexelSize + 0.5f * TexelSize, 2.0f * (float)ChunkSize * TexelSize + 0.5f * TexelSize));

		void* Buffer;
		ZESize Pitch;		
		CurrLevel->HeightTexture->Lock(&Buffer, &Pitch);
		for (ZESSize BufferY = 0; BufferY < TextureSize; BufferY++)
			for (ZESSize BufferX = 0; BufferX < TextureSize; BufferX++)
			{
				ZESSize NextBufferX = BufferX / 2;
				ZESSize NextBufferY = BufferY / 2;

				// Linear Filtering
				float Value;
				if (((DataX + BufferX) % 2) != 0 && ((DataY + BufferY) % 2) != 0)
				{
					Value  = Sample(NextLevelData,	NextDataX + NextBufferX,		NextDataY + NextBufferY);
					Value += Sample(NextLevelData,	NextDataX + NextBufferX,		NextDataY + NextBufferY + 1);
					Value += Sample(NextLevelData,	NextDataX + NextBufferX + 1,	NextDataY + NextBufferY);
					Value += Sample(NextLevelData,	NextDataX + NextBufferX + 1,	NextDataY + NextBufferY + 1);
					Value /= 4;
					}
				else if (((DataX + BufferX) % 2) != 0)
				{
					Value  = Sample(NextLevelData,	NextDataX + NextBufferX,		NextDataY + NextBufferY + DataY % 2);
					Value += Sample(NextLevelData,	NextDataX + NextBufferX + 1,	NextDataY + NextBufferY + DataY % 2);
					Value /= 2;
				}
				else if (((DataY + BufferY) % 2) != 0)
				{
					Value  = Sample(NextLevelData,	NextDataX + NextBufferX + DataX % 2,		NextDataY + NextBufferY);
					Value += Sample(NextLevelData,	NextDataX + NextBufferX + DataX % 2,		NextDataY + NextBufferY + 1);
					Value /= 2;
				}
				else
				{
					Value = Sample(NextLevelData,	NextDataX + NextBufferX + DataX % 2,	NextDataY + NextBufferY + DataY % 2);
				}

				Write(Buffer, Pitch, BufferX, BufferY)[0] = Sample(CurrentLevelData, DataX + BufferX, DataY + BufferY);
				Write(Buffer, Pitch, BufferX, BufferY)[1] = Value;
			}

		CurrLevel->HeightTexture->Unlock(0);
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

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, ZEInt PrimitiveType, ZEInt PositionX, ZEInt PositionY, ZEInt Mode, ZESize Level)
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
	ZEMatrix4x4::CreateOrientation(ScaleMatrix, ZEVector3(PositionX, 0.0f, PositionY), ZEQuaternion::Identity, ZEVector3((float)(1 << Level), 1.0f, (float)(1 << Level)));
	ZEMatrix4x4::Multiply(RenderCommand.WorldMatrix, GetWorldTransform(), ScaleMatrix);

	RenderCommand.VertexBufferOffset = (Mode & ZE_TPM_SHRINKED ? Indices.IndexShrinked[PrimitiveType] : Indices.Index[PrimitiveType]) * 3;
	RenderCommand.PrimitiveCount = (Mode & ZE_TPM_EXTENDED ? Indices.SizeExtended[PrimitiveType] : Indices.Size[PrimitiveType]) - (Mode & ZE_TPM_SHRINKED ? Indices.IndexShrinked[PrimitiveType] - Indices.Index[PrimitiveType] : 0);

	Renderer->AddToRenderList(&RenderCommand);

	return true;
}

void ZETerrain::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	if (Levels.GetCount() == 0)
		return;

	static ZEInt PositionX;
	static ZEInt PositionY;
	static ZEInt OffsetPositionX;
	static ZEInt OffsetPositionY;
	ZECamera* Camera = DrawParameters->View->Camera;
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	if (!Locked)
	{
		PositionX = Align(ZEMath::Round(CameraPosition.x - OffsetPositionX), 1);
		PositionY = Align(ZEMath::Round(CameraPosition.z - OffsetPositionY), 1);
	}
	else
	{
		OffsetPositionX = CameraPosition.x - PositionX;
		OffsetPositionY = CameraPosition.z - PositionY;
	}

	Stream(DrawParameters, PositionX, PositionY);
	
	int ActiveLevel = 0;
	for (ZESize CurrIndex = 0; CurrIndex < Levels.GetCount(); CurrIndex++)
	{
		ZEInt PrevLevelPositionX = Align(PositionX, 1 << (CurrIndex - 1));
		ZEInt PrevLevelPositionY = Align(PositionY, 1 << (CurrIndex - 1));
		ZEInt CurrLevelPositionX = Align(PositionX, 1 << CurrIndex);
		ZEInt CurrLevelPositionY = Align(PositionY, 1 << CurrIndex);
		ZEInt NextLevelPositionX = Align(PositionX, 1 << (CurrIndex + 1));
		ZEInt NextLevelPositionY = Align(PositionY, 1 << (CurrIndex + 1));

		ZEInt LeftMode = ZE_TPM_NORMAL;
		ZEInt RightMode = ZE_TPM_NORMAL;
		ZEInt TopMode = ZE_TPM_NORMAL;
		ZEInt BottomMode = ZE_TPM_NORMAL;
		
		/*
		if (CurrentLevelPositionX - NextLevelPositionX > 0)
			RightMode |= ZE_TPM_EXTENDED;
		else if (CurrentLevelPositionX - NextLevelPositionX > 0)
			LeftMode |= ZE_TPM_EXTENDED;

		if (PrevLevelPositionX - CurrentLevelPositionX > 0)
			RightMode |= ZE_TPM_SHRINKED;
		else if (PrevLevelPositionX - CurrentLevelPositionX > 0)
			LeftMode |= ZE_TPM_SHRINKED;

		if (CurrentLevelPositionY - NextLevelPositionY > 0)
			TopMode |= ZE_TPM_EXTENDED;
		else if (CurrentLevelPositionY - NextLevelPositionY > 0)
			BottomMode |= ZE_TPM_EXTENDED;

		if (PrevLevelPositionY - CurrentLevelPositionY > 0)
			TopMode |= ZE_TPM_SHRINKED;
		else if (PrevLevelPositionY - CurrentLevelPositionY > 0)
			BottomMode |= ZE_TPM_SHRINKED;*/

		if (CurrIndex == ActiveLevel)
		{
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CENTER_TOP_LEFT,		CurrLevelPositionX, CurrLevelPositionY, ZE_TPM_NORMAL, CurrIndex);
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CENTER_TOP_RIGHT,		CurrLevelPositionX, CurrLevelPositionY, ZE_TPM_NORMAL, CurrIndex);
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CENTER_BOTTOM_LEFT,		CurrLevelPositionX, CurrLevelPositionY, ZE_TPM_NORMAL, CurrIndex);
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CENTER_BOTTOM_RIGHT,	CurrLevelPositionX, CurrLevelPositionY, ZE_TPM_NORMAL, CurrIndex);
		}
		
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_TOP_LEFT,		CurrLevelPositionX, CurrLevelPositionY, TopMode | LeftMode,		CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_TOP,			CurrLevelPositionX, CurrLevelPositionY, TopMode,				CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_TOP_RIGHT,		CurrLevelPositionX, CurrLevelPositionY, TopMode | RightMode,	CurrIndex);

		DrawPrimtive(DrawParameters->Renderer, ZE_TP_LEFT,			CurrLevelPositionX, CurrLevelPositionY, LeftMode,				CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_RIGHT,			CurrLevelPositionX, CurrLevelPositionY, RightMode,				CurrIndex);

		DrawPrimtive(DrawParameters->Renderer, ZE_TP_BOTTOM_LEFT,	CurrLevelPositionX, CurrLevelPositionY, BottomMode | LeftMode,	CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_BOTTOM,			CurrLevelPositionX, CurrLevelPositionY, BottomMode,				CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_BOTTOM_RIGHT,	CurrLevelPositionX, CurrLevelPositionY, BottomMode | RightMode,	CurrIndex);
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
