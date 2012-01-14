//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSceneList.cpp
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

#include "ZEDSceneList.h"
#include <QStringList>
#include <QTreeWidgetItem>
/************************************************************************/
/*								 TreeItems					            */
/************************************************************************/

ZEDSceneListItem::ZEDSceneListItem(ZEEntity* Entity)
{
	this->Entity = Entity;
	setText(1, QString(Entity->GetName()));
	setText(0, QString().setNum(Entity->GetEntityId()));
	//setFlags(flags() | Qt::ItemIsUserCheckable);

	//if(Entity->GetVisible())
	//	setCheckState(2, Qt::Checked);
	//else
	//	setCheckState(2, Qt::Unchecked);
}

/************************************************************************/
/*								 GroupItems					            */
/************************************************************************/

ZEDSceneListGroup::ZEDSceneListGroup(QString GroupName)
{
	TreeWidgetItem = new QTreeWidgetItem();
	this->GroupName = GroupName;
	TreeWidgetItem->setText(0, GroupName);

	//TreeWidgetItem->setFlags(TreeWidgetItem->flags() | Qt::ItemIsUserCheckable);
	//TreeWidgetItem->setCheckState(2, Qt::Unchecked);
}
/*
void ZEDSceneListGroup::ToogleVisibilities()
{
	Qt::CheckState IsChecked = TreeWidgetItem->checkState(2);

	for (int I = 0; I < TreeWidgetItem->childCount(); I++)
	{
		TreeWidgetItem->child(I)->setCheckState(2, IsChecked);
	}
}*/

/************************************************************************/
/*			               SceneListTreeWidget		                    */
/************************************************************************/

void ZEDSceneListTreeWidget::GenerateTree()
{	
	for (int I = 0; I < this->Groups.count(); I++)
	{
		ZEDSceneListGroup* CurrentItem =  Groups[I];
		QObject::disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), CurrentItem, SLOT(ToogleVisibilities()));
		delete CurrentItem;
	}
	Groups.clear();
	clear();

	QStringList TempList;
	QString Name, Id;
	TempList << "Id" << "Name" << "Visible";
	setHeaderLabels(TempList);
	TempList.clear();

	ZEEntity*	CurrentEntity;///////////////////

	bool GroupExists = false;

	for (size_t I = 0; I < Scene->GetEntities().GetCount(); I++)
	{
		CurrentEntity = Scene->GetEntities().GetItem(I);
		GroupExists = false;

		if (QString(CurrentEntity->GetName()) == QString(".:SELECTION__ITEM:."))
			continue;

		for (int J = 0; J < Groups.count(); J++)
		{
			if (Groups[J]->GroupName == QString(CurrentEntity->GetDescription()->GetName()))
				GroupExists = true;
		}

		if (GroupExists == false)
		{
			ZEDSceneListGroup* NewGroup = new ZEDSceneListGroup(QString(CurrentEntity->GetDescription()->GetName()));
			//QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), NewGroup, SLOT(ToogleVisibilities()));
			Groups.append(NewGroup);
		}
	}

	for (int I = 0; I < Groups.count(); I++)
	{
		addTopLevelItem(Groups[I]->TreeWidgetItem);
	}

	for(size_t I = 0 ; I < Scene->GetEntities().GetCount(); I++)
	{
		CurrentEntity = Scene->GetEntities().GetItem(I);

		if (QString(CurrentEntity->GetName()) == QString(".:SELECTION__ITEM:."))
			continue;
		
		for (int J = 0; J < Groups.count(); J++)
		{
			if (Groups[J]->GroupName == QString(CurrentEntity->GetDescription()->GetName()))
				Groups[J]->TreeWidgetItem->addChild(new ZEDSceneListItem(CurrentEntity));
		}
	}
}

ZEDSceneListTreeWidget::ZEDSceneListTreeWidget(ZEScene* Scene, QWidget* Parent) : QTreeWidget(Parent)
{
	this->Scene = Scene;
	setSelectionMode(ExtendedSelection);
	setAlternatingRowColors(true);
}

/************************************************************************/
/*			                    SceneList                               */
/************************************************************************/

ZEDSceneList::ZEDSceneList(ZEScene* Scene, QWidget* Parent, Qt::WindowFlags Flags) : QDockWidget(Parent, Flags)
{
	this->Scene = Scene;

	setFeatures(DockWidgetClosable | DockWidgetFloatable | DockWidgetMovable);
	setWindowTitle(QString(" Scene List"));

	ContentsWidget = new QWidget(this);
	ContentsWidgetLayout = new QVBoxLayout(ContentsWidget);
	ContentsWidgetLayout->setContentsMargins(0,0,0,0);
	ContentsWidget->setLayout(ContentsWidgetLayout);
	setWidget(ContentsWidget);
	SceneTree = new ZEDSceneListTreeWidget(Scene, ContentsWidget);
	ContentsWidgetLayout->addWidget(SceneTree);
	SceneTree->GenerateTree();

	//setFloating(true);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

	show();
}

void ZEDSceneList::Update() 
{
	SceneTree->GenerateTree();
}

void ZEDSceneList::AddItem(ZEEntity* Entity)
{
	QString	CorespondingGroupName = QString(Entity->GetDescription()->GetName());
	bool GroupExsistFlag = false;

	for (int I = 0; I < SceneTree->Groups.count(); I++)
	{
		if (SceneTree->Groups[I]->TreeWidgetItem->text(0) == CorespondingGroupName)
		{
			SceneTree->Groups[I]->TreeWidgetItem->addChild(new ZEDSceneListItem(Entity));
			GroupExsistFlag = true;
			break;
		}
	}

	if (GroupExsistFlag == false)
	{
		ZEDSceneListGroup* NewGroup = new ZEDSceneListGroup(CorespondingGroupName);
		SceneTree->Groups.append(NewGroup);
		SceneTree->addTopLevelItem(NewGroup->TreeWidgetItem);
		NewGroup->TreeWidgetItem->setExpanded(true);
		NewGroup->TreeWidgetItem->addChild(new ZEDSceneListItem(Entity));
	}
}

void ZEDSceneList::RemoveItem(ZEEntity* Entity)
{
	for (int I = 0; I < SceneTree->Groups.count(); I++)
	{
		int GroupChildCount = SceneTree->Groups[I]->TreeWidgetItem->childCount();

		for(int J = 0; J < GroupChildCount; J++)
		{
			ZEDSceneListItem* CurrentItem = ((ZEDSceneListItem*)(SceneTree->Groups[I]->TreeWidgetItem->child(J)));

			if(CurrentItem->Entity == Entity)
			{
				SceneTree->Groups[I]->TreeWidgetItem->removeChild(CurrentItem);
				delete CurrentItem;
				GroupChildCount--;

				if(GroupChildCount == 0)
				{
					SceneTree->invisibleRootItem()->removeChild(SceneTree->Groups[I]->TreeWidgetItem);
					delete SceneTree->Groups[I]->TreeWidgetItem;
					SceneTree->Groups.removeAt(I);
				}
			}
		}
	}
}

void ZEDSceneList::ClearAllSelections()
{
	for(int I = 0; I < SceneTree->selectedItems().count(); I++)
		SceneTree->selectedItems().at(I)->setSelected(false);
}
