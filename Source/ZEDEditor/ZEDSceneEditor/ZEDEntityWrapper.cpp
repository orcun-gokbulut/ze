//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntityWrapper.cpp
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

#include "ZEDEntityWrapper.h"

#include "ZEMath/ZEMath.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNScreenUtilities.h"
#include "ZEUI/ZEUILabel.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDObjectManager.h"
#include "ZEDS/ZEFormat.h"
#include "ZEUI/ZEUIFontTrueType.h"

void ZEDEntityWrapper::UpdateNamingPlate()
{
	if (GetManager() == NULL ||
		GetManager()->GetEditor() == NULL || 
		GetManager()->GetEditor()->GetUIManager() == NULL)
	{
		return;
	}

	ZEUIManager* UIManager = GetManager()->GetEditor()->GetUIManager();

	if (GetEntity() == NULL || !GetNamePlateVisible())
	{
		if (NamePlate != NULL)
		{	
			UIManager->RemoveControl(NamePlate);
			NamePlate->Destroy();
			NamePlate = NULL;
			NamePlateIcon = NULL;
			NamePlateName = NULL;
			NamePlateClass = NULL;
		}
		return;
	}
	else
	{
		if (NamePlate == NULL)
		{
			NamePlate = new ZEUIControl();
			NamePlate->SetSize(ZEVector2(200, 40));

			NamePlateIcon = new ZEUIFrameControl();
			NamePlateIcon->SetSize(ZEVector2(32.0f, 32.0f));
			NamePlateIcon->SetPosition(ZEVector2::Zero);
			NamePlate->AddChildControl(NamePlateIcon);

			NamePlateName = ZEUILabel::CreateInstance();
			NamePlateName->SetSize(ZEVector2(200.0f, 40.0f));
			NamePlateName->SetPosition(ZEVector2(36.0f, 2.0f));
			NamePlateName->SetFontResource(ZEUIFontTrueType::LoadResourceShared("#R:/ZEEngine/ZEGUI/Fonts/NotoSans-Bold.ttf", 14).GetPointer());
			NamePlate->AddChildControl(NamePlateName);
			
			NamePlateClass = ZEUILabel::CreateInstance();
			NamePlateClass->SetSize(ZEVector2(200.0f, 40.0f));
			NamePlateClass->SetPosition(ZEVector2(36.0f, 16.0f));
			NamePlateClass->SetFontColor(ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));
			NamePlate->AddChildControl(NamePlateClass);

			UIManager->AddControl(NamePlate);
		}

		if (GetSelected())
		{
			if (GetFocused())
				NamePlateName->SetFontColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
			else
				NamePlateName->SetFontColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			NamePlateName->SetFontColor(ZEVector4(0.75f, 0.75f, 0.75f, 1.0f));
		}
		
		NamePlateIcon->SetTextureFileName(GetIconFileName());
		NamePlateName->SetText(ZEFormat::Format("{0} ({1})", GetName(), GetId()));
		NamePlateClass->SetText(ZEFormat::Format("Class: {0}", GetObjectClass()->GetName()));
	}
}

bool ZEDEntityWrapper::RayCastModifier(ZERayCastCollision& Collision, const void* Parameter)
{
	Collision.Object = this;
	return true;
}

bool ZEDEntityWrapper::UpdateGraphics()
{
	if (!GraphicsDirty)
		return true;

	ZEAABBox BoundingBox = GetBoundingBox();

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	ConstantBuffer->SetData(&WorldMatrix);

	Canvas.Clean();
	if (DrawBoundingBox)
	{
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

		Canvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
	}

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

	GraphicsDirty = false;

	return true;
}

bool ZEDEntityWrapper::InitializeInternal()
{
	if (!ZEDObjectWrapper::InitializeInternal())
		return false;

	Material = ZERNSimpleMaterial::CreateInstance();
	Material->SetPrimitiveType(ZEGR_PT_LINE_LIST);
	Material->SetVertexColorEnabled(true);
	Material->SetStageMask(ZERN_STAGE_FORWARD_POST_HDR);
	Material->SetDepthTestDisabled(false);
	Material->Load();

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(ZEMatrix4x4), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	UpdateNamingPlate();

	return true;
}

