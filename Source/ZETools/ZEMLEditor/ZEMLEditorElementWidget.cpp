//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLEditorElementWidget.cpp
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

#include "ZEMLEditorElementWidget.h"
#include "ui_ZEMLEditorElementWidget.h"

#include "ZEMLEditorWindow.h"
#include "ZEML\ZEMLElement.h"
#include "ZEML\ZEMLNode.h"
#include "ZEML\ZEMLProperty.h"
#include "ZEML\ZEMLData.h"
#include "ZEFile\ZEFile.h"

#include <QFileDialog>

int ZEMLEditorElementWidget::GetTypeIndex(ZEValueType ValueType)
{
	switch (ValueType)
	{
		default:
			return -1;
		case ZE_VRT_INTEGER_8:
			return 0;
		case ZE_VRT_INTEGER_16:
			return 1;
		case ZE_VRT_INTEGER_32:
			return 2;
		case ZE_VRT_INTEGER_64:
			return 3;
		case ZE_VRT_UNSIGNED_INTEGER_8:
			return 4;
		case ZE_VRT_UNSIGNED_INTEGER_16:
			return 5;
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return 6;
		case ZE_VRT_UNSIGNED_INTEGER_64:
			return 7;
		case ZE_VRT_FLOAT:
			return 8;
		case ZE_VRT_DOUBLE:
			return 9;
		case ZE_VRT_BOOLEAN:
			return 10;
		case ZE_VRT_STRING:
			return 11;
		case ZE_VRT_VECTOR2:
			return 12;
		case ZE_VRT_VECTOR2D:
			return 13;
		case ZE_VRT_VECTOR3:
			return 14;
		case ZE_VRT_VECTOR3D:
			return 15;
		case ZE_VRT_VECTOR4:
			return 16;
		case ZE_VRT_VECTOR4D:
			return 17;
		case ZE_VRT_QUATERNION:
			return 18;
		case ZE_VRT_MATRIX3X3:
			return 19;
		case ZE_VRT_MATRIX3X3D:
			return 20;
		case ZE_VRT_MATRIX4X4:
			return 21;
		case ZE_VRT_MATRIX4X4D:
			return 22;
	}
}

ZEValueType ZEMLEditorElementWidget::GetValueType(int Index)
{
	switch (Index)
	{
		default:
			return ZE_VRT_NULL;
		case 0:
			return ZE_VRT_INTEGER_8;
		case 1:
			return ZE_VRT_INTEGER_16;
		case 2:
			return ZE_VRT_INTEGER_32;
		case 3:
			return ZE_VRT_INTEGER_64;
		case 4:
			return ZE_VRT_UNSIGNED_INTEGER_8;
		case 5:
			return ZE_VRT_UNSIGNED_INTEGER_16;
		case 6:
			return ZE_VRT_UNSIGNED_INTEGER_32;
		case 7:
			return ZE_VRT_UNSIGNED_INTEGER_64;
		case 8:
			return ZE_VRT_FLOAT;
		case 9:
			return ZE_VRT_DOUBLE;
		case 10:
			return ZE_VRT_BOOLEAN;
		case 11:
			return ZE_VRT_STRING;
		case 12:
			return ZE_VRT_VECTOR2;
		case 13:
			return ZE_VRT_VECTOR2D;
		case 14:
			return ZE_VRT_VECTOR3;
		case 15:
			return ZE_VRT_VECTOR3D;
		case 16:
			return ZE_VRT_VECTOR4;
		case 17:
			return ZE_VRT_VECTOR4D;
		case 18:
			return ZE_VRT_QUATERNION;
		case 19:
			return ZE_VRT_MATRIX3X3;
		case 20:
			return ZE_VRT_MATRIX3X3D;
		case 21:
			return ZE_VRT_MATRIX4X4;
		case 22:
			return ZE_VRT_MATRIX4X4D;
	}
}

