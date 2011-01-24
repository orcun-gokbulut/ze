//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundSource.cpp
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

#include "ZESoundSource.h"
#include "ZESoundModule.h"

ZESoundSource::ZESoundSource()
{
	SoundSourceState = ZE_SSS_NONE;
	SoundSourceType = ZE_SST_NONE;
	Pan = ZE_SS_PAN_MIDDLE;
	PlaybackSpeed = 1.0f;
	Volume = ZE_SS_VOLUME_MAX;
	Looping = false;
	Streaming = false;
	LimitsEnabled = false;
	CurrentPosition = 0;
	StartPosition = 0;
	EndPosition = 0;
	SoundResource = NULL;
}

ZESoundSource::~ZESoundSource()
{
	if (SoundResource != NULL)
		SoundResource->Release();
}

bool ZESoundSource::IsStreaming() const
{
	return Streaming;
}

void ZESoundSource::SetSoundSourceState(ZESoundSourceState State)		
{
	SoundSourceState = State;
}

ZESoundSourceState ZESoundSource::GetSoundSourceState() const		
{
	return SoundSourceState;
}

void ZESoundSource::SetSoundSourceType(ZESoundSourceType Type)
{
	SoundSourceType = Type;
	
	// Update Volume
	SetVolume(GetVolume());
}

ZESoundSourceType ZESoundSource::GetSoundSourceType()
{
	return SoundSourceType;
}

void ZESoundSource::SetCurrentPosition(unsigned int SampleIndex)
{
	StartPosition = SampleIndex;
}

void ZESoundSource::SetCurrentPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;
		if (SampleIndex < SoundResource->GetSampleCount())
			SetCurrentPosition(SampleIndex);
	}
}

void ZESoundSource::SetCurrentPositionPersentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetCurrentPosition(0);
		else if (Percentage > 100.0f)
			SetCurrentPosition(SoundResource->GetSampleCount());
		else
			SetCurrentPosition((SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetCurrentPosition() const
{
	return CurrentPosition;
}

unsigned int ZESoundSource::GetCurrentPositionTime() const
{
	if (SoundResource != NULL)
		return (float)CurrentPosition / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetCurrentPositionPersentage() const
{
	if (SoundResource != NULL)
		return ((float)CurrentPosition / (float)SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetLimitsEnabled(bool Enabled)
{
	LimitsEnabled = Enabled;

	if (!LimitsEnabled)
	{
		EffectiveStartPosition = 0;
		EffectiveEndPosition = SoundResource->GetSampleCount();
	}
	else
	{
		EffectiveStartPosition = StartPosition % SoundResource->GetSampleCount();	
		EffectiveEndPosition = EndPosition % SoundResource->GetSampleCount();	
	}


}

bool ZESoundSource::GetLimitsEnabled() const
{
	return LimitsEnabled;
}

void ZESoundSource::SetStartPosition(unsigned int SampleIndex)
{
	StartPosition = SampleIndex;

	if (SoundResource != NULL)
		if (LimitsEnabled)
			EffectiveStartPosition = StartPosition % SoundResource->GetSampleCount();
}

void ZESoundSource::SetStartPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;

		if (SampleIndex > SoundResource->GetSampleCount())
			SetStartPosition(SoundResource->GetSampleCount());
		else
			SetStartPosition(SampleIndex);
	}
}

void ZESoundSource::SetStartPositionPersentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetStartPosition(0);
		else
			SetStartPosition(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetStartPosition() const
{
	return StartPosition;
}

float ZESoundSource::GetStartPositionTime() const
{
	if (SoundResource != NULL)
		return (float)StartPosition / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetStartPositionPersentage() const
{
	if (SoundResource != NULL)
		return ((float)StartPosition / SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetEndPosition(unsigned int SampleIndex)
{
	EndPosition = SampleIndex;

	if (SoundResource != NULL)
		if (LimitsEnabled)
			EffectiveEndPosition = EndPosition % SoundResource->GetSampleCount();
}

void ZESoundSource::SetEndPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;

		if (SampleIndex > SoundResource->GetSampleCount())
			SetEndPosition(SoundResource->GetSampleCount());
		else
			SetEndPosition(SampleIndex);
	}
}

void ZESoundSource::SetEndPositionPercentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetEndPosition(0);
		else
			SetEndPosition(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetEndPosition() const
{
	return EndPosition;
}

float ZESoundSource::GetEndPositionTime() const
{
	if (SoundResource != NULL)
		return (float)EndPosition / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetEndPositionPersentage() const
{
	if (SoundResource != NULL)
		return ((float)EndPosition / SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetPan(int NewPan)
{
	if (Pan > ZE_SS_PAN_RIGHT)
		Pan = 100;
	else if (Pan < ZE_SS_PAN_LEFT)
		Pan = -100;
	else
		Pan = NewPan;
}	

int ZESoundSource::GetPan() const
{
	return Pan;
}

void ZESoundSource::SetPlaybackSpeed(float Speed)
{
	PlaybackSpeed = Speed;
}

float ZESoundSource::GetPlaybackSpeed() const
{
	return PlaybackSpeed;
}

void ZESoundSource::SetVolume(unsigned int NewVolume)
{
	if (NewVolume > ZE_SS_VOLUME_MAX)
		NewVolume = ZE_SS_VOLUME_MAX;

	Volume = (float)NewVolume * ((float)zeSound->GetTypeVolume(SoundSourceType) / (float)ZE_SS_VOLUME_MAX);
}

unsigned int ZESoundSource::GetVolume() const
{
	return Volume;
}

void ZESoundSource::SetLooping(bool Enabled)
{
	Looping = Enabled;
}

bool ZESoundSource::GetLooping() const
{
	return Looping;
}

size_t ZESoundSource::GetLoopingLength()
{
	if (StartPosition < EndPosition)
		return EndPosition - StartPosition;
	else
		return StartPosition + (SoundResource->GetSampleCount() - EndPosition);
}

float ZESoundSource::GetLoopingLenghtTime()
{
	return GetLoopingLength() / SoundResource->GetSamplesPerSecond();
}

float ZESoundSource::GetLoopingLenghtPercent()
{
	return (GetLoopingLength() / SoundResource->GetSampleCount()) * 100.0f;
}

void ZESoundSource::Destroy()
{
	delete this;
}

void ZESoundSource::SetSoundResource(ZESoundResource* Resource)
{
	if (SoundResource != NULL)
		SoundResource->Release();

	if (!LimitsEnabled)
	{
		EffectiveStartPosition = 0;
		EffectiveEndPosition = SoundResource->GetSampleCount();
	}

	Resource->AddReferance();
	SoundResource = Resource;
}

ZESoundResource* ZESoundSource::GetSoundResource() const
{
	return SoundResource;
}

ZESoundSource* ZESoundSource::CreateInstance()
{
	return zeSound->CreateSoundSource();
}

#include "ZESoundSource.h.zpp"

ZEEntityRunAt ZESoundSourceDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
