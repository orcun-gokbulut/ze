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
	LastCameraPosition = ZEVector3::Zero;
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

	ZEEntity::Deinitialize();
}

bool ZETerrain::LoadTerrain()
{
	if (TerrainFileName.IsEmpty())
		return false;

	struct ZETerrainFileHeader
	{
		ZEUInt32 Width;
		ZEUInt32 Heigth;
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
	TerrainData[0].ColorData = new ZEUInt32[Header.Heigth * Header.Width];

	File.Read(TerrainData[0].HeightData, Header.Width * Header.Heigth * sizeof(float), 1);
	File.Read(TerrainData[0].ColorData, Header.Width * Header.Heigth * sizeof(ZEUInt32), 1);

	File.Close();

	ZETerrainLOD* CurrentLOD = TerrainLODs.Add();
	CurrentLOD->HeightTexture = ZETexture2D::CreateInstance();
	CurrentLOD->HeightTexture->Create(ChunkSize * 4, ChunkSize * 4, ZE_TPF_F32, false, 1);
	CurrentLOD->ColorTexture = ZETexture2D::CreateInstance();
	CurrentLOD->ColorTexture->Create(ChunkSize * 4, ChunkSize * 4, ZE_TPF_I8_4, false, 1);
	
	CurrentLOD->Material = ZETerrainMaterial::CreateInstance();
	CurrentLOD->Material->SetHeightTexture(CurrentLOD->HeightTexture);
	CurrentLOD->Material->SetColorTexture(CurrentLOD->ColorTexture);

	for (ZESize I = 1; I < 8; I++)
	{
		ZETerrainData* CurrentLevel = TerrainData.Add();
		ZETerrainData* PrevLevel = &TerrainData[I - 1];

		CurrentLevel->Width = PrevLevel->Width / 2;
		CurrentLevel->Height = PrevLevel->Height / 2;
		CurrentLevel->HeightData = new float[CurrentLevel->Width * CurrentLevel->Height];
		CurrentLevel->ColorData = new ZEUInt32[CurrentLevel->Width * CurrentLevel->Height];

		/*DownsampleHeight(PrevLevel->HeightData, CurrentLevel->HeightData, CurrentLevel->Width, CurrentLevel->Height);
		DownsampleColor(PrevLevel->ColorData, CurrentLevel->ColorData, CurrentLevel->Width, CurrentLevel->Height);*/


		ZETerrainLOD* CurrentLOD = TerrainLODs.Add();
		CurrentLOD->HeightTexture = ZETexture2D::CreateInstance();
		CurrentLOD->HeightTexture->Create(ChunkSize * 4, ChunkSize * 4, ZE_TPF_F32, false, 1);
		CurrentLOD->ColorTexture = ZETexture2D::CreateInstance();
		CurrentLOD->ColorTexture->Create(ChunkSize * 4, ChunkSize * 4, ZE_TPF_I8_4, false, 1);

		CurrentLOD->Material = ZETerrainMaterial::CreateInstance();
		CurrentLOD->Material->SetHeightTexture(CurrentLOD->HeightTexture);
		CurrentLOD->Material->SetColorTexture(CurrentLOD->ColorTexture);
	}

	return true;
}

void ZETerrain::UnloadTerrain()
{
	for (ZESize I = 0; I < 8; I++)
	{
		TerrainLODs[I].ColorTexture->Destroy();
		TerrainLODs[I].HeightTexture->Destroy();
		TerrainLODs[I].Material->Destroy();

		delete[] TerrainData[I].HeightData;
		delete[] TerrainData[I].ColorData;
	}
	TerrainData.Clear();
	TerrainLODs.Clear();
}

void ZETerrain::Stream(ZEDrawParameters* DrawParameters)
{

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

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, ZEInt PrimitiveType, const ZEVector3& Offset, const ZEVector3& Position, float Scale, bool Rotate, ZESize LOD)
{	
/*	float TextureWidth = HeightTexture->GetWidth() * 0.5f * UnitLength;
	if (Position.x - ChunkSize * 0.5f * Scale > TextureWidth ||
		Position.x + ChunkSize * 0.5f * Scale < -TextureWidth)
		return false;

	float TextureHeight = HeightTexture->GetHeight() * 0.5f * UnitLength;
	if (Position.z - ChunkSize * 0.5f * Scale > TextureHeight ||
		Position.z + ChunkSize * 0.5f * Scale < -TextureHeight)
		return false;*/


	ZERenderCommand RenderCommand;
	RenderCommand.SetZero();
	RenderCommand.Flags = ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration = VertexDeclaration;
	RenderCommand.Material = TerrainLODs[LOD].Material;
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
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	// Streaming
	if(ZEMath::Abs(CameraPosition.x - LastCameraPosition.x) > ChunkSize * UnitLength || ZEMath::Abs(CameraPosition.z - LastCameraPosition.z) > ChunkSize * UnitLength)
	{
		LastCameraPosition = CameraPosition;
		CameraPosition /= UnitLength;

		for (ZESize I = 0; I < TerrainLODs.GetCount(); I++)
		{
			ZETerrainData* CurrentLevel = TerrainData.Add();
			ZETerrainLOD* CurrentLOD = TerrainLODs.Add();

			ZESize StartX = CameraPosition.x / (1 << I);
			ZESize StartY = CameraPosition.z / (1 << I);
			ZESize Width = 4 * ChunkSize * (1 << I);

			ZEUInt32* Buffer;
			ZESize Pitch;
			CurrentLOD->ColorTexture->Lock((void**)&Buffer, &Pitch);
			for (ZESize y = 0; y < CurrentLevel->Height; y++)
				memcpy(Buffer, CurrentLevel->ColorData + StartX, Width * sizeof(ZEUInt32));
			CurrentLOD->ColorTexture->Unlock(0);

			CurrentLOD->Material->SetHeightOffset(HeightOffset);
			CurrentLOD->Material->SetHeightScale(HeightScale);
			CurrentLOD->Material->SetTextureScale(ZEVector2(UnitLength, UnitLength));
			CurrentLOD->Material->SetTextureOffset(ZEVector2(-CameraPosition.x, CameraPosition.y) / UnitLength);
		}
	}

	// Calculate quadrant
	ZEVector3 QuadPosition = ZEVector3::Zero;//CameraPosition;
	QuadPosition.x = UnitLength * ChunkSize * (ZEInt)(CameraPosition.x / ((float)ChunkSize * UnitLength) + 0.5f);
	QuadPosition.y = 0.0f;
	QuadPosition.z = UnitLength * ChunkSize * (ZEInt)(CameraPosition.z / ((float)ChunkSize * UnitLength) + 0.5f);


	for (ZESize I = 0; I < 40; I++)
	{
		bool Drawed = false;
		float Scale = (float)(1 << I) * UnitLength;

		if (I == 0)
		{
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-0.5f, 0.0f,  0.5f) * ChunkSize * Scale, Scale, false, I);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3( 0.5f, 0.0f,  0.5f) * ChunkSize * Scale, Scale, false, I);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-0.5f, 0.0f, -0.5f) * ChunkSize * Scale, Scale, false, I);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3( 0.5f, 0.0f, -0.5f) * ChunkSize * Scale, Scale, false, I);
		}
	
		// Top	
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3(-1.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-0.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3( 0.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3( 1.5f,	0.0f,	1.5f) * ChunkSize * Scale, Scale, true, I);

		// Left	
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-1.5f,	0.0f,	-0.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-1.5f,	0.0f,	 0.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3( 1.5f,	0.0f,	-0.5f) * ChunkSize * Scale, Scale, true, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3( 1.5f,	0.0f,	 0.5f) * ChunkSize * Scale, Scale, true, I);

		// Bottom
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3(-1.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, false, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-0.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3( 0.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true, I);
		Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3( 1.5f,	0.0f,	-1.5f) * ChunkSize * Scale, Scale, true, I);
	
		if (!Drawed)
			break;
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
