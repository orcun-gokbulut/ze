//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTag.cpp
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

#include "ZEDTag.h"

#include "ZEGraphics/ZEUIMaterial.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEGraphics/ZESimpleMaterial.h"

void ZEDTag::SetIcon(const ZEUIMaterial* Material)
{
	Icon.SetMaterial((ZEMaterial*)Material);
}

const ZEUIMaterial* ZEDTag::GetIcon()
{
	return (ZEUIMaterial*)Icon.GetMaterial();
}

void ZEDTag::DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, const ZEVector4& Color, ZECanvas& Canvas)
{
	ZEMatrix4x4 LocalPivot, WorldMatrix;
	ZEMatrix4x4::CreateOrientation(LocalPivot, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3((BoundingBox.Max.x - BoundingBox.Min.x), 
		(BoundingBox.Max.y - BoundingBox.Min.y), 
		(BoundingBox.Max.z - BoundingBox.Min.z))
		);
	ZEMatrix4x4::Multiply(WorldMatrix, Transform, LocalPivot);

	Canvas.SetTransfomation(WorldMatrix);
	Canvas.SetColor(Color);
	Canvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDTag::DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, const ZEVector4& Color, ZECanvas& Canvas)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	Canvas.SetColor(Color);
	Canvas.SetTransfomation(WorldMatrix);
	Canvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDTag::DrawBoundingSphere(const ZEBSphere& BoundingSphere, const ZEVector4& Color, ZECanvas& Canvas)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingSphere.Position, 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingSphere.Radius, BoundingSphere.Radius, BoundingSphere.Radius));

	Canvas.SetTransfomation(WorldMatrix);
	Canvas.SetColor(Color);
	Canvas.AddWireframeSphere(1.0f, 8, 8);
}

void ZEDTag::DrawLineSegment(const ZEVector3& StartPosition, const ZEVector3& EndPosition, const ZEVector4& Color, ZECanvas& Canvas)
{
	Canvas.SetTransfomation(ZEMatrix4x4::Identity);
	Canvas.SetColor(Color);
	Canvas.AddLine(StartPosition, EndPosition);
}

void ZEDTag::DrawRectangle(const ZERectangle3D& Rectangle, const ZEVector4& Color, ZECanvas& Canvas)
{
	Canvas.SetTransfomation(ZEMatrix4x4::Identity);
	Canvas.SetColor(Color);
	Canvas.AddWireframeQuad(Rectangle.P1, Rectangle.P2, Rectangle.P3, Rectangle.P4);
}

bool ZEDTag::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (TagMaterial == NULL)
		TagMaterial = ZESimpleMaterial::CreateInstance();

	TagRenderCommand.SetZero();
	TagRenderCommand.Material = TagMaterial;
	TagRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	TagRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	TagRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	TagRenderCommand.VertexBuffer = &TagCanvas;
	TagRenderCommand.Priority = 10;

	return true;
}

bool ZEDTag::DeinitializeSelf()
{
	TagCanvas.Clean();
	TagRenderCommand.SetZero();

	if (TagMaterial != NULL)
	{
		TagMaterial->Release();
		TagMaterial = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

ZEDTag::ZEDTag()
{
	Selected = false;
	Object = NULL;
	Color = ZEVector4::Zero;

	TagMaterial = NULL;
	TagRenderCommand.SetZero();
}

ZEDTag::~ZEDTag()
{
	if (Object != NULL)
	{
		delete Object;
		Object = NULL;
	}
}

void ZEDTag::SetObject(ZEObject* Object)
{
	if (this->Object != NULL)
		return;

	this->Object = Object;
}

ZEObject* ZEDTag::GetObject()
{
	return Object;
}

void ZEDTag::SetSelected(bool Value)
{
	Selected = Value;
}

bool ZEDTag::GetSelected()
{
	return Selected;
}

void ZEDTag::SetDrawColor(const ZEVector4& Color)
{
	this->Color = Color;
}

const ZEVector4& ZEDTag::GetDrawColor()
{
	return Color;
}

void ZEDTag::SetPosition(const ZEVector3& NewPosition)
{
	ZEEntity::SetPosition(NewPosition);
}

void ZEDTag::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEEntity::SetRotation(NewRotation);
}

void ZEDTag::SetScale(const ZEVector3& NewScale)
{
	ZEEntity::SetScale(NewScale);
}

void ZEDTag::Destroy()
{
	delete this;
}

bool ZEDTag::Save(ZEMLWriterNode* Serializer)
{
	return true;
}

bool ZEDTag::Restore(ZEMLReaderNode* Unserializer)
{
	return true;
}

void ZEDTag::Tick(float Time)
{
	ZEEntity::Tick(Time);
}

void ZEDTag::Draw(ZEDrawParameters* DrawParameters)
{
	ZEEntity::Draw(DrawParameters);
}

ZEDTag* ZEDTag::CreateInstance()
{
	return new ZEDTag();
}
