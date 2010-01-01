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
#include "Sound/SoundModule.h"
#include "Graphics/GraphicsModule.h"
#include "Core/Core.h"
#include "Serialization.h"
#include "ZEMath/Ray.h"
#include <memory.h>

void ZEScene::DrawOrientedBoundingBox(const ZEAABoundingBox& BoundingBox, const ZEMatrix4x4& Transform, ZERenderer* Renderer, ZEMaterial* Material)
{
	ZEMatrix4x4 LocalPivot;
	ZEMatrix4x4::CreateOrientation(LocalPivot, BoundingBox.GetCenter(), 
		ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f), 
		ZEVector3((BoundingBox.Max.x - BoundingBox.Min.x), 
			(BoundingBox.Max.y - BoundingBox.Min.y), 
			(BoundingBox.Max.z - BoundingBox.Min.z))
			);
	ZEMatrix4x4::Multiply(BoundingBoxRenderOrder.WorldMatrix, LocalPivot, Transform);
	BoundingBoxRenderOrder.Material = Material;
	Renderer->AddToRenderOrder(&BoundingBoxRenderOrder);
}

void ZEScene::DrawAxisAlignedBoundingBox(const ZEAABoundingBox& BoundingBox, ZERenderer* Renderer, ZEMaterial* Material)
{
	ZEMatrix4x4::CreateOrientation(BoundingBoxRenderOrder.WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f), 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z)
		);
	BoundingBoxRenderOrder.Material = Material;
	Renderer->AddToRenderOrder(&BoundingBoxRenderOrder);
}

void ZEScene::DrawBoundingSphere(const ZEBoundingSphere& BoundingSphere, ZERenderer* Renderer, ZEMaterial* Material)
{
	ZEMatrix4x4::CreateOrientation(BoundingSphereRenderOrder.WorldMatrix, BoundingSphere.Position, 
		ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f), 
		ZEVector3(BoundingSphere.Radius * 2.0f, BoundingSphere.Radius * 2.0f, BoundingSphere.Radius * 2.0f)
		);
	BoundingSphereRenderOrder.Material = Material;
	Renderer->AddToRenderOrder(&BoundingSphereRenderOrder);
}

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
	if (Renderer != NULL)
		Renderer->Destroy();

	/*ZETexture2D* Texture = zeGraphics->CreateTexture();
	Texture->Create(zeGraphics->GetScreenWidth(), zeGraphics->GetScreenHeight(), ZE_TPF_ARGB32, true);
	*/
	
	Renderer = zeGraphics->CreateFrameRenderer();
	if (Renderer == NULL)
		return false;
	
