//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSSoundSource3D.cpp
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

#include "ZEDSSoundSource3D.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEMath/ZEMath.h"

bool ZEDSSoundSource3D::CreateBuffer()
{
	if (DSBuffer != NULL)
	{
		DSBuffer->Release();
		DSBuffer = NULL;
	}

	// Check streaming available and use it necessary
	Streaming = !zeSound->GetStreamingDisabled()  && (1000 * SoundResource->GetSampleCount()) / (ZESize)SoundResource->GetSamplesPerSecond() > zeSound->GetMaxBufferSize();

	// Calculate streaming buffer
	if (Streaming)
		BufferSampleCount = (zeSound->GetMaxBufferSize() * (ZESize)SoundResource->GetSamplesPerSecond()) / 1000;

	HRESULT hr;
	WAVEFORMATEX Format;
	memset(&Format, 0, sizeof(WAVEFORMATEX)); 
	Format.cbSize			= (WORD)sizeof(WAVEFORMATEX);
	Format.wFormatTag		= WAVE_FORMAT_PCM; 
	Format.nChannels		= (WORD)SoundResource->GetChannelCount(); 
	Format.nSamplesPerSec	= (DWORD)SoundResource->GetSamplesPerSecond();
	Format.nBlockAlign		= (WORD)SoundResource->GetBlockAlign();
	Format.wBitsPerSample	= (WORD)SoundResource->GetBitsPerSample();
	Format.nAvgBytesPerSec = Format.nSamplesPerSec * Format.nBlockAlign; 

	DSBUFFERDESC DSBufferDesc;
	memset(&DSBufferDesc, 0, sizeof(DSBUFFERDESC)); 
	DSBufferDesc.dwSize			= (DWORD)sizeof(DSBUFFERDESC); 
	DSBufferDesc.dwFlags		= DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	DSBufferDesc.dwBufferBytes	= (Streaming ? (DWORD)(BufferSampleCount * SoundResource->GetBlockAlign()) : (DWORD)SoundResource->GetUncompressedDataSize()); 
	DSBufferDesc.lpwfxFormat	= &Format;
	DSBufferDesc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;

	if (FAILED(hr = GetDevice()->CreateSoundBuffer(&DSBufferDesc, &DSBuffer, NULL)))
	{
		zeError("Can not create direct sound buffer.");
		return false;
	}

	if (FAILED(hr = DSBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&DS3DBuffer)))
	{
		zeError("Can not create 3d direct sound buffer.");
		return false;
	}

	if (!Streaming)
	{
		// Load everything into buffer
		void *Buffer1, *Buffer2;
		DWORD BufferSize1, BufferSize2;
		DSBuffer->Lock(0, DSBufferDesc.dwBufferBytes, &Buffer1, &BufferSize1, &Buffer2, &BufferSize2, DSBLOCK_ENTIREBUFFER);	
			
			SoundResource->Decode(Buffer1, 0, (ZESize)BufferSize1 / SoundResource->GetBlockAlign());
			
			if (Buffer2 != NULL)
			{
				SoundResource->Decode(Buffer2, (ZESize)BufferSize1 / SoundResource->GetBlockAlign(), (ZESize)BufferSize2);
			}

			/*memcpy(Buffer1, SoundResource->GetBuffer(), (BufferSize1 < SoundResource->GetBufferSize() ? BufferSize1 : SoundResource->GetBufferSize()));
			if (BufferSize2 != 0)
				memcpy(Buffer2, SoundResource->GetBuffer() + BufferSize1, SoundResource->GetBufferSize() - BufferSize1);*/
	
		DSBuffer->Unlock(Buffer1, BufferSize1, Buffer2, BufferSize2);
	}
	else
	{
		ResetStream();
	}

	ResetParameters();

	// If sound was playing continue to play.
	if (SoundSourceState == ZE_SSS_PLAYING)
	{
		Resume();
	}

	return true;
}

