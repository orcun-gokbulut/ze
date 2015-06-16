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

#include "ZEModelDebugDrawer.h"
#include "ZEModel.h"

#include "ZERenderer/ZESimpleMaterial.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"


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

	if (ZEClass::IsDerivedFrom(ZEModel::Class(), Target->GetClass()))
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
	DrawParameters->Renderer->AddToRenderList(&ModelRenderCommand);
}

bool ZEModelDebugDrawer::InitializeSelf()
{
	if (!ZEDebugDrawer::InitializeSelf())
		return false;

	if (ModelMaterial == NULL)
		ModelMaterial = ZESimpleMaterial::CreateInstance();

	ModelRenderCommand.SetZero();
	ModelRenderCommand.Material = ModelMaterial;
	ModelRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	ModelRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	ModelRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	ModelRenderCommand.VertexBuffer = &ModelDrawCanvas;
	ModelRenderCommand.Priority = 10;

	return true;
}

bool ZEModelDebugDrawer::DeinitializeSelf()
{
	ModelDrawCanvas.Clean();
	ModelRenderCommand.SetZero();

	if (ModelMaterial != NULL)
	{
		ModelMaterial->Release();
		ModelMaterial = NULL;
	}

	return ZEDebugDrawer::DeinitializeSelf();
}

ZEModelDebugDrawer::ZEModelDebugDrawer()
{
	MeshBoundingBoxColor = ZEVector4::Zero;
	BonePositionColor = ZEVector4::Zero;
	BonePathColor = ZEVector4::Zero;
	ModelMaterial = NULL;
	ModelRenderCommand.SetZero();
}

ZEModelDebugDrawer::~ZEModelDebugDrawer()
{

}

ZEModelDebugDrawer* ZEModelDebugDrawer::CreateInstance()
{
	return new ZEModelDebugDrawer();
}
