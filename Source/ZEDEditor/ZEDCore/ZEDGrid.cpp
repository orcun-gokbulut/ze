//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGrid.cpp
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

#include "ZEDGrid.h"

#include "ZEMath/ZEMath.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

void ZEDGrid::GenerateGrid()
{
	if (GetState() == ZE_ES_NONE || GetState() == ZE_ES_DEINITIALIZING)
		return;

	ZECanvas Canvas;

	// Axis
	Canvas.SetColor(ZEVector4(AxisColor, 1.0f));
	Canvas.AddLine(ZEVector3(-0.5f, 0.0f, 0.0f), ZEVector3(0.5f, 0.0f, 0.0f));
	Canvas.AddLine(ZEVector3(0.0f, 0.0f, -0.5f), ZEVector3(0.0f, 0.0f, 0.5f));

	// Minor Grid
	MinorGridOffset = Canvas.GetVertexCount();

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
		Canvas.AddLine(ZEVector3(X * MinorGridUnitSize.x, 0.0f, -GridSize.x * 0.5f), ZEVector3(X * MinorGridUnitSize.x, 0.0f, GridSize.x * 0.5f));
	for (ZEInt Y = -MinorGridLineCountY; Y <= MinorGridLineCountY; Y++)
		Canvas.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MinorGridUnitSize.y), ZEVector3(GridSize.y * 0.5f, 0.0f, Y * MinorGridUnitSize.y));

	MinorGridCount = Canvas.GetVertexCount() - MinorGridOffset;


	// Major Grid
	MajorGridOffset = Canvas.GetVertexCount();

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
		Canvas.AddLine(ZEVector3(X * MajorGridUnitSize.x, 0.0f, -GridSize.x * 0.5f), ZEVector3(X * MajorGridUnitSize.x, 0.0f, GridSize.x * 0.5f));
	for (ZEInt Y = -MajorGridLineCountY; Y <= MajorGridLineCountY; Y++)
		Canvas.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MajorGridUnitSize.y), ZEVector3(GridSize.y * 0.5f, 0.0f, Y * MajorGridUnitSize.y));

	MajorGridCount = Canvas.GetVertexCount() - MajorGridOffset;	

	VertexBuffer = Canvas.CreateVertexBuffer();
}

