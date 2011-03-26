//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyWindowManager.cpp
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

#include <QList>
#include <QFont>
#include <QPalette>
#include "ZEDPropertyWindowManager.h"
#include "ZEGame\ZEEntity.h"
#include "ZEDPropertyWidget.h"
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "ZEDCommonControls/CSS.h"

ZEDPropertyWindowManagerGroup::ZEDPropertyWindowManagerGroup(QString GroupName, QTreeWidgetItem* Parent)
{
	QStringList TempList;
	TempList << GroupName;
	this->GroupName = GroupName;
	this->Item = new QTreeWidgetItem(Parent, TempList);
	//this->Item->setForeground(0,QBrush(QColor(0,0,0)));
	QFont Font = Item->font(0);
	Font.setBold(true);
	Item->setFont(0, Font);
	//this->Item->setBackgroundColor(0,QColor(150,150,150));
	//this->Item->setBackgroundColor(1,QColor(150,150,150));
}

ZEDPropertyWindowManagerGroup::~ZEDPropertyWindowManagerGroup()
{
	delete Item;
}

ZEDPropertyWindowManager::ZEDPropertyWindowManager(QWidget *parent, ZEClass* Class, QString WorkingDirectory) : QTreeWidget(parent)
{
	this->WorkingDirectory = WorkingDirectory;
	this->Class = Class;
	setColumnCount(2);
	QStringList HorizontalHeader;
	HorizontalHeader << "Name" << "Value";
	this->setHeaderLabels(HorizontalHeader);
	setAlternatingRowColors(true);
	UpdatePropertyWidgets();
	this->setColumnWidth(0,this->size().width()*1.5);
	this->setColumnWidth(1,this->size().width());
	//this->setAllColumnsShowFocus(true);
	this->setSortingEnabled(false);
	//QPalette Palette = palette();
	//Palette.setBrush(QPalette::AlternateBase,QBrush(QColor(230, 230, 230)));
	//Palette.setBrush(QPalette::Base,QBrush(QColor(220, 220, 220)));
	//setPalette(Palette);
	setRootIsDecorated(false);
	//setIndentation(15);
	header()->hide();
	//setStyleSheet(GetCSSFromFile(":/CSS/PropertyWindow.qss"));
	show();
}

QTreeWidgetItem* ZEDPropertyWindowManager::FindGroup(QString GroupName)
{
	for(int I = 0; I < Groups.count(); I++)
		if(GroupName == Groups[I]->GroupName)
			return Groups[I]->Item;
}

