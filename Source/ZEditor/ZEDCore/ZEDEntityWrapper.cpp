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

#include "ZEDS/ZEDelegate.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZEGizmo.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"

bool ZEDEntityWrapper::RayCastModifier(ZERayCastCollision& Collision, const void* Parameter)
{
	Collision.Object = this;
	return true;
}

bool ZEDEntityWrapper::Update()
{
	if (!Dirty)
		return true;

	ZEAABBox BoundingBox = GetLocalBoundingBox();

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	if (Material.IsNull())
	{
		Material = ZERNSimpleMaterial::CreateInstance();
		Material->SetPrimitiveType(ZEGR_PT_LINE_LIST);
		Material->SetVertexColorEnabled(true);
		Material->SetStageMask(ZERN_STAGE_FORWARD_POST);
	}

	if (ConstantBuffer.IsNull())
		ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));

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
		VertexBuffer = ZEGRVertexBuffer::Create(Canvas.GetVertexCount(), sizeof(ZECanvasVertex));

	void* Buffer;
	VertexBuffer->Lock(&Buffer);
	memcpy(Buffer, Canvas.GetBuffer(), Canvas.GetVertexCount() * sizeof(ZECanvasVertex));
	VertexBuffer->Unlock();

	Dirty = false;

	return true;
}

ZEDEntityWrapper::ZEDEntityWrapper()
{
	Dirty = true;
}

void ZEDEntityWrapper::SetId(ZEInt Id)
{
	static_cast<ZEEntity*>(Object)->SetEntityId(Id);
}

ZEInt ZEDEntityWrapper::GetId()
{
	return static_cast<ZEEntity*>(Object)->GetEntityId();
}

void ZEDEntityWrapper::SetName(const ZEString& Name)
{
	static_cast<ZEEntity*>(Object)->SetName(Name);
}

ZEString ZEDEntityWrapper::GetName()
{
	return static_cast<ZEEntity*>(Object)->GetName();
}

void ZEDEntityWrapper::SetObject(ZEObject* Object)
{
	if (!ZEClass::IsDerivedFrom(ZEEntity::Class(), Object->GetClass()))
		return;

	ZEEntity* Entity = static_cast<ZEEntity*>(Object);

	Entity->SetWrapper(this);
	ZEDObjectWrapper::SetObject(Entity);

	const ZEArray<ZEEntity*>& Children = Entity->GetChildEntities();

	//Same finding suitable wrapper algorithm in ZEDScene.

	// 	const ZEArray<ZEClass*>& WrapperTypes = ZEDCore::GetInstance()->GetWrapperTypes();
	// 
	// 	for (ZESize I = 0; I < WrapperTypes.GetCount(); I++)
	// 	{
	// 		if (WrapperTypes[]) //WrapperTypes[I]->GetAttributes()->Values*
	// 		{
	// 			ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)WrapperTypes[I]->CreateInstance();
	// 			Wrapper->SetObject(Object);
	// 		}
	// 	}

	for (ZESize I = 0; I < Children.GetCount(); I++)
	{
		ZEDEntityWrapper* ChildWrapper = ZEDEntityWrapper::CreateInstance();
		ChildWrapper->SetObject(Children[I]);
		AddChildWrapper(ChildWrapper);
	}
}

void ZEDEntityWrapper::SetVisible(bool Value)
{
	static_cast<ZEEntity*>(GetObject())->SetVisible(Value);
}

bool ZEDEntityWrapper::GetVisible()
{
	return static_cast<ZEEntity*>(GetObject())->GetVisible();
}

ZEAABBox ZEDEntityWrapper::GetLocalBoundingBox()
{
	if (GetObject() == NULL)
		return ZEAABBox();

	return static_cast<ZEEntity*>(GetObject())->GetWorldBoundingBox();
}

ZEMatrix4x4 ZEDEntityWrapper::GetWorldTransform()
{
	if (GetObject() == NULL)
		return ZEMatrix4x4::Identity;

	return static_cast<ZEEntity*>(GetObject())->GetWorldTransform();
}

void ZEDEntityWrapper::SetPosition(const ZEVector3& NewPosition)
{
	if (GetObject() == NULL)
		return;

	static_cast<ZEEntity*>(GetObject())->SetWorldPosition(NewPosition);

	Dirty = true;
}

ZEVector3 ZEDEntityWrapper::GetPosition()
{
	if (GetObject() == NULL)
		return ZEVector3::Zero;

	return static_cast<ZEEntity*>(GetObject())->GetWorldPosition();
}

void ZEDEntityWrapper::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetObject() == NULL)
		return;

	static_cast<ZEEntity*>(GetObject())->SetWorldRotation(NewRotation);

	Dirty = true;
}

ZEQuaternion ZEDEntityWrapper::GetRotation()
{
	if (GetObject() == NULL)
		return ZEQuaternion::Identity;

	return static_cast<ZEEntity*>(GetObject())->GetWorldRotation();
}

void ZEDEntityWrapper::SetScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	static_cast<ZEEntity*>(GetObject())->SetWorldScale(NewScale);

	Dirty = true;
}

ZEVector3 ZEDEntityWrapper::GetScale()
{
	if (GetObject() == NULL)
		return ZEVector3::One;

	return static_cast<ZEEntity*>(GetObject())->GetWorldScale();
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
	static_cast<ZEEntity*>(Object)->RayCast(Report, Parameters);
	Report.SetModifierFunction(ZERayCastModifierFunction());
}

void ZEDEntityWrapper::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!Update())
		return;

	Command.StageMask = Material->GetStageMask();
	Command.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEDEntityWrapper, Render, this);
	Parameters->Renderer->AddCommand(&Command);
}

void ZEDEntityWrapper::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEDObjectWrapper::Render(Parameters, Command);

	if (GetObject() == NULL)
		return;

	ZEGRContext* Context = Parameters->Context;

	if (!Material->SetupMaterial(Context, Parameters->Stage))
		return;

	Context->SetVertexBuffers(0, 1, VertexBuffer.GetPointerToPointer());
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->Draw(VertexBuffer->GetSize() / VertexBuffer->GetVertexSize(), 0);

	Material->CleanupMaterial(Context, Parameters->Stage);
}

void ZEDEntityWrapper::Tick(float ElapsedTime)
{
	ZEDObjectWrapper::Tick(ElapsedTime);
}

ZEDEntityWrapper* ZEDEntityWrapper::CreateInstance()
{
	return new ZEDEntityWrapper();
}
