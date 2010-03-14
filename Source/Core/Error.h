//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Error.h
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

#include "Definitions.h"
#include "CompileOptions.h"
#include "Option.h"

#if defined(ZE_DEBUG_ENABLED) && defined(ZE_PLATFORM_WINDOWS)
	#include <crtdbg.h>
#else
	#include <stdlib.h> 
#endif

#define ZEERROR_MAX_FILENAME_SIZE	255

#define zeBreak() __asm{ int 3 }

#ifdef ZE_DEBUG_ENABLED
	#ifdef ZE_PLATFORM_WINDOWS
		#define zeAssert(Condition, ...) if (Condition){ZEError::GetInstance()->RaiseAssert(ZE_AT_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); if (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak();} 
		#define zeWarningAssert(Condition, ...) if (Condition) ZEError::GetInstance()->RaiseAssert(ZE_AT_WARNING_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
	#else
		#define zeAssert(Condition, ...) if (Condition){ZEError::GetInstance()->RaiseAssert(ZE_AT_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); abort();} 
		#define zeWarningAssert(Condition, ...) if (Condition) ZEError::GetInstance()->RaiseAssert(ZE_AT_WARNING_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
	#endif
#else
	#ifdef ZE_DEBUG_FORCE_VERIFY
		#define zeAssert(Condition, ...) if (Condition) {}
		#define zeWarningAssert(Condition, ...) if (Condition) {}
	#else
		#define zeAssert(Condition, ...) 
		#define zeWarningAssert(Condition, ...)
	#endif
#endif

#if defined(ZE_DEBUG_ENABLED) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_PLATFORM_WINDOWS
		#define zeCriticalError(Module, ...) {if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak();ZEError::GetInstance()->RaiseError(Module, ZE_EL_CRITICAL, __VA_ARGS__);}
	#else
		#define zeCriticalError(Module, ...) {abort(); ZEError::GetInstance()->RaiseError(Module, ZE_EL_CRITICAL, __VA_ARGS__);}
	#endif
#else
	#define zeCriticalError(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_EL_CRITICAL, __VA_ARGS__)
#endif

#if defined(ZE_DEBUG_ENABLED) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_PLATFORM_WINDOWS
		#define zeError(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_EL_NONCRITICAL, __VA_ARGS__); if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zeBreak();}
	#else
		#define zeError(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_EL_NONCRITICAL, __VA_ARGS__); abort();}
	#endif
#else
	#define zeError(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_EL_NONCRITICAL, __VA_ARGS__)
#endif

#if defined(ZE_DEBUG_ENABLED) && defined(ZE_DEBUG_BREAK_ON_WARNING)
	#ifdef ZE_PLATFORM_WINDOWS
		#define zeWarning(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_EL_WARNING, __VA_ARGS__); if(_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) ==  1) zeBreak();}
	#else
		#define zeWarning(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_EL_WARNING, __VA_ARGS__); abort();}
	#endif
#else
	#define zeWarning(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_EL_WARNING, __VA_ARGS__)
#endif

#define zeNotice(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_EL_NOTICE, __VA_ARGS__)

enum ZEErrorType
{
	ZE_EL_CRITICAL,
	ZE_EL_NONCRITICAL,
	ZE_EL_WARNING,
	ZE_EL_NOTICE
};

enum ZEAssertType
{
	ZE_AT_ASSERT,
	ZE_AT_WARNING_ASSERT,
};

class ZEError
{
	private:
		static ZEOptionSection		ErrorOptions;
		bool						FileLogging;
		char						LogFileName[ZEERROR_MAX_FILENAME_SIZE];

		static char*				ErrorLevelToString(ZEErrorType ErrorLevel);
		void						LogToFile(const char* Module, ZEErrorType ErrorType, const char* Error);
		bool						OptionCallback_General(ZEOption* Option, ZETypedVariant* Value);
	public:
		void						EnableFileLogging();
		void						DisableFileLogging();

		void						SetLogFileName(const char* NewLogFile);
		char*						GetLogFileName();

		void						RaiseError(const char* Module, ZEErrorType ErrorType, const char* ErrorFormat, ... );
		void						RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, ...);

		static ZEError*				GetInstance();

									ZEError();
									~ZEError();
};

#endif
