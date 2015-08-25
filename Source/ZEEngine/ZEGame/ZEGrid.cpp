//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGrid.cpp
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

#include "ZEGrid.h"
#include "ZEScene.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZEMath/ZEMath.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

ZEDrawFlags ZEGrid::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEGrid::GenerateGrid()
{
	if (GetState() == ZE_ES_NOT_INITIALIZED || GetState() == ZE_ES_DEINITIALIZING)
		return;

	ZECanvas Canvas;

	// Axis
	Canvas.SetColor(ZEVector4(AxisColor, 1.0f));
	Canvas.AddLine(ZEVector3(-0.5f, 0.0f, 0.0f), ZEVector3(0.5f, 0.0f, 0.0f));
	Canvas.AddLine(ZEVector3(0.0f, 0.0f, -0.5f), ZEVector3(0.0f, 0.0f, 0.5f));

	// Minor Grid
	MinorGridOffset = Canvas.Vertices.GetCount();

	ZEInt MinorGridLineCountX = (ZEInt)(GridSize.x / MinorGridUnitSize.x);
	if (MinorGridLineCountX % 2 == 1)
		MinorGridLineCountX--;
	MinorGridLineCountX /= 2;
	ZEInt MinorGridLineCountY = (ZEInt)(GridSize.y / MinorGridUnitSize.y);
	if (MinorGridLineCountY % 2 == 1)
		MinorGridLineCountY--;
	MinorGridLineCountY /= 2;

	Canvas.SetColor(ZEVector4(MinorGridColor, 1.0f));
	for (ZEInt X = -MinorGridLineCountX; X <= MinorGridLineCountX; X++)
		Canvas.AddLine(ZEVector3(X * MinorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MinorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));
	for (ZEInt Y = -MinorGridLineCountY; Y <= MinorGridLineCountY; Y++)
		Canvas.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MinorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MinorGridUnitSize.y));

	MinorGridCount = Canvas.Vertices.GetCount() - MinorGridOffset;

	
	// Major Grid
	MajorGridOffset = Canvas.Vertices.GetCount();

	ZEInt MajorGridLineCountX = (ZEInt)(GridSize.x / MajorGridUnitSize.x);
	if (MajorGridLineCountX % 2 == 1)
		MajorGridLineCountX--;
	MajorGridLineCountX /= 2;
	ZEInt MajorGridLineCountY = (ZEInt)(GridSize.y / MajorGridUnitSize.y);
	if (MajorGridLineCountY % 2 == 1)
		MajorGridLineCountY--;
	MajorGridLineCountY /= 2;

	Canvas.SetColor(ZEVector4(MajorGridColor, 1.0f));
	for (ZEInt X = -MajorGridLineCountX; X <= MajorGridLineCountX; X++)
		Canvas.AddLine(ZEVector3(X * MajorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MajorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));
	for (ZEInt Y = -MajorGridLineCountY; Y <= MajorGridLineCountY; Y++)
		Canvas.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MajorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MajorGridUnitSize.y));

	MajorGridCount = Canvas.Vertices.GetCount() - MajorGridOffset;

	VertexBuffer = Canvas.CreateVertexBuffer();
}

void ZEGrid::SetGridSize(const ZEVector2& Size)
{
	if (GridSize == Size)
		return;

	GridSize = Size;
	GenerateGrid();
}

const ZEVector2& ZEGrid::GetGridSize()
{
	return GridSize;
}

void ZEGrid::SetMinorGridEnabled(bool Enable)
{
	if (MinorGridEnabled == Enable)
		return;

	MinorGridEnabled = Enable;
	GenerateGrid();
}

bool ZEGrid::GetMinorGridEnabled()
{
	return MinorGridEnabled;
}

void ZEGrid::SetMinorGridUnitSize(const ZEVector2& Size)
{
	if (MinorGridUnitSize == Size)
		return;

	MinorGridUnitSize = Size;
	GenerateGrid();
}

const ZEVector2& ZEGrid::GetMinorGridUnitSize()
{
	return MinorGridUnitSize;
}

void ZEGrid::SetMinorGridColor(const ZEVector3& Color)
{
	if (MinorGridColor == Color)
		return;

	MinorGridColor = Color;
	GenerateGrid();
}

const ZEVector3& ZEGrid::GetMinorGridColor()
{
	return MinorGridColor;
}

void ZEGrid::SetMajorGridEnabled(bool Enabled)
{
	if (MajorGridEnabled == Enabled)
		return;

	MajorGridEnabled = Enabled;
	GenerateGrid();
}

bool ZEGrid::GetMajorGridEnabled()
{
	return MajorGridEnabled;
}