void ZEDPropertyWindowManager::UpdatePropertyWidgets()
{

	QSize							RowSize;
	QStringList						GroupNames;
	QString							CurrentGroupName;
	QTreeWidgetItem*				WidgetItem;
	QFont							TopLevelFont;
	QLabel*							CurrentLabel;

	const ZEPropertyDescription*		Properties;
	size_t								PropertyCount;

	ZEClassDescription* ClassDescription = this->Class->GetClassDescription();

	while (ClassDescription != NULL)
	{
		PropertyCount = ClassDescription->GetPropertyCount();

		if(PropertyCount != 0)
			this->addTopLevelItem(new QTreeWidgetItem(QStringList(QString(ClassDescription->GetName()))));
	
		Properties = ClassDescription->GetProperties();

		for(int I = 0; I < PropertyCount; I++)
		{
			ZEPropertyDescription Property = Properties[I];

			if (Property.Visibility == false)
				continue;

			QTreeWidgetItem* TopLevelItem;

			TopLevelItem = this->topLevelItem(this->topLevelItemCount()-1);
			TopLevelItem->setExpanded(true);

			RowSize = TopLevelItem->sizeHint(0);
			TopLevelItem->setSizeHint(0, QSize(RowSize.width(),24));
			TopLevelItem->sizeHint(1);
			TopLevelItem->setSizeHint(1, QSize(RowSize.width(),24));
			TopLevelItem->setFlags(0);
			TopLevelItem->setBackgroundColor(0,QColor(150,150,150));
			TopLevelItem->setBackgroundColor(1,QColor(150,150,150));
			TopLevelFont = TopLevelItem->font(0);
			TopLevelFont.setBold(true);
			TopLevelItem->setFont(0,TopLevelFont);
			TopLevelItem->setForeground(0,QBrush(QColor(255,255,255)));


			bool GroupExists = false;

			for(int I = 0; I < PropertyCount; I++)
			{
				GroupExists = false;

				if(Properties[I].GroupName != NULL)
				{
					for(int J = 0; J < Groups.count(); J++)
					{
						if(Groups[J]->GroupName == QString(Properties[I].GroupName))
							GroupExists = true;
					}

					if(GroupExists == false)
					{
						Groups.append(new ZEDPropertyWindowManagerGroup(QString(Properties[I].GroupName), TopLevelItem));
					}
				}
			}

			switch(Property.Type)
			{
				case ZE_VRT_BOOLEAN:

					if(Property.GroupName == NULL)
					{
						QTreeWidgetItem* Item = new ZEDBoolComboBox(this, TopLevelItem, Class, Property);
						TopLevelItem->addChild(Item);
						((ZEDBoolComboBox*)(Item))->SetType(ZED_PW_TYPE_BOOLCOMBOBOX);
						PropertyWidgetsList.append(((ZEDBoolComboBox*)(Item)));
					}

					else
					{
						QTreeWidgetItem* Item = new ZEDBoolComboBox(this, FindGroup(QString(Property.GroupName)), Class, Property);
						FindGroup(QString(Property.GroupName))->addChild(Item);
						((ZEDBoolComboBox*)(Item))->SetType(ZED_PW_TYPE_BOOLCOMBOBOX);
						PropertyWidgetsList.append(((ZEDBoolComboBox*)(Item)));
					}

					break;

				case ZE_VRT_FLOAT:

					if(Property.GroupName == NULL)
					{
						QTreeWidgetItem* Item = new ZEDSpinBox1Float(this, TopLevelItem, Class, Property);
						TopLevelItem->addChild(Item);
						((ZEDSpinBox1Float*)(Item))->SetType(ZED_PW_TYPE_SPINBOXFLOAT);
						PropertyWidgetsList.append(((ZEDSpinBox1Float*)(Item)));
					}

					else
					{
						QTreeWidgetItem* Item = new ZEDSpinBox1Float(this, FindGroup(QString(Property.GroupName)), Class, Property);
						FindGroup(QString(Property.GroupName))->addChild(Item);
						((ZEDSpinBox1Float*)(Item))->SetType(ZED_PW_TYPE_SPINBOXFLOAT);
						PropertyWidgetsList.append(((ZEDSpinBox1Float*)(Item)));
					}

					break;

				case ZE_VRT_INTEGER:

					if(Property.Enumurators != NULL)
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDEnumComboBox(this, TopLevelItem, Class, Property);
							TopLevelItem->addChild(Item);
							((ZEDEnumComboBox*)(Item))->SetType(ZED_PW_TYPE_ENUMCOMBOBOX);
							PropertyWidgetsList.append(((ZEDEnumComboBox*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDEnumComboBox(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDEnumComboBox*)(Item))->SetType(ZED_PW_TYPE_ENUMCOMBOBOX);
							PropertyWidgetsList.append(((ZEDEnumComboBox*)(Item)));
						}
					}
					
					else
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDSpinBox1Integer(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDSpinBox1Integer*)(Item))->SetType(ZED_PW_TYPE_SPINBOXINT);
							PropertyWidgetsList.append(((ZEDSpinBox1Integer*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDSpinBox1Integer(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDSpinBox1Integer*)(Item))->SetType(ZED_PW_TYPE_SPINBOXINT);
							PropertyWidgetsList.append(((ZEDSpinBox1Integer*)(Item)));
						}
					}

					break;

				case ZE_VRT_VECTOR2:

					if(Property.GroupName == NULL)
					{
						QTreeWidgetItem* Item = new ZEDSpinBox2(this, TopLevelItem, Class, Property); 
						TopLevelItem->addChild(Item);
						((ZEDSpinBox2*)(Item))->SetType(ZED_PW_TYPE_SPINBOX2);
						PropertyWidgetsList.append(((ZEDSpinBox2*)(Item)));
					}

					else
					{
						QTreeWidgetItem* Item = new ZEDSpinBox2(this, FindGroup(QString(Property.GroupName)), Class, Property);
						FindGroup(QString(Property.GroupName))->addChild(Item);
						((ZEDSpinBox2*)(Item))->SetType(ZED_PW_TYPE_SPINBOX2);
						PropertyWidgetsList.append(((ZEDSpinBox2*)(Item)));
					}

					break;

				case ZE_VRT_VECTOR3:

					if(Property.Semantic == ZE_PS_COLOR)
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDColorPickerRGB(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDColorPickerRGB*)(Item))->SetType(ZED_PW_TYPE_COLORPICKERRGB);
							PropertyWidgetsList.append(((ZEDColorPickerRGB*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDColorPickerRGB(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDColorPickerRGB*)(Item))->SetType(ZED_PW_TYPE_COLORPICKERRGB);
							PropertyWidgetsList.append(((ZEDColorPickerRGB*)(Item)));
						}
					}

					else
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDSpinBox3(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDSpinBox3*)(Item))->SetType(ZED_PW_TYPE_SPINBOX3);
							PropertyWidgetsList.append(((ZEDSpinBox3*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDSpinBox3(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDSpinBox3*)(Item))->SetType(ZED_PW_TYPE_SPINBOX3);
							PropertyWidgetsList.append(((ZEDSpinBox3*)(Item)));
						}
					}

					break;

				case ZE_VRT_VECTOR4:

					if(Property.Semantic == ZE_PS_COLOR)
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDColorPickerRGBA(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDColorPickerRGBA*)(Item))->SetType(ZED_PW_TYPE_COLORPICKERRGBA);
							PropertyWidgetsList.append(((ZEDColorPickerRGBA*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDColorPickerRGBA(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDColorPickerRGBA*)(Item))->SetType(ZED_PW_TYPE_COLORPICKERRGBA);
							PropertyWidgetsList.append(((ZEDColorPickerRGBA*)(Item)));
						}
					}

					else
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDSpinBox4(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDSpinBox4*)(Item))->SetType(ZED_PW_TYPE_SPINBOX4);
							PropertyWidgetsList.append(((ZEDSpinBox4*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDSpinBox4(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDSpinBox4*)(Item))->SetType(ZED_PW_TYPE_SPINBOX4);
							PropertyWidgetsList.append(((ZEDSpinBox4*)(Item)));
						}
					}

					break;

				case ZE_VRT_QUATERNION://////////////////////////////////Need Revision

						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDRotationSpinBox3(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDRotationSpinBox3*)(Item))->SetType(ZED_PW_TYPE_ROTATIONSPINBOX3);
							PropertyWidgetsList.append(((ZEDRotationSpinBox3*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDRotationSpinBox3(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDRotationSpinBox3*)(Item))->SetType(ZED_PW_TYPE_ROTATIONSPINBOX3);
							PropertyWidgetsList.append(((ZEDRotationSpinBox3*)(Item)));
						}

					break;

				case ZE_VRT_STRING:

					if(Property.Semantic == ZE_PS_FILENAME)
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDFileBrowser(this, TopLevelItem, Class, Property, WorkingDirectory); 
							TopLevelItem->addChild(Item);
							((ZEDFileBrowser*)(Item))->SetType(ZED_PW_TYPE_FILEBROWSER);
							PropertyWidgetsList.append(((ZEDFileBrowser*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDFileBrowser(this, FindGroup(QString(Property.GroupName)), Class, Property, WorkingDirectory);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDFileBrowser*)(Item))->SetType(ZED_PW_TYPE_FILEBROWSER);
							PropertyWidgetsList.append(((ZEDFileBrowser*)(Item)));
						}
					}
					else
					{
						if(Property.GroupName == NULL)
						{
							QTreeWidgetItem* Item = new ZEDLineEdit(this, TopLevelItem, Class, Property); 
							TopLevelItem->addChild(Item);
							((ZEDLineEdit*)(Item))->SetType(ZED_PW_TYPE_LINEEDIT);
							PropertyWidgetsList.append(((ZEDLineEdit*)(Item)));
						}

						else
						{
							QTreeWidgetItem* Item = new ZEDLineEdit(this, FindGroup(QString(Property.GroupName)), Class, Property);
							FindGroup(QString(Property.GroupName))->addChild(Item);
							((ZEDLineEdit*)(Item))->SetType(ZED_PW_TYPE_LINEEDIT);
							PropertyWidgetsList.append(((ZEDLineEdit*)(Item)));
						}
					}

					break;
			}			
		}
		ClassDescription = ClassDescription->GetParent();
	}

	for (int I = 0; I < model()->rowCount(); I++)
	{
		header()->resizeSection(I,20);
	}
}

