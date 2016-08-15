//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEScene.cpp
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

#include "ZEScene.h"

#include "ZETypes.h"
#include "ZEError.h"
#include "ZEMath/ZERay.h"
#include "ZEFile/ZEFile.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEMeta/ZEProvider.h"

#include "ZEEntity.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZECamera.h"
#include "ZESound/ZESoundModule.h"
#include "ZEPhysics/ZEPhysicalWorld.h"

#include <memory.h>

#define ZE_SDF_CONSTANT_BUFFER		0x01

void ZEScene::TickEntity(ZEEntity* Entity, float ElapsedTime)
{
	bool Custom = Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM);
	bool Initialized = Entity->IsInitialized();

	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM) && !Entity->IsInitialized(), "Ticking an entity which is not initialized.");
	zeDebugCheck(!Entity->GetEnabled(), "Ticking an entity which is not enabled.");

	Entity->Tick(ElapsedTime);
}

void ZEScene::PreRenderEntity(ZEEntity* Entity, const ZERNPreRenderParameters* Parameters)
{
	zeDebugCheck(!Entity->GetVisible(), "PreRendering an entity which is not visible.");

	if (!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded())
		return;

	if (!Entity->IsLoaded())
		return;

	bool Cullable = Entity->GetEntityFlags().GetFlags(ZE_EF_CULLABLE);
	if (Cullable && Parameters->View->ViewVolume != NULL)
	{
		if (Parameters->View->ViewVolume->IntersectionTest(Entity->GetWorldBoundingBox()))
			Entity->PreRender(Parameters);
	}
	else
	{
		Entity->PreRender(Parameters);
	}
}

void ZEScene::RayCastEntity(ZEEntity* Entity, ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (Entity->GetState() < ZE_ES_LOADED)
		return;

	Entity->RayCast(Report, Parameters);

	if (Report.CheckDone())
		return;

	const ZEArray<ZEEntity*> ChildEntities = Entity->GetChildEntities();
	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
	{
		if (!Parameters.Filter(ChildEntities[I]))
			continue;

		RayCastEntity(ChildEntities[I], Report, Parameters);

		if (Report.CheckDone())
			return;
	}

	const ZEArray<ZEEntity*> Components = Entity->GetComponents();
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (!Parameters.Filter(Components[I]))
			continue;

		RayCastEntity(Components[I], Report, Parameters);

		if (Report.CheckDone())
			return;
	}
}

bool ZEScene::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	if (PhysicalWorld == NULL)
		PhysicalWorld = ZEPhysicalWorld::CreateInstance();

	if (PhysicalWorld == NULL)
	{
		zeCriticalError("Can not create physical world.");
		return false;
	}

	if (!PhysicalWorld->Initialize())
	{
		zeCriticalError("Can not create physical world.");
		return false;
	}

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Initialize();

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(Constants));
	SceneDirtyFlags.RaiseAll();

	return true;
}

bool ZEScene::DeinitializeInternal()
{
	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Deinitialize();

	if (PhysicalWorld != NULL)
		PhysicalWorld->Deinitialize();

	ConstantBuffer.Release();
	SceneDirtyFlags.RaiseAll();

	return ZEInitializable::DeinitializeInternal();
}

void ZEScene::AddToTickList(ZEEntity* Entity)
{
	bool Custom = Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM);
	bool Initialized = Entity->IsInitialized();

	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM) && !Entity->IsInitialized(), "Ticking an entity which is not initialized.");
	zeDebugCheck(!Entity->GetEnabled(), "Ticking an entity which is not enabled.");

	if (Entity->TickListLink.GetInUse())
		return;

	ZE_LOCK_SECTION(TickListLock)
	{
		TickList.AddEnd(&Entity->TickListLink);
	}
}

void ZEScene::RemoveFromTickList(ZEEntity* Entity)
{
	if (!Entity->TickListLink.GetInUse())
		return;

	ZE_LOCK_SECTION(TickListLock)
	{
		TickList.Remove(&Entity->TickListLink);
	}
}

void ZEScene::AddToRenderList(ZEEntity* Entity)
{
	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded(), "PreRendering an entity which is not loaded.");
	zeDebugCheck(!Entity->GetVisible(), "PreRendering an entity which is not visible.");

	if (Entity->RenderListLink.GetInUse())
		return;

	ZE_LOCK_SECTION(RenderListLock)
	{
		RenderList.AddEnd(&Entity->RenderListLink);
	}
}

void ZEScene::RemoveFromRenderList(ZEEntity* Entity)
{
	if (!Entity->RenderListLink.GetInUse())
		return;

	ZE_LOCK_SECTION(RenderListLock)
	{
		if (Entity->GetStatic())
			RenderListOctree.RemoveItem(Entity, Entity->GetWorldBoundingBox());

		RenderList.Remove(&Entity->RenderListLink);
	}
}

void ZEScene::EntityBoundingBoxChanged(ZEEntity* Entity)
{
	if (!Entity->GetStatic())
		return;

	if (!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded())
		return;

	return;

	ZE_LOCK_SECTION(RenderListLock)
	{
		RenderListOctree.RemoveItem(Entity);
		RenderListOctree.AddItem(Entity, Entity->GetWorldBoundingBox());
	}
}

