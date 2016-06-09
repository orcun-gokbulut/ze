//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemVector.cpp
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

#include "ZEDPropertyEditorItemVector.h"

#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEProperty.h"
#include "ZEDPropertyEditor.h"
#include "ZEDCore/ZEDObjectWrapper.h"

#include "QEvent.h"
#include <QLineEdit>
#include <QGuiApplication>
#include "ZEMath/ZEAngle.h"

void ZEDPropertyEditorItemVector::SetMemeberValue(ZEVariant& Variant, ZESize MemberIndex, double MemberValue)
{
	switch (GetProperty()->Type.Type)
	{
		case ZE_TT_VECTOR2:
		{
			ZEVector2 Temp = Variant.GetVector2();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector2(Temp);
			break;
		}

		case ZE_TT_VECTOR2D:
		{
			ZEVector2d Temp = Variant.GetVector2d();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector2d(Temp);
			break;
		}

		case ZE_TT_VECTOR3:
		{
			ZEVector3 Temp = Variant.GetVector3();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector3(Temp);
			break;
		}

		case ZE_TT_VECTOR3D:
		{
			ZEVector3d Temp = Variant.GetVector3d();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector3d(Temp);
			break;
		}

		case ZE_TT_VECTOR4:
		{
			ZEVector4 Temp = Variant.GetVector4();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector4(Temp);
			break;
		}

		case ZE_TT_VECTOR4D:
		{
			ZEVector4d Temp = Variant.GetVector4d();
			Temp.M[MemberIndex] = MemberValue;
			Variant.SetVector4d(Temp);
			break;
		}

		case ZE_TT_QUATERNION:
		{
			ZEQuaternion Temp = Variant.GetQuaternion();

			if (MemberIndex < 3)
			{
				// Euler [Rx, Ry, Rz]
				ZEVector3 Rotation;
				ZEQuaternion::ConvertToEulerAngles(Rotation, Temp);
				Rotation.M[MemberIndex] = ZEAngle::ToRadian(MemberValue);
				ZEQuaternion::CreateFromEuler(Temp, Rotation);
			}
			else
			{
				// Quaternion [w, x, y, z]
				Temp.M[MemberIndex - 3] = MemberValue;
			}

			Variant.SetQuaternion(Temp);
			break;
		}

		case ZE_TT_MATRIX3X3:
		{
			ZEMatrix3x3 Temp = Variant.GetMatrix3x3();
			Temp.MA[MemberIndex] = MemberValue;
			Variant.SetMatrix3x3(Temp);
			break;
		}

		case ZE_TT_MATRIX3X3D:
		{
			ZEMatrix3x3d Temp = Variant.GetMatrix3x3d();
			Temp.MA[MemberIndex] = MemberValue;
			Variant.SetMatrix3x3d(Temp);
			break;
		}

		case ZE_TT_MATRIX4X4:
		{
			ZEMatrix4x4 Temp = Variant.GetMatrix4x4();
			Temp.MA[MemberIndex] = MemberValue;
			Variant.SetMatrix4x4(Temp);
			break;
		}

		case ZE_TT_MATRIX4X4D:
		{
			ZEMatrix4x4d Temp = Variant.GetMatrix4x4d();
			Temp.MA[MemberIndex] = MemberValue;
			Variant.SetMatrix4x4d(Temp);
			break;
		}
	}
}