/*
	Renderer->SetOutput(Texture);
	
	PostProcessor = zeGraphics->CreatePostProcessor();
	PostProcessor->SetInput(Texture);*/

	/*Renderer->SetHDR(true, 1.0f, 0.2f);
	Renderer->SetHDRBloom(true, 1.0f, 1.0f, 5.0f);
	Renderer->SetHDRLightAdaptation(true, 0.99999f);*/

	if (ShadowRenderer != NULL)
		ShadowRenderer->Destroy();

	ShadowRenderer = zeGraphics->CreateShadowRenderer();
	if (Renderer == NULL)
		return false;

	ZECanvas Canvas;
	Canvas.AddWireframeBox(1.0f, 1.0f, 1.0f);

	BoundingBoxRenderOrder.SetZero();
	BoundingBoxRenderOrder.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_TRANSPARENT | ZE_RLF_ENABLE_ZCULLING;
	BoundingBoxRenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	BoundingBoxRenderOrder.PrimitiveType = ZE_RLPT_LINE;
	BoundingBoxRenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 2;

	if (BoundingBoxRenderOrder.VertexBuffer != NULL)
	{
		((ZEStaticVertexBuffer*)BoundingBoxRenderOrder.VertexBuffer)->Release();
		delete BoundingBoxRenderOrder.VertexBuffer;
	}
	
	BoundingBoxRenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();

	Canvas.Clean();
	Canvas.AddWireframeSphere(1.0f, 8, 8);
	BoundingSphereRenderOrder.SetZero();
	BoundingSphereRenderOrder.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_TRANSPARENT | ZE_RLF_ENABLE_ZCULLING;
	BoundingSphereRenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	BoundingSphereRenderOrder.PrimitiveType = ZE_RLPT_LINE;
	BoundingSphereRenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 2;

	if (BoundingSphereRenderOrder.VertexBuffer != NULL)
	{
		((ZEStaticVertexBuffer*)BoundingBoxRenderOrder.VertexBuffer)->Release();
		delete BoundingSphereRenderOrder.VertexBuffer;
	}

	BoundingSphereRenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();


	if (EntityAxisAlignedBoundingBoxMaterial != NULL)
	{
		EntityAxisAlignedBoundingBoxMaterial->Destroy();
		EntityAxisAlignedBoundingBoxMaterial = NULL;
	}

	EntityAxisAlignedBoundingBoxMaterial = ZEFixedMaterial::CreateInstance();
	EntityAxisAlignedBoundingBoxMaterial->SetZero();
	EntityAxisAlignedBoundingBoxMaterial->SetLightningEnabled(false);
	EntityAxisAlignedBoundingBoxMaterial->SetAmbientEnabled(true);
	EntityAxisAlignedBoundingBoxMaterial->SetAmbientColor(ZEVector3(1.0f, 1.0f, 0.0f));
	EntityAxisAlignedBoundingBoxMaterial->UpdateMaterial();


	if (EntityOrientedBoundingBoxMaterial != NULL)
	{
		EntityOrientedBoundingBoxMaterial->Destroy();
		EntityOrientedBoundingBoxMaterial = NULL;
	}

	EntityOrientedBoundingBoxMaterial = ZEFixedMaterial::CreateInstance();
	EntityOrientedBoundingBoxMaterial->SetZero();
	EntityOrientedBoundingBoxMaterial->SetLightningEnabled(false);
	EntityOrientedBoundingBoxMaterial->SetAmbientEnabled(true);
	EntityOrientedBoundingBoxMaterial->SetAmbientColor(ZEVector3(1.0f, 1.0f, 1.0f));
	EntityOrientedBoundingBoxMaterial->UpdateMaterial();


	if (EntityBoundingSphereMaterial != NULL)
	{
		EntityBoundingSphereMaterial->Destroy();
		EntityBoundingSphereMaterial = NULL;
	}

	EntityBoundingSphereMaterial = ZEFixedMaterial::CreateInstance();
	EntityBoundingSphereMaterial->SetZero();
	EntityBoundingSphereMaterial->SetLightningEnabled(false);
	EntityBoundingSphereMaterial->SetAmbientEnabled(true);
	EntityBoundingSphereMaterial->SetAmbientColor(ZEVector3(1.0f, 0.0f, 1.0f));
	EntityBoundingSphereMaterial->UpdateMaterial();


	if (ComponentAxisAlignedBoundingBoxMaterial != NULL)
	{
		ComponentAxisAlignedBoundingBoxMaterial->Destroy();
		ComponentAxisAlignedBoundingBoxMaterial = NULL;
	}

	ComponentAxisAlignedBoundingBoxMaterial = ZEFixedMaterial::CreateInstance();
	ComponentAxisAlignedBoundingBoxMaterial->SetZero();
	ComponentAxisAlignedBoundingBoxMaterial->SetLightningEnabled(false);
	ComponentAxisAlignedBoundingBoxMaterial->SetAmbientEnabled(true);
	ComponentAxisAlignedBoundingBoxMaterial->SetAmbientColor(ZEVector3(0.5f, 0.5f, 0.0f));
	ComponentAxisAlignedBoundingBoxMaterial->UpdateMaterial();


	if (ComponentOrientedBoundingBoxMaterial != NULL)
	{
		ComponentOrientedBoundingBoxMaterial->Destroy();
		ComponentOrientedBoundingBoxMaterial = NULL;
	}

	ComponentOrientedBoundingBoxMaterial = ZEFixedMaterial::CreateInstance();
	ComponentOrientedBoundingBoxMaterial->SetZero();
	ComponentOrientedBoundingBoxMaterial->SetLightningEnabled(false);
	ComponentOrientedBoundingBoxMaterial->SetAmbientEnabled(true);
	ComponentOrientedBoundingBoxMaterial->SetAmbientColor(ZEVector3(0.5f, 0.5f, 0.5f));
	ComponentOrientedBoundingBoxMaterial->UpdateMaterial();


	if (ComponentBoundingSphereMaterial != NULL)
	{
		ComponentBoundingSphereMaterial->Destroy();
		ComponentBoundingSphereMaterial = NULL;
	}

	ComponentBoundingSphereMaterial = ZEFixedMaterial::CreateInstance();
	ComponentBoundingSphereMaterial->SetZero();
	ComponentBoundingSphereMaterial->SetLightningEnabled(false);
	ComponentBoundingSphereMaterial->SetAmbientEnabled(true);
	ComponentBoundingSphereMaterial->SetAmbientColor(ZEVector3(0.5f, 0.0f, 0.5f));
	ComponentBoundingSphereMaterial->UpdateMaterial();


	if (LightRangeMaterial != NULL)
	{
		LightRangeMaterial->Destroy();
		LightRangeMaterial = NULL;
	}

	LightRangeMaterial = ZEFixedMaterial::CreateInstance();
	LightRangeMaterial->SetZero();
	LightRangeMaterial->SetLightningEnabled(false);
	LightRangeMaterial->SetAmbientEnabled(true);
	LightRangeMaterial->SetAmbientColor(ZEVector3(0.0f, 0.0f, 0.5f));
	LightRangeMaterial->UpdateMaterial();

	return true;
}

