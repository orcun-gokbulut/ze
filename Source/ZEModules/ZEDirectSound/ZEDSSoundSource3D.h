//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSSoundSource3D.h
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
#ifndef	__ZE_DS_SOUND_SOURCE_3D_H__
#define __ZE_DS_SOUND_SOURCE_3D_H__

#include "ZETypes.h"
#include "ZEDSSoundSource.h"
#include "ZESound/ZESoundSource3D.h"

#include <dsound.h>


class ZEDSSoundSource3D : public ZESoundSource3D, public ZEDSComponentBase
{
	friend class ZEDSModule;
	private:
		LPDIRECTSOUNDBUFFER			DSBuffer;
		LPDIRECTSOUND3DBUFFER		DS3DBuffer;

		ZESize						BufferSampleCount;
		ZESize						OldBufferPosition;
		ZESize						StreamPosition;
		ZEInt						LastUpdatedBufferChunk;

		bool						CreateBuffer();
		void						Stream();
		void						ResetStream();
		void						ResetParameters();
		void						StreamDecodeAndFill(ZESize BufferPosition, ZESize Position, ZESize SampleCount);

									ZEDSSoundSource3D();
		virtual						~ZEDSSoundSource3D();

	public:
		virtual void				SetSoundSourceState(ZESoundSourceState State);
		virtual void				SetCurrentPosition(ZESize SampleIndex);
		virtual ZESize				GetCurrentPosition();
		virtual void				SetStartPosition(ZESize SampleIndex);
		virtual void				SetEndPosition(ZESize SampleIndex);

		virtual void				SetPan(ZEInt NewPan);
		virtual void				SetPlaybackSpeed(float Speed);
		virtual void				SetVolume(ZEUInt NewVolume);
		virtual void				SetLooping(bool Enabled);				
							
		virtual void				Play();
		virtual void				Resume();
		virtual void				Pause();
		virtual void				Stop();

		void						Update(float ElapsedTime);

		virtual void				SetSoundResource(ZESoundResource* Resource);
		virtual void				SetPosition(const ZEVector3& NewPosition);
		virtual void				SetRotation(const ZEQuaternion& NewRotation);

		virtual void				SetMinDistance(float  NewMinDistance);
		virtual void				SetMaxDistance(float  NewMaxDistance);
		virtual void				SetConeInsideAngle(ZEUInt NewInsideAngle);
		virtual void				SetConeOutsideAngle(ZEUInt NewOutsideAngle);			
		virtual void				SetConeOutsideVolume(ZEUInt NewOutsideVolume);
};

#endif




