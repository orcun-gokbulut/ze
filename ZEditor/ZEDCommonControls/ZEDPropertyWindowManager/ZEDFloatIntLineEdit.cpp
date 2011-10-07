//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDFloatIntLineEdit.cpp
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

#include <QString>
#include <QFocusEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include "ZEDFloatIntLineEdit.h"
#include "ZEDCommonControls/CSS.h"


ZEDFloatIntLineEditPreviewer::ZEDFloatIntLineEditPreviewer(QWidget* Parent) : QWidget(Parent)
{
	setWindowFlags(Qt::ToolTip);
	this->ImageLabel = new QLabel();
	ImageLabel->setWindowFlags(Qt::ToolTip);
	this->InformationLabel = new QLabel();
	InformationLabel->setWindowFlags(Qt::ToolTip);
	this->Layout = new QVBoxLayout(this);
	this->Layout->setContentsMargins(2,2,2,2);
	this->setLayout(Layout);
	this->layout()->addWidget(ImageLabel);
	this->layout()->addWidget(InformationLabel);

	QPalette Palette = palette();
	Palette.setColor(QPalette::Base, QColor(50, 50, 50));
	setPalette(Palette);

	Palette = ImageLabel->palette();
	Palette.setColor(QPalette::Base, QColor(50, 50, 50));
	ImageLabel->setPalette(Palette);

	Palette = InformationLabel->palette();
	Palette.setColor(QPalette::Base, QColor(50, 50, 50));
	InformationLabel->setPalette(Palette);
}

ZEDFloatIntLineEditPreviewer::~ZEDFloatIntLineEditPreviewer()
{
	delete ImageLabel;
	delete InformationLabel;
	delete Layout; 
}

void ZEDFloatIntLineEditPreviewer::SetImage(QString FileName)
{
	if(Pixmap.load(FileName))
	{	
		InformationLabel->setText(QString().setNum(Pixmap.width()) + "x" + QString().setNum(Pixmap.height()));
		
		if(Pixmap.width() > 256)
			Pixmap = Pixmap.scaledToWidth(256);
		
		ImageLabel->setPixmap(Pixmap);
		ImageLabel->show();
		InformationLabel->show();
		ImageLabel->repaint();
		InformationLabel->repaint();

		QRect ScreenGeometry = QApplication::desktop()->screenGeometry(this);
		QPoint Point = QCursor::pos();
		QPoint LabelTopLeft;
		QPoint LabelBottomRight;

		if(ScreenGeometry.bottomRight().y() < (Point.y() + 256) && ScreenGeometry.bottomRight().x() < (Point.x() + 256))
		{
			LabelTopLeft.setY(Point.y() - 256);
			LabelTopLeft.setX(Point.x() - 256);
			LabelBottomRight.setX(Point.x());
			LabelBottomRight.setY(Point.y());
		}

		if(ScreenGeometry.bottomRight().y() < (Point.y() + 256) && ScreenGeometry.bottomRight().x() > (Point.x() + 256))
		{
			LabelTopLeft.setY(Point.y() - 256);
			LabelTopLeft.setX(Point.x());
			LabelBottomRight.setX(Point.x() + 256);
			LabelBottomRight.setY(Point.y());
		}

		if(ScreenGeometry.bottomRight().y() > (Point.y() + 256) && ScreenGeometry.bottomRight().x() < (Point.x() + 256))
		{
			LabelTopLeft.setY(Point.y());
			LabelTopLeft.setX(Point.x() - 256);
			LabelBottomRight.setX(Point.x());
			LabelBottomRight.setY(Point.y() + 256);
		}

		if(ScreenGeometry.bottomRight().y() > (Point.y() + 256) && ScreenGeometry.bottomRight().x() > (Point.x() + 256))
		{
			LabelTopLeft.setY(Point.y());
			LabelTopLeft.setX(Point.x());
			LabelBottomRight.setX(Point.x() + 256);
			LabelBottomRight.setY(Point.y() + 256);
		}

		setGeometry(QRect(LabelTopLeft, LabelBottomRight));
		show();
	}
}

bool ZEDFloatIntLineEdit::ParseInteger()
{
	bool* Ok = new bool;
	int ConvertedValue = 0;

	ConvertedValue = text().toInt(Ok);

	if(*Ok == true)
	{
		ParsedInt = ConvertedValue;
		LastValidIntVlaue = ConvertedValue;
		return true;
	}

	else
		return false;
}

