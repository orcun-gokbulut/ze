//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELogSession.h
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

#include "ZELog.h"
#include "ZEDS/ZELink.h"

typedef void (*ZELogCallback)(const ZELogSession*, const char* Module, ZELogType Type, const char* LogText, void* ExtraParameters);

class ZELogSession
{
	friend class ZELog;
	private:
		ZELink<ZELogSession>			Link;
		ZESSize							SessionID;
		ZEUInt32						ThreadID;
		ZELogType						MinimumLevel;
		bool							Sink;
		bool							Block;
		ZELogType						LogFileMinLevel;
		void*							LogFile;
		ZEString						LogFileName;
		ZELogCallback					Callback;
		void*							CallbackParameter;

	public:
		ZESize							GetSessionID() const;
		ZEUInt32						GetThreadID() const;

		void							SetMinimumLevel(ZELogType Level);
		ZELogType						GetMinimumLevel() const;

		void							SetBlock(bool Block);
		bool							GetBlock() const;

		void							SetSink(bool Sink);
		bool							GetSink() const;

		void							SetCallback(ZELogCallback Callback);
		ZELogCallback					GetCallback() const;

		void							SetCallbackParameter(void* Parameter);
		void*							GetCallbackParameter() const;

		const ZEString&					GetLogFileName() const;

		void							SetLogFileMinLevel(ZELogType Level);
		ZELogType						GetLogFileMinLevel() const;				

		bool							OpenLogFile(const ZEString& FileName, const ZEString& Header, bool Append);
		void							CloseLogFile();

		void							BeginSession();
		void							EndSession();

										ZELogSession();
										~ZELogSession();

		static ZEString					GenerateLogFileName(const ZEString& BaseName);
		static ZEString					GenerateLogFileHeader(const ZEString& ApplicationName = ZEString::Empty);
};
