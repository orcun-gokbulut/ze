//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationClip.h
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

#ifndef __ZE_MODEL_ANIMATION_CLIP_H__
#define __ZE_MODEL_ANIMATION_CLIP_H__

#include "ZEModelResource.h"
#include "ZEModelAnimation.h"

//typedef ZEModelResourceAnimationFrame ZEModelAnimationFrame;

class ZEModel;

struct AnimationSequence
{
	char							Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEArray<ZEModelAnimationFrame>	Frames;

	ZESize							StartFrame;
	ZESize							EndFrame;
	ZESize							FrameCount;
};

class ZEModelAnimationClip
{
	friend class ZEModelAnimationNodeClip;

	protected:

		ZEString							Name;

		ZEArray<AnimationSequence>			Sequences;
		ZEArray<ZEModelAnimationFrame*>		Frames;
		ZESize								FrameCount;

		bool								IsInitialized;
		ZESize								ReferenceCount;

		void								AddReference();
		void								RemoveReference();

											ZEModelAnimationClip();
		virtual								~ZEModelAnimationClip();												

	public:

		bool								AddAnimationSequence(const ZEModelResourceAnimation& animation);
		bool								AddAnimationSequence(const char* name, const ZEModel& model);

		bool								InsertAnimationSequence(ZESize index, const ZEModelResourceAnimation& animation);
		bool								InsertAnimationSequence(ZESize index, const char* name, const ZEModel& model);

		AnimationSequence*					GetAnimationSequence(const char* name);
		AnimationSequence*					GetAnimationSequence(ZESize index);
		ZESize								GetSequenceCount() const;

		bool								RemoveAnimationSequence(const char* name);
		bool								RemoveAnimationSequence(ZESize index);
		bool								RemoveAnimationSequence(ZESize startIndex, ZESize endIndex);

		ZEModelAnimationFrame*				GetFrame(ZESize index);
		ZESize								GetFrameCount() const;

		virtual void						Initialize();
		virtual void						Deinitialize();

		static ZEModelAnimationClip*		CreateInstance();
};

#endif
