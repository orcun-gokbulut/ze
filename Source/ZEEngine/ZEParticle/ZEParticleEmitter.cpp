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

#include "ZEParticleEffect.h"
#include "ZEParticleModifier.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERenderer/ZERNStageID.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNParticleMaterial.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZECore/ZECore.h"

bool ZEParticleEmitter::Initialize()
{
	InstanceBuffer = ZEGRConstantBuffer::CreateResource(sizeof(InstanceAttributes) * 1000);
	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));

	return true;
}

void ZEParticleEmitter::Deinitialize()
{
	Effect = NULL;
	ParticlePool.Clear();
	SortArray.Clear();
	Modifiers.Clear();
	
	Material.Release();

	InstanceBuffer.Release();
	ConstantBuffer.Release();
}

void ZEParticleEmitter::GenerateParticle(ZEParticle &Particle)
{
	if (Particle.State != ZE_PAS_DEAD)
		return;

	switch(Type)
	{
		case ZE_PET_POINT:
		{
			Particle.Position = Position;
			break;
		}
		case ZE_PET_PLANE:
		{
			Particle.Position.x = Position.x + ZERandom::GetFloatRange(-PlaneSize.x / 2.0f, PlaneSize.x / 2.0f);
			Particle.Position.y = Position.y;
			Particle.Position.z = Position.z + ZERandom::GetFloatRange(-PlaneSize.y / 2.0f, PlaneSize.y / 2.0f);
			break;
		}
		case ZE_PET_BOX:
		{
			Particle.Position.x = Position.x + ZERandom::GetFloatRange(-BoxSize.x / 2.0f, BoxSize.x / 2.0f);
			Particle.Position.y = Position.y + ZERandom::GetFloatRange(-BoxSize.y / 2.0f, BoxSize.y / 2.0f);
			Particle.Position.z = Position.z + ZERandom::GetFloatRange(-BoxSize.z / 2.0f, BoxSize.z / 2.0f);
			break;
		}
		case ZE_PET_TORUS:
		{
			float Theta = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float Phi = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float TubeRadius = ZERandom::GetFloatRange(0.0f, TorusSize.y);
			Particle.Position.x = Position.x + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
			Particle.Position.y = Position.y + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
			Particle.Position.z = Position.z + TubeRadius * ZEAngle::Sin(Phi);
			break;
		}
		case ZE_PET_SPHERE:
		{
			float Radius = ZERandom::GetFloatRange(0.0f, SphereRadius);
			float Theta = ZERandom::GetFloatRange(0.0f, (float)ZE_PIx2);
			float Phi = ZERandom::GetFloatRange(0.0f, ZE_PI);
			Particle.Position.x = Position.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
			Particle.Position.y = Position.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
			Particle.Position.z = Position.z + Radius * ZEAngle::Cos(Phi);
			break;
		}
	}

	Particle.TotalLife = ZERandom::GetFloatRange(ParticleLifeMin, ParticleLifeMax);
	Particle.Life = Particle.TotalLife;

	if(ParticleFixedAspectRatio)
	{
		float ParticleAspectRatio = ParticleSizeMax.x / ParticleSizeMax.y;
		Particle.Size2D.x = ZERandom::GetFloatRange(ParticleSizeMin.x, ParticleSizeMax.x);
		Particle.Size2D.y = Particle.Size2D.x  / ParticleAspectRatio;
	}				
	else
	{
		Particle.Size2D.x = ZERandom::GetFloatRange(ParticleSizeMin.x, ParticleSizeMax.x);
		Particle.Size2D.y = ZERandom::GetFloatRange(ParticleSizeMin.y, ParticleSizeMax.y);
	}

	Particle.InitialColor.x = ZERandom::GetFloatRange(ParticleColorMin.x, ParticleColorMax.x);
	Particle.InitialColor.y = ZERandom::GetFloatRange(ParticleColorMin.y, ParticleColorMax.y);
	Particle.InitialColor.z = ZERandom::GetFloatRange(ParticleColorMin.z, ParticleColorMax.z);
	Particle.InitialColor.w = ZERandom::GetFloatRange(ParticleColorMin.w, ParticleColorMax.w);
	Particle.Color = Particle.InitialColor;
	Particle.Rotation = 0.0f;
	Particle.Cos_NegSin = ZEVector2::UnitX;
	Particle.MinTexCoord = ZEVector2::Zero;
	Particle.MaxTexCoord = ZEVector2::One;
	Particle.State = ZE_PAS_NEW;

	if (GetParticalSpace() == ZE_PS_WORLD)
		Particle.Position = Effect->GetWorldTransform() * Particle.Position;
}

