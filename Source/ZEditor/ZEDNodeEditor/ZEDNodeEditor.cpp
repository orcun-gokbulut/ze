//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditor.cpp
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

#include <QPixmap>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <..\src\corelib\tools\qdatetime.h>

#include "ui_zednodeeditor.h"
#include "ZEDNodeEditor.h"
#include "ZEDNodeEditorGraphicsView.h"
#include "ZEDIOPortConnection.h"
#include "ZEDNodeEditorNode.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorMiniMap.h"
#include "ZETypes.h"


ZEDNodeEditor::ZEDNodeEditor(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui->setupUi(this);
	View = new ZEDNodeEditorGraphicsView(this);

	gridLayout = new QGridLayout;
	gridLayout->addWidget(View);
	setLayout(gridLayout);

	View->setVisible(true);

	QList<QAction*> ViewCustomContextList;
	QAction* AddNodeAction = new QAction("Add Node", NULL);

	ViewCustomContextList.append(AddNodeAction);
	View->AddCustomViewContextMenuItems(ViewCustomContextList);

	QObject::connect(View, SIGNAL(OnConnectionAdding(ZEDIOPortConnection* , bool&)), this, SLOT(ConnectionAddingControl(ZEDIOPortConnection* , bool& )));
	QObject::connect(View, SIGNAL(OnIOPortSelection(ZEDNodeIOPort*)), this, SLOT(IOPortSelectionControl(ZEDNodeIOPort*)));
	QObject::connect(View, SIGNAL(OnNodeSelection(QList<ZEDNodeEditorNode*>*)), this, SLOT(NodeSelectionControl(QList<ZEDNodeEditorNode*>*)));
	QObject::connect(View, SIGNAL(OnNodeDeleting(QList<ZEDNodeEditorNode*>*, bool&)), this, SLOT(NodeDeletingControl(QList<ZEDNodeEditorNode*>*, bool&)));
	QObject::connect(View, SIGNAL(OnNodeDeleted(QList<ZEDNodeEditorNode*>*)), this, SLOT(NodeDeletedControl(QList<ZEDNodeEditorNode*>*)));
	QObject::connect(View, SIGNAL(OnConnectionDeleting(QList<ZEDIOPortConnection*>*, bool&)), this, SLOT(ConnectionDeletingControl(QList<ZEDIOPortConnection*>*, bool&)));
	QObject::connect(View, SIGNAL(OnConnectionDeleted(QList<ZEDIOPortConnection*>*)), this, SLOT(ConnectionDeletedControl(QList<ZEDIOPortConnection*>*)));
	QObject::connect(View, SIGNAL(OnConnectionSelection(QList<ZEDIOPortConnection*>*)), this, SLOT(ConnectionSelectionControl(QList<ZEDIOPortConnection*>*)));
	QObject::connect(View, SIGNAL(OnConnectionAdded(ZEDIOPortConnection*)), this, SLOT(ConnectionAddedControl(ZEDIOPortConnection*)));
	QObject::connect(View->GetCustomViewContextMenuItems()->value(0), SIGNAL(triggered()), this, SLOT(AddNode()));
	Test();
}

void ZEDNodeEditor::AddNode()
{
	ZEDNodeEditorNode* NewNode = new ZEDNodeEditorNode(NULL, View, ZED_IOPA_IO_TYPE_ORDER);

	QString str;
	qsrand(QTime::currentTime().minute());

	ZEDNodeIOPort* AddPort;

	for (ZEInt I = 0; I < 30; I++)
	{
		str.setNum(qrand());
		if(I % 2 == 0)
			AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUT, str, str);
		else if(I % 3 == 0)
			AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_OUTPUT,  str, str);
		else
			AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUTOUTPUT,  str, str);

		NewNode->AddIOPort(AddPort);
	}

	NewNode->SetPreviewPixmapVisibility(true);
	View->setInteractive(true);
	View->AddNode(NewNode, View->mapToScene(QCursor::pos()));

}

