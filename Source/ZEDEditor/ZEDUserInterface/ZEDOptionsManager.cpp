//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDOptionsManager.cpp
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

#include "ZEDOptionsManager.h"

#include "ZEError.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDOptionsWindow.h"
#include "ZEDOptionsPage.h"
#include "ZEDOptionsWindowSingle.h"
#include "ZEDOptionsWindowMultiple.h"
#include "ZEDCommandManager.h"
#include "ZEDMainWindow.h"

void ZEDOptionsManager::RegisterCommands()
{
	OptionsCommand.SetText("ZEDOptionsManager::OptionsCommand");
	OptionsCommand.SetCategory("Application");
	OptionsCommand.SetText("Options");
	OptionsCommand.SetType(ZED_CT_COMMAND);
	OptionsCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Select.png");
	OptionsCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_Q));
	OptionsCommand.OnAction.AddDelegate<ZEDOptionsManager, &ZEDOptionsManager::OptionsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&OptionsCommand);
}

void ZEDOptionsManager::OptionsCommand_OnAction(const ZEDCommand* Command)
{
	ShowOptionsDialog();
}

bool ZEDOptionsManager::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	RegisterCommands();

	return true;
}

bool ZEDOptionsManager::DeinitializeInternal()
{
	return ZEDComponent::DeinitializeInternal();
}

ZEDOptionsManager::ZEDOptionsManager()
{

}

ZEDOptionsManager::~ZEDOptionsManager()
{
	zeDebugCheck(OptionsPages.GetCount() != 0, "There are still registered pages available.");
	DeinitializeInternal();
}

const ZEArray<ZEDOptionsPage*>& ZEDOptionsManager::GetOptionsPages() const
{
	return OptionsPages;
}

bool ZEDOptionsManager::RegisterOptionsPage(ZEDOptionsPage* OptionsPage)
{
	zeCheckError(OptionsPage == NULL, false, "Cannot register options. OptionsPage is NULL.");
	zeCheckError(OptionsPage->Manager != NULL, false, "Cannot register options page. Options page is already registered. Options Page Name: \"%s\".", OptionsPage->GetName());

	OptionsPage->Manager = this;
	OptionsPages.Add(OptionsPage);

	return true;
}

bool ZEDOptionsManager::UnregisterOptionsPage(ZEDOptionsPage* OptionsPage)
{
	zeCheckError(OptionsPage == NULL, false, "Cannot unregister options page. OptionPage is NULL.");
	zeCheckError(OptionsPage->Manager != this, false, "Cannot unregister options page. Options page is not registered. Option Page Name: \"%s\".", OptionsPage->GetName());

	OptionsPage->Manager = NULL;
	OptionsPages.RemoveValue(OptionsPage);

	return true;
}

void ZEDOptionsManager::ShowOptionsDialog(ZEDOptionsPage* SelectedOption)
{
	ZEDOptionsWindow* Window = new ZEDOptionsWindow(GetEditor()->GetMainWindow()->GetMainWindow());
	Window->Setup(this);
	Window->exec();
}

void ZEDOptionsManager::ShowSingleOptions(ZEDOptionsPage* Options)
{
	ZEDOptionsWindowSingle* Single = new ZEDOptionsWindowSingle(GetEditor()->GetMainWindow()->GetMainWindow());
	Single->Setup(Options);
	Single->exec();
}

void ZEDOptionsManager::ShowMultipleOptions(const ZEArray<ZEDOptionsPage*> Options)
{
	ZEDOptionsWindowMultiple* Multiple = new ZEDOptionsWindowMultiple(GetEditor()->GetMainWindow()->GetMainWindow());
	Multiple->Setup(Options);
	Multiple->exec();
}

ZEDOptionsManager* ZEDOptionsManager::CreateInstance()
{
	return new ZEDOptionsManager();
}
