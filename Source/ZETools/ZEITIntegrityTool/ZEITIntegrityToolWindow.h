//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITIntegrityToolWindow.h
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

#include "ZEITIntegrity/ZEITGenerator.h"

#include <QDialog>

class Ui_ZEITIntegrityToolWindow;
class QListWidgetItem;

class ZEITIntegrityToolWindow : public QDialog
{
	Q_OBJECT
	friend class ZELNContactModule;
	private:
		Ui_ZEITIntegrityToolWindow*		Form;

		ZEITGenerator					Generator;

		void							UpdateRecord(ZESize Index);
		void							UpdateLists();
		void							UpdateUI();

	private slots:
		void							lstIncludes_itemSelectionChanged();
		void							lstIncludes_itemChanged(QListWidgetItem* Item);
		void							btnIncludeAdd_clicked();
		void							btnIncludeRemove_clicked();

		void							lstExcludes_itemSelectionChanged();
		void							lstExcludes_itemChanged(QListWidgetItem* Item);
		void							btnExcludeAdd_clicked();
		void							btnExcludeRemove_clicked();

		void							btnGeneratorLoad_clicked();
		void							btnGeneratorSave_clicked();
		void							btnScan_clicked();

		void							tblRecords_itemSelectionChanged();
		void							btnRecordAdd_clicked();
		void							btnRecordRemove_clicked();
		void							btnRecordExclude_clicked();
		void							btnRecordUpdate_clicked();
		void							btnRecordUpdateAll_clicked();

		void							btnIntegritySave_clicked();

	public:
										ZEITIntegrityToolWindow();
										~ZEITIntegrityToolWindow();
};
