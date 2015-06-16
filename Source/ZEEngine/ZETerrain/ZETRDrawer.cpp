//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRDrawer.cpp
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

#include "ZETRDrawer.h"
#include "ZETRTerrain.h"

#include "ZERenderer/ZEMaterial.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERenderCommand.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZERenderer/ZETerrainMaterial.h"

bool ZETRDrawer::InitializeSelf()
{
	VertexBuffer.SetPrimitiveSize(GetPrimitiveSize());
	if (!VertexBuffer.Initialize())
	{
		zeError("Can not initialize.");
		return false;
	}

	Material = ZETerrainMaterial::CreateInstance();
	Material->SetWireframe(false);
	Material->SetAmbientColor(ZEVector3::Zero);
	Material->SetAmbientFactor(0.0f);

	if (DebugDraw)
	{
		DebugMaterial = ZETerrainMaterial::CreateInstance();
		DebugMaterial->SetWireframe(true);
		DebugMaterial->SetAmbientColor(ZEVector3::Zero);
		DebugMaterial->SetAmbientFactor(0.0f);
		DebugMaterial->SetDiffuseColor(ZEVector3::Zero);
		DebugMaterial->SetDiffuseFactor(0.0f);
	}

	return true;
}

void ZETRDrawer::DeinitializeSelf()
{
	VertexBuffer.Deinitialize();

	if (Material != NULL)
	{
		Material->Destroy();
		Material = NULL;
	}

	if (DebugMaterial != NULL)
	{
		DebugMaterial->Destroy();
		DebugMaterial = NULL;
	}
}

void ZETRDrawer::SetTerrain(ZETRTerrain* Terrain)
{
	this->Terrain = Terrain;
}

ZETRTerrain* ZETRDrawer::GetTerrain()
{
	return Terrain;
}

void ZETRDrawer::SetPrimitiveSize(ZEUInt PrimtiveSize)
{
	VertexBuffer.SetPrimitiveSize(PrimtiveSize);
}

ZEUInt ZETRDrawer::GetPrimitiveSize()
{
	return VertexBuffer.GetPrimitiveSize();
}

void ZETRDrawer::SetMinLevel(ZEUInt MinLevel)
{
	this->MinLevel = MinLevel;
}

ZEUInt ZETRDrawer::GetMinLevel()
{
	return MinLevel;
}

void ZETRDrawer::SetMaxLevel(ZEUInt MaxLevel)
{
	this->MaxLevel = MaxLevel;
}

ZEUInt ZETRDrawer::GetMaxLevel()
{
	return MaxLevel;
}

void ZETRDrawer::SetMaxViewDistance(float MaxViewDistance)
{
	this->MaxViewDistance = MaxViewDistance;
}

float ZETRDrawer::GetMaxViewDistance()
{
	return MaxViewDistance;
}

void ZETRDrawer::SetMaterial(ZETerrainMaterial* Material)
{
	this->Material = Material;
}

ZETerrainMaterial* ZETRDrawer::GetMaterial()
{
	return Material;
}

void ZETRDrawer::SetDebugDraw(bool Enabled)
{
	DebugDraw = Enabled;
	Reinitialize();
}

bool ZETRDrawer::GetDebugDraw()
{
	return DebugDraw;
}

