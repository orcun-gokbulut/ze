//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyBrush.cpp
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

#include "ZESkyBrush.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZERenderer/ZESkyBoxMaterial.h"
#include "ZERenderer/ZEDirectionalLight.h"
#include "ZETexture/ZETextureCubeResource.h"

#include <string.h>

ZEDrawFlags ZESkyBrush::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZESkyBrush::SetSkyBrightness(float Brightness)
{
	SkyBrightness = Brightness;

	if (IsInitialized())
		SkyMaterial->SetBrightness(Brightness);
}

float ZESkyBrush::GetSkyBrightness() const
{
	return SkyBrightness;
}

void ZESkyBrush::SetSkyColor(const ZEVector3& Color)
{
	SkyColor = Color;

	if (IsInitialized())
		SkyMaterial->SetColor(Color);
}

const ZEVector3& ZESkyBrush::GetSkyColor() const
{
	return SkyColor;
}

void ZESkyBrush::SetSkyTexture(const char* FileName)
{
	if (SkyTexture != NULL)
	{
		SkyTexture->Release();
		SkyTexture = NULL;
	}

	ZETextureOptions Options;
	Options.CompressionQuality = ZE_TCQ_HIGH;
	Options.CompressionType = ZE_TCT_NONE;
	Options.DownSample = ZE_TDS_NONE;
	Options.FileCaching = ZE_TFC_DISABLED;
	Options.MaximumMipmapLevel = 1;
	Options.MipMapping = ZE_TMM_DISABLED;

	SkyTexture = ZETextureCubeResource::LoadResource(FileName, &Options);
	
	if (SkyMaterial != NULL)
		SkyMaterial->SetTexture(SkyTexture == NULL ? NULL : SkyTexture->GetTexture());

}

const char* ZESkyBrush::GetSkyTexture() const
{
	if (SkyTexture == NULL)
		return "";
	
	return SkyTexture->GetFileName();
}

bool ZESkyBrush::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (SkyMaterial == NULL)
	{
		SkyMaterial = ZESkyBoxMaterial::CreateInstance();
		SkyMaterial->UpdateMaterial();
	}

	SkyMaterial->SetColor(SkyColor);
	SkyMaterial->SetBrightness(SkyBrightness);
	
	if (SkyTexture != NULL)
		SkyMaterial->SetTexture(SkyTexture->GetTexture());
	
	return true;
}

bool ZESkyBrush::DeinitializeSelf()
{
	if (SkyMaterial != NULL)
	{
		SkyMaterial->Destroy();
		SkyMaterial = NULL;
	}

	if (SkyTexture != NULL)
	{
		SkyTexture->Release();
		SkyTexture = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

void ZESkyBrush::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass == ZE_RP_SHADOW_MAP)
		return;

	if (SkyTexture != NULL)
	{
		SkyRenderCommand.Material = SkyMaterial;
		SkyRenderCommand.WorldMatrix = GetWorldTransform();
		DrawParameters->Renderer->AddCommand(&SkyRenderCommand);
	}
}

void ZESkyBrush::Tick(float Time)
{

}

ZESkyBrush::ZESkyBrush()
{
	SkyMaterial = NULL;
	SkyTexture = NULL;
	SkyColor = ZEVector3::One;
	SkyBrightness = 1.0f;
	SkyBox.AddBox(2.0f, 2.0, 2.0f);
	
	SkyRenderCommand.SetZero();
	SkyRenderCommand.Priority = 1;
	SkyRenderCommand.Order = 1.0f;
	SkyRenderCommand.VertexBuffer = &SkyBox;
	SkyRenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	SkyRenderCommand.PrimitiveCount = SkyBox.Vertices.GetCount() / 3;
	SkyRenderCommand.Flags = ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	SkyRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
}

ZESkyBrush::~ZESkyBrush()
{

}

ZESkyBrush* ZESkyBrush::CreateInstance()
{
	return new ZESkyBrush();
}
