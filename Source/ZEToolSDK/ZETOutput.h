//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETOutput.h
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

#ifndef __ZET_OUTPUT_H__
#define __ZET_OUTPUT_H__

#include "ZETypes.h"
#include "ZETCompileOptions.h"

enum ZESDKOutputLevel
{
	ZET_OL_NOOUTPUT				= 0,
	ZET_OL_ERROR	 			= 1,
	ZET_OL_WARNINGS				= 2,
	ZET_OL_NOTICE				= 3,
	ZET_OL_LOG					= 4,
	ZET_OL_DEBUG				= 5,
};

#if defined(ZET_DEBUG_ENABLED) && defined(ZET_PLATFORM_WINDOWS)
	#include <crtdbg.h>
#else
	#include <stdlib.h> 
#endif

#define zesdkBreak(Condition) if (Condition) { __asm { ZEInt 3 }}
#define zesdkPrint(...) ZESDKOutput::Output(__VA_ARGS__)
#define zesdkLog(Module, ...) ZESDKOutput::Output(Module, ZET_OL_LOG, __VA_ARGS__)
#define zesdkNotice(Module, ...) ZESDKOutput::Output(Module, ZET_OL_NOTICE, __VA_ARGS__)

#if defined(ZET_DEBUG_ENABLED) && defined(ZET_DEBUG_BREAK_ON_WARNING)
	#ifdef ZE_PLATFORM_WINDOWS
		#define zesdkWarning(Module, ...) {ZESDKOutput::Output(Module, ZET_OL_WARNINGS, __VA_ARGS__); if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zesdkBreak(true);}
	#else
		#define zesdkWarning(Module, ...) {ZESDKOutput::Output(Module, ZET_OL_WARNINGS, __VA_ARGS__); abort();}
	#endif
#else
		#define zesdkWarning(Module, ...) ZESDKOutput::Output(Module, ZET_OL_WARNINGS, __VA_ARGS__)
#endif

#if defined(ZET_DEBUG_ENABLED) && defined(ZET_DEBUG_BREAK_ON_ERROR)
	#ifdef ZE_PLATFORM_WINDOWS
		#define zesdkError(Module, ...) {ZESDKOutput::Output(Module, ZET_OL_ERROR, __VA_ARGS__); if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zesdkBreak(true);}
	#else
		#define zesdkError(Module, ...) {ZESDKOutput::Output(Module, ZET_OL_ERROR, __VA_ARGS__); abort();}
	#endif
#else
	#define zesdkError(Module, ...) ZESDKOutput::Output(Module, ZET_OL_ERROR, __VA_ARGS__)
#endif

#if defined(ZET_DEBUG_ENABLED)
	#define zesdkDebug(Module, ...) ZESDKOutput::Output(Module, ZET_OL_DEBUG, __VA_ARGS__)
#else
	#define zesdkDebug(Module, ...)
#endif

#ifdef ZET_DEBUG_ENABLED
	#ifdef ZET_PLATFORM_WINDOWS
		#define zeskdAssert(Condition, ...) if (Condition) {ZESDKOutput::DebugOutput(false, __FUNCTION__, __FILE__, __LINE, __VA_ARGS__); if (_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zesdkBreak(true);}
		#define zesdkWarningAssert(Condition, ...) if (Condition) {ZESDKOutput::DebugOutput(true, __FUNCTION__, __FILE__, __LINE, __VA_ARGS__, true); if (_CrtDbgReport(_CRT_WARNING, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) == 1) zesdkBreak(true);}
	#else
		#define zesdkAssert(Condition, ...) if (Condition) {ZESDKOutput::DebugOutput(true, __VA_ARGS__); abort();}
		#define zeskdWarningAssert(Condition, ...) if (Condition) {ZESDKOutput::DebugOutput(true, __FUNCTION__, __FILE__, __LINE, __VA_ARGS__, true); abort();}
	#endif
#else
	#ifdef ZET_DEBUG_FORCE_VERIFY
		#define zesdkAssert(Condition, ...) if (Condition) {}
		#define zesdkWarningAssert(Condition, ...) if (Condition) {}
	#else
		#define zesdkAssert(Condition, ...) 
		#define zesdkWarningAssert(Condition, ...)
	#endif
#endif

class ZESDKOutput
{
	public:
		static void SetOutputCallback(void (*OutputCallback)(const char*));
		static void SetOutputLevel(ZESDKOutputLevel Level);

		static void DebugOutput(const char* File, const char* Function, ZEInt Line, bool Warning, const char* Format, ...);
		static void Output(const char* Text);
		static void Output(const char* Module, const char* Text);
		static void Output(const char* Module, const char* Format, ...);
		static void Output(const char* Module, ZESDKOutputLevel Level, const char* Format, ...);
};

#endif
