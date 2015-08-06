//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationOperation.cpp
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

#include "ZEDTransformationOperation.h"
#include "ZEDObjectWrapper.h"


bool ZEDTransformationOperation::Apply()
{
	if (Type == ZED_TT_NONE)
		return false;

	if (Type == ZED_TT_TRANSLATE)
	{
		for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			Wrappers[I]->SetObjectPosition(Wrappers[I]->GetObjectPosition() + Transform.GetTranslation());
	}
	else if (Type == ZED_TT_ROTATE)
	{
		if (PivotMode == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
				Wrappers[I]->SetObjectRotation((Transform.GetRotation() * Wrappers[I]->GetObjectRotation()).Normalize());
		}
		else if (PivotMode == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = Pivot.Inverse() * Wrappers[I]->GetObjectPosition();
				ZEVector3 RotatedPositionInPivotSpace = Rotation * ObjectPositionInPivotSpace;
				Wrappers[I]->SetObjectPosition(Pivot * RotatedPositionInPivotSpace);
				Wrappers[I]->SetObjectRotation((Rotation * Wrappers[I]->GetObjectRotation()).Normalize());
			}
		}
		else if (PivotMode == ZED_SCM_SPACE_CENTER)
		{
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				Wrappers[I]->SetObjectPosition(Rotation * Wrappers[I]->GetObjectPosition());
				Wrappers[I]->SetObjectRotation((Rotation * Wrappers[I]->GetObjectRotation()).Normalize());
			}
		}
		else
		{
			return false;
		}
	}
	else if (Type == ZED_TT_SCALE)
	{
		if (PivotMode == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() * Transform.GetScale());
		}
		else if (PivotMode == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = Pivot.Inverse() * Wrappers[I]->GetObjectPosition();
				ZEVector3 ScaledPositionInPivotSpace = ObjectPositionInPivotSpace * Scale;
				Wrappers[I]->SetObjectPosition(Pivot * ScaledPositionInPivotSpace);
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() * Scale);
			}

		}
		else if (PivotMode == ZED_SCM_SPACE_CENTER)
		{
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				Wrappers[I]->SetObjectPosition(Wrappers[I]->GetObjectPosition() * Scale);
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() * Scale);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool ZEDTransformationOperation::Revert()
{
	if (Type == ZED_TT_NONE)
		return false;

	if (Type == ZED_TT_TRANSLATE)
	{
		for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			Wrappers[I]->SetObjectPosition(Wrappers[I]->GetObjectPosition() - Transform.GetTranslation());
	}
	else if (Type == ZED_TT_ROTATE)
	{
		if (PivotMode == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
				Wrappers[I]->SetObjectRotation((Transform.GetRotation().Conjugate() * Wrappers[I]->GetObjectRotation()).Normalize());
		}
		else if (PivotMode == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEQuaternion Rotation = Transform.GetRotation().Conjugate();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = Pivot.Inverse() * Wrappers[I]->GetObjectPosition();
				ZEVector3 RotatedPositionInPivotSpace = Rotation * ObjectPositionInPivotSpace;
				Wrappers[I]->SetObjectPosition(Pivot * RotatedPositionInPivotSpace);
				Wrappers[I]->SetObjectRotation((Rotation * Wrappers[I]->GetObjectRotation()).Normalize());
			}
		}
		else if (PivotMode == ZED_SCM_SPACE_CENTER)
		{
			ZEQuaternion Rotation = Transform.GetRotation().Conjugate();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				Wrappers[I]->SetObjectPosition(Rotation * Wrappers[I]->GetObjectPosition());
				Wrappers[I]->SetObjectRotation((Rotation * Wrappers[I]->GetObjectRotation()).Normalize());
			}
		}
		else
		{
			return false;
		}
	}
	else if (Type == ZED_TT_SCALE)
	{
		if (PivotMode == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() / Transform.GetScale());
		}
		else if (PivotMode == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = Pivot.Inverse() * Wrappers[I]->GetObjectPosition();
				ZEVector3 ScaledPositionInPivotSpace = ObjectPositionInPivotSpace / Scale;
				Wrappers[I]->SetObjectPosition(Pivot * ScaledPositionInPivotSpace);
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() / Scale);
			}

		}
		else if (PivotMode == ZED_SCM_SPACE_CENTER)
		{
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Wrappers.GetCount(); I++)
			{
				Wrappers[I]->SetObjectPosition(Wrappers[I]->GetObjectPosition() / Scale);
				Wrappers[I]->SetObjectScale(Wrappers[I]->GetObjectScale() / Scale);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void ZEDTransformationOperation::Destroy()
{
	delete this;
}

ZEDTransformationOperation::ZEDTransformationOperation(ZEDTransformType Type, ZEDSelectionPivotMode PivotMode, ZEMatrix4x4 Transform, const ZEArray<ZEDObjectWrapper*>& Wrappers)
{
	this->Type = Type;
	this->Transform = Transform;
	this->PivotMode = PivotMode;

	if (PivotMode != ZED_SCM_SELECTION_CENTER)
		Pivot = ZEMatrix4x4::Identity;
	else
		Pivot = ZEDSelectionManager::GetInstance()->GetSelectionPivot();

	this->Wrappers.SetCount(Wrappers.GetCount());

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
		this->Wrappers[I] = Wrappers[I];
}
