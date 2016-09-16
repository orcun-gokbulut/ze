//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLogModule.cpp
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

#include "ZELNLogModule.h"

#include "ui_ZELNLogWidget.h"

#include "ZELog.h"
#include "ZELogSession.h"

ZELN_MODULE_DECRIPTION(ZELNLogModule, "Log");

void ZELNLogModule::LogCallback(const ZELogSession* Session, const char* ModuleName, ZELogType Type, const char* LogText, void* ExtraParameters)
{
	ZELNLogModule* Module = static_cast<ZELNLogModule*>(ExtraParameters);
	const char* TypeString = ZELog::GetLogTypeString(Type);
	Module->Form->txtConsole->appendHtml(
		QString("[%1] <b>%2</b>: %3")
		.arg(ModuleName)
		.arg(TypeString)
		.arg(LogText));
}

bool ZELNLogModule::InitializeInternal()
{
	if (!ZELNModule::InitializeInternal())
		return false;

	Widget = new QWidget();
	Form = new Ui_ZELNLogWidget();
	Form->setupUi(Widget);

	Session.SetCallback(LogCallback);
	Session.SetCallbackParameter(this);
	Session.SetSink(true);
	Session.BeginSession();
	
	return true;
}

bool ZELNLogModule::DeinitializeInternal()
{
	Session.EndSession();

	delete Widget;
	Widget = NULL;

	delete Form;
	Form = NULL;

	return ZELNModule::DeinitializeInternal();
}

QWidget* ZELNLogModule::GetWidget()
{
	return Widget;
}
