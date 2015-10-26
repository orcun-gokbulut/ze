//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELog.h
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
#ifndef __ZE_LOG_H__
#define __ZE_LOG_H__

#include "ZEDLL.h"
#include "ZEPlatform.h"
#include "ZEDS/ZEString.h"
#include "ZEThread/ZELock.h"

enum ZELogType
{
	ZE_LOG_CRITICAL_ERROR	= 5,
    ZE_LOG_ERROR			= 4,
	ZE_LOG_SUCCESS			= 3,
    ZE_LOG_WARNING			= 2,
    ZE_LOG_INFO				= 1,
	ZE_LOG_DEBUG			= 0
};

#ifdef ZE_PLATFORM_COMPILER_MSVC
    #define __ZINEK_FUNCTION__ __FUNCTION__
#else
    #define __ZINEK_FUNCTION__ __PRETTY_FUNCTION__
#endif

typedef void (*ZELogCallback)(const char* Module, ZELogType Type, const char* LogText, void* ExtraParameters);
#define zeLog(...)\
	do\
	{\
		char __MODULE__[256];\
        ZELog::GetInstance()->GetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
		ZELog::GetInstance()->Log(__MODULE__, __VA_ARGS__);\
	}\
	while(false)

class ZELog
{
	private:
		void*				LogFile;
		bool				LogFileEnabled;
		ZEString			LogFileName;
		ZELogCallback		LogCallback;
		void*				LogCallbackExtraParameters;
		ZELogType			MinimumLogLevel;
		ZELock				Lock;

							ZELog();

	public:
		static void			GetModuleName(char* Output, const char* FileName, const char* Function);
		static const char*	GetLogTypeString(ZELogType Type);

        void				SetLogFileEnabled(bool Enabled);
        bool				GetLogFileEnabled();

		void				SetMinimumLogLevel(ZELogType Level);
		ZELogType			GetMinimumLogLevel();

        void				SetLogFileName(const ZEString& FileName);
        const char*			GetLogFileName();

		void				SetCallback(ZELogCallback Callback, void* ExtraParameters);
		ZELogCallback		GetCallback();

		void				Log(const char* Module, ZELogType Type, const char* Format, ...);
        void				Log(const char* Module, const char* Format, ...);

		ZE_ENGINE_EXPORT 
		static ZELog*		GetInstance();
};

#endif
