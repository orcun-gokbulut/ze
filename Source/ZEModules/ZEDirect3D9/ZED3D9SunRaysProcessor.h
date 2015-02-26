//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SunRaysProcessor.h
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

#ifndef __ZE_D3D9_SUN_RAYS_H__
#define __ZE_D3D9_SUN_RAYS_H__

#include "ZED3D9ComponentBase.h"
#include "ZEMath\ZEVector.h"
#include "ZEMeta\ZEObject.h"

class ZED3D9VertexShader;
class ZED3D9PixelShader;
class ZED3D9Texture2D;
class ZED3D9ViewPort;

ZE_META_OBJECT_DESCRIPTION(ZED3D9SunRaysProcessor);

class ZED3D9SunRaysProcessor : public ZEObject, public ZED3D9ComponentBase
{
	ZE_META_OBJECT(ZED3D9SunRaysProcessor);
	private:
		bool								Enabled;
		ZED3D9Texture2D*					Input;
		ZED3D9ViewPort*						Output;
		ZEVector3							SunDirection;
		float								Intensity;
		float								Density;
		bool								DebugOutput;

		ZED3D9VertexShader*					VertexShader;
		ZED3D9PixelShader*					PixelShader;
		LPDIRECT3DVERTEXDECLARATION9		VertexDeclaration;

	public:
		void								SetEnabled(bool Value);
		bool								GetEnabled() const;

		void								SetInput(ZED3D9Texture2D* Input);
		ZED3D9Texture2D*					GetInput();

		void								SetOutput(ZED3D9ViewPort* Output);
		ZED3D9ViewPort*						GetOutput();
		
		void								SetSunDirection(const ZEVector3& SunDirection);
		const ZEVector3&					GetSunDirection();
		
		void								SetSunDirectionFromScene();
		
		void								SetDebugOutput(bool Value);
		bool								GetDebugOutput() const;

		void								SetIntensity(float Intensity);
		float								GetIntensity() const;

		void								SetDensity(float Density);
		float								GetDensity() const;

		void								Initialize();
		void								Deinitialize();

		void								Process();

											ZED3D9SunRaysProcessor();
											~ZED3D9SunRaysProcessor();
};

/*
ZE_POST_PROCESSOR_START(Meta)
	<zinek>
		<meta> 
			<class name="ZED3D9SunRaysProcessor">
				<noinstance>true</noinstance>
				<description>ZED3D9SunRaysProcessor</description>
				<property name="Enabled" type="boolean" autogetset="yes" description="..."/>
				<property name="Intensity" type="float" autogetset="yes" description="..."/>
				<property name="Density" type="float" autogetset="yes" description="..."/>
				<property name="DebugOutput" type="boolean" autogetset="yes" description="..."/>
			</class>
		</meta>
	</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
