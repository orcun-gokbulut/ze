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

enum ZEParticleEmitterType
{
	ZE_PET_POINT,
	ZE_PET_PLANE,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};

enum ZEParticleBillboardType
{
	ZE_PBT_AXIS_ORIENTED,
	ZE_PBT_SCREEN_ALIGNED,
	ZE_PBT_VIEW_POINT_ORIENTED
};

class ZEGRConstantBuffer;
class ZEGRStructuredBuffer;
class ZEParticleEffect;
class ZEParticleModifier;
class ZERNParticleMaterial;
class ZERNRenderParameters;
struct ZERNCullParameters;

class ZEParticleEmitter : public ZEObject
{
	friend class ZEParticleModifier;
	friend class ZEParticlePhysicsModifier;
	friend class ZEParticleEffect;

	ZE_OBJECT
	private:
		ZEString							Name;
		ZEParticleEffect*					Owner;

		ZEArray<ZEParticle>					ParticlePool;
		ZEArray<ZEParticle>					SortArray;
		ZEArray<ZEParticleModifier*>		Modifiers;

		ZEHolder<ZERNParticleMaterial>		Material;
		ZERNCommand							RenderCommand;
		ZEParticleBillboardType				BillboardType;
		ZEHolder<ZEGRConstantBuffer>		InstanceBuffer;
		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;

		struct InstanceAttributes
		{
			ZEVector3						Position;
			float							Reserved0;
			ZEVector2						Size;
			ZEVector2						Cos_NegSin;
			ZEVector4						Color;
		};

		ZEVector3							AxisOrientation;

		float								ParticlesRemaining;
		ZEUInt								MaxParticleCount;
		ZEUInt								ParticlesPerSecond;
		ZEUInt								EmittedParticleCount;
		ZEUInt								AliveParticleCount;

		bool								EmitterLocal;
		bool								SortingEnabled;
		bool								ParticleFixedAspectRatio;

		ZEAABBox							BoundingBox;
		ZEVector3							Position;
		ZEParticleEmitterType				Type;

		ZEVector3							BoxSize;
		float								SphereRadius;
		ZEVector2							TorusSize;
		ZEVector2							PlaneSize;

		ZEVector4							MinColor;
		ZEVector4							MaxColor;
		ZEVector2							MinSize;
		ZEVector2							MaxSize;
		float								MinLife;
		float								MaxLife;

		bool								Continuity;

		bool								Initialize();
		void								Deinitialize();

		void 								InitializeParticlePool();
		void								GenerateParticle(ZEParticle &Particle);

		void								SetOwner(ZEParticleEffect* Owner);
		ZEParticleEffect*					GetOwner() const;

		void								SortParticles();

											ZEParticleEmitter();
		virtual								~ZEParticleEmitter();

	protected:
		ZEArray<ZEParticle>&				GetParticlePool();

	public:
		void								SetName(const ZEString& Name);
		const ZEString&						GetName() const;

		void								SetEmitterLocal(bool EmitterLocal);
		bool								GetEmitterLocal() const;

		void								SetSortingEnabled(bool SortingEnabled);
		bool								GetSortingEnabled() const;

		void								SetParticleFixedAspectRatio(bool ParticleFixedAspectRatio);
		bool								GetParticleFixedAspectRatio() const;

		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;

		const ZEAABBox&						GetBoundingBox() const;

		const ZEArray<ZEParticleModifier*>&	GetModifiers();
		void								AddModifier(ZEParticleModifier* Modifier);
		void								RemoveModifier(ZEParticleModifier* Modifier);

		void								SetParticlesPerSecond(ZEUInt ParticlesPerSecond);
		ZEUInt								GetParticlesPerSecond() const;

		ZEUInt								GetAliveParticleCount() const;

		void								SetPlaneSize(const ZEVector2& PlaneSize);
		const ZEVector2&					GetPlaneSize() const;

		void								SetBoxSize(const ZEVector3& BoxSize);
		const ZEVector3&					GetBoxSize() const;

		void								SetSphereRadius(float SphereRadius);
		float								GetSphereRadius() const;

		void								SetTorusSize(const ZEVector2& TorusRadius);
		const ZEVector2&					GetTorusSize() const;

		void								SetType(ZEParticleEmitterType EmitterType);
		ZEParticleEmitterType				GetType() const;

		void								SetMaxParticleCount(ZEUInt Value);
		ZEUInt								GetMaxParticleCount() const;

		void								Reset();
		void								SetContinuity(bool Value);
		bool								GetContinuity() const;

		void								SetMinSize(const ZEVector2& Size);
		const ZEVector2&					GetMinSize() const;
		void 								SetMaxSize(const ZEVector2& Size);
		const ZEVector2&					GetMaxSize() const;

		void 								SetMinLife(float Life);
		float 								GetMinLife() const;
		void 								SetMaxLife(float Life);
		float 								GetMaxLife() const;

		void 								SetMinColor(const ZEVector4& Color);
		const ZEVector4&					GetMinColor() const;
		void 								SetMaxColor(const ZEVector4& Color);
		const ZEVector4&					GetMaxColor() const;

		void								SetBillboardType(ZEParticleBillboardType Type);
		ZEParticleBillboardType				GetBillboardType() const;

		void								SetAxisOrientation(const ZEVector3& Orientation);
		const ZEVector3&					GetAxisOrientation() const;

		void								SetMaterial(ZEHolder<ZERNParticleMaterial> Material);
		ZEHolder<ZERNParticleMaterial>		GetMaterial() const;

		bool								PreRender(const ZERNCullParameters* CullParameters);
		void								Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);
		void								Tick(float ElapsedTime);

		static ZEParticleEmitter*			CreateInstance();
};
