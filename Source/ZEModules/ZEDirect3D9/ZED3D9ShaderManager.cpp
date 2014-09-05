//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShaderManager.cpp
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

#include "ZED3D9ShaderManager.h"
#include "ZETypes.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9Module.h"
#include "ZEData.h"

ZEUInt32 ZED3D9ShaderManager::CalculateHash(const char* FileName, const char* FunctionName, ZEUInt32 Components)
{
	ZEUInt32 Hash = 0;
	ZESize Index = 0;

	while(FileName[Index] != '\0')
	{
		Hash += FileName[Index];
		Index++;
	}

	Index = 0;
	while(FunctionName[Index] != '\0')
	{
		Hash += FunctionName[Index];
		Index++;
	}

	Hash += Components;
	
	return Hash;
}

void ZED3D9ShaderManager::ReleaseShader(ZED3D9Shader* Shader)
{
	Shader->ReferanceCount--;
	if (Shader->ReferanceCount < 1)
	{
		for (ZESize I = 0; I < Shaders.GetCount(); I++)
			if (Shaders[I] == Shader)
				Shaders[I] = NULL;

		delete Shader;
	}
}

bool ZED3D9ShaderManager::ReadFromFileCache(const char* Filename, const char* FunctionName, ZEUInt32 Components)
{
	return false;
}

void ZED3D9ShaderManager::WriteToFileCache(const char* Filenamne, const char* FunctionName, ZEUInt32 Components)
{

}

#include "AerialPerspectiveProcessor.hlsl.h"
#include "BlurMaskProcessor.hlsl.h"
#include "BlurProcessor.hlsl.h"
#include "ChannelDisorientationProcessor.hlsl.h"
#include "CloudMaterial.hlsl.h"
#include "ColorTransformProcessor.hlsl.h"
#include "DOFProcessor.hlsl.h"
#include "DownSample2x.hlsl.h"
#include "EDProcessor.hlsl.h"
#include "FixedMaterialComponents.hlsl.h"
#include "FixedMaterial.hlsl.h"
#include "FogProcessor.hlsl.h"
#include "GBuffer.hlsl.h"
#include "GrainProcessor.hlsl.h"
#include "HBAOProcessor.hlsl.h"
#include "HDRProcessor.hlsl.h"
#include "LBuffer.hlsl.h"
#include "Lights.hlsl.h"
#include "MLAAProcessor.hlsl.h"
#include "MoonMaterial.hlsl.h"
#include "PixelWorldPositionProcessor.hlsl.h"
#include "SeaMaterial.hlsl.h"
#include "ShadowMaterial.hlsl.h"
#include "Shadow.hlsl.h"
#include "SimpleMaterial.hlsl.h"
#include "SkinTransform.hlsl.h"
#include "SkyBoxMaterial.hlsl.h"
#include "SkyDomeMaterial.hlsl.h"
#include "SSAAProcessor.hlsl.h"
#include "SSAOProcessor.hlsl.h"
#include "TerrainMaterial.hlsl.h"
#include "TextureMaskProcessor.hlsl.h"
#include "TextureResizeProcessor.hlsl.h"
#include "UI.hlsl.h"
#include "UnsharpenFilterProcessor.hlsl.h"
#include "ZoomBlurProcessor.hlsl.h"

