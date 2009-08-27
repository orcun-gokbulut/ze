//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsBody.cpp
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

#include "NxPhysics.h"
#include "NxCooking.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"

#include "Core/Module.h"
#include "Physics/PhysicsModule.h"
#include "Physics/Aegia/AegiaPhysicsModule.h"
#include "Physics/Aegia/AegiaPhysicsStream.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Aegia/AegiaPhysicsWorld.h"

#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsBodyInfo.h"
#include "Physics/Aegia/AegiaPhysicsBody.h"

#include "Physics/PhysicsMaterial.h"
#include "Physics/Aegia/AegiaPhysicsMaterial.h"

#include "Physics/PhysicsShapeInfo.h"
#include "Physics/PhysicsBoxShapeInfo.h"
#include "Physics/PhysicsCapsuleShapeInfo.h"
#include "Physics/PhysicsConvexShapeInfo.h"
#include "Physics/PhysicsPlaneShapeInfo.h"
#include "Physics/PhysicsSphereShapeInfo.h"
#include "Physics/PhysicsTrimeshShapeInfo.h"

#include "Physics/PhysicsCharacterController.h"
#include "Physics/Aegia/AegiaPhysicsCharacterController.h"

ZEAegiaPhysicsBody::ZEAegiaPhysicsBody() : Actor(NULL)
{
}

ZEAegiaPhysicsBody::~ZEAegiaPhysicsBody()
{
	Deinitialize();
}

