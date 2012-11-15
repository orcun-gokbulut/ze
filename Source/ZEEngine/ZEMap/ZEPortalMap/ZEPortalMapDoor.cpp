//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapDoor.cpp
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

#include "ZEPortalMapDoor.h"
#include "ZEPortalMapResource.h"
#include "ZEPortalMap.h"
#include "ZEPortalMapPortal.h"
#include "ZEGraphics\ZESimpleMaterial.h"
#include "ZEGraphics\ZERenderer.h"

void ZEPortalMapDoor::DebugDraw(ZERenderer* Renderer)
{
	if (DebugDrawComponents.Material == NULL)
	{
		DebugDrawComponents.Material = ZESimpleMaterial::CreateInstance();

		DebugDrawComponents.BoxRenderCommand.SetZero();
		DebugDrawComponents.BoxRenderCommand.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BoxRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_NO_Z_WRITE;
		DebugDrawComponents.BoxRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BoxRenderCommand.VertexBuffer = &DebugDrawComponents.BoxCanvas;
		DebugDrawComponents.BoxRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	}

	DebugDrawComponents.BoxCanvas.Clean();
	DebugDrawComponents.BoxCanvas.SetColor(ZEVector4(0.254902f, 0.411765f, 0.882353f, 1.0f));

	ZERectangle3D Rectangle = GetRectangle();
	DebugDrawComponents.BoxCanvas.SetRotation(ZEQuaternion::Identity);
	DebugDrawComponents.BoxCanvas.SetTranslation(ZEVector3::Zero);
	DebugDrawComponents.BoxCanvas.AddWireframeQuad(Rectangle.P1, Rectangle.P2, Rectangle.P3, Rectangle.P4);
	ZEMatrix4x4 LocalMatrix;
	ZEMatrix4x4::CreateOrientation(LocalMatrix, Position, Rotation);
	DebugDrawComponents.BoxRenderCommand.WorldMatrix = ZEMatrix4x4::Identity;
	DebugDrawComponents.BoxRenderCommand.PrimitiveCount = DebugDrawComponents.BoxCanvas.Vertices.GetCount() / 2;
	DebugDrawComponents.BoxRenderCommand.Priority = 4;
	Renderer->AddToRenderList(&DebugDrawComponents.BoxRenderCommand);
}

void ZEPortalMapDoor::CalculateRectangle()
{
	float HalfWidth = (Width * Scale.x) / 2.0f;
	float HalfLength = (Length * Scale.z) / 2.0f;

	ZEMatrix4x4 Transform;
	ZEMatrix4x4::CreateOrientation(Transform, Position, Rotation);

	ZEMatrix4x4 WorldTransform;
	ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), Transform);

	ZEMatrix4x4::Transform(Rectangle.P1, WorldTransform, ZEVector3(-HalfWidth, 0.0f , HalfLength));
	ZEMatrix4x4::Transform(Rectangle.P2, WorldTransform, ZEVector3(HalfWidth, 0.0f , HalfLength));
	ZEMatrix4x4::Transform(Rectangle.P3, WorldTransform, ZEVector3(HalfWidth, 0.0f , -HalfLength));
	ZEMatrix4x4::Transform(Rectangle.P4, WorldTransform, ZEVector3(-HalfWidth, 0.0f , -HalfLength));
}

ZEPortalMap* ZEPortalMapDoor::GetOwner()
{
	return Owner;
}

const char* ZEPortalMapDoor::GetName()
{
	if (Resource != NULL)
		return Resource->Name;
	else
		return "";
}

ZEPortalMapPortal** ZEPortalMapDoor::GetPortals()
{
	return Portals;
}

const ZERectangle3D& ZEPortalMapDoor::GetRectangle()
{
	if (TransformChanged)
	{
		CalculateRectangle();
		TransformChanged = false;
	}

	return Rectangle;
}

void ZEPortalMapDoor::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
	TransformChanged = true;
}

const ZEVector3& ZEPortalMapDoor::GetPosition() const
{
	return Position;
}

void ZEPortalMapDoor::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	TransformChanged = true;
}

const ZEQuaternion& ZEPortalMapDoor::GetRotation() const
{
	return Rotation;
}

void ZEPortalMapDoor::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	TransformChanged = true;
}

const ZEVector3& ZEPortalMapDoor::GetScale() const
{
	return Scale;
}

void ZEPortalMapDoor::SetSeenThrough(bool Value)
{
	SeenThrough = Value;
}

bool ZEPortalMapDoor::GetSeenThrough()
{
	return SeenThrough;
}

void ZEPortalMapDoor::Initialize(ZEPortalMap* Owner, const ZEPortalMapResourceDoor* Resource)
{
	this->Owner = Owner;
	this->Resource = Resource;
	this->Open = Resource->IsOpen;
	this->Width = Resource->Width;
	this->Length = Resource->Length;

	this->Position = Resource->Position;
	this->Rotation = Resource->Rotation;
	this->Scale = Resource->Scale;
	
	CalculateRectangle();
	
	Portals[0] = Owner->Portals[(ZESize)Resource->PortalIds[0]];
	Portals[0]->Doors.Add(this);

	Portals[1] = Owner->Portals[(ZESize)Resource->PortalIds[1]];
	Portals[1]->Doors.Add(this);
}

void ZEPortalMapDoor::Deinitialize()
{
	Owner = NULL;
	Resource = NULL;
}

void ZEPortalMapDoor::SetOpen(bool Open)
{
	this->Open = Open;
}

bool ZEPortalMapDoor::GetOpen()
{
	return Open;
}

ZEPortalMapDoor::ZEPortalMapDoor()
{
	Owner = NULL;
	Resource = NULL;
	DebugDrawComponents.Material = NULL;
	Portals[0] = NULL;
	Portals[1] = NULL;
	Open = true;
	TransformChanged = false;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}

ZEPortalMapDoor* ZEPortalMapDoor::CreateInstance()
{
	return new ZEPortalMapDoor();
}
