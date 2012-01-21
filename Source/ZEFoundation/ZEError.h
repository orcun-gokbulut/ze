//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEError.h
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
#ifndef	__ZE_ERROR_H__
#define __ZE_ERROR_H__

#include "ZETypes.h"

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_ZINEK_VERSION_PLATFORM_WINDOWS)
	#include <crtdbg.h>
#else
	#include <stdlib.h> 
#endif

enum ZEErrorType
{
	ZE_ET_CRITICAL_ERROR,
	ZE_ET_ERROR,
	ZE_ET_WARNING,
	ZE_ET_NOTICE,
	ZE_ET_LOG,
};

enum ZEAssertType
{
	ZE_AT_ASSERT,
	ZE_AT_WARNING_ASSERT
};

#define zeBreak(Condition) if (Condition) { __asm{ ZEInt 3 }}

#include "ZEAPI.h"

#define ZE_MAX_MODULE_NAME_LENGTH 1024

#ifdef ZE_DEBUG_MEMORY_CHECKS_ENABLE
	#include <crtdbg.h>
	#define ZEDebugCheckMemory() zeAssert(!_CrtCheckMemory(), "Heap problem")
#else
	#define ZEDebugCheckMemory()
#endif


#ifdef ZE_DEBUG_ENABLE
	#ifdef ZE_ZINEK_VERSION_PLATFORM_WINDOWS
		#define zeAssert(Condition, ...) do {if (Condition) {ZEError::RaiseAssert(ZE_AT_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); if (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak(true);}} while(false)
		#define zeWarningAssert(Condition, ...) do {if (Condition) {ZEError::RaiseAssert(ZE_AT_WARNING_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);}} while(false)
	#else
		#define zeAssert(Condition, ...) do {if (Condition){ZEError::RaiseAssert(ZE_AT_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); abort();}} while(false)
		#define zeWarningAssert(Condition, ...) do {if (Condition) {ZEError::RaiseAssert(ZE_AT_WARNING_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);}} while(false)
	#endif
#else
	#ifdef ZE_DEBUG_FORCE_VERIFY
		#define zeAssert(Condition, ...) do {if (Condition) {}} while(false)
		#define zeWarningAssert(Condition, ...) do {if (Condition) {}} while(false)
	#else
		#define zeAssert(Condition, ...) do {} while(false)
		#define zeWarningAssert(Condition, ...) do {} while(false)
	#endif
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_ZINEK_VERSION_PLATFORM_WINDOWS
		#define zeCriticalError(...) do {if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak(true); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_CRITICAL_ERROR, __VA_ARGS__);} while(false)
	#else
		#define zeCriticalError(...) do {abort(); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_CRITICAL_ERROR, __VA_ARGS__);} while(false)
	#endif
#else
	#define zeCriticalError( ...) {char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_CRITICAL_ERROR, __VA_ARGS__);} while(false)
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_ZINEK_VERSION_PLATFORM_WINDOWS 
		#define zeError(...) do {if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak(true); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_ERROR, __VA_ARGS__);} while(false)
	#else
		#define zeError(...) do {abort(); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_ERROR, __VA_ARGS__);} while(false)
	#endif
#else
	#define zeError(...) do {char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_ERROR, __VA_ARGS__);} while(false)
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_WARNING)
	#ifdef ZE_ZINEK_VERSION_PLATFORM_WINDOWS
		#define zeWarning(...) do {if(_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) ==  1) zeBreak(true); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_WARNING, __VA_ARGS__);} while(false)
	#else
		#define zeWarning(...) do {abort(); char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_WARNING, __VA_ARGS__);} while(false)
	#endif
#else
	#define zeWarning(...) do {char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_WARNING, __VA_ARGS__);} while(false)
#endif

#define zeNotice(...) do {char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_NOTICE, __VA_ARGS__);} while(false)
#define zeLog(...) do {char __MODULE__[ZE_MAX_MODULE_NAME_LENGTH]; ZEError::GetModuleName(__FUNCTION__, __MODULE__); ZEError::RaiseError(__MODULE__, ZE_ET_LOG, __VA_ARGS__);} while(false)


typedef void (*ZEErrorCallback)(const char* Module, ZEErrorType Type, const char* ErrorText);
typedef void (*ZEAssertCallback)(ZEAssertType Type, const char* AssertText, const char* Function, const char* File, ZEInt Line);

class ZEError
{
	public:
		static void			GetModuleName(const char* Function, char* Output);

		static const char*	GetErrorTypeString(ZEErrorType Type);
		static const char*	GetAssertTypeString(ZEAssertType Type);

		static void			SetErrorCallback(ZEErrorCallback Callback);
		static void			SetAssertCallback(ZEAssertCallback Callback);

		static void			RaiseError(const char* Module, ZEErrorType Type, const char* ErrorText, ...);
		static void			RaiseAssert(ZEAssertType Type, const char* Function, const char* File, ZEInt Line, const char* AssertText, ...);
};


#endif
