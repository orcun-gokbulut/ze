//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRWindowUserFeedback.cpp
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

#include "ZECRWindowUserFeedback.h"

#include "ui_ZECRWindowUserFeedback.h"

#include "ZECRWindow.h"
#include "ZECRWindowViewReport.h"
#include "ZECRWindowViewPrivacyPolicy.h"
#include "ZECRCrashReport.h"
#include "ZECRProviderUserFeedback.h"

#include <QDesktopServices>
#include <QUrl>

void ZECRWindowUserFeedback::btnSend_Clicked()
{
	ZECRProviderUserFeedback* Provider = static_cast<ZECRProviderUserFeedback*>(GetWindow()->GetCrashReport()->GetProvider("UserFeedback"));
	
	if (Provider != NULL)
	{
		Provider->SetNameSurname(Form->txtNameSurname->text().toLocal8Bit().begin());
		Provider->SetEMail(Form->txtEmail->text().toLocal8Bit().begin());
		Provider->SetComment(Form->txtFeedback->toPlainText().toLocal8Bit().begin());
	}

	GetWindow()->SetPage(ZECR_WP_TRANSFERING);
}

void ZECRWindowUserFeedback::btnPrev_Clicked()
{
	GetWindow()->SetPage(ZECR_WP_INFORMATION);
}

ZECRWindowUserFeedback::ZECRWindowUserFeedback(QWidget* Parent) : ZECRWindowPage(Parent)
{
	Form = new Ui_ZECRWindowUserFeedback();
	Form->setupUi(this);

	connect(Form->btnSend, SIGNAL(clicked()), this, SLOT(btnSend_Clicked()));
	connect(Form->btnPrev, SIGNAL(clicked()), this, SLOT(btnPrev_Clicked()));
}

ZECRWindowUserFeedback::~ZECRWindowUserFeedback()
{

}