void ZEParticleEmitter::UpdateParticles(float ElapsedTime)
{
	// Kill Them
	if (!ParticleImmortal)
	{
		for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
		{
			if (ParticlePool[I].State == ZE_PAS_ALIVE)
				ParticlePool[I].Life -= ElapsedTime;

			if (ParticlePool[I].State != ZE_PAS_DEAD  && ParticlePool[I].Life < 0.0f)
			{
				ParticlePool[I].State = ZE_PAS_DEAD;
				ParticlePool[I].Life = 0.0f;
				AliveParticleCount--;
			}
		}
	}

	// Create Them
	ParticlesAccumulation += ZERandom::GetFloatRange((float)ParticlesPerSecondMin, (float)ParticleLifeMax) * ElapsedTime;
	if (ParticlesAccumulation >= 1.0f)
	{
		for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
		{
			if (ParticlePool[I].State == ZE_PAS_DEAD)
			{
				GenerateParticle(ParticlePool[I]);
				AliveParticleCount++;
				ParticlesAccumulation -= 1.0f;
			}

			if (ParticlesAccumulation < 1.0f)
				break;
		}
	}

	// If Pool is Filled
	if (ParticlesAccumulation >= 1.0f)
		ParticlesAccumulation -= (ZEUInt)ParticlesAccumulation;

	// Update Them
	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
	{
		if (!Modifiers[I]->GetEnabled())
			continue;

		Modifiers[I]->Tick(ElapsedTime);
	}

	for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if (ParticlePool[I].State != ZE_PAS_NEW)
			continue;
		
		ParticlePool[I].State = ZE_PAS_ALIVE;
	}

	/*if (SortingEnabled)
		SortParticles();*/
}

ZEParticleEmitter::ZEParticleEmitter()
{
	Effect = NULL;

	ParticlesAccumulation = 0.0f;
	ParticlesPerSecondMin = 0;
	ParticlesPerSecondMax = 0;
	AliveParticleCount = 0;

	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Type = ZE_PET_POINT;
	BoxSize = ZEVector3::One;
	SphereRadius = 1.0f;
	TorusSize = ZEVector2::One;
	PlaneSize = ZEVector2::One;

	ParticleSpace = ZE_PS_LOCAL;
	ParticleImmortal = false;
	ParticleLifeMin = 0.0f;		
	ParticleLifeMax = 0.0f;									
	ParticleSizeMin = ZEVector2::One;					
	ParticleSizeMax = ZEVector2::One;
	ParticleColorMin = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	ParticleColorMax = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	ParticleFixedAspectRatio = false;

	BillboardType = ZE_PBT_SCREEN_ALIGNED;
	AxisOrientation = -ZEVector3::UnitZ;
	SortingEnabled = false;
}

