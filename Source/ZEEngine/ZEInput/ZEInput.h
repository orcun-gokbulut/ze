//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInput.h
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
#ifndef	__ZE_INPUT_H__
#define __ZE_INPUT_H__

#include "ZEInputAction.h"
#include "ZEInputDefinitions.h"

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEExport.ZEEngine.h"

enum ZEInputButtonState
{
	ZE_IBS_PRESSING,
	ZE_IBS_PRESSED,
	ZE_IBS_RELEASED
};

enum ZEInputAxisSign
{
	ZE_IAS_POSITIVE,
	ZE_IAS_NEGATIVE,
	ZE_IAS_BOTH
};

enum ZEInputState
{
	ZE_IS_PRESSED,
	ZE_IS_RELEASED,
	ZE_IS_PRESSING,
	ZE_IS_CHANGED,
	ZE_IS_VALUE
};

enum ZEInputType
{
	ZE_IT_NONE,
	ZE_IT_BUTTON,
	ZE_IT_AXIS,
	ZE_IT_POV,
	ZE_IT_SWITCH,
	ZE_IT_VECTOR,
	ZE_IT_QUATERNION,
	ZE_IT_CURSOR,
	ZE_IT_TEXT
};

class ZEInputValue
{
	public:
		union
		{
			ZEInputState			ButtonState;
			float					AxisValue;
			float					POVValue;
			ZEInt					SwitchValue;
		};

		ZEVector4					Vector;
		ZEQuaternion				Quaternion;
		ZEVector2					Cursor;
		ZEUInt						Character;
		ZEINCharacterModifiers		CharacterModifiers;
};

class ZEInputDevice;

class ZE_EXPORT_ZEENGINE ZEInput
{
	private:
		ZEInputDevice*			Device;
		ZEString				DeviceName;
		ZEUInt					DeviceNameHash;
		ZEUInt32				Index;
		ZEInputType				Type;
		ZEInputAxisSign			AxisSign;

		bool					ConnectToDevice();
		static bool				ParseInputBinding(ZEString InputString, ZEInput& Input);

	public:
		const char*				GetDeviceName() const;
		ZEInputDevice*			GetDevice() const;
		ZEUInt32				GetIndex() const;
		ZEInputType				GetType() const;
		ZEInputAxisSign			GetAxisSign() const;
		ZEString				GetInputString() const;

		bool					IsAvailable() const;

		bool					Check(ZEInputState State) const;
		bool					Check(ZEInputState State, ZEInputValue& Value) const;

								ZEInput();

		static ZEInput			Create(const ZEString& InputString);
		static ZEInput			CreateButton(const ZEString& DeviceName, ZEUInt32 Index);
		static ZEInput			CreateAxis(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign = ZE_IAS_BOTH);
		static ZEInput			CreatePOV(const ZEString& DeviceName, ZEUInt32 Index);
		static ZEInput			CreateSwitch(const ZEString& DeviceName, ZEUInt32 Index);
		static ZEInput			CreateVector(const ZEString& DeviceName, ZEUInt32 Index);
		static ZEInput			CreateQuaternion(const ZEString& DeviceName, ZEUInt32 Index);
		static ZEInput			CreateCursor(const ZEString& DeviceName);
		static ZEInput			CreateText(const ZEString& DeviceName);
};

#endif
