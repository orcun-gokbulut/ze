//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESeaMaterial.cpp
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

#include "ZESeaMaterial.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

ZEMaterialFlags ZESeaMaterial::GetMaterialFlags() const
{
	return ZE_MTF_G_BUFFER_PASS;
}

void ZESeaMaterial::SetGlobalAmbientEnabled(bool Value)
{
	GlobalAmbientEnabled = Value;
}

bool ZESeaMaterial::GetGlobalAmbientEnabled() const
{
	return GlobalAmbientEnabled;
}

void ZESeaMaterial::SetAmbientColor(const ZEVector4& Color)
{
	AmbientColor = Color;
}

const ZEVector4& ZESeaMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZESeaMaterial::SetAmbientFactor(float Value)
{
	AmbientFactor = Value;
}

float ZESeaMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZESeaMaterial::SetDiffuseColor(const ZEVector4& Color)
{
	DiffuseColor = Color;
}

const ZEVector4& ZESeaMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZESeaMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
}

float ZESeaMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZESeaMaterial::SetSpecularColor(const ZEVector4& Color)
{
	SpecularColor = Color;
}

const ZEVector4& ZESeaMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZESeaMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
}

float ZESeaMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZESeaMaterial::SetSpecularShineness(float Factor)
{
	SpecularShineness = Factor;
}

float ZESeaMaterial::GetSpecularShineness() const
{
	return SpecularShineness;
}

void ZESeaMaterial::SetDiffuseTexture(const ZEGRTexture2D* Texture)
{
	DiffuseTexture = Texture;
}

const ZEGRTexture2D* ZESeaMaterial::GetDiffuseTexture() const
{
	return DiffuseTexture;
}

void ZESeaMaterial::SetNormalTexture(const ZEGRTexture2D* Texture)
{
	NormalTexture = Texture;
}

const ZEGRTexture2D* ZESeaMaterial::GetNormalTexture() const
{
	return NormalTexture;
}

ZESeaMaterial* ZESeaMaterial::CreateInstance()
{
	return zeGraphics->CreateSeaMaterial();
}

ZESeaMaterial::ZESeaMaterial()
{
	NormalTexture = NULL;
	DiffuseTexture = NULL;

	GlobalAmbientEnabled = false;
	AmbientColor = ZEVector4::One;
	AmbientFactor = 0.0f;
	DiffuseColor = ZEVector4::One;
	DiffuseFactor = 1.0f;
	SpecularColor = ZEVector4::One;
	SpecularFactor = 1.0f;
	SpecularShineness = 0.5f;

	NormalTextureOffset = ZEVector2::Zero;
	DiffuseTextureOffset = ZEVector2::Zero;
}

ZESeaMaterial::~ZESeaMaterial()
{

}
