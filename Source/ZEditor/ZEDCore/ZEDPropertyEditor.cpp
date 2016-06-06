//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditor.cpp
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

#include "ZEDPropertyEditor.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEClass.h"
#include "ZEDModule.h"
#include "ZEDObjectWrapper.h"
#include "ZEDSelectionEvent.h"
#include "ZEDPropertyOperation.h"
#include "ZEDOperationManager.h"
#include "ZEDPropertyEditorItemNumeric.h"
#include "ZEDPropertyEditorItemBoolean.h"
#include "ZEDPropertyEditorItemString.h"
#include "ZEDPropertyEditorItemEnumurator.h"
#include "ZEDPropertyEditorItemVector.h"

#include <QHeaderView>
#include "QStyledItemDelegate"
#include "QPainter"

class ZEDPropertyEditorGridDelegate : public QStyledItemDelegate
{
	public:
		explicit ZEDPropertyEditorGridDelegate(QObject * parent = 0) : QStyledItemDelegate(parent) { }

		void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
		{		
			QStyledItemDelegate::paint(painter, option, index);

			QPalette Palatte;
			painter->save();
			painter->setPen(QColor(76, 70, 70));
			painter->drawRect(option.rect);
			painter->restore();

		}
};

void ZEDPropertyEditor::Populate()
{
	clear();

	if (Wrappers.GetCount() == 0)
		return;

	// Find Common Ancestor
	ZEClass* BaseClass = NULL;
	
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		ZEClass* CurrentClass = Wrappers[I]->GetObject()->GetClass();
		if (BaseClass == NULL)
		{
			BaseClass = CurrentClass;
			continue;
		}

		while (!ZEClass::IsDerivedFrom(BaseClass, CurrentClass))
			BaseClass = BaseClass->GetParentClass();
	}
	
	if (BaseClass == NULL)
		return;

	ZEClass* CurrentClass = NULL;
	QTreeWidgetItem* CurrentClassItem = NULL;
	const ZEProperty* Properties = BaseClass->GetProperties();
	for (ZESize I = 0; I < BaseClass->GetPropertyCount(); I++)
	{
		if (Properties[I].MemberOf != CurrentClass)
		{
			CurrentClass = Properties[I].MemberOf;

			CurrentClassItem = new QTreeWidgetItem();
			CurrentClassItem->setText(0, CurrentClass->GetName());

			QFont Font = CurrentClassItem->font(0);
			Font.setBold(true);	
			CurrentClassItem->setFont(0, Font);

			QPalette Palette;
			CurrentClassItem->setBackgroundColor(0, QColor(76, 70, 70));
			CurrentClassItem->setBackgroundColor(1, QColor(76, 70, 70));
			CurrentClassItem->setTextColor(0, QColor(255, 255, 255));
			CurrentClassItem->setTextColor(1, QColor(255, 255, 255));

			addTopLevelItem(CurrentClassItem);
			CurrentClassItem->setExpanded(true);	
		}

		if (Properties[I].Access == ZEMT_PA_NONE || Properties[I].Access == ZEMT_PA_WRITE)
			continue;

		if (Properties[I].Type.ContainerType != ZE_CT_NONE)
			continue;

		switch (Properties[I].Type.Type)
		{
			case ZE_TT_UNDEFINED:
			case ZE_TT_VOID:
				continue;

			case ZE_TT_INTEGER_8:
			case ZE_TT_INTEGER_16:
			case ZE_TT_INTEGER_32:
			case ZE_TT_INTEGER_64:
			case ZE_TT_UNSIGNED_INTEGER_8:
			case ZE_TT_UNSIGNED_INTEGER_16:
			case ZE_TT_UNSIGNED_INTEGER_32:
			case ZE_TT_UNSIGNED_INTEGER_64:
			case ZE_TT_DOUBLE:
			case ZE_TT_FLOAT:
			{
				ZEDPropertyEditorItemNumeric* NumericItem = new ZEDPropertyEditorItemNumeric();
				NumericItem->SetProperty(&Properties[I]);
				CurrentClassItem->addChild(NumericItem);
				NumericItem->Initialize();
				NumericItem->Update();
				break;
			}

			case ZE_TT_BOOLEAN:
			{
				ZEDPropertyEditorItemBoolean* BooleanItem = new ZEDPropertyEditorItemBoolean();
				BooleanItem->SetProperty(&Properties[I]);
				CurrentClassItem->addChild(BooleanItem);
				BooleanItem->Initialize();
				BooleanItem->Update();
				break;
			}

			case ZE_TT_STRING:
			{
				ZEDPropertyEditorItemString* StringItem = new ZEDPropertyEditorItemString();
				StringItem->SetProperty(&Properties[I]);
				CurrentClassItem->addChild(StringItem);
				StringItem->Initialize();
				StringItem->Update();
				break;
			}

			case ZE_TT_ENUMERATOR:
			{
				ZEDPropertyEditorItemEnumurator* EnumuratorItem = new ZEDPropertyEditorItemEnumurator();
				EnumuratorItem->SetProperty(&Properties[I]);
				CurrentClassItem->addChild(EnumuratorItem);
				EnumuratorItem->Initialize();
				EnumuratorItem->Update();
				break;
			}

			case ZE_TT_QUATERNION:
			case ZE_TT_VECTOR2:
			case ZE_TT_VECTOR2D:
			case ZE_TT_VECTOR3:
			case ZE_TT_VECTOR3D:
			case ZE_TT_VECTOR4:
			case ZE_TT_VECTOR4D:
			case ZE_TT_MATRIX3X3:
			case ZE_TT_MATRIX3X3D:
			case ZE_TT_MATRIX4X4:
			case ZE_TT_MATRIX4X4D:
			{
				ZEDPropertyEditorItemVector* VectorItem = new ZEDPropertyEditorItemVector();
				VectorItem->SetProperty(&Properties[I]);
				CurrentClassItem->addChild(VectorItem);
				VectorItem->Initialize();
				VectorItem->Update();
				break;
			}

			case ZE_TT_OBJECT:
			case ZE_TT_OBJECT_PTR:
				continue;

			case ZE_TT_CLASS:
			{
				QTreeWidgetItem* Item = new ZEDPropertyEditorItem();
				Item->setText(0, Properties[I].Name);
				CurrentClassItem->addChild(Item);

				ZEVariant PropertyValue;
				for (ZESize N = 0; N < Wrappers.GetCount(); N++)
				{
					ZEVariant Current;
					if (!Wrappers[N]->GetObject()->GetClass()->GetProperty(Wrappers[N]->GetObject(), Properties[I].Name, Current))
					{
						Item->setText(1, "ERROR");
						break;
					}

					if (PropertyValue.IsUndefined())
					{
						PropertyValue = Current;
						Item->setText(1, Current.GetClass()->GetName());
					} 
					else if (PropertyValue.GetClass() != Current.GetClass())
					{
						Item->setText(1, "Multiple Object Class");
						break;
					}
				}
		
				continue;
			}
		}
	}
}

