//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyDome.h
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

#include "ZEEntity.h"
#include "ZEGraphics\ZECanvas.h"
#include "ZEGraphics\ZERenderCommand.h"
#include "ZETypes.h"

class ZESkyDomeMaterial;
class ZEMaterial;
class ZEScene;

class ZESkyDome : public ZEEntity
{
	protected:
		float					G;					

		float					AmbientFactor;		
		ZEVector4				AmbientColor;		

		float					MieConstant;		
		float					RayleighConstant;	

		float					MieScaleDepth;
		float					RayleighScaleDepth;

		float					OuterRadius;		
		float					InnerRadius;		

		float					SunIntensity;		

		ZEVector3				SunPosition;		
		ZEVector3				WaveLenght;			

		ZEVector3				CameraPosition;		
		ZEVector3				CameraPositionOffset;
		

		ZECanvas				SkyDomeGeometry;
		ZESkyDomeMaterial*		SkyDomeMaterial;
		ZERenderCommand			SkyDomeRenderCommand;

								ZESkyDome();
		virtual					~ZESkyDome();

	private:
		
	public:
		void					SetAmbientFactor(float Value);
		float					GetAmbientFactor() const;

		void					SetAmbientColor(ZEVector4& Color);
		ZEVector4				SetAmbientColor() const;

		void					SetSetMieConstant(float Value);
		float					GetGetMieConstant() const;

		void					SetRayleighConstant(float Value);
		float					GetRayleighConstant() const;

		void					SetMieScaleDepth(float Value);
		float					GetMieScaleDepth() const;

		void					SetRayleighScaleDepth(float Value);
		float					GetRayleighScaleDepth() const;

		void					SetSunIntensity(float Value);
		float					GetSunIntensity() const;

		void					SetSunPosition(ZEVector3& Value);
		ZEVector3				GetSunPosition() const;

		void					SetWaveLenght(ZEVector3& Value);
		ZEVector3				GetWaveLenght() const;

		void					SetOuterRadius(float Value);
		float					GetOuterRadius() const;

		void					SetInnerRadius(float Value);
		float					GetInnerRadius() const;

		void					SetCameraPosition(ZEVector3 Value);
		ZEVector3				GetCameraPosition() const;

		void					SetCameraPositionOffset(ZEVector3 Value);
		ZEVector3				GetCameraPositionOffset() const;
		
		void					SetG(float Value);
		float					GetG() const;

		virtual ZEUInt32		GetDrawFlags() const;

		virtual bool			Initialize();
		virtual void			Deinitialize();

		virtual void			Draw(ZEDrawParameters* DrawParameters);
		virtual void			Tick(float Time);

		static ZESkyDome*		CreateInstance();
};
