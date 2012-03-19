//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDebugModule.h
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
#ifndef __ZE_GRAPHICS_DEBUG_MODULE_H__
#define __ZE_GRAPHICS_DEBUG_MODULE_H__

#include "ZECore/ZEApplicationModule.h"
#include "ZEInput/ZEInputMap.h"

class ZEPlayer;
class ZEPointLight;
class ZEOmniProjectiveLight;
class ZEProjectiveLight;
class ZEDirectionalLight;
class ZECanvasBrush;
class ZEModel;
class ZEPortalMap;
class ZESkyBrush;
class ZESkyDome;
class ZEUITextControl;
class ZECloud;
class ZEMoon;

class ZEGraphicsDebugModule : public ZEApplicationModule
{
	private:
		ZEInputMap					InputMap;

		ZEModel*					Model;
		ZEPlayer*					Player;		
		ZEPointLight*				PointLight1;
		ZEPointLight*				PointLight2;
		ZEPointLight*				PointLight3;
		ZEPointLight*				PointLight4;
		ZEPointLight*				PointLight5;
		ZEPointLight*				PointLight6;
		ZEProjectiveLight*			ProjectiveLight0;
		ZEOmniProjectiveLight*		OmniProjectiveLight0;
		
		// Weather system
		ZEPortalMap*				PortalMap;
		ZEDirectionalLight*			SunLight;
		ZEDirectionalLight*			MoonLight;
		ZESkyBrush*					StarMap;
		ZESkyDome*					SkyDome;
		ZECloud*					Cloud;
		ZEMoon*						Moon;

		float						MoonPhase;
		float						CloudCover;
		float						MoonLightIntensity;
		float						SunLightIntensity;

		ZEVector3					MoonDirection;
		ZEVector3					SunDirection;
		ZEVector3					SunMoonRotation;		
		
		float						MoonPhaseChangeMultiplier;
		float						CloudCoverChangeMultiplier;
		ZEVector3					SunMoonRotationMultiplier;

		// System debug info
		ZEUITextControl*			CoordinatesText;
		ZEUITextControl*			SunDirectionText;
		ZEUITextControl*			CloudCoverText;
		ZEUITextControl*			WindSpeedText;

	public:
		virtual bool				Initialize();
		virtual void				Deinitialize();
		virtual void				Process(float ElapsedTime);

		virtual void				ProcessInputs(float ElapsedTime);

		virtual	void				DisplayStatus();
		virtual void				UpdateCloudColor();
		virtual void				IncreaseMoonPhase(float ElapsedTime);
		virtual void				DecreaseMoonPhase(float ElapsedTime);
		virtual void				IncreaseCloudCover(float ElapsedTime);
		virtual void				DecreaseCloudCover(float ElapsedTime);
		virtual void				IncreaseDayTime(float ElapsedTime);
		virtual void				DecreaseDayTime(float ElapsedTime);
		
									ZEGraphicsDebugModule();
		virtual						~ZEGraphicsDebugModule();
};

#endif





