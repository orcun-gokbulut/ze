//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALSoundSource3D.cpp
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

// FIRST OF ALL:
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !
// OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS ! OPENAL SUCKS !

#include "ZEALSoundSource3d.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZESound/ZEListener.h"

static const char* ConvertErrorToString(ALenum Error)
{
	switch(Error)
	{
		case AL_NO_ERROR:
		return "There is not currently an error.";
		case AL_INVALID_NAME:
			return "A bad name (ID) was passed to an OpenAL function.";
		case AL_INVALID_ENUM:
			return "An invalid enum value was passed to an OpenAL function.";
		case AL_INVALID_VALUE:
			return "An invalid value was passed to an OpenAL function.";
		case AL_INVALID_OPERATION:
			return "The requested operation is not valid.";
		case AL_OUT_OF_MEMORY:
			return "The requested operation resulted in OpenAL running out of memory";
		default:
			return "Unknown error.";
	}
}

static ALenum GetBufferFormat(ZESoundResource* SoundResource)
{
	switch(SoundResource->GetChannelCount())
	{
		case 1:
			switch(SoundResource->GetBitsPerSample())
			{
				case 8:
					return AL_FORMAT_MONO8;
					break;
				case 16:
					return AL_FORMAT_MONO16;
					break;
				default:
					return 0;
			}
			break;
		case 2:
			switch(SoundResource->GetBitsPerSample())
			{
				case 8:
					return AL_FORMAT_STEREO8;
					break;
				case 16:
					return AL_FORMAT_STEREO16;
					break;
				default:
					return 0;
			}
			break;
		default:
			return 0;
	}
}

