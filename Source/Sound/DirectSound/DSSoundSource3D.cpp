//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DSSoundSource3D.cpp
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

#include "DSSoundSource3D.h"
#include "Core/Error.h"
#include "Core/Console.h"

bool ZEDSSoundSource3D::CreateBuffer()
{
	if (DSBuffer != NULL)
	{
		DSBuffer->Release();
		DSBuffer = NULL;
	}

	// Check streaming available and use it necessery
	Streaming = !zeSound->GetStreamingDisabled()  && (1000 * SoundResource->GetSampleCount()) / SoundResource->GetSamplesPerSecond() > zeSound->GetMaxBufferSize();

	// Calculate streaming buffer
	if (Streaming)
		BufferSampleCount = (zeSound->GetMaxBufferSize() * SoundResource->GetSamplesPerSecond()) / 1000;

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
	DSBufferDesc.dwFlags = DSBCAPS_CTRL3D;// | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	DSBufferDesc.dwBufferBytes = (Streaming ? BufferSampleCount * SoundResource->GetBlockAlign() : SoundResource->GetPCMDataSize()); 
	DSBufferDesc.lpwfxFormat = &Format;
	DSBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

	if (FAILED(hr = GetDevice()->CreateSoundBuffer(&DSBufferDesc, &DSBuffer, NULL)))
	{
		zeError("DirectSound Sound Source 3D", "Can not create direct sound buffer.");
		return false;
	}

	if (FAILED(hr = DSBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&DS3DBuffer)))
	{
		zeError("DirectSound Sound Source 3D", "Can not create 3d direct sound buffer.");
		return false;
	}

	if (!Streaming)
	{
		// Load everything into buffer
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

	// If sound was playing continue to play.
	if (SoundSourceState == ZE_SSS_PLAYING)
		Resume();

	// Reset properties to apply sound buffer
	SetVolume(GetVolume());
	SetPan(GetPan());
	SetFrequency(GetFrequency());
	return true;
}

void ZEDSSoundSource3D::ResetStream()
{
	StreamDecodeAndFill(0, CurrentPosition, BufferSampleCount / 2);
	
	StreamPosition = CurrentPosition + BufferSampleCount / 2;
	if (StreamPosition > EndPosition)
		StreamPosition = (StreamPosition - EndPosition) + StartPosition;

	LastUpdatedBufferChunk = 1;
	OldBufferPosition = 0;
	DSBuffer->SetCurrentPosition(0);
 }

void ZEDSSoundSource3D::StreamDecodeAndFill(size_t BufferPosition, size_t Position, size_t SampleCount)
{
	void* Buffer;
	DWORD LockedBufferSize;
	DSBuffer->Lock(BufferPosition * SoundResource->GetBlockAlign(), SampleCount * SoundResource->GetBlockAlign(), &Buffer, &LockedBufferSize, NULL, NULL, NULL);

		if (Position + SampleCount < EndPosition)
			SoundResource->Decode(Buffer, Position, SampleCount);
		else
		{
			SampleCount = EndPosition - Position;
			SoundResource->Decode(Buffer, Position, SampleCount);

			size_t RemainingSampleCount = BufferSampleCount / 2 - SampleCount;
			void* RemainingSampleBuffer = ((unsigned char*)Buffer) + SampleCount * SoundResource->GetBlockAlign();

			if (Looping)
				SoundResource->Decode(RemainingSampleBuffer, StartPosition, RemainingSampleCount);
			else
				memset(RemainingSampleBuffer, (SoundResource->GetBitsPerSample() == 8 ? 0x80 : 0x00), RemainingSampleCount * SoundResource->GetBlockAlign());
		}
	
	DSBuffer->Unlock(Buffer, LockedBufferSize, NULL, NULL);
}

