//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Scene.cpp
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

#include "Scene.h"
#include "Game.h"
#include "Core/Console.h"
#include "Core/Error.h"
#include "Graphics/FrameBufferRenderer.h"
#include "Graphics/ShadowRenderer.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Serialization.h"
#include "ZEMath/Ray.h"
#include "Physics/PhysicalWorld.h"
#include "Sound/SoundModule.h"
#include <memory.h>

void ZEScene::SetVisualDebugElements(ZEDWORD VisualDebugElements)
{
	this->VisualDebugElements = VisualDebugElements;
}

ZEDWORD ZEScene::GetVisualDebugElements()
{
	return this->VisualDebugElements;
}

bool ZEScene::Initialize()
{
	Deinitialize();

	if (Renderer == NULL)
		Renderer = ZEFrameBufferRenderer::CreateInstance();

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
	
	if (!DebugDraw.Initialize())
		zeError("Scene", "Can not initialize scene debug draw.");

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

	if (Environment != NULL)
		Environment->Deinitialize();

	DebugDraw.Deinitialize();

	if (Renderer != NULL)
		Renderer->Deinitialize();

	if (ShadowRenderer != NULL)
		ShadowRenderer->Deinitialize();

	Initialized = false;
	
}

void ZEScene::Destroy()
{
	Deinitialize();

	if (Environment != NULL)
	{
		Environment->Destroy();
		Environment = NULL;
	}
	
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

	for (size_t I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();
	Entities.Clear();

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
	Entity->Deinitialize();
	Entities.DeleteValue(Entity);
}

const ZESmartArray<ZEEntity*>& ZEScene::GetEntities()
{
	return Entities;
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
	CullScene(Renderer, ActiveCamera->GetViewVolume(), true);
}

ZEEntity* ZEScene::CastRay(const ZERay& Ray, float Range)
{
	float MinT, MaxT, CurrMinT = Range / Ray.v.Length();

	ZEEntity* RayCaster = ActiveCamera->GetOwner();
	ZEEntity* IntersectedEntity = NULL;

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];

		if (CurrentEntity == RayCaster)
			continue;

		if (ZEAABoundingBox::IntersectionTest(CurrentEntity->GetWorldBoundingBox(),Ray,MinT,MaxT))
		{
			if (MinT < CurrMinT)
			{
				IntersectedEntity = CurrentEntity;
				CurrMinT = MinT;
			}
		}
	}

	return IntersectedEntity;
}

bool ZEScene::CastRay(const ZERay& Ray, float Range, ZEEntity** IntersectedEntity, ZEVector3& Position, ZEVector3& Normal)
{
	float MinT, MaxT, CurrMinT = Range / Ray.v.Length();
	*IntersectedEntity = NULL;

	ZEEntity* RayCaster = ActiveCamera->GetOwner();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];

		if (CurrentEntity == RayCaster)
			continue;

		if (CurrentEntity->CastRay(Ray,Range,MinT))
			if (MinT < CurrMinT)
			{
				CurrMinT = MinT;
				*IntersectedEntity = CurrentEntity;
			}
	}

	if (Environment != NULL)
	{
		if (Environment->CastRay(Ray, Position, Normal, MinT))
			if (MinT < CurrMinT)
			{
				*IntersectedEntity = NULL;
				return true;
			}

		Ray.GetPointOn(Position, MinT);
	}

	return *IntersectedEntity != NULL;
}

const ZECullStatistics& ZEScene::GetCullStatistics()
{
	return CullStatistics;
}


