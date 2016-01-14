//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelResource.h
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

#include "ZEResource/ZERSResource.h"
#include "ZEMDResourceMesh.h"
#include "ZEMDResourceBone.h"
#include "ZEMDResourceAnimation.h"
#include "ZEMDResourceHelper.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEDS/ZEString.h"

class ZETexture2DResource;
class ZETexture2D;

class ZEModelResource : public ZERSResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEModelResource)
	private:
		ZEList2<ZEModelResourceMesh> Meshes;
		ZEList2<ZEModelResourceBone> Bones;
		ZEList2<ZEModelResourceHelper> Helpers;
		ZEList2<ZEModelResourceAnimation> Animations;

		bool BoundingBoxIsUserDefined;
		ZEAABBox UserDefinedBoundingBox;

		bool ProcessBones(ZEModelResourceBone* Bone, ZEInt BoneId);
		bool ReadMeshes(const ZEMLReaderNode& MeshesNode);
		bool ReadBones(const ZEMLReaderNode& BonesNode);
		bool ReadAnimations(const ZEMLReaderNode& AnimationsNode);
		bool ReadHelpers(const ZEMLReaderNode& HelpersNode);
		bool ReadMaterials(const ZEMLReaderNode& MaterialsNode);
		virtual bool LoadInternal(const ZERSLoadingOptions* Option);

		virtual ~ZEModelResource();

	public:
		bool GetUserDefinedBoundingBoxEnabled() const;
		const ZEAABBox& GetUserDefinedBoundingBox() const;

		const ZEList2<ZEModelResourceMesh>& GetMeshes();
		const ZEList2<const ZEModelResourceMesh>& GetMeshes() const;
		void AddMesh(ZEModelResourceMesh* Mesh);
		void RemoveMesh(ZEModelResourceMesh* Mesh);

		const ZEList2<ZEModelResourceBone>& GetBones();
		const ZEList2<const ZEModelResourceBone>& GetBones() const;
		void AddBone(ZEModelResourceBone* Bone);
		void RemoveBone(ZEModelResourceBone* Bone);

		const ZEList2<ZEModelResourceAnimation>& GetAnimations();
		const ZEList2<const ZEModelResourceAnimation>& GetAnimations() const;
		void AddAnimation(ZEModelResourceAnimation* Animation);
		void RemoveAnimation(ZEModelResourceAnimation* Animation);

		const ZEList2<ZEModelResourceHelper>& GetHelpers();
		const ZEList2<const ZEModelResourceHelper>& GetHelpers() const;
		void AddHelper(ZEModelResourceHelper* Helper);
		void RemoveHelper(ZEModelResourceHelper* Helper);

		bool Load(const ZEMLReaderNode& ModelNode);
		bool Save(ZEMLWriterNode& ModelNode) const;

		ZEModelResource();

		static ZEHolder<ZEModelResource> Load(const ZEString& FileName);
};