void ZEDSSoundSource3D::Stream()
{
	size_t BufferSampleCount_2 = BufferSampleCount / 2;

	DWORD BufferPosition;	
	DSBuffer->GetCurrentPosition(&BufferPosition, NULL);
	BufferPosition /= SoundResource->GetBlockAlign();


	// Cursor is at second chunk
	// Fill Firts Chunk
	if ((LastUpdatedBufferChunk != 1) && (BufferPosition >= BufferSampleCount_2))
	{
		StreamDecodeAndFill(0, StreamPosition, BufferSampleCount_2);
		zeLog("SS", "Streaming 1!, Current Position : %d, StreamPosition : %d, Buffer Position : %d", CurrentPosition, StreamPosition, BufferPosition);
		LastUpdatedBufferChunk = 1;

		
		StreamPosition += BufferSampleCount_2;
		if (StreamPosition > EndPosition)
			StreamPosition = (StreamPosition - EndPosition) + StartPosition;
	}
	
	// Cursor is at first chunk
	// Fill Second Chunk
	if ((LastUpdatedBufferChunk != 2) && (BufferPosition < BufferSampleCount_2))
	{
		StreamDecodeAndFill(BufferSampleCount_2, StreamPosition, BufferSampleCount_2);
		zeLog("SS", "Streaming 2!, Current Position : %d, StreamPosition : %d, Buffer Position : %d", CurrentPosition, StreamPosition, BufferPosition);
		LastUpdatedBufferChunk = 2;

		StreamPosition += BufferSampleCount_2;
		if (StreamPosition > EndPosition)
			StreamPosition = (StreamPosition - EndPosition) + StartPosition;
	}
}

ZEDSSoundSource3D::ZEDSSoundSource3D()
{
	BufferDirtyFlag = true;
	DSBuffer = NULL;
	DS3DBuffer = NULL; 
}

ZEDSSoundSource3D::~ZEDSSoundSource3D()
{
	GetModule()->SoundSources3D.DeleteValue(this);
	if (DSBuffer != NULL)
		DSBuffer->Release();
	if (DS3DBuffer != NULL)
		DS3DBuffer->Release();
}

void ZEDSSoundSource3D::SetSoundSourceState(ZESoundSourceState State)
{
	if (State != SoundSourceState)
		switch(State)
		{
			case ZE_SSS_STOPPED:
				Stop();
				break;
			case ZE_SSS_PLAYING:
				Play();
				break;
			case ZE_SSS_PAUSED:
				Pause();
				break;
			default:
			case ZE_SSS_NONE:
				SoundSourceState = ZE_SSS_NONE;
				break;
		}
}

void ZEDSSoundSource3D::SetCurrentPosition(unsigned int SampleIndex)
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

unsigned int ZEDSSoundSource3D::GetCurrentPosition()
{
	if (Streaming)
		return CurrentPosition;
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

void ZEDSSoundSource3D::SetStartPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			StartPosition = SoundResource->GetSampleCount();
			return;
		}

	StartPosition = SampleIndex;
}

void ZEDSSoundSource3D::SetEndPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			EndPosition = SoundResource->GetSampleCount();
			return;
		}

	EndPosition = SampleIndex;
}

void ZEDSSoundSource3D::SetPan(int NewPan)
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

void ZEDSSoundSource3D::SetFrequency(unsigned int NewFrequency)
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

