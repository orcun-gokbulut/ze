//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStateScreen.cpp
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

#include "ZEStateScreen.h"

#include "ZEScene.h"
#include "ZEUI/ZEUIFontTrueType.h"
#include "ZEUI/ZEUIManager.h"
#include "ZEUI/ZEUILabel.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZECore/ZEConsole.h"
#include "ZEDS/ZEFormat.h"

ZEEntityResult ZEStateScreen::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	if (Manager == NULL)
		return ZE_ER_FAILED;

	ConsoleLines.SetCount(5);
	for (ZESize I = 0; I < ConsoleLines.GetCount(); I++)
	{
		ConsoleLines[I] = ZEUILabel::CreateInstance();
		Manager->AddControl(ConsoleLines[I]);
	}

	LoadingLabel = ZEUILabel::CreateInstance();
	LoadingLabel->SetPosition(ZEVector2::Zero);
	LoadingLabel->SetSize(ZEVector2(100, 100));
	Manager->AddControl(LoadingLabel);

	return ZE_ER_DONE;
}

ZEEntityResult ZEStateScreen::UnloadInternal()
{
	for (ZESize I = 0; I < ConsoleLines.GetCount(); I++)
	{
		Manager->RemoveControl(LoadingLabel);
		ConsoleLines[I]->Destroy();
	}
	ConsoleLines.Clear();

	Manager->RemoveControl(LoadingLabel);
	LoadingLabel->Destroy();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEStateScreen::ZEStateScreen()
{
	LastLoadingPercentage = -1;
	LastOutputBufferCount = 0;

	SetEntityFlags(ZE_EF_RENDERABLE_CUSTOM | ZE_EF_TICKABLE_CUSTOM);
}

ZEStateScreen::~ZEStateScreen()
{
	
}

void ZEStateScreen::SetManager(ZEUIManager* Manager)
{
	if (this->Manager == Manager)
		return;

	this->Manager = Manager;

	if (IsLoaded())
		Reload();
}

ZEUIManager* ZEStateScreen::GetManager()
{
	return Manager;
}

void ZEStateScreen::Tick(float ElapsedTime)
{
//	LoadingPercentage = GetScene()->GetLoadingPercentage();
}

bool ZEStateScreen::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (LoadingPercentage == 100)
	{
		LoadingLabel->SetVisiblity(false);
	}
	else
	{
		LoadingLabel->SetVisiblity(true);

		if (LastLoadingPercentage != LoadingPercentage)
		{
			LoadingLabel->SetText(ZEFormat::Format("Loading... %{0}", LoadingPercentage));
			zeLog(LoadingLabel->GetText().ToCString());
			LastLoadingPercentage = LoadingPercentage;
		}
	}

	return false;
}

ZEStateScreen* ZEStateScreen::CreateInstance()
{
	return new ZEStateScreen();
}
