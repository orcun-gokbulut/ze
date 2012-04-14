//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyDome.cpp
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

#include "ZESkyDome.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGraphics/ZESkyDomeMaterial.h"
#include "ZEDrawParameters.h"

#define		ZE_SKY_DOME_PATH		"resources\\ClippedUnitDome.zeCanvas"


void ZESkyDome::SetAmbientFactor(float Value)
{
	AmbientFactor = Value;
}

float ZESkyDome::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZESkyDome::SetMiddayAmbientColor(ZEVector3& Color)
{
	MiddayAmbientColor = Color;
}

ZEVector3 ZESkyDome::GetMiddayAmbientColor() const
{
	return MiddayAmbientColor;
}

void ZESkyDome::SetSunsetAmbientColor(ZEVector3& Color)
{
	SunsetAmbientColor = Color;
}

ZEVector3 ZESkyDome::GetSunsetAmbientColor() const
{
	return SunsetAmbientColor;
}

void ZESkyDome::SetSetMieConstant(float Value)
{
	MieConstant = Value;
}

float ZESkyDome::GetGetMieConstant() const
{
	return MieConstant;
}

void ZESkyDome::SetRayleighConstant(float Value)
{
	RayleighConstant = Value;
}

float ZESkyDome::GetRayleighConstant() const
{
	return RayleighConstant;
}

void ZESkyDome::SetMieScaleDepth(float Value)
{
	MieScaleDepth = Value;
}

float ZESkyDome::GetMieScaleDepth() const
{
	return MieScaleDepth;
}

void ZESkyDome::SetRayleighScaleDepth(float Value)
{
	RayleighScaleDepth = Value;
}

float ZESkyDome::GetRayleighScaleDepth() const
{
	return RayleighScaleDepth;
}

void ZESkyDome::SetSunIntensity(float Value)
{
	SunIntensity = Value;
}

float ZESkyDome::GetSunIntensity() const
{
	return SkyDomeMaterial->SunLightIntensity;
}

void ZESkyDome::SetSunLightDirection(const ZEVector3& Value)
{
	ZEVector3::Normalize(SunLightDirection, Value);
}

ZEVector3 ZESkyDome::GetSunLightDirection() const
{
	return SunLightDirection;
}

void ZESkyDome::SetSunLightWaveLenght(ZEVector3& Value)
{
	SunLightWaveLenght = Value;
}

ZEVector3 ZESkyDome::GetSunLightWaveLenght() const
{
	return SunLightWaveLenght;
}

void ZESkyDome::SetOuterRadius(float Value)
{
	OuterRadius = Value;
}

float ZESkyDome::GetOuterRadius() const
{
	return OuterRadius;
}

void ZESkyDome::SetInnerRadius(float Value)
{
	InnerRadius = Value;
}

float ZESkyDome::GetInnerRadius() const
{
	return InnerRadius;
}

void ZESkyDome::SetCameraPosition(ZEVector3 Value)
{
	CameraPosition = Value;
}

ZEVector3 ZESkyDome::GetCameraPosition() const
{
	return CameraPosition;
}

void ZESkyDome::SetCameraPositionOffset(ZEVector3 Value)
{
	CameraPositionOffset = Value;
}

ZEVector3 ZESkyDome::GetCameraPositionOffset() const
{

	return CameraPositionOffset;
}

void ZESkyDome::SetG(float Value)
{
	G = Value;
}

float ZESkyDome::GetG() const
{
	return G;
}

