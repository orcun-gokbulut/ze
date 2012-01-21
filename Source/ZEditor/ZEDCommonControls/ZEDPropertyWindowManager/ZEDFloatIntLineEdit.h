//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDFloatIntLineEdit.h
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

#ifndef __ZED_FLOAT_INT_EDIT_H__
#define __ZED_FLOAT_INT_EDIT_H__

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>

enum FloatIntLineEditMode
{
	IntegerMode,
	FloatMode,
	StringMode,
	FileMode,
};

class ZEDFloatIntLineEditPreviewer : public QWidget
{
	public:	

		QLabel*			ImageLabel;
		QLabel*			InformationLabel;
		QVBoxLayout*	Layout;
		QPixmap			Pixmap;

		void			SetImage(QString FileName);
						ZEDFloatIntLineEditPreviewer(QWidget* Parent = 0);
						~ZEDFloatIntLineEditPreviewer();
};

class ZEDFloatIntLineEdit : public QLineEdit
{
	Q_OBJECT

	private:

		FloatIntLineEditMode							Mode;
		ZEDFloatIntLineEditPreviewer*					PreviewFrameLabel;

		bool											Entered;

		ZEInt												IntegerStepSize;
		float											FloatStepSize;

		ZEInt												ParsedInt;
		float											ParsedFloat;

		ZEInt												LastValidIntVlaue;
		float											LastValidFloatValue;

		bool											ParseInteger();
		bool											ParseFloat();

		bool											MidButtonPressed;
		ZEInt												OldMousePositionY;
	
	protected:
	
		void											wheelEvent(QWheelEvent* Event);
		void											focusInEvent(QFocusEvent* Event);
		void											focusOutEvent(QFocusEvent* Event);
		void											mouseMoveEvent(QMouseEvent* Event);
		void											mousePressEvent(QMouseEvent* Event);
		void											mouseReleaseEvent(QMouseEvent* Event);
		void											keyPressEvent(QKeyEvent* Event);
		void											keyReleaseEvent(QKeyEvent* Event);
		void											enterEvent(QEvent* Event);
		void											leaveEvent(QEvent* Event);

	public:

		bool											IsTextChanged;

		void											SetIntegerStepSize(ZEInt StepSize);
		ZEInt												GetIntegerStepSize();

		void											SetFloatStepSize(float StepSize);
		float											GetFloatStepSize();

		void											SetInteger(ZEInt Value);
		ZEInt												GetInteger(bool* Ok = NULL);

		void											SetFloat(float Value);
		float											GetFloat(bool* Ok = NULL);

		void											SetMode(FloatIntLineEditMode Mode);
		FloatIntLineEditMode							GetMode();

														ZEDFloatIntLineEdit(FloatIntLineEditMode Mode = FloatMode, QWidget* Parent = 0);
														~ZEDFloatIntLineEdit();
};

#endif
