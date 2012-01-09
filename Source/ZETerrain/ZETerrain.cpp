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

#include "ZETerrain.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZETerrainMaterial.h"
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZEGraphics/ZEBitmap.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEFile/ZEFile.h"
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
	Material = NULL;
	VertexDeclaration = NULL;
	VertexBuffer = NULL;
	HeightTexture = NULL;
	HeightScale = 10.0f;
	HeightOffset = 0.0f;
	ColorTexture = NULL;
	NormalTexture = NULL;
}

ZETerrain::~ZETerrain()
{
	Deinitialize();
}

bool ZETerrain::Initialize()
{
	if (GetInitialized())
		return false;

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

	Material = ZETerrainMaterial::CreateInstance();
	Material->SetZero();

	LoadTerrain();

	return ZEEntity::Initialize();
}

void ZETerrain::Deinitialize()
{
	if (!GetInitialized())
		return;

	VertexDeclaration->Destroy();
	VertexDeclaration = NULL;

	VertexBuffer->Destroy();
	VertexBuffer = NULL;

	Material->Destroy();
	Material = NULL;

	HeightTexture->Destroy();
	HeightTexture = NULL;

	ColorTexture->Destroy();
	ColorTexture = NULL;

	NormalTexture->Destroy();
	NormalTexture = NULL;

	ZEEntity::Deinitialize();
}

inline void* Address(void* Buffer, size_t x, size_t y, size_t Pitch)
{
	return (void*)((char*)Buffer + Pitch * y + x * sizeof(float));
}

inline void* SampleAddress(void* Buffer, size_t x, size_t y, size_t Width, size_t Height)
{
	if (x < 0)
		x = 0;
	else if (x >= Width)
		x = Width - 1;

	if (y < 0)
		y = 0;
	else if (y >= Height)
		y = Height - 1;

	return Address(Buffer, Width, x, y);
}

static void DownsampleHeight(float* Source, float* Destination, size_t Width, size_t Height)
{
	for(size_t y = 0; y < Height / 2; y++)
		for(size_t x = 0; x < Width / 2; x++)
		{
			size_t Sx = x * 2;
			size_t Sy = y * 2;
			float Sum = 
				*(float*)SampleAddress(Source, Sx - 1,	Sy - 1, Width, Height) +
				*(float*)SampleAddress(Source, Sx,		Sy - 1, Width, Height) +
				*(float*)SampleAddress(Source, Sx + 1,	Sy - 1, Width, Height) +
				*(float*)SampleAddress(Source, Sx - 1,	Sy,		Width, Height) +  
				*(float*)SampleAddress(Source, Sx,		Sy,		Width, Height) +  
				*(float*)SampleAddress(Source, Sx + 1,	Sy,		Width, Height) +  
				*(float*)SampleAddress(Source, Sx - 1,	Sy + 1, Width, Height) +
				*(float*)SampleAddress(Source, Sx,		Sy + 1, Width, Height) +
				*(float*)SampleAddress(Source, Sx + 1,	Sy + 1, Width, Height);

			Sum /= 9;
			*(float*)Address(Destination, x, y, Width) = Sum;
		}
}

static void DownsampleColor(ZEUINT32* Source, ZEUINT32* Destination, size_t Width, size_t Height)
{
	for(size_t y = 0; y < Height; y++)
		for(size_t x = 0; x < Width; x++)
		{
			size_t Sx = x * 2;
			size_t Sy = y * 2;
			ZEUINT32 SumA = 0;
			ZEUINT32 SumR = 0;
			ZEUINT32 SumG = 0;
			ZEUINT32 SumB = 0;

			ZEUINT32 Sample;
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx - 1,	Sy - 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx,		Sy - 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);

			Sample = *(ZEUINT32*)SampleAddress(Source, Sx + 1,	Sy - 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx - 1,	Sy,		Width, Height);  
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx,		Sy,		Width, Height);  
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx + 1,	Sy,		Width, Height);  
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx - 1,	Sy + 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx,		Sy + 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);
			
			Sample = *(ZEUINT32*)SampleAddress(Source, Sx + 1,	Sy + 1, Width, Height);
			SumA += (Sample & 0xFF000000) >> 24;
			SumR += (Sample & 0x00FF0000) >> 16;
			SumG += (Sample & 0x0000FF00) >> 8;
			SumB += (Sample & 0x000000FF);

			SumA /= 9;
			SumR /= 9;
			SumG /= 9;
			SumB /= 9;

			Sample = (SumA << 24) & 0xFF000000 + (SumR << 16) & 0x0000FF00 + (SumG << 8) & 0x00FF0000 + SumB & 0x000000FF;

			*(ZEUINT32*)Address(Destination, x, y, Width) = Sample;
		}
}

