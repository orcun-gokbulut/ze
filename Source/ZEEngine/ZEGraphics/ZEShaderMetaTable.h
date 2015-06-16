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
#include "ZEShaderMeta.h"
#include "ZEShaderCompileOptions.h"


class ZEShaderMetaTable
{
	friend class ZEShader;
	friend class ZEShaderCompiler;

	public:
		ZEArray<ZEShaderSampler>		Samplers;
		ZEArray<ZEShaderTexture>		Textures;
		ZEArray<ZEShaderBuffer>			Buffers;
		ZEArray<ZEShaderInput>			Inputs;
		ZEShaderCompileOptions			CompileOptions;

	protected:
										ZEShaderMetaTable();
		virtual							~ZEShaderMetaTable();

	public:
		ZEShaderType					GetShaderType() const;
		ZEShaderModel					GetShaderModel() const;
		const ZEString&					GetShaderFileName() const;
		const ZEString&					GetShaderEntryPoint() const;
		ZESize							GetShaderParameterCount() const;
		const ZEShaderParameter*		GetShaderParameter(ZEUInt Index) const;
		const ZEShaderParameter*		GetShaderParameter(const char* Name) const;

		ZESize							GetSamplerCount() const;
		const ZEShaderSampler*			GetSamplerInfo(ZEUInt Index) const;
		const ZEShaderSampler*			GetSamplerInfo(const char* Name) const;
		
		ZESize							GetTextureCount() const;
		const ZEShaderTexture*			GetTextureInfo(ZEUInt Index) const;
		const ZEShaderTexture*			GetTextureInfo(const char* Name) const;

		ZESize							GetBufferCount() const;
		const ZEShaderBuffer*			GetBufferInfo(ZEUInt Index) const;
		const ZEShaderBuffer*			GetBufferInfo(const char* Name) const;
		
		ZESize							GetConstantCount(ZEUInt BufferIndex) const;
		const ZEShaderConstant*			GetConstantInfo(ZEUInt BufferIndex, ZEUInt Index) const;
		const ZEShaderConstant*			GetConstantInfo(ZEUInt BufferIndex, const char* Name) const;

		ZESize							GetInputCount() const;
		const ZEShaderInput*			GetInputInfo(ZEUInt Index) const;
		const ZEShaderInput*			GetInputInfo(const char* Semantic, ZEUInt8 SemantixIndex) const;

};

#endif