bool ZEScene::Deinitialize()
{
	for (size_t I = 0; I < Entities.GetCount(); I++)
		Entities[I]->Destroy();

	Entities.Clear();

	Environment.Destroy();
	if (EntityAxisAlignedBoundingBoxMaterial != NULL)
	{
		EntityAxisAlignedBoundingBoxMaterial->Destroy();
		EntityAxisAlignedBoundingBoxMaterial = NULL;
	}


	if (EntityOrientedBoundingBoxMaterial != NULL)
	{
		EntityOrientedBoundingBoxMaterial->Destroy();
		EntityOrientedBoundingBoxMaterial = NULL;
	}


	if (EntityBoundingSphereMaterial != NULL)
	{
		EntityBoundingSphereMaterial->Destroy();
		EntityBoundingSphereMaterial = NULL;
	}


	if (EntityBoundingSphereMaterial != NULL)
	{
		EntityBoundingSphereMaterial->Destroy();
		EntityBoundingSphereMaterial = NULL;
	}

	if (ComponentAxisAlignedBoundingBoxMaterial != NULL)
	{
		ComponentAxisAlignedBoundingBoxMaterial->Destroy();
		ComponentAxisAlignedBoundingBoxMaterial = NULL;
	}

	if (ComponentOrientedBoundingBoxMaterial != NULL)
	{
		ComponentOrientedBoundingBoxMaterial->Destroy();
		ComponentOrientedBoundingBoxMaterial = NULL;
	}

	if (ComponentBoundingSphereMaterial != NULL)
	{
		ComponentBoundingSphereMaterial->Destroy();
		ComponentBoundingSphereMaterial = NULL;
	}

	if (BoundingBoxRenderOrder.VertexBuffer != NULL)
	{
		((ZEStaticVertexBuffer*)BoundingBoxRenderOrder.VertexBuffer)->Destroy();
		BoundingBoxRenderOrder.VertexBuffer = NULL;
	}

	if (BoundingSphereRenderOrder.VertexBuffer != NULL)
	{
		((ZEStaticVertexBuffer*)BoundingSphereRenderOrder.VertexBuffer)->Destroy();
		BoundingSphereRenderOrder.VertexBuffer = NULL;
	}

	if (Renderer != NULL)
	{
		Renderer->Destroy();
		Renderer = NULL;
	}

	return true;
}

void ZEScene::Reset()
{

}

void ZEScene::Destroy()
{
	Deinitialize();
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
	zeSound->SetListener(Listener);
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
	Renderer->Render(ElapsedTime);
	Renderer->ClearList();
/*

	PostProcessor->ApplyGrayscale(ZE_PPS_INPUT, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurH(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurV(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurH(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurV(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurH(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurV(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurH(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurV(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurH(ZE_PPS_INTERNAL, ZE_PPD_INTERNAL);
	PostProcessor->ApplyBlurV(ZE_PPS_INTERNAL, ZE_PPD_FRAMEBUFFER);*/
}
/*
ZEEntity* ZEScene::CastRay(const ZERay& Ray, float Range, ZESmartArray<ZEEntity*>& IntersectedEntities)
{
	float MinT, MaxT, CurrMinT = Range / Ray.v.Length();

	IntersectedEntities.Clear();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];
		if (ZEBoundingSphere::IntersectionTest(CurrentEntity->GetWorldBoundingSphere(), Ray, MinT, MaxT))
			if (MinT < CurrMinT)
				IntersectedEntities.Add(CurrentEntity);
	}
}*/

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

	if (Environment.CastRay(Ray, Position, Normal, MinT))
		if (MinT < CurrMinT)
		{
			*IntersectedEntity = NULL;
			return true;
		}

	Ray.GetPointOn(Position,MinT);
	return *IntersectedEntity != NULL;
}

