//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - UIManager.cpp
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

#include "UIManager.h"
#include "UIRenderer.h"
#include "UIControl.h"
#include "Core/Error.h"

ZEUIManager::ZEUIManager() 
{
	UIRenderer = NULL;
}

ZEUIManager::~ZEUIManager() 
{
	if (UIRenderer != NULL)
		UIRenderer->Destroy();
}

void ZEUIManager::AddControl(ZEUIControl* Control)
{
	zeAssert(Controls.FindIndex(Control) != -1, "UI Control already added to ZEUIManager. (Control Name : %s)", Control->GetName());

	Controls.Add(Control);
}

void ZEUIManager::RemoveControl(ZEUIControl* Control)
{
	Controls.DeleteValue(Control);
}

ZEArray<ZEUIControl*>& ZEUIManager::GetControls()
{
	return Controls;
}

#include "UITextControl.h"
#include "FontResource.h"

bool ZEUIManager::Initialize()
{
	if (UIRenderer == NULL)
		UIRenderer = ZEUIRenderer::CreateInstance();

	UIRenderer->Initialize();


	// Test Routines

	ZEUITextControl* TestControl = new ZEUITextControl();

	TestControl->SetPosition(ZEVector2(20, 20));
	TestControl->SetSize(ZEVector2(780.0f, 100.0f));
	TestControl->SetBackgroundColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	TestControl->SetTextColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	TestControl->SetText("Kan kokuyorum ulan !");
	TestControl->SetTextWrap(true);
	TestControl->SetFontSize(ZEVector2(1.0f, 1.0f));
	TestControl->SetFont(ZEFontResource::LoadResource("test.zeFont"));

	AddControl(TestControl);

	return true;
}

void ZEUIManager::Deinitialize()
{
	UIRenderer->Destroy();
	UIRenderer = NULL;
}

void ZEUIManager::ProcessEvents()
{
}

void ZEUIManager::Render(ZERenderer* Renderer)
{
	UIRenderer->Clean();
	for (size_t I = 0; I < Controls.GetCount(); I++)
		Controls[I]->Draw(UIRenderer);

	UIRenderer->Render(Renderer);
}

void ZEUIManager::Destroy()
{
	delete this;
}

ZEUIManager* ZEUIManager::CreateInstance()
{
	return new ZEUIManager();
}
