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

#include "ZEGame.h"
#include "ZETypes.h"
#include "ZEScene.h"
#include "ZEError.h"
#include "ZEEntity.h"
#include "ZEMath/ZERay.h"
#include "ZEFile/ZEFile.h"
#include "ZECore/ZECore.h"
#include "ZESceneCuller.h"
#include "ZEDrawParameters.h"
#include "ZECore/ZEConsole.h"
#include "ZEEntityProvider.h"
#include "ZEGraphics/ZELight.h"
#include "ZEGraphics/ZECamera.h"
#include "ZESound/ZESoundModule.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZEGraphics/ZEFrameRenderer.h"
#include "ZEGraphics/ZEShadowRenderer.h"
#include "ZEInterior/ZEInterior.h"
#include "ZESerialization/ZEFileSerializer.h"
#include "ZESerialization/ZEFileUnserializer.h"
#include "ZEInterior/ZEInteriorResource.h"

#include <memory.h>

ZE_OBJECT_IMPL(ZEScene)

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

bool ZEScene::Initialize()
{
	if (Initialized)
		return true;

	if (Renderer == NULL)
		Renderer = ZEFrameRenderer::CreateInstance();

	if (Renderer == NULL)
	{
		zeCriticalError("Can not create renderer.");
		return false;
	}

	if (!Renderer->Initialize())
	{
		zeCriticalError("Can not initialize renderer.");
		return false;
	}

	if (ShadowRenderer == NULL)
		ShadowRenderer = ZEShadowRenderer::CreateInstance();

	if (ShadowRenderer == NULL)
	{
		zeCriticalError("Can not create shadow renderer.");
		return false;
	}
	
	if (!ShadowRenderer->Initialize())
	{
		zeCriticalError("Can not initialize shadow renderer.");
		return false;
	}

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
	if (Entities.Exists(Entity))
	{
		zeError("Can not add an already native entity.");
		return;
	}

	Entity->SetEntityId(LastEntityId++);
	Entities.Add(Entity);
	Entity->SetOwnerScene(this);

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(Entity->GetClass()->GetName() + ZEString(LastEntityId));

	Entity->Initialize();
}

void ZEScene::RemoveEntity(ZEEntity* Entity)
{
	if (!Entities.Exists(Entity))
	{
		zeError("Can not remove a foreign entity.");
		return;
	}

	Entity->SetOwnerScene(NULL);

	Entities.DeleteValue(Entity);
}

void ZEScene::ClearEntities()
{
	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();

	Entities.Clear();
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

ZERenderer* ZEScene::GetRenderer()
{
	return Renderer;
}

ZERenderer*	ZEScene::GetShadowRenderer()
{
	return ShadowRenderer;
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

ZESceneCuller& ZEScene::GetSceneCuller()
{
	return Culler;
}

const ZESceneStatistics& ZEScene::GetStatistics() const
{
	return Culler.GetStatistics();
}

ZEListener* ZEScene::GetActiveListener()
{
	return ActiveListener;
}

void ZEScene::Tick(ZEEntity* Entity, float ElapsedTime)
{
	if (!Entity->GetEnabled())
		return;
	
	Entity->Tick(ElapsedTime);
	
	const ZEArray<ZEEntity*>& Components = Entity->GetComponents();
	for (ZESize N = 0; N < Components.GetCount(); N++)
		Tick(Components[N], ElapsedTime);


	const ZEArray<ZEEntity*>& SubEntities = Entity->GetChildEntities();
	for (ZESize N = 0; N < SubEntities.GetCount(); N++)
		Tick(SubEntities[N], ElapsedTime);

}

void ZEScene::Tick(float ElapsedTime)
{
	if (!Enabled)
		return;

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		Tick(Entities[I], ElapsedTime);
}

void ZEScene::Render(float ElapsedTime)
{
	if (ActiveCamera == NULL)
		return;

	Renderer->SetCamera(ActiveCamera);
	
	FrameDrawParameters.ElapsedTime = ElapsedTime;
	FrameDrawParameters.FrameId = zeCore->GetFrameId();
	FrameDrawParameters.Pass = ZE_RP_COLOR;
	FrameDrawParameters.Renderer = Renderer;
	FrameDrawParameters.ViewVolume = (ZEViewVolume*)&ActiveCamera->GetViewVolume();
	FrameDrawParameters.View = (ZEView*)&ActiveCamera->GetView();
	FrameDrawParameters.Lights.Clear();

	memset(&FrameDrawParameters.Statistics, 0, sizeof(ZEDrawStatistics));

	FrameDrawParameters.Renderer->SetDrawParameters(&FrameDrawParameters);

	Culler.CullScene(this, &FrameDrawParameters);

	//Fill Draw Parameters Statistics ZESceneStats section
}

bool ZEScene::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	bool Result = false;
	if (!Parameters.FilterFunction.IsNull())
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			if (Parameters.FilterFunction(Entities[I], Parameters.FilterFunctionParameter))
				Result |= Entities[I]->RayCast(Report, Parameters);
		}
	}
	else
	{
		for (ZESize I = 0; I < Entities.GetCount(); I++)
			Result |= Entities[I]->RayCast(Report, Parameters);
	}

	return Result;
}

