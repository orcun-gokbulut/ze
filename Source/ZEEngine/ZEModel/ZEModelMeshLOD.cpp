//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMeshLOD.cpp
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

#include "ZEModelMeshLOD.h"

#include "ZEModel.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNShaderSlots.h"

void ZEModelMeshLOD::SetMaterial(ZERNMaterial* Material)
{
	this->Material = Material;
}

ZERNMaterial* ZEModelMeshLOD::GetMaterial()
{
	return Material;
}

const ZEModelResourceMeshLOD* ZEModelMeshLOD::GetLODResource()
{
	return LODResource;
}

ZEInt32 ZEModelMeshLOD::GetDrawStartDistance()
{
	return DrawStartDistance;
}

ZEInt32 ZEModelMeshLOD::GetDrawEndDistance()
{
	return DrawEndDistance;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	if (VertexBuffer.IsNull())
		return;

	ZEGRContext* Context = RenderParameters->Context;
	Material->SetupMaterial(Context, RenderParameters->Stage);

	//Context->SetIndexBuffer(IndexBuffer);
	if (RenderParameters->Type == ZERN_DT_SHADOW)
	{
		Context->SetVertexBuffers(0, 1, &VertexBuffer);
		if (Skinned)
			Context->SetVertexBuffers(1, 1, &VertexBufferSkin);
	}
	else
	{
		Context->SetVertexBuffers(0, 1, &VertexBuffer);
		//Context->SetVertexBuffer(1, VertexBufferNormals);
		if (Skinned)
			Context->SetVertexBuffers(2, 1, &VertexBufferSkin);
	}

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->Draw((ZEUInt)LODResource->VertexCount, 0);
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;
	DrawStartDistance = LODResource->LODStartDistance;
	DrawEndDistance = LODResource->LODEndDistance;
	Material = Model->GetModelResource()->GetMaterials().GetFirstItem();
	VertexBuffer = LODResource->VertexBuffer;
	VertexBufferNormals = LODResource->VertexBufferNormals;
	VertexBufferSkin = LODResource->VertexBufferSkin;
	IndexBuffer = LODResource->IndexBuffer;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));
	
	ConstantBuffer->SetData((void*)&Model->GetTransform());

	Skinned = LODResource->VertexBufferSkin.IsNull() ? false : true;
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
	Skinned = false;
}

ZEModelMeshLOD::ZEModelMeshLOD()
{
	Skinned = false;
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}
