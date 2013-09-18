//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialLightDirectional.h
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


#ifndef __ZE_MATERIAL_LIGHT_DIRECTIONAL_H__
#define __ZE_MATERIAL_LIGHT_DIRECTIONAL_H__

#include "ZETypes.h"
#include "ZEMaterial.h"

class ZEShader;
class ZERenderStage;
class ZEVertexBuffer;
class ZERenderCommand;
class ZEConstantBuffer;

class ZEMaterialLightDirectional : public ZEMaterial
{
	friend class ZELightDirectional;

	protected:
		ZEShader*					VertexShader;
		ZEShader*					PixelShader;

		ZEConstantBuffer*			LightTransformations;
		ZEConstantBuffer*			LightProperties;

		void						UpdateShaders();
		void						UpdateBuffers();

		void						DestroyShaders();
		void						DestroyBuffers();

		bool						SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

									ZEMaterialLightDirectional();
		virtual						~ZEMaterialLightDirectional();

	public:
		struct Transformations
		{
			ZEMatrix4x4				InvProjectionMatrix;
		};

		struct Properties
		{
			ZEVector3				Color;
			float					Intensity;
			ZEVector3				Direction;
			float					PenumbraSize;
			float					BiasDepthScaled;
			float					BiasSlopeScaled;
		};

		virtual ZESize				GetHash() const;

		virtual bool				SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		static ZEMaterialLightDirectional*	CreateInstance();
};

#endif
