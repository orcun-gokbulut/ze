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
#include "ZEGame.h"
#include "ZEEntity.h"
#include "ZECompoundEntity.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZECore.h"
#include "ZEGraphics/ZEFrameRenderer.h"
#include "ZEGraphics/ZEShadowRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZELight.h"
#include "ZEDrawParameters.h"
#include "ZESerialization/ZEFileSerializer.h"
#include "ZESerialization/ZEFileUnserializer.h"
#include "ZEMath/ZERay.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZESound/ZESoundModule.h"
#include "ZEMap/ZEPortalMap/ZEPortalMap.h"
#include "ZEMap/ZEPortalMap/ZEPortalMapResource.h"
#include "ZEEntityProvider.h"
#include "ZESceneCuller.h"

#include <memory.h>

bool ZEScene::Initialize()
{
	Deinitialize();

	if (Renderer == NULL)
		Renderer = ZEFrameRenderer::CreateInstance();

	if (Renderer == NULL)
	{
		zeCriticalError("Scene", "Can not create renderer.");
		return false;
	}

	if (!Renderer->Initialize())
	{
		zeCriticalError("Scene", "Can not initialize renderer.");
		return false;
	}

	if (ShadowRenderer == NULL)
		ShadowRenderer = ZEShadowRenderer::CreateInstance();

	if (Renderer == NULL)
	{
		zeCriticalError("Scene", "Can not create shadow renderer.");
		return false;
	}
	
	if (!ShadowRenderer->Initialize())
	{
		zeCriticalError("Scene", "Can not initialize shadow renderer.");
		return false;
	}

	if (PhysicalWorld == NULL)
		PhysicalWorld = ZEPhysicalWorld::CreateInstance();

	if (PhysicalWorld == NULL)
	{
		zeCriticalError("Scene", "Can not create physical world.");
		return false;
	}

	if (!PhysicalWorld->Initialize())
	{
		zeCriticalError("Scene", "Can not create physical world.");
		return false;
	}


	for (size_t I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Initialize();

	Initialized = true;
	return true;
}

void ZEScene::Deinitialize()
{
	for (size_t I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Deinitialize();

	if (PhysicalWorld != NULL)
		PhysicalWorld->Deinitialize();

	if (Renderer != NULL)
		Renderer->Deinitialize();

	if (ShadowRenderer != NULL)
		ShadowRenderer->Deinitialize();

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

	if (Renderer != NULL)
	{
		Renderer->Destroy();
		Renderer = NULL;
	}

	if (ShadowRenderer != NULL)
	{
		ShadowRenderer->Destroy();
		ShadowRenderer = NULL;
	}

	delete this;
}

void ZEScene::AddEntity(ZEEntity* Entity)
{
	Entity->SetEntityId(LastEntityId++);
	Entities.Add(Entity);
	Entity->Initialize();
}

void ZEScene::RemoveEntity(ZEEntity* Entity)
{
	Entity->Destroy();
	Entities.DeleteValue(Entity);
}

void ZEScene::ClearEntities()
{
	for (size_t I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();

	Entities.Clear();
}

const ZESmartArray<ZEEntity*>& ZEScene::GetEntities()
{
	return Entities;
}
/*
ZEArray<ZEEntity*> ZEScene::GetEntities(const char* ClassName)
{
	ZEArray<ZEEntity*> ProperEntities;
	ZEEntity* CurrentEntity = NULL;
	ZEClassDescription* CurrentDesc = NULL;
	ProperEntities.Clear();

	for (int I = 0; I < Entities.GetCount(); I++)
	{
		CurrentDesc = CurrentEntity->GetClassDescription();

		while(CurrentDesc != NULL)
		{
			if( strcmp(CurrentDesc->GetName(), ClassName) == 0 )

		}
	}
	
	return ProperEntities;
}*/

ZEArray<ZEEntity*> ZEScene::GetEntities(ZEClassDescription* Desc)
{
	ZEArray<ZEEntity*> ProperEntities;
	ZEEntity* CurrentEntity = NULL;
	ProperEntities.Clear();

	for (int I = 0; I < Entities.GetCount(); I++)
	{
		CurrentEntity = Entities[I];

		if (ZEClassDescription::CheckParent(Desc, CurrentEntity->GetClassDescription()))
			ProperEntities.Add(CurrentEntity);
	}

	return ProperEntities;
}

ZERenderer* ZEScene::GetRenderer()
{
	return Renderer;
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

void ZEScene::Tick(float ElapsedTime)
{
	for (size_t I = 0; I < Entities.GetCount(); I++)
		if (Entities[I]->GetEnabled())
			Entities[I]->Tick(ElapsedTime);
}

void ZEScene::Render(float ElapsedTime)
{
	if (ActiveCamera == NULL)
		return;

	Renderer->SetCamera(ActiveCamera);
	
	ZEDrawParameters DrawParameters;
	DrawParameters.ElapsedTime = zeCore->GetFrameTime();
	DrawParameters.FrameId = zeCore->GetFrameId();
	DrawParameters.Pass = ZE_RP_COLOR;
	DrawParameters.Renderer = Renderer;
	DrawParameters.ViewVolume = (ZEViewVolume*)&ActiveCamera->GetViewVolume();
	DrawParameters.View = (ZEView*)&ActiveCamera->GetView();
	DrawParameters.Lights.Clear();

	Culler.CullScene(this, &DrawParameters);
}

bool ZEScene::Save(const char* FileName)
{
	zeLog("Scene", "Saving scene file \"%s\".", FileName);

	ZEFileSerializer Serializer;
	if (Serializer.OpenFile(FileName))
	{
		ZEDWORD EntityCount = Entities.GetCount();
		Serializer.Write(&EntityCount, sizeof(ZEDWORD), 1);
		
		Serializer.Write(&LastEntityId, sizeof(int), 1);


		// Will be removed
		char NameBuffer[ZE_MAX_NAME_SIZE];
		memset(NameBuffer, 0, ZE_MAX_NAME_SIZE);
		Serializer.Write(&NameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE);


		for (size_t I = 0; I < Entities.GetCount(); I++)
		{
			char NameBuffer[ZE_MAX_NAME_SIZE];
			memset(NameBuffer, 0, ZE_MAX_NAME_SIZE);
			strcpy(NameBuffer, Entities[I]->GetClassDescription()->GetName());
			Serializer.Write((void*)NameBuffer, sizeof(char), ZE_MAX_NAME_SIZE);

			if (!Entities[I]->Serialize((ZESerializer*)&Serializer))
			{
				zeError("Scene", "Serialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Scene", "Serialization failed.");
				Serializer.CloseFile();
				return false;
			}
		}

		Serializer.CloseFile();
		
		zeLog("Scene", "Scene file \"%s\" saved.", FileName);

		return true;
	}
	else
	{
		zeError("Scene", "Serialization failed.");
		return false;
	}
}

bool ZEScene::Load(const char* FileName)
{
	zeLog("Scene", "Loading scene file \"%s\".", FileName);

	ZEFileUnserializer Unserializer;
	char EntityTypeName[ZE_MAX_NAME_SIZE];
	if (Unserializer.OpenFile(FileName))
	{
		ZEDWORD EntityCount;
		Unserializer.Read(&EntityCount, sizeof(ZEDWORD), 1);
		Unserializer.Read(&LastEntityId, sizeof(int), 1);

		char MapFile[ZE_MAX_FILE_NAME_SIZE];
		Unserializer.Read(MapFile, sizeof(char), ZE_MAX_FILE_NAME_SIZE);

		ClearEntities();

		for (size_t I = 0; I < EntityCount; I++)
		{
			ZEEntity* NewEntity;
			Unserializer.Read(EntityTypeName, sizeof(char), ZE_MAX_NAME_SIZE);
			NewEntity = (ZEEntity*)ZEEntityProvider::GetInstance()->CreateInstance(EntityTypeName);
			if (NewEntity == NULL)
			{
				zeError("Scene", "Unserialization can not create entity type \"%s\".", EntityTypeName);
				zeError("Scene", "Unserialization failed.");
				return false;
			}

			if (!NewEntity->Unserialize((ZEUnserializer*)&Unserializer))
			{
				zeError("Scene", "Unserialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Scene", "Unserialization failed.");
				return false;
			}

			AddEntity(NewEntity);
		}

		zeLog("Scene", "Scene file \"%s\" has been loaded.", FileName);
		return true;
	}
	else
	{
		zeError("Scene", "Can not open scene file. Unserialization failed. FileName : \"%s\"", FileName);
		return false;
	}
}

ZEScene::ZEScene()
{
	Initialized = false;
	LastEntityId = 0;
	ShadowRenderer = NULL;
	Renderer = NULL;
	ActiveCamera = NULL;
	ActiveListener = NULL;
	PhysicalWorld = NULL;
}

ZEScene::~ZEScene()
{
	Deinitialize();
}

ZEScene* ZEScene::GetInstance()
{
	return zeGame->GetScene();
}
