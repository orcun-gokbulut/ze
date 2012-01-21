//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaDebugModule.cpp
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

#include "ZEMetaDebugModule.h"

#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGame/ZEPlayer.h"
#include "ZEGame/ZECanvasBrush.h"
#include "ZEGame/ZEScene.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"
#include "ZEMeta/ZEAnimation.h"
#include "ZEMeta/ZEProvider.h"
#include "ZEGraphics/ZEMaterial.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZEPointLight.h"

#include "ZEMath/ZEAngle.h"

//#include "ZEGame/ZED.h"

#include <string.h>

bool ZEMetaDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = ZEPlayer::CreateInstance();

		Scene->AddEntity(Player);
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Player->AddToContainer("Components", ZEPointLight::CreateInstance());
		Scene->SetActiveCamera(Player->GetCamera());
	}

	ZEPointLight* Light = ZEPointLight::CreateInstance();
	Light->SetPosition(ZEVector3::Zero);
	Light->SetColor(ZEVector3(0,1,0));
	Scene->AddEntity(Light);

	ZERunTimeProperty NewProperty;
	NewProperty.Name = "CustomTestProperty";
	NewProperty.Type = ZE_VRT_VECTOR3;
	NewProperty.Value = ZEVariant(ZEVector3(10,10,10));

	Light->AddCustomProperty(NewProperty);	

	NewProperty.Name = "CustomTestProperty2";
	NewProperty.Type = ZE_VRT_VECTOR2;
	NewProperty.Value = ZEVariant(ZEVector2(10,10));

	Light->AddCustomProperty(NewProperty);

	ZEVariant Reader;
	Light->GetProperty("CustomTestProperty", Reader);
	Light->SetProperty("CustomTestProperty", ZEVariant(ZEVector3(20,20,20)));
	Light->GetProperty("CustomTestProperty", Reader);
	Light->RemoveCustomProperty("CustomTestProperty");

	ZEVariant Parameters[4];
	Parameters[0] = 1.0f;
	Parameters[1] = "Orcun";
	Parameters[2] = 5;
	Parameters[3] = ZEVector3(4.0f, 6.0f, 7.0f);
	ZEVariant Return;
	Light->CallMethod("DoSomething",Parameters, 4, Return);

	//new ZEDGrid(Scene);
 
	return true;
}

void ZEMetaDebugModule::Process(float ElapsedTime)
{
	TotalTime += ElapsedTime;
	ZEVariant ReturnValue;

	if (IfEnteredFlag == false && (ZEInt)TotalTime == 5)
	{
		Player->CallMethod("SetName", &ZEVariant("TestPlayer"), 1, ReturnValue);
		//Player->CallMethod("SetPosition", &ZEVariant(ZEVector3(0.0f, -5.0f, 0.0f)), 1, ZEVariant());
		Player->CallMethod("UpdateBoundingVolumes", NULL, 0, ZEVariant());
		Player->CallMethod("GetName", NULL, 0, ReturnValue);
		Player->CallMethod("GetFOV", NULL, 0, ReturnValue);
		Player->CallMethod("SetFOV", &ZEVariant(ZE_PI_8), 1, ReturnValue);
		IfEnteredFlag = true;
	}
}

void ZEMetaDebugModule::Deinitialize()
{
	if (Light != NULL)
	{
		Light->Destroy();
		Light = NULL;
	}

	if (Canvas != NULL)
	{
		Canvas->Destroy();
		Canvas = NULL;
	}

	if (Player != NULL)
	{
		Player->Destroy();
		Player = NULL;
	}
}

ZEMetaDebugModule::ZEMetaDebugModule()
{
	Light = NULL;
	Canvas = NULL;
	Player = NULL;
	TotalTime = 0;
	IfEnteredFlag = false;
}

ZEMetaDebugModule::~ZEMetaDebugModule()
{
	Deinitialize();
}


//// Create a light
//if (Light != NULL)
//{
//	ZELightBrush* Light = new ZELightBrush();
//	Light->SetLightType(ZE_LT_PROJECTIVE);
//	Light->SetPosition(ZEVector3(0.0f, 45.0f, 0.0f));
//	Light->SetScale(ZEVector3::One);
//	Light->GetLight()->SetRange(150);
//	Light->SetRotation(ZEQuaternion::Identity);
//	Light->GetLight()->SetColor(ZEVector3::One);
//	Light->GetLight()->SetAttenuation(0.001f, 0.0f, 2.0f);
//	Light->GetLight()->SetIntensity(5.0f);
//	Light->GetLight()->SetCastsShadows(false);
//	Light->SetProjectionFOV(ZE_PI_2);
//	Light->SetProjectionAspectRatio(1.0f);
//	Light->SetProjectionTexture("test/pavyon.bmp");
//	Scene->AddEntity(Light);
//}
//
//// Create a canvas
//if (Canvas == NULL)
//{
//	ZECanvasBrush* CanvasBrush = new ZECanvasBrush();
//	CanvasBrush->SetRotation(ZEQuaternion(ZE_PI_8, ZEVector3(0.0f, 1.0f, 0.0f)));
//	CanvasBrush->SetScale(ZEVector3(0.5f, 0.5f, 0.5f));
//	CanvasBrush->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
//	CanvasBrush->Canvas.LoadFromFile("Test\\test.zeCanvas");
//	CanvasBrush->UpdateCanvas();
//	ZEFixedMaterial* CanvasMaterial = ZEFixedMaterial::CreateInstance();
//
//	CanvasBrush->AddToContainer("Components", ZECamera::CreateInstance());
//	const ZEComponent** Components = (const ZEComponent**)CanvasBrush->GetContainerItems("Components");
//	ZESize Count = CanvasBrush->GetContainerItemCount("Components");
//	CanvasBrush->SetProperty("Position", ZEVector3(0.0f, 0.0f, 0.0f));
//	CanvasBrush->Material = CanvasMaterial;
//	CanvasMaterial->SetZero();
//	CanvasMaterial->SetLightningEnabled(true);
//	CanvasMaterial->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);
//	CanvasMaterial->SetTwoSided(false);
//	CanvasMaterial->SetRecivesShadow(false);
//	CanvasMaterial->SetAmbientEnabled(true);
//	CanvasMaterial->SetAmbientColor(ZEVector3(0.1f, 0.1f, 0.1f));
//	CanvasMaterial->SetDiffuseEnabled(true);
//	CanvasMaterial->SetDiffuseColor(ZEVector3::One);
//	CanvasMaterial->SetDiffuseMap(ZETexture2DResource::LoadResource("Test\\Diffuse.tga")->GetTexture());
//	CanvasMaterial->SetSpecularEnabled(true);
//	CanvasMaterial->SetSpecularColor(ZEVector3::One);
//	CanvasMaterial->SetSpecularShininess(64.0f);
//	CanvasMaterial->UpdateMaterial();
//
//	ZEAnimation Animation;
//	ZEPropertyAnimation* PropertyAnimation = Animation.PropertyAnimations.Add();
//	Animation.FrameCount = 10;
//	Animation.FramePerSecond = 1.0f;
//	strcpy(Animation.Name, "Orcun");
//	PropertyAnimation->ValueType = ZE_VRT_VECTOR3;
//	PropertyAnimation->Interpolate = true; 
//	PropertyAnimation->PropertyId = CanvasMaterial->GetPropertyId("AmbientColor");
//	ZEPropertyAnimationKey Keys[] =	{
//		{1.0f,		ZEVector3(0.1f, 0.1f, 1.1f)},
//		{2.1f,		ZEVector3(2.1f, 1.1f, 2.1f)},
//		{3.2f,		ZEVector3(1.5f, 1.5f, 0.5f)},
//		{4.21f,		ZEVector3(0.1f, 0.1f, 1.1f)},
//		{5.1f,		ZEVector3(1.5f, 0.5f, 0.5f)},
//		{6.15f,		ZEVector3(0.1f, 0.1f, 1.1f)},
//		{7.1f,		ZEVector3(2.1f, 2.1f, 2.1f)},
//		{8.15f,		ZEVector3(0.5f, 1.5f, 0.5f)},
//		{9.2f,		ZEVector3(2.1f, 2.1f, 2.1f)},
//		{10.21f,	ZEVector3(1.1f, 0.1f, 0.1f)}
//	};
//	PropertyAnimation->Keys.MassAdd(Keys, 10);
//
//	PropertyAnimation = Animation.PropertyAnimations.Add();
//	PropertyAnimation->ValueType = ZE_VRT_FLOAT;
//	PropertyAnimation->Interpolate = true; 
//	PropertyAnimation->PropertyId = CanvasMaterial->GetPropertyId("SpecularFactor");
//	ZEPropertyAnimationKey Keys2[] = {
//		{5.0f,		0.0f},
//		{10.0f,		1.0f},
//	};
//	PropertyAnimation->Keys.MassAdd(Keys2, 2);
//
//	ZEFileSerializer Serializer;
//	Serializer.OpenFile("Test.zeAnim");
//	ZEAnimation::WriteToFile((ZESerializer*)&Serializer, &Animation);
//	Serializer.CloseFile();
//
//	ZEAnimation Test;
//	ZEFileUnserializer Unserializer;
//	Unserializer.OpenFile("Test.zeAnim");
//	ZEAnimation::ReadFromFile((ZEUnserializer*)&Unserializer, &Test);
//	Unserializer.CloseFile();
//
//	CanvasMaterial->SetAnimationController(new ZEAnimationController());
//	CanvasMaterial->GetAnimationController()->SetAnimation(&Test);
//	CanvasMaterial->GetAnimationController()->SetAnimationSpeed(1.0f);
//	CanvasMaterial->GetAnimationController()->SetLooping(true);
//	CanvasMaterial->GetAnimationController()->PlayAnimation();
//	Scene->AddEntity(CanvasBrush);
//
//	ZEObjectProvider Provider;
//	Provider.SetBaseClassType(ZEEntity::Description());
//	Provider.RegisterClass(ZEPlayer::Description());
//	Provider.RegisterClass(ZECanvasBrush::Description());
//	Provider.RegisterClass(ZECamera::Description());
//	//Provider.RegisterClass(ZEMaterial::Description());
//
//	Provider.UnregisterClass(ZEPlayer::Description());
//
//	ZEObject* Class = Provider.CreateInstance("ZECamera");
//}
//

