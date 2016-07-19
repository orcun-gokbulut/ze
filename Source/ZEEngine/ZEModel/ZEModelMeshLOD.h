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

#include "ZEMDVertex.h"

#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEResource/ZERSHolder.h"

class ZEModel;
class ZEModelMesh;
class ZEMDResource;
class ZEModelResourceMeshLOD;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZERNMaterial;
class ZERNRenderer;
class ZERNRenderParameters;
class ZEGRConstantBuffer;

class ZEModelMeshLOD;

class ZEModelDraw : public ZEObject
{
	ZE_OBJECT
	friend class ZEModelMesh;
	friend class ZEModelMeshLOD;
	private:
		ZEModelMeshLOD*						LOD;
		ZESize								Offset;
		ZESize								Count;
		ZEHolder<const ZERNMaterial>		Material;
		mutable ZERNCommand					RenderCommand;

	public:
		ZEModelMeshLOD*						GetLOD();
		const ZEModelMeshLOD*				GetLOD() const;

		void								SetOffset(ZESize Offset);
		ZESize								GetOffset() const;

		void								SetCount(ZESize Count);
		ZESize								GetCount() const;

		void								SetMaterial(ZEHolder<const ZERNMaterial> Material);
		ZEHolder<const ZERNMaterial>		GetMaterial() const;

		void								Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

											ZEModelDraw();
};

class ZEModelMeshLOD : public ZEObject
{
	ZE_OBJECT
	friend class ZEModelMesh;
	friend class ZEModelDraw;
	private:
		ZEModel*							Model;
		ZEModelMesh*						Mesh;
		ZELink<ZEModelMeshLOD>				MeshLink;

		float								StartDistance;
		float								EndDistance;

		ZEMDVertexType						VertexType;
		ZEHolder<const ZEGRVertexBuffer>	VertexBufferBase;
		ZEHolder<const ZEGRVertexBuffer>	VertexBufferNormals;
		ZEHolder<const ZEGRVertexBuffer>	VertexBufferSkin;

		ZEMDVertexIndexType					IndexType;
		ZEHolder<const ZEGRIndexBuffer>		IndexBuffer;

		ZEArray<ZEModelDraw>				Draws;

		const ZEModelResourceMeshLOD*		LODResource;

	public:
		ZEModel*							GetModel();
		ZEModelMesh*						GetMesh();

		void								SetVertexType(ZEMDVertexType Type);
		ZEMDVertexType						GetVertexType() const;

		void								SetVertexBufferBase(ZEHolder<const ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<const ZEGRVertexBuffer>	GetVertexBufferBase() const;

		void								SetVertexBufferNormals(ZEHolder<const ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<const ZEGRVertexBuffer>	GetVertexBufferNormals() const;

		void								SetVertexBufferSkin(ZEHolder<const ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<const ZEGRVertexBuffer>	GetVertexBufferSkin() const;

		void								SetVertexBufferExtra(ZEHolder<const ZEGRVertexBuffer> VertexBuffer);
		ZEHolder<const ZEGRVertexBuffer>	GetVertexBufferExtra() const;

		void								SetIndexType(ZEMDVertexIndexType Type);
		ZEMDVertexIndexType					GetIndexType() const;

		void								SetIndexBuffer(ZEHolder<const ZEGRIndexBuffer> IndexBuffer);
		ZEHolder<const ZEGRIndexBuffer>		GetIndexBuffer() const;

		void								SetStartDistance(float Distance);
		float								GetStartDistance() const;

		void								SetEndDistance(float Distance);
		float								GetEndDistance() const;

		const ZEArray<ZEModelDraw>&			GetDraws();
		void								AddDraw(const ZEModelDraw& Draw);
		void								RemoveDraw(ZESize Index);

		void								Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);

		void								Load(ZEModel* Model, ZERSHolder<const ZEMDResource> ModelResource, const ZEModelResourceMeshLOD* Resource);
		void								Unload();

											ZEModelMeshLOD();
											~ZEModelMeshLOD();
};