void ZETRDrawer::DrawPrimitive(ZEDrawParameters* DrawParameters, 
							   float WorldPositionX, float WorldPositionY, ZEUInt Level, 
							   float LocalPositionX, float LocalPositionY, 
							   ZETRPrimitiveType Type, ZEInt NegativeExtent, ZEInt PositiveExtent, 
							   float MinHeight, float MaxHeigt)
{
	float Scale = ZEMath::Power(2, Level);
	ZEAABBox BoundingBox;
	BoundingBox.Min = ZEVector3((float)(LocalPositionX + VertexBuffer.GetPrimitiveSize() - 1) * Scale, MinHeight, (float)(LocalPositionY + VertexBuffer.GetPrimitiveSize() - 1) * Scale);
	BoundingBox.Max = ZEVector3((float)(LocalPositionX + VertexBuffer.GetPrimitiveSize() + 1) * Scale, MaxHeigt, (float)(LocalPositionY + VertexBuffer.GetPrimitiveSize() + 1) * Scale);

	/*if (DrawParameters->View->Camera->GetViewVolume().CullTest(BoundingBox))
		return;*/
	
	float PositionX = LocalPositionX * (float)VertexBuffer.GetPrimitiveSize() * Scale;
	float PositionY = LocalPositionY * (float)VertexBuffer.GetPrimitiveSize() * Scale;

	ZETRPrimitive Primitive = VertexBuffer.GetPrimitive(Type, NegativeExtent, PositiveExtent);

	ZERenderCommand RenderCommand;
	RenderCommand.SetZero();
	RenderCommand.Flags					= ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration		= VertexBuffer.GetVertexDeclaration();
	RenderCommand.Material				= Material;
	RenderCommand.Order					= 0;
	RenderCommand.Pipeline				= ZE_RORP_3D;
	RenderCommand.VertexBuffer			= VertexBuffer.GetVertexBuffer();
	RenderCommand.PrimitiveType			= ZE_ROPT_TRIANGLE;
	RenderCommand.Priority				= 3;
	RenderCommand.InstanceData			= (void*)Level;
	RenderCommand.VertexBufferOffset	= Primitive.VertexOffset;
	RenderCommand.PrimitiveCount		= Primitive.VertexCount / 3;

	ZEMatrix4x4::CreateOrientation(RenderCommand.LocalMatrix, ZEVector3(PositionX, 0.0f, PositionY), ZEVector3(Scale, 1.0f, Scale));
	ZEMatrix4x4 WorldTransform;
	ZEMatrix4x4::CreateTranslation(WorldTransform, ZEVector3(WorldPositionX, 0.0f, WorldPositionY));
	ZEMatrix4x4::Multiply(RenderCommand.WorldMatrix, WorldTransform, RenderCommand.LocalMatrix);
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);

	if (DebugDraw)
	{
		RenderCommand.Material = DebugMaterial;
		DrawParameters->Renderer->AddToRenderList(&RenderCommand);
	}
}

void ZETRDrawer::SetViewPosition(const ZEVector3& Position)
{
	ViewPosition = Position;
}

const ZEVector3& ZETRDrawer::GetViewPosition()
{
	return ViewPosition;
}

