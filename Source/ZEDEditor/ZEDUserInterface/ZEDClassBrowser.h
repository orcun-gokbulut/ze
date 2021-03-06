//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDClassBrowser.h
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

#include "ZEDWindow.h"
#include "ZEDS\ZEArray.h"

class ZEDObjectWrapper;
class ZEDClassModel;
class ZEDSelectionManager;
class QItemSelection;
class Ui_ZEDClassBrowser;

class ZEDClassBrowser : public ZEDWindow
{
	Q_OBJECT
	private:
		QWidget*							Widget;
		Ui_ZEDClassBrowser*					Form;	
		ZEDClassModel*						Model;
		ZEDObjectWrapper*					DestinationWrapper;

		virtual bool						InitializeInternal() override;
		virtual bool						DeinitializeInternal() override;

		virtual void						SelectionManager_OnSelectionChanged(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Selection);

		void								Update();

	private slots:
		void								txtSearch_textChanged(const QString& Text);
		
		void								trwClasses_itemSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		void								btnMode_toggled(bool Checked);
		void								btnAdd_clicked();

	public:
		void								SetClassModel(ZEDClassModel* Model);
		ZEDClassModel*						GetClassModel() const;

											ZEDClassBrowser();
		virtual								~ZEDClassBrowser();
};
