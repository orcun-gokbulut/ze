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

#include "ZESound/ZESoundSource.h"
#include "ZEALComponentBase.h"

#include <AL/al.h>

class ZEALSoundSource : public ZESoundSource, public ZEALComponentBase
{
	friend class ZEALModule;
	private:
		bool								Allocated;
		ALuint								ALSource;
		ALuint								ALBuffer1;
		ALuint								ALBuffer2;
		char*								InnerStreamBuffer;

		ZESize								BufferPosition;
		ZESize								BufferSampleCount;
		ZESize								OldBufferPosition;
		ZESize								StreamPosition;

		bool								CreateBuffer();
		void								DestroyBufferSource();

		void								Stream();
		void								ResetStream();
		void								StreamDecodeAndFill(ZESize BufferPosition, ZESize Position, ZESize SampleCount);

		virtual void						UpdateResource() override;
		void								UpdateParameters();
		void								UpdateOrientation();

		virtual void						LocalTransformChanged() override;
		virtual void						ParentTransformChanged() override;

											ZEALSoundSource();
		virtual								~ZEALSoundSource() override;

	public:
		virtual void						SetCurrentPosition(ZESize SampleIndex) override;

		virtual void						SetSpeed(float Speed) override;
		virtual void						SetVolume(float Volume) override;
		virtual void						SetPan(float Pan) override;
		virtual void						SetLooping(bool Enabled) override;				

		virtual void						SetMinDistance(float  NewMinDistance);
		virtual void						SetMaxDistance(float  NewMaxDistance);
		virtual void						SetConeInsideAngle(float NewInsideAngle);
		virtual void						SetConeOutsideAngle(float NewOutsideAngle);
		virtual void						SetConeOutsideVolume(float NewOutsideVolume);

		virtual void						Play() override;
		virtual void						Pause() override;
		virtual void						Stop() override;

		void								Update(float ElapsedTime);
};
