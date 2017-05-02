//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectWrapper3D.cpp
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

#include "ZEDObjectWrapper3D.h"

#include "ZEMath/ZEMath.h"
#include "ZEDS/ZEFormat.h"
#include "ZEUI/ZEUIManager.h"
#include "ZEUI/ZEUIControl.h"
#include "ZEUI/ZEUIFrameControl.h"
#include "ZEUI/ZEUILabel.h"
#include "ZEUI/ZEUIFontTrueType.h"
#include "ZEError.h"
#include "ZEDEditor.h"
#include "ZEDObjectManager.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNScreenUtilities.h"


bool ZEDObjectWrapper3D::InitializeInternal()
{
	if (!ZEDObjectWrapper::InitializeInternal())
		return false;

	UpdateNameplate();
	UpdateGraphics();

	return true;
}

bool ZEDObjectWrapper3D::DeinitializeInternal()
{
	if (Nameplate != NULL)
	{
		ZEUIManager* UIManager = GetManager()->GetEditor()->GetUIManager();
		UIManager->RemoveControl(Nameplate);
		Nameplate->Destroy();
		Nameplate = NULL;
	}

	Material.Release();
	ConstantBuffer.Release();
	VertexBuffer.Release();

	return ZEDObjectWrapper::DeinitializeInternal();
}

ZEGRCanvas* ZEDObjectWrapper3D::GetCanvas()
{
	return &Canvas;
}

ZEUIControl* ZEDObjectWrapper3D::GetNameplate()
{
	return Nameplate;
}

void ZEDObjectWrapper3D::UpdateNameplate()
{
	if (GetNameplateVisible())
	{
		ZEDObjectManager* Manager = GetManager();
		if (Manager == NULL)
			return;

		ZEDEditor* Editor = Manager->GetEditor();
		if (Editor == NULL)
			return;

		ZEUIManager* UIManager = Editor->GetUIManager();
		if (UIManager == NULL)
			return;

		if (Nameplate == NULL)
		{
			Nameplate = new ZEUIControl();
			Nameplate->SetSize(ZEVector2(200, 40));

			NameplateIcon = new ZEUIFrameControl();
			NameplateIcon->SetSize(ZEVector2(32.0f, 32.0f));
			NameplateIcon->SetPosition(ZEVector2::Zero);
			Nameplate->AddChildControl(NameplateIcon);

			NameplateName = ZEUILabel::CreateInstance();
			NameplateName->SetSize(ZEVector2(200.0f, 40.0f));
			NameplateName->SetPosition(ZEVector2(36.0f, 2.0f));
			NameplateName->SetFontResource(ZEUIFontTrueType::LoadResourceShared("#R:/ZEEngine/ZEGUI/Fonts/NotoSans-Bold.ttf", 14).GetPointer());
			Nameplate->AddChildControl(NameplateName);

			NameplateClass = ZEUILabel::CreateInstance();
			NameplateClass->SetSize(ZEVector2(200.0f, 40.0f));
			NameplateClass->SetPosition(ZEVector2(36.0f, 16.0f));
			NameplateClass->SetFontColor(ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));
			Nameplate->AddChildControl(NameplateClass);

			UIManager->AddControl(Nameplate);
		}

		if (GetSelected())
		{
			NameplateName->SetVisiblity(true);
			NameplateClass->SetVisiblity(true);
			if (GetFocused())
				NameplateName->SetFontColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
			else
				NameplateName->SetFontColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			NameplateName->SetVisiblity(false);
			NameplateClass->SetVisiblity(false);
		}

		NameplateIcon->SetTextureFileName(GetIconFileName());
		NameplateName->SetText(ZEFormat::Format("{0} ({1})", GetName(), GetId()));

		ZEString ClassName;

		if (GetObjectClass() != NULL)
			ClassName = GetObjectClass()->GetName();
		else
			ClassName = "-";

		NameplateClass->SetText(ZEFormat::Format("Class: {0}", ClassName));
	}
	else
	{
		if (Nameplate == NULL)
			return;

		ZEDObjectManager* Manager = GetManager();
		if (Manager == NULL)
			return;

		ZEDEditor* Editor = Manager->GetEditor();
		if (Editor == NULL)
			return;

		ZEUIManager* UIManager = Editor->GetUIManager();
		if (UIManager == NULL)
			return;

		UIManager->RemoveControl(Nameplate);
		Nameplate->Destroy();
		Nameplate = NULL;
		NameplateIcon = NULL;
		NameplateName = NULL;
		NameplateClass = NULL;
	}
}

