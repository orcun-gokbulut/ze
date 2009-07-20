//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Shader.cpp
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

#include "D3D9Shader.h"
#include "D3D9Common.h"
#include "Core/Error.h"
#include "Core/ResourceFile.h"

#include <stdio.h>

bool ZED3D9ShaderPass::Compile(const char* VertexShaderSource, const char* PixelShaderSource, ZEArray<D3DXMACRO>* Macros)
{
	if (!ZED3D9ComponentBase::CompileVertexShader(VertexShaderSource, &VertexShader, Macros))
		return false;

	if (!ZED3D9ComponentBase::CompilePixelShader(PixelShaderSource, &PixelShader, Macros))
		return false;

	(*Macros)[0].Name = "ZESHADER_SKINTRANSFORM";
	if (!ZED3D9ComponentBase::CompileVertexShader(VertexShaderSource, &SkinnedVertexShader, Macros))
		return false;
	(*Macros)[0].Name = "";

	return true;
}

void ZED3D9ShaderPass::Release()
{
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(SkinnedVertexShader);
}

ZED3D9ShaderPass::ZED3D9ShaderPass()
{
	PixelShader = NULL;
	VertexShader = NULL;
	SkinnedVertexShader = NULL;
	Shared = false;
}

ZED3D9ShaderPass::~ZED3D9ShaderPass()
{
	if (!Shared)
		Release();
}

struct
{
	ZED3D9Shader* Shader;
	int	ReferanceCount;
} ShaderDB[2048];

char ComponentNames[32][30];
ZED3D9ShaderPass ShadowPass;
ZED3D9Shader* SkyShader;

#define ZE_MAX_SHADER_SOURCE_SIZE 32768

char PreLightPassVS[ZE_MAX_SHADER_SOURCE_SIZE];
char PreLightPassPS[ZE_MAX_SHADER_SOURCE_SIZE];
char PointLightVS[ZE_MAX_SHADER_SOURCE_SIZE];
char PointLightPS[ZE_MAX_SHADER_SOURCE_SIZE];
char DirectionalLightVS[ZE_MAX_SHADER_SOURCE_SIZE];
char DirectionalLightPS[ZE_MAX_SHADER_SOURCE_SIZE];
char ProjectiveLightVS[ZE_MAX_SHADER_SOURCE_SIZE];
char ProjectiveLightPS[ZE_MAX_SHADER_SOURCE_SIZE];
char OmniProjectiveLightVS[ZE_MAX_SHADER_SOURCE_SIZE];
char OmniProjectiveLightPS[ZE_MAX_SHADER_SOURCE_SIZE];
char ShadowPassVS[ZE_MAX_SHADER_SOURCE_SIZE];
char ShadowPassPS[ZE_MAX_SHADER_SOURCE_SIZE];
char SkyBoxVS[ZE_MAX_SHADER_SOURCE_SIZE];
char SkyBoxPS[ZE_MAX_SHADER_SOURCE_SIZE];

bool ZED3D9Shader::Initialize()
{
	for (int I = 0; I < 32; I++)
		sprintf(ComponentNames[I], "ZESHADER_COMPONENT%d", I);

	for (size_t I = 0; I < 2048; I++)
	{
		ShaderDB[I].Shader = NULL;
		ShaderDB[I].ReferanceCount = 0;
	}

	ZEResourceFile::ReadTextFile("Shaders\\PreLightning.vs", PreLightPassVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\PreLightning.ps", PreLightPassPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\PointLight.vs", PointLightVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\PointLight.ps", PointLightPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\DirectionalLight.vs", DirectionalLightVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\DirectionalLight.ps", DirectionalLightPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ProjectiveLight.vs", ProjectiveLightVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ProjectiveLight.ps", ProjectiveLightPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\OmniProjectiveLight.vs", OmniProjectiveLightVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\OmniProjectiveLight.ps", OmniProjectiveLightPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.vs", ShadowPassVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.ps", ShadowPassPS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\SkyBox.vs", SkyBoxVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\SkyBox.ps", SkyBoxPS, ZE_MAX_SHADER_SOURCE_SIZE);
	
	ZEArray<D3DXMACRO> Macros;
	Macros.Add();
	Macros[0].Name = "";
	Macros[0].Definition = "";
	Macros.Add();
	Macros[1].Name = NULL;
	Macros[1].Definition = NULL;
	::ShadowPass.Compile(ShadowPassVS, ShadowPassPS, &Macros);
	::ShadowPass.Shared = true;	
	
	SkyShader = new ZED3D9Shader();

	SkyShader->PreLightPass.Compile(SkyBoxVS, SkyBoxPS, &Macros);
	return true;
}


void ZED3D9Shader::Destroy()
{
	for (size_t I = 0; I < 2048; I++)
		if (ShaderDB[I].Shader != NULL)
		{
			if (ShaderDB[I].ReferanceCount == 0)
				zeWarning("Shader System", "Shader is still in use. Destroying it. (ShaderComponents : %d)", I);
			ShaderDB[I].ReferanceCount = 0;
			ShaderDB[I].Shader->Release();
			ShaderDB[I].Shader = NULL;
		}
}

ZEShader* ZED3D9Shader::Create(unsigned int ShaderComponents)
{
	if (ShaderComponents == ZESHADER_SKY)
	{
		return SkyShader;
	}

	//ShaderComponents = ShaderComponents & !ZESHADER_NORMALMAP;
	if (ShaderDB[ShaderComponents].Shader != NULL)
	{
		ShaderDB[ShaderComponents].ReferanceCount++;
		return ShaderDB[ShaderComponents].Shader;
	}

	ShaderDB[ShaderComponents].ReferanceCount = 1;
	ShaderDB[ShaderComponents].Shader = new ZED3D9Shader();

	LPD3DXBUFFER OutputLog;
	ZEArray<D3DXMACRO> ComponentMacros;
	
	D3DXMACRO* CurrentMacro = ComponentMacros.Add();
	CurrentMacro->Definition = "";
	CurrentMacro->Name = "";

	for (int I = 0; I < ZESHADER_COMPONENTCOUNT; I++)
	{
		if ((ShaderComponents & (0x00000001 << I)) != 0)
		{
			CurrentMacro = ComponentMacros.Add();
			CurrentMacro->Name = ComponentNames[I];
			CurrentMacro->Definition = "";
		}
	}

	CurrentMacro = ComponentMacros.Add();
	CurrentMacro->Name = NULL;
	CurrentMacro->Definition = NULL;

	ShaderDB[ShaderComponents].Shader->PreLightPass.Compile(PreLightPassVS, PreLightPassPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->PointLightPass.Compile(PointLightVS, PointLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->DirectionalLightPass.Compile(DirectionalLightVS, DirectionalLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->ProjectiveLightPass.Compile(ProjectiveLightVS, ProjectiveLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->OmniProjectiveLightPass.Compile(OmniProjectiveLightVS, OmniProjectiveLightPS, &ComponentMacros);

	CurrentMacro = &ComponentMacros[ComponentMacros.GetCount() - 1];
	CurrentMacro->Name = "ZESHADER_SHADOWMAP";
	CurrentMacro->Definition = "";

	CurrentMacro = ComponentMacros.Add();
	CurrentMacro->Name = NULL;
	CurrentMacro->Definition = NULL;

	ShaderDB[ShaderComponents].Shader->ShadowedPointLightPass.Compile(PointLightVS, PointLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->ShadowedDirectionalLightPass.Compile(DirectionalLightVS, DirectionalLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->ShadowedProjectiveLightPass.Compile(ProjectiveLightVS, ProjectiveLightPS, &ComponentMacros);
	ShaderDB[ShaderComponents].Shader->ShadowedOmniProjectiveLightPass.Compile(OmniProjectiveLightVS, OmniProjectiveLightPS, &ComponentMacros);

	ShaderDB[ShaderComponents].Shader->ShaderComponents = ShaderComponents;
	ShaderDB[ShaderComponents].Shader->ShadowPass = ::ShadowPass;
	return ShaderDB[ShaderComponents].Shader;
}

void ZED3D9Shader::Release()
{
	ShaderDB[ShaderComponents].ReferanceCount--;

	if (ShaderDB[ShaderComponents].ReferanceCount < 1)
	{
		ShaderDB[ShaderComponents].ReferanceCount = 0;
		ShaderDB[ShaderComponents].Shader = NULL;
		delete this;
	}
}

ZED3D9Shader::ZED3D9Shader()
{
}

ZED3D9Shader::~ZED3D9Shader()
{

}

