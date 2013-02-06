//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShaderMetaTable.h
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
#ifndef __ZE_SHADER_META_TABLE_H__
#define __ZE_SHADER_META_TABLE_H__

#include "ZETypes.h"
#include "ZETexture.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEShaderMetaInfo.h"
#include "ZEShaderCompileOptions.h"


class ZEShaderMetaTable
{
	friend class ZEShader;
	friend class ZEShaderCompiler;

	// Should be public for only internal usage
	public:
		ZEArray<ZEShaderSamplerInfo>		Samplers;
		ZEArray<ZEShaderTextureInfo>		Textures;
		ZEArray<ZEShaderBufferInfo>			Buffers;
		ZEArray<ZEShaderInputInfo>			Inputs;
		ZEShaderCompileOptions				CompileOptions;

	protected:
											ZEShaderMetaTable();
		virtual								~ZEShaderMetaTable();

	public:
											// Compile Options
		ZEShaderType						GetShaderType() const;
		ZEShaderModel						GetShaderModel() const;
		const ZEString&						GetShaderFileName() const;
		const ZEString&						GetShaderEntryPoint() const;
		ZESize								GetShaderParameterCount() const;
		const ZEShaderCompilerParameter*	GetShaderParameter(ZESize Index) const;
		const ZEShaderCompilerParameter*	GetShaderParameter(const ZEString& Name) const;

											// Sampler
		ZESize								GetSamplerCount() const;
		const ZEShaderSamplerInfo*			GetSamplerInfo(ZESize Index) const;
		const ZEShaderSamplerInfo*			GetSamplerInfo(const ZEString& Name) const;
		
											// Texture
		ZESize								GetTextureCount() const;
		const ZEShaderTextureInfo*			GetTextureInfo(ZESize Index) const;
		const ZEShaderTextureInfo*			GetTextureInfo(const ZEString& Name) const;

											// Buffer
		ZESize								GetBufferCount() const;
		const ZEShaderBufferInfo*			GetBufferInfo(ZESize Index) const;
		const ZEShaderBufferInfo*			GetBufferInfo(const ZEString& Name) const;
													
											// Constant
		ZESize								GetConstantCount(ZESize BufferIndex) const;
		const ZEShaderConstantInfo*			GetConstantInfo(ZESize BufferIndex, ZESize Index) const;
		const ZEShaderConstantInfo*			GetConstantInfo(ZESize BufferIndex, const ZEString& Name) const;

											// Input Signature
		ZESize								GetInputCount() const;
		const ZEShaderInputInfo*			GetInputInfo(ZESize Index) const;
		const ZEShaderInputInfo*			GetInputInfo(const ZEString& Semantic, ZEUInt32 SemantixIndex) const;

};

#endif
