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

#define ZE_PEF_LIGHT_RECEIVER						0x1
#define ZE_PEF_RENDER_STREAKS						0x2
#define ZE_PEF_BILLBOARD_VIEW						0x4
#define ZE_PEF_BILLBOARD_AXIS						0x8
#define ZE_PEF_TILED_RENDERING						0x10
#define ZE_PEF_VELOCITY_PER_PARTICLE				0x20
#define ZE_PEF_ACCELERATION_PER_PARTICLE			0x40
#define ZE_PEF_SIZE_PER_PARTICLE					0x80
#define ZE_PEF_COLOR_PER_PARTICLE					0x100
#define ZE_PEF_TEXCOORDS_PER_PARTICLE				0x200
#define ZE_PEF_ROTATION_PER_PARTICLE				0x400
#define ZE_PEF_ANGULAR_ACCELERATION_PER_PARTICLE	0x800
#define ZE_PEF_ANGULAR_VELOCITY_PER_PARTICLE		0x1000
#define ZE_PEF_TOTAL_LIFE_PER_PARTICLE				0x2000

ZE_ENUM(ZEParticleEmitterType)
{
	ZE_PET_POINT,
	ZE_PET_PLANE,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};

ZE_ENUM(ZEParticleBillboardType)
{
	ZE_PBT_AXIS_ORIENTED,
	ZE_PBT_VELOCITY_ORIENTED,
	ZE_PBT_VIEW_POINT_ORIENTED
};

ZE_ENUM(ZEParticleAxisOrientation)
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
class ZERNMaterial;

struct ZEParticlePool
{
	ZESize				Count;
	ZEArray<ZEVector3>	Positions;
	ZEArray<ZEVector3>	Velocities;
	ZEArray<ZEVector3>	Accelerations;
	ZEArray<ZEVector2>	Sizes;
	ZEArray<ZEVector4>	Colors;
	ZEArray<ZEVector4>	TexCoords;
	ZEArray<float>		Rotations;
	ZEArray<float>		AngularVelocities;
	ZEArray<float>		AngularAccelerations;
	ZEArray<float>		Lifes;
	ZEArray<float>		TotalLifes;
};

typedef ZEFlags			ZEParticleEmitterFlags;

class ZERNCommandParticleEmitter : public ZERNCommand
{
	ZE_OBJECT
	public:
		ZEHolder<const ZERNMaterial>	Material;

		ZEVector3						Axis;
		ZEParticleEmitterFlags			Flags;

		ZEVector4						ParticleTexCoords;
		ZEVector4						ParticleColor;
		ZEVector2						ParticleSize;
		float							ParticleRotation;
		ZEUInt							ParticleCount;

		void*							Data;
		ZESize							DataSize;
};

class ZEParticleEmitter : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZEParticleModifier;
	friend class ZEParticleEffect;
	private:
		ZEFlags								DirtyFlags;
		ZEParticleEffect*					Effect;
		ZEString							Name;

		ZEParticleEmitterFlags				Flags;

		ZEUInt								LastSpawnedParticleCount;
		ZESize								AliveParticleCount;
		ZESize								DeadParticleStartIndex;
		ZESize								DeadParticleCount;
		ZEArray<ZEParticleModifier*>		Modifiers;

		ZEParticlePool						ParticlePool;
		ZEArray<ZEUInt>						AliveParticleIndices;
		ZEArray<ZEUInt>						DeadParticleIndices;
		ZEArray<ZEUInt>						SortedAliveParticleIndices;

		ZEParticleAxisOrientation			AxisOrientation;
		ZEUInt								EmissionRateMin;
		ZEUInt								EmissionRateMax;
		float								CurrentTime;
		float								TotalTime;

		bool								SortingEnabled;
		bool								ParticleFixedAspectRatio;
		bool								LocalSpace;

		ZEAABBox							BoundingBox;
		ZEVector3							Position;
		ZEParticleEmitterType				Type;
		ZEParticleBillboardType				BillboardType;
		bool								LightReceiver;
		bool								TiledRendering;

		ZEVector3							BoxSize;
		float								SphereRadius;
		ZEVector2							TorusSize;
		ZEVector2							PlaneSize;

		ZEVector4							ParticleColor;
		ZEVector2							ParticleSize;
		ZEVector4							ParticleTexCoords;
		float								ParticleRotation;
		float								ParticleLifeMin;
		float								ParticleLifeMax;
		bool								ParticleImmortal;
		bool								ParticleLocalSpace;

		ZERNCommandParticleEmitter			RenderCommand;
		
		ZEHolder<ZERNParticleMaterial>		Material;
		
		virtual bool						InitializeInternal() override;
		virtual bool						DeinitializeInternal() override;

		void								LocalTransformChanged();
		void								EffectTransformChanged();

		void								UpdateParticlePool();
		void								GenerateParticle(ZEUInt ParticleIndex);
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

		void								SetEmissionRate(ZEUInt Rate);
		ZEUInt								GetEmissionRate() const;

		void								SetEmissionRateMin(ZEUInt Rate);
		ZEUInt								GetEmissionRateMin() const;

		void								SetEmissionRateMax(ZEUInt Rate);
		ZEUInt								GetEmissionRateMax() const;

		void								SetEmissionDuration(float Duration);
		float								GetEmissionDuration() const;
		// Particle Pool
		const ZEParticlePool&				GetParticlePool() const;

		const ZEArray<ZEUInt>&				GetAliveParticleIndices() const;
		const ZEArray<ZEUInt>&				GetDeadParticleIndices() const;

		ZEUInt								GetAliveParticleCount() const;
		ZEUInt								GetDeadParticleStartIndex() const;
		ZEUInt								GetDeadParticleCount() const;
		ZEUInt								GetLastSpawnedParticleCount() const;

		void								SetPoolSize(ZESize Size);
		ZESize								GetPoolSize() const;
		
		void								SetSortingEnabled(bool SortingEnabled);
		bool								GetSortingEnabled() const;

		// Particle
		void								SetParticleLocalSpace(bool ParticleLocalSpace);
		bool								GetParticleLocalSpace() const;

		void								SetParticleLifeMin(float Life);
		float								GetParticleLifeMin() const;

		void								SetParticleLifeMax(float Life);
		float								GetParticleLifeMax() const;

		void								SetParticleColor(const ZEVector4& Color);
		const ZEVector4&					GetParticleColor() const;

		void								SetParticleSize(const ZEVector2& Size);
		const ZEVector2&					GetParticleSize() const;

		void								SetParticleTexCoords(const ZEVector4& TexCoords);
		const ZEVector4&					GetParticleTexCoords() const;

		void								SetParticleRotation(float Rotation);
		float								GetParticleRotation() const;

		void								SetParticleImmortal(bool Enabled);
		bool								GetParticleImmortal() const;

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

		void								SetLightReceiver(bool LightReceiver);
		bool								GetLightReceiver() const;

		void								SetTiledRendering(bool TiledRendering);
		bool								GetTiledRendering() const;

		void								SetMaterial(ZEHolder<ZERNParticleMaterial> Material);
		ZEHolder<ZERNParticleMaterial>		GetMaterial() const;

		void								ResetPool();
		void								Tick(float ElapsedTime);
		bool								PreRender(const ZERNPreRenderParameters* Parameters);

		static ZEParticleEmitter*			CreateInstance();
};
