//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Entity.cpp
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

#include "Entity.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Ray.h"
#include "Core/Error.h"
#include <string.h>

void ZEEntity::SetBoundingVolumeMechanism(ZEBoundingVolumeMechnism Mechanism)
{
	BoundingVolumeMechanism = Mechanism;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
}

void ZEEntity::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
}

const ZEAABoundingBox& ZEEntity::GetLocalBoundingBox()
{
	return LocalBoundingBox;
}


void ZEEntity::UpdateComponents()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->OwnerWorldTransformChanged();
}

void ZEEntity::RegisterComponent(ZEComponent* Component)
{
	ZEDWORD ComponentRenderFlag = Component->GetDrawFlags();

	if (Component->GetDrawFlags() & ZE_DF_DRAW)
	{
		if (Component->GetVisible())
			UpdateBoundingVolumes();
		DrawFlags |= ZE_DF_DRAW_COMPONENTS;
	}

	if (Component->GetDrawFlags() & ZE_DF_LIGHT_SOURCE)
		DrawFlags |= ZE_DF_LIGHT_SOURCE;

	Component->SetOwner(this);
	Component->Initialize();
	Components.Add(Component);
}

void ZEEntity::UnregisterComponent(ZEComponent* Component)
{
	Component->Deinitialize();
	Components.DeleteValue(Component);

	if (DrawFlags & ZE_DF_AUTO)
	{
		if (Component->GetDrawFlags() & ZE_DF_DRAW)
		{
			bool Drawable = false;
			for (size_t I = 0; I < Components.GetCount(); I++)
				if (Components[I]->GetDrawFlags() & ZE_DF_DRAW)
				{
					Drawable = true;
					break;
				}
			if (Drawable)
				DrawFlags |= ZE_DF_DRAW_COMPONENTS;
			else
				DrawFlags &= !ZE_DF_DRAW_COMPONENTS;
		}


		if (Component->GetDrawFlags() & ZE_DF_LIGHT_SOURCE)
		{
			bool HasLight = false;
			for (size_t I = 0; I < Components.GetCount(); I++)
				if (Components[I]->GetDrawFlags() & ZE_DF_LIGHT_SOURCE)
				{
					HasLight = true;
					break;
				}

			if (HasLight)
				DrawFlags |= ZE_DF_LIGHT_SOURCE;
			else
				DrawFlags &= !ZE_DF_LIGHT_SOURCE;
		}
	}
}

void ZEEntity::SetEntityId(int EntityId)
{
	this->EntityId = EntityId;
}

int ZEEntity::GetEntityId() const
{
	return EntityId;
}

const ZEArray<ZEComponent *> &	ZEEntity::GetComponents()
{
	return Components; 
}

const ZEAABoundingBox &	 ZEEntity::GetWorldBoundingBox()
{
	if (UpdateBoundingBox)
	{
		bool NoBoundingBox = true;

		if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USELOCALONLY)
		{
			ZEVector3 Point;
			ZEMatrix4x4 WorldTransform = GetWorldTransform();
			ZEMatrix4x4::Transform(Point, WorldTransform, LocalBoundingBox.GetVertex(0));
			WorldBoundingBox.Min = WorldBoundingBox.Max = Point;
			for (int I = 1; I < 8; I++)
			{
				ZEMatrix4x4::Transform(Point, WorldTransform, LocalBoundingBox.GetVertex(I));
				if (Point.x < WorldBoundingBox.Min.x) WorldBoundingBox.Min.x = Point.x;
				if (Point.y < WorldBoundingBox.Min.y) WorldBoundingBox.Min.y = Point.y;
				if (Point.z < WorldBoundingBox.Min.z) WorldBoundingBox.Min.z = Point.z;

				if (Point.x > WorldBoundingBox.Max.x) WorldBoundingBox.Max.x = Point.x;
				if (Point.y > WorldBoundingBox.Max.y) WorldBoundingBox.Max.y = Point.y;
				if (Point.z > WorldBoundingBox.Max.z) WorldBoundingBox.Max.z = Point.z;
			}
			NoBoundingBox = false;
		}

		if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USECOMPONENTS)
			for (size_t I = 0; I < Components.GetCount(); I++)
			{
				if ((Components[I]->GetDrawFlags() & ZE_DF_DRAW) && Components[I]->GetVisible())
				{
					const ZEAABoundingBox& CompBoundingBox = Components[I]->GetWorldBoundingBox();
					if (NoBoundingBox == true)
					{
						WorldBoundingBox.Min = WorldBoundingBox.Max = CompBoundingBox.GetVertex(0);
						NoBoundingBox = false;
					}

					for (int N = 0; N < 8; N++)
					{
						ZEVector3 Point = CompBoundingBox.GetVertex(N);
						if (Point.x < WorldBoundingBox.Min.x) WorldBoundingBox.Min.x = Point.x;
						if (Point.y < WorldBoundingBox.Min.y) WorldBoundingBox.Min.y = Point.y;
						if (Point.z < WorldBoundingBox.Min.z) WorldBoundingBox.Min.z = Point.z;

						if (Point.x > WorldBoundingBox.Max.x) WorldBoundingBox.Max.x = Point.x;
						if (Point.y > WorldBoundingBox.Max.y) WorldBoundingBox.Max.y = Point.y;
						if (Point.z > WorldBoundingBox.Max.z) WorldBoundingBox.Max.z = Point.z;
					}
				}
			}

		if (NoBoundingBox == true)
			WorldBoundingBox.Max = WorldBoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);

		UpdateBoundingBox = false;
	}

	return WorldBoundingBox;
}