const char* ZED3D9ShaderManager::GetInternal(const char* Filename)
{
	if (stricmp(Filename, "AerialPerspectiveProcessor.hlsl") == 0)
		return (const char*)AerialPerspectiveProcessor_hlsl().GetData();
	else if(stricmp(Filename, "BlurMaskProcessor.hlsl") == 0)
		return (const char*)BlurMaskProcessor_hlsl().GetData();
	else if(stricmp(Filename, "BlurProcessor.hlsl") == 0)
		return (const char*)BlurProcessor_hlsl().GetData();
	else if(stricmp(Filename, "ChannelDisorientationProcessor.hlsl") == 0)
		return (const char*)ChannelDisorientationProcessor_hlsl().GetData();
	else if(stricmp(Filename, "CloudMaterial.hlsl") == 0)
		return (const char*)CloudMaterial_hlsl().GetData();
	else if(stricmp(Filename, "ColorTransformProcessor.hlsl") == 0)
		return (const char*)ColorTransformProcessor_hlsl().GetData();
	else if(stricmp(Filename, "DOFProcessor.hlsl") == 0)
		return (const char*)DOFProcessor_hlsl().GetData();
	else if(stricmp(Filename, "DownSample2x.hlsl") == 0)
		return (const char*)DownSample2x_hlsl().GetData();
	else if(stricmp(Filename, "EDProcessor.hlsl") == 0)
		return (const char*)EDProcessor_hlsl().GetData();
	else if(stricmp(Filename, "FixedMaterial.hlsl") == 0)
		return (const char*)FixedMaterial_hlsl().GetData();
	else if(stricmp(Filename, "FixedMaterialComponents.hlsl") == 0)
		return (const char*)FixedMaterialComponents_hlsl().GetData();
	else if(stricmp(Filename, "FogProcessor.hlsl") == 0)
		return (const char*)FogProcessor_hlsl().GetData();
	else if(stricmp(Filename, "GBuffer.hlsl") == 0)
		return (const char*)GBuffer_hlsl().GetData();
	else if(stricmp(Filename, "GrainProcessor.hlsl") == 0)
		return (const char*)GrainProcessor_hlsl().GetData();
	else if(stricmp(Filename, "HBAOProcessor.hlsl") == 0)
		return (const char*)HBAOProcessor_hlsl().GetData();
	else if(stricmp(Filename, "HDRProcessor.hlsl") == 0)
		return (const char*)HDRProcessor_hlsl().GetData();
	else if(stricmp(Filename, "LBuffer.hlsl") == 0)
		return (const char*)LBuffer_hlsl().GetData();
	else if(stricmp(Filename, "Lights.hlsl") == 0)
		return (const char*)Lights_hlsl().GetData();
	else if(stricmp(Filename, "MLAAProcessor.hlsl") == 0)
		return (const char*)MLAAProcessor_hlsl().GetData();
	else if(stricmp(Filename, "MoonMaterial.hlsl") == 0)
		return (const char*)MoonMaterial_hlsl().GetData();
	else if(stricmp(Filename, "PixelWorldPositionProcessor.hlsl") == 0)
		return (const char*)PixelWorldPositionProcessor_hlsl().GetData();
	else if(stricmp(Filename, "SeaMaterial.hlsl") == 0)
		return (const char*)SeaMaterial_hlsl().GetData();
	else if(stricmp(Filename, "Shadow.hlsl") == 0)
		return (const char*)Shadow_hlsl().GetData();
	else if(stricmp(Filename, "ShadowMaterial.hlsl") == 0)
		return (const char*)ShadowMaterial_hlsl().GetData();
	else if(stricmp(Filename, "SimpleMaterial.hlsl") == 0)
		return (const char*)SimpleMaterial_hlsl().GetData();
	else if(stricmp(Filename, "SkinTransform.hlsl") == 0)
		return (const char*)SkinTransform_hlsl().GetData();
	else if(stricmp(Filename, "SkyBoxMaterial.hlsl") == 0)
		return (const char*)SkyBoxMaterial_hlsl().GetData();
	else if(stricmp(Filename, "SkyDomeMaterial.hlsl") == 0)
		return (const char*)SkyDomeMaterial_hlsl().GetData();
	else if(stricmp(Filename, "SSAAProcessor.hlsl") == 0)
		return (const char*)SSAAProcessor_hlsl().GetData();
	else if(stricmp(Filename, "SSAOProcessor.hlsl") == 0)
		return (const char*)SSAOProcessor_hlsl().GetData();
	else if(stricmp(Filename, "TerrainMaterial.hlsl") == 0)
		return (const char*)TerrainMaterial_hlsl().GetData();
	else if(stricmp(Filename, "TextureMaskProcessor.hlsl") == 0)
		return (const char*)TextureMaskProcessor_hlsl().GetData();
	else if(stricmp(Filename, "TextureResizeProcessor.hlsl") == 0)
		return (const char*)TextureResizeProcessor_hlsl().GetData();
	else if(stricmp(Filename, "UI.hlsl") == 0)
		return (const char*)UI_hlsl().GetData();
	else if(stricmp(Filename, "UnsharpenFilterProcessor.hlsl") == 0)
		return (const char*)UnsharpenFilterProcessor_hlsl().GetData();
	else if(stricmp(Filename, "ZoomBlurProcessor.hlsl") == 0)
		return (const char*)ZoomBlurProcessor_hlsl().GetData();
	else 
		return NULL;

}

