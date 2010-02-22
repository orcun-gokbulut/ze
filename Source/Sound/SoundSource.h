//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundSource.h
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
#ifndef	__ZE_SOUND_SOURCE_H__
#define __ZE_SOUND_SOURCE_H__

#include "Core/Component.h"
#include "Meta/Class.h"
#include "SoundResource.h"

#define ZE_SS_PAN_LEFT				-100
#define ZE_SS_PAN_MIDDLE			0
#define ZE_SS_PAN_RIGHT				100

#define ZE_SS_VOLUME_MAX			100
#define ZE_SS_VOLUME_MIN			0

#define ZE_SS_FREQUENCY_DEFAULT		0

#define ZE_SS_MAX_TYPE 256

enum ZESoundSourceType
{
	ZE_SST_NONE					= 0,
	ZE_SST_EFFECT				= 1,
	ZE_SST_DIALOG				= 2,
	ZE_SST_MUSIC				= 3,
	ZE_SST_VIDEO				= 4,
	ZE_SST_PLAYER_COMM			= 5
};

enum  ZESoundSourceState
{
	ZE_SSS_NONE,
	ZE_SSS_PLAYING,
	ZE_SSS_STOPPED,
	ZE_SSS_PAUSED,
}; 

class ZESoundSourceEffect;

ZE_META_CLASS_DESCRIPTION(ZESoundSource);

class ZESoundSource : public ZEComponent
{
	ZE_META_CLASS()
	protected:
		ZESoundResource*			SoundResource;
		ZESoundSourceState			SoundSourceState;
		ZESoundSourceType			SoundSourceType;

		int							Pan;
		unsigned int				Frequency;
		unsigned int				Volume;
		bool						Loop;
		bool						Limited;
		bool						Streaming;

		unsigned int				CurrentPosition;
		unsigned int				OldPosition;
		unsigned int				StartPosition;
		unsigned int				EndPosition;

									ZESoundSource();
		virtual 					~ZESoundSource();

	public:
		virtual bool				IsStreaming() const;

		virtual void				SetSoundSourceState(ZESoundSourceState State);
		ZESoundSourceState			GetSoundSourceState() const;
	
		ZESoundSourceType			GetSoundSourceType();
		void						SetSoundSourceType(ZESoundSourceType Type);

		virtual void				SetCurrentPosition(unsigned int SampleIndex);
		void						SetCurrentPositionTime(float Seconds);
		void						SetCurrentPositionPersentage(float Percent);

		virtual unsigned int		GetCurrentPosition() const;
		unsigned int				GetCurrentPositionTime() const;
		float						GetCurrentPositionPersentage() const;

		virtual void				SetStartPosition(unsigned int SampleIndex);
		void						SetStartPositionTime(float Seconds);
		void						SetStartPositionPersentage(float Percent);

		unsigned int				GetStartPosition() const;
		float						GetStartPositionTime() const;
		float						GetStartPositionPersentage() const;

		virtual void				SetEndPosition(unsigned int SampleIndex);
		void						SetEndPositionTime(float Seconds);
		void						SetEndPositionPercentage(float Percentage);

		unsigned int				GetEndPosition() const;
		float						GetEndPositionTime() const;
		float						GetEndPositionPersentage() const;

		virtual void				SetLoop(bool Enabled);
		bool						GetLoop() const;

		size_t						GetLoopLength();
		float						GetLoopLenghtTime();
		float						GetLoopLenghtPercent();

		virtual void				SetPan(int NewPan);
		int							GetPan() const;
							
		void						SetPlaybackSpeed(float Speed);
		float						GetPlaybackSpeed() const;
							
		virtual void				SetFrequency(unsigned int NewFrequency);
		unsigned int				GetFrequency() const;
							
		virtual void				SetVolume(unsigned int NewVolume);
		unsigned int				GetVolume() const;
						
		virtual void				Play() = 0;
		virtual void				Resume() = 0;
		virtual void				Pause() = 0;
		virtual void				Stop() = 0;
						
		void						Destroy();

		virtual void				SetSoundResource(ZESoundResource* Resource) = 0;
		ZESoundResource*			GetSoundResource() const;

		static ZESoundSource*		CreateInstance();
};

/*
ZE_POSTPROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZESoundSource" parent="ZEComponent" noinstance="true">
			<description>Sound Source</description>
			<property name="CurrentPosition" type="integer" autogetset="yes">
				<constraints>
					<minvalue value="0"/>
				</constraints>
			</property>
			<property name="Volume" type="integer" autogetset="yes">
				<constraints>
					<minvalue value="0"/>
					<maxvalue value="100"/>
				</constraints>
			</property>
			<property name="Pan" type="integer" autogetset="yes">
				<constraints>
					<minvalue value="-100"/>
					<maxvalue value="100"/>
				</constraints>
			</property>
			<property name="PlaybackSpeed" type="float" autogetset="yes">
				<constraints>
					<minvalue value="0.1f"/>
				</constraints>
			</property>
			<property name="Loop" type="boolean" autogetset="yes"/>
			<property name="StartPosition" type="integer" autogetset="yes"/>
			<property name="EndPosition" type="integer" autogetset="yes"/>
			<property name="SoundSourceState" type="integer" autogetset="yes">
				<enumurator name="ZESoundSourceState">
					<item name="Playing" value="ZE_SSS_PLAYING"/>
					<item name="Stopped" value="ZE_SSS_STOPPED"/>
					<item name="Paused" value="ZE_SSS_PAUSED"/>
				</enumurator>
			</property>
			<container name="Effects" type="ZESoundEffect"/>
		</class>
	</meta>
</zinek>
ZE_POSTPROCESSOR_END()
*/
#endif
