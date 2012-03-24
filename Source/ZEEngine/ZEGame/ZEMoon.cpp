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
#include "ZETypes.h"
#include "ZEEntity.h"
#include "ZEMath\ZEVector.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZEMoonMaterial.h"
#include "ZEGraphics\ZETexture3DResource.h"


void ZEMoon::SetMoonPhase(float Phase)
{
	MoonPhase = Phase;
}

float ZEMoon::GetMoonPhase() const
{
	return MoonPhase;
}

void ZEMoon::SetMoonScale(float Scale)
{
	MoonScale = Scale;
}

float ZEMoon::GetMoonScale() const
{
	return MoonScale;
}

void ZEMoon::SetMoonRotation(float Rotation)
{
	MoonRotation = Rotation;
}

float ZEMoon::GetMoonRotation() const
{
	return MoonRotation;
}

void ZEMoon::SetMoonAmbientFactor(float Value)
{
	MoonAmbientFactor = Value;
}

float ZEMoon::GetMoonAmbientFactor() const
{
	return MoonAmbientFactor;
}

void ZEMoon::SetMoonAmbientColor(ZEVector3& Color)
{
	MoonAmbientColor = Color;
}

const ZEVector3& ZEMoon::GetMoonAmbientColor() const
{
	return MoonAmbientColor;
}

void ZEMoon::SetMoonDirection(ZEVector3& Direction)
{
	MoonDirection = Direction;
}

const ZEVector3& ZEMoon::GetGetMoonDirection() const
{
	return MoonDirection;
}

void ZEMoon::SetMoonTexture(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	// Load the texture
	if (MoonTexture != NULL)
	{
		MoonTexture->Release();
		MoonTexture = NULL;
	}

	MoonTexture = ZETexture3DResource::LoadResource(FileName, HorizTileCount, VertTileCount, NULL);

}

const ZEString ZEMoon::GetMoonTexture() const
{
	return (MoonTexture == NULL) ? "" : MoonTexture->GetFileName();
}

ZEDrawFlags ZEMoon::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZEMoon::Initialize()
{
	if (MoonMaterial == NULL)
	{
		MoonMaterial = ZEMoonMaterial::CreateInstance();
		MoonMaterial->UpdateMaterial();

		MoonMaterial->MoonPhase				= MoonPhase;
		MoonMaterial->MoonAmbientColor		= MoonAmbientColor;
		MoonMaterial->MoonAmbientFactor	= MoonAmbientFactor;
		MoonMaterial->MoonScale				= MoonScale;
		MoonMaterial->MoonRotation			= MoonRotation;
		MoonMaterial->MoonDirection			= MoonDirection;
	
		if (MoonTexture != NULL)
			MoonMaterial->MoonTexture = MoonTexture->GetTexture();
	}

	return ZEEntity::Initialize();
}

void ZEMoon::Deinitialize()
{
	if (MoonMaterial != NULL)
	{
		MoonMaterial->Destroy();
		MoonMaterial = NULL;
	}

	if (MoonTexture != NULL)
	{
		MoonTexture->Release();
		MoonTexture = NULL;
	}
}

void ZEMoon::Draw(ZEDrawParameters* DrawParameters)
{
	if (MoonTexture == NULL || MoonMaterial == NULL)
	{
		zeWarning("Uninitialized variable in ZEMoon");
		return;
	}

	MoonMaterial->MoonPhase				= MoonPhase;
	MoonMaterial->MoonAmbientColor		= MoonAmbientColor;
	MoonMaterial->MoonAmbientFactor		= MoonAmbientFactor;
	MoonMaterial->MoonScale				= MoonScale;
	MoonMaterial->MoonRotation			= MoonRotation;
	MoonMaterial->MoonDirection			= MoonDirection;
	
	if (MoonTexture != NULL)
		MoonMaterial->MoonTexture = MoonTexture->GetTexture();

	// ---------------------------------------

	MoonRenderCommand.Order				= 1.2f;
	MoonRenderCommand.Priority			= 1;
	MoonRenderCommand.Pipeline			= ZE_RORP_3D;
	MoonRenderCommand.PrimitiveCount	= 0;
	MoonRenderCommand.PrimitiveType		= ZE_ROPT_TRIANGLE_STRIPT;
	MoonRenderCommand.Flags				= ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	MoonRenderCommand.VertexBufferOffset= 0;
	MoonRenderCommand.IndexBuffer		= (ZEIndexBuffer*)NULL;
	MoonRenderCommand.VertexBuffer		= (ZEVertexBuffer*)NULL;
	MoonRenderCommand.VertexDeclaration	= (ZEVertexDeclaration*)NULL;
	
	// ---------------------------------------

	MoonRenderCommand.Material			= (ZEMaterial*)MoonMaterial;
	MoonRenderCommand.WorldMatrix		= GetWorldTransform();
	DrawParameters->Renderer->AddToRenderList(&MoonRenderCommand);
	
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
	MoonMaterial			= NULL;
	MoonTexture				= NULL;
	
	MoonPhase				= 0.0f;
	MoonScale				= 1.0f;
	MoonRotation			= 0.0f;
	MoonAmbientFactor		= 1.0f;
	MoonAmbientColor		= ZEVector3::One;
	MoonDirection			= ZEVector3::UnitY;

	// Fill render command
	MoonRenderCommand.SetZero();
	MoonRenderCommand.Priority				= 4;
	MoonRenderCommand.Order					= 4.0f;
	MoonRenderCommand.PrimitiveCount		= 0;
	MoonRenderCommand.VertexBuffer			= NULL;
	MoonRenderCommand.IndexBuffer			= NULL;
	MoonRenderCommand.VertexDeclaration		= NULL;
	MoonRenderCommand.PrimitiveType			= ZE_ROPT_TRIANGLE_STRIPT;
	MoonRenderCommand.Flags					= ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	
}

ZEMoon::~ZEMoon()
{
	this->Deinitialize();
}
