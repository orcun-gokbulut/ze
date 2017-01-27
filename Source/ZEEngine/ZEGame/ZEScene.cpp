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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZECamera.h"
#include "ZESound/ZESoundModule.h"
#include "ZEPhysics/ZEPhysicalWorld.h"

#include <memory.h>

#define ZE_SDF_CONSTANT_BUFFER		0x01

void ZEScene::TickEntity(ZEEntity* Entity, float ElapsedTime)
{
	if ((!Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM) && !Entity->IsInitialized()) || !Entity->GetEnabled())
		return;

	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM) && !Entity->IsInitialized(), "Ticking an entity which is not initialized.");
	zeDebugCheck(!Entity->GetEnabled(), "Ticking an entity which is not enabled.");

	Entity->Tick(ElapsedTime);
}

bool ZEScene::PreRenderEntity(ZEEntity* Entity, const ZERNPreRenderParameters* Parameters)
{
	if (!Entity->GetVisible())
		return false;

	zeDebugCheck(!Entity->GetVisible(), "PreRendering an entity which is not visible.");

	if (!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded())
		return false;

	if (!Entity->IsLoaded())
		return false;

	bool Cullable = Entity->GetEntityFlags().GetFlags(ZE_EF_CULLABLE);
	if (Cullable && Parameters->View->ViewVolume != NULL)
	{
		if (!Parameters->View->ViewVolume->IntersectionTest(Entity->GetWorldBoundingBox()))
			return false;

		Entity->PreRender(Parameters);
	}
	else
	{
		Entity->PreRender(Parameters);
	}

	return true;
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

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	SceneDirtyFlags.RaiseAll();

	return true;
}

bool ZEScene::DeinitializeInternal()
{
	DeinitializeEntities();

	if (PhysicalWorld != NULL)
		PhysicalWorld->Deinitialize();

	ConstantBuffer.Release();
	SceneDirtyFlags.RaiseAll();

	return ZEInitializable::DeinitializeInternal();
}

void ZEScene::AddToTickList(ZEEntity* Entity)
{
	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_TICKABLE_CUSTOM) && !Entity->IsInitialized(), "Ticking an entity which is not initialized.");
	zeDebugCheck(!Entity->GetEnabled(), "Ticking an entity which is not enabled.");
	 
	TickList.LockWrite();
	{
		if (Entity->TickListLink.GetInUse())
		{
			TickList.UnlockWrite();
			return;
		}

		TickList.AddEnd(&Entity->TickListLink);
	}
	TickList.UnlockWrite();
}

void ZEScene::RemoveFromTickList(ZEEntity* Entity)
{
	TickList.LockWrite();
	{
		if (!Entity->TickListLink.GetInUse())
		{
			TickList.UnlockWrite();
			return;
		}

		TickList.Remove(&Entity->TickListLink);
	}
	TickList.UnlockWrite();
}

void ZEScene::AddToRenderList(ZEEntity* Entity)
{
	RenderList.LockWrite();
	{
		zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded(), "Adding an entity to render list which is not loaded.");
		zeDebugCheck(!Entity->GetVisible(), "Adding an entity to render list which is not visible.");
	
		if (Entity->RenderListLink.GetInUse() || Entity->RenderListOctree != NULL)
		{
			RenderList.UnlockWrite();
			return;
		}

		if (SpatialDatabase && 
			Entity->GetEntityFlags().GetFlags(ZE_EF_STATIC_SUPPORT) && 
			Entity->GetStatic())
		{
			Entity->RenderListOctree = RenderListOctree.AddItem(Entity, Entity->GetWorldBoundingBox());
		}
		else
		{
			RenderList.AddEnd(&Entity->RenderListLink);
		}
	}
	RenderList.UnlockWrite();
}

void ZEScene::RemoveFromRenderList(ZEEntity* Entity)
{
	RenderList.LockWrite();
	{
		if (Entity->RenderListLink.GetInUse())
		{
			RenderList.Remove(&Entity->RenderListLink);
			Entity->RenderListOctree = NULL;
		}
		else if (Entity->RenderListOctree != NULL)
		{
			Entity->RenderListOctree->RemoveItem(Entity);
			Entity->RenderListOctree = NULL;
		}
	}
	RenderList.UnlockWrite();
}

