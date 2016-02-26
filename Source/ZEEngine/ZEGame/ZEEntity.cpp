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

#include "ZEError.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZEMeta/ZEProvider.h"

#include "ZEScene.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNView.h"

#define ZE_EDF_LOCAL_TRANSFORM					0x0001
#define ZE_EDF_INV_LOCAL_TRANSFORM				0x0002
#define ZE_EDF_WORLD_TRANSFORM					0x0004
#define ZE_EDF_INV_WORLD_TRANSFORM				0x0008
#define ZE_EDF_WORLD_BOUNDING_BOX				0x0010

void ZEEntity::LocalTransformChanged()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_LOCAL_TRANSFORM | ZE_EDF_INV_LOCAL_TRANSFORM	|
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	|
		ZE_EDF_WORLD_BOUNDING_BOX);

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->ParentTransformChanged();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->ParentTransformChanged();
}

void ZEEntity::ParentTransformChanged()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	|
		ZE_EDF_WORLD_BOUNDING_BOX);

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->ParentTransformChanged();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->ParentTransformChanged();
}

void ZEEntity::BoundingBoxChanged()
{
	EntityDirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
}

bool ZEEntity::AddComponent(ZEEntity* Entity)
{
	if (Entity->Parent != NULL)
	{
		zeError("Component already has an owner. Can not register component.");
		return false;
	}

	if (!(Entity->Scene == NULL || Entity->Scene == this->Scene))
	{
		zeError("Component already has been entitled to another scene. Can not register component.");
		return false;
	}

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	if (!Entity->SetParent(this))
		return false;

	Entity->SetScene(this->Scene);

	Components.Add(Entity);

	if (State == ZE_ES_LOADED || State == ZE_ES_LOADING)
		Entity->Load();
	else if (State == ZE_ES_INITIALIZING || State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	if(Entity->GetName().IsEmpty())
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

	Entity->Deinitialize();
	Components.RemoveValue(Entity);

	Entity->Parent = NULL;
	Entity->SetScene(NULL);
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

	if (!Entity->SetParent(this))
		return false;

	Entity->SetScene(this->Scene);
	ChildEntities.Add(Entity);

	if (State == ZE_ES_LOADED || State == ZE_ES_LOADING)
		Entity->Load();
	else if (State == ZE_ES_INITIALIZED || State == ZE_ES_INITIALIZING)
		Entity->Initialize();

	if(Entity->GetName().IsEmpty())
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

	ChildEntities.RemoveValue(Entity);

	Entity->Parent = NULL;
	Entity->SetScene(NULL);
}

bool ZEEntity::SetParent(ZEEntity* Owner)
{
	this->Parent = Owner;
	ParentTransformChanged();
	return true;
}

void ZEEntity::SetScene(ZEScene* Scene)
{
	this->Scene = Scene;

	ZESize SubItemCount = Components.GetCount();
	for (ZESize I = 0; I < SubItemCount; I++)
		Components[I]->SetScene(Scene);

	SubItemCount = ChildEntities.GetCount();
	for (ZESize I = 0; I < SubItemCount; I++)
		ChildEntities[I]->SetScene(Scene);
}

void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	BoundingBoxChanged();
}

