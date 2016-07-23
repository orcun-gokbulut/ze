//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceHelper.cpp
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

#include "ZEMDResourceHelper.h"

#include "ZEML\ZEMLReader.h"

void ZEMDResourceHelper::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEMDResourceHelper::GetName() const 
{
	return Name;
}

void ZEMDResourceHelper::SetOwnerType(ZEMDResourceHelperParentType OwnerType)
{
	this->OwnerType = OwnerType;
}

ZEMDResourceHelperParentType ZEMDResourceHelper::GetOwnerType() const 
{
	return OwnerType;
}

void ZEMDResourceHelper::SetOwnerId(ZEInt32 OwnerId)
{
	this->OwnerId = OwnerId;
}

ZEInt32 ZEMDResourceHelper::GetOwnerId() const 
{
	return OwnerId;
}

void ZEMDResourceHelper::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEMDResourceHelper::GetPosition() const 
{
	return Position;
}

void ZEMDResourceHelper::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEMDResourceHelper::GetRotation() const 
{
	return Rotation;
}

void ZEMDResourceHelper::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEMDResourceHelper::GetScale() const 
{
	return Scale;
}

void ZEMDResourceHelper::SetUserDefinedProperties(ZEString UserDefinedProperties)
{
	this->UserDefinedProperties = UserDefinedProperties;
}

ZEString ZEMDResourceHelper::GetUserDefinedProperties() const 
{
	return UserDefinedProperties;
}

bool ZEMDResourceHelper::Unserialize(const ZEMLReaderNode& HelperNode)
{
	zeCheckError(!HelperNode.IsValid(), false, "Invalid Helper node.");
	zeCheckError(HelperNode.GetName() != "Helper", false, "Invalid Helper node name.");

	SetName(HelperNode.ReadString("Name"));
	SetOwnerType((ZEMDResourceHelperParentType)HelperNode.ReadInt32("OwnerType", ZE_MRHPT_MODEL));
	SetOwnerId(HelperNode.ReadInt32("OwnerId", -1));
	SetPosition(HelperNode.ReadVector3("Position", ZEVector3::Zero));
	SetRotation(HelperNode.ReadQuaternion("Rotation", ZEQuaternion::Identity));
	SetScale(HelperNode.ReadVector3("Scale", ZEVector3::One));
	SetUserDefinedProperties(HelperNode.ReadString("UserDefinedProperties"));

	return true;
}

bool ZEMDResourceHelper::Serialize(ZEMLWriterNode& HelperNode) const
{
	return false;
}

ZEMDResourceHelper::ZEMDResourceHelper() : Link(this)
{
	OwnerType = ZE_MRHPT_MODEL;
	OwnerId = -1;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}
