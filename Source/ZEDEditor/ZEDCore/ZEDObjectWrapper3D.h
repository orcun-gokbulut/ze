//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectWrapper3D.h
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

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEGraphics/ZEGRCanvas.h"

class ZEGRBuffer;
class ZEUILabel;
class ZEUIControl;
class ZEUIFrameControl;
class ZERNCommand;
class ZERNSimpleMaterial;
class ZERNPreRenderParameters;
class ZERNRenderParameters;
class ZERayCastReport;
class ZERayCastParameters;

class ZEDObjectWrapper3D : public ZEDObjectWrapper
{
	ZE_OBJECT
	friend class ZEDObjectManager;
	private:
		bool										Pickable;
		bool										BoundingBoxVisible;
		bool										NameplateVisible;

		ZEGRCanvas									Canvas;
		ZEHolder<ZEGRBuffer>						VertexBuffer;
		ZEHolder<ZEGRBuffer>						ConstantBuffer;
		ZEHolder<ZERNSimpleMaterial>				Material;
		ZERNCommand									Command;

		ZEUIControl*								Nameplate;
		ZEUIFrameControl*							NameplateIcon;
		ZEUILabel*									NameplateName;
		ZEUILabel*									NameplateClass;

	protected:
		/*void										SetNameplateText(const ZEString& String);
		const ZEString&								GetNameplateText() const;

		void										SetNameplateIconFileName(const ZEString& String);
		const ZEString&								GetNameplateIconFileName();*/

		virtual void								UpdateNameplate();
		virtual void								UpdateGraphics();

		virtual bool								InitializeInternal() override;
		virtual bool								DeinitializeInternal() override;

													ZEDObjectWrapper3D();
		virtual										~ZEDObjectWrapper3D();

	public:
		ZEDObjectWrapper3D*							GetParent3D() const;

		virtual void								SetObject(ZEObject* Object) override;

		virtual void								SetId(ZEInt Id) override;
		virtual void								SetName(const ZEString& Name) override;

		virtual void								SetPosition(const ZEVector3& Position);
		virtual ZEVector3							GetPosition() const;

		virtual void								SetRotation(const ZEQuaternion& Rotation);
		virtual ZEQuaternion						GetRotation() const;

		virtual void								SetScale(const ZEVector3& Scale);
		virtual ZEVector3							GetScale() const;

		const ZEArray<ZEDObjectWrapper3D*>&			GetChildWrapper3Ds();
		virtual bool								AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update) override;

		virtual ZEAABBox							GetBoundingBox() const;
		virtual ZEMatrix4x4							GetWorldTransform() const;

		virtual void								SetVisible(bool Visible);
		virtual bool								GetVisible() const;

		void										SetPickable(bool Pickable);
		bool										GetPickable() const;

		virtual void								SetNameplateVisible(bool Visible);
		bool										GetNameplateVisible() const;

		virtual void								SetBoundingBoxVisible(bool Visible);
		bool										GetBoundingBoxVisible() const;

		virtual void								PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void								Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		virtual void								RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		virtual void								UpdateLocal() override;
}
ZE_META_ATTRIBUTE(ZEDObjectWrapper.TargetClass, "");
