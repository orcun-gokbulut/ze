//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESystemMessageManager.cpp
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
#include "ZETypes.h"
#include "ZECore.h"
#include "ZEError.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"

#define WINDOWS_MEAN_AND_LEAN
#include <Windows.h>


ZESystemMessageManager::ZESystemMessageManager()
{

}

ZESystemMessageManager::~ZESystemMessageManager()
{

}

const ZEArray<ZESystemMessageHandler*>& ZESystemMessageManager::GetMessageHandlers()
{
	return Handlers;
}

void ZESystemMessageManager::RegisterMessageHandler(ZESystemMessageHandler* Handler)
{
	if (Handlers.Exists(Handler))
	{
		zeError("Handler is already added.");
		return;
	}

	Handlers.Add(Handler);
}

void ZESystemMessageManager::UnregisterMessageHandler(ZESystemMessageHandler* Handler)
{
	Handlers.DeleteValue(Handler);
}

void ZESystemMessageManager::ProcessMessages()
{
	MSG Msg;
	ZeroMemory(&Msg, sizeof(Msg));
	while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{	
		TranslateMessage(&Msg);

		bool Handled = false;

		for(ZESize I = 0; I < Handlers.GetCount(); I++)
			if (Msg.message >= Handlers[I]->MinMessage && Msg.message <= Handlers[I]->MaxMessage &&
				(Handlers[I]->TargetWindow == NULL || Handlers[I]->TargetWindow == Msg.hwnd))
			{
				if (Handlers[I]->Callback(&Msg))
					Handled = true;
			}

		/*if (!Handled)
			DefWindowProc(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);*/

		DispatchMessage(&Msg);
	}
}

ZESystemMessageManager* ZESystemMessageManager::GetInstance()
{
	return ZECore::GetInstance()->GetSystemMessageManager();
}