bool ZEScene::Save(const ZEString& FileName)
{
	zeLog("Saving scene file \"%s\".", FileName.GetValue());

	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile Serializer;
	if (Serializer.Open(NewPath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		ZEUInt32 EntityCount = (ZEUInt32)Entities.GetCount();
		Serializer.Write(&EntityCount, sizeof(ZEUInt32), 1);
		
		Serializer.Write(&LastEntityId, sizeof(ZEInt), 1);

		// Will be removed
		char NameBuffer[ZE_MAX_NAME_SIZE];
		memset(NameBuffer, 0, ZE_MAX_NAME_SIZE);
		Serializer.Write(&NameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE);

		for (ZESize I = 0; I < Entities.GetCount(); I++)
		{
			char NameBuffer[ZE_MAX_NAME_SIZE];
			memset(NameBuffer, 0, ZE_MAX_NAME_SIZE);
			strcpy(NameBuffer, Entities[I]->GetClass()->GetName());
			Serializer.Write((void*)NameBuffer, sizeof(char), ZE_MAX_NAME_SIZE);

			//ZEMETADEBUGCHECK!!!
			/*if (!Entities[I]->Serialize((ZESerializer*)&Serializer))
			{
				zeError("Serialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Serialization failed.");
				Serializer.Close();
				return false;
			}*/
		}

		Serializer.Close();
		
		zeLog("Scene file \"%s\" saved.", NewPath.GetValue());
		Serializer.Close();
		return true;
	}
	else
	{
		zeError("Serialization failed.");
		return false;
	}
}

bool ZEScene::Load(const ZEString& FileName)
{
	zeLog("Loading scene file \"%s\".", FileName.GetValue());

	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile Unserializer;
	char EntityTypeName[ZE_MAX_NAME_SIZE];

	if (Unserializer.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE))
	{
		ZEUInt32 EntityCount;
		Unserializer.Read(&EntityCount, sizeof(ZEUInt32), 1);
		Unserializer.Read(&LastEntityId, sizeof(ZEInt), 1);

		char MapFile[ZE_MAX_FILE_NAME_SIZE];
		Unserializer.Read(MapFile, sizeof(char), ZE_MAX_FILE_NAME_SIZE);

		ClearEntities();

		for (ZESize I = 0; I < (ZESize)EntityCount; I++)
		{
			ZEEntity* NewEntity;
			Unserializer.Read(EntityTypeName, sizeof(char), ZE_MAX_NAME_SIZE);
			//ZEMETADEBUGCHECK!!!
			//NewEntity = ZEProvider::CreateInstance(EntityTypeName);
			//NewEntity = (ZEEntity*)ZEEntityProvider::GetInstance()->CreateInstance(EntityTypeName);
			if (NewEntity == NULL)
			{
				zeError("Unserialization can not create entity type \"%s\".", EntityTypeName);
				zeError("Unserialization failed.");
				return false;
			}

			//ZEMETADEBUGCHECK!!!
			/*if (!NewEntity->Unserialize((ZEUnserializer*)&Unserializer))
			{
				zeError("Unserialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Unserialization failed.");
				Unserializer.Close();
				return false;
			}*/

			AddEntity(NewEntity);
		}

		zeLog("Scene file \"%s\" has been loaded.", NewPath.GetValue());
		Unserializer.Close();
		return true;
	}
	else
	{
		zeError("Can not open scene file. Unserialization failed. FileName : \"%s\"", NewPath.GetValue());
		return false;
	}
}

void ZEScene::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
}

float ZEScene::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEScene::SetAmbientColor(ZEVector3 Color)
{
	AmbientColor = Color;
}

const ZEVector3& ZEScene::GetAmbientColor() const
{
	return AmbientColor;
}

ZEScene::ZEScene()
{
	Initialized = false;
	LastEntityId = 0;
	PostProcessor = NULL;
	ShadowRenderer = NULL;
	Renderer = NULL;
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

void ZEScene::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEScene::GetEnabled() const
{
	return Enabled;
}
