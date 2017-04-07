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

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEAngle.h"
#include "ZECore/ZETimeManager.h"
#include "ZEDViewportManager.h"
#include "ZEDViewport.h"
#include "ZEDEditor.h"


void ZEDViewportController::Viewport_OnKeyboardKeyPressing(ZEDViewport* Viewport, ZEDKeyboardKey Key)
{
	ZEVector3 Position = Viewport->GetPosition();
	ZEQuaternion Rotation = Viewport->GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;

	float ElapsedTime = ZETimeManager::GetInstance()->GetFrameTimeDelta();

	float ModStepSize = StepSize;
	if (Viewport->GetKeyModifiers().GetFlags(ZED_VKM_SHIFT))
		ModStepSize *= 8.0f;

	if (Viewport->GetKeyModifiers().GetFlags(ZED_VKM_CTRL))
		ModStepSize *= 0.25f;


	if (Key == ZED_VKK_W)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Viewport->SetPosition(Position);

		ZEMTEventBase::Acquire();
	}
	else if (Key == ZED_VKK_S)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitZ);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Viewport->SetPosition(Position);

		ZEMTEventBase::Acquire();
	}
	else if (Key == ZED_VKK_A)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::NegUnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Viewport->SetPosition(Position);

		ZEMTEventBase::Acquire();
	}
	else if (Key == ZED_VKK_D)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3::UnitX);
		ZEVector3::Scale(PositionChange, PositionChange, ModStepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Viewport->SetPosition(Position);

		ZEMTEventBase::Acquire();
	}
}

void ZEDViewportController::Viewport_OnMouseButtonPressed(ZEDViewport* Viewport, ZEDMouseButton Button)
{
	if (Button != ZED_VMB_MIDDLE && (Button != ZED_VMB_LEFT || Viewport->GetKeyModifiers() != ZED_VKM_ALT))
		return;

	Active = true;
	ZEQuaternion::ConvertToEulerAngles(Rx, Ry, Rz, Viewport->GetRotation());
	ZEMTEventBase::Acquire();
}

void ZEDViewportController::Viewport_OnMouseButtonReleased(ZEDViewport* Viewport, ZEDMouseButton Button)
{
	if (Button != ZED_VMB_MIDDLE && (Button != ZED_VMB_LEFT || Viewport->GetKeyModifiers() != ZED_VKM_ALT))
		return;

	Active = false;
	ZEMTEventBase::Acquire();
}

void ZEDViewportController::Viewport_OnMouseMoved(ZEDViewport* Viewport, const ZEVector2& Position)
{
	if (!Active)
		return;

	Rx += Viewport->GetMousePositionDelta().y * 0.01f * MouseSensivity;
	Ry += Viewport->GetMousePositionDelta().x * 0.01f * MouseSensivity;

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

	Viewport->SetRotation(NewRotation);

	ZEMTEventBase::Acquire();
}

bool ZEDViewportController::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	GetEditor()->GetViewportManager()->OnViewportKeyboardKeyPressing.AddDelegate<ZEDViewportController, &ZEDViewportController::Viewport_OnKeyboardKeyPressing>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonPressed.AddDelegate<ZEDViewportController, &ZEDViewportController::Viewport_OnMouseButtonPressed>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonReleased.AddDelegate<ZEDViewportController, &ZEDViewportController::Viewport_OnMouseButtonReleased>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseMoved.AddDelegate<ZEDViewportController,  &ZEDViewportController::Viewport_OnMouseMoved>(this);

	return true;
}

bool ZEDViewportController::DeinitializeInternal()
{
	GetEditor()->GetViewportManager()->OnViewportKeyboardKeyPressing.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonPressed.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonReleased.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseMoved.DisconnectObject(this);

	return ZEInitializable::DeinitializeInternal();
}
	
ZEDViewportController::ZEDViewportController()
{
	MouseSensivity = 1.0f;
	StepSize = 20.0f;
	LockCamera = false;
	Active = false;
	Rx = Ry = Rz = 0.0f;
}

ZEDViewportController::~ZEDViewportController()
{

}

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

ZEDViewportController* ZEDViewportController::CreateInstance()
{
	return new ZEDViewportController();
}
