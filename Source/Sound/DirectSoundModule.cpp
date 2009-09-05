//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DirectSoundModule.cpp
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

#include "DirectSoundModule.h"
#include "ZEDS/ZEDS.h"
#include "ZEMath/ZEMath.h"
#include "Core/Core.h"
#include <dsound.h>


#define MAX_SOUNDBUFFER_COUNT	256

LPDIRECTSOUND8				DS		      = NULL;
LPDIRECTSOUNDBUFFER			DSPrimary     = NULL;
LPDIRECTSOUND3DLISTENER		DSListener    = NULL;


LPDIRECTSOUNDNOTIFY				DSNotify ; 
DSBPOSITIONNOTIFY				PositionNotify[2];
ZEArray<LPDIRECTSOUNDBUFFER>	DSBuffers;
ZEArray<LPDIRECTSOUND3DBUFFER>	DS3DBuffers;
HANDLE							NotifyEvent[2];
	

#define MapVector3(A, B)		(A).x = (B).x; (A).y = (B).y; (A).z = (B).z


ZEModuleType ZEDirectSoundModuleDescription::GetType()
{
	return ZE_MT_SOUND;
}

ZEModuleAttribute ZEDirectSoundModuleDescription::GetAttributes()
{
	return ZE_MA_DEBUG;
}

int ZEDirectSoundModuleDescription::GetRequiredZinekEngineVersion()
{
	return 0;
}

int ZEDirectSoundModuleDescription::GetMajorVersion()
{
	return 0;
}

int ZEDirectSoundModuleDescription::GetMinorVersion()
{
	return 4;
}

const char* ZEDirectSoundModuleDescription::GetCopyright()
{
	return "Copyright(c) 2007-2008, Zinek Engine Group. All rights reserved.";
}

const char* ZEDirectSoundModuleDescription::GetName()
{
	return "DirectSound";
}

ZEOptionSection* ZEDirectSoundModuleDescription::GetOptions()
{
	return NULL;
}

ZEModule* ZEDirectSoundModuleDescription::CreateModuleInstance()
{
	return new ZEDirectSoundModule;
}

bool ZEDirectSoundModuleDescription::CheckCompatible()
{
	return true;
}

ZEModuleDescription* ZEDirectSoundModule::GetModuleDescription()
{
	static ZEDirectSoundModuleDescription Desc;
	return &Desc;
}

bool ZEDirectSoundModule::IsEnabled()
{
	return Enabled;
}

void ZEDirectSoundModule::SetEnabled(bool Enabled)
{
	this->Enabled = true;
}

bool ZEDirectSoundModule::Initialize()
{	
	zeLog("Initializing DirectSound.\r\n");
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
	PrimaryBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;


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
	
	DSBuffers.SetCount(MAX_SOUNDBUFFER_COUNT);
	DSBuffers.FillWith(NULL);
	DS3DBuffers.SetCount(MAX_SOUNDBUFFER_COUNT);
	DS3DBuffers.FillWith(NULL);
	return true;
}

