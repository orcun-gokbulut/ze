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
		Form->txtValueFloat11->setText(QString::number(Value.x));
		Form->txtValueFloat12->setVisible(true);
		Form->txtValueFloat12->setText(QString::number(Value.y));
		Form->txtValueFloat13->setVisible(true);
		Form->txtValueFloat13->setText(QString::number(Value.z));
		Form->txtValueFloat14->setVisible(true);
		Form->txtValueFloat14->setText(QString::number(Value.w));
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

void ZEMLEditorElementWidget::ConfigureUI()
{
	Form->grpElement->setVisible(false);
	Form->grpNode->setVisible(false);
	Form->grpProperty->setVisible(false);
	Form->grpData->setVisible(false);
	Form->lblNoElementSelected->setVisible(true);

	if (Element == NULL)
		return;

	Form->grpElement->setVisible(false);
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
		Form->grpNode->setVisible(true);
		Form->lblElementType->setText("Node");
		ConfigureUIProperty();
	}
	else if (Element->GetType() == ZEML_ET_DATA)
	{
		Form->grpNode->setVisible(true);
		Form->lblElementType->setText("Node");

		ZEMLData* Data = (ZEMLData*)Element;
		Form->lblDataSize->setText(QString::number(Data->GetDataSize()));
	}
}

void ZEMLEditorElementWidget::cmbValueType_OnCurrentIndexChanged()
{
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();

	ZEValue Value;
	if (Form->cmbValueType->currentText() == "Int8")
		Value.SetType(ZE_VRT_INTEGER_8);
	else if (Form->cmbValueType->currentText() == "Int16")
		Value.SetType(ZE_VRT_INTEGER_16);
	else if (Form->cmbValueType->currentText() == "Int32")
		Value.SetType(ZE_VRT_INTEGER_32);
	else if (Form->cmbValueType->currentText() == "Int64")
		Value.SetType(ZE_VRT_INTEGER_64);
	else if (Form->cmbValueType->currentText() == "UInt8")
		Value.SetType(ZE_VRT_UNSIGNED_INTEGER_8);
	else if (Form->cmbValueType->currentText() == "UInt16")
		Value.SetType(ZE_VRT_UNSIGNED_INTEGER_16);
	else if (Form->cmbValueType->currentText() == "UInt32")
		Value.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
	else if (Form->cmbValueType->currentText() == "UInt64")
		Value.SetType(ZE_VRT_UNSIGNED_INTEGER_64);
	else if (Form->cmbValueType->currentText() == "Float")
		Value.SetType(ZE_VRT_FLOAT);
	else if (Form->cmbValueType->currentText() == "Double")
		Value.SetType(ZE_VRT_DOUBLE);
	else if (Form->cmbValueType->currentText() == "Boolean")
		Value.SetType(ZE_VRT_BOOLEAN);
	else if (Form->cmbValueType->currentText() == "String")
		Value.SetType(ZE_VRT_STRING);
	else if (Form->cmbValueType->currentText() == "Vector2")
		Value.SetType(ZE_VRT_VECTOR2);
	else if (Form->cmbValueType->currentText() == "Vector3")
		Value.SetType(ZE_VRT_VECTOR3);
	else if (Form->cmbValueType->currentText() == "Vector4")
		Value.SetType(ZE_VRT_VECTOR4);
	else if (Form->cmbValueType->currentText() == "Quaternion")
		Value.SetType(ZE_VRT_QUATERNION);
	else if (Form->cmbValueType->currentText() == "Matrix3x3")
		Value.SetType(ZE_VRT_MATRIX3X3);
	else if (Form->cmbValueType->currentText() == "Matrix4x4")
		Value.SetType(ZE_VRT_MATRIX4X4);
	
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
		Form->txtValueFloat11->setStyleSheet(Success ? "" : "background:#FFAAAA;"); 
		Valid = Success; 
		return;
	}
	
	ZEMLProperty* Property = (ZEMLProperty*)Element;
	ZEValue OldValue = Property->GetValue();

	int Column = sender()->property("column").toInt();
	int Row = sender()->property("row").toInt();
	if (Property->GetValue().GetType() == ZE_VRT_VECTOR2)
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
		Matrix.M[Row][Column] = NewValue;
		Property->SetMatrix3x3(Matrix);
	}
	else if (Property->GetValueType() == ZE_VRT_MATRIX4X4)
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
}

ZEMLEditorElementWidget::~ZEMLEditorElementWidget()
{
	delete Form;
}
