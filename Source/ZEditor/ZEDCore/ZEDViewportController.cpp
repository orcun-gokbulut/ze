//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportController.cpp
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

#include "ZEDViewportController.h"

#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEDViewPort.h"
#include "ZECore\ZECore.h"
#include "ZEMath\ZEAngle.h"

void ZEDViewportController::SetLockCamera(bool Enabled)
{
	LockCamera = Enabled;
}

bool ZEDViewportController::GetLockCamera()
{
	return LockCamera;
}

void ZEDViewportController::SetStepSize(float StepSize)
{
	this->StepSize = StepSize;
}

float ZEDViewportController::GetStepSize()
{
	return StepSize;
}

void ZEDViewportController::SetMouseSensivity(float Sensivity)
{
	MouseSensivity = Sensivity;
}

float ZEDViewportController::GetMouseSensivity()
{
	return MouseSensivity;
}

void ZEDViewportController::ViewportKeyboardEvent(const ZEDViewportKeyboardEvent* Event)
{
	if (Event->GetType() != ZED_VIET_BUTTON_PRESSING)
		return;

	ZEVector3 Position = Event->GetViewport()->GetPosition();
	ZEQuaternion Rotation = Event->GetViewport()->GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;

	float ElapsedTime = zeCore->GetElapsedTime();

	float ModStepSize = StepSize;
	if ((Event->GetModifiers() & ZED_VKM_SHIFT) != 0)
		ModStepSize *= 5.0f;

	if ((Event->GetModifiers() & ZED_VKM_CTRL) != 0)
		ModStepSize *= 0.2f;


	if (Event->GetKey() == ZED_VKK_W)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event->GetViewport()->SetPosition(Position);
		
		Event->Acquire();
	}
	else if (Event->GetKey() == ZED_VKK_S)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event->GetViewport()->SetPosition(Position);

		Event->Acquire();
	}
	else if (Event->GetKey() == ZED_VKK_A)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event->GetViewport()->SetPosition(Position);

		Event->Acquire();
	}
	else if (Event->GetKey() == ZED_VKK_D)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event->GetViewport()->SetPosition(Position);

		Event->Acquire();
	}
}

void ZEDViewportController::ViewportMouseEvent(const ZEDViewportMouseEvent* Event)
{
	if (Event->GetButton() == ZED_VMB_MIDDLE || (Event->GetButton() == ZED_VMB_LEFT && Event->GetModifiers() == ZED_VKM_ALT))
	{
		if (Event->GetType() == ZED_VIET_BUTTON_PRESSED)
		{
			Active = true;
			ZEQuaternion::ConvertToEulerAngles(Rx, Ry, Rz, Event->GetViewport()->GetRotation());
			Event->Acquire();
		}
		else if (Event->GetType() == ZED_VIET_BUTTON_RELEASED)
		{
			Active = false;
			Event->Acquire();
		}
	}

	if (Event->GetType() != ZED_VIET_MOUSE_MOVED)
		return;

	if (!Active)
		return;

	Rx += Event->GetDelta().y * 0.01f * MouseSensivity;
	Ry += Event->GetDelta().x * 0.01f * MouseSensivity;

	if (Ry < -ZE_PI)
		Ry = ZE_PI;
	else if (Ry > ZE_PI)
		Ry = -ZE_PI;

	if (Rx > ZE_PI_2)
		Rx = ZE_PI_2;
	else if (Rx < -ZE_PI_2)
		Rx = -ZE_PI_2;

	ZEQuaternion NewRotation;
	ZEQuaternion::CreateFromEuler(NewRotation, Rx, Ry, Rz);

	Event->GetViewport()->SetRotation(NewRotation);

	Event->Acquire();
}

ZEDViewportController::ZEDViewportController()
{
	MouseSensivity = 1.0f;
	StepSize = 10.0f;
	LockCamera = false;
	Active = false;
	Rx = Ry = Rz = 0.0f;
}

ZEDViewportController::~ZEDViewportController()
{

}
