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
	ZE_LOG_CRITICAL_ERROR	= 6,
    ZE_LOG_ERROR			= 5,
    ZE_LOG_WARNING			= 4,
	ZE_LOG_SUCCESS			= 3,
	ZE_LOG_INFO				= 2,
	ZE_LOG_DEBUG			= 1,
	ZE_LOG_NONE				= 0
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
        ZELog::UtilityGetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
		ZELog::GetInstance()->Log(__MODULE__, __VA_ARGS__);\
	}\
	while(false)

#define zeDebugLog(...)\
	do\
	{\
		if (ZELog::GetInstance()->GetMinimumLevel() > ZE_LOG_DEBUG)\
			break;\
		char __MODULE__[256];\
		ZELog::UtilityGetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
		ZELog::GetInstance()->Log(__MODULE__, ZE_LOG_DEBUG, __VA_ARGS__);\
	}\
	while(false)

#define zeDebugLogOnce(...)\
	do\
	{\
		if (ZELog::GetInstance()->GetMinimumLevel() > ZE_LOG_DEBUG)\
			break;\
		static bool Logged = false; \
		if (Logged)\
			break;\
		Logged = true;\
		char __MODULE__[256];\
		ZELog::UtilityGetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
		ZELog::GetInstance()->Log(__MODULE__, ZE_LOG_DEBUG, __VA_ARGS__);\
	}\
	while(false)

#define zeDebugLogOccured(Times, ...)\
	do\
	{\
		if (ZELog::GetInstance()->GetMinimumLevel() > ZE_LOG_DEBUG) \
			break;\
		static int Count = 0;\
		if (Count == 0)\
		{\
			char __MODULE__[256];\
			ZELog::UtilityGetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
			ZELog::GetInstance()->Log(__MODULE__, ZE_LOG_DEBUG, __VA_ARGS__);\
		}\
		Count++;\
		if (Count >= Times)\
			Count = 0;\
	}\
	while(false)

#define zeDebugLogPerFrame(Times, ...)\
	do\
	{\
		if (ZELog::GetInstance()->GetMinimumLevel() > ZE_LOG_DEBUG) \
			break;\
		static int LastFrame = -1;\
		if (LastFrame == ZECore::GetInstance()->GetFrameId())\
			break;\
		char __MODULE__[256];\
		ZELog::UtilityGetModuleName(__MODULE__, __FILE__, __ZINEK_FUNCTION__);\
		ZELog::GetInstance()->Log(__MODULE__, ZE_LOG_DEBUG, __VA_ARGS__);\
		LastFrame = ZECore::GetInstance()->GetFrameId(); \
	}\
	while(false)

class ZELog
{
	private:
		ZELock				Lock;
		ZELogType			MinimumLogLevel;

		ZELogCallback		Callback;
		void*				CallbackParameter;

		void*				LogFile;
		bool				LogFileEnabled;
		ZEString			LogFilePath;
		ZELogType			LogFileMinimumLevel;

		void				OpenLogFile();
		void				LogInternal(const char* Module, ZELogType Type, const char* Format, va_list args);

							ZELog();
							~ZELog();

	public:
		void				SetMinimumLevel(ZELogType Level);
		ZELogType			GetMinimumLevel();

		void				SetCallback(ZELogCallback Callback);
		ZELogCallback		GetCallback();

		void				SetCallbackParameter(void* ExtraParameters);
		void*				GetCallbackParameter();

		void				SetLogFileEnabled(bool Enabled);
		bool				GetLogFileEnabled();

		void				SetLogFilePath(const ZEString& FileName);
		const char*			GetLogFilePath();

		void				SetLogFileMinimumLevel(ZELogType Level);
		ZELogType			GetLogFileMinimumLevel();

		void				Log(const char* Module, ZELogType Type, const char* Format, ...);
        void				Log(const char* Module, const char* Format, ...);

		ZE_ENGINE_EXPORT 
		static ZELog*		GetInstance();
		
		static void			UtilityGetModuleName(char* Output, const char* FileName, const char* Function);
		static const char*	UtilityGetLogTypeString(ZELogType Type);

};

#endif
