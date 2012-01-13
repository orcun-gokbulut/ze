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

ZEUInt32 ZED3D9ShaderManager::CalculateHash(const char* FileName, const char* FunctionName, ZEUInt32 Components)
{
	ZEUInt32 Hash = 0;
	size_t Index = 0;

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
		for (size_t I = 0; I < Shaders.GetCount(); I++)
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
	for (size_t I = 0; I < Shaders.GetCount(); I++)
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
		if (!ZED3D9CommonTools::CompilePixelShader(&PixelShader, FileName, FunctionName, Profile, Components))
			return NULL;

		Shader = new ZED3D9PixelShader();
		((ZED3D9PixelShader*)Shader)->PixelShader = PixelShader;
	} 
	else if (Type == ZE_D3D9_ST_VERTEX)
	{
		LPDIRECT3DVERTEXSHADER9 VertexShader = NULL;
		if (!ZED3D9CommonTools::CompileVertexShader(&VertexShader, FileName, FunctionName, Profile, Components))
			return NULL;

		Shader = new ZED3D9VertexShader();
		((ZED3D9VertexShader*)Shader)->VertexShader = VertexShader;
	}

	strncpy(Shader->FileName, FileName, 100);
	strncpy(Shader->FunctionName, FunctionName, 100);
	Shader->Components = Components;
	Shader->Hash = CalculateHash(FileName, FunctionName, Components);

	
	// Add to shader list
	bool Found = false;
	for (size_t I = 0; I < Shaders.GetCount(); I++)
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
