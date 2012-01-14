//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntitySelector.h
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
#ifndef __ZED_ENTITY_SELECTOR_H__
#define __ZED_ENTITY_SELECTOR_H__

#include <QDialog.h>
#include <QWidget.h>
#include <QVector.h>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QToolBar>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "ZEGame\ZEEntity.h"
#include <ZEDS/ZEArray.h>
#include <ZESDK.h>

class ZEDEntitySelector;

class ZEDEntitySelectorItem : public QTreeWidgetItem
{
	public:
		
		ZEEntity* Entity;	
};

class ZEDEntitySelectorButton : public QPushButton
{
	public:

		ZEDEntitySelector*	ParentSelector;
		bool				Role;

	protected:

		virtual void mouseReleaseEvent(QMouseEvent* Event);
};

class ZEDEntitySelector : public QDialog
{
	Q_OBJECT

	private:

		QMenuBar*							MenuBar;
		QMenu*								Menu;
		QTreeWidget*						ListTreeWidget;
		ZEDEntitySelectorButton*			SelectButton;
		ZEDEntitySelectorButton*			CancelButton;
		QVBoxLayout*						MainLayout;
		QHBoxLayout*						ButtonLayout;
		QHBoxLayout*						SearchFilterLayout;
		QSpacerItem*						Spacer;
		QStringList							Headers;
		QLabel*								FilterLabel;
		QLabel*								SearchLabel;
		QLineEdit*							SearchLineEdit;
		QComboBox*							FilterComboBox;
		ZEArray<ZEEntity*>*					SelectedEntities;
		void								GenerateList();

	
	public:


		void								GetSelectedEntities(ZEArray<ZEEntity*>* SelectedEntities);
		void								FillSelectedEntities();
											ZEDEntitySelector();

	public slots:
		

		void								UpdateListByFilter();
		void								UpdateListBySearch();
};

#endif
