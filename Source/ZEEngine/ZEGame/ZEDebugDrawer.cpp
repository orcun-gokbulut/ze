//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDebugDrawer.cpp
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

#include "ZEDebugDrawer.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics/ZELight.h"

void ZEDebugDrawer::DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, const ZEVector4& Color)
{
	ZEMatrix4x4 LocalPivot, WorldMatrix;
	ZEMatrix4x4::CreateOrientation(LocalPivot, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3((BoundingBox.Max.x - BoundingBox.Min.x), 
		(BoundingBox.Max.y - BoundingBox.Min.y), 
		(BoundingBox.Max.z - BoundingBox.Min.z))
		);
	ZEMatrix4x4::Multiply(WorldMatrix, Transform, LocalPivot);

	DrawCanvas.SetTransfomation(WorldMatrix);
	DrawCanvas.SetColor(Color);
	DrawCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDebugDrawer::DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, const ZEVector4& Color)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	DrawCanvas.SetColor(Color);
	DrawCanvas.SetTransfomation(WorldMatrix);
	DrawCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDebugDrawer::DrawBoundingSphere(const ZEBSphere& BoundingSphere, const ZEVector4& Color)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingSphere.Position, 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingSphere.Radius, BoundingSphere.Radius, BoundingSphere.Radius));

	DrawCanvas.SetTransfomation(WorldMatrix);
	DrawCanvas.SetColor(Color);
	DrawCanvas.AddWireframeSphere(1.0f, 8, 8);
}

void ZEDebugDrawer::DebugDrawEntity(ZEEntity* Entity)
{
	if (ZEObjectDescription::CheckParent(ZELight::Description(), Entity->GetDescription()))
	{
		if (((ZELight*)Entity)->GetLightType() != ZE_LT_DIRECTIONAL)
			DrawBoundingSphere(ZEBSphere(Entity->GetWorldPosition(), ((ZELight*)Entity)->GetRange()), ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));
	}
	else
	{
		DrawAxisAlignedBoundingBox(Entity->GetWorldBoundingBox(), ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));
	}

	const ZEArray<ZEEntity*>& Components = Entity->GetComponents();

	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		DebugDrawEntity(Components[I]);
	}

	const ZEArray<ZEEntity*>& ChildEntities = Entity->GetChildEntities();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
	{
		DebugDrawEntity(ChildEntities[I]);
	}
	
}

void ZEDebugDrawer::SetTarget(ZEEntity* Target)
{
	this->Target = Target;
}

ZEEntity* ZEDebugDrawer::GetTarget()
{
	return Target;
}

ZEDrawFlags ZEDebugDrawer::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEDebugDrawer::Clean()
{
	DrawCanvas.Clean();
}

void ZEDebugDrawer::Tick(float ElapsedTime)
{
	Clean();

	if (Target != NULL)
		DebugDrawEntity(Target);


}

void ZEDebugDrawer::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawCanvas.Vertices.GetCount() == 0)
		return;

	RenderCommand.PrimitiveCount = DrawCanvas.Vertices.GetCount() / 2;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

bool ZEDebugDrawer::Initialize()
{
	if (IsInitialized())
		return false;

	if (Material == NULL)
		Material = ZESimpleMaterial::CreateInstance();

	RenderCommand.SetZero();
	RenderCommand.Material = Material;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_LINE;
	RenderCommand.VertexBuffer = &DrawCanvas;

	return ZEEntity::Initialize();
}

void ZEDebugDrawer::Deinitialize()
{
	if (!IsInitialized())
		return;

	Target = NULL;
	DrawCanvas.Clean();
	RenderCommand.SetZero();

	if (Material != NULL)
	{
		Material->Release();
		Material = NULL;
	}

	ZEEntity::Deinitialize();
}

void ZEDebugDrawer::Destroy()
{
	delete this;
}

ZEDebugDrawer::ZEDebugDrawer()
{
	Target = NULL;
	Material = NULL;
	RenderCommand.SetZero();
}

ZEDebugDrawer::~ZEDebugDrawer()
{
	Deinitialize();
}

ZEDebugDrawer* ZEDebugDrawer::CreateInstance()
{
	return new ZEDebugDrawer(); 
}

