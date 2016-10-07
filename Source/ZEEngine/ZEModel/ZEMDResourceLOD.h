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

#include "ZEMeta/ZEObject.h"
#include "ZEDestroyable.h"

#include "ZEMDVertex.h"
#include "ZEMDResourceDraw.h"

#include "ZEDS/ZELink.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"

class ZEMLReaderNode;
class ZEMLWriterNode;

ZE_META_FORWARD_DECLARE(ZERNMaterial, "ZERenderer/ZERNMaterial.h");
ZE_META_FORWARD_DECLARE(ZEGRBuffer, "ZERenderer/ZEGRBuffer.h");

class ZEMDResourceLOD : public ZEObject, public ZEDestroyable
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEMDResourceLOD)
	friend class ZEMDResourceMesh;
	friend class ZEMDResource;
	friend class ZEMDResourceDraw;
	private:
		ZEMDResourceMesh*						ResourceMesh;
		ZELink<ZEMDResourceLOD>					Link;

		ZEInt32									Level;
		float									StartDistance;
		float									EndDistance;

		ZEMDVertexType							VertexType;
		ZEArray<ZEMDVertex>						Vertices;
		ZEArray<ZEMDVertexSkin>					VerticesSkin;
		ZEArray<ZEMDResourceDraw>				Draws;
		
		ZEMDVertexIndexType						IndexType;
		ZEArray<ZEUInt16>						Indices;
		ZEArray<ZEUInt32>						Indices32;

		ZEHolder<const ZEGRBuffer>				VertexBuffer;
		ZEHolder<const ZEGRBuffer>				IndexBuffer;
		
												ZEMDResourceLOD();
		virtual									~ZEMDResourceLOD();

	public:
		ZEMDResource*							GetResource();
		const ZEMDResource*						GetResource() const;

		ZEMDResourceMesh*						GetResourceMesh();
		const ZEMDResourceMesh*					GetResourceMesh() const;

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

		void									SetVertexBuffer(ZEHolder<const ZEGRBuffer> VertexBuffer);
		ZEHolder<const ZEGRBuffer>				GetVertexBuffer() const;

		void									SetIndexBuffer(ZEHolder<const ZEGRBuffer> IndexBuffer);
		ZEHolder<const ZEGRBuffer>				GetIndexBuffer() const;

		const ZEArray<ZEMDResourceDraw>&		GetDraws() const;
		void									AddDraw(const ZEMDResourceDraw& Draw);
		void									RemoveDraw(ZESize Index);

		void									GenerateBuffers();

		bool									Unserialize(const ZEMLReaderNode& LODNode);
		bool									Serialize(ZEMLWriterNode& LODNode) const;

		static ZEMDResourceLOD*					CreateInstance();
};
