//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEmitter.h
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

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEParticle.h"

ZE_ENUM(ZEParticleEmitterType)
{
	ZE_PET_POINT,
	ZE_PET_PLANE,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};

enum ZEParticleSpace
{
	ZE_PS_LOCAL,
	ZE_PS_WORLD,
	ZE_PS_SCREEN
};

ZE_ENUM(ZEParticleBillboardType)
{
	ZE_PBT_AXIS_ORIENTED,
	ZE_PBT_SCREEN_ALIGNED,
	ZE_PBT_VIEW_POINT_ORIENTED
};

class ZEParticleEffect;
class ZEParticleModifier;
class ZEGRBuffer;
class ZERNView;
class ZERNParticleMaterial;
class ZERNRenderParameters;
class ZERNPreRenderParameters;

class ZEParticleEmitter : public ZEObject
{
	ZE_OBJECT
	friend class ZEParticleModifier;
	friend class ZEParticlePhysicsModifier;
	friend class ZEParticleEffect;
	private:
		ZEParticleEffect*					Effect;
		ZEString							Name;

		ZESize								AliveParticleCount;
		ZEArray<ZEParticle>					ParticlePool;
		ZEArray<ZEParticle>					SortArray;
		ZEArray<ZEParticleModifier*>		Modifiers;

		struct InstanceAttributes
		{
			ZEVector3						Position;
			float							Reserved0;
			ZEVector2						Size;
			ZEVector2						Cos_NegSin;
			ZEVector4						Color;
		};

		ZEVector3							AxisOrientation;
		ZEUInt								ParticlesPerSecondMin;
		ZEUInt								ParticlesPerSecondMax;
		float								ParticlesAccumulation;

		bool								SortingEnabled;
		bool								ParticleFixedAspectRatio;

		ZEAABBox							BoundingBox;
		ZEVector3							Position;
		ZEParticleEmitterType				Type;

		ZEVector3							BoxSize;
		float								SphereRadius;
		ZEVector2							TorusSize;
		ZEVector2							PlaneSize;

		ZEParticleSpace						ParticleSpace;
		ZEVector4							ParticleColorMin;
		ZEVector4							ParticleColorMax;
		ZEVector2							ParticleSizeMin;
		ZEVector2							ParticleSizeMax;
		float								ParticleLifeMin;
		float								ParticleLifeMax;
		bool								ParticleImmortal;

		ZEHolder<ZERNParticleMaterial>		Material;
		ZERNCommand							RenderCommand;
		ZEParticleBillboardType				BillboardType;
		ZEHolder<ZEGRBuffer>				InstanceBuffer;
		ZEHolder<ZEGRBuffer>				ConstantBuffer;


		bool								Initialize();
		void								Deinitialize();

		void								GenerateParticle(ZEParticle &Particle);
		void								UpdateParticles(float ElapsedTime);
		void								SortParticles(const ZERNView& View);


											ZEParticleEmitter();
		virtual								~ZEParticleEmitter();

	public:
		ZEParticleEffect*					GetEffect() const;

		void								SetName(const ZEString& Name);
		const ZEString&						GetName() const;


		// Emitter
		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;

		void								SetType(ZEParticleEmitterType EmitterType);
		ZEParticleEmitterType				GetType() const;

		void								SetPlaneSize(const ZEVector2& PlaneSize);
		const ZEVector2&					GetPlaneSize() const;

		void								SetBoxSize(const ZEVector3& BoxSize);
		const ZEVector3&					GetBoxSize() const;

		void								SetSphereRadius(float SphereRadius);
		float								GetSphereRadius() const;

		void								SetTorusSize(const ZEVector2& TorusRadius);
		const ZEVector2&					GetTorusSize() const;


		// Particle Pool
		const ZEArray<ZEParticle>&			GetPool() const;

		void								SetPoolSize(ZESize Value);
		ZESize								GetPoolSize() const;
		ZEUInt								GetAliveParticleCount() const;

		void								SetSortingEnabled(bool SortingEnabled);
		bool								GetSortingEnabled() const;


		// Particle
		void								SetParticleSpace(ZEParticleSpace Space);
		ZEParticleSpace						GetParticalSpace() const;

		void								SetParticlesPerSecond(ZEUInt ParticlesPerSecond);
		ZEUInt								GetParticlesPerSecond() const;

		void								SetParticlesPerSecondMin(ZEUInt ParticlesPerSecond);
		ZEUInt								GetParticlesPerSecondMin() const;

		void								SetParticlesPerSecondMax(ZEUInt ParticlesPerSecond);
		ZEUInt								GetParticlesPerSecondMax() const;
		
		void								SetParticleImmortal(bool Enabled);
		bool								GetParticleImmortal() const;

		void 								SetParticleLifeMin(float Life);
		float 								GetParticleLifeMin() const;
		void 								SetParticleLifeMax(float Life);
		float 								GetParticleLifeMax() const;

		void								SetParticleSizeMin(const ZEVector2& Size);
		const ZEVector2&					GetParticleSizeMin() const;
		void 								SetParticleSizeMax(const ZEVector2& Size);
		const ZEVector2&					GetParticleSizeMax() const;

		void 								SetParticleColorMin(const ZEVector4& Color);
		const ZEVector4&					GetParticleColorMin() const;
		void 								SetParticleColorMax(const ZEVector4& Color);
		const ZEVector4&					GetParticleColorMax() const;

		void								SetParticleFixedAspectRatio(bool ParticleFixedAspectRatio);
		bool								GetParticleFixedAspectRatio() const;

		// Modifiers
		const ZEArray<ZEParticleModifier*>&	GetModifiers() const;
		void								AddModifier(ZEParticleModifier* Modifier);
		void								RemoveModifier(ZEParticleModifier* Modifier);


		// Rendering
		const ZEAABBox&						GetBoundingBox() const;

		void								SetBillboardType(ZEParticleBillboardType Type);
		ZEParticleBillboardType				GetBillboardType() const;

		void								SetAxisOrientation(const ZEVector3& Orientation);
		const ZEVector3&					GetAxisOrientation() const;

		void								SetMaterial(ZEHolder<ZERNParticleMaterial> Material);
		ZEHolder<ZERNParticleMaterial>		GetMaterial() const;


		// Functions
		void								GeneratePool();
		void								ResetPool();

		void								Tick(float ElapsedTime);
		bool								PreRender(const ZERNPreRenderParameters* Parameters);
		void								Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		static ZEParticleEmitter*			CreateInstance();
};
