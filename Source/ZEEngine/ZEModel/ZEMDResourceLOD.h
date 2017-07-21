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
		ZESize									VertexOffset;
		ZESize									VertexCount;

		ZEMDVertexIndexType						IndexType;
		ZESize									IndexOffset;
		ZESize									IndexCount;

		bool									FacingCamera;

		ZEArray<ZEMDResourceDraw>				Draws;
		
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

		void									SetVertexOffset(ZESize Offset);
		ZESize									GetVertexOffset() const;

		void									SetVertexCount(ZESize Count);
		ZESize									GetVertexCount() const;

		void									SetIndexType(ZEMDVertexIndexType IndexType);
		ZEMDVertexIndexType						GetIndexType() const;

		void									SetIndexOffset(ZESize Offset);
		ZESize									GetIndexOffset() const;

		void									SetIndexCount(ZESize Count);
		ZESize									GetIndexCount() const;

		const ZEArray<ZEMDResourceDraw>&		GetDraws() const;
		void									AddDraw(const ZEMDResourceDraw& Draw);
		void									RemoveDraw(ZESize Index);

		void									SetFacingCamera(bool FacingCamera);
		bool									GetFacingCamera() const;

		bool									Unserialize(const ZEMLReaderNode& LODNode);
		bool									Serialize(ZEMLWriterNode& LODNode) const;

		static ZEMDResourceLOD*					CreateInstance();
};
