//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DSModule.cpp
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

#include "DSModule.h"
#include "DSModuleDescription.h"
#include "ZEMath/Vector.h"
#include "Core/Core.h"
#include "Core/Error.h"
#include "Core/Console.h"
#include "Core/Window.h"
#include "DSListener.h"
#include "DSSoundSource.h"
#include "DSSoundSource3D.h"
#include <dsound.h>

#define MAX_SOUNDBUFFER_COUNT	256
#define MapVector3(A, B)		(A).x = (B).x; (A).y = (B).y; (A).z = (B).z

void ZEDSModule::UpdateVolumes(ZESoundSourceType SourceType)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		if (SoundSources[I]->GetSoundSourceType() == SourceType)
			SoundSources[I]->SetVolume(SoundSources[I]->GetVolume());

/*	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		if (SoundSources3D[I]->GetSoundSourceType() == SourceType)
			SoundSources3D[I]->SetVolume(SoundSources[I]->GetVolume());
*/
}

void ZEDSModule::UpdateStreams()
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->CreateBuffer(false);

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources[I]->CreateBuffer(true);
}

ZEDSModule::ZEDSModule()
{
	UpdateTime = 0.2f;
	DS = NULL;
	DSPrimary = NULL;
	DSListener = NULL;
}

ZEDSModule::~ZEDSModule()
{
}

ZEModuleDescription* ZEDSModule::GetModuleDescription()
{
	static ZEDSModuleDescription Desc;
	return &Desc;
}

LPDIRECTSOUND8 ZEDSModule::GetDevice()
{
	return DS;
}

LPDIRECTSOUND3DLISTENER8 ZEDSModule::GetListener()
{
	return DSListener;
}

LPDIRECTSOUNDBUFFER ZEDSModule::GetPrimaryBuffer()
{
	return DSPrimary;
}

bool ZEDSModule::IsEnabled()
{
	return Enabled;
}

void ZEDSModule::SetEnabled(bool Enabled)
{
	this->Enabled = true;
}

