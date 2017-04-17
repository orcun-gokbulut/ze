//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGeographicEntity.h
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

#include "ZEGame/ZEEntity.h"
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEMatrixd.h"
#include "ZEExport.ZEEngine.h"

class ZE_EXPORT_ZEENGINE ZEGeographicEntity : public ZEEntity
{
	friend class ZESectorManager;
	ZE_OBJECT;
	private:
		ZELink<ZEGeographicEntity>			GeoLink;
		mutable ZEFlags						GeographicEntityDirtyFlags;

		ZEVector3d							GeographicPosition;
		ZEQuaternion						GeographicRotation;
		ZEVector3d							GeographicScale;

		mutable ZEMatrix4x4d				GeographicTransform;
		mutable ZEMatrix4x4d				InvGeographicTransform;

	protected:
		virtual void						GeographicTransformChanged();

											ZEGeographicEntity();

	public:
		const ZEMatrix4x4d&					GetGeographicTransform() const;
		const ZEMatrix4x4d&					GetInvGeographicTransform() const;

		virtual void						SetGeographicPosition(const ZEVector3d& Position);
		ZEVector3d							GetGeographicPosition() const;

		virtual void						SetGeographicRotation(const ZEQuaternion& Rotation);
		ZEQuaternion						GetGeographicRotation() const;

		virtual void						SetGeographicScale(const ZEVector3d& Scale);
		ZEVector3d							GetGeographicScale() const;

		static ZEGeographicEntity*			CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEGeographicEntity.png");
