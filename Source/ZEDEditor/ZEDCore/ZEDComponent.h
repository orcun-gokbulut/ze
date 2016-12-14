//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDComponent.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"
#include "ZEDestroyable.h"

class ZEDEditor;
class ZEDEditorEvent;
class ZEDEvent;
class ZEDObjectEvent;
class ZEDTickEvent;
class ZEDSelectionEvent;
class ZEDTransformationEvent;
class ZEDViewportKeyboardEvent;
class ZEDViewportMouseEvent;
class ZEDViewportChangedEvent;
class ZEDViewportRenderEvent;
class ZEDAssetEvent;

class ZEDComponent : public ZEObject, public ZEInitializable, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEDEditor;
	private:
		ZEDEditor*						Editor;

	protected:
		virtual void					EventReceived(const ZEDEvent* Event);
	
		virtual void					EditorEvent(const ZEDEditorEvent* Event);
		virtual void					ObjectEvent(const ZEDObjectEvent* Event);
		virtual void					SelectionEvent(const ZEDSelectionEvent* Event);
		virtual void					TransformationEvent(const ZEDTransformationEvent* Event);
		virtual	void					TickEvent(const ZEDTickEvent* Event);
		virtual void					ViewportKeyboardEvent(const ZEDViewportKeyboardEvent* Event);
		virtual void					ViewportMouseEvent(const ZEDViewportMouseEvent* Event);
		virtual void					ViewportChangedEvent(const ZEDViewportChangedEvent* Event);
		virtual void					ViewportRenderEvent(const ZEDViewportRenderEvent* Event);
		virtual void					AssetEvent(const ZEDAssetEvent& Event);

		void							RaiseEvent(const ZEDEvent* Event);
	
										ZEDComponent();
		virtual							~ZEDComponent();

	public:
		ZEDEditor*						GetEditor() const;
};
