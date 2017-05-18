//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashHandler_Windows.cpp
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

#include "ZECrashHandler.h"
#include "ZECore.h"
#include "ZEDS/ZEFormat.h"
#include "ZEGUID.h"
#include "ZEBase64.h"
#include "ZEVersion.h"
#include "ZELog.h"
#include "ZELogSession.h"
#include "ZEFile/ZEFileInfo.h"

#include <stdlib.h>
#include <signal.h>
#include <new.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef void (__cdecl * SignalHandlerType)(int);

LPTOP_LEVEL_EXCEPTION_FILTER OldUnhandledExceptionHandler = NULL;
static _PNH OldOutOfMemoryHandler = NULL;
static _invalid_parameter_handler OldInvalidParameterHandler = NULL;
static _purecall_handler OldPureVirtualCallHandler = NULL;
static SignalHandlerType OldAbortSignalHandler = NULL;
static SignalHandlerType OldInvalidInstructionSignalHandler = NULL;
static SignalHandlerType OldTerminateSignalHandler = NULL;

#include "ZEThread/ZEThread.h"
#include "ZETools/ZECrashReport/ZECRReportParameters.h"



LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* Ex)
{
	ZECrashHandler* CrashHandler = ZECore::GetInstance()->GetCrashHandler();
	if (CrashHandler != NULL)
	{

		switch (Ex->ExceptionRecord->ExceptionCode)
		{
			case EXCEPTION_ACCESS_VIOLATION:
				CrashHandler->Crashed(ZE_CR_ACCESS_VIOLATION);
				return EXCEPTION_EXECUTE_HANDLER;

			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				CrashHandler->Crashed(ZE_CR_INDEX_OUT_OF_BOUNDS);
				return EXCEPTION_EXECUTE_HANDLER;

			case EXCEPTION_PRIV_INSTRUCTION:
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				CrashHandler->Crashed(ZE_CR_ILLEGAL_INSTRUCTION);
				return EXCEPTION_EXECUTE_HANDLER;

			case EXCEPTION_STACK_OVERFLOW:
				CrashHandler->Crashed(ZE_CR_STACK_OVERFLOW);
				return EXCEPTION_EXECUTE_HANDLER;

			case EXCEPTION_INVALID_HANDLE:
				CrashHandler->Crashed(ZE_CR_INVALID_CALL);
				return EXCEPTION_EXECUTE_HANDLER;

			case EXCEPTION_GUARD_PAGE:
			case EXCEPTION_IN_PAGE_ERROR:
				CrashHandler->Crashed(ZE_CR_OTHER);
				return EXCEPTION_EXECUTE_HANDLER;

			default:
			case EXCEPTION_DATATYPE_MISALIGNMENT:
			case EXCEPTION_BREAKPOINT:
			case EXCEPTION_SINGLE_STEP:
			case EXCEPTION_FLT_DENORMAL_OPERAND:
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			case EXCEPTION_FLT_INEXACT_RESULT:
			case EXCEPTION_FLT_INVALID_OPERATION:
			case EXCEPTION_FLT_OVERFLOW:
			case EXCEPTION_FLT_STACK_CHECK:
			case EXCEPTION_FLT_UNDERFLOW:
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			case EXCEPTION_INT_OVERFLOW:
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			case EXCEPTION_INVALID_DISPOSITION:
			case CONTROL_C_EXIT:
				CrashHandler->Crashed(ZE_CR_OTHER);
				return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	if (OldUnhandledExceptionHandler != NULL)
		return OldUnhandledExceptionHandler(Ex);

	return EXCEPTION_EXECUTE_HANDLER;
}

static void PureVirtualCallHandler()
{
	ZECrashHandler* CrashHandler = ZECore::GetInstance()->GetCrashHandler();
	if (CrashHandler != NULL)
		CrashHandler->Crashed(ZE_CR_PURE_VIRTUAL_CALL);

	if (OldPureVirtualCallHandler != NULL)
		OldPureVirtualCallHandler();
}

static int OutOfMemoryHandler(size_t A)
{  
	ZECrashHandler* CrashHandler = ZECore::GetInstance()->GetCrashHandler();
	if (CrashHandler != NULL)
		CrashHandler->Crashed(ZE_CR_OUT_OF_MEMORY);

	if (OldOutOfMemoryHandler != NULL)
		return OldOutOfMemoryHandler(A);

	return 0;
}  

static void InvalidParameterHandler(const wchar_t* A, const wchar_t* B, const wchar_t* C, unsigned int D, uintptr_t E)
{
	ZECrashHandler* CrashHandler = ZECore::GetInstance()->GetCrashHandler();
	if (CrashHandler != NULL)
		CrashHandler->Crashed(ZE_CR_INVALID_CALL);

	if (OldInvalidParameterHandler != NULL)
		OldInvalidParameterHandler(A, B, C, D, E);
}

static void SignalHandler(int Signal)
{
	ZECrashHandler* CrashHandler = ZECore::GetInstance()->GetCrashHandler();

	switch (Signal)
	{
		default:
			break;
		
		case SIGTERM:
			if (CrashHandler != NULL)
				CrashHandler->Crashed(ZE_CR_PREMATURE_TERMINATION);
			
			if (OldTerminateSignalHandler != NULL)
				OldTerminateSignalHandler(Signal);

			break;

		case SIGABRT:
			if (CrashHandler != NULL)
				CrashHandler->Crashed(ZE_CR_ABORT);

			if (OldAbortSignalHandler != NULL)
				OldAbortSignalHandler(Signal);

			break;
		
		case SIGILL:
			if (CrashHandler != NULL)
				CrashHandler->Crashed(ZE_CR_OTHER);

			if (OldInvalidInstructionSignalHandler != NULL)
				OldInvalidInstructionSignalHandler(Signal);

			break;
	}
}

void ZECrashHandler::RegisterHandlers()
{
	_set_new_mode(1);

	OldUnhandledExceptionHandler = SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	OldOutOfMemoryHandler = _set_new_handler(OutOfMemoryHandler);
	OldInvalidParameterHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
	OldPureVirtualCallHandler = _set_purecall_handler(PureVirtualCallHandler);   
	OldAbortSignalHandler = signal(SIGABRT, SignalHandler);
	OldInvalidInstructionSignalHandler = signal(SIGILL, SignalHandler);
	OldTerminateSignalHandler = signal(SIGTERM, SignalHandler);
}

void ZECrashHandler::UnregisterHandlers()
{
	SetUnhandledExceptionFilter(OldUnhandledExceptionHandler);
	_set_new_handler(OldOutOfMemoryHandler);
	_set_invalid_parameter_handler(OldInvalidParameterHandler);
	_set_purecall_handler(OldPureVirtualCallHandler);   
	signal(SIGABRT, OldAbortSignalHandler);
	signal(SIGILL, OldInvalidInstructionSignalHandler);
	signal(SIGTERM, OldTerminateSignalHandler);
}

bool ZECrashHandler::InitializeInternal()
{
	if (!ZEModule::InitializeInternal())
		return false;

	RegisterHandlers();

	return true;
}

bool ZECrashHandler::DeinitializeInternal()
{
	UnregisterHandlers();

	return ZEModule::DeinitializeInternal();
}

ZECrashHandler::ZECrashHandler()
{
	#if ZE_DEBUG_ENABLE
		ExecuteCrashReporter = true;
	#else
		ExecuteCrashReporter = true;
	#endif
}

ZECrashHandler::~ZECrashHandler()
{
	Deinitialize();
}

void ZECrashHandler::Crashed(ZECrashReason Reason)
{
	if (!GetEnabled())
		return;

	if (!ExecuteCrashReporter)
		return;

	CrashLock.Lock();

	HANDLE Process = GetCurrentProcess();

	UnregisterHandlers();

	ZEString GUID = ZEGUID::Generate().ToString();
	ZEString NamedPipeName = ZEFormat::Format("\\\\.\\pipe\\ZECRCrashReporter-{0}", GUID);
	HANDLE NamedPipeHandle = CreateNamedPipe(NamedPipeName.ToCString(), PIPE_ACCESS_DUPLEX,	PIPE_TYPE_MESSAGE | PIPE_NOWAIT, 1, NULL, NULL, INFINITE, NULL);
	if (NamedPipeHandle == INVALID_HANDLE_VALUE)
		TerminateProcess(GetModuleHandle(NULL), EXIT_FAILURE);

	ZEString DLLPath = ZEFileInfo("#E:/").GetRealPath().Path;
	#ifdef ZE_DEBUG_ENABLE
	const char* DLLFileName = "ZECRCrashReporter-Debug.dll";
	#else	
	const char* DLLFileName = "ZECRCrashReporter.dll";
	#endif

	ZEString CommandArgument = ZEFormat::Format("rundll32.exe \"{0}\\{1}\", ReportCrash {2}", DLLPath, DLLFileName, NamedPipeName);

	STARTUPINFO StartUpInfo;
	StartUpInfo.cb = sizeof(STARTUPINFO);
	memset(&StartUpInfo, 0, sizeof(STARTUPINFO));

	PROCESS_INFORMATION ProcessInfo;
	memset(&ProcessInfo, 0, sizeof(PROCESS_INFORMATION));

	if (!CreateProcess(NULL, (char*)CommandArgument.ToCString(), NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, DLLPath, &StartUpInfo, &ProcessInfo))
		TerminateProcess(Process, EXIT_FAILURE);

	DWORD Signal = 0;
	DWORD ExitCode;
	while (GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode))
	{
		if (ExitCode != STILL_ACTIVE)
			TerminateProcess(Process, EXIT_FAILURE);

		DWORD ReadCount = 0;
		if (!ReadFile(NamedPipeHandle, &Signal, sizeof(DWORD), &ReadCount, NULL))
			TerminateProcess(NamedPipeHandle, EXIT_FAILURE);
		
		if (ReadCount == 0)
		{
			WaitForSingleObject(NamedPipeHandle, 1000);
			continue;
		}

		break;
	}

	if (Signal != 0xEEFF0012)
		TerminateProcess(Process, EXIT_FAILURE);

	ZECRReportParameters Parameters;
	memset(&Parameters, 0, sizeof(ZECRReportParameters));
	GenerateParameters(Parameters);
	Parameters.Reason = Reason;
	Parameters.ProcessId = GetCurrentProcessId();
	GetModuleFileName(NULL, Parameters.Executable, 1024);
	
	if (!WriteFile(NamedPipeHandle, &Parameters, sizeof(ZECRReportParameters), NULL, NULL))
		TerminateProcess(Process, EXIT_FAILURE);

	while (GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode))
	{
		if (ExitCode != STILL_ACTIVE)
			TerminateProcess(Process, EXIT_FAILURE);

		DWORD ReadCount = 0;
		if (!ReadFile(NamedPipeHandle, &Signal, sizeof(DWORD), &ReadCount, NULL))
			TerminateProcess(NamedPipeHandle, EXIT_FAILURE);

		if (ReadCount == 0)
		{
			WaitForSingleObject(NamedPipeHandle, 1000);
			continue;
		}

		break;
	}

	TerminateProcess(Process, EXIT_FAILURE);
}
