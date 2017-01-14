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

#define ZE_SS_MAX_TYPE 6

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

		ZEString							SoundFileName;
		ZEHolder<const ZESoundResource>		SoundResource;
		bool								SoundResourceExternal;
		ZESoundSourceState					SoundSourceState;
		ZESoundSourceType					SoundSourceType;
		
		bool								Positional;
		float								Pan;
		float								Speed;
		float								Volume;
		bool								Looping;
		bool								AutoPlay;
		bool								Streaming;
		ZEVector3							Velocity;
		ZEVector3							VelocityOldPosition;
		float								MinDistance;
		float								MaxDistance;
		float								ConeInsideAngle;
		float								ConeOutsideAngle;
		ZEVector3							ConeDirection;
		float								ConeOutsideVolume;

		ZESize								CurrentPosition;
		ZESize								LocalOldPosition;
		
		ZESize								EffectiveStartPosition;
		ZESize								EffectiveEndPosition;
		bool								LimitsEnabled;

	protected:
		void								SetSoundSourceState(ZESoundSourceState State);
		void								SetStreaming(bool Streaming);

		ZEVector3							GetVelocity() const;

		ZESize								GetEffectiveStartPosition() const;
		ZESize								GetEffectiveEndPosition() const;
		float								GetEffectiveVolume(float Volume) const;
		
		void								ApplyLimits();
		virtual void						UpdateResource();
		void								UpdateParameters();
		void								UpdateVelocity(float ElapsedTime);
		
		virtual ZEEntityResult				InitializeInternal() override;
		virtual ZEEntityResult				DeinitializeInternal()override;

		virtual ZEEntityResult				LoadInternal() override;
		virtual ZEEntityResult				UnloadInternal() override;

											ZESoundSource();
		virtual 							~ZESoundSource();

	public:
		ZESoundSourceState					GetSoundSourceState() const;
		bool								IsStreaming() const;

		virtual void						SetPositional(bool Enabled);
		bool								GetPositional() const;

		virtual void						SetSoundSourceType(ZESoundSourceType Type);
		ZESoundSourceType					GetSoundSourceType() const;

		virtual void						SetCurrentPosition(ZESize SampleIndex);
		ZESize								GetCurrentPosition() const;
		void								SetCurrentPositionTime(float Seconds);
		ZEUInt								GetCurrentPositionTime() const;
		void								SetCurrentPositionPersentage(float Percent);
		float								GetCurrentPositionPersentage() const;

		void								SetLimitsEnabled(bool Enabled);
		bool								GetLimitsEnabled() const;

		virtual void						SetStartPosition(ZESize SampleIndex);
		ZESize								GetStartPosition() const;
		void								SetStartPositionTime(float Seconds);
		float								GetStartPositionTime() const;
		void								SetStartPositionPersentage(float Percent);
		float								GetStartPositionPersentage() const;

		virtual void						SetEndPosition(ZESize SampleIndex);
		ZESize								GetEndPosition() const;
		void								SetEndPositionTime(float Seconds);
		float								GetEndPositionTime() const;
		void								SetEndPositionPercentage(float Percentage);
		float								GetEndPositionPercentage() const;

		virtual void						SetPan(float Pan);
		float								GetPan() const;
							
		virtual void						SetSpeed(float Speed);
		float								GetSpeed() const;
														
		virtual void						SetVolume(float Volume);
		float								GetVolume() const;

		virtual void						SetLooping(bool Enabled);
		bool								GetLooping() const;

		void								SetAutoPlay(bool AutoPlay);
		bool								GetAutoPlay() const;

		ZESize								GetLoopingLength() const;
		float								GetLoopingLenghtTime() const;
		float								GetLoopingLenghtPercent() const;

		virtual void						SetMinDistance(float Distance);
		float								GetMinDistance() const;

		virtual void						SetMaxDistance(float Distance);
		float								GetMaxDistance() const;

		virtual void						SetConeInsideAngle(float Angle);
		float								GetConeInsideAngle() const;

		virtual void						SetConeOutsideAngle(float Angle);
		float								GetConeOutsideAngle() const;

		virtual void						SetConeOutsideVolume(float Volume);
		float								GetConeOutsideVolume() const;

		virtual void						Play() = 0;
		void								Replay();
		virtual void						Pause() = 0;
		virtual void						Stop() = 0;

		void								SetSoundFileName(const ZEString& FileName);
		const ZEString&						GetSoundFileName() const;

		void								SetSoundResource(ZEHolder<const ZESoundResource> Resource);
		ZEHolder<const ZESoundResource>		GetSoundResource() const;

		static ZESoundSource*				CreateInstance();
}
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZESound.png")
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Pickable, false)
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper.NameplateVisible, false)
ZE_META_ATTRIBUTE(ZEDEditor.EntityWrapper3D.BoundingBoxVisible, false);
