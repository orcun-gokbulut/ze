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

#include "ZEParticleEffect.h"
#include "ZEParticleEmitter.h"
#include "ZEParticleModifier.h"
#include "ZEMath/ZEAngle.h"
#include "ZERandom.h"

#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEVertexTypes.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGame/ZEDrawParameters.h"

#define RAND_BETWEEN_TWO_FLOAT(Min, Max) (((Max) - (Min)) * ZERandom::GetFloat() + (Min))

void ZEParticleEmitter::Tick(float TimeElapsed)
{
	float ParticleCreationTime = 1.0f / (float)ParticlesPerSecond;

	BoundingBox.Min = ZEVector3::MaxValue;
	BoundingBox.Max = ZEVector3::MinValue;

	LastCreation += TimeElapsed;

	if(LastCreation > ParticleCreationTime)
	{
		for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
		{
			if(ParticlePool[I].IsAlive == false)
			{
				GenerateParticle(ParticlePool[I]);
				EmittedParticleCount++;
				break;
			}			
		}

		LastCreation = 0.0f;
	}

	for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].IsAlive == true)
		{
			ParticlePool[I].Position += (ParticlePool[I].Velocity * TimeElapsed);
			ParticlePool[I].Velocity += ParticlePool[I].Acceleration;
			ParticlePool[I].Life -= TimeElapsed;

			ZEVector3 Size = ZEVector3(ParticlePool[I].Size, ParticlePool[I].Size, ParticlePool[I].Size) * 0.5f;
			ZEVector3::Max(BoundingBox.Max, BoundingBox.Max, ParticlePool[I].Position + Size);
			ZEVector3::Min(BoundingBox.Min, BoundingBox.Min, ParticlePool[I].Position - Size);
		}

		if(ParticlePool[I].Life < 0.0f)	
			ParticlePool[I].IsAlive = false;
	}

	if(IsContinuous == false)
	{
		if(EmittedParticleCount > MaxParticleCount)
			for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
			{
				if(ParticlePool[I].IsAlive == true)
					return;
				else
					Owner->RemoveEmitter(this);
			}
	}

	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
		Modifiers[I]->Tick(TimeElapsed);
}

void ZEParticleEmitter::InitializeParticlePool()
{
	ParticlePool.SetCount(MaxParticleCount);

	for(ZESize I = 0; I < MaxParticleCount; I++)
		GenerateParticle(ParticlePool[I]);
}

void ZEParticleEmitter::GenerateParticle(ZEParticle &Particle)
{
	switch(Type)
	{
		case ZE_PET_POINT:
			Particle.Position = Position;
			break;
		case ZE_PET_BOX:
			Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2, BoxSize.x / 2);
			Particle.Position.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2, BoxSize.y / 2);
			Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2, BoxSize.z / 2);
			break;
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

	Particle.UpVector.x					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.x, MaxUpVector.x);
	Particle.UpVector.y					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.y, MaxUpVector.y);
	Particle.UpVector.z					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.z, MaxUpVector.z);
	ZEVector3::Normalize(Particle.UpVector, Particle.UpVector);

	Particle.Acceleration.x				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
	Particle.Acceleration.y				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
	Particle.Acceleration.z				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);
	Particle.Velocity.x					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
	Particle.Velocity.y					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
	Particle.Velocity.z					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
	Particle.AngularAcceleration.x		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
	Particle.AngularAcceleration.y		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
	Particle.AngularAcceleration.z		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);
	Particle.AngularVelocity.x			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
	Particle.AngularVelocity.y			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
	Particle.AngularVelocity.z			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
	Particle.Life						= RAND_BETWEEN_TWO_FLOAT(MinLife, MaxLife);
	Particle.Size						= RAND_BETWEEN_TWO_FLOAT(MinSize, MaxSize);
	Particle.BounceFactor				= RAND_BETWEEN_TWO_FLOAT(MinBounceFactor, MaxBounceFactor);
	Particle.Color.x					= RAND_BETWEEN_TWO_FLOAT(MinColor.x, MaxColor.x);
	Particle.Color.y					= RAND_BETWEEN_TWO_FLOAT(MinColor.y, MaxColor.y);
	Particle.Color.z					= RAND_BETWEEN_TWO_FLOAT(MinColor.z, MaxColor.z);
	Particle.Color.w					= RAND_BETWEEN_TWO_FLOAT(MinColor.w, MaxColor.w);
	Particle.IsAlive					= true;
}

