//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditor.h
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

#ifndef ZEDNODEEDITOR_H
#define ZEDNODEEDITOR_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>


class ZEDNodeEditorGraphicsView;
class ZEDNodeEditorItem;
class ZEDIOPortConnection;
class ZEDNodeEditorNode;
class ZEDNodeIOPort;

namespace Ui
{
	class ZEDNodeEditorClass;
}

class ZEDNodeEditor : public QWidget
{
	Q_OBJECT

public:

	ZEDNodeEditor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ZEDNodeEditor();

	void Test();

private:

	Ui::ZEDNodeEditorClass* ui;
	ZEDNodeEditorGraphicsView* View;
	QGridLayout* gridLayout;

	public slots:


		void							IOPortSelectionControl(ZEDNodeIOPort* SelectedIOPort);

		void							NodeSelectionControl(QList<ZEDNodeEditorNode*>*	SelectedNodes);
		void							NodeDeletingControl(QList<ZEDNodeEditorNode*>*	DeletingSelectedNodes, bool& DeletingNodesValidation);
		void							NodeDeletedControl(QList<ZEDNodeEditorNode*>*	DeletedSelectedNodes);

		void							ConnectionDeletingControl(QList<ZEDIOPortConnection*>* DeletingSelectedConnections, bool& DeletingConnectionsValidation);
		void							ConnectionDeletedControl(QList<ZEDIOPortConnection*>* DeletedSelectedConnections);
		void							ConnectionSelectionControl(QList<ZEDIOPortConnection*>* SelectedConnections);
		void							ConnectionAddingControl(ZEDIOPortConnection* Connection, bool& ConnectionAddingValidation);
		void							ConnectionAddedControl(ZEDIOPortConnection* ConnectionAdded);
		
		void							AddNode();

};

#endif // ZEDNODEEDITOR_H