void ZEMLEditorElementWidget::ConfigureProperty()
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	
	Form->txtValueInt->setVisible(false);
	Form->txtValueString->setVisible(false);
	Form->chkValueBoolean->setVisible(false);
	Form->txtValueFloat11->setVisible(false);
	Form->txtValueFloat12->setVisible(false);
	Form->txtValueFloat13->setVisible(false);
	Form->txtValueFloat14->setVisible(false);
	Form->txtValueFloat21->setVisible(false);
	Form->txtValueFloat22->setVisible(false);
	Form->txtValueFloat23->setVisible(false);
	Form->txtValueFloat24->setVisible(false);
	Form->txtValueFloat31->setVisible(false);
	Form->txtValueFloat32->setVisible(false);
	Form->txtValueFloat33->setVisible(false);
	Form->txtValueFloat34->setVisible(false);
	Form->txtValueFloat41->setVisible(false);
	Form->txtValueFloat42->setVisible(false);
	Form->txtValueFloat43->setVisible(false);
	Form->txtValueFloat44->setVisible(false);

	Form->cmbValueType->blockSignals(true);
	Form->cmbValueType->setCurrentIndex(GetTypeIndex(Property->GetValue().GetType()));
	Form->cmbValueType->blockSignals(false);

	if (Property->GetValueType() == ZEML_VT_INT8)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetInt8()));
	}
	else if (Property->GetValueType() == ZEML_VT_INT16)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetInt16()));
	}
	else if (Property->GetValueType() == ZEML_VT_INT32)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetInt32()));
	}
	else if (Property->GetValueType() == ZEML_VT_INT64)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetInt64()));
	}
	else if (Property->GetValueType() == ZEML_VT_UINT8)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetUInt8()));
	}
	else if (Property->GetValueType() == ZEML_VT_UINT16)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetUInt16()));
	}
	else if (Property->GetValueType() == ZEML_VT_UINT32)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetUInt32()));
	}
	else if (Property->GetValueType() == ZEML_VT_UINT64)
	{
		Form->txtValueInt->setVisible(true);
		Form->txtValueInt->setText(QString::number(Property->GetValue().GetUInt64()));
	}
	else if (Property->GetValueType() == ZEML_VT_FLOAT)
	{
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Property->GetValue().GetFloat()));
	}
	else if (Property->GetValueType() == ZEML_VT_DOUBLE)
	{
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Property->GetValue().GetDouble()));
	}
	else if (Property->GetValueType() == ZEML_VT_STRING)
	{
		Form->txtValueString->setVisible(true);
		ValueStringDocument->blockSignals(true);
		ValueStringDocument->setPlainText(Property->GetValue().GetString().ToCString());
		ValueStringDocument->blockSignals(false);
	}
	else if (Property->GetValueType() == ZEML_VT_BOOLEAN)
	{
		Form->chkValueBoolean->setVisible(true);
		Form->chkValueBoolean->setCheckState(Property->GetValue().GetBoolean() ? Qt::Checked : Qt::Unchecked);
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR2)
	{
		const ZEVector2& Value = Property->GetValue().GetVector2();
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.x));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.y));
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR2D)
	{
		const ZEVector2d& Value = Property->GetValue().GetVector2d();
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.x, 'g', 14));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.y, 'g', 14));
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR3)
	{
		const ZEVector3& Value = Property->GetValue().GetVector3();
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.x));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.y));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.z));
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR3D)
	{
		const ZEVector3d& Value = Property->GetValue().GetVector3d();
		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.x, 'g', 14));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.y, 'g', 14));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.z, 'g', 14));
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR4)
	{
		const ZEVector4& Value = Property->GetValue().GetVector4();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Property->GetValue().GetVector4().x));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Property->GetValue().GetVector4().y));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Property->GetValue().GetVector4().z));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Property->GetValue().GetVector4().w));
	}
	else if (Property->GetValueType() == ZEML_VT_VECTOR4D)
	{
		const ZEVector4d& Value = Property->GetValue().GetVector4d();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Property->GetValue().GetVector4d().x, 'g', 14));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Property->GetValue().GetVector4d().y, 'g', 14));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Property->GetValue().GetVector4d().z, 'g', 14));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Property->GetValue().GetVector4d().w, 'g', 14));
	}
	else if (Property->GetValueType() == ZEML_VT_QUATERNION)
	{
		const ZEQuaternion& Value = Property->GetValue().GetQuaternion();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.w));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.x));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.y));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Value.z));
	}
	else if (Property->GetValueType() == ZEML_VT_MATRIX3X3)
	{
		const ZEMatrix3x3& Value = Property->GetValue().GetMatrix3x3();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.M11));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.M12));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.M13));
		Form->txtValueFloat21->setVisible(true);
		Form->txtValueFloat21->setText(QString::number(Value.M21));
		Form->txtValueFloat22->setVisible(true);
		Form->txtValueFloat22->setText(QString::number(Value.M22));
		Form->txtValueFloat23->setVisible(true);
		Form->txtValueFloat23->setText(QString::number(Value.M23));
		Form->txtValueFloat31->setVisible(true);
		Form->txtValueFloat31->setText(QString::number(Value.M31));
		Form->txtValueFloat32->setVisible(true);
		Form->txtValueFloat32->setText(QString::number(Value.M32));
		Form->txtValueFloat33->setVisible(true);
		Form->txtValueFloat33->setText(QString::number(Value.M33));

	}
	else if (Property->GetValueType() == ZEML_VT_MATRIX3X3D)
	{
		const ZEMatrix3x3d& Value = Property->GetValue().GetMatrix3x3d();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.M11, 'g', 14));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.M12, 'g', 14));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.M13, 'g', 14));
		Form->txtValueFloat21->setVisible(true);
		Form->txtValueFloat21->setText(QString::number(Value.M21, 'g', 14));
		Form->txtValueFloat22->setVisible(true);
		Form->txtValueFloat22->setText(QString::number(Value.M22, 'g', 14));
		Form->txtValueFloat23->setVisible(true);
		Form->txtValueFloat23->setText(QString::number(Value.M23, 'g', 14));
		Form->txtValueFloat31->setVisible(true);
		Form->txtValueFloat31->setText(QString::number(Value.M31, 'g', 14));
		Form->txtValueFloat32->setVisible(true);
		Form->txtValueFloat32->setText(QString::number(Value.M32, 'g', 14));
		Form->txtValueFloat33->setVisible(true);
		Form->txtValueFloat33->setText(QString::number(Value.M33, 'g', 14));

	}
	else if (Property->GetValueType() == ZEML_VT_MATRIX4X4)
	{
		const ZEMatrix4x4& Value = Property->GetValue().GetMatrix4x4();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.M11));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.M12));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.M13));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Value.M14));
		Form->txtValueFloat21->setVisible(true);
		Form->txtValueFloat21->setText(QString::number(Value.M21));
		Form->txtValueFloat22->setVisible(true);
		Form->txtValueFloat22->setText(QString::number(Value.M22));
		Form->txtValueFloat23->setVisible(true);
		Form->txtValueFloat23->setText(QString::number(Value.M23));
		Form->txtValueFloat24->setVisible(true);
		Form->txtValueFloat24->setText(QString::number(Value.M24));
		Form->txtValueFloat31->setVisible(true);
		Form->txtValueFloat31->setText(QString::number(Value.M31));
		Form->txtValueFloat32->setVisible(true);
		Form->txtValueFloat32->setText(QString::number(Value.M32));
		Form->txtValueFloat33->setVisible(true);
		Form->txtValueFloat33->setText(QString::number(Value.M33));
		Form->txtValueFloat34->setVisible(true);
		Form->txtValueFloat34->setText(QString::number(Value.M34));
		Form->txtValueFloat41->setVisible(true);
		Form->txtValueFloat41->setText(QString::number(Value.M41));
		Form->txtValueFloat42->setVisible(true);
		Form->txtValueFloat42->setText(QString::number(Value.M42));
		Form->txtValueFloat43->setVisible(true);
		Form->txtValueFloat43->setText(QString::number(Value.M43));
		Form->txtValueFloat44->setVisible(true);
		Form->txtValueFloat44->setText(QString::number(Value.M44));
	}
	else if (Property->GetValueType() == ZEML_VT_MATRIX4X4D)
	{
		const ZEMatrix4x4d& Value = Property->GetValue().GetMatrix4x4d();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.M11, 'g', 14));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.M12, 'g', 14));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.M13, 'g', 14));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Value.M14, 'g', 14));
		Form->txtValueFloat21->setVisible(true);
		Form->txtValueFloat21->setText(QString::number(Value.M21, 'g', 14));
		Form->txtValueFloat22->setVisible(true);
		Form->txtValueFloat22->setText(QString::number(Value.M22, 'g', 14));
		Form->txtValueFloat23->setVisible(true);
		Form->txtValueFloat23->setText(QString::number(Value.M23, 'g', 14));
		Form->txtValueFloat24->setVisible(true);
		Form->txtValueFloat24->setText(QString::number(Value.M24, 'g', 14));
		Form->txtValueFloat31->setVisible(true);
		Form->txtValueFloat31->setText(QString::number(Value.M31, 'g', 14));
		Form->txtValueFloat32->setVisible(true);
		Form->txtValueFloat32->setText(QString::number(Value.M32, 'g', 14));
		Form->txtValueFloat33->setVisible(true);
		Form->txtValueFloat33->setText(QString::number(Value.M33, 'g', 14));
		Form->txtValueFloat34->setVisible(true);
		Form->txtValueFloat34->setText(QString::number(Value.M34, 'g', 14));
		Form->txtValueFloat41->setVisible(true);
		Form->txtValueFloat41->setText(QString::number(Value.M41, 'g', 14));
		Form->txtValueFloat42->setVisible(true);
		Form->txtValueFloat42->setText(QString::number(Value.M42, 'g', 14));
		Form->txtValueFloat43->setVisible(true);
		Form->txtValueFloat43->setText(QString::number(Value.M43, 'g', 14));
		Form->txtValueFloat44->setVisible(true);
		Form->txtValueFloat44->setText(QString::number(Value.M44, 'g', 14));
	}
}

