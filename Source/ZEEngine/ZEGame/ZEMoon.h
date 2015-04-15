//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMoon.h
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
#ifndef __ZE_MOON_H__
#define __ZE_MOON_H__

#include "ZETypes.h"
#include "ZEEntity.h"
#include "ZEGraphics\ZERenderCommand.h"

ZE_META_FORWARD_DECLARE(ZEMoonMaterial, "ZEGraphics/ZEMoonMaterial.h")
ZE_META_FORWARD_DECLARE(ZETexture3DResource, "ZETexture/ZETexture3DResource.h")

class ZEMoon : public ZEEntity
{
	ZE_OBJECT

	protected:
		ZEMoonMaterial*				Material;
		ZETexture3DResource*		Texture;
		ZERenderCommand				RenderCommand;

		float						Phase;
		float						Scale;
		float						Rotation;
		float						AmbientFactor;
		ZEVector3					AmbientColor;
		ZEVector3					Direction;

		virtual bool				InitializeSelf();
		virtual bool				DeinitializeSelf();

									ZEMoon();
		virtual						~ZEMoon();

	private:

	public:
		virtual void				SetPhase(float Phase);
		float						GetPhase() const;

		virtual void				SetScale(float Scale);
		float						GetScale() const;
		
		virtual void				SetRotation(float Rotation);
		float						GetRotation() const;

		virtual void				SetAmbientFactor(float Value);
		float						GetAmbientFactor() const;
		
		virtual void				SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&			GetAmbientColor() const;

		virtual void				SetDirection(const ZEVector3& NewDirection);
		const ZEVector3&			GetGetDirection() const;

		virtual void				SetTexture(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount);
		const ZEString				GetTexture() const;

		virtual ZEDrawFlags			GetDrawFlags() const;

		virtual void				Draw(ZEDrawParameters* DrawParameters);
		virtual void				Tick(float Time);

		static ZEMoon*				CreateInstance();
};

#endif // __ZE_MOON_H__
