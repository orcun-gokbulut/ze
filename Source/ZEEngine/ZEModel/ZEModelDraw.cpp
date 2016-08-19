//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelDraw.cpp
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

#include "ZEModelDraw.h"

#include "ZEModelMesh.h"
#include "ZEModelMeshLOD.h"
#include "ZEMDResourceDraw.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRIndexBuffer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"


ZEModelMeshLOD* ZEModelDraw::GetLOD()
{
	return LOD;
}

const ZEModelMeshLOD* ZEModelDraw::GetLOD() const
{
	return LOD;
}

void ZEModelDraw::SetOffset(ZESize Offset)
{
	this->Offset = Offset;
}

ZESize ZEModelDraw::GetOffset() const
{
	return Offset;
}

void ZEModelDraw::SetCount(ZESize Count)
{
	this->Count = Count;
}

ZESize ZEModelDraw::GetCount() const
{
	return Count;
}

void ZEModelDraw::SetMaterial(ZEHolder<const ZERNMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<const ZERNMaterial> ZEModelDraw::GetMaterial() const
{
	return Material;
}

void ZEModelDraw::ResetMaterial()
{
	SetMaterial(Resource->GetMaterial());
}

void ZEModelDraw::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (GetLOD() == NULL)
		return;

	if (GetLOD()->GetMesh() == NULL)
		return;

	if (GetLOD()->VertexBuffer.IsNull())
		return;

	if (GetLOD()->GetIndexType() != ZEMD_VIT_NONE && GetLOD()->IndexBuffer.IsNull())
		return;

	ZEGRContext* Context = Parameters->Context;
	Context->SetVertexBuffer(0, GetLOD()->VertexBuffer);
	
	if (GetLOD()->GetIndexType() != ZEMD_VIT_NONE)
		Context->SetIndexBuffer(GetLOD()->GetIndexBuffer());

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, GetLOD()->GetMesh()->ConstantBuffer);

	if (!GetMaterial()->SetupMaterial(Parameters->Context, Parameters->Stage))
		return;

	if (GetLOD()->GetIndexType() == ZEMD_VIT_NONE)
		Context->Draw(GetCount(), GetOffset());
	else
		Context->DrawIndexed(GetCount(), GetOffset(), 0);

	GetMaterial()->CleanupMaterial(Parameters->Context, Parameters->Stage);
}


ZEModelDraw::ZEModelDraw()
{
	LOD = NULL;
	Offset = 0;
	Count = 0;
	Resource = NULL;
	RenderCommand.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEModelDraw, Render, this);
}