void ZEDSSoundSource3D::ResetParameters()
{
	// Reset properties to apply sound buffer
	float EffectiveVolume = (float)Volume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	DSBuffer->SetVolume((LONG)(ZEMath::Log10(EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f));
	DSBuffer->SetPan((((Pan + 100)*(DSBPAN_RIGHT - DSBPAN_LEFT))/200) + DSBPAN_LEFT);
	DSBuffer->SetFrequency((LONG)(PlaybackSpeed * (float)SoundResource->GetSamplesPerSecond()));

	DS3DBUFFER Params;
	const ZEVector3& WorldPosition = GetWorldPosition();
	Params.vPosition.x = WorldPosition.x;
	Params.vPosition.y = WorldPosition.y;
	Params.vPosition.z = WorldPosition.z;
    const ZEVector3& WorldVelocity = ZEVector3::Zero; /*GetWorldVelocity();*/
	Params.vVelocity.x = WorldVelocity.x;
    Params.vVelocity.y = WorldVelocity.y;
    Params.vVelocity.z = WorldVelocity.z;
	
	ZEVector3 Direction;
	ZEQuaternion::VectorProduct(Direction, GetWorldRotation(), ZEVector3::UnitZ);
    Params.vConeOrientation.x = Direction.x;
    Params.vConeOrientation.y = Direction.x;
    Params.vConeOrientation.z = Direction.x;
	EffectiveVolume = (float)ConeOutsideVolume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);

	Params.dwInsideConeAngle = ConeInsideAngle;
	Params.dwOutsideConeAngle = ConeOutsideAngle;
	Params.flMinDistance = MinDistance;
	Params.flMaxDistance = MaxDistance;
	Params.lConeOutsideVolume = (LONG)(ZEMath::Log10(EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f);

	Params.dwMode = DS3DMODE_NORMAL ;
	
	DS3DBuffer->SetAllParameters(&Params, DS3D_IMMEDIATE);
}

void ZEDSSoundSource3D::ResetStream()
{
	StreamDecodeAndFill(0, CurrentPosition, BufferSampleCount / 2);
	
	StreamPosition = CurrentPosition + BufferSampleCount / 2;
	if (StreamPosition > EffectiveEndPosition)
		StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;

	LastUpdatedBufferChunk = 1;
	OldBufferPosition = 0;
	DSBuffer->SetCurrentPosition(0);
 }

void ZEDSSoundSource3D::StreamDecodeAndFill(ZESize BufferPosition, ZESize Position, ZESize SampleCount)
{
	void* Buffer;
	DWORD LockedBufferSize;

	DSBuffer->Lock((DWORD)(BufferPosition * SoundResource->GetBlockAlign()), (DWORD)(SampleCount * SoundResource->GetBlockAlign()), &Buffer, &LockedBufferSize, NULL, NULL, NULL);

		if (Position + SampleCount < EffectiveEndPosition)
		{
			SoundResource->Decode(Buffer, Position, SampleCount);
		}
		else
		{
			SampleCount = EffectiveEndPosition - Position;
			SoundResource->Decode(Buffer, Position, SampleCount);

			ZESize RemainingSampleCount = BufferSampleCount / 2 - SampleCount;
			void* RemainingSampleBuffer = ((unsigned char*)Buffer) + SampleCount * SoundResource->GetBlockAlign();

			if (Looping)
			{
				SoundResource->Decode(RemainingSampleBuffer, EffectiveStartPosition, RemainingSampleCount);
			}
			else
			{
				memset(RemainingSampleBuffer, (SoundResource->GetBitsPerSample() == 8 ? 0x80 : 0x00), RemainingSampleCount * SoundResource->GetBlockAlign());
			}
		}
	
	DSBuffer->Unlock(Buffer, LockedBufferSize, NULL, NULL);
}

void ZEDSSoundSource3D::Stream()
{
	ZESize BufferSampleCount_2 = BufferSampleCount / 2;

	DWORD Temp;	
	DSBuffer->GetCurrentPosition(&Temp, NULL);
	ZESize BufferPosition = (ZESize)Temp / SoundResource->GetBlockAlign();

	// Cursor is at second chunk
	// Fill First Chunk
	if ((LastUpdatedBufferChunk != 1) && (BufferPosition >= BufferSampleCount_2))
	{
		StreamDecodeAndFill(0, StreamPosition, BufferSampleCount_2);
		LastUpdatedBufferChunk = 1;

		StreamPosition += BufferSampleCount_2;
		if (StreamPosition > EffectiveEndPosition)
		{
			StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;
		}
	}
	
	// Cursor is at first chunk
	// Fill Second Chunk
	if ((LastUpdatedBufferChunk != 2) && (BufferPosition < BufferSampleCount_2))
	{
		StreamDecodeAndFill(BufferSampleCount_2, StreamPosition, BufferSampleCount_2);
		LastUpdatedBufferChunk = 2;

		StreamPosition += BufferSampleCount_2;
		if (StreamPosition > EffectiveEndPosition)
		{
			StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;
		}
	}
}

ZEDSSoundSource3D::ZEDSSoundSource3D()
{
	DSBuffer = NULL;
	DS3DBuffer = NULL; 
}

ZEDSSoundSource3D::~ZEDSSoundSource3D()
{
	GetModule()->SoundSources3D.RemoveValue(this);
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

void ZEDSSoundSource3D::SetCurrentPosition(ZESize SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			SampleIndex = SoundResource->GetSampleCount();
		}

		if (Streaming)
		{
			CurrentPosition = SampleIndex;
		}
		else
		{
			DSBuffer->SetCurrentPosition((DWORD)(SampleIndex * SoundResource->GetBlockAlign()));
		}
	}
}

