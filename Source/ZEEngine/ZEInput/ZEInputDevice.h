//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputDevice.h
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
#ifndef	__ZE_INPUT_DEVICE_H__
#define __ZE_INPUT_DEVICE_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"

enum ZEInputDeviceLifeState
{
	ZE_IDLS_NOT_INITIALIZED				= 0,
	ZE_IDLS_INITIALIZING				= 1,
	ZE_IDLS_INITIALIZED					= 2,
	ZE_IDLS_DEINITIALIZING				= 3
};

enum ZEInputDeviceType
{
	ZE_IDT_NONE,
	ZE_IDT_MOUSE,
	ZE_IDT_KEYBOARD,
	ZE_IDT_JOYSTICK,
	ZE_IDT_GAMEPAD,
	ZE_IDT_WHEEL,
	ZE_IDT_SENSOR,
	ZE_IDT_OTHER
};

class ZEInputDeviceDescription
{
	public:
		ZEString							Name;
		ZEUInt								NameHash;
		ZEUInt								Index;
		ZEInputDeviceType					Type;

		ZEString							FullName;

		ZEString							SinkName;
		ZEUInt								SinkNameHash;
		bool								Sink;

		ZESize								ButtonCount;
		ZESize								AxisCount;
		ZESize								POVCount;
		ZESize								SwitchCount;
		ZESize								VectorCount;
		ZESize								QuaternionCount;

		void								Clear();

											ZEInputDeviceDescription();
};

class ZEInputDeviceState
{
	public:
		struct  
		{
			ZEArray<bool>					OldValues;
			ZEArray<bool>					CurrentValues;

		} Buttons;

		struct  
		{
			ZEArray<float>					OldValues;
			ZEArray<float>					CurrentValues;
		} Axises;

		struct  
		{
			ZEArray<ZEInt>					OldValues;
			ZEArray<ZEInt>					CurrentValues;
		} POVs;

		struct  
		{
			ZEArray<ZEUInt>					OldValues;
			ZEArray<ZEUInt>					CurrentValues;
		} Switches;

		struct  
		{
			ZEArray<ZEVector4>				OldValues;
			ZEArray<ZEVector4>				CurrentValues;
		} Vectors;

		struct  
		{
			ZEArray<ZEQuaternion>			OldValues;
			ZEArray<ZEQuaternion>			CurrentValues;
		} Quaternions;

		void								Initialize(const ZEInputDeviceDescription& Description);
		void								Reset();
		void								Advance();
		void								AdvanceAndReset();
		void								Clear();
};

class ZEInputDeviceIndexes
{
	private:
		static ZESize							KeyboardIndex;
		static ZESize							MouseIndex;
		static ZESize							JoystickIndex;
		static ZESize							GamepadIndex;
		static ZESize							WheelIndex;
		static ZESize							SensorIndex;
		static ZESize							OtherIndex;

	public:
		static ZESize							GetNewDeviceIndex(ZEInputDeviceType Type);
};

class ZEInputDevice
{
	private:
		bool									Enabled;
		bool									Acquired;
		ZEInputDeviceLifeState					LifeState;

	protected:
		ZEInputDeviceState						State;
		ZEInputDeviceDescription				Description;

		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

	public:
		const ZEString&							GetName();
		virtual const ZEInputDeviceDescription&	GetDescription();

		virtual void							SetEnabled(bool Enabled);
		bool									GetEnabled();

		bool									IsAcquired();
		virtual void							Acquire();
		virtual void							UnAcquire();

		bool									IsInitialized();
		bool									Initialize();
		bool									Deinitialize();

		const ZEInputDeviceState&				GetState();

		virtual void							Destroy();

												ZEInputDevice();
		virtual									~ZEInputDevice();
};

#endif
