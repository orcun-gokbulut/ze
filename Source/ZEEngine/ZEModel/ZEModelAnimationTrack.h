//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationTrack.h
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

#ifndef __ZE_MODEL_ANIMATION_TRACK_H__
#define __ZE_MODEL_ANIMATION_TRACK_H__

#include "ZETypes.h"
#include "ZEModelAnimation.h"
#include "ZEMeta/ZEObject.h"

ZE_META_FORWARD_DECLARE(ZEModel, "ZEModel.h")

enum ZEModelAnimationBlendMode
{
	ZE_MABM_NONE		= 0,
	ZE_MABM_INTERPOLATE = 1,
	ZE_MABM_ADDAPTIVE	= 2,
	ZE_MABM_OVERWRITE	= 3,
};

ZE_CLASS(ZEModelAnimationTrack)

class ZEModelAnimationTrack : public ZEObject
{
	friend class ZEModel;
	ZE_OBJECT

	private:
		ZEModel*						Owner;
		const ZEModelAnimation*			Animation;

		ZEModelAnimationState			State;

		float							BlendFactor;
		ZEModelAnimationBlendMode		BlendMode;

		ZEUInt							StartFrame;
		ZEUInt							EndFrame;

		ZEUInt							EffectiveStartFrame;
		ZEUInt							EffectiveEndFrame;
		bool							LimitsEnabled;

		float							Speed;
		float							CurrentFrame;
		ZEUInt							LOD;
		bool							Looping;

		
		void							UpdateAnimation();
		void							UpdateMeshesAndBones();
		void							ApplyLimits();

	public:
		void							SetOwner(ZEModel* Model);
		ZEModel*						GetOwner();

		void							SetAnimation(const ZEModelAnimation* Animation);
		const ZEModelAnimation*			GetAnimation();

		void							SetAnimationByName(const char* Name);
		const char*						GetAnimationName();

		void							SetState(ZEModelAnimationState State);
		ZEModelAnimationState			GetState();

		void							SetLOD(ZEUInt LOD);
		ZEUInt							GetLOD();

		void							SetSpeed(float FPS);
		float							GetSpeed();

		void							SetBlendMode(ZEModelAnimationBlendMode Mode);
		ZEModelAnimationBlendMode		GetBlendMode();

		void							SetBlendFactor(float Factor);
		float							GetBlendFactor();

		void							SetLooping(bool Looping);
		bool							GetLooping();

		void							SetLimitsEnabled(bool Enabled);
		bool							GetLimitsEnabled();

		void							SetCurrentFrame(float Frame);
		void							SetCurrentFrameByTime(float Seconds);
		void							SetCurrentFrameByPercentage(float Percentage);

		float 							GetCurrentFrame();
		float							GetCurrentFrameByTime();
		float							GetCurrentFrameByPercentage();

		void							SetStartFrame(ZEUInt Frame);
		void							SetStartFrameByTime(float Seconds);
		void							SetStartFrameByPercentage(float Percentage);

		ZEUInt							GetStartFrame();
		float							GetStartFrameByTime();
		float							GetStartFrameByPercentage();

		void							SetEndFrame(ZEUInt Frame);
		void							SetEndFrameByTime(float Seconds);
		void							SetEndFrameByPercentage(float Percentage);

		ZEUInt							GetEndFrame();
		float							GetEndFrameByTime();
		float							GetEndFrameByPercentage();

		void							Play(ZEUInt StartFrame, ZEUInt EndFrame);
		void							Play();
		void							Resume();
		void							Pause();
		void							Stop();

		void							Tick(float ElapsedTime);

										ZEModelAnimationTrack();
};

#endif