ZESize ZEDSSoundSource3D::GetCurrentPosition()
{
	if (Streaming)
	{
		return CurrentPosition;
	}
	else
	{
		if (DSBuffer != NULL)
		{
			DWORD Temp;
			DSBuffer->GetCurrentPosition(&Temp, NULL);
			ZESize Position = (ZESize)Temp / SoundResource->GetBlockAlign();
			return Position;
		}
		else
		{
			return 0;
		}
	}
}

void ZEDSSoundSource3D::SetStartPosition(ZESize SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			EffectiveStartPosition = SampleIndex % SoundResource->GetSampleCount();
			return;
		}
	}

	EffectiveStartPosition = SampleIndex;
}

void ZEDSSoundSource3D::SetEndPosition(ZESize SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
		{
			EffectiveEndPosition = SampleIndex % SoundResource->GetSampleCount();
			return;
		}
	}

	EffectiveEndPosition = SampleIndex;
}

void ZEDSSoundSource3D::SetPan(ZEInt NewPan)
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

void ZEDSSoundSource3D::SetPlaybackSpeed(float Speed)
{
	PlaybackSpeed = Speed;

	if (SoundResource != NULL)
		DSBuffer->SetFrequency((DWORD)(Speed * SoundResource->GetSamplesPerSecond()));
}