void ZEMLEditorElementWidget::ConfigureForm()
{
	Form->lblNoElementSelected->setVisible(true);
	Form->grpElement->setVisible(false);
	Form->grpNode->setVisible(false);
	Form->grpProperty->setVisible(false);
	Form->grpData->setVisible(false);

	if (Element == NULL)
		return;

	Form->lblNoElementSelected->setVisible(false);
	Form->grpElement->setVisible(true);

	Form->txtName->setText(Element->GetName().ToCString());
	Form->lblElementSize->setText("-");

	if (Element->GetType() == ZEML_ET_NODE)
	{
		Form->grpNode->setVisible(true);
		Form->lblElementType->setText("Node");

		ZEMLNode* Node = (ZEMLNode*)Element;
		Form->lblElementCount->setText(QString::number(Node->GetElements().GetCount()));
		Form->lblNodeCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET_NODE).GetCount()));
		Form->lblPropertyCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET_PROPERTY).GetCount()));
		Form->lblDataCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET_DATA).GetCount()));
	}
	else if (Element->GetType() == ZEML_ET_PROPERTY)
	{
		Form->grpProperty->setVisible(true);
		Form->lblElementType->setText("Property");
		ConfigureProperty();
	}
	else if (Element->GetType() == ZEML_ET_DATA)
	{
		Form->grpData->setVisible(true);
		Form->lblElementType->setText("Data");

		ZEMLData* Data = (ZEMLData*)Element;
		Form->lblDataSize->setText(QString::number(Data->GetDataSize()));
	}
}

