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
#include "ZEMath/ZEVector.h"
#include "ZECore/ZECore.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEALListener.h"
#include "ZEALSoundSource.h"
#include "ZEALSoundSource3D.h"

#define MAX_SOUNDBUFFER_COUNT	256
#define MapVector3(A, B)		(A).x = (B).x; (A).y = (B).y; (A).z = (B).z

const ZEArray<ZESoundDevice>& ZEALModule::GetDeviceList()
{
	return DeviceList;
}

void ZEALModule::UpdateVolumes(ZESoundSourceType SourceType)
{
	for (ZESize I = 0; I < SoundSources.GetCount(); I++)
		if (SoundSources[I]->GetSoundSourceType() == SourceType)
			SoundSources[I]->SetVolume(SoundSources[I]->GetVolume());

	for (ZESize I = 0; I < SoundSources3D.GetCount(); I++)
		if (SoundSources3D[I]->GetSoundSourceType() == SourceType)
			SoundSources3D[I]->SetVolume(SoundSources[I]->GetVolume());

}

void ZEALModule::UpdateStreams()
{
	for (ZESize I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->CreateBuffer();

	for (ZESize I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources[I]->CreateBuffer();
}

ZEALModule::ZEALModule()
{
	UpdateTime = 0.2f;
}

ZEALModule::~ZEALModule()
{
}

ALCdevice* ZEALModule::GetDevice()
{
	return Device;
}

ALCcontext* ZEALModule::GetContext()
{
	return Context;
}

bool ZEALModule::InitializeInternal()
{	
	zeLog("Initializing OpenAL module.");

	if (!ZESoundModule::InitializeInternal())
		return false;
	

	zeLog("Opening device.");
	Device = alcOpenDevice(NULL); // select the "preferred device"
	if (Device == NULL) 
	{
		zeError("Can not open OpenAL device.");
		return false;
	}

	Context = alcCreateContext(Device, NULL);
	alcMakeContextCurrent(Context);

	ZEALComponentBase::BaseInitialize(this);

	for(ZESize I = 0; I < ZE_SS_MAX_TYPE; I++)
	{
		TypeVolumes[I] = 100;
	}

	SetStreamingDisabled(SoundOptions.GetOption("StreamingDisabled")->GetValue().GetBoolean());
	SetMaxBufferSize((ZESize)SoundOptions.GetOption("MaxBufferSize")->GetValue().GetInt32());
	SetMasterVolume(SoundOptions.GetOption("MasterVolume")->GetValue().GetInt32());
	SetSpeakerLayout((ZESpeakerLayout)SoundOptions.GetOption("SpeakerLayout")->GetValue().GetInt32());
	SetTypeVolume(ZE_SST_EFFECT, SoundOptions.GetOption("EffectVolume")->GetValue().GetInt32());
	SetTypeVolume(ZE_SST_DIALOG, SoundOptions.GetOption("DialogVolume")->GetValue().GetInt32());
	SetTypeVolume(ZE_SST_MUSIC, SoundOptions.GetOption("MusicVolume")->GetValue().GetInt32());
	SetTypeVolume(ZE_SST_VIDEO, SoundOptions.GetOption("VideoVolume")->GetValue().GetInt32());
	SetTypeVolume(ZE_SST_PLAYER_COMM, SoundOptions.GetOption("PlayerCommVolume")->GetValue().GetInt32());

	zeLog("OpenAL module initialized.");

	return true;
}

bool ZEALModule::DeinitializeInternal()
{	
	zeLog("Destroying OpenAL.");

	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);

	return ZESoundModule::DeinitializeInternal();
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

void ZEALModule::SetMasterVolume(ZEUInt Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		MasterVolume = ZE_SS_VOLUME_MAX;
	else
		MasterVolume = Volume;
	
	alListenerf(AL_GAIN, MasterVolume / 100.0f);
}

ZEUInt ZEALModule::GetMasterVolume()
{
	return MasterVolume;
}

void ZEALModule::SetTypeVolume(ZESoundSourceType Type, ZEUInt Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;
	else
		TypeVolumes[Type] = Volume;

	UpdateVolumes(Type);
}

ZEUInt ZEALModule::GetTypeVolume(ZESoundSourceType Type)
{
	zeDebugCheck(Type >= 256, "Sound source types are limited to 256");
	return TypeVolumes[Type];
}


void ZEALModule::SetMaxBufferSize(ZESize BufferSize)
{
	MaxBufferSize = BufferSize;
}

ZESize ZEALModule::GetMaxBufferSize()
{
	return MaxBufferSize;
}

void ZEALModule::ProcessSound(float ElapsedTime)
{
	for (ZESize I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->Update(ElapsedTime);

	for (ZESize I = 0; I < SoundSources3D.GetCount(); I++)
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

ZEALModule* ZEALModule::CreateInstance()
{
	return new ZEALModule();
}
