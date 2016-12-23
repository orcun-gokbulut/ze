//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetDependency.cpp
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

#include "ZEDAssetDependency.h"

#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

void ZEDAssetDependency::SetType(ZEDAssetDependencyType Type)
{
	this->Type = Type;
}

ZEDAssetDependencyType ZEDAssetDependency::GetType() const
{
	return Type;
}

void ZEDAssetDependency::SetRequired(bool Required)
{
	this->Required = Required;
}

bool ZEDAssetDependency::GetRequired() const
{
	return Required;
}

void ZEDAssetDependency::SetTargetType(ZEDAssetDependencyTargetType Type)
{
	this->TargetType = Type;
}

ZEDAssetDependencyTargetType ZEDAssetDependency::GetTargetType() const
{
	return TargetType;
}

void ZEDAssetDependency::SetTarget(const ZEString& Target)
{
	this->Target = Target;
}

const ZEString& ZEDAssetDependency::GetTarget() const
{
	return Target;
}

bool ZEDAssetDependency::Load(ZEMLReaderNode* DependencyNode)
{
	zeCheckError(DependencyNode == NULL, false, "Cannot load asset dependency meta data. DependencyNode is NULL");
	zeCheckError(DependencyNode->GetName() != "Dependency", false, "Cannot load asset dependency meta data. Unknown dependency node name.");

	SetType((ZEDAssetDependencyType)DependencyNode->ReadUInt8("TargetType", ZED_ADT_NONE));
	SetRequired(DependencyNode->ReadBoolean("TargetType", true));
	SetTargetType((ZEDAssetDependencyTargetType)DependencyNode->ReadUInt8("TargetType", ZED_ADTT_NONE));
	SetTarget(DependencyNode->ReadString("TargetType"));

	return true;
}

bool ZEDAssetDependency::Save(ZEMLWriterNode* DependenciesNode) const
{
	zeCheckError(DependenciesNode == NULL, false, "Cannot load asset dependency meta data. DependenciesNode is NULL");

	ZEMLWriterNode DependencyNode;
	if (!DependenciesNode->OpenNode("Dependency", DependencyNode))
	{

	}

	DependenciesNode->WriteUInt8("Type", (ZEUInt8)GetType());
	DependenciesNode->WriteBool("Required", GetRequired());
	DependenciesNode->WriteUInt8("TargetType", (ZEUInt8)GetTargetType());
	DependenciesNode->WriteString("Target", GetTarget());

	return true;
}

ZEDAssetDependency::ZEDAssetDependency()
{
	Type = ZED_ADT_NONE;
	Required = true;
	TargetType = ZED_ADTT_NONE;
}
