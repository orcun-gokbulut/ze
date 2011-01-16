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
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGraphics/ZECamera.h"

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

	int MinorGridLineCountX = GridSize.x / MinorGridUnitSize.x;
	if (MinorGridLineCountX % 2 == 1)
		MinorGridLineCountX--;
	MinorGridLineCountX /= 2;

	int MinorGridLineCountY = GridSize.y / MinorGridUnitSize.y;
	if (MinorGridLineCountY % 2 == 1)
		MinorGridLineCountY--;
	MinorGridLineCountY /= 2;


	for (int X = -MinorGridLineCountX; X <= MinorGridLineCountX; X++)
		MinorGrid.AddLine(ZEVector3(X * MinorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MinorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));

	for (int Y = -MinorGridLineCountY; Y <= MinorGridLineCountY; Y++)
		MinorGrid.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MinorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MinorGridUnitSize.y));


	MajorGrid.Clean();

	MajorGrid.SetColor(ZEVector4(MajorGridColor, 1.0f));

	int MajorGridLineCountX = GridSize.x / MajorGridUnitSize.x;
	if (MajorGridLineCountX % 2 == 1)
		MajorGridLineCountX--;
	MajorGridLineCountX /= 2;

	int MajorGridLineCountY = GridSize.y / MajorGridUnitSize.y;
	if (MajorGridLineCountY % 2 == 1)
		MajorGridLineCountY--;
	MajorGridLineCountY /= 2;


	for (int X = -MajorGridLineCountX; X <= MajorGridLineCountX; X++)
		MajorGrid.AddLine(ZEVector3(X * MajorGridUnitSize.x, 0.0f, -GridSize.y * 0.5f), ZEVector3(X * MajorGridUnitSize.x, 0.0f, GridSize.y * 0.5f));

	for (int Y = -MajorGridLineCountY; Y <= MajorGridLineCountY; Y++)
		MajorGrid.AddLine(ZEVector3(-GridSize.y * 0.5f, 0.0f, Y * MajorGridUnitSize.y), ZEVector3(GridSize.x * 0.5f, 0.0f, Y * MajorGridUnitSize.y));
}

void ZEGrid::Draw(ZEDrawParameters* Parameters)
{
	ZEVector3 CameraPosition = Parameters->View->Camera->GetWorldPosition();
	
	if (MinorGridEnabled)
	{
		ZEVector3 MinorGridPosition(
			CameraPosition.x - fmod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - fmod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(RenderOrder.WorldMatrix, MinorGridPosition, GetRotation(), ZEVector3::One);
		RenderOrder.VertexBuffer = &MinorGrid;
		RenderOrder.PrimitiveCount = MinorGrid.Vertices.GetCount() / 2;
		Parameters->Renderer->AddToRenderList(&RenderOrder);
	}

	if (MajorGridEnabled)
	{
		ZEVector3 MajorGridPosition(
			CameraPosition.x - fmod(CameraPosition.x, MajorGridUnitSize.x),
			0.0f,
			CameraPosition.z - fmod(CameraPosition.z, MajorGridUnitSize.y));

		ZEMatrix4x4::CreateOrientation(RenderOrder.WorldMatrix, MajorGridPosition, GetRotation(), ZEVector3::One);
		RenderOrder.VertexBuffer = &MajorGrid;
		RenderOrder.PrimitiveCount = MajorGrid.Vertices.GetCount() / 2;
		Parameters->Renderer->AddToRenderList(&RenderOrder);
	}

	if (AxisEnabled)
	{
		ZEVector3 AxisXGridPosition(CameraPosition.x - fmod(CameraPosition.x, MajorGridUnitSize.x), 0.0f, 0.0f);
		ZEVector3 AxisYGridPosition(0.0f, 0.0f, CameraPosition.z - fmod(CameraPosition.z, MajorGridUnitSize.y));

		RenderOrder.PrimitiveCount = 1;

		if (AxisYGridPosition.z > -GridSize.y * 0.5f && AxisYGridPosition.z < GridSize.y * 0.5f)
		{
			ZEMatrix4x4::CreateOrientation(RenderOrder.WorldMatrix, AxisXGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));
			RenderOrder.VertexBuffer = &AxisX;
			Parameters->Renderer->AddToRenderList(&RenderOrder);
		}

		if (AxisXGridPosition.x > -GridSize.x * 0.5f && AxisXGridPosition.x < GridSize.x * 0.5f)
		{
			ZEMatrix4x4::CreateOrientation(RenderOrder.WorldMatrix, AxisYGridPosition, GetRotation(), ZEVector3(GridSize.x, 0.0f, GridSize.y));
			RenderOrder.VertexBuffer = &AxisZ;
			Parameters->Renderer->AddToRenderList(&RenderOrder);
		}
	}
}

bool ZEGrid::Initialize()
{
	GenerateGrid();
	
	if (Material == NULL)
		Material = ZESimpleMaterial::CreateInstance();

	RenderOrder.SetZero();
	RenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderOrder.PrimitiveType = ZE_ROPT_LINE;
	RenderOrder.Flags = ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_NO_Z_WRITE;
	RenderOrder.Material = Material;

	return true;
}

void ZEGrid::Deinitialize()
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

#include "ZEGrid.h.zpp"

ZEEntityRunAt ZEGridDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
