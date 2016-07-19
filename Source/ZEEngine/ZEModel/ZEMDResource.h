//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResource.h
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

#include "ZEResource/ZERSResourceLoadable.h"

#include "ZEDS/ZEString.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEResource/ZERSHolder.h"
#include "ZEGraphics/ZEGRVertexLayout.h"

ZE_META_FORWARD_DECLARE(ZEMDResourceMesh, "ZEMDResourceMesh.h");
ZE_META_FORWARD_DECLARE(ZEMDResourceBone, "ZEMDResourceBone.h");
ZE_META_FORWARD_DECLARE(ZEMDResourceAnimation, "ZEMDResourceAnimation.h");
ZE_META_FORWARD_DECLARE(ZEMDResourceHelper, "ZEMDResourceHelper.h");


class ZEMDResource : public ZERSResourceLoadable
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEMDResource)
	friend class ZERSTemplates;
	private:
		ZEList2<ZEMDResourceMesh>						Meshes;
		ZEList2<ZEMDResourceBone>						Bones;
		ZEList2<ZEMDResourceHelper>						Helpers;
		ZEList2<ZEMDResourceAnimation>					Animations;

		bool											BoundingBoxIsUserDefined;
		ZEAABBox										UserDefinedBoundingBox;

		bool											ReadMeshes(const ZEMLReaderNode& MeshesNode);
		bool											ReadBones(const ZEMLReaderNode& BonesNode);
		bool											ReadAnimations(const ZEMLReaderNode& AnimationsNode);
		bool											ReadHelpers(const ZEMLReaderNode& HelpersNode);
		bool											ReadMaterials(const ZEMLReaderNode& MaterialsNode);
		
		virtual ZETaskResult							LoadInternal();
		virtual ZETaskResult							UnloadInternal();

														ZEMDResource();
		virtual											~ZEMDResource();

	public:
		bool											GetUserDefinedBoundingBoxEnabled() const;
		const ZEAABBox&									GetUserDefinedBoundingBox() const;

		const ZEList2<ZEMDResourceMesh>&				GetMeshes();
		const ZEList2<const ZEMDResourceMesh>&			GetMeshes() const;
		void											AddMesh(ZEMDResourceMesh* Mesh);
		void											RemoveMesh(ZEMDResourceMesh* Mesh);

		const ZEList2<ZEMDResourceBone>&				GetBones();
		const ZEList2<const ZEMDResourceBone>&			GetBones() const;
		void											AddBone(ZEMDResourceBone* Bone);
		void											RemoveBone(ZEMDResourceBone* Bone);

		const ZEList2<ZEMDResourceAnimation>&			GetAnimations();
		const ZEList2<const ZEMDResourceAnimation>&		GetAnimations() const;
		void											AddAnimation(ZEMDResourceAnimation* Animation);
		void											RemoveAnimation(ZEMDResourceAnimation* Animation);

		const ZEList2<ZEMDResourceHelper>&				GetHelpers();
		const ZEList2<const ZEMDResourceHelper>&		GetHelpers() const;
		void											AddHelper(ZEMDResourceHelper* Helper);
		void											RemoveHelper(ZEMDResourceHelper* Helper);

		static ZERSHolder<ZEMDResource>					LoadResource(const ZEString& FileName);
		static ZERSHolder<const ZEMDResource>			LoadResourceShared(const ZEString& FileName);
};
