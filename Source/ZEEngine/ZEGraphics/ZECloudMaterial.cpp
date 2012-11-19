//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECloudMaterial.cpp
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

#include "ZECloudMaterial.h"
#include "ZEGraphicsModule.h"
#include "ZECamera.h"
#include "ZEGame\ZEScene.h"
#include "ZEDS\ZEArray.h"
#include "ZETypes.h"
#include "ZEMath\ZEAABBox.h"
#include "ZEGame\ZEEntity.h"
#include "ZETexture2D.h"


ZECloudMaterial::ZECloudMaterial()
{
	Camera					= NULL;
	G						= 0.7f;
	LightScale				= 8.0f;
	AmbientScale			= 0.1f;
	EarthRadius				= 21600000.0f;
	AtmosphereHeight		= 30000.0f;
	CloudCover				= 0.5f;
	CloudPlaneHeight		= 600.0f;
	CloudPlaneHeightFallOff = 0.0f;
	UVOffset				= ZEVector2(0.0f, 0.0f);
	WindVelocity			= ZEVector2(0.005f, 0.005f);
	SunLightDirection		= ZEVector3(0.0f, -1.0f, 0.0f);
	SunLightColor			= ZEVector3(1.2f, 1.2f, 1.2f);
	Rayleigh				= ZEVector3(0.3f, 0.45f, 6.5f);
	Mie						= ZEVector3(0.3f, 0.3f, 0.3f);
	AmbientColor			= ZEVector3(0.3f, 0.35f, 0.4f);

	CloudBoundingBox.Max	= ZEVector3::Zero;
	CloudBoundingBox.Min	= ZEVector3::Zero;

	CloudFormationTexture	= NULL;
}

ZECloudMaterial::~ZECloudMaterial()
{
	Camera					= NULL;
	CloudFormationTexture	= NULL;
}

void ZECloudMaterial::UpdateParameters(float Time)
{
	Camera = zeScene->GetActiveCamera();

	// Update texture UVOffset
	ZEVector2 Vector;
	ZEVector2::Scale(Vector, WindVelocity, Time);
	ZEVector2::Add(UVOffset, UVOffset, Vector);

	// Adjust the height so that clouds are always above.
	// cloud height = m_fDefaultHeight + m_fFallOffHeight * squaredistance_in_horizontal
	float Range = 0.5f * Camera->GetFarZ();
	float Height = Range * 0.12f;
	CloudPlaneHeight = Height + Camera->GetWorldPosition().y;
	CloudPlaneHeightFallOff  = -(0.1f / Range) * (Camera->GetWorldPosition().y / Height + 1.0f);

	// Update Bounding Box
	CloudBoundingBox.Max.y = CloudPlaneHeight;

	GroundBoundingBox.Min = ZEVector3(-12750.0f, 0.0f, -12750.0f);
	GroundBoundingBox.Max = ZEVector3(12750.0f,  25500.0f,  12750.0f);
	
	// Update scattering parameters based on cloud cover
	float LocalCloudCover = (CloudCover - 0.7f) / (1.0f - 0.7f);
	// Original : (CloudCover - 0.7f) / (1.0f - 0.7f);
	LocalCloudCover = ZEMath::Max(0.0f, LocalCloudCover);
	
	// Mie scattering is caused by vapor.
	float LocalMie =  0.05f * (1.0f - LocalCloudCover) + 1.5f * LocalCloudCover;
	// Original : 0.05f * (1.0f - LocalCloudCover) + 1.5f * LocalCloudCover;
	Mie = ZEVector3(LocalMie, LocalMie, LocalMie);
	
	// Rayleigh scattering
	float LocalRayleigh = 0.9f * LocalCloudCover + 1.0f * (1.0f - LocalCloudCover);
	// Original : 0.9f * LocalCloudCover + 1.0f * (1.0f - LocalCloudCover);
	
	ZEVector3 FineRayleigh(0.05f, 0.15f, 1.5f);
	ZEVector3::Scale(Rayleigh, FineRayleigh, LocalRayleigh);
	
	// Ambient color
	static ZEVector3 FineAmbient(0.3f, 0.35f, 0.4f);
	static ZEVector3 CloudyAmbient(0.35f, 0.35f, 0.35f);
	ZEVector3::Lerp(AmbientColor, FineAmbient, CloudyAmbient, LocalCloudCover);

	// When cloudy, ambient term of scattering is risen
	AmbientScale = 0.5f * (1.0f - LocalCloudCover) + 1.0f * LocalCloudCover;
	// Original : 0.5f * (1.0f - LocalCloudCover) + 1.0f * LocalCloudCover;

	// AmbientScale = 0.5f; //AmbientScale > 0.8f ? 0.8f : AmbientScale;

}

ZEMaterialFlags ZECloudMaterial::GetMaterialFlags() const
{
	return ZE_MTF_NONE;
}

ZECloudMaterial* ZECloudMaterial::CreateInstance()
{
	return zeGraphics->CreateCloudMaterial();
}
