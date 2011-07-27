//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDBrowserItem.cpp
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

#include "ZEDBrowserItem.h"
#include "ZEDCore/ZEDPreviewWidget.h"
#include "ZEDCore/ZEDFileExtension.h"
#include "ZEDCore/ZEDPlugInManager.h"
#include <QMouseEvent>
#include <QFileInfo>

void ZEDBrowserItem::Clicked(QMouseEvent* Event)
{
	if(Event->button() == LeftButton)
		SetSelected(true);
}

void ZEDBrowserItem::DoubleClicked(QMouseEvent* Event)
{
	if(Event->button() == LeftButton)
	{
		ZEDFileExtension* Extension = GetRelatedExtension();

		if(Extension != NULL)
			Extension->GetEditor(FileName);
	}
}

void ZEDBrowserItem::ContextMenuRequested(QContextMenuEvent *Event)
{
	//Get Additional Menu Actions From PlugIn
}

void ZEDBrowserItem::mousePressEvent(QMouseEvent* Event)
{
	Clicked(Event);
}

void ZEDBrowserItem::mouseDoubleClickEvent(QMouseEvent* Event)
{
	if(Event->button() == LeftButton)
		DoubleClicked(Event);
}

void ZEDBrowserItem::contextMenuEvent(QContextMenuEvent *Event)
{
	ContextMenuRequested(Event);
}

void ZEDBrowserItem::SetSelected(bool IsSelected)
{
	this->IsSelected = IsSelected;

	if(IsSelected)
	{
		setFrameStyle(6);
		setFrameShadow(QFrame::Plain);
	}
	else
	{
		setFrameStyle(3);
		setFrameShadow(QFrame::Plain);
	}
}

bool ZEDBrowserItem::GetSelected() const
{
	return IsSelected;
}

ZEDBrowserItem::ZEDBrowserItem(QString FileName)
{
	if(!QFile::exists(FileName))
		return;

	PreviewWidget = NULL;
	IsSelected = false;

	setMinimumSize(256, 296);
	setMaximumSize(256, 296);

	setFrameStyle(3);
	setFrameShadow(QFrame::Plain);

	Layout = new QVBoxLayout();
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Header = new QLabel(this);
	Footer = new QLabel(this);

	this->FileName = FileName;
	QFileInfo FileInfo(FileName);
	FileType = FileInfo.suffix().toUpper();
	Header->setText(FileInfo.fileName());
	Footer->setText(QString::number(FileInfo.size()));

	setLayout(Layout);

	Layout->addWidget(Header);

	ZEDFileExtension* Extension = GetRelatedExtension();

	if(Extension != NULL)
		Extension->GetPreviewWidget(FileName);

	if(PreviewWidget != NULL)
		Layout->addWidget(PreviewWidget);

	Layout->addWidget(Footer);
}

ZEDBrowserItem::~ZEDBrowserItem()
{

}

ZEDFileExtension* ZEDBrowserItem::GetRelatedExtension()
{
	ZEDExtension* CurrentExtension;

	for (int I = 0; I < zedPlugInManager->GetExtensions().count(); I++)
	{
		CurrentExtension = zedPlugInManager->GetExtensions().at(I);

		if(CurrentExtension->GetDescription()->GetType() == ZED_FILE_EXTENSION)
		{
			int SupportedFileFormatCount = ((ZEDFileExtension*)(CurrentExtension))->GetSupportedFileFormats().count();

			for (int J = 0; J < SupportedFileFormatCount; J++)
			{
				if(((ZEDFileExtension*)(CurrentExtension))->GetSupportedFileFormats().at(I) == FileType)
					return ((ZEDFileExtension*)(CurrentExtension));
			}
		}
	}

	return NULL;
}