void ZEScene::UpdateOctree(ZEEntity* Entity)
{
	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_CULLABLE), "Non-culluble entity updating octree.");
	zeDebugCheck(!Entity->GetStatic(), "Non-static entity updating octree.");
	zeDebugCheck(!Entity->GetEntityFlags().GetFlags(ZE_EF_RENDERABLE_CUSTOM) && !Entity->IsLoaded(), "Not loaded entity updating octree.");

	if (!SpatialDatabase)
		return;

	RenderList.LockWrite();
	{
		if (Entity->RenderListOctree != NULL)
			Entity->RenderListOctree->RemoveItem(Entity);

		Entity->RenderListOctree = RenderListOctree.AddItem(Entity, Entity->GetWorldBoundingBox());
	}
	RenderList.UnlockWrite();
}

void ZEScene::UpdateConstantBuffer()
{
	if (!SceneDirtyFlags.GetFlags(ZE_SDF_CONSTANT_BUFFER))
		return;

	Constants.AmbientColor = AmbientColor * AmbientFactor;

	ConstantBuffer->SetData(&Constants);

	SceneDirtyFlags.UnraiseFlags(ZE_SDF_CONSTANT_BUFFER);
}


void ZEScene::InitializeEntities()
{
	if (!IsInitialized())
		return;

	EntityState = ZE_ES_INITIALIZED;

	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
			Entities[I]->Initialize();
	}
	Entities.UnlockRead();
}

void ZEScene::DeinitializeEntities()
{
	if (!IsInitialized() && GetInitializationState() != ZE_IS_DEINITIALIZING)
		return;

	EntityState = ZE_ES_LOADED;

	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
			Entities[I]->Deinitialize();
	}
	Entities.UnlockRead();
}

void ZEScene::LoadEntities()
{
	if (!IsInitialized())
		return;

	EntityState = ZE_ES_LOADED;

	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
			Entities[I]->Load();
	}
	Entities.UnlockRead();
}

void ZEScene::UnloadEntities()
{
	if (!IsInitialized() && GetInitializationState() != ZE_IS_DEINITIALIZING)
		return;

	EntityState = ZE_ES_NONE;

	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
			Entities[I]->Unload();
	}
	Entities.UnlockRead();
}

const ZEGRBuffer* ZEScene::GetConstantBuffer()
{
	UpdateConstantBuffer();
	return ConstantBuffer;
}

ZEPhysicalWorld* ZEScene::GetPhysicalWorld()
{
	return PhysicalWorld;
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
	if (AmbientFactor == Factor)
		return;

	AmbientFactor = Factor;

	SceneDirtyFlags.RaiseFlags(ZE_SDF_CONSTANT_BUFFER);
}

float ZEScene::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEScene::SetAmbientColor(ZEVector3 Color)
{
	if (AmbientColor == Color)
		return;

	AmbientColor = Color;

	SceneDirtyFlags.RaiseFlags(ZE_SDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEScene::GetAmbientColor() const
{
	return AmbientColor;
}

void ZEScene::SetSpatialDatabase(bool Enabled)
{
	if (SpatialDatabase == Enabled)
		return;

	SpatialDatabase = Enabled;
}

bool ZEScene::GetSpatialDatabase()
{
	return SpatialDatabase;
}


ZEUInt ZEScene::GetLoadingPercentage()
{
	ZEEntityLoadingScore TotalScore;
	TotalScore.Score = 0;
	TotalScore.Count = 0;

	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			ZEEntityLoadingScore EntityScore = Entities[I]->GetLoadingScore();
			TotalScore.Score += EntityScore.Score;
			TotalScore.Count += EntityScore.Count;
		}
	}
	Entities.UnlockRead();

	if (TotalScore.Count == 0)
		return 100;
	else
		return (ZEUInt)(TotalScore.Score / TotalScore.Count);
}

