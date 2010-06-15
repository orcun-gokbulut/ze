//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputMap.cpp
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
#include "ZEInputMap.h"
#include "ZEInputDefinitions.h"
#include <string.h>
#include <stdio.h>

ZEInputBinding::ZEInputBinding(int ActionId, const char* Name, ZEInputEvent& Event)
{
	this->ActionId = ActionId;
	strcpy_s(this->Name, 50, Name);
	this->Event = Event;
}


ZEInputBinding::ZEInputBinding()
{
}


void ZEInputEvent::GetEventName(char* Buffer, size_t MaxSize)
{
	switch (DeviceType)
	{
		case ZE_IDT_KEYBOARD:
			sprintf_s(Buffer, MaxSize, "KBRD%d%s", DeviceIndex, zeGetKeyboardButtonShortName(ButtonId));
			break;

		case ZE_IDT_MOUSE:
			if (InputType == ZE_IT_BUTTON)
				sprintf_s(Buffer, MaxSize, "MOUS%d%d", DeviceIndex, ButtonId);
			else
				sprintf_s(Buffer, MaxSize, "MOUS%d%d%c", DeviceIndex, AxisId, (AxisSign == ZE_IAS_POSITIVE ? '+' : '-'));
			break;

		case ZE_IDT_JOYSTICK:
			if (InputType == ZE_IT_BUTTON)
				sprintf_s(Buffer, MaxSize, "JSTK%d%d", DeviceIndex, ButtonId);
			else
				sprintf_s(Buffer, MaxSize, "JSTK%d%d%c", DeviceIndex, AxisId, (AxisSign == ZE_IAS_POSITIVE ? '+' : '-'));
			break;
	}
}

void ZEInputEvent::GetEventShortName(char* Buffer, size_t MaxSize)
{
	switch (DeviceType)
	{
		case ZE_IDT_KEYBOARD:
			if (DeviceIndex == 0)
				sprintf_s(Buffer, MaxSize, "%s", DeviceIndex, zeGetKeyboardButtonShortName(ButtonId));
			else
				sprintf_s(Buffer, MaxSize, "Keyboard %d %s", DeviceIndex, zeGetKeyboardButtonName(ButtonId));
			break;

		case ZE_IDT_MOUSE:
			if (InputType == ZE_IT_BUTTON)
				switch(ButtonId)
				{
					case ZE_IMB_LEFTBUTTON:
						sprintf_s(Buffer, MaxSize, "Mouse Left Button");
						break;
					case ZE_IMB_RIGHTBUTTON:
						sprintf_s(Buffer, MaxSize, "Mouse Right Button");
						break;
					case ZE_IMB_MIDDLEBUTTON:
						sprintf_s(Buffer, MaxSize, "Mouse Middle Button");
						break;
					default:
						sprintf_s(Buffer, MaxSize, "Mouse %d Button", ButtonId);
						break;
				}
			else
				switch(ButtonId)
				{
					case ZE_IMA_HORIZANTAL_AXIS:
						sprintf_s(Buffer, MaxSize, "Mouse %s", (AxisSign == ZE_IAS_NEGATIVE ? "Left" : "Right"));
						break;
					case ZE_IMA_VERTICAL_AXIS:
						sprintf_s(Buffer, MaxSize, "Mouse %s", (AxisSign == ZE_IAS_NEGATIVE ? "Down" : "Up"));
						break;
					case ZE_IMA_SCROLL_AXIS:
						sprintf_s(Buffer, MaxSize, "Mouse %s", (AxisSign == ZE_IAS_NEGATIVE ? "Scroll Up" : "Scroll Down"));
						break;
					default:
						sprintf_s(Buffer, MaxSize, "Mouse %s %d Axis", (AxisSign == ZE_IAS_NEGATIVE ? "Negative" : "Positive"), AxisId);
						break;
				}
			break;

		case ZE_IDT_JOYSTICK:
			if (InputType == ZE_IT_BUTTON)
				sprintf_s(Buffer, MaxSize, "Joystick %d Button %d", DeviceIndex, ButtonId);
			else
				switch(AxisId)
				{
					case ZE_IJA_XAXIS:
						sprintf_s(Buffer, MaxSize, "Joystick %d %s X Axis", DeviceIndex, (AxisSign == ZE_IAS_POSITIVE ? "Positive" : "Negative"));
						break;
					case ZE_IJA_YAXIS:
						sprintf_s(Buffer, MaxSize, "Joystick %d %s Y Axis", DeviceIndex, (AxisSign == ZE_IAS_POSITIVE ? "Positive" : "Negative"));
						break;
					case ZE_IJA_ZAXIS:
						sprintf_s(Buffer, MaxSize, "Joystick %d %s Z Axis", DeviceIndex, (AxisSign == ZE_IAS_POSITIVE ? "Positive" : "Negative"));
						break;
					case ZE_IJA_WAXIS:
						sprintf_s(Buffer, MaxSize, "Joystick %d %s W Axis", DeviceIndex, (AxisSign == ZE_IAS_POSITIVE ? "Positive" : "Negative"));
						break;
					default:
						sprintf_s(Buffer, MaxSize, "Joystick %d %s %d Axis", DeviceIndex, (AxisSign == ZE_IAS_POSITIVE ? "Positive" : "Negative"), AxisId );
						break;
				}
			break;
	}
}
									
ZEInputEvent::ZEInputEvent()
{
}

ZEInputEvent::ZEInputEvent(ZEInputDeviceType DeviceType, ZEDWORD DeviceIndex, unsigned char ButtonId, ZEInputButtonState ButtonState)
{
	this->InputType = ZE_IT_BUTTON;
	this->DeviceType = DeviceType;
	this->DeviceIndex = DeviceIndex;
	this->ButtonId = ButtonId;
	this->ButtonState = ButtonState;
}

ZEInputEvent::ZEInputEvent(ZEInputDeviceType DeviceType, ZEDWORD DeviceIndex, unsigned char AxisId,	ZEInputAxisSign AxisSign)
{
	this->InputType = ZE_IT_AXIS;
	this->DeviceType = DeviceType;
	this->DeviceIndex = DeviceIndex;
	this->AxisId = AxisId;
	this->AxisSign = AxisSign;
}


