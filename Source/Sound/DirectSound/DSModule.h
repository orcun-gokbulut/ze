//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DSModule.h
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
#ifndef	__ZE_DS_MODULE_H__
#define __ZE_DS_MODULE_H__

#include "ZEDS/Array.h"
#include "Sound/SoundModule.h"
#include <dsound.h>

class ZEDSSoundSource;
class ZEDSSoundSource3D;
class ZEDSListener;

#undef PlaySound
class ZEDSModule : public ZESoundModule
{
	friend class ZEDSModuleDescription;
	friend class ZEDSSoundSource;
	friend class ZEDSSoundSource;
	friend class ZEDSSoundSource3D;
	private:
		unsigned int						MasterVolume;
		bool								Enabled;
		ZEDSListener*						ActiveListener;
		ZESmartArray<ZEDSListener*>			Listeners;
		ZESmartArray<ZEDSSoundSource*>		SoundSources;
		ZESmartArray<ZEDSSoundSource3D*>	SoundSources3D;
		unsigned int						BufferSize;

	public:
		ZEModuleDescription*				GetModuleDescription();		

		LPDIRECTSOUND8						GetDevice();

		virtual bool						IsEnabled();
		virtual void						SetEnabled(bool Enabled);

		virtual bool						Initialize();
		virtual void						Deinitialize();

		virtual void						SetMasterVolume(unsigned int Volume);
		virtual unsigned int				GetMasterVolume();

		virtual void						SetMaxBufferSize(unsigned int BufferSize); 
		virtual unsigned int				GetMaxBufferSize();

		virtual void						ProcessSound(float ElapsedTime);

		virtual void						PlaySound(ZESoundResource* SoundResource);

		virtual	void						SetActiveListener(ZEListener* NewListener);
		virtual ZEListener*					GetActiveListener();

		virtual ZESoundSource*				CreateSoundSource();
		virtual ZESoundSource3D*			CreateSoundSource3D();
		virtual ZEListener*					CreateListener();
/*
		virtual int						CreateBuffer(ZESoundSource* Source, ZESoundResource* Resource);
		virtual void					ReleaseBuffer(ZESoundSource* Source);

		virtual void					Play(ZESoundSource* Source);
		virtual void					Stop(ZESoundSource* Source);
		virtual void					Pause(ZESoundSource* Source);
		virtual void					PlayLoop(ZESoundSource* Source);

		virtual void					UpdateSource(ZESoundSource* Source);
		virtual void				    UpdateSource3D(ZESoundSource3D* Source3D);

		virtual void					ProcessSound();

		virtual void					PauseAllSounds();
		virtual void					StopAllSounds();

		virtual void					ReloadBuffer(ZESoundSource* Source);
		virtual void					ReloadAllBuffers();
		
		virtual void					SetListener(ZEListener* NewListener);
		virtual void				    UpdateListener(ZEListener* NewListener);
								
										~ZEDirectSoundModule();*/
};		
#endif
