//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlayer.h
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
#ifndef	__ZE_PLAYER_H__
#define __ZE_PLAYER_H__
 
#include "ZEAI/ZEActor.h"
#include "ZEInput/ZEInputMap.h"
#include "ZEAI/ZESteering.h"

class ZESteeringPlayerFree : public ZESteering
{
	private:
		
		ZEInputMap			InputMap;
		float				Rx, Ry, Rz;
		float				Friction;

	public:

		virtual ZESteeringOutput	Process(float ElapsedTime);

									~ZESteeringPlayerFree();
									ZESteeringPlayerFree();

};

class ZEListener;
class ZELightProjective;
class ZECamera;
class ZEModelIKChain;

class ZEPlayer : public ZEActor
{
	ZE_OBJECT

	private:
		ZEInputMap				InputMap;
		ZECamera*				Camera;
		ZEListener*				Listener;
		float					FOV;

	protected:
								ZEPlayer();
		virtual					~ZEPlayer();

	public:
		ZECamera*				GetCamera();
		ZEListener*				GetListener();

		void					SetFOV(float FOV);
		float					GetFOV();

		virtual void			Tick(float Time);
			
		static ZEPlayer*		CreateInstance();
};
#endif
