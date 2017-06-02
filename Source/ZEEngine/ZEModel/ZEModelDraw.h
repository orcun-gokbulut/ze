//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelDraw.h
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

#include "ZEDS/ZEList2.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEExport.ZEEngine.h"

class ZEModel;
class ZEModelMesh;
class ZEModelMeshLOD;
class ZEModelDraw;
class ZEMDResourceDraw;
class ZEGRBuffer;
class ZERNPreRenderParameters;
class ZERNRenderParameters;

ZEMT_FORWARD_DECLARE(ZERNGeometry);
ZEMT_FORWARD_DECLARE(ZERNMaterial);

class ZE_EXPORT_ZEENGINE ZERNCommandDraw : public ZERNCommand
{
	ZE_OBJECT
	private:
		ZELink<ZERNCommandDraw>			InstanceLink;
		ZEList2<ZERNCommandDraw>		Instances;

	public:
		ZEHolder<const ZERNMaterial>	Material;
		ZEHolder<const ZERNGeometry>	Geometry;
		ZEHolder<const ZEGRBuffer>		TransformConstantBuffer;
		ZEHolder<const ZEGRBuffer>		DrawConstantBuffer;
		ZEHolder<const ZEGRBuffer>		BoneConstantBuffer;

		ZERNInstanceData				InstanceData;

		virtual bool					AddSubCommand(ZERNCommand* Command) override;
		virtual void					Reset() override;
		virtual void					Clear() override;
		virtual void					Execute(const ZERNRenderParameters* RenderParameters) override;

										ZERNCommandDraw();
};

class ZE_EXPORT_ZEENGINE ZEModelDraw : public ZEObject
{
	ZE_OBJECT
	friend class ZEModelMesh;
	friend class ZEModelMeshLOD;
	private:
		const ZEMDResourceDraw*						Resource;
		ZEModelMeshLOD*								LOD;
		ZEUInt32									VertexOffset;
		ZEUInt32									VertexCount;
		ZEUInt32									IndexOffset;
		ZEUInt32									IndexCount;
		ZEHolder<const ZERNGeometry>				Geometry;
		ZEHolder<const ZERNMaterial>				Material;
		ZERNCommandDraw								RenderCommand;
		ZERNCommandDraw								RenderCommandShadow;
		mutable bool								DirtyConstants;
		
		mutable ZEHolder<ZEGRBuffer>				ConstantBuffer;

		ZEVector3									Color;
		float										Opacity;

		mutable struct
		{
			ZEVector3								Color;
			float									Opacity;

			ZEVector3								Reserved;
			ZEBool32								LODTransition;
		} Constants;

		bool										Load(const ZEMDResourceDraw* Resource);
		bool										Unload();

	public:
		ZEModel*									GetModel();
		const ZEModel*								GetModel() const;

		ZEModelMesh*								GetMesh();
		const ZEModelMesh*							GetMesh() const;

		ZEModelMeshLOD*								GetLOD();
		const ZEModelMeshLOD*						GetLOD() const;

		void										SetVertexOffset(ZEUInt32 Offset);
		ZEUInt32									GetVertexOffset() const;

		void										SetVertexCount(ZEUInt32 Count);
		ZEUInt32									GetVertexCount() const;

		void										SetIndexOffset(ZEUInt32 Offset);
		ZEUInt32									GetIndexOffset() const;

		void										SetIndexCount(ZEUInt32 Count);
		ZEUInt32									GetIndexCount() const;

		void										SetColor(const ZEVector3& Color);
		const ZEVector3&							GetColor() const;

		void										SetOpacity(float Opacity);
		float										GetOpacity() const;

		void										SetMaterial(const ZERNMaterial* Material);
		const ZERNMaterial*							GetMaterial() const;
		void										ResetMaterial();

		void										SetGeometry(const ZERNGeometry* Geometry);
		const ZERNGeometry*							GetGeometry() const;

		const ZEGRBuffer*							GetConstantBuffer() const;

		bool										PreRender(const ZERNPreRenderParameters* PreRenderParameters);

													ZEModelDraw();
		virtual										~ZEModelDraw();
};
