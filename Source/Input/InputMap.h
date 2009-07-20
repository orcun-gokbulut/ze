//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - InputMap.h
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

#pragma once
#ifndef	__ZE_INPUT_MAP_H__
#define __ZE_INPUT_MAP_H__

#include "ZEDS/ZEDS.h"

enum ZEInputButtonState
{
	ZE_IBS_ALL,
	ZE_IBS_PRESSED,
	ZE_IBS_RELEASED
};

enum ZEInputAxisSign
{
	ZE_IAS_POSITIVE,
	ZE_IAS_NEGATIVE
};

enum ZEInputType
{
	ZE_IT_AXIS, 
	ZE_IT_BUTTON
};

enum ZEInputDeviceType
{
	ZE_IDT_NONE,
	ZE_IDT_MOUSE,
	ZE_IDT_KEYBOARD,
	ZE_IDT_JOYSTICK
};

class ZEInputEvent
{
	public:
		ZEInputType					InputType;
		ZEInputDeviceType			DeviceType;
		ZEDWORD						DeviceIndex;
		union 
		{
			struct
			{
				unsigned char		ButtonId;
				ZEInputButtonState	ButtonState;
			};
			struct
			{
				unsigned char		AxisId;
				ZEInputAxisSign		AxisSign;
			};		
		};

		void						GetEventName(char* Buffer, size_t MaxSize);
		void						GetEventShortName(char* Buffer, size_t MaxSize);
									
									ZEInputEvent();
									ZEInputEvent(const char* ShortName);
									ZEInputEvent(ZEInputDeviceType Device, 
										ZEDWORD DeviceIndex, 
										unsigned char ButtonId, 
										ZEInputButtonState ButtonState);

									ZEInputEvent(ZEInputDeviceType Device, 
										ZEDWORD DeviceIndex, 
										unsigned char AxisId,
										ZEInputAxisSign AxisSign);
};


class ZEInputBinding
{
	public: 
		char						Name[50];
		int							ActionId;
		ZEInputEvent				Event;

									ZEInputBinding(int ActionId, const char* Name, ZEInputEvent& Event);
									ZEInputBinding();
};

class ZEInputAction
{
	public:
		int							Id;
		ZEInputBinding*				From;

		union
		{
			ZEInputButtonState		ButtonState;
			int						AxisValue;
		};
};


class ZEInputMap
{
	public:
		ZEArray<ZEInputBinding>		InputBindings;
		ZEInputAction				InputActions[50];
		int							InputActionCount;
};

#endif
   



  


