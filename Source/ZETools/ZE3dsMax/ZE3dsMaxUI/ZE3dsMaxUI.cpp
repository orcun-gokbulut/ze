//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUI - Copy.cpp
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

#include "ZE3dsMaxUI.h"
#include "imenus.h"
#include "imenuman.h"

class ZEMakeZEMeshAction : public ActionItem
{
	public:
		virtual BOOL			ExecuteAction();
		CoreExport BOOL			Execute();
		virtual void			GetButtonText(MSTR &buttonText);
		virtual void			GetMenuText(MSTR &menuText);
		virtual void			GetDescriptionText(MSTR &descText);
		virtual void			GetCategoryText(MSTR &catText);
		virtual BOOL			IsChecked();
		virtual BOOL			IsItemVisible();
		virtual BOOL			IsEnabled();
		virtual MaxIcon*		GetIcon();
		virtual void			DeleteThis();

};
DWORD ZE3dsMaxUI::Start()
{
	// Define Actions
	// Menus

	MessageBox(NULL, "Start", "B�ll�", MB_OK);
	return GUPRESULT_KEEP;
}

class ZEMenuItem : public IMenuItem
{
	public:

};

void ZE3dsMaxUI::Stop()
{
	MessageBox(NULL, "B�ll�k", "Stop", MB_OK);
}

ZE3dsMaxUI::ZE3dsMaxUI()
{

}

ZE3dsMaxUI::~ZE3dsMaxUI()
{

}
