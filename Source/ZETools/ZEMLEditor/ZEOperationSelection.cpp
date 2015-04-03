//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOperationSelection.cpp
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

#include "ZEOperationSelection.h"
#include "ZEML\ZEMLElement.h"
#include "ZEMLEditorWindow.h"

#include <QtGui\QTreeWidgetItem>

bool ZEOperationSelection::Apply()
{
	if (Items.GetCount() == 0)
		return true;

	Items[0]->treeWidget()->blockSignals(true);
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->setSelected(true);
	Items[0]->treeWidget()->blockSignals(false);
	return true;
}

bool ZEOperationSelection::Revert()
{
	if (Items.GetCount() == 0)
		return true;

	Items[0]->treeWidget()->blockSignals(true);
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->setSelected(false);
	Items[0]->treeWidget()->blockSignals(false);
	return true;
}

ZEOperationSelection::ZEOperationSelection(const QList<QTreeWidgetItem*>& Items)
{
	this->Items.SetCount(Items.count());
	for (ZESize I = 0; I < Items.count(); I++)
		this->Items[I] = Items[I];
}