const ZEBoundingSphere&	ZEEntity::GetWorldBoundingSphere()
{
	if (UpdateBoundingSphere)
	{
		GetWorldBoundingBox().GenerateBoundingSphere(WorldBoundingSphere);
		UpdateBoundingSphere = false;
	}

	return WorldBoundingSphere;
}

ZEDWORD ZEEntity::GetDrawFlags() const
{
	return DrawFlags;
}

void ZEEntity::SetName(const char* NewName)
{
	strncpy(Name, NewName, ZE_MAX_NAME_SIZE);
}

const char* ZEEntity::GetName() const
{
	return Name;
}

void ZEEntity::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEEntity::GetVisible() const
{
	return Visible;
}

void ZEEntity::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEEntity::GetEnabled() const
{
	return Enabled;
}

void ZEEntity::SetPosition(const ZEVector3& NewPosition) 
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Position = NewPosition;
	UpdateComponents();
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation) 
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Rotation = NewRotation;
	UpdateComponents();
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
}

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Scale = NewScale;
	UpdateComponents();
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform()
{
	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::CreateOrientation(WorldTransform, Position, Rotation, Scale);
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

void ZEEntity::SetVelocity(const ZEVector3& NewVelocity)
{
	Velocity = NewVelocity;
}

const ZEVector3& ZEEntity::GetVelocity() const
{
	return Velocity;
}

void ZEEntity::Initialize()
{
}

void ZEEntity::Deinitialize()
{
}

void ZEEntity::Destroy()
{
	delete this;
}

void ZEEntity::Reset()
{
	Deinitialize();
	Initialize();
}

void ZEEntity::UpdateBoundingVolumes()
{
	if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USECOMPONENTS)
	{
		UpdateBoundingSphere = true;
		UpdateBoundingBox = true;
	}
}

bool ZEEntity::CastRay(const ZERay & Ray,const float Range,float &MinT)
{
	float MaxT,CurrMinT = Range;

	if (!ZEAABoundingBox::IntersectionTest(GetWorldBoundingBox(), Ray, MinT, MaxT))
		return false;

	if (BoundingVolumeMechanism != ZE_BVM_USECOMPONENTS)
		return true;

	for (size_t I = 0; I < Components.GetSize(); I++)
	{
		ZERay LocalRay;
		ZEMatrix4x4 InvWorldTransform;
		ZEMatrix4x4::Inverse(InvWorldTransform, Components[I]->GetWorldTransform());
		
		ZEMatrix4x4::Transform(LocalRay.p, InvWorldTransform, Ray.p);
		ZEMatrix4x4::Transform3x3(LocalRay.v, InvWorldTransform, Ray.p);
		if (ZEAABoundingBox::IntersectionTest(Components[I]->GetLocalBoundingBox(), LocalRay, MinT,MaxT))
			if (CurrMinT > MinT)
				CurrMinT = MinT;		
	}

	if (Range < CurrMinT) 
		return false;

	MinT = CurrMinT;
	return true;
}

void ZEEntity::Tick(float Time)
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Tick(Time);

	ZEVector3::Sub(Velocity, Position, OldPosition);
	ZEVector3::Scale(Velocity, Velocity, 1.0f / Time);
	OldPosition = Position;
}

void ZEEntity::Update()
{

}

void ZEEntity::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
}

ZEEntity::ZEEntity()
{
	Name[0] = '\0';
	DrawFlags = ZE_DF_AUTO;
	BoundingVolumeMechanism = ZE_BVM_USELOCALONLY;
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Scale = ZEVector3(1.0f, 1.0f ,1.0f);
	Enabled = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	UpdateWorldTransform = true;
	Visible = true;
}

ZEEntity::~ZEEntity()
{
	ZEWARNINGASSERT(Components.GetCount() != 0, "Entity : %s. There are uninitialized component available.", GetClassDescription()->GetName());
}

#include "Entity.h.zpp"

ZEEntityRunAt ZEEntityDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
