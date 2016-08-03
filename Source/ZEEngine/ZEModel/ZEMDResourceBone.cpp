//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceBone.cpp
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

#include "ZEMDResourceBone.h"

#include "ZEML/ZEMLReader.h"
#include "ZEMDResource.h"

ZEMDResourceBone::ZEMDResourceBone() : Link(this)
{
	Resource = NULL;
	ParentBoneId = -1;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	BoundingBox = ZEAABBox::Zero;
}

ZEMDResourceBone::~ZEMDResourceBone()
{
	if (GetResource() != NULL)
		GetResource()->RemoveBone(this);
}

ZEMDResource* ZEMDResourceBone::GetResource()
{
	return Resource;
}

const ZEMDResource* ZEMDResourceBone::GetResource() const
{
	return Resource;
}

void ZEMDResourceBone::SetName(const ZEString& Value)
{
	Name = Value;
}

const ZEString& ZEMDResourceBone::GetName() const 
{
	return Name;
}

void ZEMDResourceBone::SetBoundingBox(const ZEAABBox& Value)
{
	BoundingBox = Value;
}

const ZEAABBox& ZEMDResourceBone::GetBoundingBox() const 
{
	return BoundingBox;
}
void ZEMDResourceBone::SetParentBoneId(ZEInt32 BoneId)
{
	ParentBoneId = BoneId;
}

ZEInt32 ZEMDResourceBone::GetParentBoneId() const 
{
	return ParentBoneId;
}
void ZEMDResourceBone::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEMDResourceBone::GetPosition() const 
{
	return Position;
}
void ZEMDResourceBone::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEMDResourceBone::GetRotation() const 
{
	return Rotation;
}
void ZEMDResourceBone::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEMDResourceBone::GetScale() const 
{
	return Scale;
}

const ZEMDResourcePhysicalBody& ZEMDResourceBone::GetPhysicalBody() const
{
	return PhysicalBody;
}

ZEMDResourcePhysicalBody& ZEMDResourceBone::GetPhysicalBody()
{
	return PhysicalBody;
}

void ZEMDResourceBone::SetPhysicalJoint(const ZEMDResourcePhysicalBody& Body)
{
	PhysicalBody = Body;
}

void ZEMDResourceBone::SetPhysicalJoint(const ZEMDResourcePhysicalJoint& Joint)
{
	PhysicalJoint = Joint;
}

const ZEMDResourcePhysicalJoint& ZEMDResourceBone::GetPhysicalJoint() const
{
	return PhysicalJoint;
}

ZEMDResourcePhysicalJoint& ZEMDResourceBone::GetPhysicalJoint()
{
	return PhysicalJoint;
}

void ZEMDResourceBone::SetUserDefinedProperties(const ZEString& UserDefinedProperties)
{
	this->UserDefinedProperties = UserDefinedProperties;
}

const ZEString& ZEMDResourceBone::GetUserDefinedProperties() const
{
	return UserDefinedProperties;
}

bool ZEMDResourceBone::Unserialize(const ZEMLReaderNode& BoneNode)
{
	zeCheckError(!BoneNode.IsValid(), false, "Invalid Bone node.");
	zeCheckError(BoneNode.GetName() != "Bone", false, "Invalid Bone node name.");

	SetName(BoneNode.ReadString("Name"));
	SetParentBoneId(BoneNode.ReadInt32("ParentBone", -1));
	SetPosition(BoneNode.ReadVector3("RelativePosition", ZEVector3::Zero));
	SetRotation(BoneNode.ReadQuaternion("RelativeRotation", ZEQuaternion::Identity));
	SetScale(BoneNode.ReadVector3("RelativeScale", ZEVector3::One));
	SetUserDefinedProperties(BoneNode.ReadString("UserDefinedProperties"));

	ZEMLReaderNode BoundingBoxNode = BoneNode.GetNode("BoundingBox");
	ZEAABBox BoundingBox;
	BoundingBox.Max = BoundingBoxNode.ReadVector3("Max", ZEVector3::Zero);
	BoundingBox.Min = BoundingBoxNode.ReadVector3("Min", ZEVector3::Zero);

	ZEMLReaderNode PhysicalJointNode = BoneNode.GetNode("PhysicalJoint");
	if (PhysicalJointNode.IsValid())
	{
		if (!PhysicalJoint.Unserialize(PhysicalJointNode))
			return false;
	}
	else
	{
		PhysicalJoint = ZEMDResourcePhysicalJoint();
	}

	ZEMLReaderNode PhysicalBodyNode = BoneNode.GetNode("PhysicalBody");
	if (PhysicalBodyNode.IsValid())
	{
		if (!PhysicalBody.Unserialize(PhysicalBodyNode))
			return false;
	}
	else
	{
		PhysicalBody = ZEMDResourcePhysicalBody();
	}

	return true;
}

bool ZEMDResourceBone::Serialize(ZEMLWriterNode& BoneNode) const
{
	return false;
}

ZEMDResourceBone* ZEMDResourceBone::CreateInstance()
{
	return new ZEMDResourceBone();
}
