//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Animation.h
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
#ifndef __ZE_ANIMATION_H__
#define __ZE_ANIMATION_H__

#include "Definitions.h"
#include "ZEDS/Array.h"
#include "ZEDS/Variant.h"

enum ZEAnimationState
{
	ZE_AS_NONE			= 0,
	ZE_AS_PLAYING		= 1,
	ZE_AS_STOPPED		= 2,
	ZE_AS_PAUSED		= 3
};

struct ZEPropertyAnimationKey
{
	float								Time;
	ZEVariant							Value;
};

struct ZEPropertyAnimation
{
	int									PropertyId;
	ZEVariantType						ValueType;
	bool								Interpolate;
	ZEArray<ZEPropertyAnimationKey>		Keys;
};

class ZEResourceFile;
class ZEAnimation
{
	public:
		char							Name[ZE_MAX_NAME_SIZE];
		int								FrameCount;
		float							FramePerSecond;
		ZEArray<ZEPropertyAnimation>	PropertyAnimations;

		static bool						ReadFromFile(ZEUnserializer* Unserializer, ZEAnimation* Animation);
		static bool						WriteToFile(ZESerializer* Serializer, ZEAnimation* Animation);
};



class ZEClass;
class ZEAnimationController
{
	private:
		ZEClass*						Owner;
		ZEAnimation*					Animation;
		float							StartFrame;
		float							EndFrame;
		float							CurrentFrame;
		bool							Looping;
		ZEAnimationState				State;
		float							Speed;

	public:
		void							SetOwner(ZEClass* Owner);
		ZEClass*						GetOwner();

		void							SetLooping(bool Loop);
		bool							GetLooping();
		
		void							SetCurrentFrame(float Frame);
		float							GetCurrentFrame();
		
		void							SetStartFrame(float Start);
		float							GetStartFrame();

		void							SetEndFrame(float End);
		float							GetEndFrame();

		void							SetAnimationSpeed(float Speed);
		float							GetAnimationSpeed();

		ZEAnimationState				GetAnimationState();

		void							SetAnimation(ZEAnimation* Animation);
		ZEAnimation*					GetAnimation();

		void							PlayAnimation();
		void							PauseAnimation();
		void							StopAnimation();
		void							ResumeAnimation();

		void							AdvanceAnimation(float TimeElapsed);

										ZEAnimationController();
										~ZEAnimationController();
};
#endif
