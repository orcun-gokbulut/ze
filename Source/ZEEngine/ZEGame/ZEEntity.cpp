//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEntity.cpp
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

#include "ZEEntity.h"

#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMath.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZEError.h"
#include "ZEScene.h"
#include "ZEEntityProvider.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNView.h"
#include "ZEMath/ZEViewVolume.h"

#include <string.h>

#define ZE_EDF_TRANSFORM					0x01
#define ZE_EDF_INV_TRANSFORM				0x02
#define ZE_EDF_WORLD_TRANSFORM				0x04
#define ZE_EDF_INV_WORLD_TRANSFORM			0x08
#define ZE_EDF_WORLD_BOUNDING_BOX			0x10

void ZEEntity::ParentChanged()
{
	Scene = (Parent != NULL ? Parent->GetScene() : NULL);

	TransformChangedWorld();

	ze_for_each(Component, Components)
		Component->ParentChanged();

	ze_for_each(ChildEntity, ChildEntities)
		ChildEntity->ParentChanged();
}

void ZEEntity::TransformChangedLocal()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_TRANSFORM | ZE_EDF_INV_TRANSFORM |
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	| ZE_EDF_WORLD_BOUNDING_BOX);

	ze_for_each(Component, Components)
		Component->TransformChangedWorld();
	
	ze_for_each(ChildEntity, ChildEntities)
		ChildEntity->TransformChangedWorld();
}

void ZEEntity::TransformChangedWorld()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	| ZE_EDF_WORLD_BOUNDING_BOX);

	ze_for_each(Component, Components)
		Component->TransformChangedWorld();

	ze_for_each(ChildEntity, ChildEntities)
		ChildEntity->TransformChangedWorld();
}

const ZEList2<ZEEntity>& ZEEntity::GetComponents() const
{
	return Components;
}

bool ZEEntity::AddComponent(ZEEntity* Entity)
{
	zeCheckError(Entity->Parent != NULL, false, "Component already has an owner. Can not register component.");
	zeCheckError(!(Entity->Scene == NULL || Entity->Scene == this->Scene), false, "Component already has been entitled to another scene. Can not register component.");

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	Components.AddEnd(&Entity->ParentLink);

	Entity->Parent = this;
	Entity->ParentChanged();

	if (State == ZE_ES_INITIALIZING || State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(GetName() + "->" +  Entity->GetClass()->GetName());

	return true;
}

void ZEEntity::RemoveComponent(ZEEntity* Entity)
{
	if (Entity->Parent != this)
	{
		zeError("Can not remove non-component entity.");
		return;
	}

	Entity->Parent = NULL;
	Entity->ParentChanged();

	Components.Remove(&Entity->ParentLink);
}


bool ZEEntity::InitializeSelf()
{
	State = ZE_ES_INITIALIZING;
	return true;
}

void ZEEntity::DeinitializeSelf()
{
	State = ZE_ES_NOT_INITIALIZED;
}

bool ZEEntity::LoadSelf()
{
	LoadingState = ZE_ELS_LOADING;
}

void ZEEntity::UnloadSelf()
{
	zeDebugCheck(Resources.GetCount() != 0, "There are still resources associated with this entity.");
	LoadingState = ZE_ELS_NOT_LOADED;
}

ZEEntity::ZEEntity() : ParentLink(this)
{
	Parent = NULL;
	Scene = NULL;
	State = ZE_ES_NOT_INITIALIZED;
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Transform = ZEMatrix4x4::Identity;
	InvTransform = ZEMatrix4x4::Identity;
	WorldTransform = ZEMatrix4x4::Identity;
	InvWorldTransform = ZEMatrix4x4::Identity;
	BoundingBox = ZEAABBox::Zero;
	WorldBoundingBox = ZEAABBox::Zero;
	Enabled = true;
	Visible = true;
}

ZEEntity::~ZEEntity()
{
	Deinitialize();
	Unload();

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Destroy();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->Destroy();
}

ZEEntity* ZEEntity::GetParent() const
{
	return Parent;
}

ZEScene* ZEEntity::GetScene() const
{
	return Scene;
}

ZEEntityState ZEEntity::GetInitalizationState() const
{
	return State;
}

ZEEntityFlags ZEEntity::GetFlags() const
{
	return EntiyFlags;
}

void ZEEntity::SetEntityId(ZEInt EntityId)
{
	this->EntityId = EntityId;
}

ZEInt ZEEntity::GetEntityId() const
{
	return EntityId;
}

void ZEEntity::SetName(ZEString NewName)
{
	Name = NewName;
}

ZEString ZEEntity::GetName() const
{
	return Name;
}

void ZEEntity::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
	TransformChangedLocal();
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	TransformChangedLocal();
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
}

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	TransformChangedLocal();
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
}

void ZEEntity::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEEntity::GetEnabled() const
{
	return Enabled;
}

void ZEEntity::SetVisible(bool Visibility)
{
	this->Visible = Visibility;
}