void ZEGrid::SetMajorGridUnitSize(const ZEVector2& Size)
{
	if (MajorGridUnitSize == Size)
		return;

	MajorGridUnitSize = Size;
	GenerateGrid();
}

const ZEVector2& ZEGrid::GetMajorGridUnitSize()
{
	return MajorGridUnitSize;
}

void ZEGrid::SetMajorGridColor(const ZEVector3& Color)
{
	if (MajorGridColor == Color)
		return;

	MajorGridColor = Color;
	GenerateGrid();
}

const ZEVector3& ZEGrid::GetMajorGridColor()
{
	return MajorGridColor;
}

void ZEGrid::SetAxisEnabled(bool Enabled)
{
	if (AxisEnabled == Enabled)
		return;

	AxisEnabled = Enabled;
	GenerateGrid();
}

bool ZEGrid::GetAxisEnabled()
{
	return AxisEnabled;
}

void ZEGrid::SetAxisColor(const ZEVector3& Color)
{
	if (AxisColor == Color)
		return;

	this->AxisColor = Color;
	GenerateGrid();
}

const ZEVector3& ZEGrid::GetAxisColor()
{
	return AxisColor;
}

bool ZEGrid::PreRender(const ZERNCullParameters* Parameters)
{
	// Update Constants
	ZEVector3 CameraPosition = Parameters->View->Position;

	if (AxisEnabled)
	{
		RenderCommand.Order = 2.0f;
		ZEVector3 AxisXGridPosition(CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x), 0.0f, 0.0f);
		ZEVector3 AxisYGridPosition(0.0f, 0.0f, CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		if (AxisYGridPosition.z > -GridSize.y * 0.5f && AxisYGridPosition.z < GridSize.y * 0.5f)
			ZEMatrix4x4::CreateOrientation(Constants.AxisXTransform, AxisXGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));

		if (AxisXGridPosition.x > -GridSize.x * 0.5f && AxisXGridPosition.x < GridSize.x * 0.5f)
			ZEMatrix4x4::CreateOrientation(Constants.AxisYTransform, AxisYGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));
	}


	if (MinorGridEnabled)
	{
		RenderCommand.Order = 1.0f;
		ZEVector3 MinorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));
		ZEMatrix4x4::CreateOrientation(Constants.MinorGridTransform, MinorGridPosition, GetRotation(), ZEVector3::One);
	}

	if (MajorGridEnabled)
	{
		RenderCommand.Order = 1.0f;
		ZEVector3 MajorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(Constants.MajorGridTransform, MajorGridPosition, GetRotation(), ZEVector3::One);
	}

	RenderCommand.Priority = 0;
	RenderCommand.Order = 0;
	RenderCommand.StageMask = Material->GetStageMask();

	void* Buffer;
	ConstantBuffer->Lock(&Buffer);
	memcpy(Buffer, &Constants, sizeof(Constants));
	ConstantBuffer->Unlock();

	Parameters->Renderer->AddCommand(&RenderCommand);
	return true;
}

void ZEGrid::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	Context->SetVertexBuffer(0, VertexBuffer);

	Material->SetupMaterial(Context, Parameters->Stage);

	if (MinorGridEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer, 0, 4);
		Context->Draw(MinorGridCount, MinorGridOffset);
	}

	if (MajorGridEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer, 4, 4);
		Context->Draw(MajorGridCount, MajorGridOffset);
	}

	if (AxisEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer, 8, 4);
		Context->Draw(2, 0);
		
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer, 12, 4);
		Context->Draw(2, 2);
	}

	Material->CleanupMaterial(Context, Parameters->Stage);
}

bool ZEGrid::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	RenderCommand.Entity = this;
	RenderCommand.Priority = 0;
	RenderCommand.Order = 0;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	Material = ZERNSimpleMaterial::CreateInstance();
	Material->SetVertexColor(true);
	Material->Initialize();

	GenerateGrid();
	
	return true;
}

bool ZEGrid::DeinitializeSelf()
{
	Material.Release();
	VertexBuffer.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEGrid::ZEGrid()
{
	GridSize = ZEVector2(100.0f, 100.0f);

	MinorGridEnabled = true;
	MinorGridUnitSize = ZEVector2(1.0f, 1.0f);
	MinorGridColor = ZEVector3(0.4f, 0.4f, 0.4f);

	MajorGridEnabled = true;
	MajorGridUnitSize = ZEVector2(5.0f, 5.0f);
	MajorGridColor = ZEVector3(0.3f, 0.3f, 0.3f);

	AxisEnabled = true;
	AxisColor = ZEVector3::Zero;
}

ZEGrid* ZEGrid::CreateInstance()
{
	return new ZEGrid();
}