/*
bool ZEScene::CastRay(const ZERay& Ray, float Range)
{
	float MinT, MaxT, CurrMinT = Range / Ray.v.Length();
	*Entity = NULL;
	*Component = NULL;
	
	if (Filter & (ZE_RCF_ENTITY | ZE_RCF_COMPONENT))
		for (size_t I = 0; I < Entities.GetCount(); I++)
		{
			ZEEntity* CurrentEntity = Entities[I];
			if (ZEBoundingSphere::IntersectionTest(CurrentEntity->GetWorldBoundingSphere(), Ray)) // && ZEAABoundingBox::IntersectionTest(CurrentEntity->GetWorldBoundingBox(), Ray))
				for (size_t N = 0; N < CurrentEntity->GetComponents().GetCount(); N++)
				{
					ZEComponent* CurrentComponent = CurrentEntity->GetComponents()[N];
					if (ZEBoundingSphere::IntersectionTest(CurrentComponent->GetWorldBoundingSphere(), Ray, MinT, MaxT)) // && ZEAABoundingBox::IntersectionTest(CurrentComponent->GetWorldBoundingBox(), Ray))
						if (Filter & ZE_RCF_ CurrentComponent->CastRay(Ray, Position, Normal, TEnterance, TExit))
							if (CurrMinT > MinT)
							{
								CurrMinT = Mint;
								*Component = CurrentComponent;
								*Entity = CurrentEntity;
							}
				}
		}

	if (Filter & (ZE_RCF_MAP)
		if (Environment.CastRay(Ray, Position, Normal, TEnterance, TExit))
			if (MinT > TEnterance)
			{
				*Component = NULL;
				*Entity = NULL;
				return true;
			}

	return (*Component == NULL);
}*/

