//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSModule.cpp
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

#include "ZEDSModule.h"
#include "ZEMath/ZEVector.h"
#include "ZECore/ZECore.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEWindow.h"
#include "ZEDSListener.h"
#include "ZEDSSoundSource.h"
#include "ZEDSSoundSource3D.h"
#include <dsound.h>

ZE_MODULE_DESCRIPTION(ZEDSModule, ZESoundModule, NULL)

#define MAX_SOUNDBUFFER_COUNT	256
#define MapVector3(A, B)		(A).x = (B).x; (A).y = (B).y; (A).z = (B).z

static BOOL CALLBACK DSEnumProc(LPGUID GUID, LPCTSTR DeviceName, LPCTSTR DriverName, LPVOID ModulePointer)
{
	if (GUID == NULL)
		return TRUE;

	ZEDSModule* Module = (ZEDSModule*)ModulePointer;

	ZESoundDevice* Device = Module->DeviceList.Add();
	Device->DeviceId = Module->DeviceList.GetCount();
	strncpy(Device->DeviceName, DeviceName, ZE_MAX_DEVICE_NAME_SIZE);
	strncpy(Device->DriverName, DriverName, ZE_MAX_DEVICE_NAME_SIZE);
	Module->DeviceGUIDList.Add(*GUID);

	zeLog("Found Sound Device; "
		"Index : %d, "
		"Device Name: \"%s\", "
		"Device Driver: \"%s\", "
		"Device GUID: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		Device->DeviceId,
		Device->DeviceName,
		Device->DriverName,
		GUID->Data1, GUID->Data2, GUID->Data3,
		GUID->Data4[0], GUID->Data4[1], GUID->Data4[2], GUID->Data4[3],
		GUID->Data4[4], GUID->Data4[5], GUID->Data4[6], GUID->Data4[7]);



	return TRUE;
}

void ZEDSModule::UpdateVolumes(ZESoundSourceType SourceType)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		if (SoundSources[I]->GetSoundSourceType() == SourceType)
			SoundSources[I]->SetVolume(SoundSources[I]->GetVolume());

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		if (SoundSources3D[I]->GetSoundSourceType() == SourceType)
			SoundSources3D[I]->SetVolume(SoundSources[I]->GetVolume());

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

const ZEArray<ZESoundDevice>& ZEDSModule::GetDeviceList()
{
	return DeviceList;
}

bool ZEDSModule::Initialize()
{	
	HRESULT hr;
	
	zeLog("Initializing DirectSound module.");
	zeLog("Enumurating sound devices.");
	
	DeviceList.Clear();
	DeviceList.Add();

	hr = DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, (VOID*)this);
	if (FAILED(hr))
    {
		zeError("Can not enumurate sound devices.");
		return false;
    }

	unsigned int DeviceId = SoundOptions.GetOption("DeviceId")->GetValue().GetInteger();

	GUID* DeviceGUID = NULL;
	if (DeviceId > DeviceList.GetCount())
	{
		zeWarning("Wrong device id. Using default sound device.");
	}
	else
	{
		if (DeviceId == 0)
		{
			zeLog("Using default sound device.");
		}
		else
		{
			zeLog("Using \"%s\" (Driver : \"%s\") sound device.", DeviceList[DeviceId].DeviceName, DeviceList[DeviceId].DeviceName);
			DeviceGUID = &DeviceGUIDList[DeviceId];
		}
	}

	hr = DirectSoundCreate8(DeviceGUID, &DS, NULL);
	if (FAILED(hr))
	{	
		Destroy();
		zeError("Can not create DirectSound.");
		return false;
	}

	hr=DS->SetCooperativeLevel((HWND)zeWindow->GetHandle(), DSSCL_PRIORITY);
	
	if (FAILED(hr))
	{	
		Destroy();
		zeError("Can not set DirectSound device cooperative level.");
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
		zeError("Can create direct sound primary buffer.");
		return false;
	}

	hr = DSPrimary->QueryInterface(IID_IDirectSound3DListener8, (VOID**)&DSListener);
	if (FAILED(hr))
	{
		Destroy();
		zeError("Can not create a Direct Sound listener.");
		return false;
	}

	ZEDSComponentBase::BaseInitialize(this);

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

	zeLog("DirectSound Initialized.");

	return ZESoundModule::Initialize();
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

	ZESoundModule::Deinitialize();
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
	DSPrimary->SetVolume((LONG)(log10f((float)MasterVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f));
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
		TypeVolumes[Type] = Volume;

	UpdateVolumes(Type);
}

unsigned int ZEDSModule::GetTypeVolume(ZESoundSourceType Type)
{
	zeAssert(Type >= 256, "Sound source types are limited to 256");
	return TypeVolumes[Type];
}


void ZEDSModule::SetMaxBufferSize(size_t BufferSize)
{
	MaxBufferSize = BufferSize;
}

size_t ZEDSModule::GetMaxBufferSize()
{
	return MaxBufferSize;
}

void ZEDSModule::ProcessSound(float ElapsedTime)
{
	for (size_t I = 0; I < SoundSources.GetCount(); I++)
		SoundSources[I]->Update(ElapsedTime);

	for (size_t I = 0; I < SoundSources3D.GetCount(); I++)
		SoundSources3D[I]->Update(ElapsedTime);
	
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
	ActiveListener->ResetParameters();
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
	ZEDSSoundSource3D* NewSoundSource = new ZEDSSoundSource3D();
	SoundSources3D.Add(NewSoundSource);
	return NewSoundSource;
}

ZEListener* ZEDSModule::CreateListener()
{
	ZEDSListener* Listener = new ZEDSListener();
	Listeners.Add(Listener);
	return Listener;
}




