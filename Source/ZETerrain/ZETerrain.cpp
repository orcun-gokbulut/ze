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
		zeError("ZETerrain", "Can not create vertex elements.");
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

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, int PrimitiveType, const ZEVector3& WorldPosition, const ZEVector3& LocalPosition, float Scale, bool Rotate)
{	
	/*float TextureWidth = HeightTexture->GetWidth() * 0.5f - 0.1f;
	if (WorldPosition.x + (LocalPosition.x - ChunkSize * 0.5f) * Scale >= TextureWidth || 
		WorldPosition.x + (LocalPosition.x + ChunkSize * 0.5f) * Scale <= -TextureWidth)
		return false;

	float TextureHeight = HeightTexture->GetHeight() * 0.5f - 0.5f / HeightTexture->GetHeight() - 0.1f;
	if (WorldPosition.y + (LocalPosition.y - ChunkSize * 0.5f) * Scale >= TextureHeight || 
		WorldPosition.y + (LocalPosition.y + ChunkSize * 0.5f) * Scale <= -TextureHeight)
		return false;*/

	ZERenderOrder RenderOrder;
	RenderOrder.SetZero();
	RenderOrder.Flags = ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderOrder.VertexDeclaration = VertexDeclaration;
	RenderOrder.Material = Material;
	RenderOrder.Order = 0;
	RenderOrder.Pipeline = ZE_RORP_3D;
	RenderOrder.VertexBuffer = VertexBuffer;
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.Priority = 3;

	ZEMatrix4x4 OrientationMatrix, ScaleMatrix, TranslationMatrix, TempMatrix;
	ZEMatrix4x4::CreateOrientation(OrientationMatrix, 
		LocalPosition + ZEVector3(ChunkSize * 0.5f, 0.0f, ChunkSize * -0.5f),
		(Rotate ? ZEQuaternion(ZE_PI, ZEVector3::UnitY) : ZEQuaternion::Identity));
	ZEMatrix4x4::CreateScale(ScaleMatrix, ZEVector3(Scale, Scale, Scale));
	ZEMatrix4x4::Multiply(TempMatrix, ScaleMatrix, OrientationMatrix);
	ZEMatrix4x4::CreateTranslation(TranslationMatrix, WorldPosition);
	ZEMatrix4x4::Multiply(RenderOrder.WorldMatrix, TranslationMatrix, TempMatrix);


	switch(PrimitiveType)
	{
		case 0:
			RenderOrder.VertexBufferOffset = Indices.CenterQuadIndex;
			RenderOrder.PrimitiveCount = Indices.CenterQuadSize / 3;
			break;

		case 1:
			RenderOrder.VertexBufferOffset = Indices.LeftTopCornerIndex;
			RenderOrder.PrimitiveCount = Indices.LeftTopCornerSize / 3;
			break;

		case 2:
			RenderOrder.VertexBufferOffset = Indices.LeftBottomCornerIndex;
			RenderOrder.PrimitiveCount = Indices.LeftBottomCornerSize / 3;
			break;

		case 3:
			RenderOrder.VertexBufferOffset = Indices.LeftEdgeIndex;
			RenderOrder.PrimitiveCount = Indices.LeftEdgeSize / 3;
			break;

		case 4:
			RenderOrder.VertexBufferOffset = Indices.TopEdgeIndex;
			RenderOrder.PrimitiveCount = Indices.TopEdgeSize / 3;
			break;
	}

	Renderer->AddToRenderList(&RenderOrder);

	return true;
}

