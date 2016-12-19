//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetBrowser.cpp
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

#include "ZEDAssetBrowser.h"

#include "ZEDS/ZEFormat.h"
#include "ZEDAssetModel.h"
#include "ZEDCore/ZEDEditor.h"
#include "ui_ZEDAssetBrowser.h"

bool ZEDAssetBrowser::InitializeInternal()
{
	if (!ZEDWindow::InitializeInternal())
		return false;

	GetEditor()->AddComponent(Model);

	return true;
}

bool ZEDAssetBrowser::DeinitializeInternal()
{
	GetEditor()->RemoveComponent(Model);

	return ZEDWindow::DeinitializeInternal();
}

void ZEDAssetBrowser::txtSearch_textChanged(const QString& Text)
{
	Model->SetSearchPattern(ZEFormat::Format("*{0}*", Text.toUtf8().begin()));
}

void ZEDAssetBrowser::cmbCategories_currentIndexChanged(const QString& text)
{

}

ZEDAssetBrowser::ZEDAssetBrowser()
{
	SetName("Asset Browser");

	Widget = new QWidget();
	SetWidget(Widget);

	Form = new Ui_ZEDAssetBrowser();
	Form->setupUi(Widget);

	Model = new ZEDAssetModel();
	Model->SetMode(ZED_AMM_TREE);
	Model->SetHierarcy(ZED_AMH_DIRECTORY);
	ZEArray<ZEDAssetModelColumn> ModelColumns;
	ModelColumns.SetCount(4);
	ModelColumns[0].SetType(ZED_AMHT_NAME);
	ModelColumns[0].SetHeaderText("Name");
	ModelColumns[0].SetSelectable(true);
	ModelColumns[1].SetType(ZED_AMHT_TYPE);
	ModelColumns[1].SetHeaderText("Type");
	ModelColumns[1].SetAlignmnent(Qt::AlignHCenter | Qt::AlignVCenter);
	ModelColumns[2].SetType(ZED_AMHT_SIZE);
	ModelColumns[2].SetHeaderText("Size");
	ModelColumns[2].SetAlignmnent(Qt::AlignHCenter | Qt::AlignVCenter);
	ModelColumns[3].SetType(ZED_AMHT_LAST_MODIFICATION_TIME);
	ModelColumns[3].SetHeaderText("Modified");
	ModelColumns[3].SetAlignmnent(Qt::AlignHCenter | Qt::AlignVCenter);
	Model->SetColumns(ModelColumns);

	Form->trwAssets->setModel(Model);
	Form->trwAssets->setDragEnabled(true);
	Form->trwAssets->header()->setStretchLastSection(false);
	Form->trwAssets->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	Form->trwAssets->setSelectionBehavior(QAbstractItemView::SelectItems);
	Form->trwAssets->setSelectionMode(QAbstractItemView::ExtendedSelection);
	Form->trwAssets->setAlternatingRowColors(true);
	Form->trwAssets->setDragEnabled(true),
	

	connect(Form->txtSearch, SIGNAL(textChanged(const QString&)), this, SLOT(txtSearch_textChanged(const QString&)));
	connect(Form->cmbCategories, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbCategories_currentIndexChanged(const QString&)));
}

ZEDAssetBrowser::~ZEDAssetBrowser()
{
	delete Form;
	delete Widget;
}