bool ZEDSModule::Initialize()
{	
	zeOutput("Initializing DirectSound.\r\n");
	HRESULT hr;
	
	hr = DirectSoundCreate8(NULL, &DS, NULL);
	if (FAILED(hr))
	{	
		Destroy();
		zeError("DirectSound Module", "No DirectSound.");
		return false;
	}

	hr=DS->SetCooperativeLevel((HWND)zeWindow->GetHandle(), DSSCL_PRIORITY);
	
	if (FAILED(hr))
	{	
		Destroy();
		zeError("DirectSound Module", "Can not set DirectSound device cooperative level.");
		return false;
	}
	
	DSBUFFERDESC PrimaryBufferDesc;
	ZeroMemory(&PrimaryBufferDesc, sizeof(DSBUFFERDESC));
	PrimaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	PrimaryBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;

	hr = DS->CreateSoundBuffer(&PrimaryBufferDesc, &DSPrimary, NULL);
	if (FAILED(hr))
	{
		Destroy();
		zeError("DirectSound Module", "Can create direct sound primary buffer.");
		return false;
	}

	hr = DSPrimary->QueryInterface(IID_IDirectSound3DListener8, (VOID**)&DSListener);
	if (FAILED(hr))
	{
		Destroy();
		zeError("DirectSound Module", "Can not create a Direct Sound listener.");
		return false;
	}

	ZEDSComponentBase::BaseInitialize(this);

	memset(TypeVolumes, 100, sizeof(TypeVolumes));
	SetStreamingDisabled(SoundOptions.GetOption("StreamingDisabled")->GetValue().GetBoolean());
	SetMaxBufferSize(SoundOptions.GetOption("MaxBufferSize")->GetValue().GetInteger());
	SetMasterVolume(SoundOptions.GetOption("MasterVolume")->GetValue().GetInteger());
	SetSpeakerLayout((ZESpeakerLayout)SoundOptions.GetOption("SpeakerLayout")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_EFFECT, SoundOptions.GetOption("EffectVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_DIALOG, SoundOptions.GetOption("DialogVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_MUSIC, SoundOptions.GetOption("MusicVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_VIDEO, SoundOptions.GetOption("VideoVolume")->GetValue().GetInteger());
	SetTypeVolume(ZE_SST_PLAYER_COMM, SoundOptions.GetOption("PlayerCommVolume")->GetValue().GetInteger());

	return true;
}

void ZEDSModule::Deinitialize()
{	
	zeOutput("Destroying DirectSound.\r\n");
	if (DSListener != NULL)
	{
		DSListener->Release();
		DSListener = NULL;
	}

	if (DSPrimary != NULL)
	{
		DSPrimary->Release();
		DSPrimary = NULL;
	}

	if (DS != NULL)
	{
		DS->Release();
		DS = NULL;
	}
}

void ZEDSModule::SetSpeakerLayout(ZESpeakerLayout Layout)
{
	// Dangerous in DirectSound. 
}

ZESpeakerLayout ZEDSModule::GetSpeakerLayout()
{
	return ZE_SL_AUTOMATIC;
}

void ZEDSModule::SetStreamingDisabled(bool Disabled)
{
	StreamingDisabled = Disabled;
	UpdateStreams();
}

bool ZEDSModule::GetStreamingDisabled()
{
	return StreamingDisabled;
}

void ZEDSModule::SetMasterVolume(unsigned int Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		MasterVolume = ZE_SS_VOLUME_MAX;
	else
		MasterVolume = Volume;
	
	//DSPrimary->SetVolume(((MasterVolume - ZE_SS_VOLUME_MIN) * (DSBVOLUME_MAX - DSBVOLUME_MIN)) / (ZE_SS_VOLUME_MAX - ZE_SS_VOLUME_MIN) + DSBVOLUME_MIN);
	DSPrimary->SetVolume(log10f((float)MasterVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f);
}

unsigned int ZEDSModule::GetMasterVolume()
{
	return MasterVolume;
}

void ZEDSModule::SetTypeVolume(ZESoundSourceType Type, unsigned int Volume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;
	else
		Volume = Volume;

	TypeVolumes[Type] = Volume;

	UpdateVolumes(Type);
}

unsigned int ZEDSModule::GetTypeVolume(ZESoundSourceType Type)
{
	ZEASSERT(Type >= 256, "Sound source types are limited to 256");
	return TypeVolumes[Type];
}


void ZEDSModule::SetMaxBufferSize(unsigned int BufferSize)
{
	MaxBufferSize = BufferSize;
}

unsigned int ZEDSModule::GetMaxBufferSize()
{
	return MaxBufferSize;
}

void ZEDSModule::ProcessSound(float ElapsedTime)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->Update(ElapsedTime);

	/*for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources3D[I]->Update(ElapsedTime);*/
	
	if (UpdateTime  < 0.0f)
	{
		DSListener->CommitDeferredSettings();
		UpdateTime = 0.2f;
	}
	UpdateTime -= ElapsedTime;
}

void ZEDSModule::PlaySound(ZESoundResource* SoundResource)
{
}

void ZEDSModule::SetActiveListener(ZEListener* Listener)
{
	ActiveListener = (ZEDSListener*)Listener;
	ActiveListener->Update();
}

ZEListener* ZEDSModule::GetActiveListener()
{
	return ActiveListener;
}

ZESoundSource* ZEDSModule::CreateSoundSource()
{
	ZEDSSoundSource* NewSoundSource = new ZEDSSoundSource();
	SoundSources.Add(NewSoundSource);
	return NewSoundSource;
}

ZESoundSource3D* ZEDSModule::CreateSoundSource3D()
{
	return new ZEDSSoundSource3D();
}

ZEListener* ZEDSModule::CreateListener()
{
	return new ZEDSListener();
}

/*
ZEDSModule::~ZEDSModule()
{
	Deinitialize();
}

int ZEDSModule::CreateBuffer(ZESoundSource* Source, ZESoundResource* SoundResource)
{
	HRESULT hr;
	if (SoundResource->ChannelCount != 1 && Source->Is3D())
	{
		zeError("DirectSound Module", "3D sound source's resource must be mono.");
		return NULL;
	}
	
	int I, BufferId;
	for (I = 0; I < DSBuffers.GetCount(); I++)
		if (DSBuffers[I] == NULL)
		{
			BufferId = I;
			break;
		}
	if (I == MAX_SOUNDBUFFER_COUNT)
	{
		zeError("DirectSound Module", "Can not create sound buffer becouse max sound buffer limit reached.");
		return -1;
	}


	WAVEFORMATEX Format;
	memset(&Format, 0, sizeof(WAVEFORMATEX)); 
	Format.cbSize = sizeof(WAVEFORMATEX);
	Format.wFormatTag = WAVE_FORMAT_PCM; 
	Format.nChannels = SoundResource->ChannelCount; 
	Format.nSamplesPerSec = SoundResource->SamplesPerSecond;
	Format.nBlockAlign = SoundResource->BlockAlign;
	Format.wBitsPerSample = SoundResource->BitsPerSample;
	Format.nAvgBytesPerSec = Format.nSamplesPerSec * Format.nBlockAlign; 

	DSBUFFERDESC DSBufferDesc;
	memset(&DSBufferDesc, 0, sizeof(DSBUFFERDESC)); 
	DSBufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	DSBufferDesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | (Source->Is3D() ? DSBCAPS_CTRL3D : DSBCAPS_CTRLPAN) | DSBCAPS_CTRLPOSITIONNOTIFY;
	DSBufferDesc.dwBufferBytes = SoundResource->BufferSize; 
	DSBufferDesc.lpwfxFormat = &Format;
	DSBufferDesc.guid3DAlgorithm = (Source->Is3D() ? DS3DALG_HRTF_FULL : GUID_NULL);

	LPDIRECTSOUNDBUFFER *DSBuffer = &DSBuffers[BufferId];
	if (FAILED(hr = DS->CreateSoundBuffer(&DSBufferDesc, DSBuffer, NULL)))
	{
		zeError("DirectSound Module", "Can not create Direct Sound buffer.");
		return -1;
	}
	
	else
	{	
		DS3DBuffers[BufferId] = NULL;
		if (Source->Is3D())
		{
			if ((*DSBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID *)&DS3DBuffers[BufferId]) != DS_OK)
			{
				zeError("DirectSound Module", "Can not create 3D Direct Sound buffer.");
				(*DSBuffer)->Release();
				(*DSBuffer) = NULL;
				return -1;
			}
		}

		void *Buffer1, *Buffer2;
		DWORD BufferSize1, BufferSize2;
		(*DSBuffer)->Lock(0, DSBufferDesc.dwBufferBytes, &Buffer1, &BufferSize1, &Buffer2, &BufferSize2, DSBLOCK_ENTIREBUFFER);
			memcpy(Buffer1, SoundResource->Buffer, (BufferSize1 < SoundResource->BufferSize ? BufferSize1 : SoundResource->BufferSize));
			if (BufferSize2 != 0)
				memcpy(Buffer2, SoundResource->Buffer + BufferSize1, SoundResource->BufferSize - BufferSize1);
		(*DSBuffer)->Unlock(Buffer1, BufferSize1, Buffer2, BufferSize2);
	}
	return BufferId;
}

void ZEDSModule::UpdateSource3D(ZESoundSource3D* Source3D)
{
	DS3DBUFFER DS3D;
	LPDIRECTSOUND3DBUFFER Current = DS3DBuffers[Source3D->GetBufferId()];
	
	DS3D.dwSize = sizeof(DS3DBUFFER);
	ZEVector3 Temp;
	Temp = Source3D->GetLocalPosition();
	MapVector3(DS3D.vPosition, Temp);

	Temp = Source3D->GetOwner()->GetVelocity();
	MapVector3(DS3D.vVelocity, Temp);

	ZEQuaternion::VectorProduct(Temp, Source3D->GetWorldRotation(), ZEVector3(0,0,1));
	MapVector3(DS3D.vConeOrientation, Temp);

	DS3D.dwInsideConeAngle = Source3D->GetInsideConeAngle();
	DS3D.dwOutsideConeAngle = Source3D->GetOutsideConeAngle();
	DS3D.lConeOutsideVolume = Source3D->GetConeOutsideVolume();

	DS3D.flMaxDistance = Source3D->GetMaxDistance();
	DS3D.flMinDistance = Source3D->GetMinDistance();

	DS3D.dwMode = DS3DMODE_NORMAL;

	Current->SetAllParameters(&DS3D, DS3D_DEFERRED);
}

void ZEDSModule::UpdateSource(ZESoundSource* Source)
{
	LPDIRECTSOUNDBUFFER Current = DSBuffers[Source->GetBufferId()];
	Current->SetCurrentPosition(Source->GetCursor());
	Current->SetFrequency(Source->GetFrequency());
	Current->SetPan(Source->GetPan());
	Current->SetVolume(Source->GetVolume());
}


void ZEDSModule::ReloadBuffer(ZESoundSource* Source)
{
	DSBuffers[Source->GetBufferId()]->Restore();
}

void ZEDSModule::ReloadAllBuffers()
{
	for (size_t i=0; i<DSBuffers.GetCount(); i++)
	
		DSBuffers[i]->Restore();
}

void ZEDSModule::UpdateListener(ZEListener* NewListener)
{
	DS3DLISTENER Listener;
	ZEQuaternion Rotation = NewListener->GetWorldRotation();

	ZEVector3		Temp;
	Listener.dwSize = sizeof(DS3DLISTENER);
	
	Temp = NewListener->GetWorldPosition();
	MapVector3(Listener.vPosition, Temp);

	Temp = NewListener->GetOwner()->GetVelocity();
	MapVector3(Listener.vVelocity, Temp);

	ZEQuaternion::VectorProduct(Temp, Rotation, ZEVector3(0, 0, 1));
	MapVector3(Listener.vOrientFront, Temp);

	ZEQuaternion::VectorProduct(Temp, Rotation, ZEVector3(0, 1, 0));
	MapVector3(Listener.vOrientTop, Temp);

	Listener.flDistanceFactor = NewListener->GetDistanceFactor();
	Listener.flDopplerFactor  = NewListener->GetDopplerFactor();
	Listener.flRolloffFactor  = NewListener->GetRollOffFactor();
	
	DSListener->SetAllParameters(&Listener,DS3D_DEFERRED);

}
void ZEDSModule::SetListener(ZEListener* NewListener)
{
	UpdateListener(NewListener);
}
*/
