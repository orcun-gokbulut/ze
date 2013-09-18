//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelDebugDrawer.cpp
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

#include "ZEModel.h"
#include "ZEModelDebugDrawer.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZERenderer/ZEMaterialSimple.h"
#include "ZEGraphics/ZEGraphicsModule.h"

void ZEModelDebugDrawer::DebugDrawEntity()
{
	ZEDebugDrawer::DebugDrawEntity();

	ZEModel* ModelTarget = (ZEModel*)Target;

	for (ZESize I = 0; I < ModelTarget->Bones.GetCount(); I++)
	{
		DrawBoundingSphere(ZEBSphere(ModelTarget->Bones[I].GetWorldPosition(), 0.02f), BonePositionColor, ModelDrawCanvas);

		if (ModelTarget->Bones[I].GetParentBone() != NULL)
			DrawLineSegment(ModelTarget->Bones[I].GetWorldPosition(), ModelTarget->Bones[I].GetParentBone()->GetWorldPosition(), BonePathColor, ModelDrawCanvas);
	}

	for (ZESize I = 0; I < ModelTarget->Meshes.GetCount(); I++)
		DrawAxisAlignedBoundingBox(ModelTarget->Meshes[I].GetWorldBoundingBox(), MeshBoundingBoxColor, ModelDrawCanvas);

}

void ZEModelDebugDrawer::SetTarget(ZEEntity* Target)
{
	if (Target == NULL)
		return;

	if (ZEObjectDescription::CheckParent(ZEModel::Description(), Target->GetDescription()) || (ZEModel::Description() == Target->GetDescription()))
	{
		if (MeshBoundingBoxColor == ZEVector4::Zero)
			SetMeshBoundingBoxColor(ZEVector4(0.25f, 1.0f, 0.25f, 1.0f));

		if (BonePathColor == ZEVector4::Zero)
			SetBonePathColor(ZEVector4(0.0f, 1.0f, 1.0f, 1.0f));

		if (BonePositionColor == ZEVector4::Zero)
			SetBonePositionColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));

		ZEDebugDrawer::SetTarget(Target);

	}
}

void ZEModelDebugDrawer::SetMeshBoundingBoxColor(ZEVector4 Color)
{
	MeshBoundingBoxColor = Color;
}

ZEVector4 ZEModelDebugDrawer::GetMeshBoundingBoxColor() const
{
	return MeshBoundingBoxColor;
}

void ZEModelDebugDrawer::SetBonePositionColor(ZEVector4 Color)
{
	BonePositionColor = Color;
}

ZEVector4 ZEModelDebugDrawer::GetBonePositionColor() const
{
	return BonePositionColor;
}

void ZEModelDebugDrawer::SetBonePathColor(ZEVector4 Color)
{
	BonePathColor = Color;
}

ZEVector4 ZEModelDebugDrawer::GetBonePathColor() const
{
	return BonePathColor;
}

void ZEModelDebugDrawer::Draw(ZEDrawParameters* DrawParameters)
{
	ModelDrawCanvas.Clean();

	ZEDebugDrawer::Draw(DrawParameters);

	if (ModelDrawCanvas.Vertices.GetCount() == 0)
		return;

	ModelRenderCommand.PrimitiveCount = ModelDrawCanvas.Vertices.GetCount() / 2;
	//DrawParameters->Renderer->AddRenderCommand(&ModelRenderCommand);
}

bool ZEModelDebugDrawer::InitializeSelf()
{
	if (!ZEDebugDrawer::InitializeSelf())
		return false;

	if (ModelMaterial == NULL)
		ModelMaterial = ZEMaterialSimple::CreateInstance();

	ModelRenderCommand.Material = ModelMaterial;
//	ModelRenderCommand.Flags = ZE_RCF_VIEW_PROJECTION_TRANSFORM;
	ModelRenderCommand.VertexLayout = NULL;
	ModelRenderCommand.PrimitiveType = ZE_PT_LINE_LIST;
	// ModelRenderCommand.VertexBuffer = &ModelDrawCanvas;
	ModelRenderCommand.Priority = 10;

	return true;
}

bool ZEModelDebugDrawer::DeinitializeSelf()
{
	ModelDrawCanvas.Clean();

	ZE_DESTROY(ModelMaterial);

	return ZEDebugDrawer::DeinitializeSelf();
}

ZEModelDebugDrawer::ZEModelDebugDrawer()
{
	MeshBoundingBoxColor = ZEVector4::Zero;
	BonePositionColor = ZEVector4::Zero;
	BonePathColor = ZEVector4::Zero;
	ModelMaterial = NULL;
}

ZEModelDebugDrawer::~ZEModelDebugDrawer()
{

}

ZEModelDebugDrawer* ZEModelDebugDrawer::CreateInstance()
{
	return new ZEModelDebugDrawer();
}
