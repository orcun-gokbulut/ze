//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUI.h
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
#ifndef __ZE_3DS_MAX_UI_H__
#define __ZE_3DS_MAX_UI_H__

#include <Max.h>
#include <MaxIcon.h>
#include <bmmlib.h>
#include <guplib.h>

#define ZECommonUtilsHelperAttributesAdd_Action_ID Class_ID(0x346704d6, 0x5f240495)
#define ZECommonUtilsRemoveAttributes_Action_ID Class_ID(0x1073561b, 0x4e375559)
#define ZE3dsMax_Action_Table_ID Class_ID(0x388e24cf, 0x4e944a58)
#define ZE3dsMax_Menu_Context_ID Class_ID(0x6a122cbe, 0xac55d7d)

extern HINSTANCE hInstance;

class ZE3dsMaxCommonUtilsActionAddHelperAttributes : public ActionItem
{
private:

	MaxBmpFileIcon*		HelperActionIcon;

public:
	int					GetId();
	BOOL				ExecuteAction();

	void				GetButtonText(MSTR& buttonText);
	void				GetMenuText(MSTR& menuText);
	void				GetDescriptionText(MSTR& descText);
	void				GetCategoryText(MSTR& catText);

	BOOL				IsChecked();
	BOOL				IsItemVisible();
	BOOL				IsEnabled();

	MaxIcon*			GetIcon();

	void				DeleteThis();

	ZE3dsMaxCommonUtilsActionAddHelperAttributes();
};

class ZE3dsMaxCommonUtilsRemoveAttributes : public ActionItem
{
private:

	MaxBmpFileIcon*		RemoveActionIcon;

public:
	int					GetId();
	BOOL				ExecuteAction();

	void				GetButtonText(MSTR& buttonText);
	void				GetMenuText(MSTR& menuText);
	void				GetDescriptionText(MSTR& descText);
	void				GetCategoryText(MSTR& catText);

	BOOL				IsChecked();
	BOOL				IsItemVisible();
	BOOL				IsEnabled();

	MaxIcon*			GetIcon();

	void				DeleteThis();

						ZE3dsMaxCommonUtilsRemoveAttributes();
};

class ZE3dsMaxActionCallback : public ActionCallback
{
	public:
		ActionTable*	ZinekActionTable;

		BOOL			ExecuteAction(int id);

						ZE3dsMaxActionCallback(ActionTable* Table);

};

class ZE3dsMaxUI : public GUP 
{
	public:
		virtual DWORD		Start(); 
		virtual void		Stop();

		virtual void		DeleteThis();

							ZE3dsMaxUI();
		virtual				~ZE3dsMaxUI();
};

#endif
