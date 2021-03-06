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

#include "ZEScene.h"

#include "ZEError.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEMeta/ZEMTProvider.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZERenderer/ZERNView.h"

#define ZE_EDF_LOCAL_TRANSFORM					0x0001
#define ZE_EDF_INV_LOCAL_TRANSFORM				0x0002
#define ZE_EDF_WORLD_TRANSFORM					0x0004
#define ZE_EDF_INV_WORLD_TRANSFORM				0x0008
#define ZE_EDF_WORLD_BOUNDING_BOX				0x0010

void ZEEntity::GetComponentsInternal(ZEClass* Class, ZEArray<ZEEntity*>& Output) const
{
	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(Class, Components[I]))
				Output.Add(Components[I]);

			Components[I]->GetComponentsInternal(Class, Output);
		}
	}
	Components.UnlockRead();
}

void ZEEntity::GetComponentsInternal(const ZEString& Name, ZEArray<ZEEntity*>& Output) const
{
	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (Components[I]->GetName() == Name)
				Output.Add(Components[I]);

			Components[I]->GetComponentsInternal(Name, Output);
		}
	}
	Components.UnlockRead();
}

void ZEEntity::GetChildEntitiesInternal(ZEClass* Class, ZEArray<ZEEntity*>& Output) const
{
	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(Class, ChildEntities[I]))
				Output.Add(ChildEntities[I]);

			ChildEntities[I]->GetChildEntitiesInternal(Class, Output);
		}
	}
	ChildEntities.UnlockRead();
}

void ZEEntity::GetChildEntitiesInternal(const ZEString& Name, ZEArray<ZEEntity*>& Output) const
{
	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (ChildEntities[I]->GetName() == Name)
				Output.Add(ChildEntities[I]);

			ChildEntities[I]->GetChildEntitiesInternal(Name, Output);
		}
	}
	ChildEntities.UnlockRead();
}

void ZEEntity::SetWrapper(ZEDObjectWrapper* Wrapper)
{
	this->Wrapper = Wrapper;
}

void ZEEntity::UpdateTickabilityState()
{
	bool Tickable;
	EntityLock.LockNested();
	{
		if (GetScene() == NULL)
		{
			EntityLock.Unlock();
			return;
		}

		Tickable = EnabledFlattened &&
			(GetEntityFlags().GetFlags(ZE_EF_TICKABLE) && IsInitialized() ||
			GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM));
	}
	EntityLock.Unlock();

	if (Tickable)
		GetScene()->AddToTickList(this);
	else
		GetScene()->RemoveFromTickList(this);
}

void ZEEntity::UpdateOctree()
{
	EntityLock.LockNested();
	{
		if (GetScene() != NULL && GetStatic() && IsLoaded() &&
			GetEntityFlags().GetFlags(ZE_EF_CULLABLE) &&
			GetEntityFlags().GetFlags(ZE_EF_STATIC_SUPPORT))
		{
			GetScene()->UpdateOctree(this);
		}
	}
	EntityLock.Unlock();
}

void ZEEntity::UpdateRenderabilityState(bool Forced)
{
	bool Renderable;
	EntityLock.LockNested();
	{
		if (GetScene() == NULL)
		{
			EntityLock.Unlock();
			return;
		}

		Renderable = 
			Visible && 
			VisibleFlattened &&
			(GetEntityFlags().GetFlags(ZE_EF_RENDERABLE) && IsLoaded() ||
			GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM));

	}
	EntityLock.Unlock();

	if (Forced)
		GetScene()->RemoveFromRenderList(this);

	if (Renderable)
		GetScene()->AddToRenderList(this);
	else
		GetScene()->RemoveFromRenderList(this);
}

void ZEEntity::ParentVisibleChanged()
{
	EntityLock.LockNested();
	{
		bool NewState = GetVisible();
		if (GetParent() != NULL)
			NewState &= GetParent()->VisibleFlattened;

		if (VisibleFlattened == NewState)
		{
			EntityLock.Unlock();
			return;
		}

		VisibleFlattened = NewState;
		UpdateRenderabilityState();

		Components.LockRead();
		{
			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->ParentVisibleChanged();
		}
		Components.UnlockRead();

		ChildEntities.LockRead();
		{
			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->ParentVisibleChanged();
		}
		ChildEntities.UnlockRead();

	}
	EntityLock.Unlock();
}

void ZEEntity::ParentEnabledChanged()
{
	EntityLock.LockNested();
	{
		bool NewState = GetEnabled();
		if (GetParent() != NULL)
			NewState &= GetParent()->EnabledFlattened;

		if (EnabledFlattened == NewState)
		{
			EntityLock.Unlock();
			return;
		}

		EnabledFlattened = NewState;
		UpdateTickabilityState();

		Components.LockRead();
		{
			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->ParentEnabledChanged();
		}
		Components.UnlockRead();

		ChildEntities.LockRead();
		{
			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->ParentEnabledChanged();
		}
		ChildEntities.UnlockRead();
	}
	EntityLock.Unlock();
}

