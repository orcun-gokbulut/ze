//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLEditorFormatSelector.cpp
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

#include "ZEMLEditorFormatSelector.h"
#include "ui_ZEMLEditorFormatSelector.h"
#include "ZEML\ZEMLFormat.h"

void ZEMLEditorFormatSelector::cmbFormats_onCurrentIndexChanged(int Index)
{
	Format = (ZEMLFormatDescription*)Form->cmbFormats->itemData(Index, Qt::UserRole).toULongLong();
}

void ZEMLEditorFormatSelector::SetFormat(ZEMLFormatDescription* Description)
{
	Format = Description;
	for (int I = 0; I < Form->cmbFormats->count(); I++)
	{
		ZEMLFormatDescription* Current = (ZEMLFormatDescription*)Form->cmbFormats->itemData(I, Qt::UserRole).toULongLong();
		if (Current == Description)
		{
			Form->cmbFormats->blockSignals(true);
			Form->cmbFormats->setCurrentIndex(I);
			Form->cmbFormats->blockSignals(false);
			break;
		}
	}
}

ZEMLFormatDescription* ZEMLEditorFormatSelector::GetFormat()
{
	return Format;
}

ZEMLEditorFormatSelector::ZEMLEditorFormatSelector(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZEMLEditorFormatSelector();
	Form->setupUi(this);

	ZEMLFormatDescription*const* Formats = ZEMLFormat::GetFormats();
	for (ZESize I = 0; I < ZEMLFormat::GetFormatCount(); I++)
	{
		if (Formats[I]->GetSupport() & ZEML_FS_WRITE)
			Form->cmbFormats->addItem(Formats[I]->GetName(), QVariant((ZEUInt64)Formats[I]));
	}

	Form->cmbFormats->setCurrentIndex(0);

	connect(Form->cmbFormats, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbFormats_onCurrentIndexChanged(int)));
}

ZEMLEditorFormatSelector::~ZEMLEditorFormatSelector()
{

}
