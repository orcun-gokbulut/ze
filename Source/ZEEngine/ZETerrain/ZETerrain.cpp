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
#include "ZETexture/ZEBitmap.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEFile/ZEFile.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

#define ZE_TPM_NORMAL			0
#define ZE_TPM_SHRINK_NEGATIVE	1
#define ZE_TPM_SHRINK_POSITIVE	2
#define ZE_TPM_EXTEND_NEGATIVE	3
#define ZE_TPM_EXTEND_POSITIVE	4

ZE_OBJECT_IMPL(ZETerrain)

ZEDrawFlags ZETerrain::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
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

float ZETerrain::GetHeight(float X, float Z)
{
	Z = -Z / 2.0f;
	X = X / 2.0f;
	float Height1, Height2, Height3, Height4;
	ZESize DataHeight = DataLevels[1].ElevationHeight;
	ZESize DataWidth = DataLevels[1].ElevationWidth;

	if(X > (ZESize)DataWidth || X < 0.0f)
		return 0.0f;

	if(Z > (ZESize)DataHeight || Z < 0.0f)
		return 0.0f;

	float FractX, FractZ;

	FractX = X - (ZESize)X;
	FractZ = Z - (ZESize)Z;

	Height1 = DataLevels[1].ElevationData[(ZESize)(((ZEInt)Z * DataWidth) + (ZEInt)X)];
	Height2 = DataLevels[1].ElevationData[(ZESize)(((ZEInt)Z * DataWidth) + (ZEInt)X) + 1];

	Height1 = ZEMath::Lerp(Height1, Height2, FractX);

	Height3 = DataLevels[1].ElevationData[(ZESize)((((ZEInt)Z + 1) * DataWidth) + (ZEInt)X)];
	Height4 = DataLevels[1].ElevationData[(ZESize)((((ZEInt)Z + 1) * DataWidth) + (ZEInt)X) + 1];

	Height3 = ZEMath::Lerp(Height3, Height4, FractX);

	Height1 = ZEMath::Lerp(Height1, Height3, FractZ);

	return (Height1 * HeightScale) + HeightOffset;
}

ZETerrain::ZETerrain()
{
	Wireframe = false;
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
	ColorTexture = NULL;
	DetailNormalTexture = NULL;
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

	return true;
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

	Levels.SetCount((ZESize)MaxLevel);
	for (ZESize I = 0; I < Levels.GetCount(); I++)
	{
		ZETerrainLevel* CurrentLevel = &Levels[I];
		CurrentLevel->ElevationTexture = ZETexture2D::CreateInstance();
		if (!CurrentLevel->ElevationTexture->Create(ChunkSize * 4 + 8 + 1, ChunkSize * 4 + 8 + 1, 1, ZE_TPF_F32_2, false))
			return false;

		CurrentLevel->Material = ZETerrainMaterial::CreateInstance();
		CurrentLevel->Material->Level = I;
		CurrentLevel->Material->SetHeightTexture(CurrentLevel->ElevationTexture);
		CurrentLevel->Material->SetColorTexture(CurrentLevel->ElevationTexture);
		CurrentLevel->Material->SetBlendTreshold(0.75f);
		CurrentLevel->Material->SetChunkSize(ChunkSize);
		CurrentLevel->Material->SetColorTexture((ZETexture2D*)ColorTexture);
		CurrentLevel->Material->SetNormalTexture((ZETexture2D*)DetailNormalTexture);
	}

	return true;
}

