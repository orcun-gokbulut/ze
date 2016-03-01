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
#ifndef	__ZE_MODEL_HELPER_H__
#define __ZE_MODEL_HELPER_H__

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMeta/ZEObject.h"

enum ZEModelHelperOwnerType
{
	ZE_MHOT_MODEL			= 0,
	ZE_MHOT_MESH			= 1,
	ZE_MHOT_BONE			= 2
};
struct ZEModelResourceHelper;
ZE_META_FORWARD_DECLARE(ZEModel, "ZEModel.h")
ZE_META_FORWARD_DECLARE(ZEModelMesh,"ZEModelMesh.h")
ZE_META_FORWARD_DECLARE(ZEModelBone,"ZEModelBone.h")

#include "ZEModelMesh.h"
#include "ZEModelBone.h"

class ZEModelHelper : public ZEObject
{
	ZE_OBJECT

	private:

		ZEModel*							OwnerModel;

		ZEModelHelperOwnerType				OwnerType;

		ZEModelMesh*						OwnerMesh;
		ZEModelBone*						OwnerBone;

		const ZEModelResourceHelper*		HelperResource;

		ZEVector3							Position;
		ZEVector3							Scale;
		ZEQuaternion						Rotation;

	public:

		const char*							GetName() const;

		ZEModelHelperOwnerType				GetOwnerType() const;
		const ZEString&						GetUserDefinedProperties() const;

		ZEModelMesh*						GetMeshOwner() const;
		ZEModelBone*						GetBoneOwner() const;

		void								SetPosition(const ZEVector3& LocalPosition);
		const ZEVector3&					GetPosition() const;
		const ZEVector3						GetModelPosition() const;
		const ZEVector3						GetWorldPosition() const;

		void								SetRotation(const ZEQuaternion& LocalRotation);
		const ZEQuaternion&					GetRotation() const;
		const ZEQuaternion					GetModelRotation() const;
		const ZEQuaternion					GetWorldRotation() const;

		void								SetScale(const ZEVector3& LocalScale);
		const ZEVector3&					GetScale() const;
		const ZEVector3						GetModelScale() const;
		const ZEVector3						GetWorldScale() const;

		ZEVector3							GetFront() const;
		ZEVector3							GetRight() const;
		ZEVector3							GetUp() const;

		ZEVector3							GetModelFront() const;
		ZEVector3							GetModelRight() const;
		ZEVector3							GetModelUp() const;

		ZEVector3							GetWorldFront() const;
		ZEVector3							GetWorldRight() const;
		ZEVector3							GetWorldUp() const;

		void								Initialize(ZEModel* Model, const ZEModelResourceHelper* HelperResource);
		void								Deinitialize();

											ZEModelHelper();

};



#endif
