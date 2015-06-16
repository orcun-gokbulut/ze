//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelActions.h
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
#ifndef __ZE_3DS_MAX_MODEL_ACTIONS_H__
#define __ZE_3DS_MAX_MODEL_ACTIONS_H__

#define ZEModelMeshAttributesAdd_Action_ID Class_ID(0x4b2d6b30, 0x5eb35637)
#define ZEModelBoneAttributesAdd_Action_ID Class_ID(0x779e5248, 0xee814e0)
#define ZEModelPhysicalBodyAttributesAdd_Action_ID Class_ID(0x319f1073, 0x14c22af)
#define ZEModelBoundingBoxAttributesAdd_Action_ID Class_ID(0x696e1909, 0x113208f5)


#include <Max.h>
#include <ActionTable.h>
#include <MaxIcon.h>

class ZE3dsMaxModelActionAddMeshAttributes : public ActionItem
{
	private:

		MaxBmpFileIcon*		MeshActionIcon;

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

							ZE3dsMaxModelActionAddMeshAttributes();
};

class ZE3dsMaxModelActionAddBoneAttributes : public ActionItem
{
	private:

		MaxBmpFileIcon*		BoneActionIcon;

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

							ZE3dsMaxModelActionAddBoneAttributes();
};

class ZE3dsMaxModelActionAddPhysicalBodyAttributes : public ActionItem
{
	private:

		MaxBmpFileIcon*		PhysicalBodyActionIcon;

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

							ZE3dsMaxModelActionAddPhysicalBodyAttributes();
};

class ZE3dsMaxModelActionAddBoundingBoxAttributes : public ActionItem
{
private:

	MaxBmpFileIcon*		BoundingBoxActionIcon;

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

						ZE3dsMaxModelActionAddBoundingBoxAttributes();
};

#endif
