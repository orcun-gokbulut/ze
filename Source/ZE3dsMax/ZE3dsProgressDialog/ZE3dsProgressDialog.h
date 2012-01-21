//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsProgressDialog.h
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
#ifndef __PROGRESS_DIALOG_H__
#define __PROGRESS_DIALOG_H__

#include "ZETypes.h"

#include <tchar.h>
#include <stdio.h>


#define zepdOutput(...) ZE3dsProgressDialog::GetInstance()->Output(__VA_ARGS__)
#define zepdLog(...) ZE3dsProgressDialog::GetInstance()->Output(ZE3DS_PDOL_LOG, __VA_ARGS__)
#define zepdWarning(...) ZE3dsProgressDialog::GetInstance()->Output(ZE3DS_PDOL_WARNINGS, __VA_ARGS__)
#define zepdError(...) ZE3dsProgressDialog::GetInstance()->Output(ZE3DS_PDOL_ERRORS, __VA_ARGS__)

enum ZE3dsProgressDialogOutputLevel
{
	ZE3DS_PDOL_ERRORS = 0,
	ZE3DS_PDOL_WARNINGS = 1,
	ZE3DS_PDOL_NOTICE = 2,
	ZE3DS_PDOL_LOG = 3

};

class ZE3dsProgressDialog
{
	private:
		void*						handle;
		ZEInt							UpdateTreshold;
		ZEInt							OutputLevel;
		FILE*						LogFile;
		static	ZE3dsProgressDialog*	Instance;

	public:
		void						Create(void* Instance);

		void						Show();
		void						Hide();

		void						Update();
		void						ProcessMessages();

		void						SetOutputLevel(ZE3dsProgressDialogOutputLevel OutputLevel);

		void						StartFileLogging(TCHAR* Filename);
		void						StopFileLogging();
		
		bool						IsCanceled();
		void						SetExitMode(bool Exit);

		void						Output(TCHAR* Format, ...);
		void						Output(ZE3dsProgressDialogOutputLevel OutputLevel, TCHAR* Format, ...);

		void						SetProgress(ZEInt Value);
		void						SetProgressRange(ZEInt Min, ZEInt Max);

		void						SetPartialProgress(ZEInt Value);
		void						SetPartialProgressRange(ZEInt Min, ZEInt Max);

									ZE3dsProgressDialog();
									~ZE3dsProgressDialog();

		static ZE3dsProgressDialog*	GetInstance();
};
#endif
