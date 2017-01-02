//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMultiplexerMaterial.cpp
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

#include "ZERNMultiplexerMaterial.h"

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEResource/ZERSTemplates.h"

void ZERNMultiplexerMaterial::UpdateSelection()
{
	if (!IsLoadedOrLoading())
		return;

	const ZERNMaterial* DefaultConfiguration = NULL;
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		if (Materials[I].Name == SelectedMaterialName)
		{
			SelectedMaterial = Materials[I].Material.GetPointer();
			return;
		}

		if (Materials[I].Name == DefaultMaterialName)
			DefaultConfiguration = Materials[I].Material.GetPointer();
	}

	SelectedMaterial = DefaultConfiguration;
}

ZETaskResult ZERNMultiplexerMaterial::LoadInternal()
{
	if (!GetFileName().IsEmpty())
	{
		ZEMLReader MaterialReader;
		if (!MaterialReader.Open(GetFileName()))
		{
			zeError("Cannot load ZERNMultiplexerMaterial. Cannot open file. File Name: %s", GetFileName().ToCString());
			return ZE_TR_FAILED;
		}

		ZEMLReaderNode MaterialNode = MaterialReader.GetRootNode();
		if (!Unserialize(&MaterialNode))
			return ZE_TR_FAILED;
	}

	return ZE_TR_DONE;
}

ZETaskResult ZERNMultiplexerMaterial::UnloadInternal()
{
	return ZE_TR_DONE;
}

ZERNMultiplexerMaterial::ZERNMultiplexerMaterial()
{
	SelectedMaterial = NULL;
	Register();
}

ZERNMultiplexerMaterial::~ZERNMultiplexerMaterial()
{

}

ZEUInt ZERNMultiplexerMaterial::GetStageMask() const 
{
	if (SelectedMaterial == NULL)
		return 0;

	return SelectedMaterial->GetStageMask();
}

const ZEArray<ZERNMultiplexerSubMaterial>& ZERNMultiplexerMaterial::GetMaterials() const
{
	return Materials;
}

const ZERNMultiplexerSubMaterial* ZERNMultiplexerMaterial::GetMaterial(const ZEString& Name) const
{
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		if (Materials[I].Name == Name)
			return &Materials[I];
	}

	return NULL;
}

bool ZERNMultiplexerMaterial::AddMaterial(const ZEString& Name, ZERNMaterial* Material)
{
	zeCheckError(Material == NULL, false, "Cannot add configuration. Material is NULL. Configuration Name: \"%s\". Material File Name: \"%s\".", Name.ToCString(),this->GetFileName().ToCString());
	zeCheckError(GetMaterial(Name) != NULL, false, "Cannot add configuration. Configuration with the same name has already been added. Configuration Name: \"%s\". Material File Name: \"%s\".", Name.ToCString(),this->GetFileName().ToCString());

	ZERNMultiplexerSubMaterial NewConfiguration;
	NewConfiguration.Name = Name;
	NewConfiguration.Material = Material;

	Materials.Add(NewConfiguration);
	UpdateSelection();

	return true;
}

bool ZERNMultiplexerMaterial::RemoveMaterial(const ZEString& Name)
{
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		if (Materials[I].Name != Name)
			continue;

		Materials.Remove(I);
		UpdateSelection();
		return true;
	}

	zeError("Cannot remove configuration. There is no such a configuration with that name. Configuration Name: \"%s\". Material File Name: \"%s\".", Name.ToCString(),this->GetFileName().ToCString());

	return false;
}

void ZERNMultiplexerMaterial::SetDefaultMaterial(const ZEString& Name)
{
	if (DefaultMaterialName == Name)
		return;

	DefaultMaterialName = Name;

	UpdateSelection();
}

const ZEString& ZERNMultiplexerMaterial::GetDefaultMaterial() const
{
	return DefaultMaterialName;
}

void ZERNMultiplexerMaterial::SetSelectedMaterial(const ZEString& Name)
{
	if (SelectedMaterialName == Name)
		return;

	SelectedMaterialName = Name;

	UpdateSelection();
}

ZEHolder<const ZERNMaterial> ZERNMultiplexerMaterial::GetSelectedMaterial() const
{
	return SelectedMaterial;
}

bool ZERNMultiplexerMaterial::PreRender(ZERNCommand& Command) const
{
	if (SelectedMaterial == NULL)
		return false;

	return SelectedMaterial->PreRender(Command);
}

bool ZERNMultiplexerMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	if (SelectedMaterial == NULL)
		return false;

	return SelectedMaterial->SetupMaterial(Context, Stage, Instanced);
}

void ZERNMultiplexerMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	if (SelectedMaterial == NULL)
		return;

	SelectedMaterial->CleanupMaterial(Context, Stage, Instanced);
}

bool ZERNMultiplexerMaterial::Update()
{
	if (SelectedMaterial == NULL)
		return false;

	return const_cast<ZERNMaterial*>(SelectedMaterial)->Update();
}

bool ZERNMultiplexerMaterial::Serialize(ZEMLWriterNode* MaterialNode)
{
	MaterialNode->WriteString("Class", GetClass()->GetName());
	MaterialNode->WriteString("GUID", GetGUID().ToString());
	MaterialNode->WriteUInt8("VersionMajor", 1);
	MaterialNode->WriteUInt8("VersionMinor", 0);

	ZEMLWriterNode PropertiesNode;
	MaterialNode->OpenNode("Properties", PropertiesNode);
	PropertiesNode.WriteString("DefaultMaterial", DefaultMaterialName);

	ZEMLWriterNode MaterialsNode;
	PropertiesNode.OpenNode("Materials", MaterialsNode);
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		ZEMLWriterNode SubMaterialNode;
		MaterialsNode.OpenNode("Material", SubMaterialNode);
		SubMaterialNode.WriteString("Name", Materials[I].Name);
		SubMaterialNode.WriteString("FileName", (Materials[I].Material != NULL ? Materials[I].Material->GetFileName() : ""));
		SubMaterialNode.CloseNode();
	}
	MaterialsNode.CloseNode();

	PropertiesNode.CloseNode();

	return true;
}

bool ZERNMultiplexerMaterial::Unserialize(ZEMLReaderNode* MaterialNode)
{
	ZEString FileName = MaterialNode->GetFile()->GetPath();

	zeCheckError(MaterialNode == NULL || !MaterialNode->IsValid(), false, "ZERNMultiplexerMaterial loading failed. ZEML Root Node is not valid. File : \"%s\"", FileName.ToCString());

	ZEString ClassName = MaterialNode->ReadString("Class");
	if (ClassName.IsEmpty())
		zeWarning("Class tag in ZERNMaterial is empty. Unknown material class. This can cause problems. File Name: \"%s\".", FileName.ToCString());
	else
		zeCheckError(ClassName != GetClass()->GetName(), false, "Cannot load ZERNMultiplexerMaterial. Class is different than ZERNMultiplexerMaterial. Class Name: \"%s\", File Name: \"%s\".", ClassName.ToCString(), FileName.ToCString());

	ZEMLReaderNode PropertiesNode;
	PropertiesNode = MaterialNode->GetNode("Properties");

	SetGUID(ZEGUID::FromString(MaterialNode->ReadString("GUID")));
	SetName(PropertiesNode.ReadString("Name"));
	SetDefaultMaterial(PropertiesNode.ReadString("DefaultMaterial"));

	ZEMLReaderNode MaterialsNode = PropertiesNode.GetNode("Materials");
	if (MaterialsNode.IsValid())
	{
		ZESize Count = MaterialsNode.GetNodeCount("Material");
		Materials.SetCount(Count);
		for (ZESize I = 0; I < Count; I++)
		{
			ZEMLReaderNode SubMaterialNode = MaterialsNode.GetNode("Material", I);
			Materials[I].Name = SubMaterialNode.ReadString("Name");
			Materials[I].Material = ZERNMaterial::LoadResourceShared(ZEPathInfo::CombineRelativePath(MaterialNode->GetFile()->GetPath(), SubMaterialNode.ReadString("FileName")));
		}
	}

	UpdateSelection();

	return true;
}

ZERNMultiplexerMaterial* ZERNMultiplexerMaterial::CreateInstance()
{
	return new ZERNMultiplexerMaterial();
}

ZEHolder<ZERNMultiplexerMaterial> ZERNMultiplexerMaterial::CreateResource()
{
	return ZERSTemplates::CreateResource<ZERNMultiplexerMaterial>(&ZERSTemplates::InstanciatorFunction<ZERNMultiplexerMaterial>);
}

ZEHolder<ZERNMultiplexerMaterial> ZERNMultiplexerMaterial::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZERNMultiplexerMaterial>(FileName, &ZERSTemplates::InstanciatorFunction<ZERNMultiplexerMaterial>);
}

ZEHolder<const ZERNMultiplexerMaterial> ZERNMultiplexerMaterial::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZERNMultiplexerMaterial>(FileName, &ZERSTemplates::InstanciatorFunction<ZERNMultiplexerMaterial>);

}