void ZEScene::UpdateConstantBuffer()
{
	if (!SceneDirtyFlags.GetFlags(ZE_SDF_CONSTANT_BUFFER))
		return;

	ConstantBuffer->SetData(&Constants);

	SceneDirtyFlags.UnraiseFlags(ZE_SDF_CONSTANT_BUFFER);
}

const ZEGRConstantBuffer* ZEScene::GetConstantBuffer()
{
	UpdateConstantBuffer();
	return ConstantBuffer;
}

ZEPhysicalWorld* ZEScene::GetPhysicalWorld()
{
	return PhysicalWorld;
}

void ZEScene::SetActiveCamera(ZECamera* Camera)
{
	ActiveCamera = Camera;
}

ZECamera* ZEScene::GetActiveCamera()
{
	return ActiveCamera;
}

void ZEScene::SetActiveListener(ZEListener* Listener)
{
	ActiveListener = Listener;
	zeSound->SetActiveListener(Listener);
}

ZEListener* ZEScene::GetActiveListener()
{
	return ActiveListener;
}

void ZEScene::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEScene::GetEnabled() const
{
	return Enabled;
}

void ZEScene::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;

	Constants.AmbientColor = AmbientColor * AmbientFactor;
	SceneDirtyFlags.RaiseFlags(ZE_SDF_CONSTANT_BUFFER);
}

float ZEScene::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEScene::SetAmbientColor(ZEVector3 Color)
{
	AmbientColor = Color;

	Constants.AmbientColor = AmbientColor * AmbientFactor;
	SceneDirtyFlags.RaiseFlags(ZE_SDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEScene::GetAmbientColor() const
{
	return AmbientColor;
}

ZEUInt ZEScene::GetLoadingPercentage()
{
	ZEEntityLoadingScore TotalScore;
	TotalScore.Score = 0;
	TotalScore.Count = 0;

	ZE_LOCK_SECTION(SceneLock)
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			ZEEntityLoadingScore EntityScore = Entities[I]->GetLoadingScore();
			TotalScore.Score += EntityScore.Score;
			TotalScore.Count += EntityScore.Count;
		}
	}

	if (TotalScore.Count == 0)
		return 100;
	else
		return (ZEUInt)(TotalScore.Score / TotalScore.Count);
}

const ZESmartArray<ZEEntity*>& ZEScene::GetEntities()
{
	return Entities;
}

ZEArray<ZEEntity*> ZEScene::GetEntities(ZEClass* Class)
{
	ZEArray<ZEEntity*> ProperEntities;
	ZEEntity* CurrentEntity = NULL;
	ProperEntities.Clear();

	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		CurrentEntity = Entities[I];
		if (Class->IsDerivedFrom(Class, CurrentEntity->GetClass()))
			ProperEntities.Add(CurrentEntity);
	}

	return ProperEntities;
}

void ZEScene::AddEntity(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Entity is NULL.");
	zeCheckError(Entity->GetScene() != NULL, ZE_VOID, "Entity is already added to a scene.");

	ZE_LOCK_SECTION(SceneLock)
	{
		Entity->SetEntityId(LastEntityId++);
		Entities.Add(Entity);
	}
	
	Entity->SetScene(this);
	Entity->Load();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(Entity->GetClass()->GetName() + ZEString(LastEntityId));

	if (IsInitializedOrInitializing())
		Entity->Initialize();
}

void ZEScene::RemoveEntity(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Entity is NULL.");
	zeCheckError(Entity->GetScene() != this, ZE_VOID, "Entity does not belong to this scene.");
	zeCheckError(Entity->GetParent() != NULL, ZE_VOID, "Entity is not a root entity.");

	Entity->Deinitialize();
	Entity->SetScene(NULL);

	ZE_LOCK_SECTION(SceneLock)
	{
		Entities.RemoveValue(Entity);
	}
}

void ZEScene::ClearEntities()
{
	while(Entities.GetCount() != 0)
		Entities.GetFirstItem()->Destroy();
}

void ZEScene::Tick(float ElapsedTime)
{
	if (!Enabled)
		return;

	ZE_LOCK_SECTION(TickListLock)
	{
		ze_for_each(Entity, TickList)
			TickEntity(Entity.GetPointer(), ElapsedTime);
	}
}

void ZEScene::PreRender(const ZERNPreRenderParameters* Parameters)
{
	Parameters->Renderer->StartScene(GetConstantBuffer());

	ZE_LOCK_SECTION(RenderListLock)
	{
		ze_for_each_iterator(Node, RenderListOctree.Traverse(Parameters->View->ViewVolume))
		{
			for (ZESize I = 0; I < Node->GetItemCount(); I++)
			{
				ZEEntity* Entity = Node->GetItem(I);
				PreRenderEntity(Entity, Parameters);
			}
		}

		ze_for_each(Entity, RenderList)
			PreRenderEntity(Entity.GetPointer(), Parameters);
	}

	Parameters->Renderer->EndScene();
}