void ZEAegiaPhysicsBody::Initialize(ZEPhysicsBodyInfo& Info)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();

	if (Actor == NULL && World != NULL)
	{
		NxActorDesc ActorDesc;
		NxBodyDesc BodyDesc;
		//set body desc
		if (Info.Mass>0)
		{
			ActorDesc.body = &BodyDesc;
			ActorDesc.density = 1;
		}
		//set shape descs
		for (int i=0; i< (int)Info.ShapeInfos.GetCount(); i++)
		{
			switch (Info.ShapeInfos[i]->GetType())
			{
				case ZEPhysicsShapeInfo::ZE_PST_PLANE:
				{
					ZEPhysicsPlaneShapeInfo* PlaneInfo = (ZEPhysicsPlaneShapeInfo*)Info.ShapeInfos[i];
					NxPlaneShapeDesc PlaneDesc;
					PlaneDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (PlaneInfo->Trigger)PlaneDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					PlaneDesc.d = PlaneInfo->Height;
					PlaneDesc.normal = TONX(PlaneInfo->Normal);
					PlaneDesc.localPose.t = TONX(PlaneInfo->LocalPosition);
					PlaneDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(PlaneInfo->CollisionMask);
					if (PlaneInfo->Material)
					{
						PlaneDesc.materialIndex = PlaneInfo->Material->GetIndex();
					}
					ActorDesc.shapes.pushBack(&PlaneDesc);
					break;
				}
				case ZEPhysicsShapeInfo::ZE_PST_BOX:
				{
					ZEPhysicsBoxShapeInfo* BoxInfo = (ZEPhysicsBoxShapeInfo*)Info.ShapeInfos[i];
					NxBoxShapeDesc BoxDesc;
					BoxDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (BoxInfo->Trigger)BoxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					BoxDesc.dimensions = TONX(BoxInfo->Dimensions);
					BoxDesc.localPose.t = TONX(BoxInfo->LocalPosition);
					BoxDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(BoxInfo->CollisionMask);
					if (BoxInfo->Material)
					{
						BoxDesc.materialIndex = BoxInfo->Material->GetIndex();
					}
					ActorDesc.shapes.pushBack(&BoxDesc);
					break;
				}
				case ZEPhysicsShapeInfo::ZE_PST_CAPSULE:
				{
					ZEPhysicsCapsuleShapeInfo* CapsuleInfo = (ZEPhysicsCapsuleShapeInfo*)Info.ShapeInfos[i];
					NxCapsuleShapeDesc CapsuleDesc;
					CapsuleDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (CapsuleInfo->Trigger)CapsuleDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					CapsuleDesc.height = CapsuleInfo->Height;
					CapsuleDesc.radius = CapsuleInfo->Radius;
					CapsuleDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(CapsuleInfo->CollisionMask);;
					CapsuleDesc.localPose.t = TONX(CapsuleInfo->LocalPosition);
					if (CapsuleInfo->Material)
					{
						CapsuleDesc.materialIndex = CapsuleInfo->Material->GetIndex();
					}
					ActorDesc.shapes.pushBack(&CapsuleDesc);
					break;
				}
				case ZEPhysicsShapeInfo::ZE_PST_SPHERE:
				{
					ZEPhysicsSphereShapeInfo* SphereInfo = (ZEPhysicsSphereShapeInfo*)Info.ShapeInfos[i];
					NxSphereShapeDesc SphereDesc;
					SphereDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (SphereInfo->Trigger)SphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					SphereDesc.radius = SphereInfo->Radius;
					SphereDesc.localPose.t = TONX(SphereInfo->LocalPosition);
					SphereDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(SphereInfo->CollisionMask);;
					if (SphereInfo->Material)
					{
						SphereDesc.materialIndex = SphereInfo->Material->GetIndex();
					}
					ActorDesc.shapes.pushBack(&SphereDesc);
					break;
				}
				case ZEPhysicsShapeInfo::ZE_PST_CONVEX:
				{
					ZEPhysicsConvexShapeInfo* ConvexInfo = (ZEPhysicsConvexShapeInfo*)Info.ShapeInfos[i];
					NxConvexShapeDesc ConvexDesc;
					ConvexDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (ConvexInfo->Trigger)ConvexDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					ConvexDesc.localPose.t = TONX(ConvexInfo->LocalPosition);
					ConvexDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(ConvexInfo->CollisionMask);;

					//first prepare vertices
					NxVec3* Verts = new NxVec3[ConvexInfo->Vertices.GetCount()];
					for (int i=0; i<(int)ConvexInfo->Vertices.GetCount(); i++)
					{
						Verts[i] = TONX(ConvexInfo->Vertices[i]);
						/*Verts[i].x *= ConvexInfo->Scale.x;
						Verts[i].y *= ConvexInfo->Scale.y;
						Verts[i].z *= ConvexInfo->Scale.z;*/
					}

					//prepare mesh description
					NxConvexMeshDesc ConvexMeshDesc;
					ConvexMeshDesc.numVertices		= (NxU32)ConvexInfo->Vertices.GetCount();
					ConvexMeshDesc.pointStrideBytes	= sizeof(NxVec3);
					ConvexMeshDesc.points			= Verts;
					ConvexMeshDesc.flags			= NX_CF_COMPUTE_CONVEX;

					//cook the mesh
					if (World->GetCooker()->NxInitCooking())
					{
						//Cooking from memory    
						ZEAegiaPhysicsMemoryWriteBuffer Buffer;
						if (World->GetCooker()->NxCookConvexMesh(ConvexMeshDesc, Buffer))
						{
							//set mesh data
							ConvexDesc.meshData = World->GetSdk()->createConvexMesh(ZEAegiaPhysicsMemoryReadBuffer(Buffer.data));
						}
						else
						{
							//Creation Error
						}
					}
					else
					{
						//Creation Error
					}

					//delete temp
					delete [] Verts;

					//set material
					if (ConvexInfo->Material)
					{
						ConvexDesc.materialIndex = ConvexInfo->Material->GetIndex();
					}

					ActorDesc.shapes.pushBack(&ConvexDesc);
					break;
				}
				case ZEPhysicsShapeInfo::ZE_PST_TRIMESH:
				{
					ZEPhysicsTrimeshShapeInfo* TrimeshInfo = (ZEPhysicsTrimeshShapeInfo*)Info.ShapeInfos[i];
					NxTriangleMeshShapeDesc TrimeshDesc;
					TrimeshDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					if (TrimeshInfo->Trigger)TrimeshDesc.shapeFlags |= NX_TRIGGER_ENABLE;
					TrimeshDesc.localPose.t = TONX(TrimeshInfo->LocalPosition);
					TrimeshDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(TrimeshInfo->CollisionMask);;

					//first prepare vertices
					NxVec3* Verts = new NxVec3[TrimeshInfo->Vertices.GetCount()];
					for (int i=0; i<(int)TrimeshInfo->Vertices.GetCount(); i++)
					{
						Verts[i] = TONX(TrimeshInfo->Vertices[i]);
						/*Verts[i].x *= TrimeshInfo->Scale.x;
						Verts[i].y *= TrimeshInfo->Scale.y;
						Verts[i].z *= TrimeshInfo->Scale.z;*/
					}

					//prepare indexes
					unsigned int* Indxs = new unsigned int [TrimeshInfo->Indexes.GetCount()];
					for (int i=0; i<(int)TrimeshInfo->Indexes.GetCount(); i++)
					{
						Indxs[i] = TrimeshInfo->Indexes[i];
					}

					//prepare mesh description
					NxTriangleMeshDesc TriangleMeshDesc;
					TriangleMeshDesc.numVertices = (NxU32)TrimeshInfo->Vertices.GetCount();
					TriangleMeshDesc.numTriangles = (NxU32)TrimeshInfo->Indexes.GetCount()/3;
					TriangleMeshDesc.pointStrideBytes = sizeof(NxVec3);
					TriangleMeshDesc.triangleStrideBytes = sizeof(NxU32) * 3;
					TriangleMeshDesc.points = Verts;
					TriangleMeshDesc.triangles = Indxs;
					TriangleMeshDesc.flags = 0;

					//cook the mesh
					if (World->GetCooker()->NxInitCooking())
					{
						//Cooking from memory    
						ZEAegiaPhysicsMemoryWriteBuffer Buffer;
						if (World->GetCooker()->NxCookTriangleMesh(TriangleMeshDesc, Buffer))
						{
							//init convex desc
							TrimeshDesc.meshData = World->GetSdk()->createTriangleMesh(ZEAegiaPhysicsMemoryReadBuffer(Buffer.data));
						}
						else
						{
							//Creation Error
						}
					}
					else
					{
						//Creation Error
					}

					//delete temp
					delete [] Verts;
					delete [] Indxs;

					//set material
					if (TrimeshInfo->Material)
					{
						TrimeshDesc.materialIndex = TrimeshInfo->Material->GetIndex();
					}

					ActorDesc.shapes.pushBack(&TrimeshDesc);
					break;
				}
			}
		}
		//create
		if (ActorDesc.isValid())
		{
			Actor = World->GetScene()->createActor(ActorDesc);
			//set attributes
			Actor->setGlobalPosition(TONX(Info.Position));
			Actor->setGlobalOrientationQuat(TONX(Info.Orientation));
			if (Info.Mass>0)Actor->updateMassFromShapes(0,Info.Mass);
			Actor->setLinearDamping(Info.LinearDamp);
			Actor->setAngularDamping(Info.AngularDamp);
			if (Info.Kinematic)Actor->raiseBodyFlag(NX_BF_KINEMATIC);
			//set user data to this
			Actor->userData = this;
			NxShape*const* shapes = Actor->getShapes();
			NxU32 nShapes = Actor->getNbShapes();
			while (nShapes--)
			{
				shapes[nShapes]->userData = this;
			}
			//Actor->raiseBodyFlag(NX_BF_FROZEN_ROT);
		}
		else
		{
		}
	}
}