bool ZEEntity::GetVisible() const
{
	return Visible;
}
void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	EntityDirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
}

const ZEAABBox& ZEEntity::GetBoundingBox() const
{
	return BoundingBox;
}

const ZEMatrix4x4& ZEEntity::GetTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, GetPosition(), GetRotation(), GetScale());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEEntity::GetInvTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvTransform, GetTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_INV_TRANSFORM);
	}

	return InvTransform;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
	{
		if (Parent == NULL)
		{
			ZEMatrix4x4::Multiply(WorldTransform, Parent->GetWorldTransform(), GetTransform());
		}
		else
		{
			WorldTransform = GetTransform();
		}
		
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
	}

	return WorldTransform;

}

const ZEMatrix4x4& ZEEntity::GetInvWorldTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_INV_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_INV_WORLD_TRANSFORM);
	}

	return InvWorldTransform;
}

ZEVector3 ZEEntity::GetFront()
{
	return Rotation * ZEVector3::UnitZ;
}

ZEVector3 ZEEntity::GetRight()
{
	return Rotation * ZEVector3::UnitX;
}

ZEVector3 ZEEntity::GetUp()
{
	return Rotation * ZEVector3::UnitY;
}

void ZEEntity::SetWorldPosition(const ZEVector3& NewPosition)
{
	if (Parent != NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Parent->GetInvWorldTransform(), NewPosition);
		SetPosition(Result);
	}
	else
	{
		SetPosition(NewPosition);
	}
}

const ZEVector3 ZEEntity::GetWorldPosition() const
{
	if (Parent != NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Parent->GetWorldTransform(), GetPosition());
		return Temp;
	}

	return GetPosition();
}

void ZEEntity::SetWorldRotation(const ZEQuaternion& NewRotation)
{
	if (Parent != NULL)
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Parent->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, NewRotation);
		SetRotation(Result);
	}
	else
		SetRotation(NewRotation);
}

const ZEQuaternion ZEEntity::GetWorldRotation() const
{
	if (Parent != NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Parent->GetWorldRotation(), GetRotation());
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}

	return GetRotation();
}

void ZEEntity::SetWorldScale(const ZEVector3& NewScale)
{
	if (Parent != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, NewScale, Parent->GetWorldScale());
		SetScale(Temp);
	}
	else
		SetScale(NewScale);
}

const ZEVector3 ZEEntity::GetWorldScale() const
{
	if (Parent != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Parent->GetWorldScale(), GetScale());
		return Temp;
	}

	return GetScale();
}

const ZEAABBox& ZEEntity::GetWorldBoundingBox() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, GetBoundingBox(), GetWorldTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

ZEVector3 ZEEntity::GetWorldFront() const
{
	return GetWorldRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEEntity::GetWorldRight() const
{
	return GetWorldRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEEntity::GetWorldUp() const
{
	return GetWorldRotation() * ZEVector3::UnitY;
}

const ZEArray<ZEEntity*>& ZEEntity::GetChildEntities() const
{
	return ChildEntities; 
}

bool ZEEntity::AddChildEntity(ZEEntity* Entity)
{
	if (Entity->Parent != NULL)
	{
		zeError("Entity already has an owner. Can not register entity.");
		return false;
	}

	if (!(Entity->Scene == NULL || Entity->Scene == this->Scene))
	{
		zeError("Child entity already has been entitled to another scene. Can not register child entity.");
		return false;
	}

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	ChildEntities.Add(Entity);

	Entity->Parent = this;
	Entity->ParentChanged();

	if (State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(GetName() + "->" +  Entity->GetClass()->GetName());

	return true;
}

void ZEEntity::RemoveChildEntity(ZEEntity* Entity)
{
	if (Entity->Parent != this)
	{
		zeError("Can not remove non-child entity.");
		return;
	}

	Entity->Parent = NULL;
	Entity->ParentChanged();

	ChildEntities.RemoveValue(Entity);
}

bool ZEEntity::IsInitialized()
{
	return (State == ZE_ES_INITIALIZED);
}

bool ZEEntity::Initialize()
{
	if (IsInitialized())
		return false;

	for (ZESize I = 0; I < Components.GetCount(); I++)
		if(!Components[I]->Initialize())
			return false;

	if (!InitializeSelf())
		return false;

	if (State != ZE_ES_INITIALIZING)
		return false;

	zeDebugCheck(State != ZE_ES_INITIALIZED, "This entity class has broken InitializeSelf chain problem.");

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		if(!ChildEntities[I]->Initialize())
			return false;

	return true;
}

void ZEEntity::Deinitialize()
{
	if (!IsInitialized())
		return;
	
	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->Deinitialize();

	State = ZE_ES_DEINITIALIZING;

	DeinitializeSelf();

	zeDebugCheck(State != ZE_ES_NOT_INITIALIZED, "This entity class has broken DeinitializeSelf chain problem.");

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();
}

bool ZEEntity::Load()
{
	if (LoadingState == ZE_ELS_LOADING || 
		LoadingState == ZE_ELS_LOADED ||  
		LoadingState == ZE_ELS_SEMI_LOADED)
	{
		return false;
	}

	ze_for_each(Component, Components)
		if(!Component->Load())
			return false;

	if (!LoadSelf())
		return false;

	zeDebugCheck(
		LoadingState != ZE_ELS_LOADING && 
		LoadingState != ZE_ELS_LOADED &&  
		LoadingState != ZE_ELS_SEMI_LOADED, "This entity class has broken LoadSelf chain problem.");

	ze_for_each(ChildEntity, ChildEntities)
		if(!ChildEntity->Load())
			return false;

	return true;
}

void ZEEntity::Unload()
{
	if (LoadingState != ZE_ELS_UNLOADING)
		return;

	LoadingState = ZE_ELS_UNLOADING;

	ze_for_each(ChildEntity, ChildEntities)
		ChildEntity->Unload();

	UnloadSelf();

	zeDebugCheck(LoadingState != ZE_ELS_NOT_LOADED, "This entity class has broken UnloadSelf chain problem.");

	ze_for_each(ChildEntity, ChildEntities)
		ChildEntity->Deinitialize();
}

void ZEEntity::Destroy()
{
	delete this;
}

void ZEEntity::Tick(float Time)
{

}

bool ZEEntity::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!GetVisible())
		return false;

	if (!EntityDirtyFlags.GetFlags(ZE_EF_RENDERABLE))
		return false;

	return true;
}

void ZEEntity::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	
}

