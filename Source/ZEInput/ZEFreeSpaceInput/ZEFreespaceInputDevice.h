//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFreespaceInputDevice.h
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
#ifndef	__ZE_FREESPACE_INPUT_DEVICE_H__
#define __ZE_FREESPACE_INPUT_DEVICE_H__

#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEInput\ZEInputDevice.h"

#include <Freespace\freespace.h>

class ZEFreespaceInputDevice : public ZEInputDevice
{
	friend class ZEFreespaceInputDeviceExtension;
	private:
		FreespaceDeviceId				Device;
		FreespaceDeviceInfo				DeviceInfo;
		unsigned int					DeviceIndex;

		float							Axises[3];
		bool							Buttons[5], OldButtons[5];
		ZEVector3						LinearPosition;
		ZEQuaternion					Orientation;

	public:
		virtual unsigned int			GetDeviceId();
		virtual const char*				GetDeviceName();
		virtual unsigned int			GetDeviceIndex();

		virtual const 
		ZEArray<ZEInputDescription>&	GetInputDescriptions();

		virtual ZEInputEvent			GetCurrentInputEvents();

		virtual void					Acquire();
		virtual void					UnAcuire();

		virtual bool					Initialize();
		virtual void					Deinitialize();

		virtual void					ProcessInputs();

		virtual bool					ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* InputAction);
};

#endif