double ZEDPropertyEditorItemVector::GetMemeberValue(const ZEVariant& Variant, ZESize MemberIndex)
{
	switch (GetProperty()->Type.Type)
	{
		case ZE_TT_VECTOR2:
			return Variant.GetVector2().M[MemberIndex];

		case ZE_TT_VECTOR2D:
			return Variant.GetVector2d().M[MemberIndex];

		case ZE_TT_VECTOR3:
			return Variant.GetVector3().M[MemberIndex];

		case ZE_TT_VECTOR3D:
			return Variant.GetVector3d().M[MemberIndex];

		case ZE_TT_VECTOR4:
			return Variant.GetVector4().M[MemberIndex];

		case ZE_TT_VECTOR4D:
			return Variant.GetVector4d().M[MemberIndex];

		case ZE_TT_QUATERNION:
			if (MemberIndex < 3)
			{
				// Euler [Rx, Ry, Rz]
				ZEVector3 Rotation;
				ZEQuaternion::ConvertToEulerAngles(Rotation, Variant.GetQuaternion());
				return ZEAngle::ToDegree(Rotation.M[MemberIndex]);
			}
			else
			{
				// Quaternion [w, x, y, z]
				return Variant.GetQuaternion().M[MemberIndex - 3];
			}
			break;

		case ZE_TT_MATRIX3X3:
			return  Variant.GetMatrix3x3().MA[MemberIndex];

		case ZE_TT_MATRIX3X3D:
			return Variant.GetMatrix3x3d().MA[MemberIndex];

		case ZE_TT_MATRIX4X4:
			return Variant.GetMatrix4x4().MA[MemberIndex];

		case ZE_TT_MATRIX4X4D:
			return Variant.GetMatrix4x4d().MA[MemberIndex];
		
		default:
			return 0.0;
	}
}

ZESSize ZEDPropertyEditorItemVector::FindIndex(QLineEdit* TextEdit)
{
	for (ZESize I = 0; I < RowCount * ColumnCount; I++)
	{
		if (TextEdits[I] == TextEdit)
			return I;
	}

	return -1;
}

bool ZEDPropertyEditorItemVector::InitializeSelf()
{
	if (!ZEDPropertyEditorItem::InitializeSelf())
	{
		setText(1, "Error");
		return false;
	}

	const ZEProperty* Property = GetProperty();
	switch (GetProperty()->Type.Type)
	{
		case ZE_TT_VECTOR2:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			RowCount = 1;
			ColumnCount = 2;
			Detail = false;
			break;

		case ZE_TT_VECTOR2D:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			RowCount = 1;
			ColumnCount = 2;
			Detail = false;
			break;

		case ZE_TT_VECTOR3:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			Labels[2]  = "z";
			RowCount = 1;
			ColumnCount = 3;
			Detail = false;
			break;

		case ZE_TT_VECTOR3D:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			Labels[2]  = "z";
			RowCount = 1;
			ColumnCount = 4;
			Detail = false;
			break;

		case ZE_TT_VECTOR4:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			Labels[2]  = "z";
			Labels[3]  = "w";
			RowCount = 1;
			ColumnCount = 4;
			Detail = false;
			break;

		case ZE_TT_VECTOR4D:
			Labels[0]  = "x"; 
			Labels[1]  = "y";
			Labels[2]  = "z";
			Labels[3]  = "w";
			RowCount = 1;
			ColumnCount = 4;
			Detail = false;
			break;

		case ZE_TT_QUATERNION:
			Labels[0]  = "Rx"; 
			Labels[1]  = "Ry";
			Labels[2]  = "Rz";
			Labels[3]  = "Qw";
			Labels[4]  = "Qx"; 
			Labels[5]  = "Qy";
			Labels[6]  = "Qz";
			RowCount = 1;
			ColumnCount = 7;
			Detail = true;
			break;

		case ZE_TT_MATRIX3X3:
			Labels[0]  = "M11";
			Labels[1]  = "M21"; 
			Labels[2]  = "M31";
			Labels[4]  = "M12";
			Labels[5]  = "M22";
			Labels[6]  = "M32";
			Labels[8]  = "M13";
			Labels[9]  = "M23";
			Labels[10] = "M33";
			Labels[12] = "M14";
			Labels[13] = "M24";
			Labels[14] = "M34";
			RowCount = 3;
			ColumnCount = 3;
			Detail = true;
			break;

		case ZE_TT_MATRIX3X3D:
			Labels[0]  = "M11";
			Labels[1]  = "M21"; 
			Labels[2]  = "M31";
			Labels[4]  = "M12";
			Labels[5]  = "M22";
			Labels[6]  = "M32";
			Labels[8]  = "M13";
			Labels[9]  = "M23";
			Labels[10] = "M33";
			Labels[12] = "M14";
			Labels[13] = "M24";
			Labels[14] = "M34";
			RowCount = 3;
			ColumnCount = 3;
			Detail = true;
			break;

		case ZE_TT_MATRIX4X4:
			Labels[0]  = "M11";
			Labels[1]  = "M21"; 
			Labels[2]  = "M31";
			Labels[3]  = "M41";
			Labels[4]  = "M12";
			Labels[5]  = "M22";
			Labels[6]  = "M32";
			Labels[7]  = "M42";
			Labels[8]  = "M13";
			Labels[9]  = "M23";
			Labels[10] = "M33";
			Labels[11] = "M43";
			Labels[12] = "M14";
			Labels[13] = "M24";
			Labels[14] = "M34";
			Labels[15] = "M44";
			RowCount = 4;
			ColumnCount = 4;
			Detail = true;
			break;

		case ZE_TT_MATRIX4X4D:
			Labels[0]  = "M11";
			Labels[1]  = "M21"; 
			Labels[2]  = "M31";
			Labels[3]  = "M41";
			Labels[4]  = "M12";
			Labels[5]  = "M22";
			Labels[6]  = "M32";
			Labels[7]  = "M42";
			Labels[8]  = "M13";
			Labels[9]  = "M23";
			Labels[10] = "M33";
			Labels[11] = "M43";
			Labels[12] = "M14";
			Labels[13] = "M24";
			Labels[14] = "M34";
			Labels[15] = "M44";
			RowCount = 4;
			ColumnCount = 4;
			Detail = true;
			break;

		default:
			setText(1, "Type Error");
			return false;
	}

	for (ZESize I = 0; I < RowCount * ColumnCount; I++)
	{
		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(0, Labels[I]);
		addChild(Item);

		TextEdits[I] = new QLineEdit();
		TextEdits[I]->setFrame(false);
		TextEdits[I]->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
		TextEdits[I]->setReadOnly(Property->Access == ZEMT_PA_READ);
		TextEdits[I]->installEventFilter(this);
		treeWidget()->setItemWidget(Item, 1, TextEdits[I]);

		connect(TextEdits[I], SIGNAL(textChanged(const QString&)), this, SLOT(TextEdit_textChanged(const QString&)));
		connect(TextEdits[I], SIGNAL(editingFinished()), this, SLOT(TextEdit_editingFinished()));
	}

	return true;
}

