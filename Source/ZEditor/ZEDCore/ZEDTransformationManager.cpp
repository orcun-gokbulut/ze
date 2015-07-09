//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationManager.cpp
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

#include "ZEDTransformationManager.h"
#include "ZEDCore.h"
#include "ZEDGizmo.h"
#include "ZEDTransformationOperation.h"
#include "ZEDObjectWrapper.h"
#include "ZEDSelectionManager.h"
#include "ZEDOperationManager.h"

ZEDTransformationManager::ZEDTransformationManager()
{
	Gizmo = ZEDGizmo::CreateInstance();
	TransformType = ZED_TT_NONE;
	Transform = ZEMatrix4x4::Identity;
}

void ZEDTransformationManager::SetTransformType(ZEDTransformType Type)
{
	TransformType = Type;
}

ZEDTransformType ZEDTransformationManager::GetTransformType()
{
	return TransformType;
}

void ZEDTransformationManager::SetTransformSpace(ZEDTransformSpace Space)
{
	TransformSpace = Space;
}

ZEDTransformSpace ZEDTransformationManager::GetTransformSpace()
{
	return TransformSpace;
}

ZEDGizmo* ZEDTransformationManager::GetGizmo()
{
	return Gizmo;
}

void ZEDTransformationManager::BeginTransform(ZEDTransformType Type)
{
	if (Type == ZED_TT_NONE)
		return;

	if (TransformType != ZED_TT_NONE)
		return;

	SetTransformType(Type);
}

void ZEDTransformationManager::ResetTransform()
{
	if (TransformType == ZED_TT_NONE)
		return;

	const ZEArray<ZEDObjectWrapper*>& Selection = ZEDSelectionManager::GetInstance()->GetSelectedObjects();

	for (ZESize I = 0; I < Selection.GetCount(); I++)
	{
		switch (TransformType)
		{
			case ZED_TT_TRANSLATE:
				Selection[I]->SetWorldPosition(Selection[I]->GetWorldPosition() + (Transform.Inverse()).GetTranslation());
				break;
			case ZED_TT_ROTATE:
			{
				ZEQuaternion Result;
				ZEQuaternion::CreateFromMatrix(Result, Transform);
				Selection[I]->SetWorldRotation(Selection[I]->GetWorldRotation() * Result.Conjugate());
				break;
			}
			case ZED_TT_SCALE:
				Selection[I]->SetWorldScale(Selection[I]->GetWorldScale() * (Transform.Inverse()).GetScale());
				break;
		}
	}
}

void ZEDTransformationManager::ApplyTransform(ZEMatrix4x4 Transform)
{
	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	const ZEArray<ZEDObjectWrapper*>& Selection = SelectionManager->GetSelectedObjects();

	 if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_ENTITY_PIVOT)
	 {
		 for (ZESize I = 0; I < Selection.GetCount(); I++)
		 {

		 }
	 }
	 else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SELECTION_CENTER)
	 {

	 }
	 else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SPACE_CENTER)
	 {

	 }

	 ZEMatrix4x4::Multiply(this->Transform, Transform, this->Transform);
}

void ZEDTransformationManager::EndTransform()
{
	ZEDOperationManager::GetInstance()->DoOperation(new ZEDTransformationOperation(TransformType, Transform, ZEDSelectionManager::GetInstance()->GetSelectedObjects()));

	TransformType = ZED_TT_NONE;
	Transform = ZEMatrix4x4::Identity;
}

void ZEDTransformationManager::Destroy()
{
	if (Gizmo != NULL)
		delete Gizmo;

	delete this;
}

ZEDTransformationManager* ZEDTransformationManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetTransformationManager();
}