ZEDObjectWrapper* ZEEntity::GetWrapper() const
{
	return Wrapper;
}

void ZEEntity::LocalTransformChanged()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_LOCAL_TRANSFORM | ZE_EDF_INV_LOCAL_TRANSFORM	|
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	|
		ZE_EDF_WORLD_BOUNDING_BOX);

	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
			Components[I]->ParentTransformChanged();
	}
	Components.UnlockRead();

	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			ChildEntities[I]->ParentTransformChanged();
	}
	ChildEntities.UnlockRead();

	UpdateOctree();
}

void ZEEntity::ParentTransformChanged()
{
	EntityDirtyFlags.RaiseFlags(
		ZE_EDF_WORLD_TRANSFORM | ZE_EDF_INV_WORLD_TRANSFORM	|
		ZE_EDF_WORLD_BOUNDING_BOX);

	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
			Components[I]->ParentTransformChanged();
	}
	Components.UnlockRead();

	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			ChildEntities[I]->ParentTransformChanged();
	}
	ChildEntities.UnlockRead();

	UpdateOctree();
}

void ZEEntity::BoundingBoxChanged()
{
	EntityDirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);

	UpdateOctree();
}

bool ZEEntity::AddComponent(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, false, "Cannot add component entity. Component Entity is NULL.");
	zeCheckError(Entity->IsDestroyed(), false, "Cannot add child entity. Component Entity is destroyed. (DANGEROUS!!!)");
	zeCheckError(Entity->Parent != NULL, false, 
		"Can not add component entity. Component entity belongs to another Entity. Parent Entity Name: \"%s\", Component Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(Entity->Scene != NULL , false, 
		"Can not add component entity. Component entity already added to a scene. Parent Entity Name: \"%s\", Component Entity Name: \"%s\".",
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(!Entity->CheckParent(this), false, 
		"Can not add component entity. Component entity rejected parent entity. Parent Entity Name: \"%s\", Component Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(!CheckComponent(this), false, 
		"Can not add component entity. Parent entity rejected the component entity. Parent Entity Name: \"%s\", Component Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	Components.LockWriteNested();
	{
		Entity->SetParent(this);
		Entity->SetScene(this->Scene);

		Components.Add(Entity);
	}
	Components.UnlockWrite();

	if (State == ZE_ES_LOADED || State == ZE_ES_LOADING)
		Entity->Load();
	else if (State == ZE_ES_INITIALIZING || State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	return true;
}

void ZEEntity::RemoveComponent(ZEEntity* Entity)
{
	zeCheckError(Entity->Parent != this, ZE_VOID, "Cannot remove non-component entity.");

	Entity->Deinitialize();

	Components.LockWriteNested();
	{
		Entity->SetParent(NULL);
		Entity->SetScene(NULL);
		Components.RemoveValue(Entity);
	}
	Components.UnlockWrite();
}

void ZEEntity::ClearComponents()
{
	Components.LockWriteNested();
	{
		while(Components.GetCount() != 0)
		{
			ZEEntity* Component = Components.GetFirstItem();
			Component->Destroy();
		}
	}
	Components.UnlockWrite();
}

bool ZEEntity::AddChildEntity(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, false, "Cannot add child entity. Child Entity is NULL.");
	zeCheckError(Entity->IsDestroyed(), false, "Cannot add child entity. Entity is destroyed. (DANGEROUS!!!)");
	zeCheckError(Entity->Parent != NULL, false, 
		"Can not add child entity. Child entity belongs to another Entity. Parent Entity Name: \"%s\", Child Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(Entity->Scene != NULL , false, 
		"Can not add child entity. Child entity already added to a scene. Parent Entity Name: \"%s\", Child Entity Name: \"%s\".",
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(!Entity->CheckParent(this), false, 
		"Can not add child entity. Child entity rejected parent entity. Parent Entity Name: \"%s\", Child Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());
	zeCheckError(!CheckChildEntity(this), false, 
		"Can not add child entity. Parent entity rejected the child entity. Parent Entity Name: \"%s\", Child Entity Name: \"%s\".", 
		GetName().ToCString(), Entity->GetName().ToCString());

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	ChildEntities.LockWriteNested();
	{
		Entity->SetParent(this);
		Entity->SetScene(this->Scene);

		ChildEntities.Add(Entity);
	}
	ChildEntities.UnlockWrite();

	if (State == ZE_ES_LOADED || State == ZE_ES_LOADING)
		Entity->Load();
	else if (State == ZE_ES_INITIALIZED || State == ZE_ES_INITIALIZING)
		Entity->Initialize();

	if (Entity->GetName().IsEmpty())
		Entity->SetName(GetName() + "->" +  Entity->GetClass()->GetName());

	return true;
}

void ZEEntity::RemoveChildEntity(ZEEntity* Entity)
{
	zeCheckError(Entity->Parent != this, ZE_VOID, "Cannot remove non-child entity.");

	ChildEntities.LockWriteNested();
	{
		ChildEntities.RemoveValue(Entity);

		Entity->SetParent(NULL);
		Entity->SetScene(NULL);
	}
	ChildEntities.UnlockWrite();
}

void ZEEntity::ClearChildEntities()
{
	ChildEntities.LockWrite();
	{
		while (ChildEntities.GetCount() != 0)
		{
			ZEEntity* ChildEntity = ChildEntities.GetFirstItem();
			ChildEntity->Destroy();
		}
	}
	ChildEntities.UnlockWrite();
}

bool ZEEntity::CheckParent(ZEEntity* Parent)
{
	return true;
}

bool ZEEntity::CheckComponent(ZEEntity* Parent)
{
	return true;
}

bool ZEEntity::CheckChildEntity(ZEEntity* Parent)
{
	return true;
}

void ZEEntity::SetParent(ZEEntity* Parent)
{
	EntityLock.LockNested();
	{
		this->Parent = Parent;
		ParentTransformChanged();
		ParentEnabledChanged();
		ParentVisibleChanged();
	}
	EntityLock.Unlock();
}

void ZEEntity::SetScene(ZEScene* NewScene)
{
	EntityLock.LockNested();
	{
		if (Scene == NewScene)
		{
			EntityLock.Unlock();
			return;
		}

		if (Scene != NULL)
		{
			if (GetEntityFlags().GetFlags(ZE_EF_TICKABLE) || GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM))
				Scene->RemoveFromTickList(this);

			if (GetEntityFlags().GetFlags(ZE_EF_RENDERABLE) || GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM))
				Scene->RemoveFromRenderList(this);
		}

		Scene = NewScene;

		if (Scene != NULL)
		{
			UpdateTickabilityState();
			UpdateRenderabilityState();
		}

		Components.LockRead();
		{
			for (ZESize I = 0; I < Components.GetCount(); I++)
				Components[I]->SetScene(Scene);
		}
		Components.UnlockRead();

		ChildEntities.LockRead();
		{
			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
				ChildEntities[I]->SetScene(Scene);
		}
		ChildEntities.UnlockRead();

	}
	EntityLock.Unlock();
}

void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox, bool NoEvent)
{
	if (this->BoundingBox.Min == BoundingBox.Min && this->BoundingBox.Max == BoundingBox.Max)
		return;

	this->BoundingBox = BoundingBox;

	if (!NoEvent)
		BoundingBoxChanged();
}

