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
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZERenderer.h"

void ZEDEntityWrapper::DrawBoundingBox(const ZEVector4& Color)
{
	ZEAABBox BoundingBox = GetObjectBoundingBox();

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, BoundingBox.GetCenter(), 
		ZEQuaternion::Identity, 
		ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, BoundingBox.Max.y - BoundingBox.Min.y, BoundingBox.Max.z - BoundingBox.Min.z));

	DrawCanvas.SetColor(Color);
	DrawCanvas.SetTransfomation(WorldMatrix);
	DrawCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);
}

void ZEDEntityWrapper::SetObject(ZEObject* Object)
{
	if (!ZEClass::IsDerivedFrom(ZEEntity::Class(), Object->GetClass()))
		return;

	ZEEntity* Entity = (ZEEntity*)Object;

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

ZEObject* ZEDEntityWrapper::GetObject()
{
	return ZEDObjectWrapper::GetObject();
}

void ZEDEntityWrapper::SetObjectEnabled(bool Value)
{
	((ZEEntity*)GetObject())->SetEnabled(Value);
}

bool ZEDEntityWrapper::GetObjectEnabled()
{
	return ((ZEEntity*)GetObject())->GetEnabled();
}

void ZEDEntityWrapper::SetObjectVisibility(bool Value)
{
	((ZEEntity*)GetObject())->SetVisible(Value);
}

bool ZEDEntityWrapper::GetObjectVisibility()
{
	return ((ZEEntity*)GetObject())->GetVisible();
}

ZEAABBox ZEDEntityWrapper::GetObjectBoundingBox()
{
	if (GetObject() == NULL)
		return ZEAABBox();

	return ((ZEEntity*)GetObject())->GetWorldBoundingBox();
}

ZEMatrix4x4 ZEDEntityWrapper::GetObjectTransform()
{
	if (GetObject() == NULL)
		return ZEMatrix4x4::Identity;

	return ((ZEEntity*)GetObject())->GetWorldTransform();
}

void ZEDEntityWrapper::SetObjectPosition(const ZEVector3& NewPosition)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldPosition(NewPosition);
}

ZEVector3 ZEDEntityWrapper::GetObjectPosition()
{
	if (GetObject() == NULL)
		return ZEVector3::Zero;

	return ((ZEEntity*)GetObject())->GetWorldPosition();
}

void ZEDEntityWrapper::SetObjectRotation(const ZEQuaternion& NewRotation)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldRotation(NewRotation);
}

ZEQuaternion ZEDEntityWrapper::GetObjectRotation()
{
	if (GetObject() == NULL)
		return ZEQuaternion::Identity;

	return ((ZEEntity*)GetObject())->GetWorldRotation();
}

void ZEDEntityWrapper::SetObjectScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldScale(NewScale);
}

ZEVector3 ZEDEntityWrapper::GetObjectScale()
{
	if (GetObject() == NULL)
		return ZEVector3::One;

	return ((ZEEntity*)GetObject())->GetWorldScale();
}

void ZEDEntityWrapper::Draw(ZEDrawParameters* Parameters)
{
	RenderCommand.PrimitiveType = ZE_ROPT_LINE;
	DrawCanvas.Clean();

	if (GetObject() == NULL)
		return;

	DrawBoundingBox(ZEVector4(0.8f, 0.8f, 0.8f, 1.0f));

	if (DrawCanvas.Vertices.GetCount() == 0)
		return;

	RenderCommand.PrimitiveCount = DrawCanvas.Vertices.GetCount() / 2;
	Parameters->Renderer->AddToRenderList(&RenderCommand);

	ZEDObjectWrapper::Draw(Parameters);
}

void ZEDEntityWrapper::Tick(float ElapsedTime)
{
	ZEDObjectWrapper::Tick(ElapsedTime);
}

ZEDEntityWrapper* ZEDEntityWrapper::CreateInstance()
{
	return new ZEDEntityWrapper();
}
