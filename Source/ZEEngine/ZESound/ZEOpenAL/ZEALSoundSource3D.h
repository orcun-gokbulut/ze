//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALSoundSource3D.h
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
#ifndef	__ZE_AL_SOUND_SOURCE_3D_H__
#define __ZE_AL_SOUND_SOURCE_3D_H__

#include "ZETypes.h"
#include "ZEALComponentBase.h"
#include "ZESound/ZESoundSource3D.h"

#include <AL/al.h>


class ZEALSoundSource3D : public ZESoundSource3D, public ZEALComponentBase
{
	friend class ZEALModule;
	private:
		bool					Allocated;
		ALuint					ALSource;
		ALuint					ALBuffer1;
		ALuint					ALBuffer2;
		char*					InnerStreamBuffer;

		ZESize					BufferPosition;
		ZESize					BufferSampleCount;
		ZESize					OldBufferPosition;
		ZESize					StreamPosition;

		bool					CreateBuffer();
		void					ResetParameters();
		void					DestroyBufferSource();

		void					Stream();
		void					ResetStream();
		void					StreamDecodeAndFill(ZESize BufferPosition, ZESize Position, ZESize SampleCount);

								ZEALSoundSource3D();
		virtual					~ZEALSoundSource3D();

	public:
		virtual void			SetSoundSourceState(ZESoundSourceState State);
		virtual void			SetCurrentPosition(ZEUInt SampleIndex);
		virtual ZEUInt			GetCurrentPosition();

		virtual void			SetPan(ZEInt NewPan);
		virtual void			SetPlaybackSpeed(float Speed);
		virtual void			SetVolume(ZEUInt NewVolume);
		virtual void			SetLooping(bool Enabled);				
							
		virtual void			Play();
		virtual void			Resume();
		virtual void			Pause();
		virtual void			Stop();

		void					Update(float ElapsedTime);

		virtual void			SetSoundResource(ZESoundResource* Resource);

		virtual void			SetPosition(const ZEVector3& NewPosition);
		virtual void			SetRotation(const ZEQuaternion& NewRotation);

		virtual void			SetMinDistance(float  NewMinDistance);
		virtual void			SetMaxDistance(float  NewMaxDistance);
		virtual void			SetConeInsideAngle(ZEUInt NewInsideAngle);
		virtual void			SetConeOutsideAngle(ZEUInt NewOutsideAngle);			
		virtual void			SetConeOutsideVolume(ZEUInt NewOutsideVolume);
};
#endif