void ZEDPropertyWindowManager::UpdatePropertyWidgetValues()
{
	for(int I = 0; I < PropertyWidgetsList.count(); I++)
	{
		PropertyWidgetsList[I]->UpdateValues();
	}
}

void ZEDPropertyWindowManager::EmitPropertyChange()
{
	emit PropertyChangedSignal();
}

void ZEDPropertyWindowManager::DisplayCurveEditor(ZEClass* Class, ZEPropertyDescription Property)
{
	#pragma message (".:IMPLEMENT:. ZEDPropertyWindowManager::DisplayCurveEditor(ZEClass* Class, ZEPropertyDescription Property)")
}

void ZEDPropertyWindowManager::UpdateCurveEditor()
{
	#pragma message (".:IMPLEMENT:.	ZEDPropertyWindowManager::UpdateCurveEditor()")
}

void ZEDPropertyWindowManager::drawRow(QPainter* Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QTreeWidget::drawRow(Painter, Option, Index);
	
	for(int I = 0; I < columnCount(); I++)
	{
		QModelIndex CurrentIndex = Index.sibling(Index.row(), I);
		if(CurrentIndex.isValid())
		{
			if(I == 0)
			{
				QRect GridRect = visualRect(CurrentIndex);
				GridRect.setTopLeft(QPoint(0, GridRect.topLeft().y()));
				//Painter->setPen(QColor(170,170,127));
				Painter->setPen(QColor(170,170,170));
				Painter->drawRect(GridRect);
			}

			else
			{
				QRect GridRect = visualRect(CurrentIndex);
				//Painter->setPen(QColor(170,170,127));
				Painter->setPen(QColor(170,170,170));
				Painter->drawRect(GridRect);
			}
		} 
	}
}

ZEDPropertyWindowManager::~ZEDPropertyWindowManager()
{
	//for(int I = 0; I < PropertyWidgetsList.count(); I++)
	//	delete PropertyWidgetsList[I];

	for(int I = 0; I < Groups.count(); I++)
		delete Groups[I];
}
