//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLauncher.h
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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEInitializable.h"
#include "ZEDestroyable.h"

class ZELNModule;
class ZELNLauncherWindow;
class ZEMLReaderNode;

enum ZELNLaunchStatus
{
	ZELN_LS_NOT_RUNNING,
	ZELN_LS_LAUNCHING,
	ZELN_LS_RUNNING,
	ZELN_LS_TERMINATING,
	ZELN_LS_ERROR
};

struct ZELNLaunchInformation
{
	ZEString			BinaryPath;
	ZEArray<ZEString>	Parameters;
	ZEInt64				ProcessId;
};

class ZELNLauncher : public ZEInitializable, public ZEDestroyable
{
	private:
		static ZELNLauncher*			Instance;
		ZEArray<ZELNModule*>			Modules;
		ZELNLauncherWindow*				Window;
		ZELNLaunchStatus				Status;
		bool							AllowedToLaunch;
		ZELNLaunchInformation			Information;
		ZEString						ProductName;
		ZEUInt							ProductVersionMajor;
		ZEUInt							ProductVersionMinor;
		ZEString						ProductMainExecutable;
		ZEString						CommandLineArguments;

		void							InitializeApplication(const ZEMLReaderNode& ApplicationNode);
		void							InitializeModules(const ZEMLReaderNode& ModulesNode);

		virtual bool					InitializeInternal();
		virtual bool					DeinitializeInternal();

										ZELNLauncher();
		virtual							~ZELNLauncher();

	public:
		const ZEArray<ZELNModule*>		GetModules();
		ZELNLauncherWindow*				GetWindow();

		void							SetProductName(const ZEString& Name);
		const ZEString&					GetProductName();

		void							SetProductMainExecutable(const ZEString& FileName);
		const ZEString&					GetProductMainExecutable();

		void							SetProductMajorVersion(ZEUInt Version);
		ZEUInt							GetProductVersionMajor();
		
		void							SetProductMinorVersion(ZEUInt Version);
		ZEUInt							GetProductVersionMinor();

		ZELNLaunchStatus				GetStatus();
		const ZELNLaunchInformation&	GetInformation();
		bool							GetAllowedToLaunch();

		void							Launch();
		void							Terminate();

		void							Update();

		static ZELNLauncher*			GetInstance();
		static ZELNLauncher*			CreateInstance();
};
