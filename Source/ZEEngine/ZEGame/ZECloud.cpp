//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECloud.cpp
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

#include "ZECloud.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGraphics/ZECloudMaterial.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZETexture2DResource.h"


void ZECloud::SetAmbientColor(ZEVector3 Color)
{
	AmbientColor = Color;
}

ZEVector3 ZECloud::GetAmbientColor()
{
	return AmbientColor;
}

void ZECloud::SetSunLightColor(ZEVector3 Color)
{
	SunLightColor = Color;
}

ZEVector3 ZECloud::GetSunLightColor()
{
	return SunLightColor;
}

void ZECloud::SetRayleigh(ZEVector3  Vector)
{
	Rayleigh = Vector;
}

ZEVector3 ZECloud::GetRayleigh()
{
	return Rayleigh;
}

void ZECloud::SetMie(ZEVector3 Vector)
{
	Mie = Vector;
}

ZEVector3 ZECloud::GetMie()
{
	return Mie;
}

void ZECloud::SetG(float Value)
{
	G = Value;
}

float ZECloud::GetG()
{
	return G;
}

void ZECloud::SetLightScale(float Value)
{
	LightScale = Value;
}

float ZECloud::GetLightScale()
{
	return LightScale;
}

void ZECloud::SetAmbientScale(float Value)
{
	AmbientScale = Value;
}

float ZECloud::GetAmbientScale()
{
	return AmbientScale;
}

void ZECloud::SetEarthRadius(float Value)
{
	EarthRadius = Value;
}

float ZECloud::GetEarthRadius()
{
	return EarthRadius;
}

void ZECloud::SetAtmosphereHeight(float Value)
{
	AtmosphereHeight = Value;
}

float ZECloud::GetAtmosphereHeight()
{
	return AtmosphereHeight;
}

void ZECloud::SetCloudCover(float Value)
{
	CloudCover = Value;
}

float ZECloud::GetCloudCover()
{
	return CloudCover;
}

void ZECloud::SetCloudPlaneHeight(float Value)
{
	CloudPlaneHeight = Value;
}

float ZECloud::GetCloudPlaneHeight()
{
	return CloudPlaneHeight;
}

void ZECloud::SetWindVelocity(ZEVector2 Value)
{
	WindVelocity = Value;
}

ZEVector2 ZECloud::GetWindVelocity()
{
	return WindVelocity;
}

void ZECloud::SetSunLightDirection(ZEVector3 Value)
{
	ZEVector3::Normalize(SunLightDirection, Value);
}

ZEVector3 ZECloud::GetSunLightDirection()
{
	return SunLightDirection;
}

void ZECloud::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZECloud::GetCamera()
{
	return Camera;
}

void ZECloud::SetCloudFormationTexture(const ZEString& FileName)
{
	// Load the texture
	if (CloudFormationTexture != NULL)
	{
		CloudFormationTexture->Release();
		CloudFormationTexture = NULL;
	}

	ZETextureOptions TextureOption = {
		ZE_TCT_NONE,
		ZE_TCQ_LOW,
		ZE_TDS_NONE,
		ZE_TFC_DISABLED,
		ZE_TMM_DISABLED,
		1
	};

	CloudFormationTexture = ZETexture2DResource::LoadResource(FileName, &TextureOption);
}

const ZEString ZECloud::GetCloudFormationTexture() const
{
	return (CloudFormationTexture == NULL) ? "" : CloudFormationTexture->GetFileName();
}

