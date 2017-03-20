//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMaterial.cpp
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

#include "ZERNMaterial.h"

#include "ZERNStage.h"
#include "ZERNStageID.h"
#include "ZERNCommand.h"
#include "ZEML/ZEMLReader.h"
#include "ZEMeta/ZEMTProvider.h"
#include "ZEResource/ZERSTemplates.h"

ZEClass* ZERNMaterial::DetermineClass(const ZEString& FileName)
{
	ZEMLReader MaterialFile;
	if (!MaterialFile.Open(FileName))
		return NULL;

	ZEString ClassName = MaterialFile.GetRootNode().ReadString("Class");

	MaterialFile.Close();

	ZEClass* Class = ZEMTProvider::GetInstance()->GetClass(ClassName);
	if (Class == NULL)
		return NULL;

	if (Class == ZERNMaterial::Class())
		return NULL;

	if (!ZEClass::IsDerivedFrom(ZERNMaterial::Class(), Class))
		return NULL;

	return Class;
}

ZERSResource* ZERNMaterial::Instanciator(const void* Params)
{
	const ZEString* FileName = static_cast<const ZEString*>(Params);
	ZEClass* Class = DetermineClass(*FileName);
	if (Class == NULL)
		return NULL;

	return static_cast<ZERSResource*>(Class->CreateInstance());
}

ZERNMaterial::ZERNMaterial()
{
	GUID = ZEGUID::Zero;
}

ZERNMaterial::~ZERNMaterial()
{

}

void ZERNMaterial::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZERNMaterial::GetGUID() const
{
	return GUID;
}

void ZERNMaterial::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZERNMaterial::GetName() const
{
	return Name;
}

bool ZERNMaterial::PreRender(ZERNCommand& Command) const
{
	if (!IsLoaded())
		return false;

	Command.StageMask = GetStageMask();

	return true;
}

bool ZERNMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	if (!IsLoaded())
		return false;

	if (Context == NULL || Stage == NULL || !Stage->GetEnabled())
		return false;

	return true;
}

void ZERNMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{

}

bool ZERNMaterial::Update()
{
	if (!IsLoaded())
		return false;

	return true;
}

bool ZERNMaterial::Serialize(ZEMLWriterNode* MaterialNode)
{
	return false;
}

bool ZERNMaterial::Unserialize(ZEMLReaderNode* MaterialNode)
{
	return false;
}

ZEHolder<ZERNMaterial> ZERNMaterial::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZERNMaterial>(FileName, &Instanciator, &FileName);
}

ZEHolder<const ZERNMaterial> ZERNMaterial::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZERNMaterial>(FileName, &Instanciator, &FileName);
}
