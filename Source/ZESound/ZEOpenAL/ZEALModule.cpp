//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALModule.cpp
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

#include "ZEALModule.h"
#include "ZEALModuleDescription.h"
#include "ZEMath/ZEVector.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEWindow.h"
#include "ZEALListener.h"
#include "ZEALSoundSource.h"
#include "ZEALSoundSource3D.h"
#include <dsound.h>

#define MAX_SOUNDBUFFER_COUNT	256
#define MapVector3(A, B)		(A).x = (B).x; (A).y = (B).y; (A).z = (B).z

void ZEALModule::UpdateVolumes(ZESoundSourceType SourceType)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		if (SoundSources[I]->GetSoundSourceType() == SourceType)
			SoundSources[I]->SetVolume(SoundSources[I]->GetVolume());

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		if (SoundSources3D[I]->GetSoundSourceType() == SourceType)
			SoundSources3D[I]->SetVolume(SoundSources[I]->GetVolume());

}

void ZEALModule::UpdateStreams()
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->CreateBuffer();

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources[I]->CreateBuffer();
}

ZEALModule::ZEALModule()
{
	UpdateTime = 0.2f;
}

ZEALModule::~ZEALModule()
{
}

ZEModuleDescription* ZEALModule::GetModuleDescription()
{
	static ZEALModuleDescription Desc;
	return &Desc;
}

const ZEArray<ZESoundDevice>& ZEALModule::GetDeviceList()
{
	return DeviceList;
}

ALCdevice* ZEALModule::GetDevice()
{
	return Device;
}

ALCcontext* ZEALModule::GetContext()
{
	return Context;
}

bool ZEALModule::Initialize()
{	
	zeLog("OpenAL Module", "Initializing OpenAL module.");
	
	zeLog("OpenAL Module", "Enumurating Sound Devices.");
	const ALCchar* DeviceNames = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	if (DeviceNames == NULL)
	{
		zeError("OpenAL Module", "Can not enumurate devices.");
		return false;
	}

	size_t I = 0, Start = 0;
	while (true)
	{
		if (DeviceNames[I] == '\0')
		{
			ZESoundDevice* Device = DeviceList.Add();
			Device->DeviceId = DeviceList.GetCount();
			strncpy(Device->DeviceName, (char*)&DeviceNames[Start], ZE_MAX_DEVICE_NAME_SIZE);
			Device->DriverName[0] = '\0';

			zeLog("OpenAL Module", "Found sound device; "
				"Index : %d, "
				"Device Name: \"%s\", "
				"Device Driver: \"%s\".",
				Device->DeviceId,
				Device->DeviceName,
				Device->DriverName);

			if (DeviceNames[I] == '\0' && DeviceNames[I + 1] == '\0')
				break;
		}
		I++;
	}

	unsigned int DeviceId = SoundOptions.GetOption("DeviceId")->GetValue().GetInteger();

	ALchar* DeviceName = NULL;
	if (DeviceId > DeviceList.GetCount())
	{
		zeWarning("OpenAL Module", "Wrong device id. Using sound default device.");
	}
	else
		if (DeviceId == 0)
			zeLog("OpenAL Module", "Using default sound device");
		else
		{
			DeviceName = (ALchar*)(const char*)DeviceList[DeviceId].DeviceName;
			zeLog("OpenAL Module", "Using \"%s\" sound device.", DeviceName);
		}

	zeLog("OpenAL Module", "Opening device.");
	Device = alcOpenDevice(NULL); // select the "preferred device"
	if (Device == NULL) 
	{
		zeError("OpenAL Module", "Can not open OpenAL device.");
		return false;
	}

	Context = alcCreateContext(Device, NULL);
	alcMakeContextCurrent(Context);

	ZEALComponentBase::BaseInitialize(this);

	for(size_t I = 0; I < ZE_SS_MAX_TYPE; I++)
		TypeVolumes[I] = 100;

	SetStreamingDisabled(SoundOptions.GetOption("StreamingDisabled")->GetValue().GetBoolean());
	SetMaxBufferSize(SoundOptions.GetOption("MaxBufferSize")->GetValue().GetInteger());
	SetMasterVolume(SoundOptions.GetOption("MasterVolume")->GetValue().GetInteger());
	SetSpeakerLayout((ZESpeakerLayout)SoundOptions.GetOption("SpeakerLayout")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_EFFECT, SoundOptions.GetOption("EffectVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_DIALOG, SoundOptions.GetOption("DialogVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_MUSIC, SoundOptions.GetOption("MusicVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_VIDEO, SoundOptions.GetOption("VideoVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_PLAYER_COMM, SoundOptions.GetOption("PlayerCommVolume")->GetValue().GetInteger());

	zeLog("OpenAL Module", "OpenAL module initialized.");

	return ZESoundModule::Initialize();
}

void ZEALModule::Deinitialize()
{	
	zeLog("OpenAL Module", "Destroying OpenAL.");

	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);

	ZESoundModule::Deinitialize();
}

void ZEALModule::SetSpeakerLayout(ZESpeakerLayout Layout)
{
	// Not available in OpenAL
}

ZESpeakerLayout ZEALModule::GetSpeakerLayout()
{
	return ZE_SL_AUTOMATIC;
}

void ZEALModule::SetStreamingDisabled(bool Disabled)
{
	StreamingDisabled = Disabled;
	UpdateStreams();
}

bool ZEALModule::GetStreamingDisabled()
{
	return StreamingDisabled;
}

void ZEALModule::SetMasterVolume(unsigned int Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		MasterVolume = ZE_SS_VOLUME_MAX;
	else
		MasterVolume = Volume;
	
	alListenerf(AL_GAIN, MasterVolume / 100.0f);
}

unsigned int ZEALModule::GetMasterVolume()
{
	return MasterVolume;
}

void ZEALModule::SetTypeVolume(ZESoundSourceType Type, unsigned int Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;
	else
		TypeVolumes[Type] = Volume;

	UpdateVolumes(Type);
}

unsigned int ZEALModule::GetTypeVolume(ZESoundSourceType Type)
{
	zeAssert(Type >= 256, "Sound source types are limited to 256");
	return TypeVolumes[Type];
}


void ZEALModule::SetMaxBufferSize(unsigned int BufferSize)
{
	MaxBufferSize = BufferSize;
}

unsigned int ZEALModule::GetMaxBufferSize()
{
	return MaxBufferSize;
}

void ZEALModule::ProcessSound(float ElapsedTime)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->Update(ElapsedTime);

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources3D[I]->Update(ElapsedTime);
}

#undef PlaySound
void ZEALModule::PlaySound(ZESoundResource* SoundResource)
{

}

void ZEALModule::SetActiveListener(ZEListener* Listener)
{
	ActiveListener = (ZEALListener*)Listener;
	ActiveListener->ResetParameters();
}

ZEListener* ZEALModule::GetActiveListener()
{
	return ActiveListener;
}

ZESoundSource* ZEALModule::CreateSoundSource()
{
	ZEALSoundSource* NewSoundSource = new ZEALSoundSource();
	SoundSources.Add(NewSoundSource);
	return NewSoundSource;
}

ZESoundSource3D* ZEALModule::CreateSoundSource3D()
{
	ZEALSoundSource3D* NewSoundSource = new ZEALSoundSource3D();
	SoundSources3D.Add(NewSoundSource);
	return NewSoundSource;
	return NULL;
}

ZEListener* ZEALModule::CreateListener()
{
	return new ZEALListener();
}