ZETaskResult ZEEntity::ManageStates(ZETaskThread* Thread, void* Parameters)
{
	/*zeLog("%s::ManageStates, State: %s, TargetState:%s", 
		GetName().ToCString(), 
		ZEEntityState_Declaration()->ToText(State, "Unknown"), 
		ZEEntityState_Declaration()->ToText(TargetState, "Unknown"));*/

	if (State == ZE_ES_NONE)
	{
		if (TargetState == ZE_ES_LOADED || TargetState == ZE_ES_INITIALIZED)
		{
			State = ZE_ES_LOADING;
			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->Load();

			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->Load();

			return ZE_TR_COOPERATING;
		}
	}
	else if (State == ZE_ES_LOADED)
	{
		if (TargetState == ZE_ES_NONE)
		{
			State = ZE_ES_UNLOADING;

			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->Unload();

			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->Unload();


			return ZE_TR_COOPERATING;
		}
		else if (TargetState == ZE_ES_INITIALIZED)
		{
			State = ZE_ES_INITIALIZING;

			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->Initialize();

			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->Initialize();

			return ZE_TR_COOPERATING;
		}
	}
	else if (State == ZE_ES_INITIALIZED)
	{
		if (TargetState == ZE_ES_LOADED || TargetState == ZE_ES_NONE)
		{
			State = ZE_ES_DEINITIALIZING;

			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->Deinitialize();

			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->Deinitialize();

			return ZE_TR_COOPERATING;
		}
	}
	else if (State == ZE_ES_LOADING)
	{
		ZEEntityResult Result = LoadInternal();
		if (Result == ZE_ER_DONE)
		{
			State = ZE_ES_LOADED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else
		{
			State = ZE_ES_ERROR_LOADING;
			return ZE_TR_FAILED;
		}
	}
	else if (State == ZE_ES_UNLOADING)
	{
		ZEEntityResult Result = UnloadInternal();
		if (Result == ZE_ER_DONE)
		{
			State = ZE_ES_NOT_LOADED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else
		{
			State = ZE_ES_ERROR_UNLOADING;
			return ZE_TR_FAILED;
		}
	}
	else if (State == ZE_ES_INITIALIZING)
	{
		ZEEntityResult Result = InitializeInternal();
		if (Result == ZE_ER_DONE)
		{
			State = ZE_ES_INITIALIZED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else
		{
			State = ZE_ES_ERROR_INITIALIZATION;
			return ZE_TR_FAILED;
		}
	}
	else if (State == ZE_ES_DEINITIALIZING)
	{
		ZEEntityResult Result = DeinitializeInternal();
		if (Result == ZE_ER_DONE)
		{
			State = ZE_ES_LOADED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else
		{
			State = ZE_ES_ERROR_DEINITIALIZATION;
			return ZE_TR_FAILED;
		}
	}

	return ZE_TR_DONE;
}

void ZEEntity::ManagetStatesSerial()
{
	while (ManageStates(NULL, 0) == ZE_TR_COOPERATING);
}

void ZEEntity::SetSerialOperation(bool SerialOperation)
{
	this->SerialOperation = SerialOperation;
}

bool ZEEntity::GetSerialOperation() const 
{
	return SerialOperation;
}

bool ZEEntity::InitializeSelf()
{
	return true;
}

bool ZEEntity::DeinitializeSelf()
{
	return true;
}

ZEEntityResult ZEEntity::LoadInternal()
{
	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::UnloadInternal()
{
	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::InitializeInternal()
{
	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::DeinitializeInternal()
{
	return ZE_ER_DONE;
}

ZEEntity::ZEEntity()
{
	Parent = NULL;
	Scene = NULL;
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Enabled = true;
	Visible = true;
	SerialOperation = true;
	State = ZE_ES_NOT_INITIALIZED;
	ManageTask.SetFunction(ZEDelegateMethod(ZETaskFunction, ZEEntity, ManageStates, this));
	LocalTransformChanged();
}

ZEEntity::~ZEEntity()
{
	Unload();

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Destroy();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->Destroy();
}

ZEDrawFlags ZEEntity::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

ZEEntity* ZEEntity::GetParent() const
{
	return Parent;
}

ZEScene* ZEEntity::GetScene() const
{
	return Scene;
}

const ZEArray<ZEEntity*>& ZEEntity::GetComponents() const
{
	return Components;
}

const ZEArray<ZEEntity*>& ZEEntity::GetChildEntities() const
{
	return ChildEntities; 
}

const ZEAABBox& ZEEntity::GetBoundingBox() const
{
	return BoundingBox;
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

const ZEMatrix4x4& ZEEntity::GetTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, GetPosition(), GetRotation(), GetScale());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform() const
{
	if (Parent == NULL)
	{
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
		return GetTransform();
	}

	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(WorldTransform, Parent->GetWorldTransform(), GetTransform());
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

void ZEEntity::SetVisible(bool Visibility)
{
	this->Visible = Visibility;
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
	Position = NewPosition;
	LocalTransformChanged();
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
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

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	LocalTransformChanged();
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
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
	{
		SetRotation(NewRotation);
	}
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

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	LocalTransformChanged();
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
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
	{
		SetScale(NewScale);
	}
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

ZEVector3 ZEEntity::GetFront() const
{
	return Rotation * ZEVector3::UnitZ;
}

ZEVector3 ZEEntity::GetRight() const
{
	return Rotation * ZEVector3::UnitX;
}

ZEVector3 ZEEntity::GetUp() const
{
	return Rotation * ZEVector3::UnitY;
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

ZEEntityState ZEEntity::GetState() const
{
	return State;
}

bool ZEEntity::IsLoaded() const
{
	return State >= ZE_ES_LOADED && State != ZE_ES_ERROR_LOADING && State != ZE_ES_ERROR_UNLOADING;
}

bool ZEEntity::IsInitialized() const
{
	return State == ZE_ES_INITIALIZED;
}

void ZEEntity::Initialize()
{
	if (TargetState == ZE_ES_INITIALIZED)
		return;

	TargetState = ZE_ES_INITIALIZED;

	if (SerialOperation)
		ManagetStatesSerial();
	else
		ManageTask.Run();
	
}

void ZEEntity::Deinitialize()
{
	if (TargetState == ZE_ES_DEINITIALIZED)
		return;

	if (TargetState >= ZE_ES_LOADED)
		TargetState = ZE_ES_LOADED;
	else
		TargetState = ZE_ES_NONE;

	if (SerialOperation)
		ManagetStatesSerial();
	else
		ManageTask.Run();
}

void ZEEntity::Load()
{
	if (TargetState == ZE_ES_LOADED)
		return;

	TargetState = ZE_ES_LOADED;
	
	if (SerialOperation)
		ManagetStatesSerial();
	else
		ManageTask.Run();
}

void ZEEntity::Unload()
{
	if (TargetState == ZE_ES_NONE)
		return;

	TargetState = ZE_ES_NONE;
	
	if (SerialOperation)
		ManagetStatesSerial();
	else
		ManageTask.Run();
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

	ZEDrawFlags Flags = GetDrawFlags();
	if (!Flags.GetFlags(ZE_DF_DRAW))
		return false;

	//two times cull test
	//if (Flags.GetFlags(ZE_DF_CULL) && CullParameters->View->ViewVolume->CullTest(GetWorldBoundingBox()))
		//return false;

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
	if (Serializer == NULL)
		return false;

	ZEMLWriterNode PropertiesNode; 
	Serializer->OpenNode("Properties", PropertiesNode);
	
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

	if (ChildEntities.GetCount() != 0)
	{
		ZEMLWriterNode SubEntitiesNode, EntityNode;
		Serializer->OpenNode("ChildEntities", SubEntitiesNode);

		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			SubEntitiesNode.OpenNode("Entity", EntityNode);
			EntityNode.WriteString("Class", ChildEntities[I]->GetClass()->GetName());
			ChildEntities[I]->Save(&EntityNode);
			EntityNode.CloseNode();
		}

		SubEntitiesNode.CloseNode();
	}
	
	return true;
}

bool ZEEntity::Restore(ZEMLReaderNode* Unserializer)
{
	if (Unserializer == NULL)
		return false;

	ZEMLReaderNode PropertiesNode = Unserializer->GetNode("Properties");

	if (!PropertiesNode.IsValid())
		return false;

	const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();

	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].ElementType != ZEML_ET_PROPERTY)
			continue;

		if (!GetClass()->SetProperty(this, Elements[I].Name, ZEVariant(Elements[I].Value)))
			zeWarning("Cannot restore property. Entity: \"%s\", Property: \"%s\".", GetClass()->GetName(), Elements[I].Name.ToCString());
	}

	ZEMLReaderNode SubEntitiesNode = Unserializer->GetNode("ChildEntities");

	if (!SubEntitiesNode.IsValid())
		return true;

	ChildEntities.SetCount(SubEntitiesNode.GetNodeCount("Entity"));

	ZEClass* NewSubEntityClass = NULL;
	ZEEntity* NewSubEntity = NULL;

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
	{
		ZEMLReaderNode SubEntityNode = SubEntitiesNode.GetNode("Entity", I);

		NewSubEntityClass = ZEProvider::GetInstance()->GetClass(SubEntityNode.ReadString("Class"));

		if (NewSubEntityClass == NULL)
		{
			zeError("Unserialization failed. Child Entity class is not registered. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
			return false;
		}

		NewSubEntity = (ZEEntity*)NewSubEntityClass->CreateInstance();

		if (NewSubEntity == NULL)
		{
			zeError("Unserialization failed. Cannot create instance of a child entity. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
			NewSubEntity->Destroy();
			return false;
		}

		if (!NewSubEntity->Restore(&SubEntityNode))
		{
			zeError("Unserialization failed. Unserialization of child entity has failed. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
			NewSubEntity->Destroy();
			return false;
		}

		ChildEntities[I] = NewSubEntity;

		if (!NewSubEntity->SetParent(this))
		{
			zeError("Unserialization failed. Cannot add child entity. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
			ChildEntities[I] = NULL;
			NewSubEntity->Destroy();
			return false;
		}
	}

	return true;
}
