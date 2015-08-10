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
#include "ZERenderer\ZERNRenderer.h"
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

bool ZEMoon::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	return true;
}

bool ZEMoon::DeinitializeSelf()
{
	return ZEEntity::DeinitializeSelf();
}

ZEMoon* ZEMoon::CreateInstance()
{
	return new ZEMoon();
}

ZEMoon::ZEMoon()
{	
	Texture				= NULL;
	Phase				= 0.3f;
	Scale				= 0.05f;
	Rotation			= 0.0f;
	AmbientFactor		= 1.0f;
	AmbientColor		= ZEVector3::One;
	Direction			= ZEVector3(0.0001f, 1.0f, 0.0001f);
}

ZEMoon::~ZEMoon()
{
	
}