void ZEDObjectWrapper3D::UpdateCanvas()
{
	ConstantBuffer->SetData(&GetWorldTransform());

	if (GetSelected())
	{
		if (GetFocused())
			Canvas.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		else
			Canvas.SetColor(ZEVector4::One);
	}
	else
	{
		Canvas.SetColor(ZEVector4(0.5, 0.5, 0.5, 1.0f));
	}

	Canvas.ResetTransforms();
	ZEAABBox BoundingBox = GetBoundingBox();
	Canvas.ApplyTranslation(BoundingBox.GetCenter());
	Canvas.AddWireframeBox(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z);
}

void ZEDObjectWrapper3D::UpdateGraphics()
{
	Canvas.Clean();

	if (GetBoundingBoxVisible())
	{
		if (Material.IsNull())
		{
			Material = ZERNSimpleMaterial::CreateInstance();
			Material->SetPrimitiveType(ZEGR_PT_LINE_LIST);
			Material->SetVertexColorEnabled(true);
			Material->SetStageMask(ZERN_STAGE_FORWARD_POST_HDR);
			Material->SetDepthTestDisabled(false);
			Material->Load();
		}

		if (ConstantBuffer.IsNull())
			ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(ZEMatrix4x4), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

		UpdateCanvas();

		if (Canvas.GetBufferSize() != 0)
		{
			if (VertexBuffer.IsNull() ||
				VertexBuffer->GetSize() < Canvas.GetBufferSize())
			{
				VertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, Canvas.GetBufferSize(), sizeof(ZECanvasVertex), ZEGR_RU_DYNAMIC, ZEGR_RBF_VERTEX_BUFFER);
			}

			void* Buffer;
			VertexBuffer->Map(ZEGR_RMT_WRITE_DISCARD, &Buffer);
			memcpy(Buffer, Canvas.GetBuffer(), Canvas.GetBufferSize());
			VertexBuffer->Unmap();
		}
	}
	else
	{
		if (!Material.IsNull())
			Material.Release();
		
		if (!ConstantBuffer.IsNull())
			ConstantBuffer.Release();

		if (!VertexBuffer.IsNull())
			VertexBuffer.Release();
	}
}

ZEDObjectWrapper3D::ZEDObjectWrapper3D()
{
	Pickable = true;
	Transformable = true;
	NameplateVisible = true;
	BoundingBoxVisible = true;

	Nameplate = NULL;
	NameplateIcon = NULL;
	NameplateName = NULL;
	NameplateClass = NULL;
}

ZEDObjectWrapper3D::~ZEDObjectWrapper3D()
{
	Deinitialize();
}

ZEDObjectWrapper3D* ZEDObjectWrapper3D::GetParent3D() const
{
	zeDebugCheck(!ZEClass::IsDerivedFrom(ZEDObjectWrapper3D::Class(), GetParent3D()), "Parent is not derived from ZEDObjectWrapper3D");
	return ZEClass::Cast<ZEDObjectWrapper3D>(GetParent());
}

void ZEDObjectWrapper3D::SetObject(ZEObject* Object)
{
	if (GetObject() == Object)
		return;

	Pickable = Object->GetClass()->CheckAttributeHasValue("ZEDEditor.ObjectWrapper3D.Pickable", "true");
	Transformable = Object->GetClass()->CheckAttributeHasValue("ZEDEditor.ObjectWrapper3D.Transformable", "true");
	NameplateVisible = Object->GetClass()->CheckAttributeHasValue("ZEDEditor.ObjectWrapper3D.NameplateVisible", "true");
	BoundingBoxVisible = Object->GetClass()->CheckAttributeHasValue("ZEDEditor.ObjectWrapper3D.BoundingBoxVisible", "true");

	ZEDObjectWrapper::SetObject(Object);
}

void ZEDObjectWrapper3D::SetId(ZEInt Id)
{
	UpdateLocal();
}

void ZEDObjectWrapper3D::SetName(const ZEString& Name)
{
	UpdateLocal();
}

ZEAABBox ZEDObjectWrapper3D::GetBoundingBox() const
{
	return ZEAABBox::Zero;
}