ZED3D9ShaderManager::ZED3D9ShaderManager()
{

}

ZED3D9ShaderManager::~ZED3D9ShaderManager()
{

}

ZED3D9Shader* ZED3D9ShaderManager::GetShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, ZED3D9ShaderType Type, const char* Profile)
{
	// Check Memory Cache
	ZEUInt32 Hash = CalculateHash(FileName, FunctionName, Components);
	for (ZESize I = 0; I < Shaders.GetCount(); I++)
		if (Shaders[I] != NULL)
			if (Shaders[I]->Hash == Hash)
				if (Shaders[I]->GetComponents() == Components &&
					Shaders[I]->GetShaderType() == Type &&
					stricmp(Shaders[I]->GetFileName(), FileName) == 0 && 
					strcmp(Shaders[I]->GetFunctionName(), FunctionName) == 0)
				{
					Shaders[I]->ReferanceCount++;
					return Shaders[I];
				}

	// Check File Cache
	
	// Compile
	ZED3D9Shader* Shader;
	if (Type == ZE_D3D9_ST_PIXEL)
	{
		LPDIRECT3DPIXELSHADER9 PixelShader = NULL;
		const char* ShaderSource = GetInternal(FileName);
		if (ShaderSource == NULL)
		{
			if (!ZED3D9CommonTools::CompilePixelShader(&PixelShader, FileName, FunctionName, Profile, Components))
				return NULL;
		}
		else
		{
			if (!ZED3D9CommonTools::CompilePixelShaderFromMemory(&PixelShader, ShaderSource, FunctionName, Profile, Components))
				return NULL;
		}

		Shader = new ZED3D9PixelShader();
		((ZED3D9PixelShader*)Shader)->PixelShader = PixelShader;
	} 
	else if (Type == ZE_D3D9_ST_VERTEX)
	{
		LPDIRECT3DVERTEXSHADER9 VertexShader = NULL;
		const char* ShaderSource = GetInternal(FileName);
		if (ShaderSource == NULL)
		{
			if (!ZED3D9CommonTools::CompileVertexShader(&VertexShader, FileName, FunctionName, Profile, Components))
				return NULL;
		}
		else
		{
			if (!ZED3D9CommonTools::CompileVertexShaderFromMemory(&VertexShader, ShaderSource, FunctionName, Profile, Components))
				return NULL;
		}

		Shader = new ZED3D9VertexShader();
		((ZED3D9VertexShader*)Shader)->VertexShader = VertexShader;
	}

	strncpy(Shader->FileName, FileName, 100);
	strncpy(Shader->FunctionName, FunctionName, 100);
	Shader->Components = Components;
	Shader->Hash = CalculateHash(FileName, FunctionName, Components);

	
	// Add to shader list
	bool Found = false;
	for (ZESize I = 0; I < Shaders.GetCount(); I++)
		if (Shaders[I] == NULL)
		{
			Found = true;
			break;
		}

	if (!Found)
		Shaders.Add(Shader);

	return Shader;
}

ZED3D9PixelShader* ZED3D9ShaderManager::GetPixelShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile)
{
	return (ZED3D9PixelShader*)GetShader(FileName, FunctionName, Components, ZE_D3D9_ST_PIXEL, Profile);
}

ZED3D9VertexShader* ZED3D9ShaderManager::GetVertexShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile)
{
	return (ZED3D9VertexShader*)GetShader(FileName, FunctionName, Components, ZE_D3D9_ST_VERTEX, Profile);
}

ZED3D9ShaderManager* ZED3D9ShaderManager::GetInstance()
{
	return ((ZED3D9Module*)ZEGraphicsModule::GetInstance())->GetShaderManager();
}
