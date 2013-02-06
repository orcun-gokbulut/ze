//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESceneDebugDraw.cpp
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

#include "ZESceneDebugDraw.h"
#include "ZERenderer/ZESimpleMaterial.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEOBBox.h"

void ZESceneDebugDraw::DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, ZERenderer* Renderer, const ZEVector4& Color)
{
	ZEMatrix4x4 LocalPivot, WorldMatrix;
	ZEMatrix4x4::CreateOrientation(LocalPivot, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3((BoundingBox.Max.x - BoundingBox.Min.x), 
			(BoundingBox.Max.y - BoundingBox.Min.y), 
			(BoundingBox.Max.z - BoundingBox.Min.z))
			);
	ZEMatrix4x4::Multiply(WorldMatrix, Transform, LocalPivot);

	VertexBuffer.SetTransfomation(WorldMatrix);
	VertexBuffer.SetColor(Color);
	VertexBuffer.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZESceneDebugDraw::DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, ZERenderer* Renderer, const ZEVector4& Color)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	VertexBuffer.SetColor(Color);
	VertexBuffer.SetTransfomation(WorldMatrix);
	VertexBuffer.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZESceneDebugDraw::DrawBoundingSphere(const ZEBSphere& BoundingSphere, ZERenderer* Renderer, const ZEVector4& Color)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingSphere.Position, 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingSphere.Radius, BoundingSphere.Radius, BoundingSphere.Radius));
	VertexBuffer.SetTransfomation(WorldMatrix);
	VertexBuffer.SetColor(Color);
	VertexBuffer.AddWireframeSphere(1.0f, 8, 8);
}

void ZESceneDebugDraw::Clean()
{
	VertexBuffer.Clean();
}

bool ZESceneDebugDraw::Initialize()
{
	/*
	if (Initialized)
		return false;

	if (Material == NULL)
		Material = ZESimpleMaterial::CreateInstance();

	RenderCommand.SetZero();
	RenderCommand.Material = Material;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	//RenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_LINE_LIST;
	RenderCommand.VertexBuffer = &VertexBuffer;
	
	Initialized = true;
	*/
	return true;
}

void ZESceneDebugDraw::Deinitialize()
{
	if (!Initialized)
		return;

	VertexBuffer.Clean();
	RenderCommand.SetZero();
	if (Material != NULL)
	{
		Material->Destroy();
		Material = NULL;
	}

	Initialized = false;
}

void ZESceneDebugDraw::Draw(ZERenderer* Renderer)
{
	if (VertexBuffer.Vertices.GetCount() == 0)
		return;

	RenderCommand.PrimitiveCount = VertexBuffer.Vertices.GetCount() / 2;
	Renderer->AddToRenderList(&RenderCommand);
}

ZESceneDebugDraw::ZESceneDebugDraw()
{
	Material = NULL;
	Initialized = false;
	RenderCommand.SetZero();
}

ZESceneDebugDraw::~ZESceneDebugDraw()
{
	Deinitialize();
}