ZEDrawFlags ZESkyDome::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZESkyDome::Initialize()
{
	// Create Material
	if (SkyDomeMaterial == NULL)
	{
		SkyDomeMaterial = ZESkyDomeMaterial::CreateInstance();
		SkyDomeMaterial->UpdateMaterial();

		// Set initial parameters
		SkyDomeMaterial->G						= G;
		SkyDomeMaterial->SunsetAmbientColor		= SunsetAmbientColor;
		SkyDomeMaterial->MiddayAmbientColor		= MiddayAmbientColor;
		SkyDomeMaterial->AmbientFactor			= AmbientFactor;
		SkyDomeMaterial->MieConstant			= MieConstant;
		SkyDomeMaterial->RayleighConstant		= RayleighConstant;
		SkyDomeMaterial->MieScaleDepth			= MieScaleDepth;
		SkyDomeMaterial->RayleighScaleDepth		= RayleighScaleDepth;
		SkyDomeMaterial->OuterRadius			= OuterRadius;
		SkyDomeMaterial->InnerRadius			= InnerRadius;
		SkyDomeMaterial->SunLightIntensity		= SunIntensity;
		SkyDomeMaterial->SunLightDirection		= SunLightDirection;
		SkyDomeMaterial->SunLightWaveLenght		= SunLightWaveLenght;
		SkyDomeMaterial->CameraPosition			= CameraPosition;
		SkyDomeMaterial->CameraPositionOffset	= CameraPositionOffset;
	}

	// Load sky dome
	if (SkyDomeGeometry.IsEmpty())
	{
		if (!SkyDomeGeometry.LoadFromFile(ZE_SKY_DOME_PATH))
		{
			zeAssert("Cannot Load Sky Dome Geometry From: \"%s\"", ZE_SKY_DOME_PATH);
			return false;
		}
	}

	SkyDomeRenderCommand.Priority = 1;
	SkyDomeRenderCommand.Order = 1.1f;
	SkyDomeRenderCommand.Pipeline = ZE_RORP_3D;
	SkyDomeRenderCommand.VertexBuffer = &SkyDomeGeometry;
	SkyDomeRenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	SkyDomeRenderCommand.Flags = ZE_ROF_NONE;
	SkyDomeRenderCommand.PrimitiveCount = SkyDomeGeometry.Vertices.GetCount() / 3;
	SkyDomeRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();

	return ZEEntity::Initialize();
}

void ZESkyDome::Deinitialize()
{	
	if (SkyDomeMaterial != NULL)
	{
		SkyDomeMaterial->Destroy();
		SkyDomeMaterial = NULL;
	}

	if (!SkyDomeGeometry.IsEmpty())
		SkyDomeGeometry.Clean();

	SkyDomeRenderCommand.SetZero();	

	ZEEntity::Deinitialize();
}

void ZESkyDome::Draw(ZEDrawParameters* DrawParameters)
{
	SkyDomeMaterial->G						= G;
	SkyDomeMaterial->SunsetAmbientColor		= SunsetAmbientColor;
	SkyDomeMaterial->MiddayAmbientColor		= MiddayAmbientColor;
	SkyDomeMaterial->AmbientFactor			= AmbientFactor;
	SkyDomeMaterial->MieConstant			= MieConstant;
	SkyDomeMaterial->RayleighConstant		= RayleighConstant;
	SkyDomeMaterial->MieScaleDepth			= MieScaleDepth;
	SkyDomeMaterial->RayleighScaleDepth		= RayleighScaleDepth;
	SkyDomeMaterial->OuterRadius			= OuterRadius;
	SkyDomeMaterial->InnerRadius			= InnerRadius;
	SkyDomeMaterial->SunLightIntensity		= SunIntensity;
	SkyDomeMaterial->SunLightDirection		= SunLightDirection;
	SkyDomeMaterial->SunLightWaveLenght		= SunLightWaveLenght;
	SkyDomeMaterial->CameraPosition			= CameraPosition;
	SkyDomeMaterial->CameraPositionOffset	= CameraPositionOffset;

	SkyDomeRenderCommand.Material			= SkyDomeMaterial;
	SkyDomeRenderCommand.WorldMatrix		= GetWorldTransform();
	DrawParameters->Renderer->AddToRenderList(&SkyDomeRenderCommand);
}

void ZESkyDome::Tick(float Time)
{

}

ZESkyDome* ZESkyDome::CreateInstance()
{
	return new ZESkyDome();
}

ZESkyDome::ZESkyDome()
{
	MieConstant				= 0.0010f;
	MieScaleDepth			= 0.1000f;

	RayleighConstant		= 0.0025f;
	RayleighScaleDepth		= 0.2500f;

	SunIntensity			= 20.0f;
	G						= -0.99f;

	OuterRadius				= 61500.0f;
	InnerRadius				= 60000.0f;

	AmbientFactor			= 1.0000f;
	MiddayAmbientColor		= ZEVector3(0.0f, 0.0f, 0.0f);
	SunsetAmbientColor		= ZEVector3(0.0f, 0.0f, 0.0f);

	SunLightDirection		= ZEVector3(0.0f, 0.0f, -1.0f);
	SunLightWaveLenght		= ZEVector3(0.650f, 0.570f, 0.475f);

	CameraPosition			= ZEVector3(0.0f, 0.0f, 0.0f);
	CameraPositionOffset	= ZEVector3(0.0f, 60001.0f, 0.0f);

	SkyDomeMaterial			= NULL;
	
	SkyDomeGeometry.Clean();
	SkyDomeRenderCommand.SetZero();	
}

ZESkyDome::~ZESkyDome()
{
	Deinitialize();
}
