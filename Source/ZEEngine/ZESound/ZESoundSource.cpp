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
#include "ZEFile\ZEPathInfo.h"
#include "ZEMath\ZEAngle.h"
#include "ZEMath\ZEMath.h"

void ZESoundSource::SetStreaming(bool Streaming)
{
	this->Streaming = Streaming;
}

ZEVector3 ZESoundSource::GetVelocity() const
{
	return Velocity;
}

ZESize ZESoundSource::GetEffectiveStartPosition() const
{
	return EffectiveStartPosition;
}

ZESize ZESoundSource::GetEffectiveEndPosition() const
{
	return EffectiveEndPosition;
}

float ZESoundSource::GetEffectiveVolume(float Volume) const
{
	return ZEMath::Saturate(ZEMath::Saturate(Volume) * zeSound->GetTypeVolume(GetSoundSourceType()));
}

void ZESoundSource::ApplyLimits()
{
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

void ZESoundSource::UpdateResource()
{
	ApplyLimits();
}

void ZESoundSource::UpdateVelocity(float ElapsedTime)
{
	ZEVector3 WorldPosition = GetWorldPosition();
	ZEVector3 NewVelocity = VelocityOldPosition - WorldPosition;
	ZEVector3::Lerp(NewVelocity, Velocity, NewVelocity, 8 * ElapsedTime);

	if (NewVelocity.LengthSquare() > 83 * 83)
		NewVelocity = NewVelocity.Normalize() * 83;

	Velocity = NewVelocity;
	VelocityOldPosition = WorldPosition;
}

ZEEntityResult ZESoundSource::InitializeInternal()
{
	ZE_ENTITY_INITIALIZE_CHAIN(ZEEntity);

	Velocity = ZEVector3::Zero;
	VelocityOldPosition = GetWorldPosition();

	if (GetAutoPlay())
		Play();

	return ZE_ER_DONE;
}

ZEEntityResult ZESoundSource::DeinitializeInternal()
{
	Stop();

	ZE_ENTITY_DEINITIALIZE_CHAIN(ZEEntity);
	
	return ZE_ER_DONE;
}

ZEEntityResult ZESoundSource::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	ZE_ENTITY_RESOURCE_FENCE_LOADED(SoundResource, ZE_ER_FAILED)
	{
		if (!SoundResourceExternal)
			SoundResource = ZESoundResource::LoadResourceShared(SoundFileName);
		
		return ZE_ER_WAIT;
	}

	UpdateResource();

	return ZE_ER_DONE;
}

ZEEntityResult ZESoundSource::UnloadInternal()
{
	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	
	if (!SoundResourceExternal)
		SoundResource.Release();

	return ZE_ER_DONE;
}

ZESoundSource::ZESoundSource()
{
	Velocity = ZEVector3::Zero;
	VelocityOldPosition = ZEVector3::Zero;
	Positional = true;
	SoundResourceExternal = false;
	SoundSourceState = ZE_SSS_NONE;
	SoundSourceType = ZE_SST_NONE;
	AutoPlay = false;
	Pan = 0.0f;
	Speed = 1.0f;
	Volume = 1.0f;
	Looping = false;
	Streaming = false;
	LimitsEnabled = false;
	CurrentPosition = 0;
	StartPosition = 0;
	EndPosition = 0;
	SoundResource = NULL;
	LocalOldPosition = 0;
	EffectiveStartPosition = 0;
	EffectiveEndPosition = 0;
	MinDistance = 0.0f;
	MaxDistance = 100.0f;
	ConeInsideAngle = ZE_PIx2;
	ConeOutsideAngle = ZE_PIx2;
	ConeOutsideVolume = 1.0f;
}

ZESoundSource::~ZESoundSource()
{

}

bool ZESoundSource::IsStreaming() const
{
	return Streaming;
}

void ZESoundSource::SetPositional(bool Enabled)
{
	Positional = Enabled;
}

bool ZESoundSource::GetPositional() const
{
	return Positional;
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
}

ZESoundSourceType ZESoundSource::GetSoundSourceType() const
{
	return SoundSourceType;
}

void ZESoundSource::SetCurrentPosition(ZESize SampleIndex)
{
	StartPosition = SampleIndex;
}

void ZESoundSource::SetCurrentPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		ZESize SampleIndex = (ZESize)((float)SoundResource->GetSamplesPerSecond() * Seconds);	
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
			SetCurrentPosition((ZESize)(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage));
	}
}

ZESize ZESoundSource::GetCurrentPosition() const
{
	return CurrentPosition;
}

ZEUInt ZESoundSource::GetCurrentPositionTime() const
{
	if (SoundResource != NULL)
		return (ZEUInt)(CurrentPosition / SoundResource->GetSamplesPerSecond());
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

	ApplyLimits();
}