bool ZEEntity::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (!ZEAABBox::IntersectionTest(GetWorldBoundingBox(), Parameters.Ray))
		return false;

	ZERay LocalRay;
	ZEMatrix4x4::Transform(LocalRay.p, GetInvWorldTransform(), Parameters.Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, GetInvWorldTransform(), Parameters.Ray.v);
	LocalRay.v.NormalizeSelf();

	float TMin;
	if (!ZEAABBox::IntersectionTest(BoundingBox, LocalRay, TMin))
		return false;

	ZEVector3 IntersectionPoint;
	ZEMatrix4x4::Transform(IntersectionPoint, GetWorldTransform(), LocalRay.GetPointOn(TMin));
	float DistanceSquare = ZEVector3::DistanceSquare(Parameters.Ray.p, IntersectionPoint);
	if (Report.Distance * Report.Distance > DistanceSquare && DistanceSquare < Parameters.MaximumDistance * Parameters.MaximumDistance)
	{
		Report.Position = IntersectionPoint;
		Report.Distance = ZEMath::Sqrt(DistanceSquare);
		Report.Entity = this;
		Report.SubComponent = NULL;
		Report.PoligonIndex = 0;
		Report.Normal = ZEVector3::Zero;
		Report.Binormal = ZEVector3::Zero;
		
		return true;
	}

	return false;
}


bool ZEEntity::Save(ZEMLWriterNode* Serializer)
{
	ZEMLWriterNode EntityNode;
	Serializer->OpenNode("Entity", EntityNode);
		EntityNode.WriteString("Class", GetClass()->GetName());
		ZEMLWriterNode PropertiesNode; 
		EntityNode.OpenNode("Properties", PropertiesNode);
			const ZEProperty* Properties = GetClass()->GetProperties();
			for (ZESize I = 0; I < GetClass()->GetPropertyCount(); I++)
			{
				const ZEProperty* Current = &Properties[I];
				if (Current->Type.ContainerType != ZE_CT_NONE)
					continue;

				if (Current->Type.TypeQualifier != ZE_TQ_VALUE)
					continue;

				if (Current->Type.Type == ZE_TT_OBJECT || Current->Type.Type == ZE_TT_OBJECT_PTR)
					continue;

				if ((Current->Access & ZEMT_PA_READ_WRITE) != ZEMT_PA_READ_WRITE)
					continue;

				ZEVariant Variant;
				GetClass()->GetProperty(this, Current->ID, Variant);

				ZEValue Value = Variant.GetValue();
				if (Value.IsNull())
					continue;

				PropertiesNode.WriteValue(Current->Name, Value);
			}
		PropertiesNode.CloseNode();
	EntityNode.CloseNode();
	
	return true;
}

bool ZEEntity::Restore(ZEMLReaderNode* Unserializer)
{
	if (Unserializer->GetName() != "Entity")
		return false;

	ZEMLReaderNode PropertiesNode = Unserializer->GetNode("Properties");
	const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();

	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].ElementType != ZEML_ET_PROPERTY)
			continue;


		if (!GetClass()->SetProperty(this, Elements[I].Name, ZEVariant(Elements[I].Value)))
			zeWarning("Cannot restore property. Entity: \"%s\", Property: \"%s\".", GetClass()->GetName(), Elements[I].Name.ToCString());
	}

	return true;
}