bool ZETerrain::LoadTerrain()
{
	if (HeightTexture != NULL)
	{
		HeightTexture->Destroy();
		HeightTexture = NULL;
	}

	if (ColorTexture != NULL)
	{
		ColorTexture->Destroy();
		ColorTexture = NULL;
	}

	if (NormalTexture != NULL)
	{
		ColorTexture->Destroy();
		ColorTexture = NULL;
	}


	if (TerrainFileName.IsEmpty())
		return true;

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
		zeError("ZETerrain", "Can not open terrain file.");
		File.Close();
		return false;
	}

	File.Read(&Header, sizeof(ZETerrainFileHeader), 1);

	// HeithtTexture
	HeightTexture = ZETexture2D::CreateInstance();
	Result = HeightTexture->Create(Header.Width, Header.Heigth, ZE_TPF_LUM_HDR, false, 1);
	if (!Result)
	{
		zeError("ZETerrain", "Can not create height texture.");
		File.Close();
		return false;
	}

	void* Buffer;
	unsigned int Pitch;
	HeightTexture->Lock(&Buffer, &Pitch, 0);
	if (Buffer == NULL)
	{
		zeError("ZETerrain", "Can not lock height texture.");
		File.Close();
		return false;
	}

	for (int y = 0; y < Header.Heigth; y++)
		File.Read((float*)((char*)Buffer + y * Pitch), sizeof(float) * Header.Width, 1);

	HeightTexture->Unlock(0);

	// ColorTexture
	ColorTexture = ZETexture2D::CreateInstance();
	Result = ColorTexture->Create(Header.Width, Header.Heigth, ZE_TPF_A8R8G8B8, false, 1);
	if (!Result)
	{
		zeError("ZETerrain", "Can not create color texture.");
		File.Close();
		return false;
	}

	ColorTexture->Lock(&Buffer, &Pitch, 0);
	if (Buffer == NULL)
	{
		zeError("ZETerrain", "Can not lock height texture.");
		File.Close();
		return false;
	}

	for (int y = 0; y < Header.Heigth; y++)
		File.Read((ZEUINT32*)((char*)Buffer + y * Pitch), sizeof(ZEUINT32) * Header.Width, 1);

	ColorTexture->Unlock(0);

	/*// ColorTexture
	NormalTexture = ZETexture2D::CreateInstance();
	Result = NormalTexture->Create(Header.Width, Header.Heigth, ZE_TPF_A8R8G8B8, false, 1);
	if (!Result)
	{
		zeError("ZETerrain", "Can not create color texture.");
		File.Close();
		return false;
	}

	NormalTexture->Lock(&Buffer, &Pitch, 0);
	if (Buffer == NULL)
	{
		zeError("ZETerrain", "Can not lock height texture.");
		File.Close();
		return false;
	}

	for (int y = 0; y < Header.Heigth; y++)
		File.Read((ZEUINT32*)((char*)Buffer + y * Pitch), sizeof(ZEUINT32) * Header.Width, 1);

	NormalTexture->Unlock(0);*/

	File.Close();
	return true;
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

void ZETerrain::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	ZECamera* Camera = DrawParameters->View->Camera;

	Material->SetHeightTexture(HeightTexture);
	Material->SetHeightOffset(HeightOffset);
	Material->SetHeightScale(HeightScale);
	Material->SetColorTexture(ColorTexture);
	Material->SetNormalTexture(NormalTexture);

	float QuadSize = ChunkSize;

	// Calculate quadrant
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	ZEVector3 QuadPosition = CameraPosition;
	QuadPosition.x = UnitLength * ChunkSize * (int)(CameraPosition.x / ((float)ChunkSize * UnitLength) + 0.5f);
	QuadPosition.y = 0.0f;
	QuadPosition.z = UnitLength * ChunkSize * (int)(CameraPosition.z / ((float)ChunkSize * UnitLength) + 0.5f);

	int MinLevel = 0;
	for (size_t I = 0; I < MaxLevel; I++)
	{
		bool Drawed = false;
		if (I == 0)
		{
			float Scale = (float)(1 << MinLevel) * UnitLength;

			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-QuadSize, 0.0f, QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(0.0f , 0.0f, QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3(-QuadSize, 0.0f, 0.0f), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 0, QuadPosition, ZEVector3::Zero, UnitLength);
		}
		else
		{
			float Scale = (float)(1 << (I + MinLevel - 1)) * UnitLength;
			
			// Top	
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3(-2.0f * QuadSize,	0.0f,	2.0f * QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-1.0f * QuadSize,	0.0f,	2.0f * QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(0.0f,				0.0f,	2.0f * QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3(QuadSize,			0.0f,	2.0f * QuadSize), Scale, true);

			// Left				// Right
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-2.0f * QuadSize,	0.0f,	QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(-2.0f * QuadSize,	0.0f,	0.0f), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(QuadSize,			0.0f,	QuadSize), Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 3, QuadPosition, ZEVector3(QuadSize,			0.0f,	0.0f), Scale, true);

			// Bottom
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 2, QuadPosition, ZEVector3(-2.0f * QuadSize,	0.0f,	-QuadSize), Scale);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(-1.0f * QuadSize,	0.0f,	-QuadSize), Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 4, QuadPosition, ZEVector3(0.0f,				0.0f,	-QuadSize), Scale, true);
			Drawed |= DrawPrimtive(DrawParameters->Renderer, 1, QuadPosition, ZEVector3(QuadSize,			0.0f,	-QuadSize), Scale, true);
		
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

#include "ZETerrain.h.zpp"
