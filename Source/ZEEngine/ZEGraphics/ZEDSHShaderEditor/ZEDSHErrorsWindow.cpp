//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHErrorsWindow.cpp
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

#include "ZEDSHErrorsWindow.h"

#include <QtGui/QTableWidget>

QTableWidget* ZEDSHErrorsWindow::GetErrorTable()
{
	return ErrorTable;
}

void ZEDSHErrorsWindow::ParseErrors(const char* Errors)
{

}

ZEDSHErrorsWindow::ZEDSHErrorsWindow(QWidget* Parent) : QDockWidget(Parent)
{
	QTableWidget* ErrorTable = new QTableWidget(this);
	
	ErrorTable->setColumnCount(6);
	QTableWidgetItem* Column0 = new QTableWidgetItem("Type");
	ErrorTable->setHorizontalHeaderItem(0, Column0);

	QTableWidgetItem* Column1 = new QTableWidgetItem("Index");
	ErrorTable->setHorizontalHeaderItem(1, Column1);
	
	QTableWidgetItem* Column2 = new QTableWidgetItem("Description");
	ErrorTable->setHorizontalHeaderItem(2, Column2);
	
	QTableWidgetItem* Column3 = new QTableWidgetItem("File");
	ErrorTable->setHorizontalHeaderItem(3, Column3);
	
	QTableWidgetItem* Column4 = new QTableWidgetItem("Line");
	ErrorTable->setHorizontalHeaderItem(4, Column4);
	
	QTableWidgetItem* Column5 = new QTableWidgetItem("Column");
	ErrorTable->setHorizontalHeaderItem(5, Column5);

	setWidget(ErrorTable);
}