void ZEScene::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		if (!Parameters.Filter(Entities[I]))
			continue;

		RayCastEntity(Entities[I], Report, Parameters);

		if (Report.CheckDone())
			break;
	}
}

bool ZEScene::Serialize(const ZEString& FileName)
{
	zeLog("Saving scene file \"%s\".", FileName.GetValue());

	ZEFile SceneFile;
	if (!SceneFile.Open(FileName, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot save scene. Cannot write to scene file. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	ZEMLWriter Writer;
	Writer.Open(&SceneFile);
	ZEMLWriterNode SceneNode;
	Writer.OpenRootNode("ZEScene", SceneNode);

	SceneNode.WriteUInt8("VersionMajor", 1);
	SceneNode.WriteUInt8("VersionMinor", 0);

	ZEMLWriterNode EntitiesNode;
	SceneNode.OpenNode("Entities", EntitiesNode);

	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{	
		if (Entities[I]->GetClass() == NULL)
		{
			zeError("Serialization of entity \"%s\" has failed. Entity class is not registered." , Entities[I]->GetName());
			zeError("Serialization failed.");
			return false;
		}

		ZEMLWriterNode EntityNode;
		EntitiesNode.OpenNode("Entity", EntityNode);
		EntityNode.WriteString("Class", Entities[I]->GetClass()->GetName());

		if (!Entities[I]->Serialize(&EntityNode))
		{
			zeError("Serialization of entity \"%s\" has failed.", Entities[I]->GetName());
			zeError("Serialization failed.");
			EntityNode.CloseNode();
			EntitiesNode.CloseNode();
			SceneNode.CloseNode();
			Writer.Close();
			SceneFile.Close();
			return false;
		}

		EntityNode.CloseNode();
	}

	EntitiesNode.CloseNode();
	SceneNode.CloseNode();
	Writer.Close();
	SceneFile.Close();		
	zeLog("Scene file \"%s\" has been saved.", FileName.GetValue());

	return true;
}

bool ZEScene::Unserialize(const ZEString& FileName)
{
	zeLog("Loading scene file \"%s\".", FileName.GetValue());
	
	ZEFile SceneFile;
	if (!SceneFile.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot load scene. Cannot open scene file. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	ZEMLReader Reader;
	Reader.Open(&SceneFile);
	ZEMLReaderNode SceneNode = Reader.GetRootNode();

	if (SceneNode.GetName() != "ZEScene")
	{
		zeError("Cannot load scene. Corrupted scene file. File Name: \"%s\".", FileName.ToCString());
		SceneFile.Close();
		return false;
	}

	ClearEntities();

	ZEMLReaderNode EntitiesNode = SceneNode.GetNode("Entities");
	ZESize EntityCount = EntitiesNode.GetNodeCount("Entity");
	for (ZESize I = 0; I < EntityCount; I++)
	{
		ZEMLReaderNode EntityNode = EntitiesNode.GetNode("Entity", I);
		if (!EntityNode.IsValid())
		{
			zeError("Cannot load scene. Corrupted scene file. File Name: \"%s\".", FileName.ToCString());
			SceneFile.Close();
			return false;
		}
		
		ZEClass* EntityClass = ZEProvider::GetInstance()->GetClass(EntityNode.ReadString("Class"));
		if (EntityClass == NULL)
		{
			zeWarning("Problem in loading scene. Entity class is not registered. Class Name: \"%s\".", EntityNode.ReadString("Class").ToCString());
			continue;
		}

		ZEEntity* NewEntity = (ZEEntity*)EntityClass->CreateInstance();
		if (NewEntity == NULL)
		{
			zeError("Cannot load scene. Cannot create instance of an entity. Class Name: \"%s\".", EntityNode.ReadString("Class").ToCString());
			SceneFile.Close();
			return false; 
		}

		if (!NewEntity->Unserialize(&EntityNode))
		{
			zeError("Unserialization failed. Unserialization of entity has failed. Class Name: \"%s\".", EntityNode.ReadString("Class").ToCString());
			SceneFile.Close();
			return false;
		}

		AddEntity(NewEntity);
	}

	SceneFile.Close();

	zeLog("Scene file \"%s\" has been loaded.", FileName.GetValue());
	return true;
}

void ZEScene::LockScene()
{
	SceneLock.Lock();
}

void ZEScene::UnlockScene()
{
	SceneLock.Unlock();
}

ZEScene::ZEScene()
{
	SceneDirtyFlags.RaiseAll();
	LastEntityId = 0;
	ActiveCamera = NULL;
	ActiveListener = NULL;
	PhysicalWorld = NULL;
	Enabled = true;
	AmbientColor = ZEVector3::One;
	AmbientFactor = 0.0f;
}

ZEScene::~ZEScene()
{
	ClearEntities();

	if (PhysicalWorld != NULL)
	{
		PhysicalWorld->Destroy();
		PhysicalWorld = NULL;
	}
}

ZEScene* ZEScene::CreateInstance()
{
	return new ZEScene();
}