ZETaskResult ZEEntity::UpdateStateTaskFunction(ZETaskThread* Thread, ZESize InstanceIndex, void* Parameters)
{
	/*zeLog("%s::ManageStates, State: %s, TargetState:%s", 
		GetClass()->GetName(), 
		ZEEntityState_Enumerator()->ToText(State, "Unknown"), 
		ZEEntityState_Enumerator()->ToText(TargetState, "Unknown"));*/

	// Persistent States
	if (State == ZE_ES_NONE)
	{
		ReloadFlag = false;
		ReinitializeFlag = false;
		SetLocalLoadingPercentage(0);
		UpdateRenderabilityState();

		if (TargetState == ZE_ES_DESTROYED)
		{
			State = ZE_ES_DESTROYING;

			ClearChildEntities();
			ClearComponents();
			
			PostDestroyInternal();

			delete this;
			return ZE_TR_DONE;
		}
		else if (TargetState >= ZE_ES_LOADED)
		{
			State = ZE_ES_LOADING;

			Components.LockRead();
			{
				for (ZESize I = 0; I < Components.GetCount(); I++)
					Components[I]->Load();
			}
			Components.UnlockRead();

			ChildEntities.LockRead();
			{
				for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
					ChildEntities[I]->Load();
			}
			ChildEntities.UnlockRead();

			return ZE_TR_COOPERATING;
		}

	}
	else if (State == ZE_ES_LOADED)
	{
		ReinitializeFlag = false;
		SetLocalLoadingPercentage(100);
		UpdateRenderabilityState();
		UpdateTickabilityState();

		if (ReloadFlag || TargetState < ZE_ES_LOADED)
		{
			State = ZE_ES_UNLOADING;

			Components.LockRead();
			{
				for (ZESize I = 0; I < Components.GetCount(); I++)
					Components[I]->Unload();
			}
			Components.UnlockRead();

			ChildEntities.LockRead();
			{
				for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
					ChildEntities[I]->Unload();
			}
			ChildEntities.UnlockRead();
		}
		else if (TargetState == ZE_ES_INITIALIZED)
		{
			State = ZE_ES_INITIALIZING;

			Components.LockRead();
			{
				for (ZESize I = 0; I < Components.GetCount(); I++)
					Components[I]->Initialize();
			}
			Components.UnlockRead();

			ChildEntities.LockRead();
			{
				for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
					ChildEntities[I]->Initialize();
			}
			ChildEntities.UnlockRead();
		}
	}
	else if (State == ZE_ES_INITIALIZED)
	{
		UpdateTickabilityState();

		if (ReloadFlag || ReinitializeFlag || TargetState < ZE_ES_INITIALIZED)
		{
			State = ZE_ES_DEINITIALIZING;

			Components.LockRead();
			{
				for (ZESize I = 0; I < Components.GetCount(); I++)
					Components[I]->Deinitialize();
			}
			Components.UnlockRead();

			ChildEntities.LockRead();
			{
				for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
					ChildEntities[I]->Deinitialize();
			}
			ChildEntities.UnlockRead();
		}
	}
	
	// Transition States
	if (State == ZE_ES_DESTROYING)
	{
		return ZE_TR_COOPERATING;
	}
	else if (State == ZE_ES_LOADING)
	{
		#ifdef ZE_DEBUG_ENABLE
		LoadInternalChainCheck = false;
		#endif
		
		ZEEntityResult Result = LoadInternal();	
		if (Result == ZE_ER_DONE)
		{
			zeDebugCheck(!LoadInternalChainCheck, "LoadInternal chain problem. Chain is not completed. Class Name: \"%s\".", GetClass()->GetName());

			State = ZE_ES_LOADED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_FAILED_CLEANUP)
		{
			UnloadInternal();
			State = ZE_ES_ERROR_LOADING;
			return ZE_TR_FAILED;
		}
		else
		{
			State = ZE_ES_ERROR_LOADING;
			return ZE_TR_FAILED;
		}
	}
	else if (State == ZE_ES_UNLOADING)
	{
		#ifdef ZE_DEBUG_ENABLE
		UnloadInternalChainCheck = false;
		#endif

		ZEEntityResult Result = UnloadInternal();
		if (Result == ZE_ER_DONE)
		{
			zeDebugCheck(!UnloadInternalChainCheck, "UnloadInternal chain problem. Chain is not completed. Class Name: \"%s\".", GetClass()->GetName());

			State = ZE_ES_NONE;		
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
		#ifdef ZE_DEBUG_ENABLE
		InitializeInternalChainCheck = false;
		#endif

		ZEEntityResult Result = InitializeInternal();
		if (Result == ZE_ER_DONE)
		{
			zeDebugCheck(!InitializeInternalChainCheck, "InitializeInternal chain problem. Chain is not completed. Class Name: \"%s\".", GetClass()->GetName());

			State = ZE_ES_INITIALIZED;
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_WAIT)
		{
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_ER_FAILED_CLEANUP)
		{
			DeinitializeInternal();
			State = ZE_ES_ERROR_LOADING;
			return ZE_TR_FAILED;
		}
		else
		{
			State = ZE_ES_ERROR_INITIALIZATION;
			return ZE_TR_FAILED;
		}
	}
	else if (State == ZE_ES_DEINITIALIZING)
	{
		#ifdef ZE_DEBUG_ENABLE
		DeinitializeInternalChainCheck = false;
		#endif

		ZEEntityResult Result = DeinitializeInternal();
		if (Result == ZE_ER_DONE)
		{
			zeDebugCheck(!DeinitializeInternalChainCheck, "DeinitializeInternal chain problem. Chain is not completed. Class Name: \"%s\".", GetClass()->GetName());

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

void ZEEntity::UpdateStateSerial()
{
	while (UpdateStateTaskFunction(NULL, 0, NULL) == ZE_TR_COOPERATING);
}

void ZEEntity::UpdateState()
{
	if (SerialOperation)
		UpdateStateSerial();
	else
		UpdateStateTask.Run();
}

void ZEEntity::SetEntityFlags(ZEEntityFlags Flags)
{
	if (EntityFlags == Flags)
		return;

	EntityFlags = Flags;

	UpdateRenderabilityState(true);
	UpdateTickabilityState();
}

void ZEEntity::SetSerialOperation(bool SerialOperation)
{
	this->SerialOperation = SerialOperation;
}

bool ZEEntity::GetSerialOperation() const 
{
	return SerialOperation;
}

void ZEEntity::SetLocalLoadingPercentage(ZEUInt Percentage)
{
	if (Percentage > 100)
		Percentage = 100;
	
	EntityLock.LockNested();
	{
		LocalLoadingPercentage = Percentage;
	}
	EntityLock.Unlock();
}

ZEUInt ZEEntity::GetLocalLoadingPercentage()
{
	return LocalLoadingPercentage;
}

ZEEntityResult ZEEntity::LoadInternal()
{
	zeDebugCheck(GetState() != ZE_ES_LOADING, "LoadInternal chain problem. Entity state is wrong. State: %s, Class Name: \"%s\".", 
		ZEEntityState_Enumerator()->ToText(GetState(), "Unknown").ToCString(), GetClass()->GetName());

	#ifdef ZE_DEBUG_ENABLE
	LoadInternalChainCheck = true;
	#endif

	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::UnloadInternal()
{
	zeDebugCheck(GetState() != ZE_ES_UNLOADING  && GetState() != ZE_ES_LOADING, "UnloadInternal chain problem. Entity state is wrong. State: %s, Class Name: \"%s\".", 
		ZEEntityState_Enumerator()->ToText(GetState(), "Unknown").ToCString(), GetClass()->GetName());

	#ifdef ZE_DEBUG_ENABLE
	UnloadInternalChainCheck = true;
	#endif

	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::InitializeInternal()
{
	zeDebugCheck(GetState() != ZE_ES_INITIALIZING, "InitializeInternal chain problem. Entity state is wrong. State: %s, Class Name: \"%s\".", 
		ZEEntityState_Enumerator()->ToText(GetState(), "Unknown").ToCString(), GetClass()->GetName());

	#ifdef ZE_DEBUG_ENABLE
	InitializeInternalChainCheck = true;
	#endif

	return ZE_ER_DONE;
}

ZEEntityResult ZEEntity::DeinitializeInternal()
{
	zeDebugCheck(GetState() != ZE_ES_INITIALIZING && GetState() != ZE_ES_DEINITIALIZING, "DeinitializeInternal chain problem. Entity state is wrong. State: %s, Class Name: \"%s\".", 
		ZEEntityState_Enumerator()->ToText(GetState(), "Unknown").ToCString(), GetClass()->GetName());

	#ifdef ZE_DEBUG_ENABLE
	DeinitializeInternalChainCheck = true;
	#endif

	return ZE_ER_DONE;
}

void ZEEntity::PreDestroyInternal()
{
	zeDebugCheck(TargetState != ZE_ES_DESTROYING, "Entity state is wrong. PreDestroyInternal function called explicitly while object is not destroying.");
}

void ZEEntity::PostDestroyInternal()
{
	zeDebugCheck(TargetState != ZE_ES_DESTROYED, "Entity state is wrong. PostDestroyInternal function called explicitly while object is not destroying.");
}

ZEEntity::ZEEntity() : TickListLink(this), RenderListLink(this)
{
	Parent = NULL;
	Scene = NULL;
	Wrapper = NULL;

	State = ZE_ES_NONE;
	TargetState = ZE_ES_NONE;
	ReloadFlag = false;
	ReinitializeFlag = false;
	SerialOperation = false;
	LocalLoadingPercentage = 0;
	UpdateStateTask.SetPoolId(ZE_TPI_CONCURENT);
	UpdateStateTask.SetFunction(ZEDelegateMethod(ZETaskFunction, ZEEntity, UpdateStateTaskFunction, this));

	EntityId = 0;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Enabled = true;
	EnabledFlattened = true;
	Visible = true;
	VisibleFlattened = true;
	EntityFlags = ZE_EF_NONE;
	Static = false;
	
	Transform = ZEMatrix4x4::Identity;
	InvTransform = ZEMatrix4x4::Identity;
	WorldTransform = ZEMatrix4x4::Identity;
	InvWorldTransform = ZEMatrix4x4::Identity;
	BoundingBox = ZEAABBox::Zero;
	WorldBoundingBox = ZEAABBox::Zero;
	RenderListOctree = NULL;

	#ifdef ZE_DEBUG_ENABLE
	LoadInternalChainCheck = false;
	UnloadInternalChainCheck = false;
	InitializeInternalChainCheck = false;
	DeinitializeInternalChainCheck = false;
	DestroyInternalChainCheck = false;
	#endif

	LocalTransformChanged();
}

ZEEntity::~ZEEntity()
{
	zeDebugCheck(Components.GetCount() != 0, "Destroy problem. Components exists.");
	zeDebugCheck(ChildEntities.GetCount() != 0, "Destroy problem. Child Entities.");
	zeDebugCheck(Parent != NULL, "Destroy problem. Parent is not NULL.");
}

ZEEntity* ZEEntity::GetParent() const
{
	return Parent;
}

ZEScene* ZEEntity::GetScene() const
{
	return Scene;
}

ZEEntity* ZEEntity::GetComponent(ZEClass* Class, bool Recursive)
{
	Components.LockRead();
	{
		ZEEntity* Output = NULL;
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (!ZEClass::IsDerivedFrom(Class, Components[I]))
				continue;

			Output = Components[I];
			Components.UnlockRead();
			return Output;
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < Components.GetCount(); I++)
			{
				Output = Components[I]->GetComponent(Class, true);
				if (Output == NULL)
					continue;

				Components.UnlockRead();
				return Output;
			}
		}
	}
	Components.UnlockRead();

	return NULL;
}

ZEEntity* ZEEntity::GetComponent(const ZEString& Name, bool Recursive)
{
	Components.LockRead();
	{
		ZEEntity* Output = NULL;
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (Components[I]->GetName() != Name)
				continue;

			Output = Components[I];
			Components.UnlockRead();
			return Output;
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < Components.GetCount(); I++)
			{
				Output = Components[I]->GetComponent(Name, true);
				if (Output == NULL)
					continue;

				Components.UnlockRead();
				return Output;
			}
		}
	}
	Components.UnlockRead();

	return NULL;
}

const ZEArray<ZEEntity*>& ZEEntity::GetComponents() const
{
	return Components;
}

const ZEArray<ZEEntity*> ZEEntity::GetComponents(ZEClass* Class, bool Recursive) const
{
	ZEArray<ZEEntity*> Output;
	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(Class, Components[I]))
				Output.Add(Components[I]);

			if (Recursive)
				GetComponentsInternal(Class, Output);
		}
	}
	Components.UnlockRead();
	
	return Output;
}

