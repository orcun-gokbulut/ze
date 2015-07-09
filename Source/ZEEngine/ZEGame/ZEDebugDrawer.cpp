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
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEDrawParameters.h"
#include "ZERenderer/ZELight.h"
#include "ZEUI/ZEUIDebugDrawTag.h"
#include "ZEGame.h"
#include "ZEScene.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZERenderer/ZECamera.h"
#include "ZERandom.h"
#include "ZEMath/ZERectangle3D.h"

void ZEDebugDrawer::DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, const ZEVector4& Color, ZECanvas& Canvas)
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

void ZEDebugDrawer::DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, const ZEVector4& Color, ZECanvas& Canvas)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	Canvas.SetColor(Color);
	Canvas.SetTransfomation(WorldMatrix);
	Canvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDebugDrawer::DrawBoundingSphere(const ZEBSphere& BoundingSphere, const ZEVector4& Color, ZECanvas& Canvas)
{
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingSphere.Position, 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingSphere.Radius, BoundingSphere.Radius, BoundingSphere.Radius));

	Canvas.SetTransfomation(WorldMatrix);
	Canvas.SetColor(Color);
	Canvas.AddWireframeSphere(1.0f, 8, 8);
}

void ZEDebugDrawer::DrawLineSegment(const ZEVector3& StartPosition, const ZEVector3& EndPosition, const ZEVector4& Color, ZECanvas& Canvas)
{
	Canvas.SetTransfomation(ZEMatrix4x4::Identity);
	Canvas.SetColor(Color);
	Canvas.AddLine(StartPosition, EndPosition);
}

void ZEDebugDrawer::DrawRectangle(const ZERectangle3D& Rectangle, const ZEVector4& Color, ZECanvas& Canvas)
{
	Canvas.SetTransfomation(ZEMatrix4x4::Identity);
	Canvas.SetColor(Color);
	Canvas.AddWireframeQuad(Rectangle.P1, Rectangle.P2, Rectangle.P3, Rectangle.P4);
}

void ZEDebugDrawer::DebugDrawEntity()
{
	if (ZEClass::IsDerivedFrom(ZELight::Class(), Target->GetClass()))
	{
		if (((ZELight*)Target)->GetLightType() != ZE_LT_DIRECTIONAL && DebugDrawElements.GetFlags(ZE_DDE_LIGHT_RANGE))
		{
			DrawBoundingSphere(ZEBSphere(Target->GetWorldPosition(), ((ZELight*)Target)->GetRange()), DrawColor, DrawCanvas);
			EntityTag->SetVisiblity(true);
		}
		else
		{
			EntityTag->SetVisiblity(false);
		}
	}
	else
	{
		if (DebugDrawElements.GetFlags(ZE_DDE_ENTITY_WORLD_BOUNDING_BOX))
		{
			DrawAxisAlignedBoundingBox(Target->GetWorldBoundingBox(), DrawColor, DrawCanvas);
			EntityTag->SetVisiblity(true);
		}
		else
		{
			EntityTag->SetVisiblity(false);
		}
	}	
}

void ZEDebugDrawer::SetTarget(ZEEntity* Target)
{
	if (Target == NULL)
		return;

	this->Target = Target;
	Target->AddChildEntity(this);

	if (ZEClass::IsDerivedFrom(ZELight::Class(), Target->GetClass()))
	{
		if (DrawColor == ZEVector4::Zero)
			SetDebugDrawColor(ZEVector4(1.0f, 0.96f, 0.8f, 1.0f));
	}
	else
	{
		if (DrawColor == ZEVector4::Zero)
			SetDebugDrawColor(ZEVector4(0.8f, 0.8f, 0.8f, 1.0f));
	}

}

ZEEntity* ZEDebugDrawer::GetTarget()
{
	return Target;
}

ZEDrawFlags ZEDebugDrawer::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEDebugDrawer::SetDebugDrawColor(ZEVector4 Color)
{
	DrawColor = Color;

	if (IsInitialized())
		EntityTag->SetFontColor(DrawColor);
}

ZEVector4 ZEDebugDrawer::GetDebugDrawColor() const
{
	return DrawColor;
}

void ZEDebugDrawer::SetDebugDrawElements(ZEUInt Elements)
{
	DebugDrawElements.UnraiseFlags(ZE_DDE_ALL);
	DebugDrawElements.RaiseFlags(Elements);
}

ZEUInt ZEDebugDrawer::GetDebugDrawElements() const
{
	return DebugDrawElements.Value;
}

void ZEDebugDrawer::Draw(ZEDrawParameters* DrawParameters)
{
	DrawCanvas.Clean();

	if (Target != NULL)
	{
		DebugDrawEntity();
		EntityTag->SetNameField(Target->GetName());
		EntityTag->SetTypeField(Target->GetClass()->GetName());

		ZEInt32 WindowWidth, WindowHeight;
		zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);

		ZECamera* Camera = zeScene->GetActiveCamera();

		ZEVector4 LabelPositionInViewProj;
		ZEMatrix4x4::Transform(LabelPositionInViewProj, Camera->GetViewProjectionTransform(), ZEVector4(Target->GetWorldPosition(), 1.0f));

		ZEVector4 LabelPositionInScreen;
		ZEVector4::Divide(LabelPositionInScreen, LabelPositionInViewProj, LabelPositionInViewProj.w);

		if (LabelPositionInViewProj.z < 0.0f)
			EntityTag->SetVisiblity(false);

		if (EntityTag->GetVisiblity())
		{
			LabelPositionInScreen.x = (ZEInt32)(((LabelPositionInScreen.x + 1.0f) / 2.0f) * WindowWidth);
			LabelPositionInScreen.y = (ZEInt32)(((LabelPositionInScreen.y - 1.0f) / -2.0f) * WindowHeight);

			EntityTag->SetPosition(LabelPositionInScreen.ToVector2());
			EntityTag->SetZOrder((ZEInt32)-LabelPositionInViewProj.z);
		}

	}

	if (DrawCanvas.Vertices.GetCount() == 0)
		return;

	RenderCommand.PrimitiveCount = DrawCanvas.Vertices.GetCount() / 2;
	DrawParameters->Renderer->AddCommand(&RenderCommand);
}

bool ZEDebugDrawer::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (Material == NULL)
		Material = ZERNSimpleMaterial::CreateInstance();

	RenderCommand.SetZero();
	RenderCommand.Material = Material;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;
	RenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_LINE;
	RenderCommand.VertexBuffer = &DrawCanvas;
	RenderCommand.Priority = 10;

	EntityTag = ZEUIDebugDrawTag::CreateInstance();
	EntityTag->SetPosition(ZEVector2(0.0f, 0.0f));
	EntityTag->SetFontColor(DrawColor);
	EntityTag->SetIcon("zinek2.png");
	EntityTag->SetZOrder(1);
	zeGame->UIManager->AddControl(EntityTag);

	return true;
}

bool ZEDebugDrawer::DeinitializeSelf()
{
	Target = NULL;
	DrawCanvas.Clean();
	RenderCommand.SetZero();

	if (Material != NULL)
	{
		Material->Release();
		Material = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

ZEDebugDrawer::ZEDebugDrawer()
{
	Target = NULL;
	DrawColor = ZEVector4::Zero;
	DebugDrawElements.RaiseFlags(ZE_DDE_ALL);
	Material = NULL;
	RenderCommand.SetZero();
}

ZEDebugDrawer::~ZEDebugDrawer()
{

}

ZEDebugDrawer* ZEDebugDrawer::CreateInstance()
{
	return new ZEDebugDrawer(); 
}



