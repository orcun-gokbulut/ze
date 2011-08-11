//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFreespaceInputDevice.cpp
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

#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"

#include "ZEInput\ZEInputMapBinding.h"
#include "ZEInput\ZEInputEvent.h"
#include "ZEInput\ZEInputAction.h"

#include "ZEFreespaceInputDevice.h"

#include <freespace/freespace.h>

bool ZEFreespaceInputDevice::Initialize()
{
	zeLog("Freespace Input Module", "Initializing module.");

	int Result;
	Result = freespace_init();
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not initialize Freespace. Error Code : %d.", Result);
		return false;
	}

	FreespaceDeviceId Devices[50];
	int NumberOfDevices;

	Result = freespace_getDeviceList(Devices, 50, &NumberOfDevices);
	if (NumberOfDevices == 0) 
	{
		zeError("Freespace Input Module", "Can not aquire number of freespace devices. Error Code : %d.", Result);
		return false;
	}
	zeLog("Freespace Input Module", "Found %d number of devices.", NumberOfDevices);

	zeLog("Freespace Input Module", "Opening first device.");
	Device = Devices[0];
	Result = freespace_openDevice(Device);
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not open device. Error Code : %d.", Result);
		return false;
	}

	Result = freespace_flush(Device);
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not flush device. Error Code : %d.", Result);
		return false;
	}

	freespace_message Message;
	memset(&Message, 0, sizeof(Message));
	Message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
	Message.dataModeRequest.enableBodyMotion = 1;
	Message.dataModeRequest.enableUserPosition = 1;
	Message.dataModeRequest.aggregate = 1;
	Message.dataModeRequest.inhibitPowerManager = 1;


	Result = freespace_sendMessage(Device, &Message);
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not send message. Error Code : %d.", Result);
		return false;
	}

	zeLog("Freespace Input Module", "Module initialized.");
	
	memset(Axises, 0, sizeof(Axises));
	memset(Buttons, 0, sizeof(Buttons));
	memset(Buttons, 0, sizeof(OldButtons));
	Orientation = ZEQuaternion::Identity;
	LinearPosition = ZEVector3::Zero;

	return true;
}

void ZEFreespaceInputDevice::Deinitialize()
{
	zeLog("Freespace Input Module", "Deinitializing module.");

	freespace_message Message;
	memset(&Message, 0, sizeof(Message));
	Message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
	Message.dataModeRequest.enableMouseMovement = 1;
	
	int Result = freespace_sendMessage(Device, &Message);
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not send message. Error Code : %d.", Result);
	}

	freespace_closeDevice(Device);
	freespace_exit();

	zeLog("Freespace Input Module", "Module deinitialized.");
}

void ZEFreespaceInputDevice::ProcessInputs()
{
	memset(Axises, 0, sizeof(Axises));
	memset(Buttons, 0, sizeof(Buttons));

	freespace_message Message;
	for (int I = 0; I < 10; I++)
	{
		int Result = freespace_readMessage(Device, &Message, 1);
		if (Result == FREESPACE_ERROR_TIMEOUT || Result == FREESPACE_ERROR_INTERRUPTED) 
			return;

		if (Result != FREESPACE_SUCCESS) 
		{
			zeError("Freespace Input Module", "Can not read message. Error Code : %d.", Result);
			return;
		}

		if (Message.messageType == FREESPACE_MESSAGE_USERFRAME)
		{
			Buttons[0] = Message.userFrame.button1 & 0x80;
			Buttons[1] = Message.userFrame.button2 & 0x80;
			Buttons[2] = Message.userFrame.button3 & 0x80;
			Buttons[3] = Message.userFrame.button4 & 0x80;
			Buttons[4] = Message.userFrame.button5 & 0x80;

			LinearPosition.x = Message.userFrame.linearPosX;
			LinearPosition.y = Message.userFrame.linearPosY;
			LinearPosition.z = Message.userFrame.linearPosZ;

			Orientation.w = Message.userFrame.angularPosA;
			Orientation.x = Message.userFrame.angularPosB;
			Orientation.y = Message.userFrame.angularPosD;
			Orientation.z = Message.userFrame.angularPosC;
			Orientation.NormalizeSelf();
			Orientation.ConjugateSelf();

			Axises[0] += Message.userFrame.deltaX;
			Axises[1] += Message.userFrame.deltaY;
			Axises[2] += Message.userFrame.deltaWheel;
		}
	}

	OldButtons[0] = Buttons[0];
	OldButtons[1] = Buttons[1];
	OldButtons[2] = Buttons[2];
	OldButtons[3] = Buttons[3];
	OldButtons[4] = Buttons[4];
}

bool ZEFreespaceInputDevice::ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* Action)
{
	ZEInputEvent* InputEvent = &InputBinding->Event;
	if (InputEvent->DeviceType == ZE_IDT_OTHER && InputEvent->DeviceIndex == 0)
	{
		switch(InputEvent->InputType)
		{
			case ZE_IT_AXIS:
				if (InputEvent->AxisId < 3)
				{
					if (InputEvent->AxisSign == ZE_IAS_POSITIVE)
					{
						if (Axises[InputEvent->AxisId] > 0)
						{
							Action->Id = InputBinding->ActionId;
							Action->From =  InputBinding;
							Action->AxisValue = Axises[InputEvent->AxisId];
						}
					}
					else
					{
						if (Axises[InputEvent->AxisId] < 0)
						{
							Action->Id = InputBinding->ActionId;
							Action->From =  InputBinding;
							Action->AxisValue = abs(Axises[InputEvent->AxisId]);
						}
					}
				}
				break;

			case ZE_IT_BUTTON:
				for (int I = 0; I < 5; I++)
					if ((InputEvent->ButtonState == ZE_IBS_ALL && (Buttons[InputEvent->ButtonId] & 0x80)) ||
						(InputEvent->ButtonState == ZE_IBS_PRESSED && (Buttons[InputEvent->ButtonId] & 0x80) && !(OldButtons[InputEvent->ButtonId] & 0x80)) || 
						(InputEvent->ButtonState == ZE_IBS_RELEASED && !(Buttons[InputEvent->ButtonId] & 0x80) && (OldButtons[InputEvent->ButtonId] & 0x80)))
					{
						Action->Id = InputBinding->ActionId;
						Action->From =  InputBinding;					
					}
				break;

			case ZE_IT_QUATERNION:
				if (InputEvent->OrientationId == 0)
				{
					Action->Id = InputBinding->ActionId;
					Action->From =  InputBinding;
					Action->Orientation = Orientation;
				}
				break;

			case ZE_IT_VECTOR3:
				if (InputEvent->VectorId == 0)
				{
					Action->Id = InputBinding->ActionId;
					Action->From =  InputBinding;
					Action->Vector = LinearPosition;
				}
				break;
		}
	}

	return true;
}