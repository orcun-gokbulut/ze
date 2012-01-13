//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShaderManager.h
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

#pragma once
#ifndef	__ZE_D3D9_SHADER_MANAGER_H__
#define __ZE_D3D9_SHADER_MANAGER_H__

#include "ZED3D9Shader.h"
#include "ZEDS/ZEArray.h"
#include "ZEFile/ZEFileCache.h"

class ZED3D9ShaderManager
{
	friend void ZED3D9Shader::Release();
	friend ZED3D9Module;
	private:
		ZESmartArray<ZED3D9Shader*>		Shaders;
		//ZEFileCache						ShaderFileCache;

		ZEUInt32							CalculateHash(const char* FileName, const char* FunctionName, ZEUInt32 Components);
		void							ReleaseShader(ZED3D9Shader* Shader);
		bool							ReadFromFileCache(const char* Filename, const char* FunctionName, ZEUInt32 Components);
		void							WriteToFileCache(const char* Filenamne, const char* FunctionName, ZEUInt32 Components);

										ZED3D9ShaderManager();
										~ZED3D9ShaderManager();
	public:
		ZED3D9Shader*					GetShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, ZED3D9ShaderType Type, const char* Profile);
		ZED3D9PixelShader*				GetPixelShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile);	
		ZED3D9VertexShader*				GetVertexShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile);

		static ZED3D9ShaderManager*		GetInstance();
};
#endif