void ZEScene::CullScene(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, bool LightsEnabled)
{
	
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
					if ((Component->GetDrawFlags() & ZE_DF_LIGHT_SOURCE) && ViewVolume.LightCullTest((ZELight*)Component))
					{

						// If visual debug elements enabled then visualize lights range
						if (VisualDebugElements & ZE_VDE_LIGHT_RANGE)
							DrawBoundingSphere(ZEBoundingSphere(Component->GetWorldPosition(), ((ZELight*)Component)->GetRange()), Renderer, LightRangeMaterial);

						// If light is casting shadows generate shadow maps of the light
						if (((ZELight*)Component)->GetCastsShadows())
							((ZELight*)Component)->RenderShadowMap(this, ShadowRenderer);

						// Add light to visible lights list.
						VisibleLights.Add((ZELight*)Component);
					}
				}
			}


	// Step 2 : Draw entities and their components

	ZESmartArray<ZELight*> EntityLights; // List of lights that affect particular entity
	ZESmartArray<const ZERLLight*> Lights; 

	// Loop throught scene entities;
	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];
		
		ZEDWORD EntityDrawFlags = CurrentEntity->GetDrawFlags();

		// Check wheather entity is drawable and visible
		if ((EntityDrawFlags & ZE_DF_DRAW) && CurrentEntity->GetVisible())
		{
			// If entity is cullable, test it with view volume. If entity is not in view volume than discard it
			if (EntityDrawFlags & ZE_DF_CULL && ViewVolume.CullTest(CurrentEntity))
					continue;

			// Step 2.5 : Find lights that have effect on entity.
			EntityLights.Clear();
			Lights.Clear();
			
			// Loop through lights that has effect on view area. Lights that tested and passed on Step 1
			for (size_t M = 0; M < VisibleLights.GetCount(); M++)
			{
				const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

				// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
				if (VisibleLights[M]->GetLightType() != ZE_LT_DIRECTIONAL || LightViewVolume.CullTest(CurrentEntity))
				{
					// Add light to list which contains lights effect entity
					Lights.Add(VisibleLights[M]->GetRenderOrderLight());
					EntityLights.Add(VisibleLights[M]);
				}
			}

			// Draw visual debug elements local and axis aligned entites bounding boxes and bounding sphere if enabled
			if (VisualDebugElements & ZE_VDE_ENTITY_ORIENTED_BOUNDINGBOX)
				DrawOrientedBoundingBox(CurrentEntity->GetLocalBoundingBox(), CurrentEntity->GetWorldTransform(), Renderer, EntityOrientedBoundingBoxMaterial);

			if (VisualDebugElements & ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX)
				DrawAxisAlignedBoundingBox(CurrentEntity->GetWorldBoundingBox(), Renderer, EntityAxisAlignedBoundingBoxMaterial);

			if (VisualDebugElements & ZE_VDE_ENTITY_BOUNDINGSPHERE)
				DrawBoundingSphere(CurrentEntity->GetWorldBoundingSphere(), Renderer, EntityBoundingSphereMaterial);

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
					if ((Component->GetDrawFlags() & ZE_DF_DRAW) && Component->GetVisible() && ViewVolume.CullTest(Component))
					{
						Lights.Clear();
						// Loop throught the lights affecting entity in order to find lights affecting component
						for (size_t M = 0; M < EntityLights.GetCount(); M++)
						{
							// Test light view volume and entity's bounding volumes in order to detect entity lies in lights effect area
							const ZEViewVolume& LightViewVolume = VisibleLights[M]->GetViewVolume();

							if (VisibleLights[M]->GetLightType() != ZE_LT_DIRECTIONAL || LightViewVolume.CullTest(Components[N]))
								Lights.Add(VisibleLights[M]->GetRenderOrderLight());
						}

						// Draw bounding volumes of the components if enabled
						if (VisualDebugElements & ZE_VDE_COMPONENT_ORIENTED_BOUNDINGBOX)
							DrawOrientedBoundingBox(Component->GetLocalBoundingBox(), Component->GetWorldTransform(), Renderer, ComponentOrientedBoundingBoxMaterial);

						if (VisualDebugElements & ZE_VDE_COMPONENT_AXISALIGNED_BOUNDINGBOX)
							DrawAxisAlignedBoundingBox(Component->GetWorldBoundingBox(), Renderer, ComponentAxisAlignedBoundingBoxMaterial);

						if (VisualDebugElements & ZE_VDE_COMPONENT_BOUNDINGSPHERE)
							DrawBoundingSphere(Component->GetWorldBoundingSphere(), Renderer, ComponentBoundingSphereMaterial);
						
						// Call components's draw routine to make it draw it self
						Component->Draw(Renderer, Lights);
					}
				}
			}
		}
	}
	Environment.Render(Renderer, ViewVolume, VisibleLights);
}


bool ZEScene::LoadEnvironment(const char* FileName)
{
	if (!Environment.Destroy())
		return false;

	if (!Environment.Load(FileName))
		return false;

	Environment.Initialize();
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
		if (strcmp(Environment.GetFileName(), "") != 0)
			Serializer.Write(Environment.GetFileName(), sizeof(char), ZE_MAX_FILE_NAME_SIZE);
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
	ZEFileUnserializer Unserializer;
	char EntityTypeName[ZE_MAX_NAME_SIZE];
	if (Unserializer.OpenFile(FileName))
	{
		ZEDWORD EntityCount;
		Unserializer.Read(&EntityCount, sizeof(ZEDWORD), 1);

		Unserializer.Read(&LastEntityId, sizeof(int), 1);
		char MapFile[ZE_MAX_FILE_NAME_SIZE];
		Unserializer.Read(MapFile, sizeof(char), ZE_MAX_FILE_NAME_SIZE);
		Environment.Initialize();
		if (!Environment.Load(MapFile))
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
			Entities[I] = zeCore->GetGame()->CreateEntityInstance(EntityTypeName);
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
	EntityAxisAlignedBoundingBoxMaterial = NULL;
	EntityOrientedBoundingBoxMaterial = NULL;
	EntityBoundingSphereMaterial = NULL;
	EntityBoundingSphereMaterial = NULL;
	ComponentAxisAlignedBoundingBoxMaterial = NULL;
	ComponentOrientedBoundingBoxMaterial = NULL;
	ComponentBoundingSphereMaterial = NULL;
	LightRangeMaterial = NULL;
	LastEntityId = 0;
	ShadowRenderer = NULL;
	Renderer = NULL;
	ActiveCamera = NULL;
	ActiveListener = NULL;

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
