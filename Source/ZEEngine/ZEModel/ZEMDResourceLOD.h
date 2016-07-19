//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceLOD.h
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

#include "ZEMDVertex.h"
#include "ZEMDResourceDraw.h"

#include "ZEDS\ZELink.h"
#include "ZEDS\ZEArray.h"
#include "ZEPointer\ZEHolder.h"

class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEMLReaderNode;
class ZEMLWriterNode;

class ZEModelResourceMeshLOD : public ZEObject
{
	ZE_OBJECT
	friend class ZEMDResourceMesh;
	friend class ZEMDResource;
	private:
		ZELink<ZEModelResourceMeshLOD>			Link;

		ZEInt32									Level;
		float									StartDistance;
		float									EndDistance;
		ZEArray<ZEUInt16>						AffectingBoneIds;

		ZEMDVertexType							VertexType;
		ZEArray<ZEMDVertex>						Vertices;
		ZEArray<ZEMDVertexSkin>					VerticesSkin;
		ZEArray<ZEMDResourceDraw>				Draws;
		
		ZEMDVertexIndexType						IndexType;
		ZEArray<ZEUInt16>						Indices;
		ZEArray<ZEUInt32>						Indices32;

		ZEHolder<ZEGRVertexBuffer>				VertexBufferBase;
		ZEHolder<ZEGRVertexBuffer>				VertexBufferNormals;
		ZEHolder<ZEGRVertexBuffer>				VertexBufferSkin;
		ZEHolder<ZEGRVertexBuffer>				VertexBufferExtra;
		ZEHolder<ZEGRIndexBuffer>				IndexBuffer;

		ZEInt									MaterialID; // Backwards Compatibility
		ZEHolder<ZERNMaterial>					Material;

	public:
		void									SetLevel(ZEInt32 LODLevel);
		ZEInt32									GetLevel() const;

		void									SetStartDistance(float StartDistance);
		float									GetStartDistance() const;

		void									SetEndDistance(float EndDistance);
		float									GetEndDistance() const;

		void									SetVertexType(ZEMDVertexType VertexType);
		ZEMDVertexType							GetVertexType() const;

		void									SetIndexType(ZEMDVertexIndexType IndexType);
		ZEMDVertexIndexType						GetIndexType() const;

		void									SetVertices(const ZEArray<ZEMDVertex>& Vertices);
		const ZEArray<ZEMDVertex>&				GetVertices() const;

		void									SetVerticesSkin(const ZEArray<ZEMDVertexSkin>& VerticesSkin);
		const ZEArray<ZEMDVertexSkin>&			GetVerticesSkin() const;
	
		void									SetIndices(const ZEArray<ZEUInt16>& Indices);
		const ZEArray<ZEUInt16>&				GetIndices() const;

		void									SetIndices32(const ZEArray<ZEUInt32>& Indices);
		const ZEArray<ZEUInt32>&				GetIndices32() const;

		void									SetAffectingBoneIds(const ZEArray<ZEUInt16>& BoneIds);
		const ZEArray<ZEUInt16>&				GetAffectingBoneIds() const;

		void									SetVertexBufferBase(ZEHolder<ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<ZEGRVertexBuffer>				GetVertexBufferBase() const;

		void									SetVertexBufferNormals(ZEHolder<ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<ZEGRVertexBuffer>				GetVertexBufferNormals() const;

		void									SetVertexBufferSkin(ZEHolder<ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<ZEGRVertexBuffer>				GetVertexBufferSkin() const;

		void									SetVertexBufferExtra(ZEHolder<ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<ZEGRVertexBuffer>				GetVertexBufferExtra() const;

		void									SetIndexBuffer(ZEHolder<ZEGRIndexBuffer> IndexBuffer);
		ZEHolder<ZEGRIndexBuffer>				GetIndexBuffer() const;

		void									SetMaterial(ZEHolder<ZERNMaterial> Material);
		ZEHolder<const ZERNMaterial>			GetMaterial() const;

		void									SetMaterialFileName(const ZEString& MaterialFilePath);
		const ZEString&							GetMaterialFileName();

		const ZEArray<ZEMDResourceDraw>&		GetDraws() const;
		void									AddDraw(const ZEMDResourceDraw& Draw);
		void									RemoveDraw(ZESize Index);

		void									GenerateBuffers();

		bool									Load(const ZEMLReaderNode& LODNode);
		bool									Save(ZEMLWriterNode& LODNode) const;

												ZEModelResourceMeshLOD();
												~ZEModelResourceMeshLOD();
};
