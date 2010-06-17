//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALSoundSource.h
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
#ifndef	__ZE_AL_SOUND_SOURCE_H__
#define __ZE_AL_SOUND_SOURCE_H__

#include "ZEALComponentBase.h"
#include "..\ZESoundSource.h"
#include <al.h>

class ZEALSoundSource : public ZESoundSource, public ZEALComponentBase
{
	friend class ZEALModule;
	private:
		bool						Allocated;
		ALuint						ALSource;
		ALuint						ALBuffer1;
		ALuint						ALBuffer2;
		char*						InnerStreamBuffer;

		size_t						BufferPosition;
		size_t						BufferSampleCount;
		size_t						OldBufferPosition;
		size_t						StreamPosition;

		bool						CreateBuffer();
		void						ResetParameters();
		void						DestroyBufferSource();

		void						Stream();
		void						ResetStream();
		void						StreamDecodeAndFill(size_t BufferPosition, size_t Position, size_t SampleCount);

									ZEALSoundSource();
		virtual						~ZEALSoundSource();

	public:
		virtual void				SetSoundSourceState(ZESoundSourceState State);
		virtual void				SetCurrentPosition(unsigned int SampleIndex);
		virtual unsigned int		GetCurrentPosition();

		virtual void				SetPan(int NewPan);
		virtual void				SetPlaybackSpeed(float Speed);
		virtual void				SetVolume(unsigned int NewVolume);
		virtual void				SetLooping(bool Enabled);				
							
		virtual void				Play();
		virtual void				Resume();
		virtual void				Pause();
		virtual void				Stop();

		void						Update(float ElapsedTime);

		virtual void				SetSoundResource(ZESoundResource* Resource);
};

#endif