const ZESmartArray<ZEEntity*>& ZEScene::GetEntities()
{
	return Entities;
}

ZEArray<ZEEntity*> ZEScene::GetEntities(ZEClass* Class, bool Recursive)
{
	ZEArray<ZEEntity*> Output;
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(Class, Entities[I]))
				Output.Add(Entities[I]);

			if (Recursive)
				Entities[I]->GetChildEntitiesInternal(Class, Output);
		}
	}
	Entities.UnlockRead();

	return Output;
}

ZEArray<ZEEntity*> ZEScene::GetEntities(const ZEString& Name, bool Recursive)
{
	ZEArray<ZEEntity*> Output;
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (Entities[I]->GetName() == Name)
				Output.Add(Entities[I]);

			if (Recursive)
				Entities[I]->GetChildEntitiesInternal(Name, Output);
		}
	}
	Entities.UnlockRead();

	return Output;
}


ZEEntity* ZEScene::GetEntity(const ZEString& Name, bool Recursive)
{
	ZEEntity* Entity = NULL;
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (Entities[I]->GetName() == Name)
			{
				Entity = Entities[I];
				Entities.UnlockRead();
				return Entity;
			}
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < Entities.GetCount(); I++)
			{
				Entity = Entities[I]->GetChildEntity(Name, true);
				if (Entity == NULL)
					continue;

				Entities.UnlockRead();
				return Entity;
			}
		}
	}
	Entities.UnlockRead();

	return NULL;
}

ZEEntity* ZEScene::GetEntity(ZEClass* Class, bool Recursive)
{
	ZEEntity* Entity = NULL;
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (!ZEClass::IsDerivedFrom(Class, Entities[I]))
				continue;

			Entity = Entities[I];
			Entities.UnlockRead();
			return Entity;
		}

		if (Recursive)
		{
			for (ZESize I = 0; I < Entities.GetCount(); I++)
			{
				Entity = Entities[I]->GetChildEntity(Class, true);
				if (Entity == NULL)
					continue;

				Entities.UnlockRead();
				return Entity;
			}
		}
	}
	Entities.UnlockRead();

	return NULL;
}

void ZEScene::AddEntity(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Entity is NULL.");
	zeCheckError(Entity->GetScene() != NULL, ZE_VOID, "Entity is already added to a scene.");

	Entities.LockWrite();
	{
		Entity->SetEntityId(LastEntityId++);
		Entities.Add(Entity);
	}
	Entities.UnlockWrite();
	
	Entity->SetScene(this);
	Entity->Load();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(Entity->GetClass()->GetName() + ZEString::FromUInt32(LastEntityId));

	if (EntityState == ZE_ES_INITIALIZED)
		Entity->Initialize();
	else if (EntityState == ZE_ES_LOADED)
		Entity->Load();
}

void ZEScene::RemoveEntity(ZEEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Entity is NULL.");
	zeCheckError(Entity->GetScene() != this, ZE_VOID, "Entity does not belong to this scene.");
	zeCheckError(Entity->GetParent() != NULL, ZE_VOID, "Entity is not a root entity.");

	Entity->Deinitialize();
	Entity->SetScene(NULL);

	Entities.LockWriteNested();
	{
		Entities.RemoveValue(Entity);
	}
	Entities.UnlockWrite();
}

void ZEScene::ClearEntities()
{
	Entities.LockWriteNested();
	{
		while(Entities.GetCount() != 0)
			Entities.GetFirstItem()->Destroy();
	}
	Entities.UnlockWrite();
}

bool ZEScene::IsEntitiesLoaded()
{
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (Entities[I]->IsFailed()) // Ignore
				continue;

			if (Entities[I]->IsLoaded())
				continue;

			Entities.UnlockRead();
			return false;
		}
	}
	Entities.UnlockRead();

	return true;
}

bool ZEScene::IsEntitiesInitialized()
{
	Entities.LockRead();
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (Entities[I]->IsFailed()) // Ignore
				continue;

			if (Entities[I]->IsInitialized())
				continue;

			Entities.UnlockRead();
			return false;
		}
	}
	Entities.UnlockRead();

	return true;
}