bool ZEDPropertyEditorItemVector::eventFilter(QObject* Object, QEvent* Event)
{
	bool Found = false;
	for (ZESize I = 0; I < RowCount * ColumnCount; I++)
	{
		if (static_cast<QLineEdit*>(Object) == TextEdits[I])
		{
			Found = true;
			break;
		}
	}
	
	if (Found && Event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(Event)->key() == Qt::Key_Escape)
		Update();

	return false;
}

void ZEDPropertyEditorItemVector::TextEdit_textChanged(const QString& Text)
{
	QLineEdit* TextEdit = static_cast<QLineEdit*>(sender());

	ZESSize Index = FindIndex(TextEdit);
	if (Index == -1)
		return;

	bool Result = false;
	double Value = Text.toDouble(&Result);
	if (!Result)
	{
		TextEdit->setStyleSheet("* { background-color: rgba(255, 200, 210, 255); }");
	}
	else
	{
		ValueChanged = true;
		TextEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	}
}

void ZEDPropertyEditorItemVector::TextEdit_editingFinished()
{
	QLineEdit* TextEdit = static_cast<QLineEdit*>(sender());
	ZESSize Index = FindIndex(TextEdit);
	if (Index == -1)
		return;

	bool Result = false;
	double Value = TextEdit->text().toDouble(&Result);
	if (!Result)
	{
		Update();
		return;
	}
	
	if (!ValueChanged)
		return;

	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetPropertyEditor()->GetWrappers();
	ZEArray<ZEVariant> Values;
	Values.SetCount(Wrappers.GetCount());
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		ZEObject* Object = Wrappers[I]->GetObject();
		ZEClass* Class = Object->GetClass();
		if (!Class->GetProperty(Object, GetProperty()->ID, Values[I]))
			return;

		SetMemeberValue(Values[I], Index, Value);
	}

	Changed(Values);
	Update();
}

