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
#include "ZEInitializable.h"

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEParticle.h"
#include "ZEExport.ZEEngine.h"

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEParticleEmitterType)
{
	ZE_PET_POINT,
	ZE_PET_PLANE,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEParticleBillboardType)
{
	ZE_PBT_AXIS_ORIENTED,
	ZE_PBT_VELOCITY_ORIENTED,
	ZE_PBT_VIEW_POINT_ORIENTED
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZEParticleAxisOrientation)
{
	ZE_PAO_X,
	ZE_PAO_Y,
	ZE_PAO_Z,
	ZE_PAO_NEGATIVE_X,
	ZE_PAO_NEGATIVE_Y,
	ZE_PAO_NEGATIVE_Z,
};

class ZEParticleEffect;
class ZEParticleModifier;
class ZEGRBuffer;
class ZERNView;
class ZERNParticleMaterial;
class ZERNRenderParameters;
class ZERNPreRenderParameters;

class ZE_EXPORT_ZEENGINE ZEParticleEmitter : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZEParticleModifier;
	friend class ZEParticleEffect;
	private:
		ZEFlags								DirtyFlags;
		ZEParticleEffect*					Effect;
		ZEString							Name;

		ZESize								AliveParticleCount;
		ZEArray<ZEParticle>					ParticlePool;
		ZEArray<ZEParticle>					SortArray;
		ZEArray<ZEParticleModifier*>		Modifiers;

		ZEParticleAxisOrientation			AxisOrientation;
		ZEUInt								ParticlesPerSecondMin;
		ZEUInt								ParticlesPerSecondMax;
		float								ParticlesAccumulation;

		bool								SortingEnabled;
		bool								ParticleFixedAspectRatio;
		bool								LocalSpace;

		ZEAABBox							BoundingBox;
		ZEVector3							Position;
		ZEParticleEmitterType				Type;
		ZEParticleBillboardType				BillboardType;

		ZEVector3							BoxSize;
		float								SphereRadius;
		ZEVector2							TorusSize;
		ZEVector2							PlaneSize;

		ZEVector4							ParticleColorMin;
		ZEVector4							ParticleColorMax;
		ZEVector2							ParticleSizeMin;
		ZEVector2							ParticleSizeMax;
		float								ParticleLifeMin;
		float								ParticleLifeMax;
		bool								ParticleImmortal;
		bool								ParticleLocalSpace;

		ZERNCommand							RenderCommand;

		ZEHolder<ZERNParticleMaterial>		Material;
		ZEHolder<ZEGRBuffer>				InstanceBuffer;
		ZEHolder<ZEGRBuffer>				ConstantBuffer;

		struct
		{
			ZEVector3						Axis;
			ZEUInt							BillboardType;
		} Constants;

		struct InstanceAttributes
		{
			ZEVector3						Position;
			float							Reserved;
			ZEVector2						Size;
			ZEVector2						Cos_NegSin;
			ZEVector4						Color;
		};

		virtual bool						InitializeInternal() override;
		virtual bool						DeinitializeInternal() override;

		void								UpdateConstantBuffer();

		void								LocalTransformChanged();
		void								EffectTransformChanged();

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

		void								SetLocalSpace(bool LocalSpace);
		bool								GetLocalSpace() const;

		// Particle Pool
		const ZEArray<ZEParticle>&			GetPool() const;

		void								SetPoolSize(ZESize Value);
		ZESize								GetPoolSize() const;
		ZEUInt								GetAliveParticleCount() const;

		void								SetSortingEnabled(bool SortingEnabled);
		bool								GetSortingEnabled() const;

		// Particle
		void								SetParticleLocalSpace(bool ParticleLocalSpace);
		bool								GetParticleLocalSpace() const;

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

		void								SetBillboardType(ZEParticleBillboardType BillboardType);
		ZEParticleBillboardType				GetBillboardType() const;

		void								SetAxisOrientation(ZEParticleAxisOrientation AxisOrientation);
		ZEParticleAxisOrientation			GetAxisOrientation() const;

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