bool ZEDEntityWrapper::DeinitializeInternal()
{
	if (NamePlate != NULL)
	{
		ZEUIManager* UIManager = GetManager()->GetEditor()->GetUIManager();
		UIManager->RemoveControl(NamePlate);
		NamePlate->Destroy();
		NamePlate = NULL;
	}

	Material.Release();
	ConstantBuffer.Release();
	VertexBuffer.Release();

	return ZEDObjectWrapper::DeinitializeInternal();
}

ZEDEntityWrapper::ZEDEntityWrapper()
{
	GraphicsDirty = true;
	NamePlate = NULL;
	NamePlateIcon = NULL;
	NamePlateName = NULL;
	NamePlateClass = NULL;
}

void ZEDEntityWrapper::SetId(ZEInt Id)
{
	ZEDObjectWrapper::SetId(Id);
	GetEntity()->SetEntityId(Id);
}

ZEInt ZEDEntityWrapper::GetId() const
{
	return GetEntity()->GetEntityId();
}

void ZEDEntityWrapper::SetName(const ZEString& Name)
{
	ZEDObjectWrapper::SetName(Name);
	GetEntity()->SetName(Name);
}

ZEString ZEDEntityWrapper::GetName() const
{
	return GetEntity()->GetName();
}

void ZEDEntityWrapper::SetObject(ZEObject* Object)
{
	if (!ZEClass::IsDerivedFrom(ZEEntity::Class(), Object->GetClass()))
		return;

	ZEDObjectWrapper::SetObject(Object);
	GetEntity()->SetWrapper(this);

	DrawBoundingBox =  Object->GetClass()->CheckAttributeHasValue("ZEDEditor.EntityWrapper.DrawBoundingBox", "true");

	Update();
	UpdateNamingPlate();
}

ZEEntity* ZEDEntityWrapper::GetEntity() const
{
	return static_cast<ZEEntity*>(GetObject());
}

void ZEDEntityWrapper::SetVisible(bool Value)
{
	static_cast<ZEEntity*>(GetObject())->SetVisible(Value);
}

bool ZEDEntityWrapper::GetVisible() const
{
	return static_cast<ZEEntity*>(GetObject())->GetVisible();
}

void ZEDEntityWrapper::SetDrawBoundingBox(bool Enabled)
{
	if (DrawBoundingBox == Enabled)
		return;

	DrawBoundingBox = Enabled;

	GraphicsDirty = true;
}

bool ZEDEntityWrapper::GetDrawBoundingBox()
{
	return DrawBoundingBox;
}

bool ZEDEntityWrapper::CheckChildrenClass(ZEClass* Class)
{
	return ZEClass::IsDerivedFrom(ZEEntity::Class(), Class);
}

bool ZEDEntityWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper != NULL && !ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return false;

	if (!ZEDObjectWrapper::AddChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetEntity() != NULL)
			GetEntity()->AddChildEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

bool ZEDEntityWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper != NULL && !ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return false;

	if (!ZEDObjectWrapper::RemoveChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetEntity() != NULL)
			GetEntity()->RemoveChildEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

ZEAABBox ZEDEntityWrapper::GetBoundingBox() const
{
	if (GetEntity() == NULL)
		return ZEAABBox();

	return GetEntity()->GetBoundingBox();
}

ZEMatrix4x4 ZEDEntityWrapper::GetWorldTransform() const
{
	if (GetEntity() == NULL)
		return ZEMatrix4x4::Identity;

	return GetEntity()->GetWorldTransform();
}

void ZEDEntityWrapper::SetPosition(const ZEVector3& NewPosition)
{
	if (GetEntity() == NULL)
		return;

	GetEntity()->SetWorldPosition(NewPosition);

	GraphicsDirty = true;

	ZEDObjectWrapper::SetPosition(NewPosition);
}