bool ZETerrain::LoadTerrain()
{
	if (TerrainFileName.IsEmpty())
		return false;

	struct ZETerrainFileHeader
	{
		ZEDWORD Width;
		ZEDWORD Heigth;
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

	File.Read(&Header, sizeof(ZETerrainFileHeader), 1);

	TerrainData.Add();
	TerrainData[0].Height = Header.Heigth;
	TerrainData[0].Width = Header.Width;
	TerrainData[0].HeightData = new float[Header.Heigth * Header.Width];
	TerrainData[0].ColorData = new ZEUINT32[Header.Heigth * Header.Width];

	File.Read(TerrainData[0].HeightData, Header.Width * Header.Heigth * sizeof(float), 1);
	File.Read(TerrainData[0].ColorData, Header.Width * Header.Heigth * sizeof(ZEUINT32), 1);

	File.Close();

	ZETerrainLOD* CurrentLOD = TerrainLODs.Add();
	CurrentLOD->HeightTexture = ZETexture2D::CreateInstance();
	CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_LUM_HDR, false, 1);
	CurrentLOD->ColorTexture = ZETexture2D::CreateInstance();
	CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_LUM_HDR, false, 1);
	CurrentLOD->NormalTexture = ZETexture2D::CreateInstance();
	CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_LUM_HDR, false, 1);


	for (size_t I = 1; I < 8; I++)
	{
		ZETerrainData* CurrentLevel = TerrainData.Add();
		ZETerrainData* PrevLevel = &TerrainData[I - 1];

		CurrentLevel->Width = PrevLevel->Width / 2;
		CurrentLevel->Height = PrevLevel->Width / 2;
		CurrentLevel->HeightData = new float[CurrentLevel->Width * CurrentLevel->Height * sizeof(float)];
		CurrentLevel->ColorData = new ZEUINT32[CurrentLevel->Width * CurrentLevel->Height * sizeof(ZEUINT32)];

		DownsampleHeight(PrevLevel->HeightData, CurrentLevel->HeightData, CurrentLevel->Width, CurrentLevel->Height);
		DownsampleColor(PrevLevel->ColorData, CurrentLevel->ColorData, CurrentLevel->Width, CurrentLevel->Height);


		ZETerrainLOD* CurrentLOD = TerrainLODs.Add();
		CurrentLOD->HeightTexture = ZETexture2D::CreateInstance();
		CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_A8R8G8B8, false, 1);
		CurrentLOD->ColorTexture = ZETexture2D::CreateInstance();
		CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_A8R8G8B8, false, 1);
		CurrentLOD->NormalTexture = ZETexture2D::CreateInstance();
		CurrentLOD->HeightTexture->Create(ChunkSize * 3, ChunkSize * 3, ZE_TPF_A8R8G8B8, false, 1);
	}

	return true;
}

void ZETerrain::UnloadTerrain()
{
	for (size_t I = 0; I < 8; I++)
	{
		delete[] TerrainData[I].HeightData;
		delete[] TerrainData[I].ColorData;
	}
	TerrainData.Clear();
}

void ZETerrain::Stream()
{
	ZEVector3 OldLocalPosition;
	ZEVector3 LocalPosition;

	if(ZEMath::Abs(LocalPosition.x - OldLocalPosition.x) < 1.0f || ZEMath::Abs(LocalPosition.z - OldLocalPosition.z) < 1.0f)
		return;

	for (size_t I = 0; I < TerrainLODs.GetCount(); I++)
	{
		
	}
	

	// Calculate Position -> UV 
	// Copy from that UV
	// Check TWO

	// Full Update
	
	// Semi Update
		// Vertical Update
			// Non-Divided
			// Divided

		// Horizontal Update
			// Non-Divided
			// Divided

}

void ZETerrain::SetTerrainFile(const ZEString& FileName)
{
	TerrainFileName = FileName;
	if (GetInitialized())
		LoadTerrain();
}

