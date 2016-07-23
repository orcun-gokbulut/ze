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
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNScreenUtilities.h"
#include "ZEUI/ZEUILabel.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDObjectManager.h"

bool ZEDEntityWrapper::RayCastModifier(ZERayCastCollision& Collision, const void* Parameter)
{
	Collision.Object = this;
	return true;
}

bool ZEDEntityWrapper::UpdateGraphics()
{
	if (!Dirty)
		return true;

	ZEAABBox BoundingBox = GetLocalBoundingBox();

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	ConstantBuffer->SetData(&WorldMatrix);

	Canvas.Clean();
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

	if (VertexBuffer.IsNull() ||
		VertexBuffer->GetSize() < Canvas.GetBufferSize())
		VertexBuffer = ZEGRVertexBuffer::CreateResource(Canvas.GetVertexCount(), sizeof(ZECanvasVertex));

	void* Buffer;
	VertexBuffer->Lock(&Buffer);
	memcpy(Buffer, Canvas.GetBuffer(), Canvas.GetVertexCount() * sizeof(ZECanvasVertex));
	VertexBuffer->Unlock();

	Dirty = false;

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

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));

	NamePlate = new ZEUIControl();

	NamePlateLabel = new ZEUILabel();
	NamePlateIcon = new ZEUIFrameControl();
	NamePlateIcon->SetSize(ZEVector2(32.0f, 32.0f));
	NamePlateIcon->SetPosition(ZEVector2::Zero);

	NamePlateIcon->SetTexturePath(GetIcon());
	NamePlateIcon = new ZEUIFrameControl();
	NamePlateIcon->SetSize(ZEVector2(100.0f, 40.0f));
	NamePlateIcon->SetPosition(ZEVector2(36.0f, 0.0f));

	NamePlate->AddChildControl(NamePlateIcon);
	NamePlate->AddChildControl(NamePlateLabel);

	/*ZEUIManager* UIManager = GetManager()->GetEditor()->GetUIManager();
	UIManager->AddControl(NamePlate);*/

	return true;
}

bool ZEDEntityWrapper::DeinitializeInternal()
{
	delete NamePlate;

	Material.Release();
	ConstantBuffer.Release();
	VertexBuffer.Release();

	return ZEDObjectWrapper::DeinitializeInternal();
}

ZEDEntityWrapper::ZEDEntityWrapper()
{
	Dirty = true;
}

void ZEDEntityWrapper::SetId(ZEInt Id)
{
	GetEntity()->SetEntityId(Id);
}

ZEInt ZEDEntityWrapper::GetId() const
{
	return GetEntity()->GetEntityId();
}

void ZEDEntityWrapper::SetName(const ZEString& Name)
{
	GetEntity()->SetName(Name);
	NamePlateLabel->SetText(Name);
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
	
	if (IsInitialized())
	{
		NamePlateLabel->SetText(static_cast<ZEEntity*>(Object)->GetName());
		NamePlateIcon->SetTexturePath("#R:/Resources/ZEEngine/ZEGUI/Textures/SemiChecked.png");

		Update();
	}
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

ZEAABBox ZEDEntityWrapper::GetLocalBoundingBox() const
{
	if (GetEntity() == NULL)
		return ZEAABBox();

	return GetEntity()->GetWorldBoundingBox();
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

	Dirty = true;

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

	Dirty = true;

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

	Dirty = true;

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
	Dirty = true;
}

void ZEDEntityWrapper::SetFocused(bool Focused)
{
	if (GetFocused() == Focused)
		return;

	ZEDObjectWrapper::SetFocused(Focused);
	Dirty = true;
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


	ZEVector2 ScreenPosition = ZERNScreenUtilities::WorldToScreen(*Parameters->View, GetPosition());
	NamePlate->SetPosition(ScreenPosition);
	NamePlate->SetZOrder(ZEMath::Abs(Parameters->View->Position.z - GetPosition().z));
}

void ZEDEntityWrapper::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEDObjectWrapper::Render(Parameters, Command);

	if (GetObject() == NULL)
		return;

	ZEGRContext* Context = Parameters->Context;

	if (!Material->SetupMaterial(Context, Parameters->Stage))
		return;

	Context->SetVertexBuffer(0, VertexBuffer);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->Draw(VertexBuffer->GetSize() / VertexBuffer->GetVertexStride(), 0);

	Material->CleanupMaterial(Context, Parameters->Stage);
}

void ZEDEntityWrapper::Tick(float ElapsedTime)
{
	ZEDObjectWrapper::Tick(ElapsedTime);
}

void ZEDEntityWrapper::Update()
{
	ZEEntity* Entity = GetEntity();
	const ZEArray<ZEEntity*>& ChildEntities = Entity->GetChildEntities();

	SyncronizeChildWrappers((ZEObject*const*)ChildEntities.GetConstCArray(), ChildEntities.GetCount());
}

ZEDEntityWrapper* ZEDEntityWrapper::CreateInstance()
{
	return new ZEDEntityWrapper();
}
