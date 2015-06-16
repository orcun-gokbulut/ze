//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESimpleMaterial.cpp
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

#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZESimpleMaterial.h"

ZESimpleMaterial::ZESimpleMaterial()
{
	Wireframe = false;
	TwoSided = false;
	VertexColorEnabled = true;
	TransparancyMode = ZE_MTM_NONE;
	TransparancyCullLimit = 0x80;
	MaterialColor = ZEVector4::One;
	Texture = NULL;
	TextureAddressModeU = ZE_TAM_MIRROR;
	TextureAddressModeV = ZE_TAM_MIRROR;
}

ZESimpleMaterial::~ZESimpleMaterial()
{
	
}

ZEMaterialFlags ZESimpleMaterial::GetMaterialFlags() const
{
	return ZE_MTF_NONE;
}

void ZESimpleMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZESimpleMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZESimpleMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZESimpleMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZESimpleMaterial::SetVertexColor(bool Enable)
{
	VertexColorEnabled = Enable;
}

bool ZESimpleMaterial::GetVertexColor()
{
	return VertexColorEnabled;
}


void ZESimpleMaterial::SetMaterialColor(const ZEVector4& Color)
{
	MaterialColor = Color;
}

const ZEVector4& ZESimpleMaterial::GetMaterialColor() const
{
	return MaterialColor;
}

void ZESimpleMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;

}

ZEMaterialTransparancyMode ZESimpleMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZESimpleMaterial::SetTransparancyCullLimit(ZEUInt Limit)
{
	Limit = TransparancyCullLimit;
}

ZEUInt ZESimpleMaterial::GetTransparancyCullLimit() const
{
	return TransparancyCullLimit;
}


void ZESimpleMaterial::SetTexture(const ZETexture2D* Texture)
{
	this->Texture = Texture;
}

const ZETexture2D* ZESimpleMaterial::GetTexture() const
{
	return Texture;
}

void ZESimpleMaterial::SetTextureAddressModeU(ZETextureAddressMode Mode)
{
	TextureAddressModeU = Mode;
}

ZETextureAddressMode ZESimpleMaterial::GetTextureAddressModeU() const
{
	return TextureAddressModeU;
}

void ZESimpleMaterial::SetTextureAddressModeV(ZETextureAddressMode Mode)
{
	TextureAddressModeV = Mode;
}

ZETextureAddressMode ZESimpleMaterial::GetTextureAddressModeV() const
{
	return TextureAddressModeV;
}

ZESimpleMaterial* ZESimpleMaterial::CreateInstance()
{
	return zeGraphics->CreateSimpleMaterial();
}
