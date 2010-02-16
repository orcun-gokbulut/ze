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

#include "Meta/Class.h"
#include "SoundResource.h"

#define ZE_SS_PAN_LEFT				-100
#define ZE_SS_PAN_MIDDLE			0
#define ZE_SS_PAN_RIGHT				100

#define ZE_SS_VOLUME_MAX			100
#define ZE_SS_VOLUME_MIN			0

#define ZE_SS_FREQUENCY_DEFAULT		0


enum  ZESoundSourceState
{
	ZE_SSS_NONE,
	ZE_SSS_PLAYING,
	ZE_SSS_STOPPED,
	ZE_SSS_PAUSED,
}; 

ZE_META_CLASS_DESCRIPTION(ZESoundSource);

class ZESoundSource : public ZEClass
{
	ZE_META_CLASS()
	protected:
		ZESoundResource*			SoundResource;
		ZESoundSourceState			SoundSourceState;

		int							Pan;
		unsigned int				Frequency;
		unsigned int				Volume;
		bool						Loop;
		bool						Limited;
		bool						Streaming;

		unsigned int				CurrentCursor;
		unsigned int				StartCursor;
		unsigned int				EndCursor;

									ZESoundSource();
		virtual 					~ZESoundSource();

	public:
		virtual bool				IsStreaming() const;

		virtual void				SetSoundSourceState(ZESoundSourceState State);
		ZESoundSourceState			GetSoundSourceState() const;
	
		virtual void				SetCurrentCursor(unsigned int SampleIndex);
		void						SetCurrentCursorTime(float Seconds);
		void						SetCurrentCursorPersentage(float Percent);

		virtual unsigned int		GetCurrentCursor() const;
		unsigned int				GetCurrentCursorTime() const;
		float						GetCurrentCursorPersentage() const;

		virtual void				SetStartCursor(unsigned int SampleIndex);
		void						SetStartCursorTime(float Seconds);
		void						SetStartCursorPersentage(float Percent);

		unsigned int				GetStartCursor() const;
		float						GetStartCursorTime() const;
		float						GetStartCursorPersentage() const;

		virtual void				SetEndCursor(unsigned int SampleIndex);
		void						SetEndCursorTime(float Seconds);
		void						SetEndCursorPercentage(float Percentage);

		unsigned int				GetEndCursor() const;
		float						GetEndCursorTime() const;
		float						GetEndCursorPersentage() const;

		virtual void				SetLoop(bool Enabled);
		bool						GetLoop() const;

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
		<class name="ZESoundSource" noinstance="true">
			<description>Sound Source</description>
			<property name="CurrentCursor" type="integer" autogetset="yes">
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
			<property name="StartCursor" type="integer" autogetset="yes"/>
			<property name="EndCursor" type="integer" autogetset="yes"/>
			<property name="SoundSourceState" type="integer" autogetset="yes">
				<enumurator name="ZESoundSourceState">
					<item name="Playing" value="ZE_SSS_PLAYING"/>
					<item name="Stopped" value="ZE_SSS_STOPPED"/>
					<item name="Paused" value="ZE_SSS_PAUSED"/>
				</enumurator>
			</property>
		</class>
	</meta>
</zinek>
ZE_POSTPROCESSOR_END()
*/
#endif
