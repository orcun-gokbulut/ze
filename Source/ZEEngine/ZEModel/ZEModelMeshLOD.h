//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMeshLOD.h
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

#include "ZEModelDraw.h"
#include "ZEMDVertex.h"

#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEResource/ZERSHolder.h"

class ZEModel;
class ZEModelMesh;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRConstantBuffer;
class ZERNMaterial;
class ZERNRenderer;
class ZERNRenderParameters;

ZE_META_FORWARD_DECLARE(ZEMDResourceLOD, "ZEMDResourceLOD.h")

class ZEModelMeshLOD : public ZEObject, public ZEDestroyable
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEModelMeshLOD)
	friend class ZEModelMesh;
	friend class ZEModelDraw;
	private:
		ZEModelMesh*								Mesh;
		ZELink<ZEModelMeshLOD>						MeshLink;

		float										StartDistance;
		float										EndDistance;

		ZEMDVertexType								VertexType;
		ZEHolder<const ZEGRVertexBuffer>			VertexBuffer;

		ZEMDVertexIndexType							IndexType;
		ZEHolder<const ZEGRIndexBuffer>				IndexBuffer;

		ZEArray<ZEModelDraw>						Draws;

		bool										Load(const ZEMDResourceLOD* Resource);
		bool										Unload();
				
													ZEModelMeshLOD();
		virtual										~ZEModelMeshLOD();

	public:
		ZEModel*									GetModel();
		ZEModelMesh*								GetMesh();

		const ZEArray<ZEModelDraw>&					GetDraws();
		void										AddDraw(const ZEModelDraw& Draw);
		void										RemoveDraw(ZESize Index);

		void										SetVertexType(ZEMDVertexType Type);
		ZEMDVertexType								GetVertexType() const;

		void										SetVertexBuffer(ZEHolder<const ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<const ZEGRVertexBuffer>			GetVertexBuffer() const;

		void										SetIndexType(ZEMDVertexIndexType Type);
		ZEMDVertexIndexType							GetIndexType() const;

		void										SetIndexBuffer(ZEHolder<const ZEGRIndexBuffer> IndexBuffer);
		ZEHolder<const ZEGRIndexBuffer>				GetIndexBuffer() const;

		void										SetStartDistance(float Distance);
		float										GetStartDistance() const;

		void										SetEndDistance(float Distance);
		float										GetEndDistance() const;

		void										SetResource(ZERSHolder<const ZEMDResourceLOD> Resource);
		ZERSHolder<const ZEMDResourceLOD>			GetResource();

		void										Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);

		static ZEModelMeshLOD*						CreateInstance();
};
