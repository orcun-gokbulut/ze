//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleSystem.h
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

#include "ZEMeta/ZEObject.h"

#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEArray.h"
#include "ZEParticle.h"

class ZEParticleRenderer;
class ZEParticleOperator;
class ZEParticleGenerator;
class ZERNRenderParameters;
class ZERNCommand;
struct ZERNCullParameters;

// struct ZENewParticle
// {
// 	ZEQuaternion	Rotation3D;
// 	float			Rotation2D;
// 
// 	ZEVector3		Position;
// 	ZEVector3		Color;
// 	float			Transparency;
// 
// 	ZEVector2		Size2D;
// 	ZEVector3		Size3D;
// };

class ZEParticleSystem : public ZEObject
{
	ZE_OBJECT

	friend class ZEParticleEffect;

	private:
		ZEParticleEffect*						Owner;

		ZEArray<ZEParticle>						ParticlePool;

		ZEParticleRenderer*						Renderer;
		ZEArray<ZEParticleOperator*>			Operators;
		ZEArray<ZEParticleGenerator*>			Generators;

		ZEUInt									MaximumParticleCount;

	public:
		void									SetRenderer(ZEParticleRenderer* Renderer);
		const ZEParticleRenderer*				GetRenderer() const;

		const ZEArray<ZEParticle>&				GetParticlePool() const;

		const ZEArray<ZEParticleOperator*>&		GetOperators() const;
		bool									AddOperator(ZEParticleOperator* NewOperator);
		bool									RemoveOperator(ZEParticleOperator* OperatorToRemove);

		const ZEArray<ZEParticleGenerator*>&	GetGenerators() const;
		bool									AddGenerator(ZEParticleGenerator* NewOperator);
		bool									RemoveGenerator(ZEParticleGenerator* OperatorToRemove);

		void									SetMaximumParticleCount(ZEUInt	ParticleCount);
		ZEUInt									GetMaximumParticleCount() const;

		const ZEParticleEffect*					GetOwner() const;

		bool									PreRender(const ZERNCullParameters* CullParameters);
		void									Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);
		void									Tick(float ElapsedTime);

												ZEParticleSystem();
		virtual									~ZEParticleSystem();
};