void ZEMLEditorElementWidget::ConfigureEditMode()
{
	bool EditMode = ZEMLEditorWindow::GetInstance()->GetEditMode();

	Form->txtName->setReadOnly(!EditMode);
	Form->cmbValueType->setEnabled(EditMode);
	Form->chkValueBoolean->setEnabled(EditMode);
	Form->txtValueString->setReadOnly(!EditMode);
	Form->txtValueInt->setReadOnly(!EditMode);
	Form->txtValueFloat11->setReadOnly(!EditMode);
	Form->txtValueFloat11->setReadOnly(!EditMode);
	Form->txtValueFloat12->setReadOnly(!EditMode);
	Form->txtValueFloat13->setReadOnly(!EditMode);
	Form->txtValueFloat14->setReadOnly(!EditMode);
	Form->txtValueFloat21->setReadOnly(!EditMode);
	Form->txtValueFloat22->setReadOnly(!EditMode);
	Form->txtValueFloat23->setReadOnly(!EditMode);
	Form->txtValueFloat24->setReadOnly(!EditMode);
	Form->txtValueFloat31->setReadOnly(!EditMode);
	Form->txtValueFloat32->setReadOnly(!EditMode);
	Form->txtValueFloat33->setReadOnly(!EditMode);
	Form->txtValueFloat34->setReadOnly(!EditMode);
	Form->txtValueFloat41->setReadOnly(!EditMode);
	Form->txtValueFloat42->setReadOnly(!EditMode);
	Form->txtValueFloat43->setReadOnly(!EditMode);
	Form->txtValueFloat44->setReadOnly(!EditMode);
	Form->btnDataLoad->setEnabled(EditMode);
}

