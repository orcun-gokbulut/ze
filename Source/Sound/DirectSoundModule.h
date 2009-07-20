//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DirectSoundModule.h
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
#ifndef	__ZE_DIRECT_SOUND_H__
#define __ZE_DIRECT_SOUND_H__

#include "Sound/SoundModule.h"

class ZEDirectSoundModuleDescription : public ZEModuleDescription
{
	public:
		virtual ZEModuleType			GetType();
		virtual ZEModuleAttribute		GetAttributes();
		virtual int						GetRequiredZinekEngineVersion();
		virtual int						GetMajorVersion();
		virtual int						GetMinorVersion();
		virtual const char*				GetCopyright();
		virtual const char*				GetName();
		virtual ZEOptionSection*		GetOptions();

		virtual ZEModule*				CreateModuleInstance();
		virtual bool					CheckCompatible();
};

class ZEDirectSoundModule : public ZESoundModule
{
	private:
		bool							Enabled;

	public:
		ZEModuleDescription*			GetModuleDescription();		

		virtual bool					IsEnabled();
		virtual void					SetEnabled(bool Enabled);

		virtual bool					Initialize();
		virtual void					Deinitialize();

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
								
										~ZEDirectSoundModule();
};		
#endif
