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

#include "ZETypes.h"
#include "ZELog.h"
#include "ZEPlatform.h"
#include "ZEExport.ZEFoundation.h"
#include "ZEDS/ZEDelegate.h"

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_PLATFORM_COMPILER_MSVC)
	#include <intrin.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h> 
#endif

enum ZEErrorType
{
	ZE_ET_CRITICAL_ERROR		= 4,
	ZE_ET_ERROR					= 3,
	ZE_ET_DEBUG_CHECK_FAILED	= 2,
	ZE_ET_WARNING				= 1,
	ZE_ET_DEBUG_CHECK_WARNING	= 0,
	ZE_ET_NONE					= 0
};

#ifdef ZE_PLATFORM_WINDOWS
	#include <crtdbg.h>
	ZE_EXPORT_ZEFOUNDATION void zeBreakLock();
	ZE_EXPORT_ZEFOUNDATION void zeBreakUnlock();
	#define ZE_BREAK_INTERNAL __debugbreak()
	#define ZE_BREAK_DIALOG_INTERNAL(...) zeBreakLock(); int Result = 0; Result = _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); zeBreakUnlock(); if (Result ==  1) zeBreak(true); 
	#define ZE_MEMORY_CHECK_INTERNAL !_CrtCheckMemory()
#else
	#define ZE_BREAK_INTERNAL abort()
	#define ZE_BREAK_DIALOG_INTERNAL abort()
	#define ZE_MEMORY_CHECK_INTERNAL
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#define ZE_BREAK_ON_ERROR_INTERNAL(...) do {if (ZEError::GetInstance()->GetBreakOnErrorEnabled()) { ZE_BREAK_DIALOG_INTERNAL(__VA_ARGS__); }} while(false)
#else
	#define ZE_BREAK_ON_ERROR_INTERNAL(...) do { } while(false)
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_WARNING)
	#define ZE_BREAK_ON_WARNING_INTERNAL(...) do {if (ZEError::GetInstance()->GetBreakOnWarningEnabled()) { ZE_BREAK_DIALOG_INTERNAL(__VA_ARGS__); }} while(false)
#else
	#define ZE_BREAK_ON_WARNING_INTERNAL(...) do { } while(false)
#endif

#ifdef ZE_DEBUG_ENABLE
	#define zeBreak(Condition) do {if (Condition) ZE_BREAK_INTERNAL;} while(false)
#else
	#define zeBreak(Condition) do { } while(false)
#endif

#define zeCriticalError(...) do {zeLog(ZE_LOG_CRITICAL_ERROR, __VA_ARGS__); ZE_BREAK_ON_ERROR_INTERNAL(__VA_ARGS__); ZEError::GetInstance()->RaiseError(ZE_ET_CRITICAL_ERROR);} while(false)
#define zeError(...) do {zeLog(ZE_LOG_ERROR, __VA_ARGS__); ZE_BREAK_ON_ERROR_INTERNAL(__VA_ARGS__); ZEError::GetInstance()->RaiseError(ZE_ET_ERROR);} while(false)
#define zeWarning(...) do {zeLog(ZE_LOG_WARNING, __VA_ARGS__); ZE_BREAK_ON_WARNING_INTERNAL(__VA_ARGS__); ZEError::GetInstance()->RaiseError(ZE_ET_WARNING);} while(false)

#ifdef ZE_DEBUG_ENABLE
	#define zeDebugError(...) zeError(__VA_ARGS__)
	#define zeDebugWarning(...) zeWarning(__VA_ARGS__)
#else
	#define zeDebugError(...)
	#define zeDebugWarning(...)
#endif

#define ZE_VOID
#define zeCheck(Condition, ReturnValue)	do {if (Condition) {return ReturnValue;}} while(false)
#define zeCheckError(Condition, ReturnValue, ...) do {if (Condition) {zeError(__VA_ARGS__); return ReturnValue;}} while(false)
#define zeCheckWarning(Condition, ReturnValue, ...) do {if (Condition) {zeWarning(__VA_ARGS__); return ReturnValue;}} while(false)
#define zeCheckCriticalError(Condition, ReturnValue, ...) do {if (Condition) {zeCriticalError(__VA_ARGS__); return ReturnValue;}} while(false)

#if defined(ZE_DEBUG_ENABLE)
	#define zeDebugCheck(Condition, ...) do {if (Condition) zeError(__VA_ARGS__);} while(false)
	#define zeDebugCheckWarning(Condition, ...) do {if (Condition) zeWarning(__VA_ARGS__);} while(false)
#else
	#define zeDebugCheck(Condition, ...) do {} while(false)
	#define zeDebugCheckWarning(Condition, ...) do {} while(false)
#endif

#ifdef ZE_DEBUG_MEMORY_CHECKS_ENABLE
	#define ZEDebugCheckMemory() zeDebugCheck(ZE_MEMORY_CHECK_INTERNAL, "Heap problem !");
#else
	#define ZEDebugCheckMemory()
#endif
typedef ZEDelegate<void (ZEErrorType)> ZEErrorCallback;

class ZE_EXPORT_ZEFOUNDATION ZEError
{
	private:
		bool								BreakOnAssertEnabled;
		bool								BreakOnErrorEnabled;
		bool								BreakOnWarningEnabled;
		ZEErrorCallback						ErrorCallback;

											ZEError();

	public:
		static const char*					GetErrorTypeString(ZEErrorType Type);

		void								SetBreakOnDebugCheckEnabled(bool Enabled);
		bool								GetBreakOnDebugCheckEnabled();

		void								SetBreakOnErrorEnabled(bool Enabled);
		bool								GetBreakOnErrorEnabled();

		void								SetBreakOnWarningEnabled(bool Enabled);
		bool								GetBreakOnWarningEnabled();

		void								SetCallback(ZEErrorCallback Callback);
		ZEErrorCallback						GetCallback();

		void								RaiseError(ZEErrorType Type);

		static ZEError*						GetInstance();
};
