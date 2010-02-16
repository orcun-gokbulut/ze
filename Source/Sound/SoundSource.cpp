//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundSource.cpp
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

#include "SoundSource.h"
#include "SoundModule.h"

ZESoundSource::ZESoundSource()
{
	SoundSourceState = ZE_SSS_NONE;
	Pan = ZE_SS_PAN_MIDDLE;
	Frequency = ZE_SS_FREQUENCY_DEFAULT;
	Volume = ZE_SS_VOLUME_MAX;

	Loop = false;
	Streaming = false;
	CurrentCursor = 0;
	StartCursor = 0;
	EndCursor = 0;
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

void ZESoundSource::SetCurrentCursor(unsigned int SampleIndex)
{
	StartCursor = SampleIndex;
}

void ZESoundSource::SetCurrentCursorTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;
		if (SampleIndex > SoundResource->GetSampleCount())
			SetStartCursor(SoundResource->GetSampleCount());
	}
}

void ZESoundSource::SetCurrentCursorPersentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetCurrentCursor(0);
		else if (Percentage > 100.0f)
			SetCurrentCursor(SoundResource->GetSampleCount());
		else
			SetCurrentCursor((SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetCurrentCursor() const
{
	return CurrentCursor;
}

unsigned int ZESoundSource::GetCurrentCursorTime() const
{
	if (SoundResource != NULL)
		return (float)CurrentCursor / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetCurrentCursorPersentage() const
{
	if (SoundResource != NULL)
		return ((float)CurrentCursor / (float)SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetStartCursor(unsigned int SampleIndex)
{
	if (SoundResource == NULL)
		if (StartCursor > SoundResource->GetSampleCount())
			StartCursor = SoundResource->GetSampleCount();
}

void ZESoundSource::SetStartCursorTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;

		if (SampleIndex > SoundResource->GetSampleCount())
			SetStartCursor(SoundResource->GetSampleCount());
		else
			SetStartCursor(SampleIndex);
	}
}

void ZESoundSource::SetStartCursorPersentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetStartCursor(0);
		else
			SetStartCursor(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetStartCursor() const
{
	return StartCursor;
}

float ZESoundSource::GetStartCursorTime() const
{
	if (SoundResource != NULL)
		return (float)StartCursor / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetStartCursorPersentage() const
{
	if (SoundResource != NULL)
		return (StartCursor / SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetEndCursor(unsigned int SampleIndex)
{
	if (SoundResource != NULL)
	{
		if (SampleIndex > SoundResource->GetSampleCount())
			EndCursor = SampleIndex;
	}
}

void ZESoundSource::SetEndCursorTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		unsigned int SampleIndex = (float)SoundResource->GetSamplesPerSecond() * Seconds;

		if (SampleIndex > SoundResource->GetSampleCount())
			SetEndCursor(SoundResource->GetSampleCount());
		else
			SetEndCursor(SampleIndex);
	}
}

void ZESoundSource::SetEndCursorPercentage(float Percentage)
{
	if (SoundResource != NULL)
	{
		if (Percentage < 0.0f)
			SetEndCursor(0);
		else
			SetEndCursor(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage);
	}
}

unsigned int ZESoundSource::GetEndCursor() const
{
	return EndCursor;
}

float ZESoundSource::GetEndCursorTime() const
{
	if (SoundResource != NULL)
		return (float)EndCursor / (float)SoundResource->GetSamplesPerSecond();
	else
		return 0;
}

float ZESoundSource::GetEndCursorPersentage() const
{
	if (SoundResource != NULL)
		return (EndCursor / SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetPan(int NewPan)
{
	Pan = NewPan;
}	

int ZESoundSource::GetPan() const
{
	return Pan;
}

void ZESoundSource::SetFrequency(unsigned int NewFrequency)
{
	Frequency = NewFrequency;
}

unsigned int ZESoundSource::GetFrequency() const		
{
	return Frequency;
}

void ZESoundSource::SetPlaybackSpeed(float Speed)
{
	SetFrequency(Speed * SoundResource->GetSamplesPerSecond());
}

float ZESoundSource::GetPlaybackSpeed() const
{
	if (SoundResource != NULL)
		return Frequency / SoundResource->GetSamplesPerSecond();
	else
		return 1.0f;
}

void ZESoundSource::SetVolume(unsigned int NewVolume)
{
	Volume = NewVolume;
}

unsigned int ZESoundSource::GetVolume() const
{
	return Volume;
}

void ZESoundSource::SetLoop(bool Enabled)
{
	Loop = Enabled;
}

bool ZESoundSource::GetLoop() const
{
	return Loop;
}

void ZESoundSource::Destroy()
{
	delete this;
}

void ZESoundSource::SetSoundResource(ZESoundResource* Resource)
{
	if (SoundResource != NULL)
		SoundResource->Release();

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


#include "SoundSource.h.zpp"
