//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - CompoundEntity.cpp
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

#include "CompoundEntity.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Ray.h"
#include "Core/Error.h"
#include "EntityComponent.h"
#include <string.h>

void ZECompoundEntity::SetBoundingVolumeMechanism(ZEBoundingVolumeMechnism Mechanism)
{
	BoundingVolumeMechanism = Mechanism;
	DirtyFlags |= ZE_EDF_WORLD_BOUNDING_BOX | ZE_EDF_WORLD_BOUNDING_SPHERE;
}

void ZECompoundEntity::UpdateComponents()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->OwnerWorldTransformChanged();
}

void ZECompoundEntity::RegisterComponent(ZEEntityComponent* Component)
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

	zeAssert(Component->Owner != NULL, "Component already has a owner. Can not register component.");

	Component->Owner = this;
	Component->Initialize();
	Components.Add(Component);
}

void ZECompoundEntity::UnregisterComponent(ZEEntityComponent* Component)
{
	Component->Deinitialize();
	Components.DeleteValue(Component);
	Component->Owner = NULL;

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
		
ZEDWORD ZECompoundEntity::GetRayCastFlags() const
{
	return 0;
}

ZEEntityType ZECompoundEntity::GetEntityType()
{
	return ZE_ET_COMPOUND;
}

const ZEArray<ZEEntityComponent*>& ZECompoundEntity::GetComponents()
{
	return Components; 
}

const ZEAABoundingBox &	 ZECompoundEntity::GetWorldBoundingBox()
{
	if (DirtyFlags & ZE_EDF_WORLD_BOUNDING_BOX)
	{
		bool NoBoundingBox = true;

		if (BoundingVolumeMechanism == ZE_BVM_USE_BOTH || BoundingVolumeMechanism == ZE_BVM_USE_LOCAL_ONLY)
		{
			const ZEMatrix4x4& WorldTransform = GetWorldTransform();
			const ZEAABoundingBox& LocalBoundingBox = GetLocalBoundingBox();

			ZEVector3 Point;
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

		if (BoundingVolumeMechanism == ZE_BVM_USE_BOTH || BoundingVolumeMechanism == ZE_BVM_USE_COMPONENTS)
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

		DrawFlags &= ~ZE_EDF_WORLD_BOUNDING_BOX;
	}

	return WorldBoundingBox;
}

ZEDWORD ZECompoundEntity::GetDrawFlags() const
{
	return DrawFlags;
}
void ZECompoundEntity::SetPosition(const ZEVector3& NewPosition) 
{
	ZEEntity::SetPosition(NewPosition);
	UpdateComponents();
}


void ZECompoundEntity::SetRotation(const ZEQuaternion& NewRotation) 
{
	ZEEntity::SetRotation(NewRotation);
	UpdateComponents();
}


void ZECompoundEntity::SetScale(const ZEVector3& NewScale)
{
	ZEEntity::SetScale(NewScale);
	UpdateComponents();
}


void ZECompoundEntity::SetVelocity(const ZEVector3& NewVelocity)
{
	ZEEntity::SetVelocity(NewVelocity);
	//Velocity = NewVelocity;
}


bool ZECompoundEntity::Initialize()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Initialize();

	return true;
}

void ZECompoundEntity::Deinitialize()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();
}

/*
void ZECompoundEntity::UpdateBoundingVolumes()
{
	if (BoundingVolumeMechanism == ZE_BVM_USE_BOTH || BoundingVolumeMechanism == ZE_BVM_USE_COMPONENTS)
		DirtyFlags |= ZE_EDF_WORLD_BOUNDING_BOX | ZE_EDF_WORLD_BOUNDING_SPHERE;
}*/
void ZECompoundEntity::Tick(float Time)
{
	ZEEntity::Tick(Time);
	for (size_t I = 0; I < Components.GetCount(); I++)
		if (Components[I]->GetEnabled())
			Components[I]->Tick(Time);
}

void ZECompoundEntity::Draw(ZEDrawParameters* DrawParameters)
{

}

void ZECompoundEntity::UpdateBoundingVolumes()
{
	if (BoundingVolumeMechanism == ZE_BVM_USE_BOTH || BoundingVolumeMechanism == ZE_BVM_USE_COMPONENTS)
		DirtyFlags |= ZE_EDF_WORLD_BOUNDING_BOX | ZE_EDF_WORLD_BOUNDING_SPHERE;
}

ZECompoundEntity::ZECompoundEntity()
{
	DrawFlags = ZE_DF_DRAW | ZE_DF_DRAW_COMPONENTS | ZE_DF_CULL | ZE_DF_CULL_COMPONENTS;
	DirtyFlags = ZE_EDF_ALL;
	BoundingVolumeMechanism = ZE_BVM_USE_BOTH;
}

ZECompoundEntity::~ZECompoundEntity()
{
	Deinitialize();
	zeWarningAssert(Components.GetCount() != 0, "Entity : %s. There are uninitialized component available.", GetClassDescription()->GetName());
}

ZEEntityRunAt ZECompoundEntityDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

#include "CompoundEntity.h.zpp"
