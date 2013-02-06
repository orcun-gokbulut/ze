//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecalMaterial.h
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
#ifndef __ZE_DECAL_MATERIAL_H__ 
#define __ZE_DECAL_MATERIAL_H__

#include "ZETypes.h"
#include "ZEMath/ZEVector.h"
#include "ZEMeta/ZEObject.h"
#include "ZERenderer/ZEMaterial.h"
#include "ZEGraphics/ZESamplerState.h"

class ZEDecal;
class ZETexture2D;
class ZEIndexBuffer;
class ZEVertexBuffer;
class ZEVertexDeclaration;
class ZEStaticVertexBuffer;

// TODO:  Add reflection, refraction, environment map
class ZEDecalMaterial : public ZEMaterial
{
	friend class ZEDecal;
	
	protected:
		ZEUInt32						OldMaterialComponents;
		ZEUInt32						MaterialComponents;

		ZEVector3						ProjectionUp;
		ZEVector3						ProjectionPoint;
		ZEVector3						ProjectionDirection;
		
		float							ProjectionWidth;
		float							ProjectionHeight;
		float							ProjectionDepth;

		ZEVector3						DiffuseColor;
		ZEVector3						AmbientColor;
		ZEVector3						SpecularColor;
		ZEVector3						EmissiveColor;

		const ZETexture2D*				DiffuseMap;
		const ZETexture2D*				NormalMap;
		const ZETexture2D*				SpecularMap;
		const ZETexture2D*				OpacityMap;
		const ZETexture2D*				EmissiveMap;

		ZETextureAddressMode			DiffuseMapAddressing;
		ZETextureAddressMode			NormalMapAddressing;
		ZETextureAddressMode			SpecularMapAddressing;
		ZETextureAddressMode			OpacityMapAddressing;
		ZETextureAddressMode			EmissiveMapAddressing;

		ZEMaterialTransparancyMode		TransparancyMode;
		ZEMaterialOpacityComponent		OpacityComponent;

		union
		{
			struct
			{
				ZEVector3				MaterialDiffuseColor;
				float					DiffuseFactor;

				ZEVector3				MaterialAmbientColor;
				float					AmbientFactor;

				ZEVector3				MaterialSpecularColor;
				float					SpecularFactor;

				ZEVector3				MaterialEmissiveColor;
				float					EmissiveFactor;

				float					Opacity;
				float					FadeAmount;
				float					SpecularShininess;
				float					AlphaCullLimit;

			};

			float						MaterialParameters[20];
		};
		
		ZEMatrix4x4						DecalProjectionMatrix;

										ZEDecalMaterial();
		virtual							~ZEDecalMaterial();

	public:
		ZEMaterialFlags					GetMaterialFlags() const;
		
		void							SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode		GetTransparancyMode() const;

		void							SetAlphaCullEnabled(bool Enabled);
		bool							GetAlphaCullEnabled() const;
		void							SetAlphaCullLimit(float Factor);
		float							GetAlphaCullLimit() const;

		// Ambient
		void							SetAmbientEnabled(bool Enabled);
		bool							GetAmbientEnabled() const;
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor() const;
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor() const;

		// Diffuse
		void							SetDiffuseEnabled(bool Enabled);
		bool							GetDiffuseEnabled() const;
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor() const;
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor() const;
		void							SetDiffuseMapEnabled(bool Enabled);
		bool							GetDiffuseMapEnabled() const;
		void							SetDiffuseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDiffuseMap() const;
		void							SetDiffuseMapAddressing(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDiffuseMapAddressing() const;

		// Specular
		void							SetSpecularEnabled(bool Enabled);
		bool							GetSpecularEnabled() const;
		void							SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&				GetSpecularColor() const;
		void							SetSpecularFactor(float Factor);
		float							GetSpecularFactor() const;
		void							SetSpecularShininess(float Shininess);
		float							GetSpecularShininess() const;
		void							SetSpecularMapEnabled(bool Enabled);
		bool							GetSpecularMapEnabled() const;
		void							SetSpecularMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetSpecularMap() const;
		void							SetSpecularMapAddressing(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetSpecularMapAddressing() const;

		// Emissive
		void							SetEmissiveEnabled(bool Enabled);
		bool							GetEmissiveEnabled() const;
		void							SetEmissiveColor(const ZEVector3& Color);
		const ZEVector3&				GetEmissiveColor() const;
		void							SetEmissiveFactor(float Factor);
		float							GetEmissiveFactor() const;
		void							SetEmissiveMapEnabled(bool Enabled);
		bool							GetEmissiveMapEnabled() const;
		void							SetEmissiveMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetEmissiveMap() const;
		void							SetEmissiveMapFile(const char* Filename);
		const char*						GetEmissiveMapFile() const;
		void							SetEmissiveMapAddressing(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmissiveMapAddressing() const;

		// Normal Map
		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled() const;
		void							SetNormalMap(const ZETexture2D*	Texture);
		const ZETexture2D*				GetNormalMap() const;
		void							SetNormalMapAddressing(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressing() const;

		// Opacity
		void							SetOpacityEnabled(bool Enabled);
		bool							GetOpacityEnabled() const;
		void							SetOpacity(float Value);
		float							GetOpacity() const;
		void							SetOpacityComponent(ZEMaterialOpacityComponent Component);
		ZEMaterialOpacityComponent		GetOpacityComponent() const;
		void							SetOpacityMapEnabled(bool Enabled);
		bool							GetOpacityMapEnabled() const;
		void							SetOpacityMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetOpacityMap() const;
		void							SetOpacityMapAddressing(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressing() const;

		static ZEDecalMaterial*			CreateInstance();	
};

#endif
