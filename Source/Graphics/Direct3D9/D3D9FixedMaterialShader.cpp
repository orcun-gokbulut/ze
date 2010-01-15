//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9FixedMaterialShader.cpp
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

#include "D3D9FixedMaterialShader.h"
#include "D3D9CommonTools.h"
#include "Core/Error.h"
#include "Core/ResourceFile.h"
#include "Graphics/Material.h"
#include "Graphics/FixedMaterial.h"
#include "Graphics/MaterialComponents.h"
#include <stdio.h>

bool ZED3D9FixedMateriaShaderPass::Compile(const char* VertexShaderSource, const char* PixelShaderSource, const char* ShaderName, const char* ShaderProfile, D3DXMACRO* Macros)
{
	char PSProfile[10] = "ps_";
	char VSProfile[10] = "vs_";
	strcat_s(PSProfile, 10, ShaderProfile);
	strcat_s(VSProfile, 10, ShaderProfile);

	if (!ZED3D9CommonTools::CompileVertexShader(&VertexShader, VertexShaderSource, ShaderName, VSProfile, Macros))
		return false;

	if (!ZED3D9CommonTools::CompilePixelShader(&PixelShader, PixelShaderSource, ShaderName, PSProfile, Macros))
		return false;

	int I;
	for (I = 0; I < 32; I++)
		if (Macros[I].Name == NULL)
		{
			Macros[I].Name = "ZESHADER_SKINTRANSFORM";
			Macros[I + 1].Name = NULL;
			Macros[I + 1].Definition = NULL;
			break;
		}

	if (!ZED3D9CommonTools::CompileVertexShader(&SkinnedVertexShader, VertexShaderSource, ShaderName, VSProfile, Macros))
		return false;
	Macros[I].Name = NULL;
	Macros[I].Definition = NULL;

	return true;
}

void ZED3D9FixedMateriaShaderPass::Release()
{
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(SkinnedVertexShader);
}

ZED3D9FixedMateriaShaderPass::ZED3D9FixedMateriaShaderPass()
{
	PixelShader = NULL;
	VertexShader = NULL;
	SkinnedVertexShader = NULL;
	Shared = false;
}

ZED3D9FixedMateriaShaderPass::~ZED3D9FixedMateriaShaderPass()
{
	if (!Shared)
		Release();
}

struct ZEMaterialShaderInstance
{
	ZEMaterialType		MaterialType;
	unsigned int		ShaderUID;
	ZED3D9FixedMaterialShader*		Shader;
	int					ReferanceCount;
};

struct ZEMaterialShaderFileCache
{
	ZEMaterialType		MaterialType;
	unsigned int		ShaderUID;
	unsigned int		FilePosition;
};

ZESmartArray<ZEMaterialShaderInstance, 5> ShaderInstances;
ZESmartArray<ZEMaterialShaderFileCache, 5> FileShaderCache;

ZED3D9FixedMateriaShaderPass ShadowPass;
ZED3D9FixedMaterialShader* SkyShader;

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

const char* ZED3D9FixedMaterialShader::GetMaterialComponentName(unsigned ShaderComponent)
{
	switch(ShaderComponent)
	{
		case ZESHADER_AMBIENT:
			return "ZESHADER_AMBIENT";
		case ZESHADER_DIFFUSE:
			return "ZESHADER_DIFFUSE";
		case ZESHADER_DIFFUSEMAP:
			return "ZESHADER_DIFFUSEMAP";
		case ZESHADER_NORMALMAP:
			return "ZESHADER_NORMALMAP";
		case ZESHADER_PARALLAXMAP:
			return "ZESHADER_PARALLAXMAP";
		case ZESHADER_SPECULAR:
			return "ZESHADER_SPECULAR";
		case ZESHADER_SPECULARMAP:
			return "ZESHADER_SPECULARMAP";
		case ZESHADER_EMMISIVE:
			return "ZESHADER_EMMISIVE";
		case ZESHADER_EMMISIVEMAP:
			return "ZESHADER_EMMISIVEMAP";
		case ZESHADER_OPACITY:
			return "ZESHADER_OPACITY";
		case ZESHADER_OPACITY_DIFFUSEALPHA:
			return "ZESHADER_OPACITY_DIFFUSEALPHA";
		case ZESHADER_OPACITY_CONSTANT:
			return "ZESHADER_OPACITY_CONSTANT";
		case ZESHADER_OCAPASITYMAP:
			return "ZESHADER_OCAPASITYMAP";
		case ZESHADER_DETAILMAP:
			return "ZESHADER_DETAILMAP";
		case ZESHADER_DETAILDIFFUSEMAP:
			return "ZESHADER_DETAILDIFFUSEMAP";
		case ZESHADER_DETAILNORMALMAP:
			return "ZESHADER_DETAILNORMALMAP";
		case ZESHADER_REFLECTION:
			return "ZESHADER_REFLECTION";
		case ZESHADER_REFRACTION:
			return "ZESHADER_REFRACTION";
		case ZESHADER_LIGHTMAP:
			return "ZESHADER_LIGHTMAP";
		case ZESHADER_DISTORTIONMAP:
			return "ZESHADER_DISTORTIONMAP";
		default:
			return "";
	}
}

bool ZED3D9FixedMaterialShader::BaseInitialize()
{
	ShaderInstances.Clear();

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
	Macros[0].Name = NULL;
	Macros[0].Definition = NULL;
	Macros.Add();
	Macros[1].Name = NULL;
	Macros[1].Definition = NULL;

	::ShadowPass.Compile(ShadowPassVS, ShadowPassPS, "Shadow Pass Shader", "2_0", Macros.GetCArray());
	::ShadowPass.Shared = true;	
	
	SkyShader = new ZED3D9FixedMaterialShader();

	SkyShader->PreLightPass.Compile(SkyBoxVS, SkyBoxPS, "Fixed Material Shadow Pass", "2_0", Macros.GetCArray());
	return true;
}


