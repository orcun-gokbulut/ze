//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRShaderCompileOptions.cpp
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

#include "ZEGRShaderCompileOptions.h"

ZEGRShaderDefinition::ZEGRShaderDefinition()
{
}

ZEGRShaderDefinition::ZEGRShaderDefinition(const ZEString& Name, const ZEString& Value)
{
	this->Name = Name;
	this->Value = Value;
}

ZEUInt32 ZEGRShaderCompileOptions::GetHash() const
{
	ZEString Data;
	Data = ZEString::FromUInt8(Type) + ZEString::FromUInt8(Model) + FileName + EntryPoint;

	ze_for_each(IncDir, IncludeDirectories)
		Data += IncDir.GetItem();

	ze_for_each(Def, Definitions)
		Data += Def->Name + Def->Value;

	ZEUInt32 Hash = 0x811c9dc5;
	for (ZEUInt I = 0; I < Data.GetLength(); I++)
	{
		Hash ^= (*reinterpret_cast<ZEBYTE*>(&Data[I]));
		Hash *= 16777619;
	}

	return Hash;

}

bool ZEGRShaderCompileOptions::Equal(const ZEGRShaderCompileOptions& Other) const
{
	if (this->Type != Other.Type || 
		this->Model != Other.Model || 
		this->FileName != Other.FileName || 
		this->EntryPoint != Other.EntryPoint)
		return false;

	
	if (this->IncludeDirectories.GetCount() != Other.IncludeDirectories.GetCount())
		return false;

	for (ZEUInt I = 0; I < this->IncludeDirectories.GetCount(); I++)
	{
		if (this->IncludeDirectories[I] != Other.IncludeDirectories[I])
			return false;
	}

	if (this->Definitions.GetCount() != Other.Definitions.GetCount())
		return false;

	for (ZEUInt I = 0; I < this->Definitions.GetCount(); I++)
	{
		if (this->Definitions[I].Name != Other.Definitions[I].Name || 
			this->Definitions[I].Value != Other.Definitions[I].Value)
			return false;
	}
	
	return true;
}

ZEGRShaderCompileOptions::ZEGRShaderCompileOptions()
{
	Type = ZEGR_ST_NONE;
	Model = ZEGR_SM_5_0;
	Debug = false;
	OptimizationLevel = 3;
}
