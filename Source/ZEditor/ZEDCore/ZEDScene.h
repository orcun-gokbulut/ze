//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDScene.h
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

#include "ZEDObjectWrapper.h"
#include "ZEGame/ZEScene.h"

class ZEObject;

class ZEDScene : public ZEScene
{
	private:
		ZESmartArray<ZEDObjectWrapper*> Wrappers;
		void Tick(ZEDObjectWrapper* Wrapper, float ElapsedTime);

	protected:
		void AddWrapper(ZEObject* Object);
		void RemoveWrapper(ZEObject* Object);

		ZEDScene();

	public:
		const ZESmartArray<ZEDObjectWrapper*>& GetWrappers();
		ZEArray<ZEDObjectWrapper*> GetWrappers(ZEClass* Class);
		ZEDObjectWrapper* GetWrapper(ZEObject* Object);
		
		virtual void AddEntity(ZEEntity* Entity);
		virtual void RemoveEntity(ZEEntity* Entity);

		virtual bool Initialize();
		virtual void Deinitialize();
					 
		virtual void Tick(float ElapsedTime);
		virtual void Render(float ElapsedTime);
		virtual bool RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);
		
		static ZEDScene* CreateInstance();
};
