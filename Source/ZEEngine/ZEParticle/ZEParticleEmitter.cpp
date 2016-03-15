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

#include "ZEParticleEffect.h"
#include "ZEParticleModifier.h"
#include "ZEMath/ZEAngle.h"
#include "ZERandom.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERenderer/ZERNStageID.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNParticleMaterial.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNStageGBuffer.h"

void ZEParticleEmitter::Tick(float TimeElapsed)
{
	AliveParticleCount = 0;

	if(Continuity == false)
	{
		if(EmittedParticleCount >= MaxParticleCount)
		{
			for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
			{
				if(ParticlePool[I].State != ZE_PAS_DEAD)
				{
					continue;
				}
				else
				{
					ParticlesPerSecond = 0;
				}
			}
		}
	}

	ParticlesRemaining += ParticlesPerSecond * TimeElapsed;
	ZEUInt ParticeCountForTimeElapsed = (ZEUInt)ParticlesRemaining;
	ParticlesRemaining -= ParticeCountForTimeElapsed;

	ZEUInt ParticlesEmmitedThisFrame = 0;

	for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].State == ZE_PAS_NEW)
		{
			ParticlePool[I].State = ZE_PAS_ALIVE;
		}
	}

	if(ParticeCountForTimeElapsed != 0)
	{
		for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
		{
			if(ParticlePool[I].State == ZE_PAS_DEAD)
			{
				GenerateParticle(ParticlePool[I]);
				EmittedParticleCount++;
				ParticlesEmmitedThisFrame++;
			}

			if(ParticlesEmmitedThisFrame == ParticeCountForTimeElapsed)
				break;
		}
	}

	for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].State == ZE_PAS_ALIVE)
		{
			ParticlePool[I].Life -= TimeElapsed;
			AliveParticleCount++;
		}

		if(ParticlePool[I].State != ZE_PAS_DEAD  && ParticlePool[I].Life < 0.0f)
		{
			ParticlePool[I].State = ZE_PAS_DEAD;
		}
	}

	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
	{
		Modifiers[I]->Tick(TimeElapsed);
	}

	if(SortingEnabled)
	{
		SortParticles();
	}
	
	//zeLog("APC : %u", AliveParticleCount);
}

void ZEParticleEmitter::InitializeParticlePool()
{
	ParticlePool.SetCount((ZESize)MaxParticleCount);

	for(ZESize I = 0; I < (ZESize)MaxParticleCount; I++)
	{
		GenerateParticle(ParticlePool[I]);
	}
}

void ZEParticleEmitter::GenerateParticle(ZEParticle &Particle)
{
	if(EmitterLocal)
	{
		switch(Type)
		{
			case ZE_PET_POINT:
			{
				Particle.Position = Position;
				break;
			}
			case ZE_PET_PLANE:
			{
				Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.x / 2.0f, PlaneSize.x / 2.0f);
				Particle.Position.y = Position.y;
				Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.y / 2.0f, PlaneSize.y / 2.0f);
				break;
			}
			case ZE_PET_BOX:
			{
				Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2.0f, BoxSize.x / 2.0f);
				Particle.Position.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2.0f, BoxSize.y / 2.0f);
				Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2.0f, BoxSize.z / 2.0f);
				break;
			}
			case ZE_PET_TORUS:
			{
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, TorusSize.y);
				Particle.Position.x = Position.x + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
				Particle.Position.y = Position.y + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
				Particle.Position.z = Position.z + TubeRadius * ZEAngle::Sin(Phi);
				break;
			}
			case ZE_PET_SPHERE:
			{
				float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, SphereRadius);
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
				Particle.Position.x = Position.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
				Particle.Position.y = Position.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
				Particle.Position.z = Position.z + Radius * ZEAngle::Cos(Phi);
				break;
			}
		}
	}
	else
	{
		ZEVector3 TempPosition;

		switch(Type)
		{
			case ZE_PET_POINT:
			{
				TempPosition = Position;
				break;
			}
			case ZE_PET_PLANE:
			{		
				TempPosition.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.x / 2, PlaneSize.x / 2);
				TempPosition.y = Position.y;
				TempPosition.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.y / 2, PlaneSize.y / 2);
				break;
			}
			case ZE_PET_BOX:
			{
				TempPosition.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2, BoxSize.x / 2);
				TempPosition.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2, BoxSize.y / 2);
				TempPosition.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2, BoxSize.z / 2);
				break;
			}
			case ZE_PET_TORUS:
			{
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, TorusSize.y);
				TempPosition.x = Position.x + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
				TempPosition.y = Position.y + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
				TempPosition.z = Position.z + TubeRadius * ZEAngle::Sin(Phi);		
				break;
			}
			case ZE_PET_SPHERE:
			{
				float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, SphereRadius);
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
				TempPosition.x = Position.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
				TempPosition.y = Position.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
				TempPosition.z = Position.z + Radius * ZEAngle::Cos(Phi);		
				break;
			}
		}

		Particle.Position = GetOwner()->GetWorldTransform() * TempPosition;
	}

	Particle.TotalLife					= RAND_BETWEEN_TWO_FLOAT(MinLife, MaxLife);
	Particle.Life						= Particle.TotalLife;

	if(ParticleFixedAspectRatio)
	{
		float ParticleAspectRatio		= MaxSize.x / MaxSize.y;
		Particle.Size2D.x				= RAND_BETWEEN_TWO_FLOAT(MinSize.x, MaxSize.x);
		Particle.Size2D.y				= Particle.Size2D.x  / ParticleAspectRatio;
	}				
	else
	{
		Particle.Size2D.x				= RAND_BETWEEN_TWO_FLOAT(MinSize.x, MaxSize.x);
		Particle.Size2D.y				= RAND_BETWEEN_TWO_FLOAT(MinSize.y, MaxSize.y);
	}

	Particle.InitialColor.x				= RAND_BETWEEN_TWO_FLOAT(MinColor.x, MaxColor.x);
	Particle.InitialColor.y				= RAND_BETWEEN_TWO_FLOAT(MinColor.y, MaxColor.y);
	Particle.InitialColor.z				= RAND_BETWEEN_TWO_FLOAT(MinColor.z, MaxColor.z);
	Particle.InitialColor.w				= RAND_BETWEEN_TWO_FLOAT(MinColor.w, MaxColor.w);
	Particle.Color						= Particle.InitialColor;
	Particle.Rotation					= 0.0f;
	Particle.Cos_NegSin					= ZEVector2::UnitY;
	Particle.MinTexCoord				= ZEVector2::Zero;
	Particle.MaxTexCoord				= ZEVector2::One;
	Particle.State						= ZE_PAS_NEW;

}

