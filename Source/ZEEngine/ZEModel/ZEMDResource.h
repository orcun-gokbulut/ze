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

#include "ZEResource/ZERSResource.h"

#include "ZEDS/ZEString.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEResource/ZERSHolder.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEMDVertex.h"
#include "ZEExport.ZEEngine.h"

ZEMT_FORWARD_DECLARE(ZEMDResourceMesh);
ZEMT_FORWARD_DECLARE(ZEMDResourceBone);
ZEMT_FORWARD_DECLARE(ZEMDResourceAnimation);
ZEMT_FORWARD_DECLARE(ZEMDResourceHelper);
ZEMT_FORWARD_DECLARE(ZEGRBuffer);

class ZE_EXPORT_ZEENGINE ZEMDResource : public ZERSResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEMDResource)
	friend class ZERSTemplates;
	friend class ZEMDResourceMesh;
	friend class ZEMDResourceLOD;
	friend class ZEMDResourceDraw;
	private:
		ZEList2<ZEMDResourceMesh>						Meshes;
		ZEList2<ZEMDResourceBone>						Bones;
		ZEList2<ZEMDResourceHelper>						Helpers;
		ZEList2<ZEMDResourceAnimation>					Animations;

		ZEArray<ZEMDVertex>								Vertices;
		ZEArray<ZEMDVertexSkin>							VerticesSkin;

		ZEArray<ZEUInt16>								Indices;
		ZEArray<ZEUInt32>								Indices32;

		ZEHolder<const ZEGRBuffer>						VertexBuffers[ZEMD_VT_COUNT];
		ZEHolder<const ZEGRBuffer>						IndexBuffers[ZEMD_VIT_COUNT];

		bool											BoundingBoxIsUserDefined;
		ZEAABBox										UserDefinedBoundingBox;

		bool											ReadMeshes(const ZEMLReaderNode& MeshesNode);
		bool											ReadBones(const ZEMLReaderNode& BonesNode);
		bool											ReadAnimations(const ZEMLReaderNode& AnimationsNode);
		bool											ReadHelpers(const ZEMLReaderNode& HelpersNode);
		
		virtual ZETaskResult							LoadInternal();
		virtual ZETaskResult							UnloadInternal();

														ZEMDResource();
		virtual											~ZEMDResource();

	public:
		bool											GetUserDefinedBoundingBoxEnabled() const;
		const ZEAABBox&									GetUserDefinedBoundingBox() const;

		const ZEList2<ZEMDResourceMesh>&				GetMeshes();
		const ZEList2<const ZEMDResourceMesh>&			GetMeshes() const;
		ZEMDResourceMesh*								GetMesh(const ZEString& Name);
		const ZEMDResourceMesh*							GetMesh(const ZEString& Name) const;	
		void											AddMesh(ZEMDResourceMesh* Mesh);
		void											RemoveMesh(ZEMDResourceMesh* Mesh);

		const ZEList2<ZEMDResourceBone>&				GetBones();
		const ZEList2<const ZEMDResourceBone>&			GetBones() const;
		ZEMDResourceBone*								GetBone(const ZEString& Name);
		const ZEMDResourceBone*							GetBone(const ZEString& Name) const;	
		void											AddBone(ZEMDResourceBone* Bone);
		void											RemoveBone(ZEMDResourceBone* Bone);

		const ZEList2<ZEMDResourceAnimation>&			GetAnimations();
		const ZEList2<const ZEMDResourceAnimation>&		GetAnimations() const;
		ZEMDResourceAnimation*							GetAnimation(const ZEString& Name);
		const ZEMDResourceAnimation*					GetAnimation(const ZEString& Name) const;	
		void											AddAnimation(ZEMDResourceAnimation* Animation);
		void											RemoveAnimation(ZEMDResourceAnimation* Animation);

		const ZEList2<ZEMDResourceHelper>&				GetHelpers();
		const ZEList2<const ZEMDResourceHelper>&		GetHelpers() const;
		ZEMDResourceHelper*								GetHelper(const ZEString& Name);
		const ZEMDResourceHelper*						GetHelper(const ZEString& Name) const;	
		void											AddHelper(ZEMDResourceHelper* Helper);
		void											RemoveHelper(ZEMDResourceHelper* Helper);
		
		void											SetVertices(const ZEArray<ZEMDVertex>& Vertices);
		const ZEArray<ZEMDVertex>&						GetVertices() const;
		
		void											SetVerticesSkin(const ZEArray<ZEMDVertexSkin>& VerticesSkin);
		const ZEArray<ZEMDVertexSkin>&					GetVerticesSkin() const;
		
		void											SetIndices(const ZEArray<ZEUInt16>& Indices);
		const ZEArray<ZEUInt16>&						GetIndices() const;
		
		void											SetIndices32(const ZEArray<ZEUInt32>& Indices);
		const ZEArray<ZEUInt32>&						GetIndices32() const;

		void											SetVertexBuffer(const ZEGRBuffer* VertexBuffer, ZEMDVertexType VertexType);
		const ZEGRBuffer*								GetVertexBuffer(ZEMDVertexType VertexType) const;

		void											SetIndexBuffer(const ZEGRBuffer* IndexBuffer, ZEMDVertexIndexType IndexType);
		const ZEGRBuffer*								GetIndexBuffer(ZEMDVertexIndexType IndexType) const;

		static ZEMDResource*							CreateInstance();
		static ZERSHolder<ZEMDResource>					CreateResource();
		static ZERSHolder<ZEMDResource>					LoadResource(const ZEString& FileName);
		static ZERSHolder<const ZEMDResource>			LoadResourceShared(const ZEString& FileName);
};
