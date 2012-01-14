//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMain.cpp
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

#include "ZETModelFile/ZETModelFile.h"
#include "ZETModelFile/ZETModelFileFormat.h"
#include <iostream>

class ZEModelFile;

bool WritePhysicalJointToFile(ZEModelFilePhysicalJoint* Joint, FILE* File);
bool ReadPhysicalJointFromFile(ZEModelFilePhysicalJoint* Joint, FILE* File);

int main()
{
	ZEModelFile ModelFile;
	ZEModelFileBone* Bone = ModelFile.Bones.Add();
	Bone->BoundingBox.Min = Bone->BoundingBox.Max = ZEVector3::One;
	strcpy(Bone->Name, "Orcun");
	Bone->ParentBone = -1;

	ModelFile.WriteToFile("Orc");

	ZEModelFile TestFile;
	TestFile.ReadFromFile("Orc");

	ZEModelFilePhysicalJoint Joint;
	
	Joint.ChunkId					= ZE_MDLF_PHYSICAL_JOINT_CHUNKID;

	Joint.JointType					= 3;

	Joint.Body1Id					= 0;
	Joint.Body2Id					= 1;

	Joint.Breakable					= true;
	Joint.BreakForce				= 300.0f;
	Joint.BreakTorque				= 300.0f;

	Joint.CollideBodies				= true;

	Joint.UseGlobalAnchorAxis		= true;

	Joint.GlobalAnchor				= ZEVector3::Zero;
	Joint.GlobalAxis				= ZEQuaternion::Zero;

	Joint.LocalAnchor1				= ZEVector3::Zero;
	Joint.LocalAxis1				= ZEQuaternion::Zero;

	Joint.LocalAnchor2				= ZEVector3::Zero;
	Joint.LocalAxis2				= ZEQuaternion::Zero;

	Joint.XMotion					= 3;
	Joint.YMotion					= 3;
	Joint.ZMotion					= 3;

	Joint.LinearLimitValue			= 1.34f;
	Joint.LinearLimitRestitution	= 1.34f;
	Joint.LinearLimitSpring			= 1.34f;
	Joint.LinearLimitDamping		= 1.34f;

	Joint.TwistMotion				= 1;
	Joint.TwistLowLimitValue		= 1.34f;
	Joint.TwistLowLimitRestitution	= 1.34f;
	Joint.TwistLowLimitSpring		= 1.34f;
	Joint.TwistLowLimitDamping		= 1.34f;
	Joint.TwistHighLimitValue		= 1.34f;
	Joint.TwistHighLimitRestitution = 1.34f;
	Joint.TwistHighLimitSpring		= 1.34f;
	Joint.TwistHighLimitDamping		= 1.34f;

	Joint.Swing1Motion				= 1;
	Joint.Swing1LimitValue			= 1.34f;
	Joint.Swing1LimitRestitution	= 1.34f;
	Joint.Swing1LimitSpring			= 1.34f;
	Joint.Swing1LimitDamping		= 1.34f;

	Joint.Swing2Motion				= 1;
	Joint.Swing2LimitValue			= 1.34f;
	Joint.Swing2LimitRestitution	= 1.34f;
	Joint.Swing2LimitSpring			= 1.34f;
	Joint.Swing2LimitDamping		= 1.34f;

	FILE* File = fopen("test", "wb");
	WritePhysicalJointToFile(&Joint, File);
	fclose(File);

	ZEModelFilePhysicalJoint TestJoint;
	FILE* File2 = fopen("test", "rb");
	ReadPhysicalJointFromFile(&TestJoint, File2);
	fclose(File2);

	return 0;
}