void ZEScene::CullScene(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, bool LightsEnabled)
{
	DebugDraw.Clean();

	// Zero statistical data
	memset(&CullStatistics, 0, sizeof(ZECullStatistics));

	// Step 1 : Find all light sources that can have effect on visible area
	ZESmartArray<ZELight*> VisibleLights; // List of lights that can have effect on visible area
	
	// Check lightning enabled
 	if (LightsEnabled)
		for (size_t I = 0; I < Entities.GetCount(); I++)
			// Check wheather entity is light source or not. (Does it contains light component(s))
			if (Entities[I]->GetDrawFlags() & ZE_DF_LIGHT_SOURCE)
			{
				const ZEArray<ZEComponent*>& Components = Entities[I]->GetComponents();
				
				// Loop throught current entity's components
				for (size_t N = 0; N < Components.GetCount(); N++)
				{
					ZEComponent* Component = Components[N];
						
					// Check entities component is light source or not. If light source then test its light volume is visible in camera's view volume
					if ((Component->GetDrawFlags() & ZE_DF_LIGHT_SOURCE))
					{
						CullStatistics.TotalLightCount++;

						if (!ViewVolume.LightCullTest((ZELight*)Component))
						{
							CullStatistics.VisibleLightCount++;
							// If visual debug elements enabled then visualize lights range
							if (VisualDebugElements & ZE_VDE_LIGHT_RANGE)
								DebugDraw.DrawBoundingSphere(ZEBoundingSphere(Component->GetWorldPosition(), ((ZELight*)Component)->GetRange()), Renderer, ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));

							// If light is casting shadows generate shadow maps of the light
							if (((ZELight*)Component)->GetCastsShadows())
								((ZELight*)Component)->RenderShadowMap(this, ShadowRenderer);

							// Add light to visible lights list.
							VisibleLights.Add((ZELight*)Component);
						}
					}
				}
			}


	// Step 2 : Draw entities and their components
	ZESmartArray<ZELight*> EntityLights; // List of lights that affect particular entity
	ZESmartArray<const ZERLLight*> Lights; 

	// Loop throught scene entities;
	CullStatistics.TotalEntityCount = Entities.GetCount();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];
		
		CullStatistics.TotalComponentCount += CurrentEntity->GetComponents().GetCount();

		ZEDWORD EntityDrawFlags = CurrentEntity->GetDrawFlags();

		// Check wheather entity is drawable and visible
		if ((EntityDrawFlags & ZE_DF_DRAW) && CurrentEntity->GetVisible())
		{
			CullStatistics.DrawableEntityCount++;

			// If entity is cullable, test it with view volume. If entity is not in view volume than discard it
			if (EntityDrawFlags & ZE_DF_CULL && ViewVolume.CullTest(CurrentEntity))		
			{
				CullStatistics.CulledEntityCount++;
				continue;
			}

			CullStatistics.VisibleEntityCount++;

			// Step 2.5 : Find lights that have effect on entity.
			EntityLights.Clear();
			Lights.Clear();
			
			// Loop through lights that has effect on view area. Lights that tested and passed on Step 1
			for (size_t M = 0; M < VisibleLights.GetCount(); M++)
			{
				const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

				// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
				if (VisibleLights[M]->GetLightType() == ZE_LT_DIRECTIONAL || !LightViewVolume.CullTest(CurrentEntity))
				{
					// Add light to list which contains lights effect entity
					Lights.Add(VisibleLights[M]->GetRenderOrderLight());
					EntityLights.Add(VisibleLights[M]);
				}
			}

			if (CullStatistics.MaxLightPerEntity < EntityLights.GetCount())
				CullStatistics.MaxLightPerEntity = EntityLights.GetCount();

			// Draw visual debug elements local and axis aligned entites bounding boxes and bounding sphere if enabled
			if (VisualDebugElements & ZE_VDE_ENTITY_ORIENTED_BOUNDINGBOX)
				DebugDraw.DrawOrientedBoundingBox(CurrentEntity->GetLocalBoundingBox(), CurrentEntity->GetWorldTransform(), Renderer, ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));

			if (VisualDebugElements & ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX)
				DebugDraw.DrawAxisAlignedBoundingBox(CurrentEntity->GetWorldBoundingBox(), Renderer, ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));

			if (VisualDebugElements & ZE_VDE_ENTITY_BOUNDINGSPHERE)
				DebugDraw.DrawBoundingSphere(CurrentEntity->GetWorldBoundingSphere(), Renderer, ZEVector4(0.25f, 0.25f, 0.25f, 1.0f));

			// Call entity's draw routine to make it draw it self
			CurrentEntity->Draw(Renderer, Lights);

			// Check wheather entity has drawable components or not
			if (EntityDrawFlags & ZE_DF_DRAW_COMPONENTS)
			{
				const ZEArray<ZEComponent*> Components = CurrentEntity->GetComponents();

				// Loop throught entity's components
				for (size_t N = 0; N < Components.GetCount(); N++)
				{
					ZEComponent* Component = Components[N];

					// Check wheather component is drawable and visible also if it is cullable, test it with view volume
					if ((Component->GetDrawFlags() & ZE_DF_DRAW))
					{
					
						CullStatistics.DrawableComponentCount++;

						if (Component->GetVisible())
						{
							CullStatistics.VisibleComponentCount++;

							if (ViewVolume.CullTest(Component))
							{
								CullStatistics.CulledComponentCount++;
								continue;
							}

							Lights.Clear();
							// Loop throught the lights affecting entity in order to find lights affecting component
							for (size_t M = 0; M < EntityLights.GetCount(); M++)
							{
								// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
								const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

								if (VisibleLights[M]->GetLightType() == ZE_LT_DIRECTIONAL || !LightViewVolume.CullTest(Components[N]))
									Lights.Add(VisibleLights[M]->GetRenderOrderLight());
							}

							if (CullStatistics.MaxLightPerComponent < Lights.GetCount())
								CullStatistics.MaxLightPerComponent = Lights.GetCount();

							// Draw bounding volumes of the components if enabled
							if (VisualDebugElements & ZE_VDE_COMPONENT_ORIENTED_BOUNDINGBOX)
								DebugDraw.DrawOrientedBoundingBox(Component->GetLocalBoundingBox(), Component->GetWorldTransform(), Renderer, ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));

							if (VisualDebugElements & ZE_VDE_COMPONENT_AXISALIGNED_BOUNDINGBOX)
								DebugDraw.DrawAxisAlignedBoundingBox(Component->GetWorldBoundingBox(), Renderer, ZEVector4(0.5f, 0.5f, 0.0f, 1.0f));

							if (VisualDebugElements & ZE_VDE_COMPONENT_BOUNDINGSPHERE)
								DebugDraw.DrawBoundingSphere(Component->GetWorldBoundingSphere(), Renderer, ZEVector4(0.25f, 0.25f, 0.0f, 1.0f));
							
							// Call components's draw routine to make it draw it self
							Component->Draw(Renderer, Lights);
						}
					}
				}
			}
		}
	}
	
	if (Environment != NULL)
		Environment->Render(Renderer, ViewVolume, VisibleLights);
	
	if (VisualDebugElements != NULL)
		DebugDraw.Draw(Renderer);
}


