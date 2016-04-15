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
	TransformSpace = ZED_TS_WORLD;
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
				Selection[I]->SetObjectPosition(Selection[I]->GetObjectPosition() + (Transform.Inverse()).GetTranslation());
				break;
			case ZED_TT_ROTATE:
			{
				ZEQuaternion Temp;
				ZEQuaternion::CreateFromMatrix(Temp, Transform);
				Selection[I]->SetObjectRotation(Temp.Conjugate() * Selection[I]->GetObjectRotation());
				break;
			}
			case ZED_TT_SCALE:
				Selection[I]->SetObjectScale(Selection[I]->GetObjectScale() * (Transform.Inverse()).GetScale());
				break;
		}
	}
}

void ZEDTransformationManager::ApplyTransform(ZEMatrix4x4 Transform)
{
	if (TransformType == ZED_TT_NONE)
		return;

	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	const ZEArray<ZEDObjectWrapper*>& Selection = SelectionManager->GetSelectedObjects();
	ZEDSelectionPivotMode PivotMode = SelectionManager->GetSelectionPivotMode();

	if (TransformType == ZED_TT_TRANSLATE)
	{
		for (ZESize I = 0; I < Selection.GetCount(); I++)
			Selection[I]->SetObjectPosition(Selection[I]->GetObjectPosition() + Transform.GetTranslation());
	}
	else if (TransformType == ZED_TT_ROTATE)
	{
		if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
				Selection[I]->SetObjectRotation(Transform.GetRotation() * Selection[I]->GetObjectRotation());
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEMatrix4x4 PivotTransform = SelectionManager->GetSelectionPivot();
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = PivotTransform.Inverse() * Selection[I]->GetObjectPosition();
				ZEVector3 RotatedPositionInPivotSpace = Rotation * ObjectPositionInPivotSpace;
				Selection[I]->SetObjectPosition(PivotTransform * RotatedPositionInPivotSpace);
				Selection[I]->SetObjectRotation(Rotation * Selection[I]->GetObjectRotation());
			}
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SPACE_CENTER)
		{
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				Selection[I]->SetObjectPosition(Rotation * Selection[I]->GetObjectPosition());
				Selection[I]->SetObjectRotation(Rotation * Selection[I]->GetObjectRotation());
			}
		}
		else
		{
			return;
		}
	}
	else if (TransformType == ZED_TT_SCALE)
	{
		if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
				Selection[I]->SetObjectScale(Transform.GetScale() * Selection[I]->GetObjectScale());
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEMatrix4x4 PivotTransform = SelectionManager->GetSelectionPivot();
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = PivotTransform.Inverse() * Selection[I]->GetObjectPosition();
				ZEVector3 ScaledPositionInPivotSpace = Scale * ObjectPositionInPivotSpace;
				Selection[I]->SetObjectPosition(PivotTransform * ScaledPositionInPivotSpace);
				Selection[I]->SetObjectScale(Scale * Selection[I]->GetObjectScale());
			}

		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SPACE_CENTER)
		{
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				Selection[I]->SetObjectPosition(Scale * Selection[I]->GetObjectPosition());
				Selection[I]->SetObjectScale(Scale * Selection[I]->GetObjectScale());
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}


	 ZEMatrix4x4 CurrentTransform = this->Transform;
	 ZEMatrix4x4::Multiply(this->Transform, Transform, CurrentTransform);
}

void ZEDTransformationManager::EndTransform()
{
	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	ZEDTransformationOperation* Operation = new ZEDTransformationOperation(TransformType, SelectionManager->GetSelectionPivotMode(), Transform, SelectionManager->GetSelectedObjects());
	Operation->SetApplyEnabled(false);
	ZEDOperationManager::GetInstance()->DoOperation(Operation);
	Operation->SetApplyEnabled(true);

	TransformType = ZED_TT_NONE;
	Transform = ZEMatrix4x4::Identity;
}

void ZEDTransformationManager::Destroy()
{
	if (Gizmo != NULL)
	{
		Gizmo->Destroy();
	}

	delete this;
}

ZEDTransformationManager* ZEDTransformationManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetTransformationManager();
}
