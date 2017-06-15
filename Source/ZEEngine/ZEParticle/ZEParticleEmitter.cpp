//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEmitter.cpp
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

#include "ZEParticleEmitter.h"

#include "ZERandom.h"
#include "ZEMath/ZEAngle.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZETimeParameters.h"
#include "ZEParticleEffect.h"
#include "ZEParticleModifier.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZERenderer/ZERNStageID.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNParticleMaterial.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNStage.h"
#include "ZEMath/ZEMath.h"

#define ZE_PEDF_TRANSFORM		1
#define ZE_PEDF_PARTICLE_POOL	2

static ZEVector3 ConvertAxisOrientation(ZEParticleAxisOrientation AxisOrientation)
{
	switch (AxisOrientation)
	{
		case ZE_PAO_X:
			return ZEVector3::UnitX;

		case ZE_PAO_Y:
			return ZEVector3::UnitY;

		case ZE_PAO_Z:
			return ZEVector3::UnitZ;

		case ZE_PAO_NEGATIVE_X:
			return ZEVector3::NegUnitX;

		case ZE_PAO_NEGATIVE_Y:
			return ZEVector3::NegUnitY;

		case ZE_PAO_NEGATIVE_Z:
			return ZEVector3::NegUnitZ;

		default:
			return ZEVector3::Zero;
	}
}

bool ZEParticleEmitter::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;
	
	UpdateParticlePool();

	return true;
}

bool ZEParticleEmitter::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	Effect = NULL;
	
	LastSpawnedParticleCount = 0;
	AliveParticleCount = 0;
	DeadParticleCount = 0;

	Modifiers.Clear();

	ParticlePool.Positions.Clear();
	ParticlePool.Velocities.Clear();
	ParticlePool.Accelerations.Clear();
	ParticlePool.Sizes.Clear();
	ParticlePool.Colors.Clear();
	ParticlePool.TexCoords.Clear();
	ParticlePool.Rotations.Clear();
	ParticlePool.AngularVelocities.Clear();
	ParticlePool.AngularAccelerations.Clear();
	ParticlePool.Lifes.Clear();
	ParticlePool.TotalLifes.Clear();

	AliveParticleIndices.Clear();
	DeadParticleIndices.Clear();
	SortedAliveParticleIndices.Clear();

	CurrentTime = 0.0f;

	Material.Release();

	return ZEInitializable::DeinitializeInternal();
}

void ZEParticleEmitter::LocalTransformChanged()
{
	DirtyFlags.RaiseFlags(ZE_PEDF_TRANSFORM);
}

void ZEParticleEmitter::EffectTransformChanged()
{
	if (!GetLocalSpace())
		return;
	
	DirtyFlags.RaiseFlags(ZE_PEDF_TRANSFORM);
}

void ZEParticleEmitter::UpdateParticlePool()
{
	if (!DirtyFlags.GetFlags(ZE_PEDF_PARTICLE_POOL))
		return;

	ParticlePool.Positions.Resize(ParticlePool.Count);
	ParticlePool.Lifes.Resize(ParticlePool.Count);
	
	ParticlePool.Velocities.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_VELOCITY_PER_PARTICLE));
	ParticlePool.Accelerations.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_ACCELERATION_PER_PARTICLE));
	ParticlePool.Sizes.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_SIZE_PER_PARTICLE));
	ParticlePool.Colors.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_COLOR_PER_PARTICLE));
	ParticlePool.TexCoords.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_TEXCOORDS_PER_PARTICLE));
	ParticlePool.Rotations.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_ROTATION_PER_PARTICLE));
	ParticlePool.AngularVelocities.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_ANGULAR_VELOCITY_PER_PARTICLE));
	ParticlePool.AngularAccelerations.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_ANGULAR_ACCELERATION_PER_PARTICLE));
	ParticlePool.TotalLifes.Resize(ParticlePool.Count * (ZESize)Flags.GetFlags(ZE_PEF_TOTAL_LIFE_PER_PARTICLE));

	AliveParticleIndices.Resize(ParticlePool.Count);
	DeadParticleIndices.Resize(ParticlePool.Count);
	SortedAliveParticleIndices.Resize(ParticlePool.Count);
	
	for (ZESize I = (DeadParticleStartIndex + DeadParticleCount); I < ParticlePool.Count; I++)
		DeadParticleIndices[I % ParticlePool.Count] = I;
	
	DeadParticleCount = ParticlePool.Count;

	DirtyFlags.UnraiseFlags(ZE_PEDF_PARTICLE_POOL);
}

