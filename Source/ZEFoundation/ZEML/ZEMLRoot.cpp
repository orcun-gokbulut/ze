//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLRoot.cpp
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

#include "ZEMLRoot.h"
#include "ZEMLReader.h"
#include "ZEMLWriter.h"

ZESize ZEMLRoot::GetSize()
{

	ZESize Size = 
		4 +	// Identifier
		2 + // Version
		8 + // StartOffset
		RootNode->GetSize();

	return Size;
}

void ZEMLRoot::SetRootNode(ZEMLNode* Node)
{
	RootNode = Node;
}

ZEMLNode* ZEMLRoot::GetRootNode()
{
	return RootNode;
}

void ZEMLRoot::SetDeferredDataLoadingMode(bool Enabled)
{
	DeferredMode = Enabled;
}

bool ZEMLRoot::GetDeferredDataLoadingMode()
{
	return DeferredMode;
}

bool ZEMLRoot::Read(const char* FileName)
{
	if (RootNode == NULL)
	{
		zeError("Cannot read ZEML file. Root node pointer is not set.");
		return false;
	}

	FileName = FileName;
	ZEMLReader Reader;
	if (!Reader.Open(FileName))
		return false;

	ZEMLReaderNode ReaderNode = Reader.GetRootNode();
	bool Result = RootNode->Read(&ReaderNode);
	
	zeDebugCheck(!_CrtCheckMemory(), "Heap problem");

	return Result;
}

bool ZEMLRoot::Read(ZEFile* File)
{
	if (RootNode == NULL)
	{
		zeError("Cannot read ZEML file. Root node pointer is not set.");
		return false;
	}

	ZEMLReader Reader;
	if (!Reader.Open(File))
		return false;

	ZEMLReaderNode ReaderNode = Reader.GetRootNode();
	return RootNode->Read(&ReaderNode);
}

bool ZEMLRoot::Write(const char* FileName)
{
	if (RootNode == NULL)
	{
		zeError("Cannot read ZEML file. Root node pointer is not set.");
		return false;
	}

	ZEMLWriter Writer;
	if (!Writer.Open(FileName))
		return false;

	ZEMLWriterNode WriterNode = Writer.WriteRootNode(RootNode->GetName());
	RootNode->Write(&WriterNode);
	WriterNode.CloseNode();
	Writer.Close();

	return true;
}

bool ZEMLRoot::Write(ZEFile* File)
{
	if (RootNode == NULL)
	{
		zeError("Cannot read ZEML file. Root node pointer is not set.");
		return false;
	}

	ZEMLWriter Writer;
	if (!Writer.Open(File))
		return false;

	ZEMLWriterNode WriterNode = Writer.WriteRootNode(RootNode->GetName());
	RootNode->Write(&WriterNode);
	WriterNode.CloseNode();
	Writer.Close();

	return true;
}

ZEMLRoot::ZEMLRoot()
{
	RootNode = NULL;
	DeferredMode = false;
}
