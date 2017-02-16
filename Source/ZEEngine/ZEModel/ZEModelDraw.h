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

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZERNInstanceTag.h"

class ZEModel;
class ZEModelMesh;
class ZEModelMeshLOD;
class ZEModelDraw;
class ZEMDResourceDraw;
class ZEGRBuffer;

ZEMT_FORWARD_DECLARE(ZERNMaterial);

class ZERNCommandDraw : public ZERNCommand
{
	ZE_OBJECT
	public:
		ZEHolder<ZEGRBuffer>	VertexBuffer;
		ZEHolder<ZEGRBuffer>	IndexBuffer;
		ZEHolder<ZERNMaterial>	Material;

		ZEVector3				Position;
		ZEQuaternion			Rotation;
		ZEVector3				Scale;

		ZEVector4				Color;
		bool					LODTransition;
		bool					Instanced;
		bool					DirtyTransform;
};

class ZEMDInstanceTag : public ZERNInstanceTag
{
	ZE_OBJECT
	friend class ZEModelDraw;
	private:
		ZEModelDraw*								Draw;
		bool										Dirty;

		bool										Update();

	public:
		virtual bool								Check(const ZERNInstanceTag* Other) const;
};

class ZEModelDraw : public ZEObject
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
		ZEHolder<const ZERNMaterial>				Material;
		mutable ZERNCommand							RenderCommand;
		mutable ZEMDInstanceTag						InstanceTag;
		bool										DirtyConstants;
		
		ZEHolder<ZEGRBuffer>						ConstantBuffer;

		struct
		{
			ZEVector3								Color;
			float									Opacity;

			ZEVector3								Reserved;
			ZEBool32								LODTransition;
		} Constants;

		bool										Load(const ZEMDResourceDraw* Resource);
		bool										Unload();

		const ZEMDInstanceTag*						GetInstanceTag() const;

		void										SetLOD(ZEModelMeshLOD* LOD);

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

		void										SetLODTransition(bool LODTransition);
		bool										GetLODTransition() const;

		void										SetMaterial(const ZERNMaterial* Material);
		const ZERNMaterial*							GetMaterial() const;
		void										ResetMaterial();

		void										Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

													ZEModelDraw();
		virtual										~ZEModelDraw();
};
