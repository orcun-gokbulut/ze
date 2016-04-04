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

#include "ZERenderer/ZERNCommand.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEMatrix.h"

class ZEModel;
class ZEModelMesh;
class ZEModelResourceMeshLOD;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZERNMaterial;
class ZERNRenderer;
class ZERNRenderParameters;
class ZEGRConstantBuffer;

class ZEModelMeshLOD
{
	private:
		ZEModel*							Owner;
		ZEModelMesh*						OwnerMesh;
		ZEInt32								DrawStartDistance;
		ZEInt32								DrawEndDistance;
		const ZEModelResourceMeshLOD*		LODResource;
		
		ZEHolder<ZEGRIndexBuffer>			IndexBuffer;
		ZEHolder<ZEGRVertexBuffer>			VertexBuffer;
		ZEHolder<ZEGRVertexBuffer>			VertexBufferNormals;
		ZEHolder<ZEGRVertexBuffer>			VertexBufferSkin;
		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>		ConstantBufferSkin;

		ZEHolder<const ZERNMaterial>		Material;
		bool								Skinned;
		ZEArray<ZEMatrix4x4>				BoneTransforms;

	public:
		bool								IsSkinned() const;

		void								SetMaterial(ZEHolder<const ZERNMaterial> Material);
		ZEHolder<const ZERNMaterial>		GetMaterial() const;

		const ZEModelResourceMeshLOD*		GetLODResource() const;

		ZEInt32								GetDrawStartDistance() const;
		ZEInt32								GetDrawEndDistance() const;

		void								Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource);
		void								Deinitialize();

		void								ResetMaterial();

		void								Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);

											ZEModelMeshLOD();
											~ZEModelMeshLOD();
};