void ZETRDrawer::Draw(ZEDrawParameters* DrawParameters)
{
	if (!IsInitialized())
		return;

	Material->Terrain = GetTerrain();
	Material->ElevationLayer = GetTerrain()->GetElevationLayer();
	Material->ColorLayer = GetTerrain()->GetColorLayer();
	Material->ChunkSize = GetPrimitiveSize();

	if (DebugDraw)
	{
		DebugMaterial->Terrain = GetTerrain();
		DebugMaterial->ElevationLayer = GetTerrain()->GetElevationLayer();
		DebugMaterial->ColorLayer = GetTerrain()->GetColorLayer();
		DebugMaterial->ChunkSize = GetPrimitiveSize();
	}
	
	ZEInt PositionX = ZEMath::Floor(ViewPosition.x);
	ZEInt PositionY = ZEMath::Floor(ViewPosition.z);

	for (ZEUInt Level = MinLevel; Level < MaxLevel; Level++)
	{
		float WorldPositionX = (PositionX / (1 << Level)) * (1 << Level);
		float WorldPositionY = (PositionY / (1 << Level)) * (1 << Level);

		ZEInt ShrinkDirectionX, ShrinkDirectionY;
		if (Level == MinLevel)
		{	
			ShrinkDirectionX = 0;
			ShrinkDirectionY = 0;
		}
		else
		{
			ShrinkDirectionX = (PositionX / (ZEInt)(1 << (Level - 1))) % 2;
			ShrinkDirectionY = (PositionY / (ZEInt)(1 << (Level - 1))) % 2;
		}

		ZEInt ExtentX, ExtentY;
		if (Level == MaxLevel)
		{
			ExtentX = 0;
			ExtentY = 0;
		}
		else
		{
			ExtentX = ((PositionX / (1 << Level)) % 2) != 0 ? 1 : 0;
			ExtentY = ((PositionY / (1 << Level)) % 2) != 0 ? 1 : 0;
		}

		if (MaxViewDistance != 0 && VertexBuffer.GetPrimitiveSize() * (float)(1 << Level) > MaxViewDistance)
			break;

		if (Level == MinLevel)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, 0.0f, 0.0f, ZETR_PT_CENTER,	0, 0, 0.0f, 10.0f);

		DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  0.0f,  1.5f, ZETR_PT_VERTICAL, (ShrinkDirectionY > 0 ? -1 : 0), ExtentY,	0.0f, 10.0f);
		DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  0.0f, -1.5f, ZETR_PT_VERTICAL, ExtentY, (ShrinkDirectionY < 0 ? -1 : 0),	0.0f, 10.0f);
		DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f,  0.0f, ZETR_PT_HORIZONTAL, ExtentX, (ShrinkDirectionX < 0 ? -1 : 0),	0.0f, 10.0f);
		DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f,  0.0f, ZETR_PT_HORIZONTAL, (ShrinkDirectionX > 0 ? -1 : 0), ExtentX,	0.0f, 10.0f);

		// Top Left Corner
 		ZEInt Shrink = ((ShrinkDirectionX < 0) && (ShrinkDirectionY > 0) ? -1 : 0);
		if (ExtentX && ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f,  1.5f, ZETR_PT_CORNER_FLIP, Shrink, 1, 0.0f, 10.0f);
		else if (ExtentX)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f,  1.5f, ZETR_PT_CORNER_HORIZONTAL_FLIP, Shrink, 1, 0.0f, 0.0f);
		else if (ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f,  1.5f, ZETR_PT_CORNER_VERTICAL_FLIP, Shrink, 1, 0.0f, 10.0f);
		else
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f,  1.5f, ZETR_PT_CORNER_FLIP, Shrink, 0, 0.0f, 0.0f);
		

		// Top Right Corner
		Shrink = ((ShrinkDirectionX > 0) && (ShrinkDirectionY > 0) ? -1 : 0);
		if (ExtentX && ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f,  1.5f, ZETR_PT_CORNER, Shrink, 1, 0.0f, 10.0f);
		else if (ExtentX)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f,  1.5f, ZETR_PT_CORNER_HORIZONTAL, Shrink, 1, 0.0f, 0.0f);
		else if (ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f,  1.5f, ZETR_PT_CORNER_VERTICAL, Shrink, 1, 0.0f, 10.0f);
		else
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f,  1.5f, ZETR_PT_CORNER, Shrink, 0, 0.0f, 0.0f);


		// Bottom Right Corner
		Shrink = ((ShrinkDirectionX > 0) && (ShrinkDirectionY < 0) ? -1 : 0);
		if (ExtentX && ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f, -1.5f, ZETR_PT_CORNER_FLIP, 1, Shrink, 0.0f, 10.0f);
		else if (ExtentX)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f, -1.5f, ZETR_PT_CORNER_HORIZONTAL_FLIP, 1, Shrink, 0.0f, 0.0f);
		else if (ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f, -1.5f, ZETR_PT_CORNER_VERTICAL_FLIP, 1, Shrink, 0.0f, 10.0f);
		else
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level,  1.5f, -1.5f, ZETR_PT_CORNER_FLIP, 0, Shrink, 0.0f, 0.0f);

		// Bottom Left Corner
		Shrink = ((ShrinkDirectionX < 0) && (ShrinkDirectionY < 0) ? -1 : 0);
		if (ExtentX && ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f, -1.5f, ZETR_PT_CORNER, 1, Shrink, 0.0f, 10.0f);
		else if (ExtentX)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f, -1.5f, ZETR_PT_CORNER_HORIZONTAL, 1, Shrink, 0.0f, 0.0f);
		else if (ExtentY)
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f, -1.5f, ZETR_PT_CORNER_VERTICAL, 1, Shrink, 0.0f, 10.0f);
		else
			DrawPrimitive(DrawParameters, WorldPositionX, WorldPositionY, Level, -1.5f, -1.5f, ZETR_PT_CORNER, 0, Shrink, 0.0f, 0.0f);
	}
}

ZETRDrawer::ZETRDrawer()
{
	DebugDraw = false;
	MinLevel = 0;
	MaxLevel = 15;
	MaxViewDistance = 0;
	Material = NULL;
	DebugMaterial = NULL;
}

ZETRDrawer::~ZETRDrawer()
{
	Deinitialize();
}
