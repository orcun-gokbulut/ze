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

#include "ZEML\ZEMLElement.h"
#include "ZEML\ZEMLNode.h"
#include "ZEML\ZEMLProperty.h"
#include "ZEML\ZEMLData.h"

void ZEMLEditorElementWidget::ConfigureUIProperty()
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
		Form->txtValueString->setText(Property->GetValue().GetString().ToCString());
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
	else if (Property->GetValueType() == ZEML_VT_MATRIX4X4)
	{
		const ZEMatrix4x4& Value = Property->GetValue().GetMatrix4x4();

		Form->txtValueFloat11->setVisible(true);
		Form->txtValueFloat11->setText(QString::number(Value.M11));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.M12));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.M14));
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
}

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
		case ZE_VRT_VECTOR3:
			return 13;
		case ZE_VRT_VECTOR4:
			return 14;
		case ZE_VRT_QUATERNION:
			return 15;
		case ZE_VRT_MATRIX3X3:
			return 16;
		case ZE_VRT_MATRIX4X4:
			return 17;
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
			return ZE_VRT_VECTOR3;
		case 14:
			return ZE_VRT_VECTOR4;
		case 15:
			return ZE_VRT_QUATERNION;
		case 16:
			return ZE_VRT_MATRIX3X3;
		case 17:
			return ZE_VRT_MATRIX4X4;
	}
}

void ZEMLEditorElementWidget::ConfigureUI()
{
	Form->grpElement->setVisible(false);
	Form->grpNode->setVisible(false);
	Form->grpProperty->setVisible(false);
	Form->grpData->setVisible(false);
	Form->lblNoElementSelected->setVisible(true);

	if (Element == NULL)
		return;

	Form->grpElement->setVisible(true);
	Form->lblNoElementSelected->setVisible(false);

	Form->txtElementName->setText(Element->GetName().ToCString());
	Form->lblElementSize->setText(QString::number(Element->GetSize()));

	if (Element->GetType() == ZEML_ET1_NODE)
	{
		Form->grpNode->setVisible(true);
		Form->lblElementType->setText("Node");

		ZEMLNode* Node = (ZEMLNode*)Element;
		Form->lblElementCount->setText(QString::number(Node->GetElements().GetCount()));
		Form->lblNodeCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET1_NODE).GetCount()));
		Form->lblPropertyCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET_PROPERTY).GetCount()));
		Form->lblDataCount->setText(QString::number(Node->GetElements(NULL, ZEML_ET_DATA).GetCount()));
	}
	else if (Element->GetType() == ZEML_ET_PROPERTY)
	{
		Form->grpProperty->setVisible(true);
		Form->lblElementType->setText("Node");
		ConfigureUIProperty();
	}
	else if (Element->GetType() == ZEML_ET_DATA)
	{
		Form->grpData->setVisible(true);
		Form->lblElementType->setText("Node");

		ZEMLData* Data = (ZEMLData*)Element;
		Form->lblDataSize->setText(QString::number(Data->GetDataSize()));
	}
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

	ConfigureUI();
	
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

void ZEMLEditorElementWidget::txtValueString_OnTextEdited(const QString& NewText)
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();
	
	Property->SetString(NewText.toStdString().c_str());

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
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR3)
	{
		ZEVector3 Vector = Property->GetValue().GetVector3();
		Vector.M[Column] = NewValue;
		Property->SetVector3(Vector);
	}
	else if (Property->GetValue().GetType() == ZE_VRT_VECTOR4)
	{
		ZEVector4 Vector = Property->GetValue().GetVector4();
		Vector.M[Column] = NewValue;
		Property->SetVector4(Vector);
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
	else if (Property->GetValue().GetType() == ZE_VRT_MATRIX4X4)
	{
		ZEMatrix4x4 Matrix = Property->GetValue().GetMatrix4x4();
		Matrix.M[Column][Row] = NewValue;
		Property->SetMatrix4x4(Matrix);
	}

	emit ValueChanged(Property, Property->GetValue(), OldValue);
}


void ZEMLEditorElementWidget::SetElement(ZEMLElement* Element)
{
	this->Element = Element;
	ConfigureUI();
}

ZEMLElement* ZEMLEditorElementWidget::GetElement()
{
	return Element;
}

ZEMLEditorElementWidget::ZEMLEditorElementWidget(QWidget* Parent) : QWidget(Parent)
{
	Element = NULL;

	Form = new Ui_ZEMLEditorElementWidget();
	Form->setupUi(this);

	ConfigureUI();

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

	connect(Form->cmbValueType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbValueType_OnCurrentIndexChanged(int)));
	connect(Form->chkValueBoolean, SIGNAL(stateChanged(int)), this, SLOT(chkValueBoolean_OnStateChanged(int)));
	connect(Form->txtValueString, SIGNAL(textEdited(const QString&)), this, SLOT(txtValueString_OnTextEdited(const QString&)));
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
}

ZEMLEditorElementWidget::~ZEMLEditorElementWidget()
{
	delete Form;
}
