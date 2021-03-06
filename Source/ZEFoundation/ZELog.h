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

#define ZE_LOG_MAX_SESSION_COUNT 16

#ifdef ZE_PLATFORM_COMPILER_MSVC
    #define __ZINEK_FUNCTION__ __FUNCTION__
#else
    #define __ZINEK_FUNCTION__ __PRETTY_FUNCTION__
#endif

#define zeLog(...) do {char ModuleName_Buffer_F23A328C[256]; ZELog::GetModuleName(ModuleName_Buffer_F23A328C, __FILE__, __ZINEK_FUNCTION__); ZELog::GetInstance()->Log(ModuleName_Buffer_F23A328C, __VA_ARGS__);} while(false)
#ifdef ZE_DEBUG_ENABLE
	#define zeDebugLog(...) do {char ModuleName_Buffer_F23A328C[256]; ZELog::GetModuleName(ModuleName_Buffer_F23A328C, __FILE__, __ZINEK_FUNCTION__); ZELog::GetInstance()->Log(ModuleName_Buffer_F23A328C, ZE_LOG_DEBUG, __VA_ARGS__);} while(false)
	#define zeDebugLogOnce(...) do {static bool Logged = false; if (Logged) break; Logged = true; zeDebugLog(__VA_ARGS__);} while(false)
	#define zeDebugLogOccured(Times, ...) do {static int Count = 0; if (Count == 0) {zeDebugLog(__VA_ARGS__); } Count++; if (Count >= (Times)) Count = 0;} while(false)
	#define zeDebugLogPerFrame(Times, ...) do {static int LastFrame = -1; if (LastFrame == ZECore::GetInstance()->GetFrameId()) break; zeDebugLog(__VA_ARGS__);	LastFrame = ZECore::GetInstance()->GetFrameId();} while(false)
#else
	#define zeDebugLog(...) do { } while(false)
	#define zeDebugLogOnce(...) do { } while(false)
	#define zeDebugLogOccured(Times, ...) do { } while(false)
	#define zeDebugLogPerFrame(Times, ...) do { } while(false)
#endif

class ZELogSession;
template <typename ZEItemType, typename ZELockType> class ZEList2;

class ZELog
{
	friend ZELogSession;
	private:
		ZELock								Lock;
		ZELogSession*						RootSession;
		ZELogSession*						SessionCount;	
		ZESize								LastSessionId;
		ZEList2<ZELogSession, ZELockRW>*	Sessions;

		void								LogInternal(const char* Module, ZELogType Type, const char* Format, va_list args);

											ZELog();
											~ZELog();

	public:
		ZELogSession*						GetRootSession();
		ZELogSession*						GetCurrentSession();
		ZESize								GetSessionCount();

		void								BeginSession(ZELogSession* Session);
		void								EndSession(ZELogSession* Session);

		void								Log(const char* Module, const char* Format, ...);
		void								Log(const char* Module, ZELogType Type, const char* Format, ...);

		ZE_ENGINE_EXPORT 
		static ZELog*						GetInstance();

		static void							GetModuleName(char* Output, const char* FileName, const char* Function);
		static const char*					GetLogTypeString(ZELogType Type);
};