void ZEMLEditorElementWidget::txtName_OnTextEdited(const QString& NewText)
{
	ZEString OldName = Element->GetName();
	ZEString NewName = NewText.toUtf8().constData();
	Element->SetName(NewName);
	emit NameChanged(Element, NewName, OldName);
}

void ZEMLEditorElementWidget::cmbValueType_OnCurrentIndexChanged(int Index)
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();

	ZEValue Value;
	Value.SetType(GetValueType(Index));
	Value.Clear();
	Property->SetValue(Value);

	ConfigureForm();
	
	emit ValueChanged(Property, Property->GetValue(), OldValue);
}

void ZEMLEditorElementWidget::txtValueInt_OnTextEdited(const QString& NewText)
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;

	bool Success = false;
	ZEUInt64 NewValueUnsigned;
	ZEInt64 NewValueSigned;

	if (Property->GetValue().IsSignedInteger())
	{
		NewValueSigned = NewText.toLongLong(&Success);
		if (Property->GetValue().GetIntegerRangeMin() > NewValueSigned || 
			(NewValueSigned > 0 && (ZEUInt64)NewValueSigned > Property->GetValue().GetIntegerRangeMax()))
		{
			Success = false;
		}
	}
	else
	{
		NewValueUnsigned = NewText.toULongLong(&Success);
		if (NewValueUnsigned > Property->GetValue().GetIntegerRangeMax())
			Success = false;
	}
		
	if (!Success)
	{ 
		((QLineEdit*)sender())->setStyleSheet(Success ? "" : "background:#FFAAAA;"); 
		Valid = Success; 
		return;
	}

	ZEValue OldValue = Property->GetValue();

	ZEValueType ValueType = Property->GetValue().GetType();
	if (ValueType == ZE_VRT_INTEGER_8)
		Property->SetInt8((ZEInt8)NewValueSigned);
	else if (ValueType == ZE_VRT_INTEGER_16)
		Property->SetInt16((ZEInt16)NewValueSigned);
	else if (ValueType == ZE_VRT_INTEGER_32)
		Property->SetInt32((ZEInt32)NewValueSigned);
	else if (ValueType == ZE_VRT_INTEGER_64)
		Property->SetInt64((ZEInt64)NewValueSigned);
	else if (ValueType == ZE_VRT_UNSIGNED_INTEGER_8)
		Property->SetUInt8((ZEUInt8)NewValueUnsigned);
	else if (ValueType == ZE_VRT_UNSIGNED_INTEGER_16)
		Property->SetUInt16((ZEUInt16)NewValueUnsigned);
	else if (ValueType == ZE_VRT_UNSIGNED_INTEGER_32)
		Property->SetUInt32((ZEUInt32)NewValueUnsigned);
	else if (ValueType == ZE_VRT_UNSIGNED_INTEGER_64)
		Property->SetUInt64((ZEUInt64)NewValueUnsigned);

	emit ValueChanged(Property, Property->GetValue(), OldValue);
}

void ZEMLEditorElementWidget::ValueStringDocument_OnContentsChanged()
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();
	
	Property->SetString(ValueStringDocument->toPlainText().toUtf8().constData());

	emit ValueChanged(Property, Property->GetValue(), OldValue);
}

void ZEMLEditorElementWidget::chkValueBoolean_OnStateChanged(int state)
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();

	Property->SetBool(Form->chkValueBoolean->isChecked());

	emit ValueChanged(Property, Property->GetValue(), OldValue);
}

