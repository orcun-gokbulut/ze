//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShaderMetaTable.cpp
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

#include "ZEShaderMetaTable.h"

ZEShaderMetaTable::ZEShaderMetaTable()
{
	
}

ZEShaderMetaTable::~ZEShaderMetaTable()
{

}

// Sampler
ZESize ZEShaderMetaTable::GetSamplerCount() const
{
	return Samplers.GetCount();
}

const ZEShaderSamplerInfo* ZEShaderMetaTable::GetSamplerInfo(ZESize Index) const
{
	zeDebugCheck(Index > Samplers.GetCount(), "Index out of range");

	return &Samplers[Index];
}

const ZEShaderSamplerInfo* ZEShaderMetaTable::GetSamplerInfo(const ZEString& Name) const
{
	ZEUInt64 Hash = Name.Hash();
	ZESize Count = Samplers.GetCount();
	
	for(ZESize I = 0; I < Count; ++I)
		if (Samplers[I].Hash == Hash)
			if (Samplers[I].Name.Equals(Name))
				return &Samplers[I];
	
	return NULL;
}

// Texture
ZESize ZEShaderMetaTable::GetTextureCount() const
{
	return Textures.GetCount();
}

const ZEShaderTextureInfo* ZEShaderMetaTable::GetTextureInfo(ZESize Index) const
{
	zeDebugCheck(Index > Textures.GetCount(), "Index out of range");

	return &Textures[Index];
}

const ZEShaderTextureInfo* ZEShaderMetaTable::GetTextureInfo(const ZEString& Name) const
{
	zeDebugCheck(Name.IsEmpty(), "Empty string.");

	ZEUInt64 Hash = Name.Hash();
	ZESize Count = Textures.GetCount();
	
	for(ZESize I = 0; I < Count; ++I)
		if (Textures[I].Hash == Hash)
			if (Textures[I].Name.Equals(Name))
				return &Textures[I];
	
	return NULL;
}

// Buffer
ZESize ZEShaderMetaTable::GetBufferCount() const
{
	return Buffers.GetCount();
}

const ZEShaderBufferInfo* ZEShaderMetaTable::GetBufferInfo(ZESize Index) const
{
	zeDebugCheck(Index > Buffers.GetCount(), "Index out of range");

	return &Buffers[Index];
}

const ZEShaderBufferInfo* ZEShaderMetaTable::GetBufferInfo(const ZEString& Name) const
{
	zeDebugCheck(Name.IsEmpty(), "Empty string.");

	ZEUInt64 Hash = Name.Hash();
	ZESize Count = Buffers.GetCount();
	
	for(ZESize I = 0; I < Count; ++I)
		if (Buffers[I].Hash == Hash)
			if (Buffers[I].Name.Equals(Name))
				return &Buffers[I];

	return NULL;
}

// Constant
ZESize ZEShaderMetaTable::GetConstantCount(ZESize BufferIndex) const
{
	zeDebugCheck(BufferIndex > Buffers.GetCount(), "Index out of range");

	return Buffers[BufferIndex].Constants.GetCount();
}

const ZEShaderConstantInfo* ZEShaderMetaTable::GetConstantInfo(ZESize BufferIndex, ZESize Index) const
{
	zeDebugCheck(BufferIndex > Buffers.GetCount(), "Index out of range");
	zeDebugCheck(Index > Buffers[BufferIndex].Constants.GetCount(), "Index out of range");
	
	return &Buffers[BufferIndex].Constants[Index];
}

const ZEShaderConstantInfo* ZEShaderMetaTable::GetConstantInfo(ZESize BufferIndex, const ZEString& Name) const
{
	zeDebugCheck(BufferIndex > Buffers.GetCount(), "Index out of range");

	ZEUInt64 Hash = Name.Hash();
	ZESize Count = Buffers[BufferIndex].Constants.GetCount();

	for(ZESize I = 0; I < Count; ++I)
		if (Buffers[BufferIndex].Constants[I].Hash == Hash)
			if (Buffers[BufferIndex].Constants[I].Name.Equals(Name))
				return &Buffers[BufferIndex].Constants[I];
	
	return NULL;
}

// Input Signature
ZESize ZEShaderMetaTable::GetInputCount() const
{
	return Inputs.GetCount();
}

const ZEShaderInputInfo* ZEShaderMetaTable::GetInputInfo(ZESize Index) const
{
	zeDebugCheck(Index > Inputs.GetCount(), "Index out of range");

	return &Inputs[Index];
}


const ZEShaderInputInfo* ZEShaderMetaTable::GetInputInfo(const ZEString& Semantic, ZEUInt32 SemanticIndex) const
{
	zeDebugCheck(Semantic.IsEmpty(), "Empty string.");

	ZEUInt64 Hash = Semantic.Hash();
	ZESize Count = Inputs.GetCount();
	
	for(ZESize I = 0; I < Count; ++I)
		if (Inputs[I].Hash == Hash)
			if (Inputs[I].SemanticIndex == SemanticIndex)
				if (Inputs[I].Semantic.Equals(Semantic))
					return &Inputs[I];
	
	return NULL;
}

// Compile Options
ZEShaderType ZEShaderMetaTable::GetShaderType() const
{
	return CompileOptions.Type;
}

ZEShaderModel ZEShaderMetaTable::GetShaderModel() const
{
	return CompileOptions.Model;
}

const ZEString&	ZEShaderMetaTable::GetShaderFileName() const
{
	return CompileOptions.FileName;
}

const ZEString&	ZEShaderMetaTable::GetShaderEntryPoint() const
{
	return CompileOptions.EntryPoint;
}

ZESize ZEShaderMetaTable::GetShaderParameterCount() const
{
	return CompileOptions.Parameters.GetCount();
}

const ZEShaderCompilerParameter* ZEShaderMetaTable::GetShaderParameter(ZESize Index) const
{
	zeDebugCheck(Index > CompileOptions.Parameters.GetCount(), "Index out of range");

	return &CompileOptions.Parameters[Index];
}

const ZEShaderCompilerParameter* ZEShaderMetaTable::GetShaderParameter(const ZEString& Name) const
{
	ZEUInt64 Hash = Name.Hash();
	ZESize Count = CompileOptions.Parameters.GetCount();

	for(ZESize I = 0; I < Count; ++I)
		if (CompileOptions.Parameters[I].Hash == Hash)
			if (CompileOptions.Parameters[I].Name.Equals(Name))
				return &CompileOptions.Parameters[I];
	
	return NULL;
}
