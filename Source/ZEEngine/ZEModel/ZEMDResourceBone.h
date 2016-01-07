//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceBone.h
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

#include "ZEMeta\ZEObject.h"
#include "ZEDS\ZELink.h"
#include "ZEDS\ZEString.h"
#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEAABBox.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMDResourcePhysics.h"

class ZEMLReaderNode;
class ZEMLWriterNode;

class ZEModelResourceBone : public ZEObject
{
	ZE_OBJECT
	friend class ZEModelResource;
	private:
		ZELink<ZEModelResourceBone> Link;

		ZEString Name;
		ZEAABBox BoundingBox;
		ZEInt32 ParentBone;
		ZEVector3 Position;
		ZEQuaternion Rotation;
		ZEVector3 Scale;
		ZEMatrix4x4 LocalTransform;
		ZEMatrix4x4 ForwardTransform;
		ZEMatrix4x4 InverseTransform;
		ZEModelResourcePhysicalBody PhysicalBody;
		ZEModelResourcePhysicalJoint PhysicalJoint;
		ZEString UserDefinedProperties;

	public:
		void SetName(const ZEString& Name);
		const ZEString& GetName() const;

		void SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox& GetBoundingBox() const;

		void SetParentBone(ZEInt32 ParentBone);
		ZEInt32 GetParentBone() const;

		void SetPosition(const ZEVector3& Position);
		const ZEVector3& GetPosition() const;

		void SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion& GetRotation() const;

		void SetScale(const ZEVector3& Scale);
		const ZEVector3& GetScale() const;

		void SetLocalTransform(const ZEMatrix4x4& Transform);
		const ZEMatrix4x4& GetLocalTransform() const;

		void SetForwardTransform(const ZEMatrix4x4& Transform);
		const ZEMatrix4x4& GetForwardTransform() const;

		void SetInverseTransform(const ZEMatrix4x4& Transform);
		const ZEMatrix4x4& GetInverseTransform() const;

		void SetPhysicalJoint(const ZEModelResourcePhysicalBody& Body);
		const ZEModelResourcePhysicalBody& GetPhysicalBody() const;
		ZEModelResourcePhysicalBody& GetPhysicalBody();

		void SetPhysicalJoint(const ZEModelResourcePhysicalJoint& Joint);
		const ZEModelResourcePhysicalJoint& GetPhysicalJoint() const;
		ZEModelResourcePhysicalJoint& GetPhysicalJoint();

		void SetUserDefinedProperties(const ZEString& UserDefinedProperties);
		const ZEString& GetUserDefinedProperties() const;

		bool Load(const ZEMLReaderNode& BoneNode);
		bool Save(ZEMLWriterNode& BoneNode) const;

		ZEModelResourceBone();
};