bool ZEDFloatIntLineEdit::ParseFloat()
{
	bool* Ok = new bool;
	float ConvertedValue = 0;

	ConvertedValue = text().toFloat(Ok);

	if(*Ok == true)
	{
		ParsedFloat = ConvertedValue;
		LastValidFloatValue = ConvertedValue;
		return true;
	}

	else
		return false;
}

void ZEDFloatIntLineEdit::wheelEvent(QWheelEvent* Event)
{
	QLineEdit::wheelEvent(Event);
}

void ZEDFloatIntLineEdit::focusInEvent(QFocusEvent* Event)
{
	QLineEdit::focusInEvent(Event);
	PreviewFrameLabel->hide();
	QPalette Palette;
	Palette = palette();
	Palette.setColor(backgroundRole(), QColor(255, 255, 255));
	setPalette(Palette);
	setFrame(true);
	setAutoFillBackground(false);
	IsTextChanged = false;
}

void ZEDFloatIntLineEdit::focusOutEvent(QFocusEvent* Event)
{
	QLineEdit::focusOutEvent(Event);
	QPalette Palette;
	Palette = palette();
	Palette.setColor(backgroundRole(), Qt::transparent);
	setPalette(Palette);
	setFrame(false);
	setAutoFillBackground(false);

	if(IsTextChanged == true)
	{
		QObject::blockSignals(false);
		emit returnPressed();
		IsTextChanged = false;
		QObject::blockSignals(true);
	}
}

void ZEDFloatIntLineEdit::mouseMoveEvent(QMouseEvent* Event)
{
	QLineEdit::mouseMoveEvent(Event);

	if(Mode == StringMode || Mode == FileMode)
		return;

	if(MidButtonPressed == false)
		return;

	if(Mode == IntegerMode)
	{
		int Amount = Event->globalPos().y() - OldMousePositionY;
		LastValidIntVlaue = LastValidIntVlaue + -(Amount * IntegerStepSize);
		setText(QString().setNum(LastValidIntVlaue));
		IsTextChanged = true;
	}

	if(Mode == FloatMode)
	{
		int Amount = Event->globalPos().y() - OldMousePositionY;
		LastValidFloatValue = LastValidFloatValue + -(Amount * FloatStepSize);
		setText(QString().setNum(LastValidFloatValue));
		IsTextChanged = true;
	}

	OldMousePositionY = Event->globalPos().y();
	
}

void ZEDFloatIntLineEdit::keyPressEvent(QKeyEvent* Event)
{
	QLineEdit::keyPressEvent(Event);

	if(((Event->key() & Qt::Key_Return) != Qt::Key_Return) && ((Event->key() & Qt::Key_Enter) != Qt::Key_Enter)) 
		//&& ((Event->key() & Qt::Key_Space)		!= Qt::Key_Space) 
		//&& ((Event->key() & Qt::Key_Up)			!= Qt::Key_Up)
		//&& ((Event->key() & Qt::Key_Down)			!= Qt::Key_Down)
		//&& ((Event->key() & Qt::Key_Left)			!= Qt::Key_Left)
		//&& ((Event->key() & Qt::Key_Right)		!= Qt::Key_Right)
		//&& ((Event->key() & Qt::Key_Shift)		!= Qt::Key_Shift)
		//&& ((Event->key() & Qt::Key_Control)		!= Qt::Key_Control)
		//&& ((Event->key() & Qt::Key_Alt)			!= Qt::Key_Alt)
		//&& ((Event->key() & Qt::Key_AltGr)		!= Qt::Key_AltGr)
		//&& ((Event->key() & Qt::Key_CapsLock)		!= Qt::Key_CapsLock)
		//&& ((Event->key() & Qt::Key_PageDown)		!= Qt::Key_PageDown)
		//&& ((Event->key() & Qt::Key_PageUp)		!= Qt::Key_PageUp)
		//&& ((Event->key() & Qt::Key_NumLock)		!= Qt::Key_NumLock))
		IsTextChanged = true;

	if(Event->key() == Qt::Key_Return || Event->key() == Qt::Key_Enter)
		clearFocus();
}

void ZEDFloatIntLineEdit::keyReleaseEvent(QKeyEvent* Event)
{
	QLineEdit::keyReleaseEvent(Event);
}

