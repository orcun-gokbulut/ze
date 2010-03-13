//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Module.h
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
#ifndef	__ZE_MODULE_H__
#define __ZE_MODULE_H__

enum ZEModuleAttribute
{
	ZE_MA_NONE				= 0,
	ZE_MA_INTERNAL			= 1,
	ZE_MA_EXTERNAL			= 2,
	ZE_MA_SAFE				= 4,
	ZE_MA_DEBUG				= 8,
	ZE_MA_COMPATIBLE		= 16,
	ZE_MA_VALIDATED			= 32
};

enum ZEModuleType
{
	ZE_MT_UNKNOWN				= 0,
	ZE_MT_GRAPHICS				= 1,
	ZE_MT_SOUND					= 2,
	ZE_MT_INPUT					= 3,
	ZE_MT_PHYSICS				= 4,	
	ZE_MT_NETWORK				= 5,
	ZE_MT_GAME					= 6,
	ZE_MT_CONSOLEINTERFACE		= 7,
	ZE_MT_COMPONENT				= 8,
	ZE_MT_OTHER
};

class ZEModule;
class ZEModuleManager;
class ZEOptionSection;
class ZEModuleDescription
{
	public:
		virtual ZEModuleAttribute		GetAttributes() = 0;
		virtual ZEModuleType			GetType() = 0;
		virtual int						GetRequiredZinekEngineVersion() = 0;
		virtual int						GetMajorVersion() = 0;
		virtual int						GetMinorVersion() = 0;
		virtual const char*				GetCopyright() = 0;
		virtual const char*				GetName() = 0;

		virtual ZEOptionSection*		GetOptions() = 0;
		virtual	ZEModule*				CreateModuleInstance() = 0;
		virtual	bool					CheckCompatible() = 0;
};

class ZEModule
{
	protected:
										ZEModule();
		virtual							~ZEModule();

	public:
		virtual ZEModuleDescription*	GetModuleDescription() = 0;

		virtual bool					IsEnabled() = 0;
		virtual void					SetEnabled(bool Enabled) = 0;

		virtual bool					Initialize() = 0;
		virtual void					Deinitialize() = 0;

		virtual void					Destroy();
};
#endif

// Initialize -- Initializes component
// Deinitialize -- Deinitializes component
// Release -- Releases internal resources
// Destroy -- Delete component
