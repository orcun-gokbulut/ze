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
#ifndef	__ZE_MODEL_MESH_LOD_H__
#define __ZE_MODEL_MESH_LOD_H__

#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZECanvas.h"

class ZEModel;
class ZEMaterial;
class ZEModelMesh;
class ZEModelResourceMeshLOD;
struct ZEDrawParameters;
class ZESimpleMaterial;

class ZEModelMeshLOD
{
	private:
		ZEModel*							Owner;
		ZEModelMesh*						OwnerMesh;
		const ZEModelResourceMeshLOD*		LODResource;
		ZEVertexBuffer*						VertexBuffer;
		ZERenderOrder						RenderOrder;
		const ZEMaterial*					Material;
		bool								Skinned;

		ZESimpleMaterial*					NormalMaterial;
		ZECanvas							NormalCanvas;
		ZERenderOrder						NormalRenderOrder;

	public:
		void								ResetMaterial();
		void								SetMaterial(const ZEMaterial* Material);
		const ZEMaterial*					GetMaterial();

		bool								IsSkinned();

		void								Draw(ZEDrawParameters* DrawParameters);
		
		void								Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource);
		void								Deinitialize();

											ZEModelMeshLOD();
											~ZEModelMeshLOD();
};
#endif



