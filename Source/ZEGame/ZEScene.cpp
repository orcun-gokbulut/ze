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
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZECore.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZEShadowRenderer.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEGraphics\ZELight.h"
#include "ZEDrawParameters.h"
#include "ZESerialization.h"
#include "ZEMath\ZERay.h"
#include "ZEPhysics\ZEPhysicalWorld.h"
#include "ZESound\ZESoundModule.h"
#include "ZEMap\ZEPortalMap\ZEPortalMap.h"
#include "ZEMap\ZEPortalMap\ZEPortalMapResource.h"
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
		Renderer = ZERenderer::CreateInstance();

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

	if (Map != NULL)
		Map->Deinitialize();

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

	if (Map != NULL)
	{
		Map->Destroy();
		Map = NULL;
	}

	if (MapResource != NULL)
	{
		MapResource->Release();
		MapResource = NULL;
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
	/*float MinT, MaxT, CurrMinT = Range / Ray.v.Length();

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

	return IntersectedEntity;*/
	return NULL;
}

bool ZEScene::CastRay(const ZERay& Ray, float Range, ZEEntity** IntersectedEntity, ZEVector3& Position, ZEVector3& Normal)
{
/*	float MinT, MaxT, CurrMinT = Range / Ray.v.Length();
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

	if (Map != NULL)
	{
		if (Map->CastRay(Ray, Position, Normal, MinT))
			if (MinT < CurrMinT)
			{
				*IntersectedEntity = NULL;
				return true;
			}

		Ray.GetPointOn(Position, MinT);
	}

	return *IntersectedEntity != NULL;*/
	return NULL;
}

const ZECullStatistics& ZEScene::GetCullStatistics()
{
	return CullStatistics;
}


