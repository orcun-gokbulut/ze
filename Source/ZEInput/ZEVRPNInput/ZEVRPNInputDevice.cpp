//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVRPNInputDevice.cpp
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

#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"

#include "ZEInput/ZEInputMapBinding.h"
#include "ZEInput/ZEInputEvent.h"
#include "ZEInput/ZEInputAction.h"

#include "ZEVRPNInputDevice.h"

#include <freespace/freespace.h>

inline ZEVector3 VRPN_TO_ZE_VEC(const vrpn_float64* Position)
{
	return ZEVector3((float)Position[0], (float)Position[1], (float)Position[2]);
}

inline ZEQuaternion VRPN_TO_ZE_QUAD(const vrpn_float64* Quaternion)
{
	return ZEQuaternion((float)Quaternion[3], -(float)Quaternion[1], (float)Quaternion[2], -(float)Quaternion[0]).Normalize();	
}

static void VRPN_CALLBACK Tracker_ChangeHandler(void* UserData, const vrpn_TRACKERCB Data)
{
	ZEVRPNInputDevice* Device = (ZEVRPNInputDevice*)UserData;
	Device->Vector = VRPN_TO_ZE_VEC(Data.pos);
	Device->Quaternion = VRPN_TO_ZE_QUAD(Data.quat);
}

static void VRPN_CALLBACK Button_ChangeHandler(void* UserData, const vrpn_BUTTONCB Data)
{
	ZEVRPNInputDevice* Device = (ZEVRPNInputDevice*)UserData;

	if (Data.button > 4)
		return;

	Device->Buttons[Data.button] = true;
}

static void VRPN_CALLBACK Axis_ChangeHandler(void* UserData, const vrpn_ANALOGCB Data)
{
	ZEVRPNInputDevice* Device = (ZEVRPNInputDevice*)UserData;

	int AxisCount = Data.num_channel > 3 ? 3 : Data.num_channel;
	for (int I = 0; I < AxisCount; I++)
		Device->Axises[I] = (float)(Data.channel[I]);
}


ZEVRPNInputDevice::ZEVRPNInputDevice()
{
	DeviceName = "VRPN";
	TrackerRemote = NULL;
	ButtonRemote = NULL;
	AxisRemote = NULL;

	Axises[0] = 0.0f;
	Axises[1] = 0.0f;
	Axises[2] = 0.0f;

	Buttons[0] = false;
	Buttons[1] = false;
	Buttons[2] = false;
	Buttons[3] = false;
	Buttons[4] = false;

	OldButtons[0] = false;
	OldButtons[1] = false;
	OldButtons[2] = false;
	OldButtons[3] = false;
	OldButtons[4] = false;
}

const ZEString& ZEVRPNInputDevice::GetDeviceName()
{
	return DeviceName;
}

ZEDWORD ZEVRPNInputDevice::GetAxisCount()
{
	return 3;
}

ZEDWORD ZEVRPNInputDevice::GetButtonCount()
{
	return 5;
}

ZEDWORD ZEVRPNInputDevice::GetVector3Count()
{
	return 1;
}

ZEDWORD ZEVRPNInputDevice::GetQuaternionCount()
{
	return 1;
}

bool ZEVRPNInputDevice::Initialize()
{
	if (IsInitialized())
		return false;

	TrackerRemote = new vrpn_Tracker_Remote("Tracker0@localhost");
	TrackerRemote->register_change_handler(this, Tracker_ChangeHandler);

	ButtonRemote = new vrpn_Button_Remote("Mouse0@localhost");
	ButtonRemote->register_change_handler(this, Button_ChangeHandler);

	AxisRemote = new vrpn_Analog_Remote("Mouse0@localhost");
	AxisRemote->register_change_handler(this, Axis_ChangeHandler);

	return ZEInputDevice::Initialize();
}

void ZEVRPNInputDevice::Deinitialize()
{
	if (!IsInitialized())
		return;

	delete TrackerRemote;
	delete AxisRemote;
	delete ButtonRemote;

	return ZEInputDevice::Deinitialize();
}

void ZEVRPNInputDevice::ProcessInputs()
{
	OldButtons[0] = Buttons[0];
	OldButtons[1] = Buttons[1];
	OldButtons[2] = Buttons[2];
	OldButtons[3] = Buttons[3];
	OldButtons[4] = Buttons[4];

	memset(Axises, 0, sizeof(Axises));
	memset(Buttons, 0, sizeof(Buttons));
	
	AxisRemote->mainloop();
	ButtonRemote->mainloop();
	TrackerRemote->mainloop();

}

bool ZEVRPNInputDevice::ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* Action)
{
	ZEInputEvent* InputEvent = &InputBinding->Event;
	if (InputEvent->Device == this)
	{
		switch(InputEvent->Type)
		{
			case ZE_IT_AXIS:
				if (InputEvent->Index < 3)
				{
					if (InputEvent->AxisSign == ZE_IAS_POSITIVE)
					{
						if (Axises[InputEvent->Index] > 0)
						{
							Action->Id = InputBinding->ActionId;
							Action->From =  InputBinding;
							Action->AxisValue = (int)(Axises[InputEvent->Index]);
							return true;
						}
					}
					else
					{
						if (Axises[InputEvent->Index] < 0)
						{
							Action->Id = InputBinding->ActionId;
							Action->From =  InputBinding;
							Action->AxisValue = (int)(-Axises[InputEvent->Index]);
							return true;
						}
					}
				}
				break;

			case ZE_IT_BUTTON:
				for (int I = 0; I < 5; I++)
					if ((InputEvent->ButtonState == ZE_IBS_PRESSING && (Buttons[InputEvent->Index] == true)) ||
						(InputEvent->ButtonState == ZE_IBS_PRESSED && (Buttons[InputEvent->Index] == true) && !(OldButtons[InputEvent->Index] == true)) || 
						(InputEvent->ButtonState == ZE_IBS_RELEASED && !(Buttons[InputEvent->Index] == true) && (OldButtons[InputEvent->Index] == true)))
					{
						Action->Id = InputBinding->ActionId;
						Action->From =  InputBinding;					
						return true;
					}
				break;

			case ZE_IT_QUATERNION:
				if (InputEvent->Index == 0)
				{
					Action->Id = InputBinding->ActionId;
					Action->From =  InputBinding;
					Action->Quaternion = Quaternion;
					return true;
				}
				break;

			case ZE_IT_VECTOR3:
				if (InputEvent->Index == 0)
				{
					Action->Id = InputBinding->ActionId;
					Action->From =  InputBinding;
					Action->Vector = Vector;
					return true;
				}
				break;
		}
	}

	return false;
}