void ZEDSSoundSource3D::SetVolume(ZEUInt NewVolume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;
	else
		Volume = NewVolume;

	float EffectiveVolume = (float)Volume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (DSBuffer != NULL)
		DSBuffer->SetVolume((long)(ZEMath::Log10((float)EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f));
}

void ZEDSSoundSource3D::SetLooping(bool Enabled)
{
	Looping = Enabled;
	if (DSBuffer != NULL && !Streaming && SoundSourceState == ZE_SSS_PLAYING)
		DSBuffer->Play(0, 0, (Looping ? DSBPLAY_LOOPING : NULL));
}

void ZEDSSoundSource3D::Play()
{
	CurrentPosition = EffectiveStartPosition;

	if (DSBuffer != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		
		if (Streaming)
		{
			CurrentPosition = EffectiveStartPosition;
			ResetStream();
		}
		else
		{
			OldBufferPosition = EffectiveStartPosition * SoundResource->GetBlockAlign();
			DSBuffer->SetCurrentPosition((DWORD)(EffectiveStartPosition * SoundResource->GetBlockAlign()));
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
		{
			ResetStream();
		}
		else
		{
			DSBuffer->SetCurrentPosition((DWORD)EffectiveStartPosition);
		}

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
		{
			CurrentPosition = EffectiveStartPosition;
		}
		else
		{
			DSBuffer->SetCurrentPosition((DWORD)EffectiveStartPosition);
		}

		SoundSourceState = ZE_SSS_STOPPED;
	}
}

void ZEDSSoundSource3D::Update(float ElapsedTime)
{
	if (DSBuffer == NULL || SoundSourceState != ZE_SSS_PLAYING)
		return;
		
	DWORD Temp;
	DSBuffer->GetCurrentPosition(&Temp, NULL);
	ZESize BufferPosition = (ZESize)Temp / SoundResource->GetBlockAlign();

	// Update CurrentPosition
	bool BufferRewinded = false;

	if (Streaming)
	{
		if (BufferPosition >= OldBufferPosition)
		{
			CurrentPosition += BufferPosition - OldBufferPosition;
		}
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
		{
			BufferRewinded = true;
		}

		CurrentPosition = BufferPosition;
		OldBufferPosition = BufferPosition;
	}

	// Check Limits
	if ((LimitsEnabled && (CurrentPosition < EffectiveStartPosition || CurrentPosition > EffectiveEndPosition || (!Streaming && BufferRewinded && (EffectiveStartPosition < EffectiveEndPosition)))) || 
		(!LimitsEnabled && (CurrentPosition > SoundResource->GetSampleCount() || (!Streaming && BufferRewinded))))
	{
		if (Looping)
		{
			SetCurrentPosition(EffectiveStartPosition);
		}
		else
		{
			SoundSourceState = ZE_SSS_STOPPED;
			DSBuffer->Stop();
			return;
		}
	}

	// Stream
	if (Streaming)
	{
		Stream();
	}

}

void ZEDSSoundSource3D::SetSoundResource(ZESoundResource* Resource)
{
	if (Resource != NULL)
	{
		if (Resource->GetChannelCount() != 1)
		{
			zeError("Other than one channel (mono) sound resources can not be assigned to 3d sound source.");
			return;
		}

		Resource->AddReferance();
		SoundResource = Resource;
		SoundSourceState = ZE_SSS_STOPPED;
		CreateBuffer();
		SetLimitsEnabled(LimitsEnabled);
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

void ZEDSSoundSource3D::SetPosition(const ZEVector3& NewPosition)
{
	ZEEntity::SetPosition(NewPosition);

	if (DS3DBuffer != NULL)
	{
		const ZEVector3& WorldPosition = ZEEntity::GetWorldPosition();

		DS3DBuffer->SetPosition(WorldPosition.x, WorldPosition.y, WorldPosition.z, DS3D_DEFERRED);
	}
}

void ZEDSSoundSource3D::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEEntity::SetRotation(NewRotation);

	if (DS3DBuffer != NULL)
	{
		ZEVector3 Front;
		ZEQuaternion::VectorProduct(Front, ZEEntity::GetWorldRotation(), ZEVector3::UnitZ);

		DS3DBuffer->SetConeOrientation(Front.x, Front.y, Front.z, DS3D_DEFERRED);
	}
}

void ZEDSSoundSource3D::SetMinDistance(float NewMinDistance)
{
	MinDistance = NewMinDistance;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetMinDistance(MinDistance, DS3D_DEFERRED);
		
}

void ZEDSSoundSource3D::SetMaxDistance(float NewMaxDistance)
{
	MaxDistance = NewMaxDistance;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetMaxDistance(MinDistance, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeInsideAngle(ZEUInt NewInsideAngle)
{
	if (NewInsideAngle > 360)
		ConeInsideAngle = 360;
	else
		ConeInsideAngle = NewInsideAngle;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetConeAngles(ConeInsideAngle, ConeOutsideAngle, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeOutsideAngle(ZEUInt NewOutsideAngle)			
{
	if (NewOutsideAngle > 360)
		ConeOutsideAngle = 360;
	else
		ConeOutsideAngle = NewOutsideAngle;

	if (DS3DBuffer != NULL)
		DS3DBuffer->SetConeAngles(ConeInsideAngle, ConeOutsideAngle, DS3D_DEFERRED);
}

void ZEDSSoundSource3D::SetConeOutsideVolume(ZEUInt NewOutsideVolume)
{
	if (NewOutsideVolume > ZE_SS_VOLUME_MAX)
		ConeOutsideVolume = ZE_SS_VOLUME_MAX;
	else
		ConeOutsideVolume = NewOutsideVolume;

	float EffectiveVolume = (float)ConeOutsideVolume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (DSBuffer != NULL)
		DS3DBuffer->SetConeOutsideVolume((DWORD)(ZEMath::Log10(EffectiveVolume / 100.0f * 99.0f + 1.0f) * 5000.0f - 10000.0f), DS3D_DEFERRED);
}