bool ZESoundSource::GetLimitsEnabled() const
{
	return LimitsEnabled;
}

void ZESoundSource::SetStartPosition(ZESize SampleIndex)
{
	StartPosition = SampleIndex;

	if (SoundResource != NULL)
	{
		if (LimitsEnabled)
			EffectiveStartPosition = StartPosition % SoundResource->GetSampleCount();
	}
}

void ZESoundSource::SetStartPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		ZESize SampleIndex = (ZESize)((float)SoundResource->GetSamplesPerSecond() * Seconds);

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
			SetStartPosition((ZESize)(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage));
	}
}

ZESize ZESoundSource::GetStartPosition() const
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

void ZESoundSource::SetEndPosition(ZESize SampleIndex)
{
	EndPosition = SampleIndex;

	if (SoundResource != NULL)
	{
		if (LimitsEnabled)
			EffectiveEndPosition = EndPosition % SoundResource->GetSampleCount();
	}
}

void ZESoundSource::SetEndPositionTime(float Seconds)
{
	if (SoundResource != NULL)
	{
		ZESize SampleIndex = (ZESize)((float)SoundResource->GetSamplesPerSecond() * Seconds);

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
			SetEndPosition((ZESize)(((float)SoundResource->GetSampleCount() / 100.0f) * Percentage));
	}
}

ZESize ZESoundSource::GetEndPosition() const
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

float ZESoundSource::GetEndPositionPercentage() const
{
	if (SoundResource != NULL)
		return ((float)EndPosition / SoundResource->GetSampleCount()) * 100.0f;
	else
		return 0.0f;
}

void ZESoundSource::SetPan(float Pan)
{
	this->Pan = Pan;
}	

float ZESoundSource::GetPan() const
{
	return Pan;
}

void ZESoundSource::SetSpeed(float Speed)
{
	this->Speed = Speed;
}

float ZESoundSource::GetSpeed() const
{
	return Speed;
}

void ZESoundSource::SetVolume(float Volume)
{
	this->Volume = Volume;
}

float ZESoundSource::GetVolume() const
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

void ZESoundSource::SetAutoPlay(bool AutoPlay)
{
	AutoPlay = true;
}

bool ZESoundSource::GetAutoPlay() const
{
	return AutoPlay;
}

ZESize ZESoundSource::GetLoopingLength() const
{
	if (StartPosition < EndPosition)
		return EndPosition - StartPosition;
	else
		return StartPosition + (SoundResource->GetSampleCount() - EndPosition);
}

float ZESoundSource::GetLoopingLenghtTime() const
{
	return (float)GetLoopingLength() / (float)SoundResource->GetSamplesPerSecond();
}

float ZESoundSource::GetLoopingLenghtPercent() const
{
	return (GetLoopingLength() / SoundResource->GetSampleCount()) * 100.0f;
}

void ZESoundSource::SetMinDistance(float Distance)
{
	MinDistance = MinDistance;
}


float ZESoundSource::GetMinDistance() const
{
	return MinDistance;
}

void ZESoundSource::SetMaxDistance(float Distance)
{
	MaxDistance = Distance;
}

float ZESoundSource::GetMaxDistance() const
{
	return MaxDistance;
}

void ZESoundSource::SetConeInsideAngle(float Angle)
{
	ConeInsideAngle = Angle;
}

float ZESoundSource::GetConeInsideAngle() const
{
	return ConeInsideAngle;
}

void ZESoundSource::SetConeOutsideAngle(float Angle)
{
	ConeOutsideAngle = Angle;
}

float ZESoundSource::GetConeOutsideAngle() const
{
	return ConeOutsideAngle;
}

void ZESoundSource::SetConeOutsideVolume(float Volume)
{
	ConeOutsideVolume = Volume;
}

float ZESoundSource::GetConeOutsideVolume() const
{
	return ConeOutsideVolume;
}


void ZESoundSource::Replay()
{
	Stop();
	Play();
}

void ZESoundSource::SetSoundFileName(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(SoundFileName, FileName))
		return;

	SoundFileName = FileName;
	
	if (IsLoadedOrLoading() && !SoundResourceExternal)
		Reload();
}

const ZEString& ZESoundSource::GetSoundFileName() const
{
	return SoundFileName;
}

void ZESoundSource::SetSoundResource(ZEHolder<const ZESoundResource> Resource)
{
	if(SoundResource == Resource)
		return;

	SoundResource = Resource;
	SoundResourceExternal = (Resource != NULL);

	if (IsLoaded())
		UpdateResource();
}

ZEHolder<const ZESoundResource> ZESoundSource::GetSoundResource() const
{
	return SoundResource;
}

ZESoundSource* ZESoundSource::CreateInstance()
{
	return zeSound->CreateSoundSource();
}