void ZEParticleEmitter::SetAcceleration(const ZEVector3& EmitterAcceleration)
{
	Acceleration = EmitterAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetAcceleration() const
{
	return Acceleration;
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

void ZEParticleEmitter::SetParticlesPerSecond(ZEUInt Value)
{
	ParticlesPerSecond = Value;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecond() const
{
	return ParticlesPerSecond;
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
	Modifier->SetOwner(this);
	Modifiers.Add(Modifier);
}

void ZEParticleEmitter::RemoveModifier(ZEParticleModifier* Modifier)
{
	if (!Modifiers.Exists(Modifier))
		Modifiers.Add(Modifier);
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

void ZEParticleEmitter::SetUpVector(const ZEVector3& EmitterUpVector)
{
	UpVector = EmitterUpVector;
}

const ZEVector3& ZEParticleEmitter::GetUpVector() const
{
	return UpVector;
}

void ZEParticleEmitter::SetVelocity(const ZEVector3& EmitterVelocity)
{
	Velocity = EmitterVelocity;
}

const ZEVector3& ZEParticleEmitter::GetVelocity() const
{
	return Velocity;
}

void ZEParticleEmitter::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAcceleration() const
{
	return MinAcceleration;
}

void ZEParticleEmitter::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAcceleration() const
{
	return MaxAcceleration;
}

void ZEParticleEmitter::SetMinAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularAcceleration() const
{
	return MinAngularAcceleration;
}

void ZEParticleEmitter::SetMaxAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularAcceleration() const
{
	return MaxAngularAcceleration;
}

void ZEParticleEmitter::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMinVelocity() const
{
	return MinVelocity;
}

void ZEParticleEmitter::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxVelocity() const
{
	return MaxVelocity;
}

void ZEParticleEmitter::SetMinAngularVelocity(const ZEVector3& AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularVelocity() const
{
	return MinAngularVelocity;
}

void ZEParticleEmitter::SetMaxAngularVelocity(const ZEVector3& AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularVelocity() const
{
	return MaxAngularVelocity;
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

void ZEParticleEmitter::SetMinBounceFactor(float BounceFactor)
{
	MinBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMinBounceFactor() const
{
	return MinBounceFactor;
}

void ZEParticleEmitter::SetMaxBounceFactor(float BounceFactor)
{
	MaxBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMaxBounceFactor() const
{
	return MaxBounceFactor;
}

void ZEParticleEmitter::SetContinuity(bool Value)
{
	IsContinuous = Value;
}

bool ZEParticleEmitter::GetContinuity() const
{
	return IsContinuous;
}

void ZEParticleEmitter::SetMinLife(float Life)
{
	MinLife = Life;
}

float ZEParticleEmitter::GetMinLife() const
{
	return MinLife;
}

void ZEParticleEmitter::SetMaxLife(float Life)
{
	MaxLife = Life;
}

float ZEParticleEmitter::GetMaxLife() const
{
	return MaxLife;
}

void ZEParticleEmitter::SetMaxParticleCount(ZEUInt Value)
{
	MaxParticleCount = Value;

	ZESize OldSize = ParticlePool.GetCount();
	ParticlePool.Resize(Value);
	for (ZESize I = OldSize; I < MaxParticleCount; I++)
		GenerateParticle(ParticlePool[I]);

}

ZEUInt ZEParticleEmitter::GetMaxParticleCount() const
{
	return MaxParticleCount;
}

void ZEParticleEmitter::SetMinSize(float Size)
{
	MinSize = Size;
}

float ZEParticleEmitter::GetMinSize() const
{
	return MinSize;
}

void ZEParticleEmitter::SetMaxSize(float Size)
{
	MaxSize = Size;
}

float ZEParticleEmitter::GetMaxSize() const
{
	return MaxSize;
}

void ZEParticleEmitter::SetMinUpVector(const ZEVector3& UpVector)
{
	MinUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMinUpVector() const
{
	return MinUpVector;
}

void ZEParticleEmitter::SetMaxUpVector(const ZEVector3& UpVector)
{
	MaxUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMaxUpVector() const
{
	return MinUpVector;
}

void ZEParticleEmitter::SetBillboardType(ZEParticleBillboardType Type)
{
	BillboardType = Type;
}

ZEParticleBillboardType ZEParticleEmitter::GetBillboardType() const
{
	return BillboardType;
}

void ZEParticleEmitter::SetMaterial(ZEMaterial *Material)
{
	this->Material = Material;
}

ZEMaterial* ZEParticleEmitter::GetMaterial() const
{
	return Material;
}

ZEParticleEmitter::ZEParticleEmitter()
{
	EmittedParticleCount = 0;
	MaxParticleCount = 0;
	LastCreation = 0.0f;
	Owner = NULL;

	Velocity = ZEVector3(0.0f, 0.0f, 0.0f);		
	Acceleration = ZEVector3(0.0f, 0.0f, 0.0f);					
	ParticlesPerSecond = 0;				
	Position = ZEVector3(0.0f, 0.0f, 0.0f);						
	Type = ZE_PET_POINT;	
	MinAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MaxAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MinVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
	MaxVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
	MinAngularAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);		
	MaxAngularAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);		
	MinAngularVelocity = ZEVector3(0.0f, 0.0f, 0.0f);			
	MaxAngularVelocity = ZEVector3(0.0f, 0.0f, 0.0f);			
	MinColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);					
	MaxColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	MinUpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	MaxUpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	UpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	
	MinSize = 0.0f;					
	MaxSize = 0.0f;					
	MinLife = 0.0f;					
	MaxLife = 0.0f;					
	MinBounceFactor = 0.0f;			
	MaxBounceFactor = 0.0f;			
	MaxParticleCount = 0;			
	IsContinuous = true;		

	BoxSize = ZEVector3(1.0f, 1.0f, 1.0f);
	SphereRadius = 1.0f;
	TorusSize = ZEVector2(1.0f, 1.0f);

	VertexBuffer = NULL;
	Material = NULL;

	RenderCommand.SetZero();
	RenderCommand.Priority = 4;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_NO_Z_WRITE;
	RenderCommand.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	
	BillboardType = ZE_PBT_SCREEN_ALIGNED;
}

ZEParticleEmitter::~ZEParticleEmitter()
{
}


static void DrawParticle(ZESimpleVertex* Buffer, const ZEParticle* Particle, const ZEVector3& Right, const ZEVector3& Up)
{
	float ParticleSize_2 = Particle->Size * 0.5f;

	ZEVector3 PU, NU, PV, NV;
	ZEVector3::Scale(PV, Right, ParticleSize_2);
	ZEVector3::Scale(NV, Right, -ParticleSize_2);
	ZEVector3::Scale(PU, Up, ParticleSize_2);
	ZEVector3::Scale(NU, Up, -ParticleSize_2);


	Buffer[0].Position = Particle->Position + NV + PU;
	Buffer[0].Texcoord = ZEVector2(0.0f, 0.0f);

	Buffer[1].Position = Particle->Position + PV + PU;
	Buffer[1].Texcoord = ZEVector2(1.0f, 0.0f);

	Buffer[2].Position = Particle->Position + PV + NU;
	Buffer[2].Texcoord = ZEVector2(1.0f, 1.0f);

	Buffer[3].Position = Buffer[2].Position;
	Buffer[3].Texcoord = Buffer[2].Texcoord;

	Buffer[4].Position = Particle->Position + NV + NU;
	Buffer[4].Texcoord = ZEVector2(0.0f, 1.0f);

	Buffer[5].Position = Buffer[0].Position;
	Buffer[5].Texcoord = Buffer[0].Texcoord;
}

void ZEParticleEmitter::UpdateVertexBuffer(ZEDrawParameters* DrawParameters)
{
	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (VertexBuffer->GetBufferSize() != ParticlePool.GetCount() * sizeof(ZESimpleVertex) * 6)
	{
		if (!VertexBuffer->Create(ParticlePool.GetCount() * sizeof(ZESimpleVertex) * 6))
		{
			zeError("Could not create particle vertex buffer.");
			return;
		}
	}

	RenderCommand.PrimitiveCount = 0;

	ZESimpleVertex* Buffer = (ZESimpleVertex*)VertexBuffer->Lock();
	if (Buffer == NULL)
	{
		zeError("Could not lock particle vertex buffer.");
		return;
	}

	if (BillboardType == ZE_PBT_NONE)
	{
		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticlePool.GetCount(); N++)
		{
			ZEParticle* Particle = &ParticlePool[N];
			if (Particle->IsAlive)
			{
				DrawParticle(Buffer + VertexIndex, Particle, ZEVector3::UnitX, Particle->UpVector);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}

	}
	else if (BillboardType == ZE_PBT_SCREEN_ALIGNED)
	{
		ZEVector3 CameraRight = DrawParameters->View->Camera->GetWorldRight();
		ZEVector3 CameraUp = DrawParameters->View->Camera->GetWorldUp();

		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticlePool.GetCount(); N++)
		{
			ZEParticle* Particle = &ParticlePool[N];
			if (Particle->IsAlive)
			{
				DrawParticle(Buffer + VertexIndex, Particle, CameraRight, CameraUp);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_VIEW_PLANE_ALIGNED)
	{	
		ZEMatrix4x4 InverseWorld;
		ZEMatrix4x4::Inverse(InverseWorld, Owner->GetWorldTransform());
		ZEMatrix4x4 Transform;
		ZEMatrix4x4::Multiply(Transform, InverseWorld, DrawParameters->View->ViewTransform);
		ZEVector3 CameraDirection = DrawParameters->View->Camera->GetWorldDirection();

		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticlePool.GetCount(); N++)
		{
			ZEParticle* Particle = &ParticlePool[N];
			if (Particle->IsAlive)
			{
				ZEVector3 V, U;
				ZEVector3::CrossProduct(V, Particle->UpVector, CameraDirection);
				ZEVector3::CrossProduct(U, CameraDirection, V);

				DrawParticle(Buffer + VertexIndex, Particle, V, U);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_VIEW_POINT_ORIENTED)
	{
		ZEVector3 CameraPosition = DrawParameters->View->Camera->GetWorldPosition();
		ZEVector3 CameraUp = DrawParameters->View->Camera->GetWorldUp();
		ZEVector3 CameraDirection = DrawParameters->View->Camera->GetWorldDirection();

		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticlePool.GetCount(); N++)
		{
			ZEParticle* Particle = &ParticlePool[N];
			if (Particle->IsAlive)
			{
				ZEVector3 Direction;
				ZEVector3::Sub(Direction, Particle->Position, CameraPosition);
				ZEVector3::Normalize(Direction, Direction);

				if (!Direction.IsValid())
					Direction = CameraDirection;

				ZEVector3 V, U;
				ZEVector3::CrossProduct(V, Particle->UpVector, Direction);
				ZEVector3::CrossProduct(U, Direction, V);

				DrawParticle(Buffer + VertexIndex, Particle, V, U);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}

	VertexBuffer->Unlock();
}

void ZEParticleEmitter::Draw(ZEDrawParameters* DrawParameters)
{
	if (ParticlePool.GetCount() == 0)
		return;

	UpdateVertexBuffer(DrawParameters);

	if (RenderCommand.PrimitiveCount == 0)
		return;

	if (VertexBuffer == NULL || Material == NULL)
		return;

	RenderCommand.WorldMatrix = GetOwner()->GetWorldTransform();
	RenderCommand.VertexBuffer = VertexBuffer;
	RenderCommand.Material = Material;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

ZEParticleEmitter* ZEParticleEmitter::CreateInstance()
{
	return new ZEParticleEmitter();
}