void ZEDPropertyEditor::ObjectEvent(const ZEDObjectEvent* Event)
{
	if (!Wrappers.Exists(Event->GetWrapper()))
		return;

	if (Event->GetType() == ZED_OCET_REMOVED)
		RemoveWrapper(Event->GetWrapper());
	else if (Event->GetType() == ZED_OCET_CHANGED)
		Update();
}

void ZEDPropertyEditor::SelectionEvent(const ZEDSelectionEvent* Event)
{
	if (Event->GetType() != ZED_SET_SELECTED && Event->GetType() != ZED_SET_DESELECTED)
		return;

	for (ZESize I = 0; I < Event->GetUnselectedObjects().GetCount(); I++)
		Wrappers.RemoveValue(Event->GetUnselectedObjects()[I]);

	for (ZESize I = 0; I < Event->GetSelectedObjects().GetCount(); I++)
		Wrappers.Add(Event->GetSelectedObjects()[I]);

	Dirty = true;
}

void ZEDPropertyEditor::TickEvent(const ZEDTickEvent* Event)
{
	if (!Dirty)
		return;

	Populate();
	Dirty = false;
}

void ZEDPropertyEditor::PropertyChanged(const ZEProperty* Property, const ZEVariant& Value)
{
	ZEDPropertyOperation* Operation = ZEDPropertyOperation::Create(Wrappers,Property, Value);
	GetModule()->GetOperationManager()->DoOperation(Operation);
	
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		ZEDObjectEvent Event;
		Event.SetType(ZED_OCET_CHANGED);
		Event.SetWrapper(Wrappers[I]);
		RaiseEvent(&Event);
	}
}

void ZEDPropertyEditor::PropertyChanged(const ZEProperty* Property, const ZEArray<ZEVariant>& Values)
{
	ZEDPropertyOperation* Operation = ZEDPropertyOperation::Create(Wrappers,Property, Values);
	GetModule()->GetOperationManager()->DoOperation(Operation);

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		ZEDObjectEvent Event;
		Event.SetType(ZED_OCET_CHANGED);
		Event.SetWrapper(Wrappers[I]);
		RaiseEvent(&Event);
	}
}

const ZEArray<ZEDObjectWrapper*>& ZEDPropertyEditor::GetWrappers() const
{
	return Wrappers;
}

void ZEDPropertyEditor::AddWrapper(ZEDObjectWrapper* Wrapper)
{
	zeCheckError(Wrappers.Exists(Wrapper), ZE_VOID, "Object wrapper is already added to list.");

	Wrappers.Add(Wrapper);
	Dirty = true;
}

void ZEDPropertyEditor::RemoveWrapper(ZEDObjectWrapper* Wrapper)
{
	Wrappers.RemoveValue(Wrapper);
	Dirty = true;
}

void ZEDPropertyEditor::Update()
{
	int ItemCount = topLevelItemCount();
	for (int I = 0; I < ItemCount; I++)
	{
		QTreeWidgetItem* ClassItem = static_cast<ZEDPropertyEditorItem*>(topLevelItem(I));
		for (ZESize N = 0; N < ClassItem->childCount(); N++)
		{
			ZEDPropertyEditorItem* Item = static_cast<ZEDPropertyEditorItem*>(ClassItem->child(N));
			Item->Update();
		}
	}
}

ZEDPropertyEditor::ZEDPropertyEditor(QWidget* Parent) : QTreeWidget(Parent)
{
	Dirty = false;
	BaseClass = NULL;

	setWindowTitle("Property Editor");

	setColumnCount(2);

	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	//setItemDelegate(new ZEDPropertyEditorGridDelegate(this));

	QTreeWidgetItem* Item = new QTreeWidgetItem();
	Item->setText(0, "Name");
	Item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
	Item->setText(1, "Value");
	Item->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
	setHeaderItem(Item);

	header()->setStretchLastSection(true);
}
