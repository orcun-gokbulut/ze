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

	Streaming = (1000 * SoundResource->GetSampleCount()) / SoundResource->GetSamplesPerSecond() > zeSound->GetMaxBufferSize();
	if (Streaming)
		StreamBufferSize = (zeSound->GetMaxBufferSize() * SoundResource->GetSamplesPerSecond()) / 1000;

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
	DSBufferDesc.dwBufferBytes = (Streaming ? StreamBufferSize * SoundResource->GetBlockAlign() : SoundResource->GetPCMDataSize()); 
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
	else
		ResetStream();

	return true;
}

void ZEDSSoundSource::ResetStream()
{
	StreamDecodeAndFill(StartPosition, 0, StreamBufferSize);
	OldBufferPosition = 0;
}

void ZEDSSoundSource::StreamDecodeAndFill(size_t Position, size_t BufferPosition, size_t BufferSize)
{
	size_t SampleCount = BufferSize - BufferPosition;

	void* Buffer;
	DWORD LockedBufferSize;
	DSBuffer->Lock(Position, SampleCount, &Buffer, &LockedBufferSize, NULL, NULL, NULL);

		if (Position + SampleCount < EndPosition)
			SoundResource->Decode(Buffer, Position, SampleCount);
		else
		{
			SampleCount = EndPosition - Position;
			size_t RemainingSampleCount = (StreamBufferSize - BufferPosition) - SampleCount;
			void* RemainingSampleBuffer = ((unsigned char*)Buffer) + SampleCount * SoundResource->GetBlockAlign();

			if (Loop)
				SoundResource->Decode(RemainingSampleBuffer, StartPosition, RemainingSampleCount);
			else
				memset(RemainingSampleBuffer, (SoundResource->GetBitsPerSample() == 8 ? 0x80 : 0x00), RemainingSampleCount * SoundResource->GetBlockAlign());
		}
	
	DSBuffer->Unlock(Buffer, LockedBufferSize, NULL, NULL);
}


void ZEDSSoundSource::Stream()
{
	size_t StreamBufferSize_2 = StreamBufferSize / 2;

	DWORD BufferPosition;
	
	DSBuffer->GetCurrentPosition(&BufferPosition, NULL);

	// Cursor is at second chunk
	// Fill Firts Chunk
	if (BufferPosition >= StreamBufferSize_2 && CurrentPosition > StreamChunk1Position && StreamChunk1Position + StreamBufferSize_2 < EndPosition)
	{
		StreamChunk1Position += StreamBufferSize;
		StreamDecodeAndFill(StreamChunk1Position, 0, StreamBufferSize_2);
	}
	
	// Cursor is at first chunk
	// Fill Second Chunk
	if (BufferPosition < StreamBufferSize_2 &&  CurrentPosition > StreamChunk2Position && StreamChunk2Position + StreamBufferSize_2 < EndPosition)
	{
		StreamChunk2Position += StreamBufferSize;
		StreamDecodeAndFill(StreamChunk2Position, StreamBufferSize_2, StreamBufferSize_2);
	}
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

void ZEDSSoundSource::SetCurrentPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
			SampleIndex = SoundResource->GetSampleCount();

		if (Streaming)
			CurrentPosition = SampleIndex;
		else
			DSBuffer->SetCurrentPosition(SampleIndex * SoundResource->GetBlockAlign());
	}
}

unsigned int ZEDSSoundSource::GetCurrentPosition()
{
	if (Streaming)
	{	
		return CurrentPosition;
	}
	else
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

void ZEDSSoundSource::SetStartPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			StartPosition = SoundResource->GetSampleCount();
			return;
		}

	StartPosition = SampleIndex;
}

void ZEDSSoundSource::SetEndPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			EndPosition = SoundResource->GetSampleCount();
			return;
		}

	EndPosition = SampleIndex;
}

void ZEDSSoundSource::SetPan(int NewPan)
{
	if (NewPan> ZE_SS_PAN_RIGHT)
		Pan = ZE_SS_PAN_RIGHT;
	else if (NewPan < ZE_SS_PAN_LEFT)
		Pan = ZE_SS_PAN_LEFT;
	else
		Pan = NewPan;

	if (DSBuffer != NULL)
		DSBuffer->SetPan((((Pan + 100)*(DSBPAN_RIGHT - DSBPAN_LEFT))/200) + DSBPAN_LEFT);
}

void ZEDSSoundSource::SetFrequency(unsigned int NewFrequency)
{
	if (Frequency == ZE_SS_FREQUENCY_DEFAULT)
	{
		if (SoundResource != NULL)
			Frequency = SoundResource->GetSamplesPerSecond();
	}
	else
		Frequency = NewFrequency;

	if (DSBuffer != NULL)
		DSBuffer->SetFrequency(NewFrequency);
}

void ZEDSSoundSource::SetVolume(unsigned int NewVolume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;

	float EffectiveVolume = (float)Volume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (DSBuffer != NULL)
		DSBuffer->SetVolume(EffectiveVolume * (DSBVOLUME_MAX - DSBVOLUME_MAX) / ZE_SS_VOLUME_MAX + DSBVOLUME_MIN);
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
		
		if (Streaming)
			ResetStream();
		else
			DSBuffer->SetCurrentPosition(StartPosition);
		
		CurrentPosition = StartPosition;

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
		if (Streaming)
			CurrentPosition = StartPosition;
		else
			DSBuffer->SetCurrentPosition(StartPosition);

		SoundSourceState = ZE_SSS_STOPPED;
	}
}

void ZEDSSoundSource::Update(float ElapsedTime)
{
	if (DSBuffer == NULL || SoundSourceState != ZE_SSS_PLAYING)
		return;

	if (Streaming)
	{
		DWORD BufferPosition;
		DSBuffer->GetCurrentPosition(&BufferPosition, NULL);

		if (BufferPosition > OldBufferPosition)
			CurrentPosition += BufferPosition - OldBufferPosition;
		else
			CurrentPosition += (StreamBufferSize - OldBufferPosition) + BufferPosition;

		OldBufferPosition = BufferPosition;
	}
	else
	{
		DWORD BufferPosition;
		DSBuffer->GetCurrentPosition(&BufferPosition, NULL);

		CurrentPosition = BufferPosition;
	}


	unsigned int Position = GetCurrentPosition();
	if (Position < StartPosition || Position > EndPosition)
	{
		if (Loop)
			SetCurrentPosition(StartPosition);
		else
		{
			SoundSourceState = ZE_SSS_STOPPED;
			DSBuffer->Stop();
		}
	}

	if (Streaming)
		Stream();

}

void ZEDSSoundSource::SetSoundResource(ZESoundResource* Resource)
{
	if (Resource != NULL)
	{
		Resource->AddReferance();
		SoundResource = Resource;
		StartPosition = 0;
		CurrentPosition = 0;
		EndPosition = Resource->GetSampleCount();
		SoundSourceState = ZE_SSS_STOPPED;
		CreateBuffer();
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
