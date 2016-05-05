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

#include "ZEGame.h"
#include "ZEEntity.h"
#include "ZEEntityProvider.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZECamera.h"
#include "ZESound/ZESoundModule.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

#include <memory.h>
#include "ZERenderer/ZERNRenderParameters.h"

#define ZE_SDF_CONSTANT_BUFFER		0x01

void ZEScene::TickEntity(ZEEntity* Entity, float ElapsedTime)
{
	if (!Entity->GetEnabled())
		return;

	Entity->Tick(ElapsedTime);

	const ZEArray<ZEEntity*>& Components = Entity->GetComponents();
	for (ZESize N = 0; N < Components.GetCount(); N++)
		TickEntity(Components[N], ElapsedTime);


	const ZEArray<ZEEntity*>& SubEntities = Entity->GetChildEntities();
	for (ZESize N = 0; N < SubEntities.GetCount(); N++)
		TickEntity(SubEntities[N], ElapsedTime);

}

void ZEScene::PreRenderEntity(ZEEntity* Entity, ZERNPreRenderParameters* Parameters)
{
	if (!Entity->GetVisible())
		return;

	ZEUInt EntityDrawFlags = Entity->GetDrawFlags();

	if ((EntityDrawFlags & ZE_DF_DRAW) == ZE_DF_DRAW)
	{
		if ((EntityDrawFlags & ZE_DF_CULL) == ZE_DF_CULL && Parameters->View->ViewVolume != NULL)
		{
			if (!Parameters->View->ViewVolume->CullTest(Entity->GetWorldBoundingBox()))
				Entity->PreRender(Parameters);
		}
		else
		{
			Entity->PreRender(Parameters);
		}
	}

	const ZEArray<ZEEntity*>& Components = Entity->GetComponents();
	for (ZESize I = 0; I < Components.GetCount(); I++)
		PreRenderEntity(Components[I], Parameters);

	const ZEArray<ZEEntity*>& ChildEntities = Entity->GetChildEntities();
	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		PreRenderEntity(ChildEntities[I], Parameters);
}

void ZEScene::RayCastEntity(ZEEntity* Entity, ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
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
	zeCheckError(Entity->GetOwnerScene() != NULL, ZE_VOID, "Entity is already added to a scene.");

	Entity->SetEntityId(LastEntityId++);
	Entities.Add(Entity);
	Entity->SetOwnerScene(this);

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(Entity->GetClass()->GetName() + ZEString(LastEntityId));

	Entity->Initialize();
}

void ZEScene::RemoveEntity(ZEEntity* Entity)
{
	zeCheckError(Entity->GetOwnerScene() != this, ZE_VOID, "Entity does not belong to this scene.");

	if (Entity->GetOwner() != NULL)
		Entity->GetOwner()->RemoveChildEntity(Entity);
	else
		Entities.RemoveValue(Entity);

	Entity->SetOwnerScene(NULL);
}

void ZEScene::ClearEntities()
{
	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();

	Entities.Clear();
}

bool ZEScene::Initialize()
{
	if (Initialized)
		return true;

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
	{
		Entities[I]->SetOwnerScene(this);
		Entities[I]->Initialize();
	}

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	SceneDirtyFlags.RaiseAll();

	Initialized = true;
	return true;
}

void ZEScene::Deinitialize()
{
	if (!Initialized)
		return;

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Deinitialize();

	if (PhysicalWorld != NULL)
		PhysicalWorld->Deinitialize();

	ConstantBuffer.Release();

	SceneDirtyFlags.RaiseAll();

	Initialized = false;
}

void ZEScene::Destroy()
{
	Deinitialize();

	ClearEntities();

	if (PhysicalWorld != NULL)
	{
		PhysicalWorld->Destroy();
		PhysicalWorld = NULL;
	}

	delete this;
}

void ZEScene::Tick(float ElapsedTime)
{
	if (!Enabled)
		return;

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		TickEntity(Entities[I], ElapsedTime);
}

void ZEScene::PreRender(ZERNRenderer* Renderer)
{
	ZERNPreRenderParameters Parameters;
	Parameters.Renderer = Renderer;
	Parameters.View = &Renderer->GetView();

	Renderer->StartScene(GetConstantBuffer());
	
	for (ZESize I = 0; I < Entities.GetCount(); I++)
		PreRenderEntity(Entities[I], &Parameters);

	Renderer->EndScene();
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

bool ZEScene::Save(const ZEString& FileName)
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

		if (!Entities[I]->Save(&EntityNode))
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

bool ZEScene::Load(const ZEString& FileName)
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

		if (!NewEntity->Restore(&EntityNode))
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

ZEScene::ZEScene()
{
	SceneDirtyFlags.RaiseAll();
	Initialized = false;
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
	Deinitialize();

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();
}

ZEScene* ZEScene::GetInstance()
{
	return zeGame->GetScene();
}