void ZEMLEditorElementWidget::txtValueFloat_OnTextEdited(const QString& NewText)
{
	bool Success = false;
	float NewValue = NewText.toFloat(&Success);
	if (!Success)
	{ 
		((QWidget*)sender())->setStyleSheet("background:#FF3333"); 
		Valid = Success; 
		return;
	}

	((QWidget*)sender())->setStyleSheet(""); 

	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();

	int Column = sender()->property("column").toInt() - 1;
	int Row = sender()->property("row").toInt() - 1;
	if (Property->GetValue().GetType() == ZE_VRT_FLOAT)
	{
		Property->SetFloat(NewValue);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_DOUBLE)
	{
		Property->SetDouble(NewValue);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR2)
	{
		ZEVector2 Vector = Property->GetValue().GetVector2();
		Vector.M[Column] = NewValue;
		Property->SetVector2(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR2D)
	{
		ZEVector2d Vector = Property->GetValue().GetVector2d();
		Vector.M[Column] = NewValue;
		Property->SetVector2d(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR3)
	{
		ZEVector3 Vector = Property->GetValue().GetVector3();
		Vector.M[Column] = NewValue;
		Property->SetVector3(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR3D)
	{
		ZEVector3d Vector = Property->GetValue().GetVector3d();
		Vector.M[Column] = NewValue;
		Property->SetVector3d(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR4)
	{
		ZEVector4 Vector = Property->GetValue().GetVector4();
		Vector.M[Column] = NewValue;
		Property->SetVector4(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR4D)
	{
		ZEVector4d Vector = Property->GetValue().GetVector4d();
		Vector.M[Column] = NewValue;
		Property->SetVector4d(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_QUATERNION)
	{
		ZEQuaternion Quaternion = Property->GetValue().GetQuaternion();
		Quaternion.M[Column] = NewValue;
		Property->SetQuaternion(Quaternion);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_MATRIX3X3)
	{
		ZEMatrix3x3 Matrix = Property->GetValue().GetMatrix3x3();
		Matrix.M[Column][Row] = NewValue;
		Property->SetMatrix3x3(Matrix);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_MATRIX3X3D)
	{
		ZEMatrix3x3d Matrix = Property->GetValue().GetMatrix3x3d();
		Matrix.M[Column][Row] = NewValue;
		Property->SetMatrix3x3d(Matrix);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_MATRIX4X4)
	{
		ZEMatrix4x4 Matrix = Property->GetValue().GetMatrix4x4();
		Matrix.M[Column][Row] = NewValue;
		Property->SetMatrix4x4(Matrix);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_MATRIX4X4D)
	{
		ZEMatrix4x4d Matrix = Property->GetValue().GetMatrix4x4d();
		Matrix.M[Column][Row] = NewValue;
		Property->SetMatrix4x4d(Matrix);
	}

	emit ValueChanged(Property, Property->GetValue(), OldValue);
}

void ZEMLEditorElementWidget::btnDataLoad_OnClicked()
{
	if (Element == NULL)
		return;

	ZEMLData* Data = (ZEMLData*)Element;

	QString NewFileName = QFileDialog::getOpenFileName(this, 
		"Save Binary Data File", Data->GetName().ToCString(), 
		"ZE Binary Files (*.ZEBIN)");

	if (NewFileName.isEmpty())
		return;

	ZEFile DataFile;
	if (!DataFile.Open(NewFileName.toUtf8().constData(), ZE_FOM_READ, ZE_FCM_NONE))
		return;

	Data->Allocate(DataFile.GetSize());

	DataFile.Read(const_cast<void*>(Data->GetData()), Data->GetDataSize(), 1);
	DataFile.Close();

	Update();
}

void ZEMLEditorElementWidget::btnDataSave_OnClicked()
{
	if (Element == NULL)
		return;

	ZEMLData* Data = (ZEMLData*)Element;

	QString NewFileName = QFileDialog::getSaveFileName(this, 
		"Save Binary Data File", Data->GetName().ToCString(), 
		"ZE Binary Files (*.ZEBIN);;All Files (*.*)");

	if (NewFileName.isEmpty())
		return;

	ZEFile DataFile;
	if (!DataFile.Open(NewFileName.toUtf8().constData(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		return;
	
	DataFile.Write(Data->GetData(), Data->GetDataSize(), 1);
	DataFile.Close();

}

void ZEMLEditorElementWidget::SetElement(ZEMLElement* Element)
{
	if (this->Element == Element)
		return;

	this->Element = Element;

	Update();
}

ZEMLElement* ZEMLEditorElementWidget::GetElement()
{
	return Element;
}

void ZEMLEditorElementWidget::Update()
{
	ConfigureForm();
	ConfigureEditMode();
}

ZEMLEditorElementWidget::ZEMLEditorElementWidget(QWidget* Parent) : QWidget(Parent)
{
	Element = NULL;

	Form = new Ui_ZEMLEditorElementWidget();
	Form->setupUi(this);

	ValueStringDocument = new QTextDocument();
	Form->txtValueString->setDocument(ValueStringDocument);

	Update();

	Form->txtValueFloat11->setProperty("row", 1); Form->txtValueFloat11->setProperty("column", 1);
	Form->txtValueFloat12->setProperty("row", 1); Form->txtValueFloat12->setProperty("column", 2);
	Form->txtValueFloat13->setProperty("row", 1); Form->txtValueFloat13->setProperty("column", 3);
	Form->txtValueFloat14->setProperty("row", 1); Form->txtValueFloat14->setProperty("column", 4);
	Form->txtValueFloat21->setProperty("row", 2); Form->txtValueFloat21->setProperty("column", 1);
	Form->txtValueFloat22->setProperty("row", 2); Form->txtValueFloat22->setProperty("column", 2);
	Form->txtValueFloat23->setProperty("row", 2); Form->txtValueFloat23->setProperty("column", 3);
	Form->txtValueFloat24->setProperty("row", 2); Form->txtValueFloat24->setProperty("column", 4);
	Form->txtValueFloat31->setProperty("row", 3); Form->txtValueFloat31->setProperty("column", 1);
	Form->txtValueFloat32->setProperty("row", 3); Form->txtValueFloat32->setProperty("column", 2);
	Form->txtValueFloat33->setProperty("row", 3); Form->txtValueFloat33->setProperty("column", 3);
	Form->txtValueFloat34->setProperty("row", 3); Form->txtValueFloat34->setProperty("column", 4);
	Form->txtValueFloat41->setProperty("row", 4); Form->txtValueFloat41->setProperty("column", 1);
	Form->txtValueFloat42->setProperty("row", 4); Form->txtValueFloat42->setProperty("column", 2);
	Form->txtValueFloat43->setProperty("row", 4); Form->txtValueFloat43->setProperty("column", 3);
	Form->txtValueFloat44->setProperty("row", 4); Form->txtValueFloat44->setProperty("column", 4);

	connect(Form->txtName, SIGNAL(textEdited(const QString&)), this, SLOT(txtName_OnTextEdited(const QString&)));
	connect(Form->cmbValueType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbValueType_OnCurrentIndexChanged(int)));
	connect(Form->chkValueBoolean, SIGNAL(stateChanged(int)), this, SLOT(chkValueBoolean_OnStateChanged(int)));
	connect(ValueStringDocument, SIGNAL(contentsChanged()), this, SLOT(ValueStringDocument_OnContentsChanged()));
	connect(Form->txtValueInt, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueInt_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat11, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat11, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat12, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat13, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat14, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat21, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat22, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat23, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat24, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat31, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat32, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat33, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat34, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat41, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat42, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat43, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->txtValueFloat44, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueFloat_OnTextEdited(const QString&)));
	connect(Form->btnDataSave, SIGNAL(clicked()), this, SLOT(btnDataSave_OnClicked()));
	connect(Form->btnDataLoad, SIGNAL(clicked()), this, SLOT(btnDataLoad_OnClicked()));
}

ZEMLEditorElementWidget::~ZEMLEditorElementWidget()
{
	Form->txtValueString->setDocument(NULL);
	disconnect(ValueStringDocument, SIGNAL(contentsChanged()), this, SLOT(ValueStringDocument_OnContentsChanged()));
	delete ValueStringDocument;
	delete Form;
}
