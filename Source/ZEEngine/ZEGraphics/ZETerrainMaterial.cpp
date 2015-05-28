//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainMaterial.cpp
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

#include "ZETerrainMaterial.h"
#include <Memory.h>

#ifndef NULL
#define NULL 0
#endif

#include "ZEGraphicsModule.h"
#include "ZEMaterialComponents.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZETexture2D.h"
#include "ZETextureCube.h"

ZETerrainMaterial::ZETerrainMaterial()
{
	TwoSided = false;
	Wireframe = false;

	ElevationLayer = NULL;
	ColorLayer = NULL;

	ElevationOffset = 0.0f;
	ElevationScale = 1.0f;
	BlendThreshold = 0.125f;

	AmbientFactor = 0.0f;
	AmbientColor = ZEVector3::One;

	DiffuseFactor = 1.0f;
	DiffuseColor = ZEVector3::One;
}

ZETerrainMaterial::~ZETerrainMaterial()
{

}

ZEMaterialFlags ZETerrainMaterial::GetMaterialFlags() const
{
	return ZE_MTF_G_BUFFER_PASS | ZE_MTF_PRE_Z_PASS | ZE_MTF_SUPPORTS_SKINNING;
}

void ZETerrainMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZETerrainMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZETerrainMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZETerrainMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZETerrainMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
}

const ZEVector3& ZETerrainMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZETerrainMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
}

float ZETerrainMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZETerrainMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
}

const ZEVector3& ZETerrainMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZETerrainMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
}

float ZETerrainMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZETerrainMaterial::Tick(float ElapsedTime)
{

}

ZETerrainMaterial* ZETerrainMaterial::CreateInstance()
{
	return zeGraphics->CreateTerrainMaterial();
}
