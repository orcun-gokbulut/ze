//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALSoundSource.cpp
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

#include "ZEError.h"
#include "ZEALSoundSource.h"
#include "ZECore/ZEConsole.h"
#include "ZEMath/ZEAngle.h"
#include "ZESound/ZEListener.h"
#include "ZEMath/ZEMath.h"

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

static ALenum GetBufferFormat(const ZESoundResource* SoundResource)
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

void ZEALSoundSource::UpdateResource()
{
	ZESoundSource::UpdateResource();

	if (GetSoundResource() != NULL)
	{
		if (!CreateBuffer())
			return;

		SetSoundSourceState(ZE_SSS_STOPPED);
	}
	else
	{
		DestroyBufferSource();
		SetSoundSourceState(ZE_SSS_NONE);
	}
}

bool ZEALSoundSource::CreateBuffer()
{
	ALenum Error; // Notation sucks
	
	// Delete old buffer and source
	DestroyBufferSource();

	// Check streaming available and use it necessary
	SetStreaming(!GetModule()->GetStreamingDisabled()  && (1000 * GetSoundResource()->GetSampleCount()) / GetSoundResource()->GetSamplesPerSecond() > GetModule()->GetMaxBufferSize());

	// Calculate streaming buffer
	if (IsStreaming())
	{
		BufferSampleCount = (GetModule()->GetMaxBufferSize() * GetSoundResource()->GetSamplesPerSecond()) / 1000;
	}

	BufferSampleCount /= 2;

	// Create Buffer
	alGetError();
	alGenBuffers(1, &ALBuffer1);
	if (IsStreaming())
		alGenBuffers(1, &ALBuffer2);

	if ((Error = alGetError()) != AL_NO_ERROR)
	{
		DestroyBufferSource();
		zeError("Can not create OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
		return false;
	}


	alGetError();
	alGenSources(1, &ALSource);
	if ((Error = alGetError()) != AL_NO_ERROR)
	{
		DestroyBufferSource();
		zeError("Can not create OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
		return false;
	}

	if (!IsStreaming())
	{
		unsigned char* ALStupidBuffer = new unsigned char[GetSoundResource()->GetUncompressedDataSize()];
		GetSoundResource()->Decode(ALStupidBuffer, 0, GetSoundResource()->GetSampleCount());
		
		alGetError();
		alBufferData(ALBuffer1, GetBufferFormat(GetSoundResource()), ALStupidBuffer, (ALsizei)GetSoundResource()->GetUncompressedDataSize(), (ALsizei)GetSoundResource()->GetSamplesPerSecond());
		if ((Error = alGetError()) != AL_NO_ERROR)
		{
			DestroyBufferSource();
			zeError("Can not load data to OpenAL buffer. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
			return false;
		}
		delete[] ALStupidBuffer;

		alGetError();
		alSourcei(ALSource, AL_BUFFER, ALBuffer1);
		if ((Error = alGetError()) != AL_NO_ERROR)
		{
			DestroyBufferSource();
			zeError("Can not assign OpenAL buffer to OpenAL source. (Error No : %d, Error Text : \"%s\")", Error, ConvertErrorToString(Error));
			return false;
		}
	}
	else
	{
		if (InnerStreamBuffer != NULL)
			delete[] InnerStreamBuffer;

		InnerStreamBuffer = new char[BufferSampleCount * GetSoundResource()->GetBlockAlign()];
	}

	Allocated = true;

	// Reset properties to apply sound buffer
	UpdateParameters();
	UpdateOrientation();

	return true;
}

void ZEALSoundSource::UpdateParameters()
{
	if (!Allocated)
		return;

	alSourcef(ALSource, AL_PITCH, GetSpeed());
	alSourcef(ALSource, AL_GAIN, GetEffectiveVolume(GetVolume()));
	alSourcei(ALSource, AL_LOOPING, (!IsStreaming() && GetLooping() ? AL_TRUE : AL_FALSE));
	
	if (!GetPositional())
	{
		float Pan = ZEMath::Saturate(GetPan());
		alSourcei(ALSource, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(ALSource, AL_POSITION, Pan, 0.0f, 1.0f - Pan);
		alSource3f(ALSource, AL_VELOCITY, 0.0, 0.0, 0.0);
		alSource3f(ALSource, AL_DIRECTION, 0.0, 0.0, 0.0);
		alSourcef(ALSource, AL_MIN_GAIN, 1.0f);
		alSourcef(ALSource, AL_MAX_GAIN, 1.0f);
		alSourcef(ALSource, AL_ROLLOFF_FACTOR, 0.0);
	}
	else
	{
		alSourcei(ALSource, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(ALSource, AL_MAX_DISTANCE, GetMaxDistance());
		alSourcef(ALSource, AL_REFERENCE_DISTANCE, (GetMaxDistance() - GetMinDistance()) / 2.0f);
		alSourcef(ALSource, AL_MIN_GAIN, 0.0f);
		alSourcef(ALSource, AL_MAX_GAIN, 1.0f);
		alSourcef(ALSource, AL_CONE_INNER_ANGLE, ZEAngle::ToDegree(GetConeInsideAngle()));
		alSourcef(ALSource, AL_CONE_OUTER_ANGLE, ZEAngle::ToDegree(GetConeOutsideAngle()));
		alSourcef(ALSource, AL_CONE_OUTER_GAIN, ZEMath::Saturate(GetConeOutsideVolume()));
		alSourcef(ALSource, AL_ROLLOFF_FACTOR, GetModule()->GetActiveListener()->GetRollOffFactor());
	}
}

void ZEALSoundSource::UpdateOrientation()
{
	if (!Allocated)
		return;

	ZEVector3 Position = GetWorldPosition();
	alSource3f(ALSource, AL_POSITION, Position.x, Position.y, Position.z);

	ZEVector3 Direction = GetWorldFront();
	alSource3f(ALSource, AL_DIRECTION, Direction.x, Direction.y, Direction.z);

	ZEVector3 Velocity = GetVelocity();
	alSource3f(ALSource, AL_VELOCITY, Velocity.x, Velocity.y, Velocity.z);
}

void ZEALSoundSource::LocalTransformChanged()
{
	UpdateOrientation();
	ZESoundSource::LocalTransformChanged();
}

void ZEALSoundSource::ParentTransformChanged()
{
	UpdateOrientation();
	ZESoundSource::ParentTransformChanged();
}

void ZEALSoundSource::DestroyBufferSource()
{
	if (ALBuffer1 != NULL)
		alDeleteBuffers(1, &ALBuffer1);

	if (ALBuffer2 != NULL)
		alDeleteBuffers(1, &ALBuffer2);
	
	if (ALSource != NULL)
		alDeleteSources(1, &ALSource);
	
	if (InnerStreamBuffer != NULL)
	{
		delete[] InnerStreamBuffer;
		InnerStreamBuffer = NULL;
	}

	Allocated = false;
}

void ZEALSoundSource::ResetStream()
{	
	StreamPosition = GetCurrentPosition();
	if (StreamPosition > GetEffectiveEndPosition())
		StreamPosition = (StreamPosition - GetEffectiveEndPosition()) + GetEffectiveStartPosition();

	StreamDecodeAndFill((ZESize)ALBuffer1, StreamPosition, BufferSampleCount);
	StreamPosition += BufferSampleCount;
	if (StreamPosition > GetEffectiveEndPosition())
		StreamPosition = (StreamPosition - GetEffectiveEndPosition()) + GetEffectiveStartPosition();

	StreamDecodeAndFill((ZESize)ALBuffer2, StreamPosition, BufferSampleCount);
	StreamPosition += BufferSampleCount;
	if (StreamPosition > GetEffectiveEndPosition())
		StreamPosition = (StreamPosition - GetEffectiveEndPosition()) + GetEffectiveStartPosition();

	OldBufferPosition = 0;
	alSourcei(ALSource, AL_SAMPLE_OFFSET, 0);
	alSourcei(ALSource, AL_BUFFER, NULL);
	alSourceQueueBuffers(ALSource, 1, &ALBuffer1);
	alSourceQueueBuffers(ALSource, 1, &ALBuffer2);
}

void ZEALSoundSource::StreamDecodeAndFill(ZESize BufferPosition, ZESize Position, ZESize SampleCount)
{
	if (Position + SampleCount < GetEffectiveEndPosition())
	{
		GetSoundResource()->Decode(InnerStreamBuffer, Position, SampleCount);
	}
	else
	{
		SampleCount = GetEffectiveEndPosition() - Position;
		GetSoundResource()->Decode((void*)InnerStreamBuffer, Position, SampleCount);

		ZESize RemainingSampleCount = BufferSampleCount - SampleCount;
		void* RemainingSampleBuffer = ((ZEUInt8*)InnerStreamBuffer) + SampleCount * GetSoundResource()->GetBlockAlign();

		if (GetLooping())
		{
			GetSoundResource()->Decode(RemainingSampleBuffer, GetEffectiveStartPosition(), RemainingSampleCount);
		}
		else
		{
			memset(RemainingSampleBuffer, (GetSoundResource()->GetBitsPerSample() == 8 ? 0x80 : 0x00), RemainingSampleCount * GetSoundResource()->GetBlockAlign());
		}
	}


	alBufferData((ALuint)BufferPosition, GetBufferFormat(GetSoundResource()), InnerStreamBuffer, (ALsizei)(BufferSampleCount * GetSoundResource()->GetBlockAlign()), (ALsizei)GetSoundResource()->GetSamplesPerSecond());
}

void ZEALSoundSource::Stream()
{
	ALint BufferCount;
	ALuint Buffer;
	alGetSourcei(ALSource, AL_BUFFERS_PROCESSED, &BufferCount);
	for (ZEInt I = 0; I < BufferCount; I++)
	{
		alSourceUnqueueBuffers(ALSource, 1, &Buffer);
		alDeleteBuffers(1, &Buffer);
		alGenBuffers(1, &Buffer);

		StreamDecodeAndFill((ZESize)Buffer, StreamPosition, BufferSampleCount);
		StreamPosition += BufferSampleCount;
		if (StreamPosition > GetEffectiveEndPosition())
		{
			StreamPosition = (StreamPosition - GetEffectiveEndPosition()) + GetEffectiveStartPosition();
		}

		alSourceQueueBuffers(ALSource, 1, &Buffer);
	}
}

ZEALSoundSource::ZEALSoundSource()
{
	InnerStreamBuffer = NULL;
	ALBuffer1 = NULL;
	ALBuffer2 = NULL;
	ALSource = NULL;

	Allocated = false;
}

ZEALSoundSource::~ZEALSoundSource()
{
	GetModule()->SoundSources.RemoveValue(this);
	DestroyBufferSource();
}

void ZEALSoundSource::SetCurrentPosition(ZESize SampleIndex)
{
	if (GetSoundResource() != NULL)
	{
		if (SampleIndex > GetSoundResource()->GetSampleCount())
		{
			SampleIndex = GetSoundResource()->GetSampleCount();
		}

		if (IsStreaming())
		{
			ZESoundSource::SetCurrentPosition(SampleIndex);
			ResetStream();
		}
		else
		{
			alSourcei(ALSource, AL_SAMPLE_OFFSET, (ALint)SampleIndex);
		}
	}
}

void ZEALSoundSource::SetSpeed(float Speed)
{
	if (GetSpeed() == Speed)
		return;

	ZESoundSource::SetSpeed(Speed);

	UpdateParameters();
}

void ZEALSoundSource::SetVolume(float Volume)
{
	if (GetVolume() == Volume)
		return;

	ZESoundSource::SetVolume(Volume);
	
	UpdateParameters();
}

void ZEALSoundSource::SetPan(float Pan)
{
	if (GetPan() == Pan)
		return;

	ZESoundSource::SetPan(Pan);

	UpdateParameters();
}

void ZEALSoundSource::SetLooping(bool Enabled)
{
	if (GetLooping() == Enabled)
		return;

	ZESoundSource::SetLooping(Enabled);

	UpdateParameters();
}

void ZEALSoundSource::SetMinDistance(float Distance)
{
	if (GetMinDistance() == Distance)
		return;

	ZESoundSource::SetMinDistance(Distance);

	UpdateParameters();
}

void ZEALSoundSource::SetMaxDistance(float Distance)
{
	if (GetMaxDistance() == Distance)
		return;

	ZESoundSource::SetMaxDistance(Distance);

	UpdateParameters();
}

void ZEALSoundSource::SetConeInsideAngle(float Angle)
{
	if (GetConeInsideAngle() == Angle)
		return;

	ZESoundSource::SetConeInsideAngle(Angle);

	UpdateParameters();
}

void ZEALSoundSource::SetConeOutsideAngle(float Angle)
{
	if (GetConeOutsideAngle() == Angle)
		return;

	ZESoundSource::SetConeInsideAngle(Angle);

	UpdateParameters();
}

void ZEALSoundSource::SetConeOutsideVolume(float Volume)
{
	if (GetConeOutsideVolume() == Volume)
		return;

	ZESoundSource::SetConeOutsideVolume(Volume);

	UpdateParameters();
}

void ZEALSoundSource::Play()
{
	if (GetState() == ZE_SSS_PLAYING || GetState() == ZE_SSS_NONE)
		return;

	if (Allocated)
	{
		SetSoundSourceState(ZE_SSS_PLAYING);
		
		if (IsStreaming())
		{
			ResetStream();
		}
		else
		{
			OldBufferPosition = GetCurrentPosition();
			alBufferi(ALSource, AL_SAMPLE_OFFSET, (ALint)OldBufferPosition);
		}

		alSourcePlay(ALSource);
	}
}

void ZEALSoundSource::Pause()
{
	if (GetSoundSourceState() == ZE_SSS_PLAYING)
		return;

	if (Allocated)
	{
		SetSoundSourceState(ZE_SSS_PAUSED);
		alSourceStop(ALSource);
	}
}

void ZEALSoundSource::Stop()
{
	if (GetSoundSourceState() == ZE_SSS_STOPPED)
		return;

	if (Allocated)
	{
		alSourceStop(ALSource);

		if (IsStreaming())
			ZESoundSource::SetCurrentPosition(GetEffectiveStartPosition());
		else
			alSourcei(ALSource, AL_SAMPLE_OFFSET, (ALint)GetEffectiveStartPosition());

		SetSoundSourceState(ZE_SSS_STOPPED);
	}
}

void ZEALSoundSource::Update(float ElapsedTime)
{
	UpdateVelocity(ElapsedTime);

	if (!Allocated || GetSoundSourceState() != ZE_SSS_PLAYING)
		return;

	ALint ALBufferPosition;
	alGetSourcei(ALSource, AL_SAMPLE_OFFSET, &ALBufferPosition);
	BufferPosition = (ZESize)ALBufferPosition;

	// Update CurrentPosition
	bool BufferRewinded = false;

	if (IsStreaming())
	{
		if (BufferPosition >= OldBufferPosition)
		{
			ZESoundSource::SetCurrentPosition(GetCurrentPosition() + BufferPosition - OldBufferPosition);
		}
		else
		{
			BufferRewinded = true;
			ZESoundSource::SetCurrentPosition(GetCurrentPosition() + BufferSampleCount - OldBufferPosition + BufferPosition);
		}

		OldBufferPosition = BufferPosition;
	}
	else
	{
		if (OldBufferPosition > BufferPosition)
			BufferRewinded = true;

		ZESoundSource::SetCurrentPosition(BufferPosition);
		OldBufferPosition = BufferPosition;
	}

	// Check Limits
	if (GetCurrentPosition() < GetEffectiveStartPosition() || 
		GetCurrentPosition() > GetEffectiveEndPosition() || 
		(!IsStreaming() && BufferRewinded && (GetEffectiveStartPosition() < GetEffectiveEndPosition())))
	{
		if (GetLooping())
		{
			SetCurrentPosition(GetEffectiveStartPosition());
		}
		else
		{
			Stop();
			return;
		}
	}

	// Stream
	if (IsStreaming())
		Stream();
}
