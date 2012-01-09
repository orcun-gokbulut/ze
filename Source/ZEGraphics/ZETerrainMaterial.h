//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainMaterial.h
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
#ifndef __ZE_TERRAIN_MATERIAL_H__ 
#define __ZE_TERRAIN_MATERIAL_H__

#include "ZEMaterial.h"
#include "ZEMath/ZEVector.h"
#include "ZEDefinitions.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEClass.h"

ZE_META_CLASS_DESCRIPTION(ZETerrainMaterial)

class ZETextureCube;
class ZETextureCubeResource;
class ZETexture2D;
class ZETexture2DResource;
class ZETerrainMaterial : public ZEMaterial
{
	ZE_META_CLASS(ZETerrainMaterial)
	protected:
		bool							TwoSided;
		bool							Wireframe;
	
		ZEVector3						AmbientColor;
		float							AmbientFactor;

		ZEVector3						DiffuseColor;
		float							DiffuseFactor;

		float							HeightOffset;
		float							HeightScale;

		union
		{
			struct
			{
				ZEVector3				AmbientColorSC;
				float					Reserved1;

				ZEVector3				DiffuseColorSC;
				float					Reserved2;
			};
			float						PixelShaderConstants[8];
		};


		ZETexture2D*					HeightTexture;
		ZETexture2D*					ColorTexture;
		ZETexture2D*					NormalTexture;

										ZETerrainMaterial();
		virtual							~ZETerrainMaterial();

	public:
		virtual ZEMaterialFlags			GetMaterialFlags() const;

		// Material Options
		void							SetTwoSided(bool Enable);
		bool							GetTwoSided() const;

		void							SetWireframe(bool Enable);
		bool							GetWireframe() const;

		void							SetHeightTexture(ZETexture2D* Texture);
		ZETexture2D*					GetHeightTexture();

		void							SetHeightOffset(float Offset);
		float							GetHeightOffset();

		void							SetHeightScale(float Scale);
		float							GetHeightScale();

		void							SetColorTexture(ZETexture2D* Texture);
		ZETexture2D*					GetColorTexture();

		void							SetNormalTexture(ZETexture2D* Texture);
		ZETexture2D*					GetNormalTexture();

		// Ambient
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor() const;
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor() const;

		// Diffuse
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor() const;
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor() const;

		virtual void					SetZero();

		void							Tick(float ElapsedTime);

		static ZETerrainMaterial*		CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZETerrainMaterial" noinstance="true">
			<description>Material System</description>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif



