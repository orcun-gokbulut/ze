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
#include "ZELog.h"
#include "ZEDLL.h"

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
	ZE_ET_DEBUG_CHECK_WARNING	= 0
};


#if defined(ZE_DEBUG_ENABLE) && defined(ZE_PLATFORM_WINDOWS)
	#define zeBreak(Condition)\
		do\
		{\
			if (Condition)\
			{\
				__debugbreak();\
			}\
		}\
		while(false)
#else
	#define zeBreak(Condition)\
		do\
		{\
			if (Condition)\
			{\
				abort();\
			}\
		}\
		while(false)
#endif

#define ZE_MAX_MODULE_NAME_LENGTH 1024

#ifdef ZE_DEBUG_MEMORY_CHECKS_ENABLE
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		#include <crtdbg.h>
		#define ZEDebugCheckMemory()\
			do\
			{\
				zeDebugCheck(!_CrtCheckMemory(), "Heap problem");\
			}\
			while(false)
	#endif
#else
	#define ZEDebugCheckMemory()\
		do\
		{\
		}\
		while(false)
#endif


#ifdef ZE_DEBUG_ENABLE
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		#define zeDebugCheck(Condition, ...)\
			do\
			{\
				if (Condition)\
				{\
					zeError(__VA_ARGS__);\
					if (ZEError::GetInstance()->GetBreakOnDebugCheckEnabled() && _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1)\
						zeBreak(true);\
				}\
			}\
			while(false)
		#define zeDebugCheckWarning(Condition, ...)\
			do\
			{\
				if (Condition)\
				{\
					zeWarning(__VA_ARGS__);\
				}\
			}\
			while(false)
	#else
		#define zeDebugCheck(Condition, ...) \
			do \
			{\
				if (Condition)\
				{\
					if (ZEError::GetInstance()->GetBreakOnDebugCheckEnabled())\
						abort();\
				}\
			}\
			while(false)
		#define zeDebugCheckWarning(Condition, ...) do {if (Condition) {ZEError::GetInstance()->RaiseDebugCheckFail(ZE_DCT_WARNING, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);}} while(false)
	#endif
#else
	#ifdef ZE_DEBUG_FORCE_VERIFY
		#define zeDebugCheck(Condition, ...)\
			do\
			{\
				if (Condition)\
				{\
				}\
			}\
			while(false)
		#define zeDebugCheckWarning(Condition, ...)\
			do\
			{\
				if (Condition)\
				{\
				}\
			}\
			while(false)
	#else
		#define zeDebugCheck(Condition, ...)\
			do\
			{\
			}\
			while(false)
		#define zeDebugCheckWarning(Condition, ...)\
			do\
			{\
			}\
			while(false)
	#endif
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		#define zeCriticalError(...)\
		do\
		{\
			zeLog(ZE_LOG_CRITICAL_ERROR, __VA_ARGS__);\
			if (ZEError::GetInstance()->GetBreakOnErrorEnabled() && _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1)\
				zeBreak(true);\
			ZEError::GetInstance()->RaiseError(ZE_ET_CRITICAL_ERROR);\
		}\
		while(false)
	#else
		#define zeCriticalError(...)\
			do\
			{\
				zeLog(ZE_LOG_CRITICAL_ERROR, __VA_ARGS__);\
				if (ZEError::GetInstance()->GetBreakOnErrorEnabled())\
					abort();\
				ZEError::GetInstance()->RaiseError(ZE_ET_CRITICAL_ERROR);\
			}\
			while(false)
	#endif
#else
	#define zeCriticalError(...)\
		do\
		{\
			zeLog(ZE_LOG_CRITICAL_ERROR, __VA_ARGS__);\
			ZEError::GetInstance()->RaiseError(ZE_ET_CRITICAL_ERROR);\
		}\
		while(false)
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		#define zeError(...)\
		do\
		{\
			zeLog(ZE_LOG_ERROR, __VA_ARGS__);\
			if (ZEError::GetInstance()->GetBreakOnErrorEnabled() &&_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1)\
				zeBreak(true);\
			ZEError::GetInstance()->RaiseError(ZE_ET_ERROR);\
		}\
		while(false)
	#else
		#define zeError(...)\
			do\
			{\
				zeLog(ZE_LOG_ERROR, __VA_ARGS__);\
				abort();\
				ZEError::GetInstance()->RaiseError(ZE_ET_ERROR);\
			}\
			while(false)
	#endif
#else
	#define zeError(...)\
		do\
		{\
			zeLog(ZE_LOG_ERROR, __VA_ARGS__);\
			ZEError::GetInstance()->RaiseError(ZE_ET_ERROR);\
		}\
		while(false)
#endif

#if defined(ZE_DEBUG_ENABLE) && defined(ZE_DEBUG_BREAK_ON_WARNING)
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		#define zeWarning(...)\
			do\
			{\
				zeLog(ZE_LOG_WARNING, __VA_ARGS__);\
				if(ZEError::GetInstance()->GetBreakOnWarningEnabled() && _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) ==  1)\
					zeBreak(true);\
				ZEError::GetInstance()->RaiseError(ZE_ET_WARNING);\
			}\
			while(false)
	#else
		#define zeWarning(...)\
			do\
			{\
				zeLog(ZE_LOG_WARNING, __VA_ARGS__);\
				if(ZEError::GetInstance()->GetInstance()->GetBreakOnWarningEnabled())\
					abort();\
				ZEError::GetInstance()->RaiseError(ZE_ET_WARNING);\
			}\
			while(false)
	#endif
#else
	#define zeWarning(...)\
		do\
		{\
			zeLog(ZE_LOG_WARNING, __VA_ARGS__);\
			ZEError::GetInstance()->RaiseError(ZE_ET_WARNING);\
		}\
		while(false)
#endif

typedef void (*ZEErrorCallback)(ZEErrorType Type);

class ZEError
{
	private:
		bool				BreakOnAssertEnabled;
		bool				BreakOnErrorEnabled;
		bool				BreakOnWarningEnabled;
		ZEErrorCallback		ErrorCallback;

							ZEError();

	public:
		static const char*	GetErrorTypeString(ZEErrorType Type);

		void				SetBreakOnDebugCheckEnabled(bool Enabled);
		bool				GetBreakOnDebugCheckEnabled();

		void				SetBreakOnErrorEnabled(bool Enabled);
		bool				GetBreakOnErrorEnabled();

		void				SetBreakOnWarningEnabled(bool Enabled);
		bool				GetBreakOnWarningEnabled();

		void				SetCallback(ZEErrorCallback Callback);

		void				RaiseError(ZEErrorType Type);

		ZE_DLL_METHOD 
		static ZEError*		GetInstance();
};

#endif
