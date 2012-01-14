//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMaterialEditorViewPort.h
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
#ifndef __ZED_MATERIAL_EDITOR_VIEWPORT_H__
#define __ZED_MATERIAL_EDITOR_VIEWPORT_H__

#include <QFrame>
#include <ZEGame/ZEPlayer.h>
#include <ZEGame/ZEGrid.h>
#include <ZEModel/ZEModel.h>
#include <ZEGraphics/ZEDirectionalLight.h>
#include <QPoint>

class ZEDMaterialEditor;

class ZEDMaterialEditorViewPort : public QFrame
{
	private:

		ZEPlayer*			Camera;
		ZEGrid*				Grid;
		ZEModel*			Model;
		ZEDirectionalLight* DirectLight1;
		ZEDirectionalLight* DirectLight2;
		ZEDirectionalLight* DirectLight3;

		ZEDMaterialEditor*	ParentEditor;

		QPoint				OldMousePosition;

		float				Pitch;
		float				Yawn;
		float				Roll;

		float				YawnLight1;
		float				PitchLight1;

		float				YawnLight2;
		float				PitchLight2;

		float				YawnLight3;
		float				PitchLight3;
		

	protected:

		virtual void		resizeEvent(QResizeEvent* ResizeEvent);
		virtual void		mouseMoveEvent(QMouseEvent * Event);
		virtual void		mousePressEvent(QMouseEvent * Event);
		virtual void		wheelEvent(QWheelEvent * Event);

		virtual void		RotateModel(QPoint PositionDifference);
		virtual void		RotateLights(QPoint PositionDifference);
		virtual void		MoveLeftRight(QPoint PositionDifference);

	public:

		void				Initialize();
		ZEFixedMaterial*	GetModelMaterial();
		void				SetModelFile(const char* FileName);
		ZEDirectionalLight*	GetDirectLight1();
		ZEDirectionalLight*	GetDirectLight2();
		ZEDirectionalLight*	GetDirectLight3();
							ZEDMaterialEditorViewPort(ZEDMaterialEditor* ParentEditor, QWidget* parent = 0, Qt::WindowFlags f = 0);
};


#endif
