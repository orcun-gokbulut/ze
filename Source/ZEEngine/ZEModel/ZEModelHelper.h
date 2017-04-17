//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelHelper.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEDestroyable.h"

#include "ZEModelMesh.h"
#include "ZEModelBone.h"
#include "ZEResource/ZERSResource.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEExport.ZEEngine.h"

ZEMT_FORWARD_DECLARE(ZEModel);
ZEMT_FORWARD_DECLARE(ZEModelMesh);
ZEMT_FORWARD_DECLARE(ZEModelBone);

enum ZEModelHelperParentType
{
	ZE_MHPT_MODEL			= 0,
	ZE_MHPT_MESH			= 1,
	ZE_MHPT_BONE			= 2
};

class ZE_EXPORT_ZEENGINE ZEModelHelper : public ZEObject, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEModel;
	private:
		ZEModel*								Model;
		ZELink<ZEModelHelper>					ModelLink;

		ZEModelHelperParentType					ParentType;
		ZEModelMesh*							ParentMesh;
		ZEModelBone*							ParentBone;

		ZEString								Name;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;
		ZEString								UserDefinedProperties;

		bool									Load(const ZEMDResourceHelper* Resource);
		bool									Unload();
	
												ZEModelHelper();
		virtual									~ZEModelHelper();

	public:
		ZEModel*								GetModel() const;

		ZEModelHelperParentType					GetParentType() const;

		void									SetParentMesh(ZEModelMesh* Mesh);
		ZEModelMesh*							GetParentMesh() const;

		void									SetParentBone(ZEModelBone* Bone);
		ZEModelBone*							GetParentBone() const;

		void									SetName(const ZEString& Name);
		const ZEString&							GetName() const;

		void									SetPosition(const ZEVector3& Position);
		const ZEVector3&						GetPosition() const;

		void									SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&						GetRotation() const;

		void									SetScale(const ZEVector3& Scale);
		const ZEVector3&						GetScale() const;

		ZEVector3								GetModelPosition() const;
		ZEVector3								GetWorldPosition() const;

		ZEQuaternion							GetModelRotation() const;
		ZEQuaternion							GetWorldRotation() const;

		ZEVector3								GetModelScale() const;
		ZEVector3								GetWorldScale() const;

		ZEVector3								GetFront() const;
		ZEVector3								GetRight() const;
		ZEVector3								GetUp() const;

		ZEVector3								GetModelFront() const;
		ZEVector3								GetModelRight() const;
		ZEVector3								GetModelUp() const;

		ZEVector3								GetWorldFront() const;
		ZEVector3								GetWorldRight() const;
		ZEVector3								GetWorldUp() const;

		void									SetUserDefinedProperties(const ZEString& Properties);
		const ZEString&							GetUserDefinedProperties() const;

		static ZEModelHelper*					CreateInstance();
};