const ZEArray<ZEEntity*> ZEEntity::GetComponents(const ZEString& Name, bool Recursive) const
{
	ZEArray<ZEEntity*> Output;
	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			if (Components[I]->GetName() == Name)
				Output.Add(Components[I]);

			if (Recursive)
				GetComponentsInternal(Name, Output);
		}
	}
	Components.UnlockRead();

	return Output;
}

ZEEntity* ZEEntity::GetChildEntity(ZEClass* Class, bool Recursive)
{
	ChildEntities.LockRead();
	{
		ZEEntity* Output = NULL;
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (!ZEClass::IsDerivedFrom(Class, ChildEntities[I]))
				continue;

			Output = ChildEntities[I];
			ChildEntities.UnlockRead();
			return Output;
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			{
				Output = ChildEntities[I]->GetChildEntity(Class, true);
				if (Output == NULL)
					continue;

				ChildEntities.UnlockRead();
				return Output;
			}
		}
	}
	ChildEntities.UnlockRead();

	return NULL;
}

ZEEntity* ZEEntity::GetChildEntity(const ZEString& Name, bool Recursive)
{
	ChildEntities.LockRead();
	{
		ZEEntity* Output = NULL;
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (ChildEntities[I]->GetName() != Name)
				continue;

			Output = ChildEntities[I];
			ChildEntities.UnlockRead();
			return Output;
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			{
				Output = ChildEntities[I]->GetChildEntity(Name, true);
				if (Output == NULL)
					continue;

				ChildEntities.UnlockRead();
				return Output;
			}
		}
	}
	ChildEntities.UnlockRead();

	return NULL;
}