ZEMatrix4x4 ZEDObjectWrapper3D::GetWorldTransform() const
{
	return ZEMatrix4x4::Identity;
}

void ZEDObjectWrapper3D::SetPosition(const ZEVector3& NewPosition)
{
	Update();
}

ZEVector3 ZEDObjectWrapper3D::GetPosition() const
{
	return ZEVector3::Zero;
}

void ZEDObjectWrapper3D::SetRotation(const ZEQuaternion& NewRotation)
{
	Update();
}

ZEQuaternion ZEDObjectWrapper3D::GetRotation() const
{
	return ZEQuaternion::Identity;
}

void ZEDObjectWrapper3D::SetScale(const ZEVector3& NewScale)
{
	Update();
}

ZEVector3 ZEDObjectWrapper3D::GetScale() const
{
	return ZEVector3::One;
}

const ZEArray<ZEDObjectWrapper3D*>& ZEDObjectWrapper3D::GetChildWrapper3Ds()
{
	return *reinterpret_cast<const ZEArray<ZEDObjectWrapper3D*>*>(&GetChildWrappers());
}

bool ZEDObjectWrapper3D::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	zeCheckError(!ZEClass::IsDerivedFrom(Class(), Wrapper), false, "Add child wrapper failed. Wrapper is not derived from ZEDObjectWrapper3D.");
	return ZEDObjectWrapper::AddChildWrapper(Wrapper, Update);
}

void ZEDObjectWrapper3D::SetVisible(bool Value)
{
	Update();	
}

bool ZEDObjectWrapper3D::GetVisible() const
{
	return true;
}

void ZEDObjectWrapper3D::SetNameplateVisible(bool Visible)
{
	if (NameplateVisible == Visible)
		return;

	NameplateVisible = Visible;

	UpdateNameplate();
}

bool ZEDObjectWrapper3D::GetNameplateVisible() const
{
	return NameplateVisible;
}


void ZEDObjectWrapper3D::SetBoundingBoxVisible(bool Visible)
{
	if (BoundingBoxVisible == Visible)
		return;

	BoundingBoxVisible = Visible;

	UpdateLocal();
}

bool ZEDObjectWrapper3D::GetBoundingBoxVisible() const
{
	return BoundingBoxVisible;
}

void ZEDObjectWrapper3D::SetPickable(bool Pickable)
{
	this->Pickable = Pickable;
}

bool ZEDObjectWrapper3D::GetPickable() const
{
	return Pickable;
}

void ZEDObjectWrapper3D::SetTransformable(bool Transformable)
{
	this->Transformable = Transformable;
}

bool ZEDObjectWrapper3D::GetTransformable() const
{
	return Transformable;
}

void ZEDObjectWrapper3D::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (Canvas.GetBufferSize() != 0)
	{
		if (!Material->PreRender(Command))
			return;

		Command.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEDObjectWrapper3D, Render, this);
		Parameters->Renderer->AddCommand(&Command);
	}

	if (Nameplate != NULL)
	{
		if (GetVisible() && !GetFrozen() &&
			ZEVector3::DotProduct(Parameters->View->Direction, GetPosition() - Parameters->View->Position) > 0.0f)
		{
			ZEVector2 ScreenPosition = ZERNScreenUtilities::WorldToScreen(*Parameters->View, GetPosition());
			Nameplate->SetPosition(ZEVector2((ZEInt)ScreenPosition.x, (ZEInt)ScreenPosition.y));
			Nameplate->SetZOrder(ZEMath::Abs(Parameters->View->Position.z - GetPosition().z));
			Nameplate->SetVisiblity(true);
		}
		else
		{
			Nameplate->SetVisiblity(false);
		}
	}
}

void ZEDObjectWrapper3D::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (GetObject() == NULL)
		return;

	if (Canvas.GetBufferSize() == 0)
		return;

	ZEGRContext* Context = Parameters->Context;

	if (!Material->SetupMaterial(Context, Parameters->Stage))
		return;

	Context->SetVertexBuffer(0, VertexBuffer);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->Draw(VertexBuffer->GetElementCount(), 0);

	Material->CleanupMaterial(Context, Parameters->Stage);
}

void ZEDObjectWrapper3D::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{

}

void ZEDObjectWrapper3D::UpdateLocal()
{
	if (!IsInitialized())
		return;

	ZEDObjectWrapper::UpdateLocal();

	UpdateNameplate();
	UpdateGraphics();
}