void ZEParticleEmitter::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEParticleEmitter::GetName() const
{
	return Name;
}

void ZEParticleEmitter::SetOwner(ZEParticleEffect *Owner)
{
	this->Owner = Owner;
}

ZEParticleEffect* ZEParticleEmitter::GetOwner() const
{
	return Owner;
}

void ZEParticleEmitter::SetEmitterLocal(bool EmitterLocal)
{
	this->EmitterLocal = EmitterLocal;
}

bool ZEParticleEmitter::GetEmitterLocal() const
{
	return EmitterLocal;
}

void ZEParticleEmitter::SetSortingEnabled(bool SortingEnabled)
{
	this->SortingEnabled = SortingEnabled;
}

bool ZEParticleEmitter::GetSortingEnabled() const
{
	return SortingEnabled;
}

void ZEParticleEmitter::SetParticleFixedAspectRatio(bool ParticleFixedAspectRatio)
{
	this->ParticleFixedAspectRatio = ParticleFixedAspectRatio;
}

bool ZEParticleEmitter::GetParticleFixedAspectRatio() const
{
	return ParticleFixedAspectRatio;
}

ZEArray<ZEParticle>& ZEParticleEmitter::GetParticlePool()
{
	return ParticlePool;
}

void ZEParticleEmitter::SetParticlesPerSecond(ZEUInt ParticlesPerSecond)
{
	this->ParticlesPerSecond = ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecond() const
{
	return ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetAliveParticleCount() const
{
	return AliveParticleCount;
}

void ZEParticleEmitter::SetPosition(const ZEVector3& EmitterPosition)
{
	Position = EmitterPosition;
}

const ZEVector3& ZEParticleEmitter::GetPosition() const
{
	return Position;
}

const ZEAABBox&	ZEParticleEmitter::GetBoundingBox()
{
	return BoundingBox;
}

const ZEArray<ZEParticleModifier*>& ZEParticleEmitter::GetModifiers()
{
	return Modifiers;
}

void ZEParticleEmitter::AddModifier(ZEParticleModifier* Modifier)
{
	if (Modifier == NULL)
		return;

	if (!Modifiers.Exists(Modifier))
	{
		Modifiers.Add(Modifier);
		Modifier->SetOwner(this);
	}
}

void ZEParticleEmitter::RemoveModifier(ZEParticleModifier* Modifier)
{
	if (Modifier == NULL)
		return;

	if (Modifiers.Exists(Modifier))
		Modifiers.RemoveValue(Modifier);
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

void ZEParticleEmitter::SetType(ZEParticleEmitterType EmitterType)
{
	Type = EmitterType;
}

ZEParticleEmitterType ZEParticleEmitter::GetType() const
{
	return Type;
}

void ZEParticleEmitter::SetMinColor(const ZEVector4& Color)
{
	MinColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMinColor() const
{
	return MinColor;
}

void ZEParticleEmitter::SetMaxColor(const ZEVector4& Color)
{
	MaxColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMaxColor() const
{
	return MaxColor;
}

void ZEParticleEmitter::Reset()
{
	if (Continuity == false)
		EmittedParticleCount = 0;
}

void ZEParticleEmitter::SetContinuity(bool Continuity)
{
	this->Continuity = Continuity;
}

bool ZEParticleEmitter::GetContinuity() const
{
	return Continuity;
}

void ZEParticleEmitter::SetMinLife(float MinLife)
{
	this->MinLife = MinLife;
}

float ZEParticleEmitter::GetMinLife() const
{
	return MinLife;
}

void ZEParticleEmitter::SetMaxLife(float MaxLife)
{
	this->MaxLife = MaxLife;
}

float ZEParticleEmitter::GetMaxLife() const
{
	return MaxLife;
}

void ZEParticleEmitter::SetMaxParticleCount(ZEUInt MaxParticleCount)
{
	this->MaxParticleCount = MaxParticleCount;
	
	InstanceBuffer = ZEGRConstantBuffer::Create(sizeof(InstanceAttributes) * MaxParticleCount);
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));

	ZESize OldSize = ParticlePool.GetCount();

	ParticlePool.Resize((ZESize)MaxParticleCount);
	SortArray.Resize((ZESize)MaxParticleCount);

	for (ZESize I = OldSize; I < (ZESize)MaxParticleCount; I++)
		GenerateParticle(ParticlePool[I]);

}

ZEUInt ZEParticleEmitter::GetMaxParticleCount() const
{
	return MaxParticleCount;
}

void ZEParticleEmitter::SetMinSize(const ZEVector2& MinSize)
{
	this->MinSize = MinSize;
}

const ZEVector2& ZEParticleEmitter::GetMinSize() const
{
	return MinSize;
}

void ZEParticleEmitter::SetMaxSize(const ZEVector2& MaxSize)
{
	this->MaxSize = MaxSize;
}

const ZEVector2& ZEParticleEmitter::GetMaxSize() const
{
	return MaxSize;
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

ZEParticleEmitter::ZEParticleEmitter()
{
	EmittedParticleCount = 0;
	MaxParticleCount = 30;
	Owner = NULL;
	ParticlesRemaining = 0.0f;
	EmitterLocal = false;
	SortingEnabled = false;
	ParticleFixedAspectRatio = false;
	AliveParticleCount = 0;
			
	ParticlesPerSecond = 0;				
	Position = ZEVector3(0.0f, 0.0f, 0.0f);						
	Type = ZE_PET_POINT;				
	MinColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);					
	MaxColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	
	MinSize = ZEVector2::One;					
	MaxSize = ZEVector2::One;					
	MinLife = 0.0f;					
	MaxLife = 0.0f;									
	Continuity = true;		

	BoxSize = ZEVector3::One;
	SphereRadius = 1.0f;
	TorusSize = ZEVector2::One;
	PlaneSize = ZEVector2::One;

	Material = NULL;
	
	BillboardType = ZE_PBT_SCREEN_ALIGNED;
	AxisOrientation = -ZEVector3::UnitZ;
}

ZEParticleEmitter::~ZEParticleEmitter()
{

}

bool ZEParticleEmitter::PreRender(const ZERNCullParameters* CullParameters)
{
	RenderCommand.Entity = Owner;
	RenderCommand.ExtraParameters = this;
	RenderCommand.StageMask = ZERN_STAGE_PARTICLE_RENDERING;

	CullParameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEParticleEmitter::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	if (ParticlePool.GetCount() == 0)
		return;

	ZEGRContext* Context = RenderParameters->Context;
	ZERNStage* Stage = RenderParameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	ZEMatrix4x4 WorldMatrix = ZEMatrix4x4::Identity;
	if (EmitterLocal)
		ZEMatrix4x4::CreateOrientation(WorldMatrix, GetOwner()->GetWorldPosition(), ZEQuaternion::Identity, GetOwner()->GetWorldScale());

	ConstantBuffer->SetData(&WorldMatrix);

	InstanceAttributes* InstanceAttribute;
	InstanceBuffer->Lock(reinterpret_cast<void**>(&InstanceAttribute));
	for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if (ParticlePool[I].State == ZE_PAS_ALIVE || ParticlePool[I].State == ZE_PAS_NEW)
		{
			InstanceAttribute[I].Position = ParticlePool[I].Position;
			InstanceAttribute[I].Size = ParticlePool[I].Size2D;
			InstanceAttribute[I].Color = ParticlePool[I].Color;
		}
	}
	InstanceBuffer->Unlock();

	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(RenderParameters->Renderer->GetStage(ZERN_STAGE_GBUFFER));

	ZEGRDepthStencilBuffer* PrevDepthStencilBuffer;
	ZEGRRenderTarget* PrevRenderTarget;
	Context->GetRenderTargets(1, &PrevRenderTarget, &PrevDepthStencilBuffer);

	if (Material->GetSoftParticle())
	{
		Context->SetRenderTargets(1, &PrevRenderTarget, NULL);
		Context->SetTexture(ZEGR_ST_PIXEL, 0, StageGBuffer->GetDepthMap());
	}

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, InstanceBuffer);

	Context->DrawInstanced(6, 0, ParticlePool.GetCount(), 0);

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(1, &PrevRenderTarget, PrevDepthStencilBuffer);

	Material->CleanupMaterial(Context, Stage);

}

void ZEParticleEmitter::SortParticles()
{
	ZEVector3 CamPos = zeScene->GetActiveCamera()->GetWorldPosition();
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
