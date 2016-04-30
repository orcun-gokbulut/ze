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

bool ZEDViewportController::KeyboardEventHandler(const ZEDViewportKeyboardEvent& Event)
{
	if (Event.Type != ZED_ET_BUTTON_PRESSING)
		return false;

	ZEVector3 Position = Event.Viewport->GetPosition();
	ZEQuaternion Rotation = Event.Viewport->GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;

	float ElapsedTime = zeCore->GetElapsedTime();

	float ModStepSize = StepSize;
	if ((Event.Modifiers & ZED_KKM_SHIFT) != 0)
		ModStepSize *= 5.0f;

	if ((Event.Modifiers & ZED_KKM_CTRL) != 0)
		ModStepSize *= 0.2f;


	if (Event.Key == ZED_IKK_KEY_W)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event.Viewport->SetPosition(Position);
		return true;
	}
	else if (Event.Key == ZED_IKK_KEY_S)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event.Viewport->SetPosition(Position);

		return true;
	}
	else if (Event.Key == ZED_IKK_KEY_A)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event.Viewport->SetPosition(Position);

		return true;
	}
	else if (Event.Key == ZED_IKK_KEY_D)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Event.Viewport->SetPosition(Position);

		return true;
	}

	return false;
}

bool ZEDViewportController::MouseEventHandler(const ZEDViewportMouseEvent& Event)
{
	if (Event.Button == ZED_MB_MIDDLE || (Event.Button == ZED_MB_LEFT && Event.Modifiers == ZED_KKM_ALT))
	{
		if (Event.Type == ZED_ET_BUTTON_PRESSED)
		{
			Active = true;
			ZEQuaternion::ConvertToEulerAngles(Rx, Ry, Rz, Event.Viewport->GetRotation());
			return true;
		}
		else if (Event.Type == ZED_ET_BUTTON_RELEASED)
		{
			Active = false;
			return true;
		}
	}

	if (Event.Type != ZED_ET_MOUSE_MOVED)
		return false;

	if (!Active)
		return false;

	Rx += Event.Delta.y * 0.01f * MouseSensivity;
	Ry += Event.Delta.x * 0.01f * MouseSensivity;

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

	Event.Viewport->SetRotation(NewRotation);
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