void ZEDNodeEditor::ConnectionAddingControl(ZEDIOPortConnection* Connection, bool& ConnectionAddingValidation)
{
}
void ZEDNodeEditor::IOPortSelectionControl(ZEDNodeIOPort* SelectedIOPort)
{/*
	qDebug() << "-------------------";
	qDebug() << SelectedIOPort;*/
}
void ZEDNodeEditor::NodeSelectionControl(QList<ZEDNodeEditorNode*>*	SelectedNodes)
{/*
	qDebug() << "-------------------";
	qDebug() << SelectedNodes;*/
}
void ZEDNodeEditor::NodeDeletingControl(QList<ZEDNodeEditorNode*>*	DeletingSelectedNodes, bool& DeletingNodesValidation)
{/*
	ZEInt counter = 0;

	for (ZEInt I = 0; I < View->GetConnections()->count(); I++)
	{
		for (ZEInt J = 0; J < DeletingSelectedNodes->count(); J++)
		{
			if (View->GetConnections()->value(I)->GetFromNode() == DeletingSelectedNodes->value(J) || View->GetConnections()->value(I)->GetToNode() == DeletingSelectedNodes->value(J))
				counter ++;
		}
	}


	if (counter >= 5)
	{
		DeletingNodesValidation = false;
	}
	else
		DeletingNodesValidation = true;*/
}
void ZEDNodeEditor::NodeDeletedControl(QList<ZEDNodeEditorNode*>*	DeletedSelectedNodes)
{/*
	qDebug() << "-------------------";
	qDebug() << DeletedSelectedNodes;*/

}
void ZEDNodeEditor::ConnectionDeletingControl(QList<ZEDIOPortConnection*>* DeletingSelectedConnections, bool& DeletingConnectionsValidation)
{
	
}
void ZEDNodeEditor::ConnectionDeletedControl(QList<ZEDIOPortConnection*>* DeletedSelectedConnections)
{/*
	qDebug() << "-------------------";
	qDebug() << DeletedSelectedConnections;*/
}
void ZEDNodeEditor::ConnectionSelectionControl(QList<ZEDIOPortConnection*>* SelectedConnections)
{/*
	qDebug() << "-------------------";
	qDebug() << SelectedConnections;*/
}
void ZEDNodeEditor::ConnectionAddedControl(ZEDIOPortConnection* ConnectionAdded)
{/*
	qDebug() << "-------------------";
	qDebug() << "ConnectionAdded at" << ConnectionAdded;*/
}
ZEDNodeEditor::~ZEDNodeEditor()
{

}

void ZEDNodeEditor::Test()
{
	ZEDNodeEditorMinimap* MiniMapView = new ZEDNodeEditorMinimap(View, View->GetScene(), View);
	for (ZEInt I = 0; I < 2; I++)
	{
		ZEDNodeEditorNode* NewNode = new ZEDNodeEditorNode(NULL, View, ZED_IOPA_IO_TYPE_ORDER);
		
		QString str;
		qsrand(QTime::currentTime().minute());

		ZEDNodeIOPort* AddPort;
		
		for (ZEInt I = 0; I < 30; I++)
		{
			str.setNum(qrand());
			if(I % 2 == 0)
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUT, str, str);
			else if(I % 3 == 0)
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_OUTPUT,  str, str);
			else
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUTOUTPUT,  str, str);

			NewNode->AddIOPort(AddPort);
		}

		NewNode->SetPreviewPixmapVisibility(true);
		NewNode->SetNameText("aaaaa");
		NewNode->SetTypeText("ccccccc");
		//QPixmap PicPixmap("Images/ts.png");
		//NewNode->SetPreviewPixmap(PicPixmap);
		View->setInteractive(true);
		View->AddNode(NewNode, QPointF(200,20));

		NewNode = new ZEDNodeEditorNode(NULL, View, ZED_IOPA_ALPABETICAL_ORDER);
		for (ZEInt I = 0; I < 30; I++)
		{
			str.setNum(qrand());
			if(I % 2 == 0)
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUT, str, str);
			else if(I % 3 == 0)
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_OUTPUT, str, str);
			else
				AddPort = new ZEDNodeIOPort(NewNode, ZED_NIOPT_INPUTOUTPUT, str, str);

			NewNode->AddIOPort(AddPort);
		}

		NewNode->SetPreviewPixmapVisibility(false);
		View->setInteractive(true);
		View->AddNode(NewNode, QPointF(50,20));

		ZEDNodeEditorNode* NewNode1 = new ZEDNodeEditorNode(NULL, View, ZED_IOPA_ALPABETICAL_ORDER);
		for (ZEInt I = 0; I < 30; I++)
		{
			str.setNum(qrand());
			if(I % 2 == 0)
				AddPort = new ZEDNodeIOPort(NewNode1, ZED_NIOPT_INPUT, str, str);
			else if(I % 3 == 0)
				AddPort = new ZEDNodeIOPort(NewNode1, ZED_NIOPT_OUTPUT, str, str);
			else
				AddPort = new ZEDNodeIOPort(NewNode1, ZED_NIOPT_INPUTOUTPUT, str, str);

			NewNode1->AddIOPort(AddPort);
		}

		NewNode1->SetPreviewPixmapVisibility(true);
		View->setInteractive(true);
		View->AddNode(NewNode1, QPointF(-50,20));

	}
}