const ZEArray<ZEEntity*>& ZEEntity::GetChildEntities() const
{
	return ChildEntities; 
}

const ZEArray<ZEEntity*> ZEEntity::GetChildEntities(ZEClass* Class, bool Recursive) const
{
	ZEArray<ZEEntity*> Output;
	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(Class, ChildEntities[I]))
				Output.Add(ChildEntities[I]);

			if (Recursive)
				GetChildEntitiesInternal(Class, Output);
		}
	}
	ChildEntities.UnlockRead();

	return Output;
}

const ZEArray<ZEEntity*> ZEEntity::GetChildEntities(const ZEString& Name, bool Recursive) const
{
	ZEArray<ZEEntity*> Output;
	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			if (ChildEntities[I]->GetName() == Name)
				Output.Add(ChildEntities[I]);

			if (Recursive)
				GetChildEntitiesInternal(Name, Output);
		}
	}
	ChildEntities.UnlockRead();

	return Output;
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


const ZEMatrix4x4& ZEEntity::GetInvTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_INV_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateInvOrientation(InvTransform, Position, Rotation, ZEVector3::One);
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_INV_LOCAL_TRANSFORM);
	}

	return InvTransform;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
	{
		if (Parent == NULL)
			WorldTransform = GetTransform();
		else
			ZEMatrix4x4::Multiply(WorldTransform, Parent->GetWorldTransform(), GetTransform());

		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEEntity::GetInvWorldTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_INV_WORLD_TRANSFORM))
	{
		if (Parent == NULL)
			InvWorldTransform = GetInvTransform();
		else
			ZEMatrix4x4::Multiply(InvWorldTransform, GetInvTransform(), Parent->GetInvWorldTransform());

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

void ZEEntity::SetName(const ZEString& NewName)
{
	Name = NewName;
}

const ZEString& ZEEntity::GetName() const
{
	return Name;
}

ZEEntityFlags ZEEntity::GetEntityFlags() const
{
	return EntityFlags;
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

ZEVector3 ZEEntity::GetWorldPosition() const
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

ZEQuaternion ZEEntity::GetWorldRotation() const
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

ZEVector3 ZEEntity::GetWorldScale() const
{
	if (Parent != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Parent->GetWorldScale(), GetScale());
		return Temp;
	}

	return GetScale();
}

void ZEEntity::SetTransform(const ZEMatrix4x4& Transform)
{
	this->Position = Transform.GetTranslation();
	this->Rotation = Transform.GetRotation();
	this->Scale = Transform.GetScale();

	LocalTransformChanged();
}

void ZEEntity::SetWorldTransform(const ZEMatrix4x4& Transform)
{
	if (Parent != NULL)
	{
		ZEMatrix4x4 Result;
		ZEMatrix4x4::Multiply(Result, Parent->GetInvWorldTransform(), Transform);
		SetTransform(Result);
	}
	else
	{
		SetTransform(Transform);
	}
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

ZEEntityState ZEEntity::GetTargetState() const
{
	return TargetState;
}

bool ZEEntity::IsLoaded() const
{
	return State >= ZE_ES_LOADED;
}

bool ZEEntity::IsLoadedOrLoading() const
{
	return State >= ZE_ES_LOADING;
}

bool ZEEntity::IsInitialized() const
{
	return State >= ZE_ES_INITIALIZED;
}

bool ZEEntity::IsInitializedOrInitializing() const
{
	return State >= ZE_ES_INITIALIZING;
}

bool ZEEntity::IsFailed() const
{
	return State <= 0;
}

bool ZEEntity::IsDestroyed() const
{
	return TargetState == ZE_ES_DESTROYED || TargetState == ZE_ES_DESTROYING;
}

ZEUInt ZEEntity::GetLoadingPercentage()
{
	ZEEntityLoadingScore Score = GetLoadingScore();

	if (Score.Count == 0)
		return 0;

	return Score.Score / Score.Count;
}

ZEEntityLoadingScore ZEEntity::GetLoadingScore()
{
	ZEEntityLoadingScore TotalScore;
	if (!IsFailed())
	{
		TotalScore.Count = 1;
		TotalScore.Score = GetLocalLoadingPercentage();
	}
	else
	{
		TotalScore.Count = 0;
		TotalScore.Score = 0;
	}

	Components.LockRead();
	{
		for (ZESize I = 0; I < Components.GetCount(); I++)
		{
			ZEEntityLoadingScore ComponentScore = Components[I]->GetLoadingScore();
			TotalScore.Count += ComponentScore.Count;
			TotalScore.Score += ComponentScore.Score;
		}
	}
	Components.UnlockRead();

	ChildEntities.LockRead();
	{
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		{
			ZEEntityLoadingScore ChildEntityScore = ChildEntities[I]->GetLoadingScore();
			TotalScore.Count += ChildEntityScore.Count;
			TotalScore.Score += ChildEntityScore.Score;
		}
	}
	ChildEntities.UnlockRead();

	return TotalScore;
}

void ZEEntity::SetEnabled(bool Enabled)
{
	if (this->Enabled == Enabled)
		return;

	this->Enabled = Enabled;

	ParentEnabledChanged();
}

bool ZEEntity::GetEnabled() const
{
	return Enabled;
}

void ZEEntity::SetVisible(bool Visible)
{
	if (this->Visible == Visible)
		return;

	this->Visible = Visible;

	ParentVisibleChanged();
}

bool ZEEntity::GetVisible() const
{
	return Visible;
}

void ZEEntity::SetStatic(bool Static)
{
	if (this->Static == Static)
		return;

	this->Static = Static;
	
	UpdateRenderabilityState(true);
}

bool ZEEntity::GetStatic()
{
	return Static;
}

void ZEEntity::Initialize()
{
	if (IsDestroyed())
		return;

	if (TargetState >= ZE_ES_INITIALIZED)
		return;

	TargetState = ZE_ES_INITIALIZED;

	UpdateState();
}

void ZEEntity::Deinitialize()
{
	if (IsDestroyed())
		return;

	if (TargetState <= ZE_ES_LOADED)
		return;
	
	UpdateState();
}

void ZEEntity::Reinitialize()
{
	if (IsDestroyed())
		return;

	ReinitializeFlag = true;

	if (TargetState < ZE_ES_INITIALIZED)
		TargetState = ZE_ES_INITIALIZED;

	UpdateState();
}

void ZEEntity::Load()
{
	if (IsDestroyed())
		return;

	if (TargetState >= ZE_ES_LOADED)
		return;

	TargetState = ZE_ES_LOADED;
	
	UpdateState();
}

void ZEEntity::Unload()
{
	if (IsDestroyed())
		return;

	if (TargetState <= ZE_ES_NONE)
		return;

	TargetState = ZE_ES_NONE;

	UpdateState();
}

void ZEEntity::Reload()
{
	if (IsDestroyed())
		return;

	ReloadFlag = true;

	if (TargetState <= ZE_ES_LOADED)
		TargetState = ZE_ES_LOADED;

	UpdateState();
}

void ZEEntity::Destroy()
{
	TargetState = ZE_ES_DESTROYING;

	PreDestroyInternal();

	EntityLock.Lock();
	{
		if (Parent != NULL)
		{
			if (Parent->Components.Exists(this))
				Parent->RemoveComponent(this);
			else if (Parent->ChildEntities.Exists(this))
				Parent->RemoveChildEntity(this);
		}
		else if (GetScene() != NULL)
		{
			GetScene()->RemoveEntity(this);
		}
	}
	EntityLock.Unlock();

	TargetState = ZE_ES_DESTROYED;

	UpdateState();
}

void ZEEntity::Tick(float Time)
{

}

bool ZEEntity::PreRender(const ZERNPreRenderParameters* Parameters)
{
	return true;
}

void ZEEntity::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	
}

void ZEEntity::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERayCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetObject(this);
	Helper.SetSubObject(NULL);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());

	Helper.RayCastBoundingBox(GetWorldBoundingBox(), GetBoundingBox());
}