void ZETerrain::DestroyLevels()
{	
	for (ZESize I = 0; I < Levels.GetCount(); I++)
	{
		Levels[I].Material->Destroy();
		Levels[I].ElevationTexture->Destroy();
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
	Result = File.Open(TerrainFileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (!Result)
	{
		zeError("Can not open terrain file.");
		File.Close();
		return false;
	}

	ZEUInt32 Depth;
	File.Read(&Depth, sizeof(ZEUInt32), 1);
	DataLevels.SetCount(Depth);
	for (size_t I = 0; I < Depth; I++)
	{
		File.Read(&Header, sizeof(ZETerrainFileHeader), 1);
		DataLevels[I].ElevationHeight = Header.Heigth;
		DataLevels[I].ElevationWidth = Header.Width;
		DataLevels[I].ElevationData = new float[Header.Heigth * Header.Width];
		File.Read(DataLevels[I].ElevationData, Header.Width * Header.Heigth * sizeof(float), 1);
	}

	File.Close();

	DetailNormalTexture = ZETexture2DResource::LoadResource("normal.jpg")->GetTexture();
	ColorTexture = ZETexture2DResource::LoadResource("Diffuse.bmp")->GetTexture();
	 
	return true;
}

void ZETerrain::UnloadLevelData()
{
	for (ZESize I = 0; I < 8; I++)
		delete[] DataLevels[I].ElevationData;

	DataLevels.Clear();
}

float Sample(ZETerrainDataLevel* Data, ZESize x, ZESize y)
{
	if (x < 0 || x >= Data->ElevationWidth)
		return 0.0f;

	if (y < 0 || y >= Data->ElevationHeight)
		return 0.0f;
	
	return Data->ElevationData[y * Data->ElevationWidth + x];
}

float* Write(void* Buffer, ZESize Pitch, ZESize x, ZESize y)
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

	ZESize TextureSize = ((ZESize)ChunkSize * 4 + 8 + 1);
	float TexelSize = 1.0f / (float)TextureSize;

	ZESize LevelCount = Levels.GetCount();
	if (DataLevels.GetCount() < LevelCount)
	{
		LevelCount = DataLevels.GetCount();
	}
	if (MaxLevel < LevelCount)
	{
		LevelCount = MaxLevel;
	}

	for (ZESize CurrLevelIndex = 0; CurrLevelIndex < LevelCount - 1; CurrLevelIndex++)
	{
		ZETerrainLevel* CurrLevel = &Levels[CurrLevelIndex];
		CurrLevel->MinHeight = ZE_FLOAT_MAX;
		CurrLevel->MaxHeight = ZE_FLOAT_MIN;
		CurrLevel->Material->SetHeightOffset(HeightOffset);
		CurrLevel->Material->SetHeightScale(HeightScale);

		float TextureScale = (0.5f - 0.5f * TexelSize) / (2.0f * ChunkSize + 4.0f);
		CurrLevel->Material->SetTextureScale(ZEVector2(TextureScale, TextureScale));
		CurrLevel->Material->SetTextureOffset(ZEVector2(0.5f, 0.5f));

		ZETerrainDataLevel* CurrentLevelData = &DataLevels[CurrLevelIndex];
		ZETerrainDataLevel* NextLevelData = &DataLevels[CurrLevelIndex + 1];

		ZESSize DataX = (PositionX / (1 << CurrLevelIndex)) - (2 * ChunkSize + 4);
		ZESSize DataY = (-PositionY / (1 << CurrLevelIndex)) - (2 * ChunkSize + 4);
		
		void* Buffer;
		ZESize Pitch;		
		CurrLevel->ElevationTexture->Lock(&Buffer, &Pitch, 0);
		for (ZESize BufferY = 0; BufferY < TextureSize; BufferY++)
		{
			for (ZESize BufferX = 0; BufferX < TextureSize; BufferX++)
			{
				ZESSize PosX = DataX + BufferX;
				ZESSize PosY = DataY + BufferY;
				float Height = Sample(CurrentLevelData, PosX, PosY);

				float NextLevelHeight;
				if (PosX % 2 != 0 && PosY % 2 != 0)
				{
					NextLevelHeight  = Sample(NextLevelData, (PosX - 1) / 2, (PosY - 1) / 2);
					NextLevelHeight += Sample(NextLevelData, (PosX + 1) / 2, (PosY - 1) / 2);
					NextLevelHeight += Sample(NextLevelData, (PosX - 1) / 2, (PosY + 1) / 2);
					NextLevelHeight += Sample(NextLevelData, (PosX + 1) / 2, (PosY + 1) / 2);
					NextLevelHeight /= 4;
				}
				else if (PosX % 2 != 0)
				{
					NextLevelHeight  = Sample(NextLevelData, (PosX - 1) / 2, PosY / 2);
					NextLevelHeight += Sample(NextLevelData, (PosX + 1) / 2, PosY / 2);
					NextLevelHeight /= 2;
				}
				else if (PosY % 2 != 0)
				{
					NextLevelHeight  = Sample(NextLevelData, PosX / 2, (PosY - 1) / 2);
					NextLevelHeight += Sample(NextLevelData, PosX / 2, (PosY + 1) / 2);
					NextLevelHeight /= 2;
				}
				else
				{
					NextLevelHeight = Sample(NextLevelData,	PosX / 2 , PosY / 2);
				}
				
				Write(Buffer, Pitch, BufferX, BufferY)[0] = Height;
				Write(Buffer, Pitch, BufferX, BufferY)[1] = NextLevelHeight;

				if (Height < CurrLevel->MinHeight)
				{
					CurrLevel->MinHeight = Height;
				}
				else if (Height > CurrLevel->MaxHeight)
				{
					CurrLevel->MaxHeight = Height;
				}
			}
		}

		CurrLevel->ElevationTexture->Unlock(0);

		// Color

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

bool ZETerrain::DrawPrimtive(ZERenderer* Renderer, ZEInt PrimitiveType, ZEInt PositionX, ZEInt PositionY, ZEInt LocalPositionX, ZEInt LocalPositionY, ZEInt Mode, ZESize Level)
{	
	ZERenderCommand RenderCommand;
	RenderCommand.SetZero();
	RenderCommand.Flags				= ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration = VertexDeclaration;
	RenderCommand.Material			= Levels[Level].Material;
	RenderCommand.Order				= 0;
	RenderCommand.Pipeline			= ZE_RORP_3D;
	RenderCommand.VertexBuffer		= VertexBuffer;
	RenderCommand.PrimitiveType		= ZE_ROPT_TRIANGLE;
	RenderCommand.Priority			= 3;
	RenderCommand.Flags |= 1024;

	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateOrientation(ScaleMatrix, ZEVector3((float)PositionX, 0.0f, (float)PositionY), ZEQuaternion::Identity, ZEVector3((float)(1 << Level), 1.0f, (float)(1 << Level)));
	ZEMatrix4x4::Multiply(RenderCommand.WorldMatrix, GetWorldTransform(), ScaleMatrix);
	ZEMatrix4x4::CreateTranslation(RenderCommand.LocalMatrix, (float)LocalPositionX, 0.0f, (float)LocalPositionY);

	ZEInt BarSize;
	if (PrimitiveType == ZE_TP_CORNER_2)
	{
		BarSize = 2;
	}
	else if (PrimitiveType == ZE_TP_CORNER_1)
	{
		BarSize = 1;
	}
	else if (PrimitiveType == ZE_TP_CORNER)
	{
		BarSize = ChunkSize - 1;
	}
	else
	{
		BarSize = ChunkSize;
	}
	
	RenderCommand.VertexBufferOffset = Indices.Index[PrimitiveType] + BarSize * 2 * 3;
	RenderCommand.PrimitiveCount = ChunkSize * BarSize * 2;

	switch (Mode)
	{
		case ZE_TPM_SHRINK_NEGATIVE:
			RenderCommand.VertexBufferOffset += BarSize * 2 * 3;
			RenderCommand.PrimitiveCount -= BarSize * 2;
			break;

		case ZE_TPM_SHRINK_POSITIVE:
			RenderCommand.PrimitiveCount -= BarSize * 2;
			break;
		
		case ZE_TPM_EXTEND_NEGATIVE:
			RenderCommand.VertexBufferOffset -= BarSize * 2 * 3;
			RenderCommand.PrimitiveCount += BarSize * 2;
			break;

		case ZE_TPM_EXTEND_POSITIVE:
			RenderCommand.PrimitiveCount += BarSize * 2;
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

	static ZEInt PositionX;
	static ZEInt PositionY;
	static ZEInt OffsetPositionX;
	static ZEInt OffsetPositionY;
	ZECamera* Camera = DrawParameters->View->Camera;
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	if (!Locked)
	{
		PositionX = Align((ZEInt)ZEMath::Round(CameraPosition.x - OffsetPositionX), 1);
		PositionY = Align((ZEInt)ZEMath::Round(CameraPosition.z - OffsetPositionY), 1);
	}
	else
	{
		OffsetPositionX = (ZEInt)(CameraPosition.x - PositionX);
		OffsetPositionY = (ZEInt)(CameraPosition.z - PositionY);
	}

	Stream(DrawParameters, PositionX, PositionY);
	
	ZESize LevelCount = Levels.GetCount();
	if (DataLevels.GetCount() < LevelCount)
	{
		LevelCount = DataLevels.GetCount();
	}
	if ((ZESize)MaxLevel < LevelCount)
	{
		LevelCount = (ZESize)MaxLevel;
	}

	int ActiveLevel = 0;
	for (ZESize CurrIndex = 0; CurrIndex < LevelCount; CurrIndex++)
	{
		ZEInt CurrLevelPositionX = Align(PositionX, 1 << CurrIndex);
		ZEInt CurrLevelPositionY = Align(PositionY, 1 << CurrIndex);
		ZEInt NextLevelPositionX = Align(PositionX, 1 << (CurrIndex + 1));
		ZEInt NextLevelPositionY = Align(PositionY, 1 << (CurrIndex + 1));

		ZEInt PrevLevelPositionX; 
		ZEInt PrevLevelPositionY;
		if (CurrIndex == 0)
		{
			PrevLevelPositionX = CurrLevelPositionX;
			PrevLevelPositionY = CurrLevelPositionY;
		}
		else
		{
			PrevLevelPositionX = Align(PositionX, 1 << (CurrIndex - 1));
			PrevLevelPositionY = Align(PositionY, 1 << (CurrIndex - 1));
		}
		
		ZEInt LeftMode = ZE_TPM_NORMAL;
		ZEInt RightMode = ZE_TPM_NORMAL;
		ZEInt TopMode = ZE_TPM_NORMAL;
		ZEInt BottomMode = ZE_TPM_NORMAL;
		
		if (CurrLevelPositionX - NextLevelPositionX < 0)
		{
			RightMode = ZE_TPM_EXTEND_POSITIVE;
			LeftMode = ZE_TPM_SHRINK_NEGATIVE;
		}
		else if (CurrLevelPositionX - NextLevelPositionX > 0)
		{
			RightMode = ZE_TPM_SHRINK_POSITIVE;
			LeftMode = ZE_TPM_EXTEND_NEGATIVE;
		}

		if (CurrLevelPositionY - NextLevelPositionY < 0)
		{
			TopMode = ZE_TPM_EXTEND_POSITIVE;
			BottomMode = ZE_TPM_SHRINK_NEGATIVE;
		}
		else if (CurrLevelPositionY - NextLevelPositionY > 0)
		{
			TopMode = ZE_TPM_SHRINK_POSITIVE;
			BottomMode = ZE_TPM_EXTEND_NEGATIVE;
		}

		if (CurrIndex == ActiveLevel)
		{
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL,	 CurrLevelPositionX, CurrLevelPositionY, -(ZEInt)ChunkSize, -(ZEInt)ChunkSize, ZE_TPM_NORMAL, CurrIndex);

			DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL,	 CurrLevelPositionX, CurrLevelPositionY, -(ZEInt)ChunkSize, 0, ZE_TPM_NORMAL, CurrIndex);
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL,	 CurrLevelPositionX, CurrLevelPositionY, 0, -(ZEInt)ChunkSize, ZE_TPM_NORMAL, CurrIndex);
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL,	 CurrLevelPositionX, CurrLevelPositionY, 0, 0, ZE_TPM_NORMAL, CurrIndex);
		}
		
		// Left			
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL, CurrLevelPositionX, CurrLevelPositionY, -2 * (ZEInt)ChunkSize, -(ZEInt)ChunkSize, LeftMode, CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL, CurrLevelPositionX, CurrLevelPositionY, -2 * (ZEInt)ChunkSize, 0, LeftMode, CurrIndex);

		// Right
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL, CurrLevelPositionX, CurrLevelPositionY, (ZEInt)ChunkSize, -(ZEInt)ChunkSize, RightMode, CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_HORIZONTAL,	 CurrLevelPositionX, CurrLevelPositionY, (ZEInt)ChunkSize, 0 * (ZEInt)ChunkSize, RightMode, CurrIndex);

		// Top
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_VERTICAL, CurrLevelPositionX, CurrLevelPositionY, -(ZEInt)ChunkSize, (ZEInt)ChunkSize, TopMode, CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_VERTICAL, CurrLevelPositionX, CurrLevelPositionY, 0 * (ZEInt)ChunkSize, (ZEInt)ChunkSize, TopMode, CurrIndex);

		// Bottom
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_VERTICAL, CurrLevelPositionX, CurrLevelPositionY, (ZEInt)-ChunkSize, -2 * (ZEInt)ChunkSize, BottomMode, CurrIndex);
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_VERTICAL, CurrLevelPositionX, CurrLevelPositionY, 0, -2 * (ZEInt)ChunkSize, BottomMode, CurrIndex);

		Levels[CurrIndex].Material->SetWireframe(Wireframe);

		// Top Left
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize + 1, ChunkSize, TopMode, CurrIndex);
		if (LeftMode == ZE_TPM_NORMAL)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_1, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize, ChunkSize, TopMode, CurrIndex);
		else if (LeftMode == ZE_TPM_EXTEND_NEGATIVE)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_2, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize - 1, ChunkSize, TopMode, CurrIndex);

		// TopRight
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER, CurrLevelPositionX, CurrLevelPositionY, ChunkSize, ChunkSize, TopMode, CurrIndex);
		if (RightMode == ZE_TPM_NORMAL)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_1, CurrLevelPositionX, CurrLevelPositionY, 2 * ChunkSize - 1, ChunkSize, TopMode, CurrIndex);
		else if (RightMode == ZE_TPM_EXTEND_POSITIVE)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_2, CurrLevelPositionX, CurrLevelPositionY, 2 * ChunkSize - 1, ChunkSize, TopMode, CurrIndex);
				
		// Bottom Left
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize + 1, -2 * ChunkSize, BottomMode, CurrIndex);
		if (LeftMode == ZE_TPM_NORMAL)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_1, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize, -2 * ChunkSize, BottomMode, CurrIndex);
		else if (LeftMode == ZE_TPM_EXTEND_NEGATIVE)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_2, CurrLevelPositionX, CurrLevelPositionY, -2 * ChunkSize - 1, -2 * ChunkSize, BottomMode, CurrIndex);


		// Bottom Right
		DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER, CurrLevelPositionX, CurrLevelPositionY, ChunkSize, -2 * ChunkSize, BottomMode, CurrIndex);
		if (RightMode == ZE_TPM_NORMAL)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_1, CurrLevelPositionX, CurrLevelPositionY, 2 * ChunkSize - 1, -2 * ChunkSize, BottomMode, CurrIndex);
		else if (RightMode == ZE_TPM_EXTEND_POSITIVE)
			DrawPrimtive(DrawParameters->Renderer, ZE_TP_CORNER_2, CurrLevelPositionX, CurrLevelPositionY, 2 * ChunkSize - 1, -2 * ChunkSize, BottomMode, CurrIndex);
	}

	
	ZESSize Width = DataLevels[LevelCount - 1].ElevationWidth >> LevelCount;
	ZESSize Height = DataLevels[LevelCount - 1].ElevationWidth >> LevelCount;

	ZEInt CurrLevelPositionX = Align(PositionX, 1 << (LevelCount - 1));
	ZEInt CurrLevelPositionY = Align(PositionY, 1 << (LevelCount - 1));
}

ZETerrain* ZETerrain::CreateInstance()
{
	return new ZETerrain();
}