const ZEString& ZETerrain::GetTerrainFile()
{
	return TerrainFileName;
}

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, int PrimitiveType, const ZEVector3& Offset, const ZEVector3& Position, float Scale, bool Rotate)
{	
	float TextureWidth = HeightTexture->GetWidth() * 0.5f * UnitLength;
	if (Position.x - ChunkSize * 0.5f * Scale > TextureWidth ||
		Position.x + ChunkSize * 0.5f * Scale < -TextureWidth)
		return false;

	float TextureHeight = HeightTexture->GetHeight() * 0.5f * UnitLength;
	if (Position.z - ChunkSize * 0.5f * Scale > TextureHeight ||
		Position.z + ChunkSize * 0.5f * Scale < -TextureHeight)
		return false;


	ZERenderCommand RenderCommand;
	RenderCommand.SetZero();
	RenderCommand.Flags = ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration = VertexDeclaration;
	RenderCommand.Material = Material;
	RenderCommand.Order = 0;
	RenderCommand.Pipeline = ZE_RORP_3D;
	RenderCommand.VertexBuffer = VertexBuffer;
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderCommand.Priority = 3;
	ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, 
		Position + Offset,
		(Rotate ? ZEQuaternion(ZE_PI, ZEVector3::UnitY) : ZEQuaternion::Identity),
		ZEVector3(Scale, Scale, Scale));

	switch(PrimitiveType)
	{
		case 0:
			RenderCommand.VertexBufferOffset = Indices.CenterQuadIndex;
			RenderCommand.PrimitiveCount = Indices.CenterQuadSize / 3;
			break;

		case 1:
			RenderCommand.VertexBufferOffset = Indices.LeftTopCornerIndex;
			RenderCommand.PrimitiveCount = Indices.LeftTopCornerSize / 3;
			break;

		case 2:
			RenderCommand.VertexBufferOffset = Indices.LeftBottomCornerIndex;
			RenderCommand.PrimitiveCount = Indices.LeftBottomCornerSize / 3;
			break;

		case 3:
			RenderCommand.VertexBufferOffset = Indices.LeftEdgeIndex;
			RenderCommand.PrimitiveCount = Indices.LeftEdgeSize / 3;
			break;

		case 4:
			RenderCommand.VertexBufferOffset = Indices.TopEdgeIndex;
			RenderCommand.PrimitiveCount = Indices.TopEdgeSize / 3;
			break;
	}

	Renderer->AddToRenderList(&RenderCommand);

	return true;
}


void ZETerrain::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	ZECamera* Camera = DrawParameters->View->Camera;

	Material->SetHeightTexture(HeightTexture);
	Material->SetHeightOffset(HeightOffset);
	Material->SetHeightScale(HeightScale);
	Material->SetTextureScale(ZEVector2(UnitLength, UnitLength));
	Material->SetColorTexture(ColorTexture);
	Material->SetNormalTexture(NormalTexture);

	float QuadSize = ChunkSize;

	// Calculate quadrant
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	ZEVector3 QuadPosition = ZEVector3::Zero;//CameraPosition;
	QuadPosition.x = UnitLength * ChunkSize * (int)(CameraPosition.x / ((float)ChunkSize * UnitLength) + 0.5f);
	QuadPosition.y = 0.0f;
	QuadPosition.z = UnitLength * ChunkSize * (int)(CameraPosition.z / ((float)ChunkSize * UnitLength) + 0.5f);


	int MinLevel = 0;
	for (size_t I = 0; I < 40; I++)
	{
		bool Drawed = false;
		if (I == 0)
		{
			float Scale = (float)(1 << MinLevel) * UnitLength;

			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-0.5f, 0.0f,  0.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3( 0.5f, 0.0f,  0.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-0.5f, 0.0f, -0.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3( 0.5f, 0.0f, -0.5f) * ChunkSize * Scale, Scale);
		}
		else
		{
			float Scale = (float)(1 << (I + MinLevel - 1)) * UnitLength;
			
			// Top	
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3(-1.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-0.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3( 0.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3( 1.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale, true);

			// Left	
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-1.5f,	0.0f,	-0.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-1.5f,	0.0f,	 0.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3( 1.5f,	0.0f,	-0.5f) * ChunkSize * Scale, Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3( 1.5f,	0.0f,	 0.5f) * ChunkSize * Scale, Scale, true);

			// Bottom
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3(-1.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-0.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3( 0.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3( 1.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true);
		
			if (!Drawed)
				break;
		}
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
