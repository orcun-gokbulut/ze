//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRPageGenerating.cpp
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

#include "ZECRPageGenerating.h"

#include "ZECRWindow.h"
#include "ZECRReport.h"
#include "Ui_ZECRPageGenerating.h"

#include <QTimer>

void ZECRPageGenerating::GeneratorThread_Function(ZEThread* Thread, void* Parameters)
{
	GetWindow()->GetReport()->Generate(&GetWindow()->GetParameters());
	GetWindow()->TerminateApplication();
	Generated = true;
}

void ZECRPageGenerating::Activated()
{
	Generated = false;
	Timer->start();
	GeneratorThread.Run();
}

void ZECRPageGenerating::Timer_timeout()
{
	if (!Generated)
		return;

	Timer->stop();
	GetWindow()->TerminateApplication();
	GetWindow()->SetPage(ZECR_WP_SEND_INFORMATION);
}

void ZECRPageGenerating::btnCancel_clicked()
{
	exit(EXIT_FAILURE);
}

ZECRPageGenerating::ZECRPageGenerating(QWidget* Parent) : ZECRPage(Parent)
{
	Form = new Ui_ZECRPageGenerating();
	Form->setupUi(this);

	GeneratorThread.SetFunction(ZEThreadFunction::Create<ZECRPageGenerating, &ZECRPageGenerating::GeneratorThread_Function>(this));
	
	Generated = false;
	Timer = new QTimer(this);
	Timer->setInterval(1000);
	Timer->setSingleShot(false);
	Timer->stop();
	connect(Timer, SIGNAL(timeout()), this, SLOT(Timer_timeout()));
}

ZECRPageGenerating::~ZECRPageGenerating()
{
	delete Form;
}