void ZEParticleEmitter::GenerateParticle(ZEUInt ParticleIndex)
{
	switch (Type)
	{
		case ZE_PET_POINT:
		{
			ParticlePool.Positions[ParticleIndex] = ZEVector3::Zero;
			break;
		}
		case ZE_PET_PLANE:
		{
			ParticlePool.Positions[ParticleIndex].x = ZERandom::GetFloatRange(-PlaneSize.x / 2.0f, PlaneSize.x / 2.0f);
			ParticlePool.Positions[ParticleIndex].y = 0.0f;
			ParticlePool.Positions[ParticleIndex].z = ZERandom::GetFloatRange(-PlaneSize.y / 2.0f, PlaneSize.y / 2.0f);
			break;
		}
		case ZE_PET_BOX:
		{
			ParticlePool.Positions[ParticleIndex].x = ZERandom::GetFloatRange(-BoxSize.x / 2.0f, BoxSize.x / 2.0f);
			ParticlePool.Positions[ParticleIndex].y = ZERandom::GetFloatRange(-BoxSize.y / 2.0f, BoxSize.y / 2.0f);
			ParticlePool.Positions[ParticleIndex].z = ZERandom::GetFloatRange(-BoxSize.z / 2.0f, BoxSize.z / 2.0f);
			break;
		}
		case ZE_PET_TORUS:
		{
			float Theta = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float Phi = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float TubeRadius = ZERandom::GetFloatRange(0.0f, TorusSize.y);
			ParticlePool.Positions[ParticleIndex].x = (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
			ParticlePool.Positions[ParticleIndex].y = (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
			ParticlePool.Positions[ParticleIndex].z = TubeRadius * ZEAngle::Sin(Phi);
			break;
		}
		case ZE_PET_SPHERE:
		{
			float Radius = ZERandom::GetFloatRange(0.0f, SphereRadius);
			float Theta = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float Phi = ZERandom::GetFloatRange(0.0f, ZE_PI);
			ParticlePool.Positions[ParticleIndex].x = Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
			ParticlePool.Positions[ParticleIndex].y = Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
			ParticlePool.Positions[ParticleIndex].z = Radius * ZEAngle::Cos(Phi);
			break;
		}
	}
	
	if (!GetParticleLocalSpace())
		ParticlePool.Positions[ParticleIndex] += GetPosition();
	
	if (!GetLocalSpace())
		ParticlePool.Positions[ParticleIndex] = Effect->GetWorldTransform() * ParticlePool.Positions[ParticleIndex];

	ParticlePool.Lifes[ParticleIndex] = ZERandom::GetFloatRange(ParticleLifeMin, ParticleLifeMax);
	if (Flags.GetFlags(ZE_PEF_TOTAL_LIFE_PER_PARTICLE))
		ParticlePool.TotalLifes[ParticleIndex] = ParticlePool.Lifes[ParticleIndex];	
}

void ZEParticleEmitter::UpdateParticles(float ElapsedTime)
{
	UpdateParticlePool();
	
	ZEUInt Index = 0;
	while (Index != AliveParticleCount)
	{
		ParticlePool.Lifes[AliveParticleIndices[Index]] -= ElapsedTime;
		if (ParticlePool.Lifes[AliveParticleIndices[Index]] <= 0.0f)
		{
			DeadParticleIndices[(DeadParticleStartIndex + DeadParticleCount) % DeadParticleIndices.GetCount()] = AliveParticleIndices[Index];
			DeadParticleCount++;
			AliveParticleIndices[Index] = AliveParticleIndices[--AliveParticleCount];
			
			continue;
		}

		Index++;
	}

	CurrentTime += ElapsedTime;
	
	if (CurrentTime >= TotalTime)
		CurrentTime = 0.0f;

	ZEUInt MaxSpawnedParticleCount = EmissionRateMax * TotalTime;
	LastSpawnedParticleCount = ZERandom::GetFloatRange((float)EmissionRateMin, (float)EmissionRateMax) * ElapsedTime;
	LastSpawnedParticleCount = ZEMath::Min(LastSpawnedParticleCount, MaxSpawnedParticleCount);
	LastSpawnedParticleCount = ZEMath::Min(LastSpawnedParticleCount, (ZEUInt)DeadParticleCount);
	
	for (ZESize I = DeadParticleStartIndex; I < (DeadParticleStartIndex + LastSpawnedParticleCount); I++)
	{
		ZEUInt ParticleIndex = DeadParticleIndices[I % DeadParticleIndices.GetCount()];
		GenerateParticle(ParticleIndex);
		AliveParticleIndices[AliveParticleCount++] = ParticleIndex;
	}
	
	for (ZEUInt I = 0; I < Modifiers.GetCount(); I++)
		Modifiers[I]->Tick(ElapsedTime);
	
	DeadParticleCount -= LastSpawnedParticleCount;
	DeadParticleStartIndex = (DeadParticleStartIndex + LastSpawnedParticleCount) % DeadParticleIndices.GetCount();
}

ZEParticleEmitter::ZEParticleEmitter()
{
	DirtyFlags.RaiseAll();

	Effect = NULL;

	Flags = 0;

	LastSpawnedParticleCount = 0;
	AliveParticleCount = 0;
	DeadParticleStartIndex = 0;
	DeadParticleCount = 0;

	ParticlePool.Count = 1000;

	AxisOrientation = ZE_PAO_Y;
	EmissionRateMin = 0;
	EmissionRateMax = 0;

	CurrentTime = 0.0f;
	TotalTime = 0.0f;

	SortingEnabled = false;
	ParticleFixedAspectRatio = false;
	LocalSpace = true;

	Position = ZEVector3::Zero;
	Type = ZE_PET_POINT;
	BillboardType = ZE_PBT_VIEW_POINT_ORIENTED;
	LightReceiver = false;
	TiledRendering = false;
	BoxSize = ZEVector3::One;
	SphereRadius = 1.0f;
	TorusSize = ZEVector2::One;
	PlaneSize = ZEVector2::One;

	ParticleColor = ZEVector4::One;
	ParticleSize = ZEVector2::One;
	ParticleTexCoords = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);
	ParticleRotation = 0.0f;
	ParticleLifeMin = 0.0f;
	ParticleLifeMax = 0.0f;
	ParticleImmortal = false;
	ParticleLocalSpace = false;
}

ZEParticleEmitter::~ZEParticleEmitter()
{
	Deinitialize();

	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
		delete Modifiers[I];

	Modifiers.Clear();
}

ZEParticleEffect* ZEParticleEmitter::GetEffect() const
{
	return Effect;
}

void ZEParticleEmitter::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEParticleEmitter::GetName() const
{
	return Name;
}

void ZEParticleEmitter::SetPosition(const ZEVector3& EmitterPosition)
{
	Position = EmitterPosition;

	LocalTransformChanged();
}

const ZEVector3& ZEParticleEmitter::GetPosition() const
{
	return Position;
}

void ZEParticleEmitter::SetType(ZEParticleEmitterType EmitterType)
{
	Type = EmitterType;
}

ZEParticleEmitterType ZEParticleEmitter::GetType() const
{
	return Type;
}

void ZEParticleEmitter::SetSphereRadius(float SphereRadius)
{
	this->SphereRadius = SphereRadius;
}

float ZEParticleEmitter::GetSphereRadius() const
{
	return SphereRadius;
}

void ZEParticleEmitter::SetTorusSize(const ZEVector2& TorusRadius)
{
	this->TorusSize = TorusRadius;
}

const ZEVector2& ZEParticleEmitter::GetTorusSize() const
{
	return TorusSize;
}

void ZEParticleEmitter::SetLocalSpace(bool Local)
{
	if (this->LocalSpace == LocalSpace)
		return;

	this->LocalSpace = LocalSpace;

	DirtyFlags.RaiseFlags(ZE_PEDF_TRANSFORM);
}

bool ZEParticleEmitter::GetLocalSpace() const
{
	return LocalSpace;
}

void ZEParticleEmitter::SetPlaneSize(const ZEVector2& PlaneSize)
{
	this->PlaneSize = PlaneSize;
}

const ZEVector2& ZEParticleEmitter::GetPlaneSize() const
{
	return PlaneSize;
}

void ZEParticleEmitter::SetBoxSize(const ZEVector3& BoxSize)
{
	this->BoxSize = BoxSize;
}

const ZEVector3& ZEParticleEmitter::GetBoxSize() const
{
	return BoxSize;
}

const ZEParticlePool& ZEParticleEmitter::GetParticlePool() const
{
	return ParticlePool;
}

void ZEParticleEmitter::SetPoolSize(ZESize PoolSize)
{
	if (ParticlePool.Count == PoolSize)
		return;
	
	ParticlePool.Count = PoolSize;
	
	DirtyFlags.RaiseFlags(ZE_PEDF_PARTICLE_POOL);
}

ZESize ZEParticleEmitter::GetPoolSize() const
{
	return ParticlePool.Count;
}

const ZEArray<ZEUInt>& ZEParticleEmitter::GetAliveParticleIndices() const
{
	return AliveParticleIndices;
}

const ZEArray<ZEUInt>& ZEParticleEmitter::GetDeadParticleIndices() const
{
	return DeadParticleIndices;
}

ZEUInt ZEParticleEmitter::GetAliveParticleCount() const
{
	return AliveParticleCount;
}

ZEUInt ZEParticleEmitter::GetDeadParticleStartIndex() const
{
	return DeadParticleStartIndex;
}

ZEUInt ZEParticleEmitter::GetDeadParticleCount() const
{
	return DeadParticleCount;
}

ZEUInt ZEParticleEmitter::GetLastSpawnedParticleCount() const
{
	return LastSpawnedParticleCount;
}

void ZEParticleEmitter::SetSortingEnabled(bool SortingEnabled)
{
	this->SortingEnabled = SortingEnabled;
}

bool ZEParticleEmitter::GetSortingEnabled() const
{
	return SortingEnabled;
}

void ZEParticleEmitter::SetParticleLocalSpace(bool ParticleLocalSpace)
{
	if (this->ParticleLocalSpace == ParticleLocalSpace)
		return;

	this->ParticleLocalSpace = ParticleLocalSpace;

	DirtyFlags.RaiseFlags(ZE_PEDF_TRANSFORM);
}

bool ZEParticleEmitter::GetParticleLocalSpace() const
{
	return ParticleLocalSpace;
}

void ZEParticleEmitter::SetEmissionRate(ZEUInt Rate)
{
	EmissionRateMin = Rate;
	EmissionRateMax = Rate;
}

ZEUInt ZEParticleEmitter::GetEmissionRate() const
{
	return (EmissionRateMin + EmissionRateMax) / 2;
}

void ZEParticleEmitter::SetEmissionRateMin(ZEUInt Rate)
{
	EmissionRateMin = Rate;
}

ZEUInt ZEParticleEmitter::GetEmissionRateMin() const
{
	return EmissionRateMin;
}

void ZEParticleEmitter::SetEmissionRateMax(ZEUInt Rate)
{
	EmissionRateMax = Rate;
}

ZEUInt ZEParticleEmitter::GetEmissionRateMax() const
{
	return EmissionRateMax;
}

void ZEParticleEmitter::SetEmissionDuration(float Duration)
{
	TotalTime = Duration;
}

float ZEParticleEmitter::GetEmissionDuration() const
{
	return TotalTime;
}

void ZEParticleEmitter::SetParticleTexCoords(const ZEVector4& TexCoords)
{
	ParticleTexCoords = TexCoords;
}

const ZEVector4& ZEParticleEmitter::GetParticleTexCoords() const
{
	return ParticleTexCoords;
}

void ZEParticleEmitter::SetParticleRotation(float Angle)
{
	ParticleRotation = Angle;
}

float ZEParticleEmitter::GetParticleRotation() const
{
	return ParticleRotation;
}

void ZEParticleEmitter::SetParticleImmortal(bool Enabled)
{
	ParticleImmortal = Enabled;
}

bool ZEParticleEmitter::GetParticleImmortal() const
{
	return ParticleImmortal;
}

void ZEParticleEmitter::SetParticleLifeMin(float MinLife)
{
	this->ParticleLifeMin = MinLife;
}

float ZEParticleEmitter::GetParticleLifeMin() const
{
	return ParticleLifeMin;
}

void ZEParticleEmitter::SetParticleLifeMax(float MaxLife)
{
	this->ParticleLifeMax = MaxLife;
}

float ZEParticleEmitter::GetParticleLifeMax() const
{
	return ParticleLifeMax;
}

void ZEParticleEmitter::SetParticleSize(const ZEVector2& Size)
{
	ParticleSize = Size;
}

const ZEVector2& ZEParticleEmitter::GetParticleSize() const
{
	return ParticleSize;
}

void ZEParticleEmitter::SetParticleColor(const ZEVector4& Color)
{
	ParticleColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetParticleColor() const
{
	return ParticleColor;
}

void ZEParticleEmitter::SetParticleFixedAspectRatio(bool ParticleFixedAspectRatio)
{
	this->ParticleFixedAspectRatio = ParticleFixedAspectRatio;
}

bool ZEParticleEmitter::GetParticleFixedAspectRatio() const
{
	return ParticleFixedAspectRatio;
}

const ZEArray<ZEParticleModifier*>& ZEParticleEmitter::GetModifiers() const
{
	return Modifiers;
}

void ZEParticleEmitter::AddModifier(ZEParticleModifier* Modifier)
{
	zeCheckError(Modifier == NULL, ZE_VOID, "Modifier is NULL.");
	zeCheckError(Modifier->Emitter != NULL, ZE_VOID, "Modifier is already registered to a Particle Emitter.");

	Flags.RaiseFlags(Modifier->GetFlags());
	DirtyFlags.RaiseFlags(ZE_PEDF_PARTICLE_POOL);

	Modifier->Emitter = this;
	Modifiers.Add(Modifier);
}

void ZEParticleEmitter::RemoveModifier(ZEParticleModifier* Modifier)
{
	zeCheckError(Modifier == NULL, ZE_VOID, "Modifier is NULL.");
	zeCheckError(Modifier->Emitter != this, ZE_VOID, "Modifier is not registered to this Particle Emitter.");

	Flags.UnraiseFlags(Modifier->GetFlags());
	DirtyFlags.RaiseFlags(ZE_PEDF_PARTICLE_POOL);

	Modifier->Emitter = NULL;
	Modifiers.RemoveValue(Modifier);
}

const ZEAABBox&	ZEParticleEmitter::GetBoundingBox() const
{
	return BoundingBox;
}

void ZEParticleEmitter::SetBillboardType(ZEParticleBillboardType BillboardType)
{
	this->BillboardType = BillboardType;

	if (BillboardType == ZE_PBT_VIEW_POINT_ORIENTED)
		Flags.RaiseFlags(ZE_PEF_BILLBOARD_VIEW);
	else if (BillboardType == ZE_PBT_AXIS_ORIENTED)
		Flags.RaiseFlags(ZE_PEF_BILLBOARD_AXIS);
}

ZEParticleBillboardType ZEParticleEmitter::GetBillboardType() const
{
	return BillboardType;
}

void ZEParticleEmitter::SetAxisOrientation(ZEParticleAxisOrientation AxisOrientation)
{
	if (this->AxisOrientation == AxisOrientation)
		return;

	this->AxisOrientation = AxisOrientation;

}

ZEParticleAxisOrientation ZEParticleEmitter::GetAxisOrientation() const
{
	return AxisOrientation;
}

void ZEParticleEmitter::SetLightReceiver(bool LightReceiver)
{
	this->LightReceiver = LightReceiver;
	
	Flags.SetFlags(ZE_PEF_LIGHT_RECEIVER, LightReceiver);
}

bool ZEParticleEmitter::GetLightReceiver() const
{
	return LightReceiver;
}

void ZEParticleEmitter::SetTiledRendering(bool TiledRendering)
{
	this->TiledRendering = TiledRendering;

	Flags.SetFlags(ZE_PEF_TILED_RENDERING, TiledRendering);
}

bool ZEParticleEmitter::GetTiledRendering() const
{
	return TiledRendering;
}

void ZEParticleEmitter::SetMaterial(ZEHolder<ZERNParticleMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<ZERNParticleMaterial> ZEParticleEmitter::GetMaterial() const
{
	return Material;
}

void ZEParticleEmitter::ResetPool()
{
	DeadParticleCount += AliveParticleCount;
	AliveParticleCount = 0;
}

void ZEParticleEmitter::Tick(float TimeElapsed)
{

}

bool ZEParticleEmitter::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (Parameters->Type == ZERN_RT_SHADOW && !GetMaterial()->GetShadowCaster())
		return false;

	if (Effect->GetState() != ZE_PES_RUNNING)
		UpdateParticles(Parameters->TimeParameters->FrameTimeDelta);

	if (GetAliveParticleCount() == 0)
		return false;

	if (!Material->PreRender(RenderCommand))
		return false;

	RenderCommand.Material = Material;
	RenderCommand.Axis = ConvertAxisOrientation(GetAxisOrientation());
	RenderCommand.ParticleCount = AliveParticleCount;
	RenderCommand.Flags = Flags;
	RenderCommand.ParticleTexCoords = ParticleTexCoords;
	RenderCommand.ParticleColor = ParticleColor;
	RenderCommand.ParticleSize = ParticleSize;
	RenderCommand.ParticleRotation = ParticleRotation;

	const ZEArray<ZEUInt>& RenderParticleIndices = (GetSortingEnabled()) ? SortedAliveParticleIndices : AliveParticleIndices;
	ZEArray<ZEVector4> Buffer;
	Buffer.SetCount(AliveParticleCount);
	ZESize Offset = 0;
	float Radius = ParticleSize.Length();

	for (ZESize I = 0; I < AliveParticleCount; I++)
	{
		ZESize Index = RenderParticleIndices[I];

		ZEVector3 Position = ParticlePool.Positions[Index];

		if (GetParticleLocalSpace())
			Position += GetPosition();

		if (GetLocalSpace())
			Position = GetEffect()->GetWorldTransform() * Position;

		if (RenderCommand.Flags.GetFlags(ZE_PEF_TILED_RENDERING))
			Position = Parameters->View->ViewTransform * Position;

		if (RenderCommand.Flags.GetFlags(ZE_PEF_SIZE_PER_PARTICLE))
			Radius = ParticlePool.Sizes[Index].Length();

		Buffer[Offset++] = ZEVector4(Position, Radius);
	}

	if (RenderCommand.Flags.GetFlags(ZE_PEF_SIZE_PER_PARTICLE))
	{
		Buffer.AddMultiple(AliveParticleCount);

		for (ZESize I = 0; I < AliveParticleCount; I++)
		{
			ZESize Index = RenderParticleIndices[I];

			Buffer[Offset].x = ParticlePool.Sizes[Index].x;
			Buffer[Offset++].y = ParticlePool.Sizes[Index].y;
		}
	}

	if (RenderCommand.Flags.GetFlags(ZE_PEF_ROTATION_PER_PARTICLE))
	{
		Buffer.AddMultiple(AliveParticleCount);

		for (ZESize I = 0; I < AliveParticleCount; I++)
		{
			ZESize Index = RenderParticleIndices[I];

			Buffer[Offset++].x = ParticlePool.Rotations[Index];
		}
	}

	if (RenderCommand.Flags.GetFlags(ZE_PEF_TEXCOORDS_PER_PARTICLE))
	{
		Buffer.AddMultiple(AliveParticleCount);

		for (ZESize I = 0; I < AliveParticleCount; I++)
		{
			ZESize Index = RenderParticleIndices[I];
			Buffer[Offset++] = ParticlePool.TexCoords[Index];
		}
	}

	if (RenderCommand.Flags.GetFlags(ZE_PEF_COLOR_PER_PARTICLE))
	{
		Buffer.AddMultiple(AliveParticleCount);

		for (ZESize I = 0; I < AliveParticleCount; I++)
		{
			ZESize Index = RenderParticleIndices[I];
			Buffer[Offset++] = ParticlePool.Colors[Index];
		}
	}

	ZESize DataSize = Offset * sizeof(ZEVector4);
	RenderCommand.Data = new ZEBYTE[DataSize];
	RenderCommand.DataSize = DataSize;

	memcpy(RenderCommand.Data, Buffer.GetConstCArray(), DataSize);

	//Parameters->Renderer->AddCommand(&RenderCommand);
	RenderCommand.Reset();
	Parameters->CommandList->AddCommand(&RenderCommand);

	return true;
}

void ZEParticleEmitter::SortParticles(const ZERNView& View)
{
	ZEVector3 CamPos = View.Position;
	float DistanceSqr;

	for (ZEInt I = 0; I < 32; I++)
	{
		ZEUInt Radix = (1 << I);

		ZESize Count0 = 0;
		ZESize Count1 = 0;

		for (ZESize J = 0; J < AliveParticleCount; J++)
		{
			ZEVector3 ParticlePosition = ParticlePool.Positions[AliveParticleIndices[J]];

			if (GetParticleLocalSpace())
				ParticlePosition += GetPosition();
	
			if (GetLocalSpace())
				ParticlePosition = Effect->GetWorldTransform() * ParticlePosition;

			DistanceSqr =	(CamPos.x - ParticlePosition.x) * (CamPos.x - ParticlePosition.x) +
							(CamPos.y - ParticlePosition.y) * (CamPos.y - ParticlePosition.y) +
							(CamPos.z - ParticlePosition.z) * (CamPos.z - ParticlePosition.z);

			if (!((*(ZEUInt*)&(DistanceSqr)) & Radix))
				++Count1;
		}

		for (ZESize J = 0; J < AliveParticleCount; J++)
		{
			ZEVector3 ParticlePosition = ParticlePool.Positions[AliveParticleIndices[J]];

			if (GetParticleLocalSpace())
				ParticlePosition += GetPosition();
	
			if (GetLocalSpace())
				ParticlePosition = Effect->GetWorldTransform() * ParticlePosition;

			DistanceSqr =	(CamPos.x - ParticlePosition.x) * (CamPos.x - ParticlePosition.x) +
							(CamPos.y - ParticlePosition.y) * (CamPos.y - ParticlePosition.y) +
							(CamPos.z - ParticlePosition.z) * (CamPos.z - ParticlePosition.z);

			if ((*(ZEUInt*)&(DistanceSqr)) & Radix)
			{
				SortedAliveParticleIndices[Count1] = AliveParticleIndices[J];
				++Count1;
			}
			else 
			{
				SortedAliveParticleIndices[Count0] = AliveParticleIndices[J];
				++Count0;
			}
		}
	}
}

ZEParticleEmitter* ZEParticleEmitter::CreateInstance()
{
	return new ZEParticleEmitter();
}
