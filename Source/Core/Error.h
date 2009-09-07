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

#define ZEERROR_MAX_FILENAME_SIZE	255

#ifdef ZEDEBUG_ENABLED
	#define ZEASSERT(Condition, ...) if (Condition) ZEError::GetInstance()->RaiseAssert(ZE_ASSERTTYPE_ASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__) 
	#define ZEWARNINGASSERT(Condition, ...) if (Condition) ZEError::GetInstance()->RaiseAssert(ZE_ASSERTTYPE_WARNINGASSERT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
	#define ZEASSERT(Condition, ...) 
	#define ZEWARNINGASSERT(Condition, ...)
#endif

#if defined(ZEDEBUG_ENABLED) && defined(ZEDEBUG_BREAKONERROR)
	#define zeCriticalError(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_CRITICAL, __VA_ARGS__); abort();}
#else
	#define zeCriticalError(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_CRITICAL, __VA_ARGS__)
#endif

#if defined(ZEDEBUG_ENABLED) && defined(ZEDEBUG_BREAKONERROR)
	#define zeError(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_NONCRITICAL, __VA_ARGS__); abort();}
#else
	#define zeError(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_NONCRITICAL, __VA_ARGS__)
#endif

#if defined(ZEDEBUG_ENABLED) && defined(ZEDEBUG_BREAKONWARNING)
	#define zeWarning(Module, ...) {ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_WARNING, __VA_ARGS__); abort();}
#else
	#define zeWarning(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_WARNING, __VA_ARGS__)
#endif

#define zeNotice(Module, ...) ZEError::GetInstance()->RaiseError(Module, ZE_ERRORLEVEL_NOTICE, __VA_ARGS__)

enum ZEErrorType
{
	ZE_ERRORLEVEL_CRITICAL,
	ZE_ERRORLEVEL_NONCRITICAL,
	ZE_ERRORLEVEL_WARNING,
	ZE_ERRORLEVEL_NOTICE
};

enum ZEAssertType
{
	ZE_ASSERTTYPE_ASSERT,
	ZE_ASSERTTYPE_WARNINGASSERT,
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
		void						RaiseError(const char* Module, ZEErrorType ErrorType, const char* ErrorFormat, va_list Parameters);
		void						RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, ...);
		void						RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, va_list Parameters);

		static ZEError*				GetInstance();

									ZEError();
									~ZEError();
};

#endif