void ZEDFloatIntLineEdit::mousePressEvent(QMouseEvent* Event)
{
	QLineEdit::mousePressEvent(Event);
	if(Event->button() == Qt::MidButton)
	{
		OldMousePositionY = Event->globalPos().y();
		MidButtonPressed = true;
	}
}

void ZEDFloatIntLineEdit::mouseReleaseEvent(QMouseEvent* Event)
{
	QLineEdit::mouseReleaseEvent(Event);

	if(Event->button() == Qt::MidButton)
		MidButtonPressed = false;
}

void ZEDFloatIntLineEdit::enterEvent(QEvent* Event)
{
	QLineEdit::enterEvent(Event);
	
	if(hasFocus() == true)
		return;

	if(Mode == FileMode && Entered == false)
	{
		PreviewFrameLabel->SetImage(QString("resources//") + text());
		Entered = true;
	}
}

void ZEDFloatIntLineEdit::leaveEvent(QEvent* Event)
{
	Entered = false;

	if(Mode == FileMode)
	{
		PreviewFrameLabel->close();	
	}

	QLineEdit::leaveEvent(Event);
}

void ZEDFloatIntLineEdit::SetIntegerStepSize(int StepSize)
{
	this->IntegerStepSize = StepSize;
}

int ZEDFloatIntLineEdit::GetIntegerStepSize()
{
	return IntegerStepSize;
}

void ZEDFloatIntLineEdit::SetFloatStepSize(float StepSize)
{
	this->FloatStepSize = StepSize;
}

float ZEDFloatIntLineEdit::GetFloatStepSize()
{
	return FloatStepSize;
}

void ZEDFloatIntLineEdit::SetInteger(int Value)
{
	setText(QString().setNum(Value));
}

int ZEDFloatIntLineEdit::GetInteger(bool* Ok)
{
	if(ParseInteger() == true)
	{
		if(Ok != NULL)
			*Ok = true;

		return LastValidIntVlaue;
	}
	
	else
	{
		QMessageBox Message;
		Message.setIcon(QMessageBox::Critical);
		Message.setText(QString("Enter a valid integer value."));
		Message.exec();
		setText(QString().setNum(LastValidIntVlaue));
		
		if(Ok != NULL)
			*Ok = false;

		setFocus(Qt::OtherFocusReason);
		return 0;
	}
}

void ZEDFloatIntLineEdit::SetFloat(float Value)
{
	setText(QString().setNum(Value));
}

float ZEDFloatIntLineEdit::GetFloat(bool* Ok)
{
	if(ParseFloat() == true)
	{
		if(Ok != NULL)
			*Ok = true;

		return LastValidFloatValue;
	}

	else
	{
		QMessageBox Message;
		Message.setIcon(QMessageBox::Critical);
		Message.setText(QString("Enter a valid float value."));
		Message.exec();
		setText(QString().setNum(LastValidFloatValue));

		if(Ok != NULL)
			*Ok = false;

		setFocus(Qt::OtherFocusReason);
		return 0;
	}
}

void ZEDFloatIntLineEdit::SetMode(FloatIntLineEditMode Mode)
{
	this->Mode = Mode;
}

FloatIntLineEditMode ZEDFloatIntLineEdit::GetMode()
{
	return Mode;
}

ZEDFloatIntLineEdit::ZEDFloatIntLineEdit(FloatIntLineEditMode Mode, QWidget* Parent) : QLineEdit(Parent)
{
	this->Mode = Mode;
	this->IntegerStepSize = 1;
	this->FloatStepSize = 0.1f;
	this->LastValidIntVlaue = 0;
	this->LastValidFloatValue = 0;

	MidButtonPressed = false;
	PreviewFrameLabel = new ZEDFloatIntLineEditPreviewer();
	PreviewFrameLabel->hide();
	QPalette Palette;
	Palette = palette();
	Palette.setColor(backgroundRole(), Qt::transparent);
	setPalette(Palette);
	setFrame(false);
	setAutoFillBackground(false);
	Entered = false;
	IsTextChanged = false;
	setFixedHeight(20);
	QObject::blockSignals(true);	
	//this->setStyleSheet(GetCSSFromFile(":/CSS/PropertyWindowLineEdit.qss"));

	ParsedInt = 0;
	ParsedFloat = 0.0f;
	OldMousePositionY = 0;
}

ZEDFloatIntLineEdit::~ZEDFloatIntLineEdit()
{
	delete PreviewFrameLabel;
}
