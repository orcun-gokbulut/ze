//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightProjective.cpp
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

#include "ZEError.h"
#include "ZECanvas.h"
#include "ZERenderer.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGame/ZEScene.h"
#include "ZELightProjective.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEMaterialLightProjective.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZECore/ZECore.h"

void ZELightProjective::SetFOV(float Value)
{
	FOV = Value;
}

float ZELightProjective::GetFOV() const
{
	return FOV;
}

void ZELightProjective::SetAspectRatio(float Value)
{
	AspectRatio = Value;
}

float ZELightProjective::GetAspectRatio() const
{
	return AspectRatio;
}

void ZELightProjective::SetTextureSampler(const ZESamplerState& Sampler)
{
	TextureSampler = Sampler;
}

const ZESamplerState& ZELightProjective::GetTextureSampler() const
{
	return TextureSampler;
}

void ZELightProjective::SetProjectionTexture(const ZETexture2D* Texture)
{
	zeDebugCheck(Texture == NULL, "Null Pointer");
	ProjectionTexture = Texture;
}

const ZETexture2D* ZELightProjective::GetProjectionTexture() const
{
	return ProjectionTexture;
}

const ZEViewVolume& ZELightProjective::GetViewVolume(ZESize Index)
{
	if (UpdateViewVolume)
	{
 		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.0f, GetRange());
 		UpdateViewVolume = false;
	}

	return ViewVolume;
}

bool ZELightProjective::Initialize()
{
	if (GetInitialized())
		return true;

	ZECanvas Canvas;
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);
	Geometry = Canvas.CreateStaticVertexBuffer();
	
	Material = ZEMaterialLightProjective::CreateInstance();

	return ZEEntity::Initialize();
}

void ZELightProjective::Deinitialize()
{
	if (!GetInitialized())
		return;

	ProjectionTexture = NULL;

	ZE_DESTROY(Material);
	ZE_DESTROY(Geometry);
}

void ZELightProjective::Tick(float Time)
{
	// Update material and render command
	Material->LightCaster = true;
	Material->LightReciever = false;
	Material->ShadowCaster = ShadowCaster;
	Material->ShadowReciver = false;
	Material->Range = Range;
	Material->Color = Color;
	Material->FOV = FOV;
	Material->AspectRatio = AspectRatio;
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
	RenderCommand.Material = Material;
	RenderCommand.Pipeline = ZE_RP_3D;
	RenderCommand.PrimitiveCount = 6;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexBuffers[0] = Geometry;
}

void ZELightProjective::Draw(ZEDrawParameters* DrawParameters)
{	
	ZEUInt FrameId = zeCore->GetFrameId();

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

ZELightProjective::ZELightProjective()
{
	FOV = ZE_PI_4;
	AspectRatio = 1.0f;
	Type = ZE_LT_PROJECTIVE;

	Material = NULL;
	Geometry = NULL;
	ProjectionTexture = NULL;
	
	TextureSampler.SetMagFilter(ZE_TFM_LINEAR);
	TextureSampler.SetMinFilter(ZE_TFM_LINEAR);
	TextureSampler.SetMipFilter(ZE_TFM_LINEAR);
	TextureSampler.SetAddressU(ZE_TAM_BORDER);
	TextureSampler.SetAddressV(ZE_TAM_BORDER);
	TextureSampler.SetAddressW(ZE_TAM_BORDER);
	TextureSampler.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
}

ZELightProjective::~ZELightProjective()
{
	Deinitialize();
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