void ZEEntity::VolumeCast(ZEVolumeCastReport& Report, const ZEVolumeCastParameters& Parameters)
{
	ZEVolumeCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetObject(this);
	Helper.SetSubObject(NULL);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());

	Helper.VolumeCastBoundingBox(GetWorldBoundingBox(), GetBoundingBox());
}

bool ZEEntity::Serialize(ZEMLWriterNode* Serializer)
{	
	if (Serializer == NULL)
		return false;

	ZEMLWriterNode PropertiesNode; 
	Serializer->OpenNode("Properties", PropertiesNode);
	
	const ZEMTProperty* Properties = GetClass()->GetProperties();
	for (ZESize I = 0; I < GetClass()->GetPropertyCount(); I++)
	{
		const ZEMTProperty* Current = &Properties[I];
		if (Current->Type.GetCollectionType() != ZEMT_CT_NONE)
			continue;

		if (Current->Type.GetBaseQualifier() != ZEMT_TQ_VALUE)
			continue;

		if (Current->Type.GetBaseType() == ZEMT_BT_OBJECT || Current->Type.GetBaseType() == ZEMT_BT_OBJECT_PTR)
			continue;

		if ((Current->Access & ZEMT_PA_READ_WRITE) != ZEMT_PA_READ_WRITE)
			continue;

		if (Current->CheckAttributeHasValue("ZEMeta.Serialization", "false"))
			continue;

		ZEVariant Variant;
		GetClass()->GetProperty(this, Current->ID, Variant);

		ZEValue Value = Variant.GetValue();
		if (Value.IsNull())
			continue;

		PropertiesNode.WriteValue(Current->Name, Value);
	}
	
	PropertiesNode.CloseNode();

	ChildEntities.LockRead();
	{
		if (ChildEntities.GetCount() != 0)
		{
			ZEMLWriterNode SubEntitiesNode, EntityNode;
			Serializer->OpenNode("ChildEntities", SubEntitiesNode);

			for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			{
				SubEntitiesNode.OpenNode("Entity", EntityNode);
				EntityNode.WriteString("Class", ChildEntities[I]->GetClass()->GetName());
				ChildEntities[I]->Serialize(&EntityNode);
				EntityNode.CloseNode();
			}

			SubEntitiesNode.CloseNode();
		}
	}
	ChildEntities.UnlockRead();
	
	return true;
}

