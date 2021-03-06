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

#include "ZEDEditor.h"
#include "ZEDObjectWrapper3D.h"

bool ZEDTransformationOperation::Apply()
{
	for (ZESize I = 0; I < Transforms.GetCount(); I++)
	{
		ZEDTransformItem& TransformState = Transforms[I];
		TransformState.Wrapper->SetPosition(TransformState.Position);
		TransformState.Wrapper->SetRotation(TransformState.Rotation);
		TransformState.Wrapper->SetScale(TransformState.Scale);
		TransformState.Wrapper->SendChangedEvent();
	}

	return true;
}

bool ZEDTransformationOperation::Revert()
{
	for (ZESize I = 0; I < Transforms.GetCount(); I++)
	{
		ZEDTransformItem& TransformState = Transforms[I];
		TransformState.Wrapper->SetPosition(TransformState.OldPosition);
		TransformState.Wrapper->SetRotation(TransformState.OldRotation);
		TransformState.Wrapper->SetScale(TransformState.OldScale);
		TransformState.Wrapper->SendChangedEvent();
	}

	return true;
}

ZEDTransformationOperation::ZEDTransformationOperation()
{

}

ZEDTransformationOperation* ZEDTransformationOperation::Create(ZEDTransformType Type, const ZEArray<ZEDTransformationState>& TransformStates)
{
	ZEDTransformationOperation* Operation = new ZEDTransformationOperation();

	Operation->Transforms.SetCount(TransformStates.GetCount());
	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		Operation->Transforms[I].Wrapper = TransformStates[I].Wrapper;
		Operation->Transforms[I].Position = TransformStates[I].Wrapper->GetPosition();
		Operation->Transforms[I].Rotation = TransformStates[I].Wrapper->GetRotation();
		Operation->Transforms[I].Scale = TransformStates[I].Wrapper->GetScale();
		Operation->Transforms[I].OldPosition = TransformStates[I].OriginalPosition;
		Operation->Transforms[I].OldRotation = TransformStates[I].OriginalRotation;
		Operation->Transforms[I].OldScale = TransformStates[I].OriginalScale;
	}

	if (Type == ZED_TT_TRANSLATE)
		Operation->SetText("Change Position");
	else if (Type == ZED_TT_ROTATE)
		Operation->SetText("Change Rotation");
	else if (Type == ZED_TT_SCALE)
		Operation->SetText("Change Scale");

	return Operation;
}
