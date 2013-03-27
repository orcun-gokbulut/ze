//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLSerialWriterTests.cpp
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

#include "ZEML/ZEMLSerialWriter.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEMLDataProperty.h"


ZETestSuite(ZEMLSerialNode)
{
	ZETest("ZEMLSerialRootNode::ZEMLSerialRootNode(const ZEString& Name, ZEFile* File)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

		ZEMLSerialRootNode RootNode("RootNode", File);
		//RootNode.File: File
		//RootNode.Name: "RootNode"
		//RootNode.FileUpdatePosition: 12
		//RootNode.ParentNode: NULL
		//RootNode.Size: 0
		//RootNode.SubItemCount: 0

		ZETestCheckEqual(File->GetSize(), 28);
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("ZEMLSerialNode ZEMLSerialRootNode::OpenNode(const ZEString& Name)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);

		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		//Node.File: File
		//Node.Name: "Node"
		//Node.FileUpdatePosition: 36
		//Node.ParentNode: RootNode
		//RootNode.SubItemCount: 1
		//Node.Size: 0
		//Node.SubItemCount: 0

		ZETestCheckEqual(File->GetSize(), 52);
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialRootNode::CloseNode()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");

		RootNode.CloseNode();

		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("ZEMLSerialNode  ZEMLSerialNode::OpenNode(const ZEString& Name)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");

		ZEMLSerialNode SerialNode = Node.OpenNode("SerialNode");
		//SerialNode.File: File
		//SerialNode.Name: "SerialNode"
		//SerialNode.FileUpdatePosition: 66
		//SerialNode.ParentNode: Node
		//SerialNode.Size: 0
		//SerialNode.SubItemCount: 0
		//Node.ParentNode: RootNode

		ZETestCheckEqual(File->GetSize(), 82);
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void  ZEMLSerialNode::CloseNode()")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");

		ZEMLSerialNode SerialNode = Node.OpenNode("SerialNode");
		//Node.Size: 0

		SerialNode.CloseNode();
		//Node.Size: 30
		File->Close();
		remove("SerialWriterTests.txt");

		ZETestCase("close all nodes")
		{
			File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZEMLSerialRootNode Root("Root", File);
			ZEMLSerialNode Node1 = Root.OpenNode("Node01");
			ZEMLSerialNode Node2 = Node1.OpenNode("Node001");		
			//Root.Size : 0
			//Node1.Size : 0
			//Node2.Size : 0

			Node2.CloseNode();
			//Node1.Size : 27
			Node1.CloseNode();
			//Root.Size : 53
			Root.CloseNode();

			ZETestCheckEqual(File->GetSize(), 77);
			File->Close();
			remove("SerialWriterTests.txt");
		}
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVariant& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEVariant Value;
		Value.SetFloat(2.0f);

		Node.WriteProperty("PropFloat", Value);
		//Node.Name: "Node"
		//Node.FileUpdatePosition: 36
		//Node.Size: 25
		//Node.SubItemCount: 1
		//Node.ParentNode: RootNode
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 49
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, float Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		float Value = 2.0f;

		Node.WriteProperty("PropFloat", Value);
		//Node.Name: "Node"
		//Node.FileUpdatePosition: 36
		//Node.Size: 25
		//Node.SubItemCount: 1
		//Node.ParentNode: RootNode
		//RootNode.Size: 0
		
		Node.CloseNode();
		//RootNode.Size: 49
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, double Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		double Value = 2.0;

		Node.WriteProperty("PropDouble", Value);
		//Node.Size: 30
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 54
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt8 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEInt8 Value = 'z';

		Node.WriteProperty("PropInt8", Value);
		//Node.Size: 21
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 45
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt16 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEInt16 Value = 30;

		Node.WriteProperty("PropInt16", Value);
		//Node.Size: 23
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 47
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt32 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEInt32 Value = 500;

		Node.WriteProperty("PropInt32", Value);
		//Node.Size: 25
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 49
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt64 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEInt64 Value = 500;

		Node.WriteProperty("PropInt64", Value);
		//Node.Size: 29
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 53
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt8 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEUInt8 Value = 'z';

		Node.WriteProperty("PropUInt8", Value);
		//Node.Size: 22
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 46
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt16 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEUInt16 Value = 500;

		Node.WriteProperty("PropUInt16", Value);
		//Node.Size: 24
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 48
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt32 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEUInt32 Value = 500;

		Node.WriteProperty("PropUInt32", Value);
		//Node.Size: 26
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 50
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt64 Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEUInt64 Value = 500;

		Node.WriteProperty("PropUInt64", Value);
		//Node.Size: 30
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 54
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, bool Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		bool Value = true;

		Node.WriteProperty("PropBool", Value);
		//Node.Size: 21
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 45
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEString& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEString Value = "TestString";

		Node.WriteProperty("PropString", Value);
		//Node.Size: 33
		//RootNode.Size: 0

		ZEString Value2 = "";

		Node.WriteProperty("PropString", Value2);
		//Node.Size: 55

		Node.CloseNode();
		//RootNode.Size: 79
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const char* Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		const char* Value = "TestString";

		Node.WriteProperty("PropChar", Value);
		//Node.Size: 31
		//RootNode.Size: 0

		const char* Value2 = "";

		Node.WriteProperty("PropChar", Value2);
		//Node.Size: 51

		Node.CloseNode();
		//RootNode.Size: 75
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEQuaternion& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEQuaternion Value = ZEQuaternion::Identity;

		Node.WriteProperty("PropQuat", Value);
		//Node.Size: 36
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 60
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector2& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEVector2 Value = ZEVector2::One;

		Node.WriteProperty("PropVec2", Value);
		//Node.Size: 28
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 52
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector3& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEVector3 Value = ZEVector3::Zero;

		Node.WriteProperty("PropVec3", Value);
		//Node.Size: 32
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 56
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector4& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEVector4 Value = ZEVector4::Zero;

		Node.WriteProperty("PropVec4", Value);
		//Node.Size: 36
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 60
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEMatrix3x3& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEMatrix3x3 Value = ZEMatrix3x3::Identity;

		Node.WriteProperty("PropMatrix3x3", Value);
		//Node.Size: 61
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 85
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEMatrix4x4& Value)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		ZEMatrix4x4 Value = ZEMatrix4x4::Identity;

		Node.WriteProperty("PropMatrix4x4", Value);
		//Node.Size: 89
		//RootNode.Size: 0

		Node.CloseNode();
		//RootNode.Size: 113
		File->Close();
		remove("SerialWriterTests.txt");
	}

	ZETest("void ZEMLSerialNode::WriteDataProperty(const ZEString& Name, void* Data, ZEUInt64 DataSize)")
	{
		ZEFile* File = new ZEFile();
		File->Open("SerialWriterTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZEMLSerialRootNode RootNode("RootNode", File);
		ZEMLSerialNode Node = RootNode.OpenNode("Node");
		void* Data = new char[sizeof(unsigned char)];

		Node.WriteDataProperty("DataProp", Data, sizeof(unsigned char));
		//Node.Size: 0
		//RootNode.Size: 21
		File->Close();
		remove("SerialWriterTests.txt");
	}
}
