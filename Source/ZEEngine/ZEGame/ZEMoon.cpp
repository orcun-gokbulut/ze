//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMoon.cpp
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


#include "ZEMoon.h"
#include "ZEMath\ZEVector.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZEMoonMaterial.h"
#include "ZETexture\ZETexture3DResource.h"

void ZEMoon::SetPhase(float Phase)
{
	Phase = Phase;
}

float ZEMoon::GetPhase() const
{
	return Phase;
}

void ZEMoon::SetMoonScale(float Scale)
{
	Scale = Scale;
}

float ZEMoon::GetMoonScale() const
{
	return Scale;
}

void ZEMoon::SetMoonRotation(float Rotation)
{
	Rotation = Rotation;
}

float ZEMoon::GetMoonRotation() const
{
	return Rotation;
}

void ZEMoon::SetAmbientFactor(float Value)
{
	AmbientFactor = Value;
}

float ZEMoon::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEMoon::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
}

const ZEVector3& ZEMoon::GetAmbientColor() const
{
	return AmbientColor;
}

void ZEMoon::SetDirection(const ZEVector3& NewDirection)
{
	Direction = NewDirection;
}

const ZEVector3& ZEMoon::GetGetDirection() const
{
	return Direction;
}

void ZEMoon::SetTexture(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	// Load the texture
	if (Texture != NULL)
	{
		Texture->Release();
		Texture = NULL;
	}

	Texture = ZETexture3DResource::LoadResource(FileName, HorizTileCount, VertTileCount, NULL);

}

const ZEString ZEMoon::GetTexture() const
{
	return (Texture == NULL) ? "" : Texture->GetFileName();
}

ZEDrawFlags ZEMoon::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZEMoon::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (Material == NULL)
	{
		Material = ZEMoonMaterial::CreateInstance();
		Material->UpdateMaterial();

		Material->MoonPhase				= Phase;
		Material->MoonAmbientColor		= AmbientColor;
		Material->MoonAmbientFactor		= AmbientFactor;
		Material->MoonScale				= Scale;
		Material->MoonRotation			= Rotation;
		Material->MoonDirection			= Direction;
	
		if (Texture != NULL)
			Material->MoonTexture = Texture->GetTexture();
	}

	return true;
}

bool ZEMoon::DeinitializeSelf()
{
	if (Material != NULL)
	{
		Material->Destroy();
		Material = NULL;
	}

	if (Texture != NULL)
	{
		Texture->Release();
		Texture = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

void ZEMoon::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass == ZE_RP_SHADOW_MAP)
		return;

	if (Texture == NULL || Material == NULL)
	{
		zeWarning("Uninitialized variable in ZEMoon");
		return;
	}

	Material->MoonPhase				= Phase;
	Material->MoonAmbientColor		= AmbientColor;
	Material->MoonAmbientFactor		= AmbientFactor;
	Material->MoonScale				= Scale;
	Material->MoonRotation			= Rotation;
	Material->MoonDirection			= Direction;
	
	if (Texture != NULL)
		Material->MoonTexture = Texture->GetTexture();

	// ---------------------------------------

	RenderCommand.Order				= 1.2f;
	RenderCommand.Priority			= 1;
	RenderCommand.Pipeline			= ZE_RORP_3D;
	RenderCommand.PrimitiveCount	= 0;
	RenderCommand.PrimitiveType		= ZE_ROPT_TRIANGLE_STRIPT;
	RenderCommand.Flags				= ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderCommand.VertexBufferOffset= 0;
	RenderCommand.IndexBuffer		= (ZEIndexBuffer*)-1;
	RenderCommand.VertexBuffer		= (ZEVertexBuffer*)-1;
	RenderCommand.VertexDeclaration	= (ZEVertexDeclaration*)-1;
	
	// ---------------------------------------

	RenderCommand.Material			= (ZEMaterial*)Material;
	RenderCommand.WorldMatrix		= GetWorldTransform();
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
	
}

void ZEMoon::Tick(float Time)
{

}

ZEMoon* ZEMoon::CreateInstance()
{
	return new ZEMoon();
}

ZEMoon::ZEMoon()
{	
	Material			= NULL;
	Texture				= NULL;
	
	Phase				= 0.3f;
	Scale				= 0.05f;
	Rotation			= 0.0f;
	AmbientFactor		= 1.0f;
	AmbientColor		= ZEVector3::One;
	Direction			= ZEVector3(0.0001f, 1.0f, 0.0001f);

	// Fill render command
	RenderCommand.SetZero();
	RenderCommand.Priority				= 4;
	RenderCommand.Order					= 4.0f;
	RenderCommand.PrimitiveCount		= 0;
	RenderCommand.VertexBuffer			= NULL;
	RenderCommand.IndexBuffer			= NULL;
	RenderCommand.VertexDeclaration		= NULL;
	RenderCommand.PrimitiveType			= ZE_ROPT_TRIANGLE_STRIPT;
	RenderCommand.Flags					= ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	
}

ZEMoon::~ZEMoon()
{
	
}