bool ZEALSoundSource3D::CreateBuffer()
{
	ALenum Error; // Notation sucks
	
	// Delete old buffer and source
	DestroyBufferSource();


	// Check streaming available and use it necessery
	Streaming = !GetModule()->GetStreamingDisabled()  && (1000 * SoundResource->GetSampleCount()) / SoundResource->GetSamplesPerSecond() > GetModule()->GetMaxBufferSize();

	// Calculate streaming buffer
	if (Streaming)
		BufferSampleCount = (GetModule()->GetMaxBufferSize() * SoundResource->GetSamplesPerSecond()) / 1000;

	BufferSampleCount /= 2;

	// Create Buffer
	alGetError();
	alGenBuffers(1, &ALBuffer1);
	if (Streaming)
		alGenBuffers(1, &ALBuffer2);

	if ((Error = alGetError()) != AL_NO_ERROR)
	{
		DestroyBufferSource();
		zeError("OpenAL Sound Source", "Can not create OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
		return false;
	}


	alGetError();
	alGenSources(1, &ALSource);
	if ((Error = alGetError()) != AL_NO_ERROR)
	{
		DestroyBufferSource();
		zeError("OpenAL Sound Source", "Can not create OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
		return false;
	}

	if (!Streaming)
	{
		unsigned char* ALStupidBuffer = new unsigned char[SoundResource->GetPCMDataSize()];
		SoundResource->Decode(ALStupidBuffer, 0, SoundResource->GetSampleCount());
		
		alGetError();
		alBufferData(ALBuffer1, GetBufferFormat(SoundResource), ALStupidBuffer, SoundResource->GetPCMDataSize(), SoundResource->GetSamplesPerSecond());
		if ((Error = alGetError()) != AL_NO_ERROR)
		{
			DestroyBufferSource();
			zeError("OpenAL Sound Source", "Can not load data to OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
			return false;
		}
		delete[] ALStupidBuffer;

		alGetError();
		alSourcei(ALSource, AL_BUFFER, ALBuffer1);
		if ((Error = alGetError()) != AL_NO_ERROR)
		{
			DestroyBufferSource();
			zeError("OpenAL Sound Source", "Can not assign OpenAL buffer to OpenAL source. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
			return false;
		}

	}
	else
	{
		if (InnerStreamBuffer != NULL)
			delete[] InnerStreamBuffer;

		InnerStreamBuffer = new char[BufferSampleCount * SoundResource->GetBlockAlign()];
	}

	// If sound was playing continue to play.
	if (SoundSourceState == ZE_SSS_PLAYING)
		Resume();

	// Reset properties to apply sound buffer
	ResetParameters();

	Allocated = true;
	return true;
}


void ZEALSoundSource3D::ResetParameters()
{
	float EffectiveVolume = (float)Volume * ((float)GetModule()->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	alSourcef(ALSource, AL_PITCH, PlaybackSpeed);
	alSourcef(ALSource, AL_GAIN, EffectiveVolume / 100.0f);
	//alSourcei(ALSource, AL_LOOPING, (Streaming ? AL_FALSE : AL_TRUE));
	alSourcei(ALSource, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(ALSource, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(ALSource, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(ALSource, AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(ALSource, AL_ROLLOFF_FACTOR, GetModule()->GetActiveListener()->GetRollOffFactor());
	alSourcei(ALSource, AL_SOURCE_RELATIVE, AL_FALSE);

	const ZEVector3& Position = GetWorldPosition();
	alSource3f(ALSource, AL_POSITION, Position.x, Position.y, Position.z);
	ZEVector3 Direction;
	ZEQuaternion::VectorProduct(Direction, GetWorldRotation(), ZEVector3::UnitZ);
	alSource3f(ALSource, AL_DIRECTION, Direction.x, Direction.y, Direction.z);
	alSourcei(ALSource, AL_REFERENCE_DISTANCE, MinDistance);
	alSourcei(ALSource, AL_CONE_INNER_ANGLE, ConeInsideAngle);
	alSourcei(ALSource, AL_CONE_OUTER_ANGLE, ConeOutsideAngle);
	alSourcei(ALSource, AL_CONE_OUTER_GAIN, EffectiveVolume / 100.0f);

}

void ZEALSoundSource3D::DestroyBufferSource()
{
	if (ALBuffer1 != NULL)
		alDeleteBuffers(1, &ALBuffer1);

	if (ALBuffer2 != NULL)
		alDeleteBuffers(1, &ALBuffer2);
	
	if (ALSource != NULL)
		alDeleteSources(1, &ALSource);
	
	if (InnerStreamBuffer != NULL)
		delete[] InnerStreamBuffer;

	Allocated = false;
}

void ZEALSoundSource3D::ResetStream()
{	
	StreamPosition = CurrentPosition;
	if (StreamPosition > EffectiveEndPosition)
		StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;

	StreamDecodeAndFill(ALBuffer1, StreamPosition, BufferSampleCount);
	StreamPosition += BufferSampleCount;
	if (StreamPosition > EffectiveEndPosition)
		StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;

	StreamDecodeAndFill(ALBuffer2, StreamPosition, BufferSampleCount);
	StreamPosition += BufferSampleCount;
	if (StreamPosition > EffectiveEndPosition)
		StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;

	OldBufferPosition = 0;
	alSourcei(ALSource, AL_SAMPLE_OFFSET, 0);
	alSourcei(ALSource, AL_BUFFER, NULL);
	alSourceQueueBuffers(ALSource, 1, &ALBuffer1);
	alSourceQueueBuffers(ALSource, 1, &ALBuffer2);

}


void ZEALSoundSource3D::StreamDecodeAndFill(ALuint Buffer, size_t Position, size_t SampleCount)
{
	if (Position + SampleCount < EffectiveEndPosition)
		SoundResource->Decode(InnerStreamBuffer, Position, SampleCount);
	else
	{
		SampleCount = EffectiveEndPosition - Position;
		SoundResource->Decode(InnerStreamBuffer, Position, SampleCount);

		size_t RemainingSampleCount = BufferSampleCount - SampleCount;
		void* RemainingSampleBuffer = ((unsigned char*)InnerStreamBuffer) + SampleCount * SoundResource->GetBlockAlign();

		if (Looping)
			SoundResource->Decode(RemainingSampleBuffer, EffectiveStartPosition, RemainingSampleCount);
		else
			memset(RemainingSampleBuffer, (SoundResource->GetBitsPerSample() == 8 ? 0x80 : 0x00), RemainingSampleCount * SoundResource->GetBlockAlign());
	}


	alBufferData(Buffer, GetBufferFormat(SoundResource), InnerStreamBuffer, BufferSampleCount * SoundResource->GetBlockAlign(), SoundResource->GetSamplesPerSecond());

	zeLog("ss", "Stream Position %d, Buffer Id : %d, Error: %d", StreamPosition, Buffer, alGetError());

}

void ZEALSoundSource3D::Stream()
{
	ALint BufferCount;
	ALuint Buffer;
	alGetSourcei(ALSource, AL_BUFFERS_PROCESSED, &BufferCount);
	for (size_t I = 0; I < BufferCount; I++)
	{
		alSourceUnqueueBuffers(ALSource, 1, &Buffer);
		alDeleteBuffers(1, &Buffer);
		alGenBuffers(1, &Buffer);

		StreamDecodeAndFill(Buffer, StreamPosition, BufferSampleCount);
		StreamPosition += BufferSampleCount;
		if (StreamPosition > EffectiveEndPosition)
			StreamPosition = (StreamPosition - EffectiveEndPosition) + EffectiveStartPosition;

		alSourceQueueBuffers(ALSource, 1, &Buffer);
	}
}

ZEALSoundSource3D::ZEALSoundSource3D()
{
	InnerStreamBuffer = NULL;
	ALBuffer1 = NULL;
	ALBuffer2 = NULL;
	ALSource = NULL;

	Allocated = false;
}

ZEALSoundSource3D::~ZEALSoundSource3D()
{
	GetModule()->SoundSources3D.DeleteValue(this);
	DestroyBufferSource();
}

void ZEALSoundSource3D::SetSoundSourceState(ZESoundSourceState State)
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

void ZEALSoundSource3D::SetCurrentPosition(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
			SampleIndex = SoundResource->GetSampleCount();

		if (Streaming)
			CurrentPosition = SampleIndex;
		else
			alSourcei(ALSource, AL_SAMPLE_OFFSET, SampleIndex);
	}
}

unsigned int ZEALSoundSource3D::GetCurrentPosition()
{
	if (Streaming)
		return CurrentPosition;
	else
		if (SoundResource != NULL)
		{
			ALint Position;
			alGetSourcei(ALSource, AL_SAMPLE_OFFSET, &Position);
			return Position;
		}
		else
			return 0;
}

void ZEALSoundSource3D::SetPan(int NewPan)
{
	if (NewPan> ZE_SS_PAN_RIGHT)
		Pan = ZE_SS_PAN_RIGHT;
	else if (NewPan < ZE_SS_PAN_LEFT)
		Pan = ZE_SS_PAN_LEFT;
	else
		Pan = NewPan;

	// No panning support in OpenAL!
}

void ZEALSoundSource3D::SetPlaybackSpeed(float Speed)
{
	PlaybackSpeed = Speed;

	if (Allocated)
		alSourcef(ALSource, AL_PITCH, Speed);
}

void ZEALSoundSource3D::SetVolume(unsigned int NewVolume)
{
	if (Volume > ZE_SS_VOLUME_MAX)
		Volume = ZE_SS_VOLUME_MAX;

	float EffectiveVolume = (float)Volume * ((float)GetModule()->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (SoundResource != NULL)
		alSourcef(ALSource, AL_GAIN, EffectiveVolume / 100);
}

void ZEALSoundSource3D::SetLooping(bool Enabled)
{
	Looping = Enabled;
	if (SoundResource != NULL && !Streaming)
		alSourcei(ALSource, AL_LOOPING, Looping || Streaming ? AL_TRUE : AL_FALSE);
}

void ZEALSoundSource3D::Play()
{
	CurrentPosition = EffectiveStartPosition;

	if (Allocated)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		
		if (Streaming)
			ResetStream();
		else
		{
			OldBufferPosition = CurrentPosition;
			alBufferi(ALSource, AL_SAMPLE_OFFSET, CurrentPosition);
		}

		alSourcePlay(ALSource);
	}
}

void ZEALSoundSource3D::Resume()
{
	if (SoundResource != NULL)
	{
		SoundSourceState = ZE_SSS_PLAYING;
		
		if (Streaming)
			ResetStream();
		else
			alSourcei(ALSource, AL_SAMPLE_OFFSET, EffectiveStartPosition);

		alSourcePlay(ALSource);
	}
}

void ZEALSoundSource3D::Pause()
{
	if (Allocated)
	{
		SoundSourceState = ZE_SSS_PAUSED;
		alSourceStop(ALSource);
	}
}

void ZEALSoundSource3D::Stop()
{
	if (Allocated)
	{
		alSourceStop(ALSource);
		if (Streaming)
			CurrentPosition = EffectiveStartPosition;
		else
			alSourcei(ALSource, AL_SAMPLE_OFFSET, CurrentPosition);

		SoundSourceState = ZE_SSS_STOPPED;
	}
}

void ZEALSoundSource3D::Update(float ElapsedTime)
{
	if (!Allocated || SoundSourceState != ZE_SSS_PLAYING)
		return;
		
	ALint ALBufferPosition;
	alGetSourcei(ALSource, AL_SAMPLE_OFFSET, &ALBufferPosition);
	BufferPosition = ALBufferPosition;

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
	if (CurrentPosition < EffectiveStartPosition || CurrentPosition > EffectiveEndPosition || (!Streaming && BufferRewinded && (EffectiveStartPosition < EffectiveEndPosition)))
	{
		if (Looping)
			SetCurrentPosition(EffectiveStartPosition);
		else
		{
			SoundSourceState = ZE_SSS_STOPPED;
			alSourceStop(ALSource);
			return;
		}
	}

	// Stream
	if (Streaming)
		Stream();
}

void ZEALSoundSource3D::SetSoundResource(ZESoundResource* Resource)
{
	if (SoundResource != NULL)
	{
		SoundResource->Release();
		SoundResource = NULL;
	}

	if (Resource != NULL)
	{
		SoundResource = Resource;
		SetLimitsEnabled(LimitsEnabled);
		if (!CreateBuffer())
		{
			SoundResource = NULL;
			return;
		}
		SoundSourceState = ZE_SSS_STOPPED;		
		SoundResource->AddReferance();
	}
	else
	{
		DestroyBufferSource();
		SoundSourceState = ZE_SSS_NONE;
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

void ZEALSoundSource3D::SetPosition(const ZEVector3& NewPosition)
{
	ZEComponent::SetPosition(NewPosition);

	if (Allocated)
	{
		const ZEVector3& WorldPosition = ZEComponent::GetWorldPosition();	
		alSource3f(ALSource, AL_POSITION, WorldPosition.x, WorldPosition.y, WorldPosition.z);
	}
}

void ZEALSoundSource3D::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEComponent::SetRotation(NewRotation);

	if (Allocated)
	{
		ZEVector3 Direction;
		ZEQuaternion::VectorProduct(Direction, GetWorldRotation(), ZEVector3::UnitZ);

		alSource3f(ALSource, AL_DIRECTION,  Direction.x, Direction.y, Direction.z);
	}
}

void ZEALSoundSource3D::SetMinDistance(float  NewMinDistance)
{
	if (NewMinDistance < 0)
		MinDistance = 0;
	else
		MinDistance = NewMinDistance;

	if (Allocated)
		alSourcei(ALSource, AL_REFERENCE_DISTANCE, MinDistance);
}

void ZEALSoundSource3D::SetMaxDistance(float  NewMaxDistance)
{
	if (NewMaxDistance < 0)
		MaxDistance = 0;
	else
		MaxDistance = NewMaxDistance;

	if (Allocated)
		alSourcei(ALSource, AL_MAX_DISTANCE, MaxDistance);
}

void ZEALSoundSource3D::SetConeInsideAngle(unsigned int NewInsideAngle)
{
	if (NewInsideAngle > 360)
		ConeInsideAngle = 360;
	else
		ConeInsideAngle = NewInsideAngle;

	if (Allocated)
		alSourcei(ALSource, AL_CONE_INNER_ANGLE, ConeInsideAngle);
}

void ZEALSoundSource3D::SetConeOutsideAngle(unsigned int NewOutsideAngle)			
{
	if (NewOutsideAngle > 360)
		ConeOutsideAngle = 360;
	else
		ConeOutsideAngle = NewOutsideAngle;
	

	if (Allocated)
		alSourcei(ALSource, AL_CONE_OUTER_ANGLE, ConeOutsideAngle);
}

void ZEALSoundSource3D::SetConeOutsideVolume(unsigned int NewOutsideVolume)
{
	if (NewOutsideVolume > ZE_SS_VOLUME_MAX)
		ConeOutsideVolume = ZE_SS_VOLUME_MAX;
	else
		ConeOutsideVolume = NewOutsideVolume;

	float EffectiveVolume = (float)ConeOutsideVolume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
	
	if (Allocated)
		alSourcei(ALSource, AL_CONE_OUTER_GAIN, EffectiveVolume / 100.0f);
}