void ZEScene::CullScene(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, bool LightsEnabled)
{
	ZEDrawParameters DrawParameters;

	DrawParameters.ElapsedTime = zeCore->GetFrameTime();
	DrawParameters.FrameId = zeCore->GetFrameId();
	DrawParameters.Pass = ZE_RP_COLOR;
	DrawParameters.Renderer = Renderer;
	DrawParameters.ViewVolume = (ZEViewVolume*)&ActiveCamera->GetViewVolume();
	DrawParameters.View = (ZEView*)&ActiveCamera->GetView();
	DebugDraw.Clean();

	// Zero statistical data
	memset(&CullStatistics, 0, sizeof(ZECullStatistics));

	// Step 1 : Find all light sources that can have effect on visible area
	ZESmartArray<ZELight*> VisibleLights; // List of lights that can have effect on visible area
	
	// Check lightning enabled
 	if (LightsEnabled)
	{
		for (size_t I = 0; I < Entities.GetCount(); I++)
		{	// Check whether entity is light source or not. (Is it a light or does it contains light component(s) ?)
			if (Entities[I]->GetDrawFlags() & ZE_DF_LIGHT_SOURCE)
			{
				if (Entities[I]->GetEntityType() == ZE_ET_COMPONENT && Entities[I]->GetEnabled())
				{
					CullStatistics.TotalLightCount++;

					if (!ViewVolume.LightCullTest((ZELight*)Entities[I]))
					{
						CullStatistics.VisibleLightCount++;
						// If visual debug elements enabled then visualize lights range
						if (VisualDebugElements & ZE_VDE_LIGHT_RANGE)
							DebugDraw.DrawBoundingSphere(ZEBoundingSphere(Entities[I]->GetPosition(), ((ZELight*)Entities[I])->GetRange()), Renderer, ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));

						// If light is casting shadows generate shadow maps of the light
						if (((ZELight*)Entities[I])->GetCastsShadows())
							((ZELight*)Entities[I])->RenderShadowMap(this, ShadowRenderer);

						// Add light to visible lights list.
						VisibleLights.Add((ZELight*)Entities[I]);
					}
				}
				else if (Entities[I]->GetEntityType() == ZE_ET_COMPOUND && Entities[I]->GetEnabled())
				{
					const ZEArray<ZEComponent*>& Components = ((ZECompoundEntity*)Entities[I])->GetComponents();
					
					// Loop through current entity's components
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

				zeAssert(Entities[I]->GetEntityType() == ZE_ET_REGULAR, 
					"A regular entity claims that it is a light source. Please check entity's draw flags. (Entity Name : \"%s\", Entity Type : \"%s\")", 
					Entities[I]->GetName(), Entities[I]->GetClassDescription()->GetName());
			}
		}
	}

	Renderer->SetLights(VisibleLights);

	DrawParameters.Lights = VisibleLights;
	if (Map != NULL)
		Map->Render(&DrawParameters);

	// Step 2 : Draw entities and their components
	ZESmartArray<ZELight*> EntityLights; // List of lights that affect particular entity

	// Loop thought scene entities;
	CullStatistics.TotalEntityCount = Entities.GetCount();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];
		
		//CullStatistics.TotalComponentCount += CurrentEntity->GetComponents().GetCount();

		ZEDWORD EntityDrawFlags = CurrentEntity->GetDrawFlags();

		// Check whether entity is drawable and visible
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
			DrawParameters.Lights.Clear();
			
			// Loop through lights that has effect on view area. Lights that tested and passed on Step 1
			if (EntityDrawFlags & ZE_DF_LIGHT_RECIVER)
				for (size_t M = 0; M < VisibleLights.GetCount(); M++)
				{
					const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

					// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
					if (VisibleLights[M]->GetLightType() == ZE_LT_DIRECTIONAL || !LightViewVolume.CullTest(CurrentEntity))
					{
						// Add light to list which contains lights effect entity
						DrawParameters.Lights.Add(VisibleLights[M]);
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
			CurrentEntity->Draw(&DrawParameters);

			zeAssert(EntityDrawFlags & ZE_DF_DRAW_COMPONENTS && CurrentEntity->GetEntityType() != ZE_ET_COMPOUND, 
				"A non compound entity claims it has drawable components. Please check entity's draw flags. (Entity Name: \"%s\", Entity Type: \"%s\")", 
				CurrentEntity->GetName(),
				CurrentEntity->GetClassDescription()->GetName());

			zeAssert(EntityDrawFlags & ZE_DF_CULL_COMPONENTS && CurrentEntity->GetEntityType() != ZE_ET_COMPOUND, 
				"A non compound entity claims it has cullable components. Please check entity's draw flags. (Entity Name: \"%s\", Entity Type: \"%s\")", 
				CurrentEntity->GetName(),
				CurrentEntity->GetClassDescription()->GetName());

			// Check whether entity has drawable components or not
			if (EntityDrawFlags & ZE_DF_DRAW_COMPONENTS && CurrentEntity->GetEntityType() == ZE_ET_COMPOUND)
			{
				const ZEArray<ZEComponent*> Components = ((ZECompoundEntity*)CurrentEntity)->GetComponents();

				// Loop thought entity's components
				for (size_t N = 0; N < Components.GetCount(); N++)
				{
					ZEComponent* Component = Components[N];

					// Check whether component is drawable and visible also if it is cullable, test it with view volume
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

							DrawParameters.Lights.Clear();
							// Loop thought the lights affecting entity in order to find lights affecting component
							for (size_t M = 0; M < EntityLights.GetCount(); M++)
							{
								// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
								const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

								if (VisibleLights[M]->GetLightType() == ZE_LT_DIRECTIONAL || !LightViewVolume.CullTest(Components[N]))
									DrawParameters.Lights.Add(VisibleLights[M]);
							}

							if (CullStatistics.MaxLightPerComponent < DrawParameters.Lights.GetCount())
								CullStatistics.MaxLightPerComponent = DrawParameters.Lights.GetCount();

							// Draw bounding volumes of the components if enabled
							if (VisualDebugElements & ZE_VDE_COMPONENT_ORIENTED_BOUNDINGBOX)
								DebugDraw.DrawOrientedBoundingBox(Component->GetLocalBoundingBox(), Component->GetWorldTransform(), Renderer, ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));

							if (VisualDebugElements & ZE_VDE_COMPONENT_AXISALIGNED_BOUNDINGBOX)
								DebugDraw.DrawAxisAlignedBoundingBox(Component->GetWorldBoundingBox(), Renderer, ZEVector4(0.5f, 0.5f, 0.0f, 1.0f));

							if (VisualDebugElements & ZE_VDE_COMPONENT_BOUNDINGSPHERE)
								DebugDraw.DrawBoundingSphere(Component->GetWorldBoundingSphere(), Renderer, ZEVector4(0.25f, 0.25f, 0.0f, 1.0f));
							
							// Call component's draw routine to make it draw it self
							Component->Draw(&DrawParameters);
						}
					}
				}
			}
		}
	}

	if (VisualDebugElements != NULL)
		DebugDraw.Draw(Renderer);
}


bool ZEScene::LoadMap(const char* FileName)
{
	zeLog("Scene", "Loading map.");

	if (Map == NULL)
		Map = ZEPortalMap::CreateInstance();

	ZEPortalMapResource* MapResource = ZEPortalMapResource::LoadResource(FileName);
	if (MapResource == NULL)
	{
		zeError("Scene", "Can not load map file.");
		return false;
	}

	Map->SetResource(MapResource);
	if (!Map->Initialize())
	{
		zeError("Scene", "Can not initialize map.");
		return false;
	}
	
	zeLog("Scene", "Map loaded.");
	return true;
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
		if (MapResource != NULL)
			Serializer.Write(MapResource->GetFileName(), sizeof(char), ZE_MAX_FILE_NAME_SIZE);
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
		Map->Initialize();
		if (!LoadMap(MapFile))
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

		zeLog("Scene", "Scene file \"%s\" has been loaded.", FileName);
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
	MapResource = NULL;
	Map = NULL;

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