ZEVector3 ZEDEntityWrapper::GetPosition() const
{
	if (GetEntity() == NULL)
		return ZEVector3::Zero;

	return GetEntity()->GetWorldPosition();
}

void ZEDEntityWrapper::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetEntity() == NULL)
		return;

	GetEntity()->SetWorldRotation(NewRotation);

	GraphicsDirty = true;

	ZEDObjectWrapper::SetRotation(NewRotation);
}

ZEQuaternion ZEDEntityWrapper::GetRotation() const
{
	if (GetEntity() == NULL)
		return ZEQuaternion::Identity;

	return GetEntity()->GetWorldRotation();
}

void ZEDEntityWrapper::SetScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	GetEntity()->SetScale(NewScale);

	GraphicsDirty = true;

	ZEDObjectWrapper::SetScale(NewScale);
}

ZEVector3 ZEDEntityWrapper::GetScale() const
{
	if (GetEntity() == NULL)
		return ZEVector3::One;

	return GetEntity()->GetWorldScale();
}

void ZEDEntityWrapper::SetSelected(bool Selected)
{
	if (GetSelected() == Selected)
		return;

	ZEDObjectWrapper::SetSelected(Selected);

	UpdateNamingPlate();

	GraphicsDirty = true;
}

void ZEDEntityWrapper::SetFocused(bool Focused)
{
	if (GetFocused() == Focused)
		return;

	ZEDObjectWrapper::SetFocused(Focused);

	UpdateNamingPlate();

	GraphicsDirty = true;
}

void ZEDEntityWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	Report.SetModifierFunction(ZEDelegateMethod(ZERayCastModifierFunction, ZEDEntityWrapper, RayCastModifier, this));
	GetEntity()->RayCast(Report, Parameters);
	Report.SetModifierFunction(ZERayCastModifierFunction());
}

void ZEDEntityWrapper::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!UpdateGraphics())
		return;

	if (!Material->PreRender(Command))
		return;

	Command.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEDEntityWrapper, Render, this);
	Parameters->Renderer->AddCommand(&Command);

	if (NamePlate != NULL)
	{
		if (GetVisible() && !GetFrozen() &&
			ZEVector3::DotProduct(Parameters->View->Direction, GetPosition() - Parameters->View->Position) > 0.0f)
		{
			ZEVector2 ScreenPosition = ZERNScreenUtilities::WorldToScreen(*Parameters->View, GetPosition());
			NamePlate->SetPosition(ZEVector2((ZEInt)ScreenPosition.x, (ZEInt)ScreenPosition.y));
			NamePlate->SetZOrder(ZEMath::Abs(Parameters->View->Position.z - GetPosition().z));
			NamePlate->SetVisiblity(true);
		}
		else
		{
			NamePlate->SetVisiblity(false);
		}
	}
}

void ZEDEntityWrapper::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEDObjectWrapper::Render(Parameters, Command);

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

void ZEDEntityWrapper::Tick(float ElapsedTime)
{
	ZEDObjectWrapper::Tick(ElapsedTime);

	if (GetEntity() != NULL && GetEntity()->GetState() == ZE_ES_LOADING)
		Update();
}

void ZEDEntityWrapper::Update()
{
	if (GetEntity() == NULL)
	{
		ClearChildWrappers();
		return;
	}
	
	LockWrapper();
	SyncronizeChildWrappers((ZEObject*const*)GetEntity()->GetChildEntities().GetConstCArray(), GetEntity()->GetChildEntities().GetCount());
	UnlockWrapper();
}

void ZEDEntityWrapper::LockWrapper()
{
	if (GetEntity() != NULL)
		GetEntity()->LockEntity();
}

void ZEDEntityWrapper::UnlockWrapper()
{
	if (GetEntity() != NULL)
		GetEntity()->UnlockEntity();
}

ZEDEntityWrapper* ZEDEntityWrapper::CreateInstance()
{
	return new ZEDEntityWrapper();
}
