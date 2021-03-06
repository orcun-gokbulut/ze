//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALModule.h
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
#ifndef	__ZE_AL_MODULE_H__
#define __ZE_AL_MODULE_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZESound/ZESoundModule.h"
#include "ZESound/ZESoundSource.h"

class ZEALSoundSource;
class ZEALListener;
struct ALCdevice_struct;
struct ALCcontext_struct;

enum ZESoundSourceType;

#undef PlaySound
class ZEALModule : public ZESoundModule
{
	ZE_OBJECT
	friend class ZEALSoundListener;
	friend class ZEALSoundSource;
	private:
		ZEArray<ZESoundDevice>					DeviceList;

		float									UpdateTime;

		ALCdevice_struct*						Device;
		ALCcontext_struct*						Context;

		float									MasterVolume;
		float									TypeVolumes[ZE_SS_MAX_TYPE];

		bool									StreamingDisabled;
		ZESize									MaxBufferSize;

		ZEALListener*							ActiveListener;
		ZESmartArray<ZEALListener*>				Listeners;
		ZESmartArray<ZEALSoundSource*>			SoundSources;

		void									UpdateVolumes(ZESoundSourceType SourceType);
		void									UpdateStreams();

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

												ZEALModule();
		virtual									~ZEALModule();

	public:
		virtual const ZEArray<ZESoundDevice>&	GetDeviceList();

		ALCdevice_struct*						GetDevice();
		ALCcontext_struct*						GetContext();

		virtual void							SetSpeakerLayout(ZESpeakerLayout Layout);
		virtual ZESpeakerLayout					GetSpeakerLayout();

		virtual void							SetMasterVolume(float Volume);
		virtual float							GetMasterVolume();

		virtual void							SetTypeVolume(ZESoundSourceType Type, float Volume);
		virtual float							GetTypeVolume(ZESoundSourceType Type);

		virtual void							SetStreamingDisabled(bool Disabled);
		virtual bool							GetStreamingDisabled();

		virtual void							SetMaxBufferSize(ZESize BufferSize); 
		virtual ZESize							GetMaxBufferSize();

		virtual void							ProcessSound(float ElapsedTime);

		virtual void							PlaySound(ZESoundResource* SoundResource);

		virtual	void							SetActiveListener(ZEListener* NewListener);
		virtual ZEListener*						GetActiveListener();

		virtual ZESoundSource*					CreateSoundSource();
		virtual ZEListener*						CreateListener();

		static ZEALModule*						CreateInstance();
};		
#endif
