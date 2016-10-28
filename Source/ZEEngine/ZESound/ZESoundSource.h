//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundSource.h
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

#pragma once

#include "ZETypes.h"
#include "ZEGame/ZEEntity.h"
#include "ZESoundResource.h"

#include "ZEMeta/ZEObject.h"

#define ZE_SS_PAN_LEFT				-100
#define ZE_SS_PAN_MIDDLE			0
#define ZE_SS_PAN_RIGHT				100

#define ZE_SS_VOLUME_MAX			100
#define ZE_SS_VOLUME_MIN			0

#define ZE_SS_FREQUENCY_DEFAULT		0

#define ZE_SS_MAX_TYPE 256

ZE_ENUM(ZESoundSourceType)
{
	ZE_SST_NONE					= 0,
	ZE_SST_EFFECT				= 1,
	ZE_SST_DIALOG				= 2,
	ZE_SST_MUSIC				= 3,
	ZE_SST_VIDEO				= 4,
	ZE_SST_PLAYER_COMM			= 5
};

ZE_ENUM(ZESoundSourceState)
{
	ZE_SSS_NONE,
	ZE_SSS_PLAYING,
	ZE_SSS_STOPPED,
	ZE_SSS_PAUSED,
}; 

class ZESoundSourceEffect;

class ZESoundSource : public ZEEntity
{
	ZE_OBJECT
	private:
		ZESize								StartPosition;
		ZESize								EndPosition;

	protected:
		ZEHolder<const ZESoundResource>		SoundResource;
		ZESoundSourceState					SoundSourceState;
		ZESoundSourceType					SoundSourceType;

		ZEInt								Pan;
		float								PlaybackSpeed;
		ZEUInt								Volume;
		bool								Looping;
		bool								Streaming;

		ZESize								CurrentPosition;
		ZESize								LocalOldPosition;

		ZESize								EffectiveStartPosition;
		ZESize								EffectiveEndPosition;
		bool								LimitsEnabled;

		void								ResetParameters();

											ZESoundSource();
		virtual 							~ZESoundSource();

	public:
		bool								IsStreaming() const;

		virtual void						SetSoundSourceState(ZESoundSourceState State) = 0;
		ZESoundSourceState					GetSoundSourceState() const;
	
		ZESoundSourceType					GetSoundSourceType();
		void								SetSoundSourceType(ZESoundSourceType Type);

		virtual void						SetCurrentPosition(ZESize SampleIndex);
		void								SetCurrentPositionTime(float Seconds);
		void								SetCurrentPositionPersentage(float Percent);

		ZESize								GetCurrentPosition() const;
		ZEUInt								GetCurrentPositionTime() const;
		float								GetCurrentPositionPersentage() const;

		void								SetLimitsEnabled(bool Enabled);
		bool								GetLimitsEnabled() const;

		virtual void						SetStartPosition(ZESize SampleIndex);
		void								SetStartPositionTime(float Seconds);
		void								SetStartPositionPersentage(float Percent);

		ZESize								GetStartPosition() const;
		float								GetStartPositionTime() const;
		float								GetStartPositionPersentage() const;

		virtual void						SetEndPosition(ZESize SampleIndex);
		void								SetEndPositionTime(float Seconds);
		void								SetEndPositionPercentage(float Percentage);

		ZESize								GetEndPosition() const;
		float								GetEndPositionTime() const;
		float								GetEndPositionPercentage() const;

		virtual void						SetLooping(bool Enabled) = 0;
		bool								GetLooping() const;

		ZESize								GetLoopingLength();
		float								GetLoopingLenghtTime();
		float								GetLoopingLenghtPercent();

		virtual void						SetPan(ZEInt NewPan) = 0;
		ZEInt								GetPan() const;
							
		virtual void						SetPlaybackSpeed(float Speed);
		float								GetPlaybackSpeed() const;
														
		virtual void						SetVolume(ZEUInt NewVolume) = 0;
		ZEUInt								GetVolume() const;
						
		virtual void						Play() = 0;
		virtual void						Resume() = 0;
		virtual void						Pause() = 0;
		virtual void						Stop() = 0;

		virtual void						SetSoundResource(ZEHolder<const ZESoundResource> Resource) = 0;
		ZEHolder<const ZESoundResource>		GetSoundResource() const;

		static ZESoundSource*				CreateInstance();
}
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZESound.png");
