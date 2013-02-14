//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightOmniProjective.cpp
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

#include "ZECanvas.h"
#include "ZERenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZETexture.h"
#include "ZELightOmniProjective.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEMaterialOmniProjectiveLight.h"

void ZELightOmniProjective::SetTextureSampler(const ZESamplerState& Sampler)
{
	TextureSampler = Sampler;
}

const ZESamplerState& ZELightOmniProjective::GetTextureSampler() const
{
	return TextureSampler;
}

const ZETextureCube* ZELightOmniProjective::GetProjectionTexture() const
{
	return ProjectionTexture;
}

void ZELightOmniProjective::SetProjectionTexture(const ZETextureCube* Texture)
{
	zeDebugCheck(Texture == NULL, "Null Pointer");
	ProjectionTexture = Texture;
}

const ZEViewVolume& ZELightOmniProjective::GetViewVolume(ZESize Index)
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
 		UpdateViewVolume = false;
	}

	return ViewVolume;
}

bool ZELightOmniProjective::Initialize()
{
	if (GetInitialized())
		return true;

	ZECanvas Canvas;
	Canvas.AddSphere(1.0f, 24, 24);
	Geometry = Canvas.CreateStaticVertexBuffer();
	Material = ZEMaterialOmniProjectiveLight::CreateInstance();

	return ZEEntity::Initialize();
}

void ZELightOmniProjective::Deinitialize()
{
	if (!GetInitialized())
		return;

	ProjectionTexture = NULL;

	ZE_DESTROY(Material);
	ZE_DESTROY(Geometry);
}

void ZELightOmniProjective::Tick(float Time)
{
	// Update material and render command
	Material->LightCaster = true;
	Material->LightReciever = false;
	Material->ShadowCaster = ShadowCaster;
	Material->ShadowReciver = false;
	Material->Range = Range;
	Material->Color = Color;
	Material->Intensity = Intensity;
	Material->Attenuation = Attenuation;
	Material->WorldPosition = GetWorldPosition();
	Material->WorldRotation = GetWorldRotation();
	Material->SamplerState = TextureSampler;
	Material->ProjectionTexture = ProjectionTexture;
	
	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.Flags = 0;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = (ZEMaterial*)Material;
	RenderCommand.Pipeline = ZE_RP_3D;
	RenderCommand.PrimitiveCount = 6;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexBuffers[0] = Geometry;
}

void ZELightOmniProjective::Draw(ZEDrawParameters* DrawParameters)
{
	if (ProjectionTexture == NULL)
	{
		zeWarning("NULL projection texture");
		return;
	}

	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Radius = GetRange();
	LightBoundingSphere.Position = GetWorldPosition();
	
	if (!DrawParameters->ViewVolume->CullTest(LightBoundingSphere))
		DrawParameters->Renderer->AddRenderCommand(&RenderCommand);
}

ZELightOmniProjective::ZELightOmniProjective()
{
	Type = ZE_LT_OMNIPROJECTIVE;

	ProjectionTexture = NULL;
	Material = NULL;
	Geometry = NULL;

	TextureSampler.SetMagFilter(ZE_TFM_LINEAR);
	TextureSampler.SetMinFilter(ZE_TFM_LINEAR);
	TextureSampler.SetMipFilter(ZE_TFM_LINEAR);
	TextureSampler.SetAddressU(ZE_TAM_BORDER);
	TextureSampler.SetAddressV(ZE_TAM_BORDER);
	TextureSampler.SetAddressW(ZE_TAM_BORDER);
	TextureSampler.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
}

ZELightOmniProjective::~ZELightOmniProjective()
{
	Deinitialize();
}

ZELightOmniProjective* ZELightOmniProjective::CreateInstance()
{
	return new ZELightOmniProjective();
}