bool ZEScene::LoadEnvironment(const char* FileName)
{
	zeLog("Scene", "Loading environment.");

	if (Environment != NULL)
		Environment->Deinitialize();
	else
		Environment = ZEPortalMap::CreateInstance();

	if (!Environment->Load(FileName))
	{
		zeError("Scene", "Can not load environment.");
		return false;
	}

	if (!Environment->Initialize())
	{
		zeError("Scene", "Can not initialize environment.");
		return false;
	}
	
	zeLog("Scene", "Environment loaded.");
	return true;
}

bool ZEScene::Save(const char* FileName)
{
	ZEFileSerializer Serializer;
	if (Serializer.OpenFile(FileName))
	{
		ZEDWORD EntityCount = Entities.GetCount();
		Serializer.Write(&EntityCount, sizeof(ZEDWORD), 1);
		
		Serializer.Write(&LastEntityId, sizeof(int), 1);
		if (strcmp(Environment->GetFileName(), "") != 0)
			Serializer.Write(Environment->GetFileName(), sizeof(char), ZE_MAX_FILE_NAME_SIZE);
		else
		{
			char Temp[ZE_MAX_FILE_NAME_SIZE];
			Temp[0] = '\0';
			Serializer.Write(&Temp, sizeof(char), ZE_MAX_FILE_NAME_SIZE);
		}

		for (size_t I = 0; I < Entities.GetCount(); I++)
		{
			Serializer.Write((void*)Entities[I]->GetClassDescription()->GetType(), sizeof(char), ZE_MAX_NAME_SIZE);
			if (!Entities[I]->Serialize((ZESerializer*)&Serializer))
			{
				zeError("Scene", "Serialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Scene", "Serialization failed.");
				Serializer.CloseFile();
				return false;
			}
		}

		Serializer.CloseFile();
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
	zeLog("Scene", "Loading scene \"%s\".", FileName);
	ZEFileUnserializer Unserializer;
	char EntityTypeName[ZE_MAX_NAME_SIZE];
	if (Unserializer.OpenFile(FileName))
	{
		ZEDWORD EntityCount;
		Unserializer.Read(&EntityCount, sizeof(ZEDWORD), 1);

		Unserializer.Read(&LastEntityId, sizeof(int), 1);
		char MapFile[ZE_MAX_FILE_NAME_SIZE];
		Unserializer.Read(MapFile, sizeof(char), ZE_MAX_FILE_NAME_SIZE);
		Environment->Initialize();
		if (!Environment->Load(MapFile))
		{ 
			zeError("Scene", "Unserialization can not load map file. (Map File : \"%s\")", MapFile);
			zeError("Scene", "Unserialization failed.");
			return false;
		}

		Entities.Clear();
		Entities.SetCount(EntityCount);

		for (size_t I = 0; I < Entities.GetCount(); I++)
		{
			Unserializer.Read(EntityTypeName, sizeof(char), ZE_MAX_NAME_SIZE);
			Entities[I] = zeGame->CreateEntityInstance(EntityTypeName);
			if (Entities[I] == NULL)
			{
				zeError("Scene", "Unserialization can not create entity type \"%s\".", EntityTypeName);
				zeError("Scene", "Unserialization failed.");
				return false;
			}
			
			Entities[I]->Initialize();

			if (!Entities[I]->Unserialize((ZEUnserializer*)&Unserializer))
			{
				zeError("Scene", "Unserialization of entity \"%s\" has failed.", Entities[I]->GetName());
				zeError("Scene", "Unserialization failed.");
				return false;
			}
		}

		zeLog("Scene", "Scene loaded.");
		return true;
	}
	else
	{
		zeError("Scene", "Unserialization failed.");
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
	Environment = NULL;

	memset(&CullStatistics, 0, sizeof(ZECullStatistics));

	VisualDebugElements = ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX;
}

ZEScene::~ZEScene()
{
	Deinitialize();
}

ZEScene* ZEScene::GetInstance()
{
	return zeGame->GetScene();
}