bool ZEEntity::Unserialize(ZEMLReaderNode* Unserializer)
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


	ZESize ChildEntityCount = SubEntitiesNode.GetNodeCount("Entity");
	ZEClass* NewSubEntityClass = NULL;
	ZEEntity* NewSubEntity = NULL;
	ChildEntities.LockWrite();
	{
		for (ZESize I = 0; I < ChildEntityCount; I++)
		{
			ZEMLReaderNode SubEntityNode = SubEntitiesNode.GetNode("Entity", I);

			NewSubEntityClass = ZEMTProvider::GetInstance()->GetClass(SubEntityNode.ReadString("Class"));
			if (NewSubEntityClass == NULL)
			{
				zeError("Unserialization failed. Child Entity class is not registered. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
				ChildEntities.UnlockWrite();
				return false;
			}

			NewSubEntity = ZEClass::Cast<ZEEntity>(NewSubEntityClass->CreateInstance());
			if (NewSubEntity == NULL)
			{
				zeError("Unserialization failed. Cannot create instance of a child entity. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
				NewSubEntity->Destroy();
				ChildEntities.UnlockWrite();
				return false;
			}

			if (!NewSubEntity->Unserialize(&SubEntityNode))
			{
				zeError("Unserialization failed. Unserialization of child entity has failed. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
				NewSubEntity->Destroy();
				ChildEntities.UnlockWrite();
				return false;
			}

			if (!AddChildEntity(NewSubEntity))
			{
				zeError("Unserialization failed. Cannot add child entity. Class Name: \"%s\".", SubEntityNode.ReadString("Class").ToCString());
				ChildEntities[I] = NULL;
				NewSubEntity->Destroy();
				ChildEntities.UnlockWrite();
				return false;
			}
		}
	}
	ChildEntities.UnlockWrite();

	return true;
}

void ZEEntity::LockEntity()
{
	EntityLock.Lock();
}

void ZEEntity::UnlockEntity()
{
	EntityLock.Unlock();
}
