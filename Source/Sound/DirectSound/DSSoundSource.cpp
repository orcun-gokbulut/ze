//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DSSoundSource.cpp
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

#include "DSSoundSource.h"
#include "Core/Error.h"

bool ZEDSSoundSource::CreateBuffer()
{
	if (DSBuffer != NULL)
	{
		DSBuffer->Release();
		DSBuffer = NULL;
	}

	//Streaming = SoundResource->GetPCMDataSize() < zeSound->GetMaxBufferSize();
	Streaming = false;

	HRESULT hr;
	WAVEFORMATEX Format;
	memset(&Format, 0, sizeof(WAVEFORMATEX)); 
	Format.cbSize = sizeof(WAVEFORMATEX);
	Format.wFormatTag = WAVE_FORMAT_PCM; 
	Format.nChannels = SoundResource->GetChannelCount(); 
	Format.nSamplesPerSec = SoundResource->GetSamplesPerSecond();
	Format.nBlockAlign = SoundResource->GetBlockAlign();
	Format.wBitsPerSample = SoundResource->GetBitsPerSample();
	Format.nAvgBytesPerSec = Format.nSamplesPerSec * Format.nBlockAlign; 

	DSBUFFERDESC DSBufferDesc;
	memset(&DSBufferDesc, 0, sizeof(DSBUFFERDESC)); 
	DSBufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	DSBufferDesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLPOSITIONNOTIFY;
	DSBufferDesc.dwBufferBytes = (Streaming ? zeSound->GetMaxBufferSize() : SoundResource->GetPCMDataSize()); 
	DSBufferDesc.lpwfxFormat = &Format;
	DSBufferDesc.guid3DAlgorithm = GUID_NULL;

	if (FAILED(hr = GetDevice()->CreateSoundBuffer(&DSBufferDesc, &DSBuffer, NULL)))
	{
		zeError("DirectSound Sound Source", "Can not create Direct Sound buffer.");
		return false;
	}

	if (!Streaming)
	{
		void *Buffer1, *Buffer2;
		DWORD BufferSize1, BufferSize2;
		DSBuffer->Lock(0, DSBufferDesc.dwBufferBytes, &Buffer1, &BufferSize1, &Buffer2, &BufferSize2, DSBLOCK_ENTIREBUFFER);	
			SoundResource->Decode(Buffer1, 0, BufferSize1 / SoundResource->GetBlockAlign());
			if (Buffer2 != NULL)
				SoundResource->Decode(Buffer2, BufferSize1 / SoundResource->GetBlockAlign(), BufferSize2);

			/*memcpy(Buffer1, SoundResource->GetBuffer(), (BufferSize1 < SoundResource->GetBufferSize() ? BufferSize1 : SoundResource->GetBufferSize()));
			if (BufferSize2 != 0)
				memcpy(Buffer2, SoundResource->GetBuffer() + BufferSize1, SoundResource->GetBufferSize() - BufferSize1);*/
	
		DSBuffer->Unlock(Buffer1, BufferSize1, Buffer2, BufferSize2);
	}

	return true;
}

void ZEDSSoundSource::Stream()
{

}

void ZEDSSoundSource::ManageParameters()
{
}

ZEDSSoundSource::ZEDSSoundSource()
{
	DSBuffer = NULL;
}

ZEDSSoundSource::~ZEDSSoundSource()
{
	GetModule()->SoundSources.DeleteValue(this);
	if (DSBuffer != NULL)
		DSBuffer->Release();
}

void ZEDSSoundSource::SetCurrentCursor(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
			SampleIndex = SoundResource->GetSampleCount();

		if (DSBuffer != NULL)
			DSBuffer->SetCurrentPosition(SampleIndex * SoundResource->GetBlockAlign());
	}
}

unsigned int ZEDSSoundSource::GetCurrentCursor()
{
	if (DSBuffer != NULL)
	{
		DWORD Position;
		DSBuffer->GetCurrentPosition(&Position, NULL);
		Position /= SoundResource->GetBlockAlign();
		return Position;
	}
	else
		return 0;
}

void ZEDSSoundSource::SetStartCursor(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			StartCursor = SoundResource->GetSampleCount();
			return;
		}

	StartCursor = SampleIndex;
}

void ZEDSSoundSource::SetEndCursor(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			EndCursor = SoundResource->GetSampleCount();
			return;
		}

	EndCursor = SampleIndex;
}

void ZEDSSoundSource::SetPan(int NewPan)
{
	Pan = NewPan;
	if (DSBuffer != NULL)
		DSBuffer->SetPan((((Pan + 100)*(DSBPAN_RIGHT - DSBPAN_LEFT))/200) + DSBPAN_LEFT);
}

void ZEDSSoundSource::SetFrequency(unsigned int NewFrequency)
{
	Frequency = NewFrequency;
	if (DSBuffer != NULL)
		DSBuffer->SetFrequency(NewFrequency);
}

void ZEDSSoundSource::SetVolume(unsigned int NewVolume)
{
	Volume = NewVolume;
	if (DSBuffer != NULL)
		DSBuffer->SetVolume(Volume * (DSBVOLUME_MAX - DSBVOLUME_MAX) / 100 + DSBVOLUME_MIN);
}

void ZEDSSoundSource::SetLoop(bool Enabled)
{
	Loop = Enabled;
}

void ZEDSSoundSource::Play()
{
	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		DSBuffer->SetCurrentPosition(StartCursor);
		DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void ZEDSSoundSource::Resume()
{
	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		DSBuffer->Play(0, 0, 0);

	}
}

void ZEDSSoundSource::Pause()
{
	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PAUSED;
		DSBuffer->Stop();
	}
}

void ZEDSSoundSource::Stop()
{
	if (DSBuffer != NULL)
	{
		DSBuffer->Stop();
		DSBuffer->SetCurrentPosition(StartCursor);
		SoundSourceState = ZE_SSS_STOPPED;
	}
}

void ZEDSSoundSource::Update(float ElapsedTime)
{
	if (DSBuffer == NULL || SoundSourceState != ZE_SSS_PLAYING)
		return;

	if (Streaming)
		Stream();
	else
	{
		unsigned int Cursor = GetCurrentCursor();
		if (Cursor < StartCursor || Cursor > EndCursor)
		{
			if (Loop)
				SetCurrentCursor(StartCursor);
			else
			{
				SoundSourceState = ZE_SSS_STOPPED;
				DSBuffer->Stop();
			}
		}
	}
}

void ZEDSSoundSource::SetSoundResource(ZESoundResource* Resource)
{
	if (Resource != NULL)
	{
		Resource->AddReferance();
		SoundResource = Resource;
		StartCursor = 0;
		CurrentCursor = 0;
		EndCursor = Resource->GetSampleCount();
		SoundSourceState = ZE_SSS_STOPPED;
		CreateBuffer();
		Stream();
	}
	else
	{
		SoundSourceState = ZE_SSS_NONE;
		if (DSBuffer != NULL)
		{
			DSBuffer->Release();
			DSBuffer = NULL;
		}
	}
}
