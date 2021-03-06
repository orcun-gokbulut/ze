//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundModule.h
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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEOptionSection.h"

class ZESoundSource;
class ZEListener;
class ZESoundResource;

#define zeSound ZESoundModule::GetInstance()

enum ZESpeakerLayout
{
	ZE_SL_AUTOMATIC				= 0,
	ZE_SL_DIRECT_OUT			= 1,
	ZE_SL_MONO					= 2,
	ZE_SL_HEAD_PHONE			= 3,
	ZE_SL_STERIO_WIDE			= 4,
	ZE_SL_STERIO_NARROW			= 5,
	ZE_SL_STERIO_SURROUND		= 6,
	ZE_SL_4						= 7,
	ZE_SL_5_1					= 8,
	ZE_SL_5_1_SURROUND			= 9,
	ZE_SL_7_1					= 10,	
	ZE_SL_7_1_SURROUND			= 11,
};

enum ZESoundSourceType;

struct ZESoundDevice
{
	ZESize			DeviceId;
	ZEString		DeviceName;
	ZEString		DriverName;
};


class ZESoundModule : public ZEModule
{
	ZE_OBJECT
	protected:
		static ZEOptionSection					SoundOptions;

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

	public:
		static void								BaseInitialize();
		static void								BaseDeinitialize();

		virtual const ZEArray<ZESoundDevice>&	GetDeviceList() = 0;

		virtual void							SetSpeakerLayout(ZESpeakerLayout Layout) = 0;
		virtual ZESpeakerLayout					GetSpeakerLayout() = 0;

		virtual void							SetMasterVolume(float Volume) = 0;
		virtual float 							GetMasterVolume() = 0;

		virtual void							ProcessSound(float ElapsedTime) = 0;

		virtual void							PlaySound(ZESoundResource* SoundResource) = 0;

		virtual void							SetStreamingDisabled(bool Disabled) = 0;
		virtual bool							GetStreamingDisabled() = 0;

		virtual void							SetMaxBufferSize(ZESize BufferSize) = 0; 
		virtual ZESize							GetMaxBufferSize() = 0;

		virtual void							SetTypeVolume(ZESoundSourceType Type, float Volume) = 0;
		virtual float							GetTypeVolume(ZESoundSourceType Type) = 0;

		virtual	void							SetActiveListener(ZEListener* NewListener) = 0;
		virtual ZEListener*						GetActiveListener() = 0;

		virtual void							OptionsChanged();

		virtual ZESoundSource*					CreateSoundSource() = 0;
		virtual ZEListener*						CreateListener() = 0;

		static ZESoundModule*					GetInstance();
};
