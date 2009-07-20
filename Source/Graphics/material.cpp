//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - material.cpp
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

#include "Material.h"
#include "Core/Core.h"
#include "memory.h"
#include "GraphicsModule.h"

const ZEShader* ZEDefaultMaterial::GetShader() const 
{
	return Shader;
}

void ZEDefaultMaterial::SetZero()
{
	ShaderComponents = -1;
	TwoSided = false;
	LightningEnabled = true;
	Wireframe = false;
	TransparancyMode = ZE_TM_NOTRANSPARACY;
	TransparancyCullLimit = 0;
	Opasity = 1.0f;
	memset(Textures, 0, sizeof(Textures));
	memset(PixelShaderConstants, 0, sizeof(PixelShaderConstants));
}

const ZEVector4* ZEDefaultMaterial::GetVertexShaderConstants(int* Count) const 
{
	*Count = 0;
	return NULL;
}

const ZEVector4* ZEDefaultMaterial::GetPixelShaderConstants(int* Count) const 
{
	*Count = 4;
	return (const ZEVector4*)&PixelShaderConstants;
}

const ZETextureBase** ZEDefaultMaterial::GetTextures(int* Count) const 
{
	*Count = 9;
	return (const ZETextureBase**)Textures;
}

void ZEDefaultMaterial::SetShaderComponents(unsigned int ShaderComponents) 
{
	if (this->ShaderComponents != ShaderComponents)
	{
		if (Shader != NULL)
			Shader->Release();

		Shader = zeGraphics->CreateShader(ShaderComponents);

		this->ShaderComponents = ShaderComponents;
	}
}

unsigned int ZEDefaultMaterial::GetShaderComponents() const 
{
	return this->ShaderComponents;
}


void ZEDefaultMaterial::Release() const
{
	if (Shader != NULL)
	{
		Shader->Release();
		((ZEDefaultMaterial*)this)->Shader = NULL;
	}
}

ZEDefaultMaterial::ZEDefaultMaterial()
{
	Shader = NULL;
}

ZEDefaultMaterial::~ZEDefaultMaterial()
{
	Release();
}
