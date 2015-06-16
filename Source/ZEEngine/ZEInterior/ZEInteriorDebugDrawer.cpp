//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorDebugDrawer.cpp
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

#include "ZEInteriorDebugDrawer.h"
#include "ZEInterior.h"
#include "ZEInteriorDoor.h"
#include "ZEInteriorRoom.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZERenderer\ZESimpleMaterial.h"
#include "ZERenderer\ZERenderer.h"

void ZEInteriorDebugDrawer::DebugDrawEntity()
{
	ZEDebugDrawer::DebugDrawEntity();

	ZEInterior* InteriorTarget = (ZEInterior*)Target;

	for (ZESize I = 0; I < InteriorTarget->Doors.GetCount(); I++)
		DrawRectangle(InteriorTarget->Doors[I]->GetRectangle(), DoorColor, InteriorDrawCanvas);

	for (ZESize I = 0; I < InteriorTarget->Rooms.GetCount(); I++)
		DrawAxisAlignedBoundingBox(InteriorTarget->Rooms[I]->GetWorldBoundingBox(), RoomColor, InteriorDrawCanvas);
}

void ZEInteriorDebugDrawer::SetTarget(ZEEntity* Target)
{
	if (Target == NULL)
		return;

	if (ZEClass::IsDerivedFrom(ZEInterior::Class(), Target->GetClass()))
	{
		if (RoomColor == ZEVector4::Zero)
			SetRoomColor(ZEVector4(0.7f, 0.5f, 0.0f, 1.0f));

		if (DoorColor == ZEVector4::Zero)
			SetDoorColor(ZEVector4(0.254902f, 0.411765f, 0.882353f, 1.0f));

		ZEDebugDrawer::SetTarget(Target);
	}
}

void ZEInteriorDebugDrawer::SetRoomColor(ZEVector4 Color)
{
	RoomColor = Color;
}

ZEVector4 ZEInteriorDebugDrawer::GetRoomColor() const
{
	return RoomColor;
}

void ZEInteriorDebugDrawer::SetDoorColor(ZEVector4 Color)
{
	DoorColor = Color;
}

ZEVector4 ZEInteriorDebugDrawer::GetDoorColor() const
{
	return DoorColor;
}

void ZEInteriorDebugDrawer::Draw(ZEDrawParameters* DrawParameters)
{
	InteriorDrawCanvas.Clean();

	ZEDebugDrawer::Draw(DrawParameters);

	if (InteriorDrawCanvas.Vertices.GetCount() == 0)
		return;

	InteriorRenderCommand.PrimitiveCount = InteriorDrawCanvas.Vertices.GetCount() / 2;
	DrawParameters->Renderer->AddToRenderList(&InteriorRenderCommand);
}

bool ZEInteriorDebugDrawer::InitializeSelf()
{
	if (!ZEDebugDrawer::InitializeSelf())
		return false;

	if (InteriorMaterial == NULL)
		InteriorMaterial = ZESimpleMaterial::CreateInstance();

	InteriorRenderCommand.SetZero();
	InteriorRenderCommand.Material = InteriorMaterial;
	InteriorRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	InteriorRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	InteriorRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	InteriorRenderCommand.VertexBuffer = &InteriorDrawCanvas;
	InteriorRenderCommand.Priority = 10;

	return true;
}

bool ZEInteriorDebugDrawer::DeinitializeSelf()
{
	InteriorDrawCanvas.Clean();
	InteriorRenderCommand.SetZero();

	if (InteriorMaterial != NULL)
	{
		InteriorMaterial->Release();
		InteriorMaterial = NULL;
	}

	return ZEDebugDrawer::DeinitializeSelf();
}

ZEInteriorDebugDrawer::ZEInteriorDebugDrawer()
{
	RoomColor = ZEVector4::Zero;
	DoorColor = ZEVector4::Zero;
	InteriorMaterial = NULL;
	InteriorRenderCommand.SetZero();
}

ZEInteriorDebugDrawer::~ZEInteriorDebugDrawer()
{

}

ZEInteriorDebugDrawer* ZEInteriorDebugDrawer::CreateInstance()
{
	return new ZEInteriorDebugDrawer();
}