void ZEDSSoundSource3D::SetVolume(unsigned int NewVolume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;
	else
		Volume = NewVolume;

	float EffectiveVolume = (float)Volume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (DSBuffer != NULL)
		DSBuffer->SetVolume(log10f((float)EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f);
}

void ZEDSSoundSource3D::SetLooping(bool Enabled)
{
	Looping = Enabled;
	if (DSBuffer != NULL && !Streaming && SoundSourceState == ZE_SSS_PLAYING)
		DSBuffer->Play(0, 0, (Looping ? DSBPLAY_LOOPING : NULL));
}

void ZEDSSoundSource3D::Play()
{
	CurrentPosition = StartPosition;

	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		
		if (Streaming)
		{
			CurrentPosition = StartPosition;
			ResetStream();
		}
		else
		{
			OldBufferPosition = StartPosition * SoundResource->GetBlockAlign();
			DSBuffer->SetCurrentPosition(StartPosition * SoundResource->GetBlockAlign());
		}

		DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void ZEDSSoundSource3D::Resume()
{
	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		
		if (Streaming)
			ResetStream();
		else
			DSBuffer->SetCurrentPosition(StartPosition);

		DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void ZEDSSoundSource3D::Pause()
{
	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PAUSED;
		DSBuffer->Stop();
	}
}

void ZEDSSoundSource3D::Stop()
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

void ZEDSSoundSource3D::Update(float ElapsedTime)
{
	if (DSBuffer == NULL || SoundSourceState != ZE_SSS_PLAYING)
		return;
		
	DWORD BufferPosition;
	DSBuffer->GetCurrentPosition(&BufferPosition, NULL);
	BufferPosition /= SoundResource->GetBlockAlign();

	// Update CurrentPosition
	bool BufferRewinded = false;

	if (Streaming)
	{
		if (BufferPosition >= OldBufferPosition)
			CurrentPosition += BufferPosition - OldBufferPosition;
		else
		{
			BufferRewinded = true;
			CurrentPosition += (BufferSampleCount - OldBufferPosition) + BufferPosition;
		}

		OldBufferPosition = BufferPosition;
	}
	else
	{
		if (OldBufferPosition > BufferPosition)
			BufferRewinded = true;

		CurrentPosition = BufferPosition;
		OldBufferPosition = BufferPosition;
	}

	// Check Limits
	if (CurrentPosition < StartPosition || CurrentPosition > EndPosition || (!Streaming && BufferRewinded && (StartPosition < EndPosition)))
	{
		if (Looping)
			SetCurrentPosition(StartPosition);
		else
		{
			SoundSourceState = ZE_SSS_STOPPED;
			DSBuffer->Stop();
			return;
		}
	}

	// Stream
	if (Streaming)
		Stream();

}

void ZEDSSoundSource3D::SetSoundResource(ZESoundResource* Resource)
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

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void ZEDSSoundSource3D::SetLocalPosition(const ZEVector3& NewPosition)
{
	ZEComponent::SetLocalPosition(NewPosition);
	BufferDirtyFlag = true;
	if (DS3DBuffer != NULL)
	{
		const ZEVector3& WorldPosition = ZEComponent::GetWorldPosition();

		DS3DBuffer->SetPosition(WorldPosition.x, WorldPosition.y, WorldPosition.z, DS3D_DEFERRED);
	}
}

void ZEDSSoundSource3D::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	ZEComponent::SetLocalRotation(NewRotation);
	BufferDirtyFlag = true;

	if (DS3DBuffer != NULL)
	{
		ZEVector3 Front;
		ZEQuaternion::VectorProduct(Front, ZEComponent::GetWorldRotation(), ZEVector3::UnitZ);

		DS3DBuffer->SetConeOrientation(Front.x, Front.y, Front.z, DS3D_DEFERRED);
	}
}

void ZEDSSoundSource3D::SetMinDistance(float  NewMinDistance)
{
	MinDistance = NewMinDistance;
	BufferDirtyFlag = true;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetMinDistance(MinDistance, DS3D_DEFERRED);
		
}

void ZEDSSoundSource3D::SetMaxDistance(float  NewMaxDistance)
{
	MaxDistance = NewMaxDistance;
	BufferDirtyFlag = true;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetMaxDistance(MinDistance, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeInsideAngle(unsigned int NewInsideAngle)
{
	if (NewInsideAngle > 360)
		ConeInsideAngle = 360;
	else
		ConeInsideAngle = NewInsideAngle;

	BufferDirtyFlag = true;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetConeAngles(ConeInsideAngle, ConeOutsideAngle, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeOutsideAngle(unsigned int NewOutsideAngle)			
{
	if (NewOutsideAngle > 360)
		ConeOutsideAngle = 360;
	else
		ConeOutsideAngle = NewOutsideAngle;
	
	BufferDirtyFlag = true;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetConeAngles(ConeInsideAngle, ConeOutsideAngle, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeOutsideVolume(unsigned int NewOutsideVolume)
{
	if (NewOutsideVolume > ZE_SS_VOLUME_MAX)
		ConeOutsideVolume = ZE_SS_VOLUME_MAX;
	else
		ConeOutsideVolume = NewOutsideVolume;

	BufferDirtyFlag = true;

	float EffectiveVolume = (float)ConeOutsideVolume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (DSBuffer != NULL)
		DS3DBuffer->SetConeOutsideVolume(log10f((float)EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f, DS3D_DEFERRED);
}