ZEParticleEmitter::~ZEParticleEmitter()
{
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

const ZEArray<ZEParticle>& ZEParticleEmitter::GetPool() const
{
	return ParticlePool;
}

void ZEParticleEmitter::SetPoolSize(ZESize PoolSize)
{
	if (ParticlePool.GetCount() == PoolSize)
		return;

	ZESize OldSize = ParticlePool.GetCount();
	ParticlePool.Resize((ZESize)PoolSize);
	SortArray.Resize((ZESize)PoolSize);

	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
		Modifiers[I]->PoolSizeChanged(PoolSize);
}

ZESize ZEParticleEmitter::GetPoolSize() const
{
	return ParticlePool.GetCount();
}

ZEUInt ZEParticleEmitter::GetAliveParticleCount() const
{
	return AliveParticleCount;
}

void ZEParticleEmitter::SetSortingEnabled(bool SortingEnabled)
{
	this->SortingEnabled = SortingEnabled;
}

bool ZEParticleEmitter::GetSortingEnabled() const
{
	return SortingEnabled;
}

void ZEParticleEmitter::SetParticleSpace(ZEParticleSpace Space)
{
	ParticleSpace = Space;
}

ZEParticleSpace ZEParticleEmitter::GetParticalSpace() const
{
	return ParticleSpace;
}

void ZEParticleEmitter::SetParticlesPerSecond(ZEUInt ParticlesPerSecond)
{
	ParticlesPerSecondMin = ParticlesPerSecond;
	ParticlesPerSecondMax = ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecond() const
{
	return (ParticlesPerSecondMin + ParticlesPerSecondMax) / 2;
}

void ZEParticleEmitter::SetParticlesPerSecondMin(ZEUInt ParticlesPerSecond)
{
	ParticlesPerSecondMin = ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecondMin() const
{
	return ParticlesPerSecondMin;
}

void ZEParticleEmitter::SetParticlesPerSecondMax(ZEUInt ParticlesPerSecond)
{
	ParticlesPerSecondMax = ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecondMax() const
{
	return ParticlesPerSecondMax;
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

void ZEParticleEmitter::SetParticleSizeMin(const ZEVector2& MinSize)
{
	this->ParticleSizeMin = MinSize;
}

const ZEVector2& ZEParticleEmitter::GetParticleSizeMin() const
{
	return ParticleSizeMin;
}

void ZEParticleEmitter::SetParticleSizeMax(const ZEVector2& MaxSize)
{
	this->ParticleSizeMax = MaxSize;
}

const ZEVector2& ZEParticleEmitter::GetParticleSizeMax() const
{
	return ParticleSizeMax;
}


void ZEParticleEmitter::SetParticleColorMin(const ZEVector4& Color)
{
	ParticleColorMin = Color;
}

const ZEVector4& ZEParticleEmitter::GetParticleColorMin() const
{
	return ParticleColorMin;
}

void ZEParticleEmitter::SetParticleColorMax(const ZEVector4& Color)
{
	ParticleColorMax = Color;
}

const ZEVector4& ZEParticleEmitter::GetParticleColorMax() const
{
	return ParticleColorMax;
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

	Modifier->Emitter = this;
	Modifiers.Add(Modifier);
}

void ZEParticleEmitter::RemoveModifier(ZEParticleModifier* Modifier)
{
	zeCheckError(Modifier == NULL, ZE_VOID, "Modifier is NULL.");
	zeCheckError(Modifier->Emitter != this, ZE_VOID, "Modifier is not registered to this Particle Emitter.");

	Modifier->Emitter = NULL;
	Modifiers.RemoveValue(Modifier);
}

const ZEAABBox&	ZEParticleEmitter::GetBoundingBox() const
{
	return BoundingBox;
}

void ZEParticleEmitter::SetBillboardType(ZEParticleBillboardType Type)
{
	BillboardType = Type;
}

ZEParticleBillboardType ZEParticleEmitter::GetBillboardType() const
{
	return BillboardType;
}

void ZEParticleEmitter::SetAxisOrientation(const ZEVector3& Orientation)
{
	AxisOrientation = Orientation;
}

const ZEVector3& ZEParticleEmitter::GetAxisOrientation() const
{
	return AxisOrientation;
}

void ZEParticleEmitter::SetMaterial(ZEHolder<ZERNParticleMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<ZERNParticleMaterial> ZEParticleEmitter::GetMaterial() const
{
	return Material;
}

void ZEParticleEmitter::GeneratePool()
{
	AliveParticleCount = ParticlePool.GetCount();
	for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
		GenerateParticle(ParticlePool[I]);

	UpdateParticles(0.0f);
}

void ZEParticleEmitter::ResetPool()
{
	AliveParticleCount = 0;
	for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
		ParticlePool[I].State = ZE_PAS_NEW;
}

void ZEParticleEmitter::Tick(float TimeElapsed)
{

}

bool ZEParticleEmitter::PreRender(const ZERNPreRenderParameters* Parameters)
{
	UpdateParticles(zeCore->GetElapsedTime());

	if (GetAliveParticleCount() == 0)
		return false;

	RenderCommand.ExtraParameters = this;
	RenderCommand.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEParticleEmitter, Render, this);

	if (!Material->PreRender(RenderCommand))
		return false;
	
	Parameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEParticleEmitter::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = RenderParameters->Context;
	const ZERNStage* Stage = RenderParameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	if (GetParticalSpace() == ZE_PS_LOCAL)
	{
		ZEMatrix4x4 WorldMatrix;
		ZEMatrix4x4::CreateOrientation(WorldMatrix, GetEffect()->GetWorldPosition(), ZEQuaternion::Identity, GetEffect()->GetWorldScale());
		ConstantBuffer->SetData(&WorldMatrix);
	}
	else
	{
		ConstantBuffer->SetData(&ZEMatrix4x4::Identity);
	}

	InstanceAttributes* InstanceAttribute;
	InstanceBuffer->Lock(reinterpret_cast<void**>(&InstanceAttribute));
	ZESize ParticleCount = ParticlePool.GetCount();
	for (ZESize I = 0; I < ParticleCount; I++)
	{
		if (ParticlePool[I].State == ZE_PAS_ALIVE || ParticlePool[I].State == ZE_PAS_NEW)
		{
			InstanceAttribute[I].Position = ParticlePool[I].Position;
			InstanceAttribute[I].Size = ParticlePool[I].Size2D;
			InstanceAttribute[I].Cos_NegSin = ParticlePool[I].Cos_NegSin;
			InstanceAttribute[I].Color = ParticlePool[I].Color;
		}
	}
	InstanceBuffer->Unlock();

	const ZEGRTexture* DepthMap = Stage->GetOutput(ZERN_SO_DEPTH);

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, InstanceBuffer);
	Context->SetTextures(ZEGR_ST_PIXEL, 4, 1, &DepthMap);

	Context->DrawInstanced(4, 0, ParticleCount, 0);

	Material->CleanupMaterial(Context, Stage);
}

void ZEParticleEmitter::SortParticles(const ZERNView& View)
{
	ZEVector3 CamPos = View.Position;
	float DistanceSqr;

	for (ZEInt I = 0; I < 32; I++)
	{
		ZEUInt Radix = (1 << I);
		ZESize ParticleCount = ParticlePool.GetCount();

		ZESize Count0 = 0;
		ZESize Count1 = 0;

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			DistanceSqr =	(CamPos.x - ParticlePool[J].Position.x) * (CamPos.x - ParticlePool[J].Position.x) +
							(CamPos.y - ParticlePool[J].Position.y) * (CamPos.y - ParticlePool[J].Position.y) +
							(CamPos.z - ParticlePool[J].Position.z) * (CamPos.z - ParticlePool[J].Position.z);

			if (!((*(ZEUInt*)&(DistanceSqr)) & Radix))
				++Count1;
		}

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			DistanceSqr =	(CamPos.x - ParticlePool[J].Position.x) * (CamPos.x - ParticlePool[J].Position.x) +
							(CamPos.y - ParticlePool[J].Position.y) * (CamPos.y - ParticlePool[J].Position.y) +
							(CamPos.z - ParticlePool[J].Position.z) * (CamPos.z - ParticlePool[J].Position.z);

			if ((*(ZEUInt*)&(DistanceSqr)) & Radix)
			{
				SortArray[Count1]=ParticlePool[J];
				++Count1;
			}
			else 
			{
				SortArray[Count0]=ParticlePool[J];
				++Count0;
			}
		}

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			ParticlePool[J] = SortArray[J];
		}
	}
}

ZEParticleEmitter* ZEParticleEmitter::CreateInstance()
{
	return new ZEParticleEmitter();
}
