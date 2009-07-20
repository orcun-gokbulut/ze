//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundModule.h
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
#ifndef	__ZE_SOUND_MODULE_H__
#define __ZE_SOUND_MODULE_H__

#include "Core/Module.h"
#include "SoundResource.h"
#include "SoundSource.h"
#include "SoundSource3D.h"
#include "Listener.h"

#define zeSound ZESoundModule::GetInstance()
class ZESoundModule : public ZEModule
{
	public:
		static void					BaseInitialize();
		static void					BaseDeinitialize();

		virtual void				Play(ZESoundSource* Source) = 0;
		virtual void				Stop(ZESoundSource* Source) = 0;
		virtual void				Pause(ZESoundSource* Source) = 0;
		virtual void				PlayLoop(ZESoundSource* Source) = 0;
		
		virtual void				UpdateSource(ZESoundSource* Source) = 0 ;
		virtual void                UpdateSource3D(ZESoundSource3D* Source3D)=0;

		virtual void				ProcessSound() = 0;
		virtual void				PauseAllSounds() = 0;
		virtual void				StopAllSounds() = 0;

		virtual void				ReloadBuffer(ZESoundSource* Source) = 0;
		virtual void				ReloadAllBuffers() = 0;

		virtual	void				SetListener(ZEListener* NewListener) = 0;
		virtual void                UpdateListener(ZEListener* NewListener) = 0;

		virtual int					CreateBuffer(ZESoundSource* Source, ZESoundResource* Resource) = 0;
		virtual void				ReleaseBuffer(ZESoundSource* Source) = 0;

		static ZESoundModule*		GetInstance();
};

#endif


