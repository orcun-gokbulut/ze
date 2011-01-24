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

#include "ZEModelAnimation.h"

class ZEModel;

enum ZEModelAnimationBlendMode
{
	ZE_MABM_NONE		= 0,
	ZE_MABM_INTERPOLATE = 1,
	ZE_MABM_ADDAPTIVE	= 2,
	ZE_MABM_OVERWRITE	= 3,
};

class ZEModelAnimationTrack
{
	private:
		ZEModel*						Owner;
		const ZEModelAnimation*			Animation;
		ZEModelAnimationState			State;
		float							BlendFactor;
		ZEModelAnimationBlendMode		BlendMode;
		float							Speed;
		float							CurrentFrame;
		ZEDWORD							StartFrame;
		ZEDWORD							EndFrame;
		bool							Looping;
		int								LOD;

	public:
		void							SetOwner(ZEModel* Model);
		ZEModel*						GetOwner();

		void							SetAnimation(const ZEModelAnimation* Animation);
		const ZEModelAnimation*			GetAnimation();

		void							SetAnimationByName(const char* Name);
		const char*						GetAnimationName();

		void							SetState(ZEModelAnimationState State);
		ZEModelAnimationState			GetState();

		void							SetLOD(int LOD);
		int								GetLOD();

		void							SetSpeed(float Factor);
		float							GetSpeed();

		void							SetBlendMode(ZEModelAnimationBlendMode Mode);
		ZEModelAnimationBlendMode		GetBlendMode();

		void							SetBlendFactor(float Factor);
		float							GetBlendFactor();

		void							SetLooping(bool Looping);
		bool							GetLooping();

		void							SetCurrentFrame(unsigned int Frame);	
		unsigned int 					GetCurrentFrame();

		void							SetStartFrame(unsigned int Frame);
		unsigned int					GetStartFrame();

		void							SetEndFrame(unsigned int Frame);
		unsigned int					GetEndFrame();

		void							Play(unsigned int StartFrame, unsigned int EndFrame);
		void							Play();
		void							Resume();
		void							Pause();
		void							Stop();

		void							AdvanceAnimation(float ElapsedTime);

										ZEModelAnimationTrack();
};

#endif