void ZEScene::Tick(float ElapsedTime)
{
	if (!Enabled)
		return;

	ZEGRGraphicsModule::GetInstance()->GetMainContext()->BeginEvent("SceneTick");
	
	ZEArray<ZEEntity*> LocalTickList;
	ZESize Index = 0;
	TickList.LockRead();
	{
		LocalTickList.SetCount(TickList.GetCount());

		ze_for_each(Entity, TickList)
		{
			LocalTickList[Index] = Entity.GetPointer();
			Index++;
		}
	}
	TickList.UnlockRead();

	ze_for_each(Entity, LocalTickList)
		TickEntity(Entity.GetItem(), ElapsedTime);


	ZEGRGraphicsModule::GetInstance()->GetMainContext()->EndEvent();
}

void ZEScene::PreRender(const ZERNPreRenderParameters* Parameters)
{
	ZEGRGraphicsModule::GetInstance()->GetMainContext()->BeginEvent("ScenePreRender");

	Parameters->Renderer->BeginScene(this);

	// Statistics
	ZESize TotalRenderableEntity = 0;
	
	ZESize RenderedEntity = 0;
	float RenderRatio = 0.0f;

	ZESize CulledEntity = 0;
	float CullRatio = 0.0f;
	
	ZESize OctreeProcessedEntity = 0;
	ZESize OctreeCulledEntity = 0;
	ZESize OctreeVisitedNodeCount = 0;
	float OctreeCullRatio = 0.0f;
	float OctreePerformanceRatio = 0.0f;

	ZESize ViewTestCulledEntity = 0;
	float ViewTestCullRatio = 0.0f;

	float TotalCullRatio = 0.0f;

	RenderList.LockRead();
	{
		TotalRenderableEntity = RenderListOctree.GetTotalItemCount() + RenderList.GetCount();

		ze_for_each_iterator(Node, RenderListOctree.Traverse(Parameters->View->ViewVolume))
		{
			OctreeVisitedNodeCount++;
			OctreeProcessedEntity += Node->GetItems().GetCount();
			for (ZESize I = 0; I < Node->GetItems().GetCount(); I++)
			{		
				ZEEntity* Entity = Node->GetItems()[I];
				if (PreRenderEntity(Entity, Parameters))
					RenderedEntity++;
				else
					ViewTestCulledEntity++;
			}
		}

		OctreeCulledEntity = RenderListOctree.GetTotalItemCount() - OctreeProcessedEntity;
		OctreeCullRatio = (float)OctreeCulledEntity / (float)RenderListOctree.GetTotalItemCount();
		OctreePerformanceRatio = 1.0f - (float)(OctreeProcessedEntity + OctreeVisitedNodeCount) / (float)RenderListOctree.GetTotalItemCount();

		ze_for_each(Entity, RenderList)
		{
			if (PreRenderEntity(Entity.GetPointer(), Parameters))
				RenderedEntity++;
			else
				ViewTestCulledEntity++;
		}
		ViewTestCullRatio = (float)ViewTestCulledEntity / (float)RenderList.GetCount();

		RenderRatio = (float)RenderedEntity / (float)TotalRenderableEntity;
		CulledEntity = TotalRenderableEntity - RenderedEntity;
		CullRatio = (float)CulledEntity / (float)TotalRenderableEntity;
	}
	RenderList.UnlockRead();

	Parameters->Renderer->EndScene();

	ZEGRGraphicsModule::GetInstance()->GetMainContext()->EndEvent();
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
	AmbientFactor = 0.1f;
	SpatialDatabase = false;
	EntityState = ZE_ES_NONE;

	Constants.AmbientColor = ZEVector3::Zero;

	RenderListOctree.SetBoundingBox(ZEAABBox(-32000.0f * ZEVector3::One, 32000.0f * ZEVector3::One));
	RenderListOctree.SetMaxDepth(8);
}

ZEScene::~ZEScene()
{
	Deinitialize();

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
