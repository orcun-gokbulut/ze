//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEditor.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#pragma once

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

class ZEDMainWindow;

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  ZEDModule.h - Zinek Engine v0.6.2 Source Code                                   //
// -------------------------------------------------------------------------------//
//  Copyright (c) 2007-2010 Y. Orçun GÖKBULUT. All rights reserved.                 //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information belongs to Y. Orçun GÖKBULUT and they are   //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Y. Orçun GÖKBULUT. Storing this file, using this file, changing content  //
//  of this file, copying or duplicating this file, compiling this file, publishing //
//  this file to anykind media or network, editing  this file, selling this file,   //
//  booking this file, reading this file are probited without notice and written    //
//  permision of Y. Orçun GÖKBULUT.                                                 //
//  These terms at above can only be changed by Y. Orçun GÖKBULUT.                  //
//                                                                                  //
//  Contact Information:                                                            //
//     Adress : Zinek Code House & Game Studio                                      //
//              Aydinlar Mahallesi.                                                 //
//              Mimar Sokak. 33/5                                                   //
//              06450  Ankara/TURKEY                                                //
//     Phone  : +90 (533) 734 21 22                                                 //
//     E-Mail : contact@zinekengine.com                                             //
//     WWW    : http://www.zinekengine.com                                          //
//                                                                                  //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Orçun GÖKBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

#include "ZEDS/ZEArray.h"

class ZEDEditorCore;
class ZEDComponent;
class ZEDEvent;
class ZEDObjectManager;
class ZEDOperationManager;
class ZEDSelectionManager;
class ZEDTransformationManager;
class ZEDViewportManager;
class ZEDMainWindow;
class ZEUIManager;

class ZEDEditor : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZEDEditorCore;
	private:
		ZEDEditorCore*						Core;

		ZEArray<ZEDComponent*>				Components;
		ZEDObjectManager*					ObjectManager;
		ZEDOperationManager*				OperationManager;
		ZEDSelectionManager*				SelectionManager;
		ZEDTransformationManager*			TransformationManager;
		ZEDViewportManager*					ViewportManager;
		ZEDMainWindow*						MainWindow;
		ZEUIManager*						UIManager;

	protected:
		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

											ZEDEditor();
											~ZEDEditor();

	public:
		ZEDEditorCore*						GetCore();

		ZEDObjectManager*					GetObjectManager();
		ZEDOperationManager*				GetOperationManager();
		ZEDSelectionManager*				GetSelectionManager();
		ZEDTransformationManager*			GetTransformationManager();
		ZEDViewportManager*					GetViewportManager();
		ZEDMainWindow*						GetMainWindow();

		ZEDComponent*						GetComponent(ZEClass* Class);
		const ZEArray<ZEDComponent*>&		GetComponents();
		void								AddComponent(ZEDComponent* Component);
		void								RemoveComponent(ZEDComponent* Component);

		void								DistributeEvent(const ZEDEvent* Event);

		virtual void						Process(float ElapsedTime);
		virtual void						PostProcess(float ElapsedTime);

		virtual void						New();
		virtual bool						Save(const ZEString& FileName);
		virtual bool						Load(const ZEString& FileName);
		virtual void						Close();

		void								Exit();

		virtual void						Destroy();
};