void ZEDirectSoundModule::Deinitialize()
{	
	zeLog("Destroying DirectSound.\r\n");
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

ZEDirectSoundModule::~ZEDirectSoundModule()
{
	Destroy();
}

int ZEDirectSoundModule::CreateBuffer(ZESoundSource* Source, ZESoundResource* SoundResource)
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

void ZEDirectSoundModule::ReleaseBuffer(ZESoundSource* Source)
{
	if (DSBuffers[Source->GetBufferId()] != NULL)
	{
		DSBuffers[Source->GetBufferId()]->Release();
		DSBuffers[Source->GetBufferId()] = NULL;
		if (DS3DBuffers[Source->GetBufferId()] != NULL)
		{
			DS3DBuffers[Source->GetBufferId()]->Release();
			DS3DBuffers[Source->GetBufferId()] = NULL;
		}
	}
} 

void ZEDirectSoundModule::Play(ZESoundSource* Source)
{	
	DSBuffers[Source->GetBufferId()]->Play(0,0,0);
}

void ZEDirectSoundModule::Stop(ZESoundSource* Source)
{
	DSBuffers[Source->GetBufferId()]->Stop();
	DSBuffers[Source->GetBufferId()]->SetCurrentPosition(0);
}

void ZEDirectSoundModule::Pause(ZESoundSource* Source)
{
	DSBuffers[Source->GetBufferId()]->Stop();
}

void ZEDirectSoundModule::PlayLoop(ZESoundSource* Source)
{
	DSBuffers[Source->GetBufferId()]->Play(0,0,DSBPLAY_LOOPING);
}

void ZEDirectSoundModule::UpdateSource3D(ZESoundSource3D* Source3D)
{
	DS3DBUFFER DS3D;
	LPDIRECTSOUND3DBUFFER Current = DS3DBuffers[Source3D->GetBufferId()];
	
	DS3D.dwSize = sizeof(DS3DBUFFER);
	ZEVector3 Temp;
	Temp = Source3D->GetLocalPosition();
	MapVector3(DS3D.vPosition, Temp);

	Temp = Source3D->GetOwner()->GetVelocity();
	MapVector3(DS3D.vVelocity, Temp);

	ZEQuaternion::Transform(Temp, ZEVector3(0,0,1), Source3D->GetWorldRotation());
	MapVector3(DS3D.vConeOrientation, Temp);

	DS3D.dwInsideConeAngle = Source3D->GetInsideConeAngle();
	DS3D.dwOutsideConeAngle = Source3D->GetOutsideConeAngle();
	DS3D.lConeOutsideVolume = Source3D->GetConeOutsideVolume();

	DS3D.flMaxDistance = Source3D->GetMaxDistance();
	DS3D.flMinDistance = Source3D->GetMinDistance();

	DS3D.dwMode = DS3DMODE_NORMAL;

	Current->SetAllParameters(&DS3D, DS3D_DEFERRED);
}

void ZEDirectSoundModule::UpdateSource(ZESoundSource* Source)
{
	LPDIRECTSOUNDBUFFER Current = DSBuffers[Source->GetBufferId()];
	Current->SetCurrentPosition(Source->GetCursor());
	Current->SetFrequency(Source->GetFrequency());
	Current->SetPan(Source->GetPan());
	Current->SetVolume(Source->GetVolume());
}
		
void ZEDirectSoundModule::ProcessSound()
{
	static int FrameDrop = 0;

	if (DSListener != NULL)
	{
		if (FrameDrop == 0)
		{
			FrameDrop = 5;
			DSListener->CommitDeferredSettings();
		}
		FrameDrop--;
	}
}


void ZEDirectSoundModule::PauseAllSounds()
{
	for (size_t i=0; i<DSBuffers.GetCount(); i++)
		DSBuffers[i]->Stop();
}

void ZEDirectSoundModule::StopAllSounds()
{
	for (size_t i=0; i<DSBuffers.GetCount(); i++)
	{
		DSBuffers[i]->Stop();
		DSBuffers[i]->SetCurrentPosition(0);
	}
}

void ZEDirectSoundModule::ReloadBuffer(ZESoundSource* Source)
{
	DSBuffers[Source->GetBufferId()]->Restore();
}

void ZEDirectSoundModule::ReloadAllBuffers()
{
	for (size_t i=0; i<DSBuffers.GetCount(); i++)
	
		DSBuffers[i]->Restore();
}

void ZEDirectSoundModule::UpdateListener(ZEListener* NewListener)
{
	DS3DLISTENER Listener;
	ZEQuaternion Rotation = NewListener->GetWorldRotation();

	ZEVector3		Temp;
	Listener.dwSize = sizeof(DS3DLISTENER);
	
	Temp = NewListener->GetWorldPosition();
	MapVector3(Listener.vPosition, Temp);

	Temp = NewListener->GetOwner()->GetVelocity();
	MapVector3(Listener.vVelocity, Temp);

	ZEQuaternion::Transform(Temp, ZEVector3(0, 0, 1), Rotation);
	MapVector3(Listener.vOrientFront, Temp);

	ZEQuaternion::Transform(Temp, ZEVector3(0, 1, 0), Rotation);
	MapVector3(Listener.vOrientTop, Temp);

	Listener.flDistanceFactor = NewListener->GetDistanceFactor();
	Listener.flDopplerFactor  = NewListener->GetDopplerFactor();
	Listener.flRolloffFactor  = NewListener->GetRollOffFactor();
	
	DSListener->SetAllParameters(&Listener,DS3D_DEFERRED);

}
void ZEDirectSoundModule::SetListener(ZEListener* NewListener)
{
	UpdateListener(NewListener);
}