void ZEDPropertyEditorItemVector::Update()
{
	if (!IsInitialized())
		return;

	ValueChanged = false;

	const ZEArray<ZEDObjectWrapper*> Wrappers = (GetPropertyEditor()->GetWrappers());

	ZEArray<ZEVariant> PropertyValues;
	PropertyValues.SetCount(Wrappers.GetCount());

	for (ZESize WrapperIndex = 0; WrapperIndex < Wrappers.GetCount(); WrapperIndex++)
	{
		ZEObject* Object = Wrappers[WrapperIndex]->GetObject();
		ZEClass* Class = Object->GetClass();
		if (!Class->GetProperty(Object, GetProperty()->ID, PropertyValues[WrapperIndex]))
		{
			for (ZESize MemberIndex = 0; MemberIndex < ColumnCount * RowCount; MemberIndex++)
			{
				QSignalBlocker Blocker(TextEdits[MemberIndex]);
				TextEdits[MemberIndex]->setText("");
				TextEdits[MemberIndex]->setEnabled(false);
			}
			return;
		}
	}

	double VectorMembers[4];
	bool VectorMembersMultiValue[4];
	for (ZESize MemberIndex = 0; MemberIndex < ColumnCount * RowCount; MemberIndex++)
	{
		bool MultiValue = false;
		double MemberValue;
		for (ZESize WrapperIndex = 0; WrapperIndex < Wrappers.GetCount(); WrapperIndex++)
		{
			double CurrentValue = GetMemeberValue(PropertyValues[WrapperIndex], MemberIndex);

			if (WrapperIndex == 0)
			{
				MemberValue = CurrentValue;
			}
			else if (CurrentValue != MemberValue)
			{
				MultiValue = true;
				break;
			}
		}

		QSignalBlocker Blocker(TextEdits[MemberIndex]);
		TextEdits[MemberIndex]->setEnabled(true);
		TextEdits[MemberIndex]->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");

		if (MemberIndex < 4)
		{
			VectorMembers[MemberIndex] = MemberValue;
			VectorMembersMultiValue[MemberIndex] = MultiValue;
		}

		if (!MultiValue)
		{
			TextEdits[MemberIndex]->setText(QString::number(MemberValue, 'f', 3));
		}
		else
		{
			TextEdits[MemberIndex]->setText("");
			break;
		}	
	}

	switch(GetProperty()->Type.Type)
	{
		case ZE_TT_VECTOR2:
		case ZE_TT_VECTOR2D:
			setText(1, QString("<x:%1, y:%2>")
				.arg(VectorMembersMultiValue[0] ? "..." : QString::number(VectorMembers[0]))
				.arg(VectorMembersMultiValue[1] ? "..." : QString::number(VectorMembers[1])));
			break;

		case ZE_TT_VECTOR3:
		case ZE_TT_VECTOR3D:
			setText(1, QString("<x:%1, y:%2, z:%3>")
				.arg(VectorMembersMultiValue[0] ? "..." : QString::number(VectorMembers[0]))
				.arg(VectorMembersMultiValue[1] ? "..." : QString::number(VectorMembers[1]))
				.arg(VectorMembersMultiValue[2] ? "..." : QString::number(VectorMembers[2])));
			break;

		case ZE_TT_VECTOR4:
		case ZE_TT_VECTOR4D:
			setText(1, QString("<x:%1, y:%2, z:%3, w:%4>")
				.arg(VectorMembersMultiValue[0] ? "..." : QString::number(VectorMembers[0]))
				.arg(VectorMembersMultiValue[1] ? "..." : QString::number(VectorMembers[1]))
				.arg(VectorMembersMultiValue[2] ? "..." : QString::number(VectorMembers[2]))
				.arg(VectorMembersMultiValue[3] ? "..." : QString::number(VectorMembers[3])));
			break;

		case ZE_TT_QUATERNION:
			setText(1, QString("<Rx:%1, Ry:%2, Rz:%3>")
				.arg(VectorMembersMultiValue[0] ? "..." : QString::number(VectorMembers[0]))
				.arg(VectorMembersMultiValue[1] ? "..." : QString::number(VectorMembers[1]))
				.arg(VectorMembersMultiValue[2] ? "..." : QString::number(VectorMembers[2])));
			break;

		default:
			setText(1, "...");
			break;
	}
}

ZEDPropertyEditorItemVector::ZEDPropertyEditorItemVector()
{
	memset(Labels, 0, sizeof(Labels));
	memset(TextEdits, 0, sizeof(TextEdits));
	ValueChanged = false;
	Detail = false;
	RowCount = 0;
	ColumnCount = 0;
}
