//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeClip.h
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

#ifndef __ZE_MODEL_ANIMATION_NODE_CLIP_H__
#define __ZE_MODEL_ANIMATION_NODE_CLIP_H__

#include "ZEModelAnimationNode.h"
#include "ZEModelAnimationClip.h"


class ZEModelAnimationNodeClip : public ZEModelAnimationNode
{
	protected:
		ZEModelAnimationClip*					AnimationClip;

		ZEUInt									StartFrame;
		ZEUInt									EndFrame;

		ZEUInt									EffectiveStartFrame;
		ZEUInt									EffectiveEndFrame;
		bool									LimitsEnabled;

		float									Speed;
		float									CurrentFrame;
		bool									Looping;

		void									ApplyLimits();

		virtual void							ProcessSelf(float ElapsedTime);
		virtual bool							GenerateOutput(ZEModelAnimationFrame& output);

												ZEModelAnimationNodeClip();
		virtual									~ZEModelAnimationNodeClip();												

	public:
		void									SetAnimationClip(ZEModelAnimationClip* animationClip);
		const ZEModelAnimationClip*				GetAnimationClip();
		
		void									SetSequence(ZESize index);
		void									SetSequence(const char* name);

		void									SetSpeed(float fPS);
		float									GetSpeed();

		void									SetLooping(bool looping);
		bool									GetLooping();

		void									SetLimitsEnabled(bool enabled);
		bool									GetLimitsEnabled();

		void									SetCurrentFrame(float frame);
		void									SetCurrentFrameByTime(float seconds);
		void									SetCurrentFrameByPercentage(float percentage);

		float 									GetCurrentFrame();
		float									GetCurrentFrameByTime();
		float									GetCurrentFrameByPercentage();

		void									SetStartFrame(ZEUInt frame);
		void									SetStartFrameByTime(float seconds);
		void									SetStartFrameByPercentage(float percentage);

		ZEUInt									GetStartFrame();
		float									GetStartFrameByTime();
		float									GetStartFrameByPercentage();

		void									SetEndFrame(ZEUInt frame);
		void									SetEndFrameByTime(float seconds);
		void									SetEndFrameByPercentage(float percentage);

		ZEUInt									GetEndFrame();
		float									GetEndFrameByTime();
		float									GetEndFrameByPercentage();

		static ZEModelAnimationNodeClip*		CreateInstance();
};

#endif
