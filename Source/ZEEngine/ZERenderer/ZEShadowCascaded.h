//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShadowCascaded.h
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

#ifndef __ZE_SHADOW_CASCADED_H__
#define __ZE_SHADOW_CASCADED_H__

#include "ZETypes.h"
#include "ZEShadow.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEViewCuboid.h"


class ZETexture2D;
class ZERenderTarget;

struct ZECascade
{
	float				Start;
	float				End;
	float				Depth;

	ZETexture2D*		ShadowMap;
	ZERenderTarget*		RenderTarget;
	ZEViewCuboid		ShadowView;
	ZEMatrix4x4			ShadowTransform;
};

class ZELight;
class ZECamera;
class ZEDrawParameters;

#define ZE_SHADOW_MAX_CASCADE_COUNT		4

class ZEShadowCascaded : public ZEShadow
{
	friend class ZELightDirectional;

	protected:
		float						SplitBias;
		ZEUInt						CascadeCount;
		float						ShadowViewDepth;

		ZECascade					Cascades[ZE_SHADOW_MAX_CASCADE_COUNT];

		void						UpdateBuffers();
		void						DestroyBuffers();

									ZEShadowCascaded();
		virtual						~ZEShadowCascaded();

	public:
		void						SetCascadeCount(ZEUInt Value);
		ZEUInt						GetCascadeCount() const;

		void						SetSplitBias(float Value);
		float						GetSplitBias() const;

		void						SetShadowViewDepth(float Value);
		float						GetShadowViewDepth() const;

		const ZECascade*			GetCascade(ZESize Index) const;

		virtual ZEUInt				GetShadowVolumeCount() const;
		virtual const ZEViewVolume*	GetShadowVolume(ZESize Index) const;

		virtual bool				Initialize(ZELight* Light);
		virtual void				Deinitialize();

		virtual void				Update(ZECamera* Camera);
		virtual void				Draw(ZEDrawParameters* LightDrawParameters);
};

#endif