ZEEntityResult ZEDGrid::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	ConstantBufferAxisTransform = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));
	zeCheckError(ConstantBufferAxisTransform == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");
	
	ConstantBufferMinorGridTransform = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));
	zeCheckError(ConstantBufferMinorGridTransform == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");
	
	ConstantBufferMajorGridTransform = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));
	zeCheckError(ConstantBufferMajorGridTransform == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");

	Material = ZERNSimpleMaterial::CreateInstance();
	Material->SetPrimitiveType(ZEGR_PT_LINE_LIST);
	Material->SetVertexColorEnabled(true);
	Material->SetStageMask(ZERN_STAGE_FORWARD_POST_HDR);
	Material->Update();

	RenderCommand.Entity = this;
	RenderCommand.Priority = 0;
	RenderCommand.Order = 0;
	RenderCommand.ExtraParameters = NULL;

	GenerateGrid();
	zeCheckError(VertexBuffer == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create vertex buffer.");

	return ZE_ER_DONE;
}

ZEEntityResult ZEDGrid::UnloadInternal()
{
	VertexBuffer.Release();
	ConstantBufferAxisTransform.Release();
	ConstantBufferMinorGridTransform.Release();
	ConstantBufferMajorGridTransform.Release();
	Material.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEDGrid::ZEDGrid()
{
	GridSize = ZEVector2(100.0f, 100.0f);

	MinorGridEnabled = true;
	MinorGridUnitSize = ZEVector2(1.0f, 1.0f);
	MinorGridColor = ZEVector3(0.3f, 0.3f, 0.3f);

	MajorGridEnabled = true;
	MajorGridUnitSize = ZEVector2(5.0f, 5.0f);
	MajorGridColor = ZEVector3(0.5f, 0.5f, 0.5f);

	AxisEnabled = true;
	AxisColor = ZEVector3(0.7f, 0.7f, 0.7f);

	SetEntityFlags(ZE_EF_RENDERABLE);
}

void ZEDGrid::SetGridSize(const ZEVector2& Size)
{
	if (GridSize == Size)
		return;

	GridSize = Size;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

const ZEVector2& ZEDGrid::GetGridSize()
{
	return GridSize;
}

void ZEDGrid::SetMinorGridEnabled(bool Enable)
{
	if (MinorGridEnabled == Enable)
		return;

	MinorGridEnabled = Enable;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

bool ZEDGrid::GetMinorGridEnabled()
{
	return MinorGridEnabled;
}

void ZEDGrid::SetMinorGridUnitSize(const ZEVector2& Size)
{
	if (MinorGridUnitSize == Size)
		return;

	MinorGridUnitSize = Size;
	
	if (IsLoadedOrLoading())
		Reload();
}

const ZEVector2& ZEDGrid::GetMinorGridUnitSize()
{
	return MinorGridUnitSize;
}

void ZEDGrid::SetMinorGridColor(const ZEVector3& Color)
{
	if (MinorGridColor == Color)
		return;

	MinorGridColor = Color;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

const ZEVector3& ZEDGrid::GetMinorGridColor()
{
	return MinorGridColor;
}

void ZEDGrid::SetMajorGridEnabled(bool Enabled)
{
	if (MajorGridEnabled == Enabled)
		return;

	MajorGridEnabled = Enabled;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

bool ZEDGrid::GetMajorGridEnabled()
{
	return MajorGridEnabled;
}

void ZEDGrid::SetMajorGridUnitSize(const ZEVector2& Size)
{
	if (MajorGridUnitSize == Size)
		return;

	MajorGridUnitSize = Size;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

const ZEVector2& ZEDGrid::GetMajorGridUnitSize()
{
	return MajorGridUnitSize;
}

void ZEDGrid::SetMajorGridColor(const ZEVector3& Color)
{
	if (MajorGridColor == Color)
		return;

	MajorGridColor = Color;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

const ZEVector3& ZEDGrid::GetMajorGridColor()
{
	return MajorGridColor;
}

void ZEDGrid::SetAxisEnabled(bool Enabled)
{
	if (AxisEnabled == Enabled)
		return;

	AxisEnabled = Enabled;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

bool ZEDGrid::GetAxisEnabled()
{
	return AxisEnabled;
}

void ZEDGrid::SetAxisColor(const ZEVector3& Color)
{
	if (AxisColor == Color)
		return;

	this->AxisColor = Color;

	if (IsLoadedOrLoading())
		GenerateGrid();
}

const ZEVector3& ZEDGrid::GetAxisColor()
{
	return AxisColor;
}

bool ZEDGrid::PreRender(const ZERNPreRenderParameters* Parameters)
{
	ZEVector3 CameraPosition = Parameters->View->Position;

	if (!Material->PreRender(RenderCommand))
		return false;

	if (AxisEnabled)
	{
		ZEVector3 AxisGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));
		if ((AxisGridPosition.x > -GridSize.x * 0.5f && AxisGridPosition.x < GridSize.x * 0.5f) &&
			(AxisGridPosition.z > -GridSize.y * 0.5f && AxisGridPosition.z < GridSize.y * 0.5f))
		{
			ZEMatrix4x4::CreateOrientation(Constants.AxisTransform, AxisGridPosition, GetRotation(), ZEVector3(GridSize.x, 1.0f, GridSize.y));
		}

		ConstantBufferAxisTransform->SetData(&Constants.AxisTransform);
	}

	if (MinorGridEnabled)
	{
		ZEVector3 MinorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));
		ZEMatrix4x4::CreateOrientation(Constants.MinorGridTransform, MinorGridPosition, GetRotation(), ZEVector3::One);

		ConstantBufferMinorGridTransform->SetData(&Constants.MinorGridTransform);
	}

	if (MajorGridEnabled)
	{
		ZEVector3 MajorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(Constants.MajorGridTransform, MajorGridPosition, GetRotation(), ZEVector3::One);

		ConstantBufferMajorGridTransform->SetData(&Constants.MajorGridTransform);
	}
	
	Parameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEDGrid::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	Context->SetVertexBuffer(0, VertexBuffer);

	if (MinorGridEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBufferMinorGridTransform);
		Context->Draw(MinorGridCount, MinorGridOffset);
	}

	if (MajorGridEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBufferMajorGridTransform);
		Context->Draw(MajorGridCount, MajorGridOffset);
	}

	if (AxisEnabled)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBufferAxisTransform);
		Context->Draw(4, 0);
	}

	Material->CleanupMaterial(Context, Parameters->Stage);
}

ZEDGrid* ZEDGrid::CreateInstance()
{
	return new ZEDGrid();
}