void ZED3D9FixedMaterialShader::BaseDeinitialize()
{
	for (size_t I = 0; I <ShaderInstances.GetCount(); I++) 
		if (ShaderInstances[I].Shader != NULL) 
		{ 
			zeWarning("Material Shader System", "Material shader is still in use. Destroying it. (Shader Cache Index : %d, Material Type : %d, Shader UID : %d)", 
				I, 
				ShaderInstances[I].MaterialType,
				ShaderInstances[I].ShaderUID); 

			ShaderInstances[I].Shader->Release(); 
		} 	
	ShaderInstances.Clear();
}
 
ZED3D9FixedMaterialShader* ZED3D9FixedMaterialShader::CreateInstance(unsigned int MaterialComponents)
{
	ZEMaterialShaderInstance* FreeSpace = NULL;

	for (size_t I = 0; I < ShaderInstances.GetCount(); I++)
		if (ShaderInstances[I].Shader == NULL)
			FreeSpace = &ShaderInstances[I];
		else
			if (ShaderInstances[I].MaterialType == ZE_MT_FIXED && ShaderInstances[I].ShaderUID == MaterialComponents)
			{
				ShaderInstances[I].ReferanceCount++;
				return ShaderInstances[I].Shader;
			}
	
	if (FreeSpace == NULL)
		FreeSpace = ShaderInstances.Add();

	FreeSpace->MaterialType = ZE_MT_FIXED;
	FreeSpace->ShaderUID = MaterialComponents;
	FreeSpace->ReferanceCount = 1;
	FreeSpace->Shader = new ZED3D9FixedMaterialShader();
	FreeSpace->Shader->ShaderInstanceIndex = ShaderInstances.GetCount() - 1;

	LPD3DXBUFFER OutputLog;
	D3DXMACRO ComponentMacros[64];
	D3DXMACRO* CurrentMacro;
	unsigned int ComponentMacroCount = 0;
	for (int I = 0; I < 32; I++)
		if ((MaterialComponents & (0x00000001 << I)) != 0)
		{
			CurrentMacro = &ComponentMacros[ComponentMacroCount];
			CurrentMacro->Name = ZED3D9FixedMaterialShader::GetMaterialComponentName(MaterialComponents & (0x00000001 << I));
			CurrentMacro->Definition = "";
			ComponentMacroCount++;
		}

	CurrentMacro = &ComponentMacros[ComponentMacroCount];
	CurrentMacro->Name = NULL;
	CurrentMacro->Definition = NULL;

	FreeSpace->Shader->PreLightPass.Compile(PreLightPassVS, PreLightPassPS, "Fixed Material PreLightning Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->PointLightPass.Compile(PointLightVS, PointLightPS, "Fixed Material Point Light Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->DirectionalLightPass.Compile(DirectionalLightVS, DirectionalLightPS, "Fixed Material Directional Light Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->ProjectiveLightPass.Compile(ProjectiveLightVS, ProjectiveLightPS, "Fixed Material Projective Light Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->OmniProjectiveLightPass.Compile(OmniProjectiveLightVS, OmniProjectiveLightPS, "Fixed Material OmniProjective Pass", "2_0", ComponentMacros);

	CurrentMacro = &ComponentMacros[ComponentMacroCount];
	CurrentMacro->Name = "ZESHADER_SHADOWMAP";
	CurrentMacro->Definition = "";

	CurrentMacro = &ComponentMacros[ComponentMacroCount + 1];
	CurrentMacro->Name = NULL;
	CurrentMacro->Definition = NULL;

	FreeSpace->Shader->ShadowedPointLightPass.Compile(PointLightVS, PointLightPS, "Fixed Material Shadowed Point Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->ShadowedDirectionalLightPass.Compile(DirectionalLightVS, DirectionalLightPS,"Fixed Shadowed Material Directional Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->ShadowedProjectiveLightPass.Compile(ProjectiveLightVS, ProjectiveLightPS, "Fixed Material Shadowed Projective Pass", "2_0", ComponentMacros);
	FreeSpace->Shader->ShadowedOmniProjectiveLightPass.Compile(OmniProjectiveLightVS, OmniProjectiveLightPS, "Fixed Material Shadowed OmniProjective Pass", "2_0", ComponentMacros);

	FreeSpace->Shader->ProjectiveLightShadowPass = ::ShadowPass;
	return FreeSpace->Shader;
}

void ZED3D9FixedMaterialShader::Release()
{
	if (ShaderInstanceIndex == -1)
		return;

	ShaderInstances[ShaderInstanceIndex].ReferanceCount--;

	if (ShaderInstances[ShaderInstanceIndex].ReferanceCount < 1)
	{
		ShaderInstances[ShaderInstanceIndex].MaterialType = ZE_MT_NONE;
		ShaderInstances[ShaderInstanceIndex].ShaderUID = 0;
		ShaderInstances[ShaderInstanceIndex].ReferanceCount = 0;
		ShaderInstances[ShaderInstanceIndex].Shader = NULL;
		ShaderInstanceIndex = -1;
		ShaderComponents = 0;
		delete this;
	}
}

void ZED3D9FixedMaterialShader::Destroy()
{
	Release();
}

ZED3D9FixedMaterialShader::ZED3D9FixedMaterialShader()
{
	ShaderInstanceIndex = -1;
}

ZED3D9FixedMaterialShader::~ZED3D9FixedMaterialShader()
{

}