void ZEAegiaPhysicsBody::Deinitialize()
{
	if (Actor != NULL && ZEAegiaPhysicsWorld::getSingletonPtr() != NULL)
	{
		ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();
		if (Actor->userData != NULL)
		{
			delete Actor->userData;
		}
        //ReleaseUserDataFromShapes(actor);
		World->GetScene()->releaseActor(*Actor);
		Actor = NULL;
	}
}

void ZEAegiaPhysicsBody::AddShape(ZEPhysicsShapeInfo* Info)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();

	if (Actor != NULL && World != NULL)
	{
		switch (Info->GetType())
		{
			case ZEPhysicsShapeInfo::ZE_PST_PLANE:
			{
				ZEPhysicsPlaneShapeInfo* PlaneInfo = (ZEPhysicsPlaneShapeInfo*)Info;
				NxPlaneShapeDesc PlaneDesc;
				PlaneDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (PlaneInfo->Trigger)PlaneDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				PlaneDesc.d = PlaneInfo->Height;
				PlaneDesc.normal = TONX(PlaneInfo->Normal);
				PlaneDesc.localPose.t = TONX(PlaneInfo->LocalPosition);
				PlaneDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(PlaneInfo->CollisionMask);
				if (PlaneInfo->Material)
				{
					PlaneDesc.materialIndex = PlaneInfo->Material->GetIndex();
				}
				Actor->createShape(PlaneDesc);
				break;
			}
			case ZEPhysicsShapeInfo::ZE_PST_BOX:
			{
				ZEPhysicsBoxShapeInfo* BoxInfo = (ZEPhysicsBoxShapeInfo*)Info;
				NxBoxShapeDesc BoxDesc;
				BoxDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (BoxInfo->Trigger)BoxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				BoxDesc.dimensions = TONX(BoxInfo->Dimensions);
				BoxDesc.localPose.t = TONX(BoxInfo->LocalPosition);
				BoxDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(BoxInfo->CollisionMask);
				if (BoxInfo->Material)
				{
					BoxDesc.materialIndex = BoxInfo->Material->GetIndex();
				}
				Actor->createShape(BoxDesc);
				break;
			}
			case ZEPhysicsShapeInfo::ZE_PST_CAPSULE:
			{
				ZEPhysicsCapsuleShapeInfo* CapsuleInfo = (ZEPhysicsCapsuleShapeInfo*)Info;
				NxCapsuleShapeDesc CapsuleDesc;
				CapsuleDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (CapsuleInfo->Trigger)CapsuleDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				CapsuleDesc.height = CapsuleInfo->Height;
				CapsuleDesc.radius = CapsuleInfo->Radius;
				CapsuleDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(CapsuleInfo->CollisionMask);;
				CapsuleDesc.localPose.t = TONX(CapsuleInfo->LocalPosition);
				if (CapsuleInfo->Material)
				{
					CapsuleDesc.materialIndex = CapsuleInfo->Material->GetIndex();
				}
				Actor->createShape(CapsuleDesc);
				break;
			}
			case ZEPhysicsShapeInfo::ZE_PST_SPHERE:
			{
				ZEPhysicsSphereShapeInfo* SphereInfo = (ZEPhysicsSphereShapeInfo*)Info;
				NxSphereShapeDesc SphereDesc;
				SphereDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (SphereInfo->Trigger)SphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				SphereDesc.radius = SphereInfo->Radius;
				SphereDesc.localPose.t = TONX(SphereInfo->LocalPosition);
				SphereDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(SphereInfo->CollisionMask);;
				if (SphereInfo->Material)
				{
					SphereDesc.materialIndex = SphereInfo->Material->GetIndex();
				}
				Actor->createShape(SphereDesc);
				break;
			}
			case ZEPhysicsShapeInfo::ZE_PST_CONVEX:
			{
				ZEPhysicsConvexShapeInfo* ConvexInfo = (ZEPhysicsConvexShapeInfo*)Info;
				NxConvexShapeDesc ConvexDesc;
				ConvexDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (ConvexInfo->Trigger)ConvexDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				ConvexDesc.localPose.t = TONX(ConvexInfo->LocalPosition);
				ConvexDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(ConvexInfo->CollisionMask);;

				//first prepare vertices
				NxVec3* Verts = new NxVec3[ConvexInfo->Vertices.GetCount()];
				for (int i=0; i<(int)ConvexInfo->Vertices.GetCount(); i++)
				{
					Verts[i] = TONX(ConvexInfo->Vertices[i]);
				}

				//prepare mesh description
				NxConvexMeshDesc ConvexMeshDesc;
				ConvexMeshDesc.numVertices		= (NxU32)ConvexInfo->Vertices.GetCount();
				ConvexMeshDesc.pointStrideBytes	= sizeof(NxVec3);
				ConvexMeshDesc.points			= Verts;
				ConvexMeshDesc.flags			= NX_CF_COMPUTE_CONVEX;

				//cook the mesh
				if (World->GetCooker()->NxInitCooking())
				{
					//Cooking from memory    
					ZEAegiaPhysicsMemoryWriteBuffer Buffer;
					if (World->GetCooker()->NxCookConvexMesh(ConvexMeshDesc, Buffer))
					{
						//set mesh data
						ConvexDesc.meshData = World->GetSdk()->createConvexMesh(ZEAegiaPhysicsMemoryReadBuffer(Buffer.data));
					}
					else
					{
						//Creation Error
					}
				}
				else
				{
					//Creation Error
				}

				//delete temp
				delete [] Verts;

				//set material
				if (ConvexInfo->Material)
				{
					ConvexDesc.materialIndex = ConvexInfo->Material->GetIndex();
				}

				Actor->createShape(ConvexDesc);
				break;
			}
			case ZEPhysicsShapeInfo::ZE_PST_TRIMESH:
			{
				ZEPhysicsTrimeshShapeInfo* TrimeshInfo = (ZEPhysicsTrimeshShapeInfo*)Info;
				NxTriangleMeshShapeDesc TrimeshDesc;
				TrimeshDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
				if (TrimeshInfo->Trigger)TrimeshDesc.shapeFlags |= NX_TRIGGER_ENABLE;
				TrimeshDesc.localPose.t = TONX(TrimeshInfo->LocalPosition);
				TrimeshDesc.groupsMask = ZEAegiaPhysicsUtility::toNX(TrimeshInfo->CollisionMask);;

				//first prepare vertices
				NxVec3* Verts = new NxVec3[TrimeshInfo->Vertices.GetCount()];
				for (int i=0; i<(int)TrimeshInfo->Vertices.GetCount(); i++)
				{
					Verts[i] = TONX(TrimeshInfo->Vertices[i]);
				}

				//prepare indexes
				unsigned int* Indxs = new unsigned int [TrimeshInfo->Indexes.GetCount()];
				for (int i=0; i<(int)TrimeshInfo->Indexes.GetCount(); i++)
				{
					Indxs[i] = TrimeshInfo->Indexes[i];
				}

				//prepare mesh description
				NxTriangleMeshDesc TriangleMeshDesc;
				TriangleMeshDesc.numVertices = (NxU32)TrimeshInfo->Vertices.GetCount();
				TriangleMeshDesc.numTriangles = (NxU32)TrimeshInfo->Indexes.GetCount()/3;
				TriangleMeshDesc.pointStrideBytes = sizeof(NxVec3);
				TriangleMeshDesc.triangleStrideBytes = sizeof(NxU32) * 3;
				TriangleMeshDesc.points = Verts;
				TriangleMeshDesc.triangles = Indxs;
				TriangleMeshDesc.flags = 0;

				//cook the mesh
				if (World->GetCooker()->NxInitCooking())
				{
					//Cooking from memory    
					ZEAegiaPhysicsMemoryWriteBuffer Buffer;
					if (World->GetCooker()->NxCookTriangleMesh(TriangleMeshDesc, Buffer))
					{
						//init convex desc
						TrimeshDesc.meshData = World->GetSdk()->createTriangleMesh(ZEAegiaPhysicsMemoryReadBuffer(Buffer.data));
					}
					else
					{
						//Creation Error
					}
				}
				else
				{
					//Creation Error
				}

				//delete temp
				delete [] Verts;
				delete [] Indxs;

				//set material
				if (TrimeshInfo->Material)
				{
					TrimeshDesc.materialIndex = TrimeshInfo->Material->GetIndex();
				}

				Actor->createShape(TrimeshDesc);
				break;
			}
		}
	}
}
