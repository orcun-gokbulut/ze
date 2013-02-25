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
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEMath/ZEMath.h"

ZE_OBJECT_IMPL(ZEGrid)

ZEDrawFlags ZEGrid::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEGrid::GenerateGrid()
{
	AxisX.Clean();
	AxisX.SetColor(ZEVector4(AxisColor, 1.0f));
	AxisX.AddLine(ZEVector3(-0.5f, 0.0f, 0.0f), ZEVector3(0.5f, 0.0f, 0.0f));
	
	AxisZ.Clean();
	AxisZ.SetColor(ZEVector4(AxisColor, 1.0f));
	AxisZ.AddLine(ZEVector3(0.0f, 0.0f, -0.5f), ZEVector3(0.0f, 0.0f, 0.5f));


	MinorGrid.Clean();

	MinorGrid.SetColor(ZEVector4(MinorGridColor, 1.0f));

	ZEInt MinorGridLineCountX = (ZEInt)(GridSize.x / MinorGridUnitSize.x);
	if (MinorGridLineCountX % 2 == 1)
		MinorGridLineCountX--;
	MinorGridLineCountX /= 2;

	ZEInt MinorGridLineCountY = (ZEInt)(GridSize.y / MinorGridUnitSize.y);
	if (MinorGridLineCountY % 2 == 1)
		MinorGridLineCountY--;
	MinorGridLineCountY /= 2;


	for (ZEInt X = -MinorGridLineCountX; X <= MinorGridLineCountX; X++)
		MinorGrid.AddLine(ZEVector3(X * MinorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MinorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));

	for (ZEInt Y = -MinorGridLineCountY; Y <= MinorGridLineCountY; Y++)
		MinorGrid.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MinorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MinorGridUnitSize.y));


	MajorGrid.Clean();

	MajorGrid.SetColor(ZEVector4(MajorGridColor, 1.0f));

	ZEInt MajorGridLineCountX = (ZEInt)(GridSize.x / MajorGridUnitSize.x);
	if (MajorGridLineCountX % 2 == 1)
		MajorGridLineCountX--;
	MajorGridLineCountX /= 2;

	ZEInt MajorGridLineCountY = (ZEInt)(GridSize.y / MajorGridUnitSize.y);
	if (MajorGridLineCountY % 2 == 1)
		MajorGridLineCountY--;
	MajorGridLineCountY /= 2;


	for (ZEInt X = -MajorGridLineCountX; X <= MajorGridLineCountX; X++)
		MajorGrid.AddLine(ZEVector3(X * MajorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MajorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));

	for (ZEInt Y = -MajorGridLineCountY; Y <= MajorGridLineCountY; Y++)
		MajorGrid.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MajorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MajorGridUnitSize.y));
}

void ZEGrid::SetGridSize(const ZEVector2& Size)
{
	GridSize = Size;
}

const ZEVector2& ZEGrid::GetGridSize()
{
	return GridSize;
}

void ZEGrid::SetMinorGridEnabled(bool Enable)
{
	MinorGridEnabled = Enable;
}

bool ZEGrid::GetMinorGridEnabled()
{
	return MinorGridEnabled;
}

void ZEGrid::SetMinorGridUnitSize(const ZEVector2& Size)
{
	MinorGridUnitSize = Size;
}

const ZEVector2& ZEGrid::GetMinorGridUnitSize()
{
	return MinorGridUnitSize;
}

void ZEGrid::SetMinorGridColor(const ZEVector3& Color)
{
	MinorGridColor = Color;
}

const ZEVector3& ZEGrid::GetMinorGridColor()
{
	return MinorGridColor;
}

void ZEGrid::SetMajorGridEnabled(bool Enabled)
{
	MajorGridEnabled = Enabled;
}

bool ZEGrid::GetMajorGridEnabled()
{
	return MajorGridEnabled;
}

void ZEGrid::SetMajorGridUnitSize(const ZEVector2& Size)
{
	MajorGridUnitSize = Size;
}

const ZEVector2& ZEGrid::GetMajorGridUnitSize()
{
	return MajorGridUnitSize;
}

void ZEGrid::SetMajorGridColor(const ZEVector3& Color)
{
	MajorGridColor = Color;
}

const ZEVector3& ZEGrid::GetMajorGridColor()
{
	return MajorGridColor;
}

void ZEGrid::SetAxisEnabled(bool Enabled)
{
	AxisEnabled = Enabled;
}

bool ZEGrid::GetAxisEnabled()
{
	return AxisEnabled;
}


void ZEGrid::SetAxisColor(const ZEVector3& Color)
{
	this->AxisColor = Color;
}

const ZEVector3& ZEGrid::GetAxisColor()
{
	return AxisColor;
}

void ZEGrid::Draw(ZEDrawParameters* Parameters)
{
	ZEVector3 CameraPosition = Parameters->View->Camera->GetWorldPosition();
	
	if (MinorGridEnabled)
	{
		RenderCommand.Order = 0.0f;
		ZEVector3 MinorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, MinorGridPosition, GetRotation(), ZEVector3::One);
		RenderCommand.VertexBuffer = &MinorGrid;
		RenderCommand.PrimitiveCount = MinorGrid.Vertices.GetCount() / 2;
		Parameters->Renderer->AddToRenderList(&RenderCommand);
	}

	if (MajorGridEnabled)
	{
		RenderCommand.Order = 1.0f;
		ZEVector3 MajorGridPosition(
			CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, MajorGridPosition, GetRotation(), ZEVector3::One);
		RenderCommand.VertexBuffer = &MajorGrid;
		RenderCommand.PrimitiveCount = MajorGrid.Vertices.GetCount() / 2;
		Parameters->Renderer->AddToRenderList(&RenderCommand);
	}

	if (AxisEnabled)
	{
		RenderCommand.Order = 2.0f;
		ZEVector3 AxisXGridPosition(CameraPosition.x - ZEMath::Mod(CameraPosition.x, MajorGridUnitSize.x), 0.0f, 0.0f);
		ZEVector3 AxisYGridPosition(0.0f, 0.0f, CameraPosition.z - ZEMath::Mod(CameraPosition.z, MajorGridUnitSize.y));

		RenderCommand.PrimitiveCount = 1;

		if (AxisYGridPosition.z > -GridSize.y * 0.5f && AxisYGridPosition.z < GridSize.y * 0.5f)
		{
			ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, AxisXGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));
			RenderCommand.VertexBuffer = &AxisX;
			Parameters->Renderer->AddToRenderList(&RenderCommand);
		}

		if (AxisXGridPosition.x > -GridSize.x * 0.5f && AxisXGridPosition.x < GridSize.x * 0.5f)
		{
			ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, AxisYGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));
			RenderCommand.VertexBuffer = &AxisZ;
			Parameters->Renderer->AddToRenderList(&RenderCommand);
		}
	}
}

bool ZEGrid::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	GenerateGrid();
	
	if (Material == NULL)
		Material = ZESimpleMaterial::CreateInstance();

	RenderCommand.SetZero();
	RenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_LINE;
	RenderCommand.Flags = ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderCommand.Material = Material;

	return true;
}

bool ZEGrid::DeinitializeSelf()
{
	MinorGrid.Clean();
	MajorGrid.Clean();
	AxisX.Clean();
	AxisZ.Clean();

	if (Material != NULL)
	{
		Material->Release();
		Material = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

ZEGrid::ZEGrid()
{
	Material = NULL;

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
