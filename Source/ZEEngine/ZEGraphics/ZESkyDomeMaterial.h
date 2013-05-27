//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyDomeMaterial.h
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
#ifndef __ZE_SKY_DOME_MATERIAL_H__
#define __ZE_SKY_DOME_MATERIAL_H__

#include "ZEMaterial.h"
#include "ZEMath\ZEVector.h"

class ZECamera;

class ZESkyDomeMaterial : public ZEMaterial
{
	friend class ZED3D9Module;
	friend class ZESkyDome;

	protected:
		float						G;						// Scattering Symmetry input for phase function 

		float						MieConstant;			// Mie scattering constant
		float						RayleighConstant;		// Rayleigh scattering constant

		float						MieScaleDepth;
		float						RayleighScaleDepth;
		
		float						OuterRadius;			// Radius of Sky Dome
		float						InnerRadius;			// Radius of Earth

		float						SunLightIntensity;		// Brightness of sun
		ZEVector3					SunLightDirection;		// Direction of sun
		ZEVector3					SunLightWaveLenght;		// Wave length of sunlight

		ZEVector3					CameraPosition;			// Camera to render from
		ZEVector3					CameraPositionOffset;	// Offset to render with static camera position


									ZESkyDomeMaterial();
		virtual						~ZESkyDomeMaterial();

	private:

	public:
		ZEMaterialFlags				GetMaterialFlags() const;
		static ZESkyDomeMaterial*	CreateInstance();

};

#endif