ZEDrawFlags ZECloud::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZECloud::Initialize()
{
	// Create Material
	if (CloudMaterial == NULL)
	{
		CloudMaterial = ZECloudMaterial::CreateInstance();
		CloudMaterial->Camera = Camera;
		CloudMaterial->UpdateMaterial();
	
		// Send initial Parameters to material
		CloudMaterial->Camera					= Camera;
		CloudMaterial->EarthRadius				= EarthRadius;
		CloudMaterial->AtmosphereHeight			= AtmosphereHeight;
		CloudMaterial->CloudCover				= CloudCover;
		CloudMaterial->WindVelocity				= WindVelocity;
		CloudMaterial->CloudPlaneHeight			= CloudPlaneHeight;
		CloudMaterial->SunLightColor			= SunLightColor;
		CloudMaterial->SunLightDirection		= SunLightDirection;

		if (CloudFormationTexture != NULL)
			CloudMaterial->CloudFormationTexture = CloudFormationTexture->GetTexture();
	}


	CloudRenderCommand.Priority			= 1;
	CloudRenderCommand.Order			= 1.3f;
	CloudRenderCommand.Pipeline			= ZE_RORP_3D;
	CloudRenderCommand.VertexBuffer		= (ZEVertexBuffer*)-1;
	CloudRenderCommand.PrimitiveType	= ZE_ROPT_TRIANGLE;
	CloudRenderCommand.Flags			= ZE_ROF_NONE;
	CloudRenderCommand.PrimitiveCount	= 0;
	CloudRenderCommand.VertexDeclaration = (ZEVertexDeclaration*)-1;
	CloudRenderCommand.IndexBuffer		= (ZEIndexBuffer*)-1;

	return ZEEntity::Initialize();
}

void ZECloud::Deinitialize()
{
	if (CloudMaterial != NULL)
	{
		CloudMaterial->Destroy();
		CloudMaterial = NULL;
	}

	if (CloudFormationTexture != NULL)
	{
		CloudFormationTexture->Release();
		CloudFormationTexture = NULL;
	}

	CloudRenderCommand.SetZero();

	ZEEntity::Deinitialize();
}

void ZECloud::Draw(ZEDrawParameters* DrawParameters)
{
	// Set updated parameters to material
	
	// CloudMaterial->Rayleigh				= Rayleigh;
	// CloudMaterial->Mie					= Mie;
	// CloudMaterial->G						= G;
	// CloudMaterial->LightScale			= LightScale;
	// CloudMaterial->AmbientScale			= AmbientScale;
	// CloudMaterial->AmbientColor			= AmbientColor;
	
	// Update materials parameters before drawing
	CloudMaterial->Camera					= Camera;
	CloudMaterial->EarthRadius				= EarthRadius;
	CloudMaterial->AtmosphereHeight			= AtmosphereHeight;
	CloudMaterial->CloudCover				= CloudCover;
	CloudMaterial->WindVelocity				= WindVelocity;
	CloudMaterial->CloudPlaneHeight			= CloudPlaneHeight;
	CloudMaterial->SunLightColor			= SunLightColor;
	CloudMaterial->SunLightDirection		= SunLightDirection;

	if (CloudFormationTexture != NULL)
		CloudMaterial->CloudFormationTexture = CloudFormationTexture->GetTexture();

	CloudRenderCommand.Order				= 1.3f;
	CloudRenderCommand.Priority				= 1;

	CloudRenderCommand.VertexBufferOffset	= 0;
	CloudRenderCommand.Material				= CloudMaterial;
	CloudRenderCommand.WorldMatrix			= GetWorldTransform();
	DrawParameters->Renderer->AddToRenderList(&CloudRenderCommand);

}

void ZECloud::Tick(float Time)
{
	CloudMaterial->UpdateParameters(Time);
}

ZECloud* ZECloud::CreateInstance()
{
	return new ZECloud();
}

ZECloud::ZECloud()
{
	CloudMaterial			= NULL;
	Camera					= NULL;
	G						= 0.7f;
	LightScale				= 8.0f;
	AmbientScale			= 0.1f;
	EarthRadius				= 21600000.0f;
	AtmosphereHeight		= 30000.0f;
	CloudCover				= 0.5f;
	CloudPlaneHeight		= 600.0f;
	WindVelocity			= ZEVector2(0.005f, 0.005f);
	SunLightDirection		= ZEVector3(0.0f, -1.0f, 0.0f);
	AmbientColor			= ZEVector3(0.3f, 0.35f, 0.4f);
	SunLightColor			= ZEVector3(1.2f, 1.2f,  1.2f);
	Rayleigh				= ZEVector3(0.3f, 0.45f, 6.5f);
	Mie						= ZEVector3(0.3f, 0.3f,  0.3f);
	
	CloudFormationTexture	= NULL;

}

ZECloud::~ZECloud()
{
	this->Deinitialize();
}
