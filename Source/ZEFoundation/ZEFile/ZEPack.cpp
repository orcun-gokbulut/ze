//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPack.cpp
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

#include "ZEPack.h"
#include "ZECompressedFile.h"

#include <Windows.h>

struct ZEChunkHeader
{
	ZEUInt64				Header;
	ZEUInt64				ChunkHash;
	ZEUInt64				ChunkPosition;
	ZEUInt64				ChunkPositionDecompressed;
	ZEUInt64				ChunkSize;
	ZEUInt64				ChunkCompressedSize;
	char				IdentifierName[256];
};

// Helper function for copying Records
void CopyData(ZECompressedFile* File, ZEUInt64 From, ZEUInt64 Size, ZEUInt64 To)
{
	const ZEUInt64 BufferSize = 65536;
	char Buffer[BufferSize];

	ZEUInt64 Count = Size / BufferSize;
	ZEUInt64 LeftOver = Size % BufferSize;
	for (ZEUInt64 I = 0; I < Count; I++)
	{
		_fseeki64((FILE*)File->GetFileHandle(), From + I * BufferSize, ZE_SF_BEGINING);
		fread(Buffer, BufferSize, 1,(FILE*)File->GetFileHandle());
		_fseeki64((FILE*)File->GetFileHandle(),To + I * BufferSize, ZE_SF_BEGINING);
		fwrite(Buffer, BufferSize, 1,(FILE*)File->GetFileHandle());
	}

	if (LeftOver != 0)
	{
		_fseeki64((FILE*)File->GetFileHandle(),From + Count * BufferSize, ZE_SF_BEGINING);
		fread(Buffer, (size_t)LeftOver, 1,(FILE*)File->GetFileHandle());
		_fseeki64((FILE*)File->GetFileHandle(),To + Count * BufferSize, ZE_SF_BEGINING);
		fwrite(Buffer, (size_t)LeftOver, 1,(FILE*)File->GetFileHandle());

	}
}

// Constructor
ZEPack::ZEPack() : File(NULL)
{
	File = new ZECompressedFile();
}

// Destructor
ZEPack::~ZEPack()
{
	if(File)
	{
		if(File->IsOpen())
			File->Close();

		delete File;
		File = NULL;
	}
}

// Opens the cache file
bool ZEPack::Open(const char* PackFileName)
{
	//zeAssert(File && File->IsOpen(), "Cache is already open...");
	
	File->Open(PackFileName, ZE_FM_READ_WRITE, true);

	if (File->IsOpen())
	{
		strncpy(this->PackFileName, PackFileName, 256);
		return true;
	}

	return false;
}

// Closes the cache file
void ZEPack::ClosePack()
{
	if (File && File->IsOpen())
		File->Close();
}

// Empty the cache file
bool ZEPack::ClearPack()
{
	// If cache file is open close it
	ClosePack();

	// Remove the file
	if(remove(PackFileName) == -1)
	{
		return false;
	}

	// ReOpen the cache	
	if(Open(PackFileName))
		return true;

	return false;
}

// Returns the cache file name
const char*	ZEPack::GetPackFileName()
{
	return PackFileName;
}

// Checks if the chunk exists
bool ZEPack::ChunkExists(const char* IdentifierName)
{
	//If file is not opened return false
	if(!File)
		return false;

	//Copy identifier name to a char array
	char ChunkName[256];
	strncpy(ChunkName,IdentifierName,256);

	//Store file cursor position
	ZEUInt64 FileCursorBeforeChunkExists = _ftelli64((FILE*)File->GetFileHandle());

	bool control;

	// Go to end of file
	_fseeki64((FILE*)File->GetFileHandle(),0, SEEK_END);

	ZEUInt64 EndOfFile = 0;

	//Store end of file
	EndOfFile = _ftelli64((FILE*)File->GetFileHandle());

	// Get records start position
	_fseeki64((FILE*)File->GetFileHandle(),-(ZEInt64)sizeof(ZEUInt64), SEEK_END);
	ZEUInt64 RecordsStartPosition = 0;
	fread(&RecordsStartPosition, (ZEInt64)(ZEInt64)sizeof(ZEUInt64), 1,(FILE*)File->GetFileHandle());

	//Take first header as active header
	ZEUInt64 NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between CurrentPosition && EndOfFile
	while(NextHeaderPosition < EndOfFile)
	{
		_fseeki64((FILE*)File->GetFileHandle(),NextHeaderPosition, SEEK_SET);

		// Get header
		ZEChunkHeader Header;

		//If cant read return false
		if (fread(&Header, sizeof(ZEChunkHeader), 1,(FILE*)File->GetFileHandle()) == 0 )
			return false;

		//If header is not ZECH file is corrupted return false
		if (Header.Header != 'ZECH')
			return false;

		//Compare header chunk name and wanted chunk name
		control=true;
		for ( int i=0;i<256;i++)
		{
			if(Header.IdentifierName[i] != ChunkName[i])
			{
				control=false;
				break;
			}
		}

		//If find go to stored file cursor position and return true
		if(control)
		{
			_fseeki64((FILE*)File->GetFileHandle(),FileCursorBeforeChunkExists,SEEK_SET);
			return true;
		}

		//If cant find go to next header
		NextHeaderPosition += sizeof(ZEChunkHeader);
	}

	//If cant return false
	return false;
}

//Returns hash code
ZEUInt64 ZEPack::GetHash(const char* Identifier)
{
	char ChunkName[256];
	strncpy(ChunkName,Identifier,256);
	ZEUInt64 Hash = 0;
	ZEUInt64 I = 0;	
	ZEUInt64 Char = 0;
	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		Char = ChunkName[I];
		Hash = Char + (Hash << 6) + (Hash << 16) - Hash;
		I++;
	}
	return (ZEUInt64)Hash;
}

//Deletes a chunk
bool ZEPack::DeleteChunk(const char* IdentifierName)
{
	// if file is not opened return false
	if( !File || !File->IsOpen())
		return false;

	bool Control;
	bool WeGotDataToDelete=false;

	//Copy Identifier name to a char array 
	char ChunkName[256];
	strncpy(ChunkName,IdentifierName,256);

	ZEChunkHeader TemporaryHeader;

	ZEUInt64 ChunkWillBeDeletedStartPoint;
	ZEUInt64 ChunkWillBeDeletedSize;
	ZEUInt64 ChunkWillBeDeletedEndPoint;
	ZEUInt64 HeaderWillBeDeletedStartPoint;
	ZEUInt64 HeaderWillBeDeletedSize;
	ZEUInt64 HeaderWillBeDeletedEndPoint;
	ZEUInt64 NewEndOfFile;
	ZEUInt64 ChunkWillBeDeletedSizeD;

	//Go to end of file
	_fseeki64((FILE*)File->GetFileHandle(),0,SEEK_END);

	//Get end of file position
	ZEUInt64 EndOfFile = _ftelli64((FILE*)File->GetFileHandle());

	//Go to chunk headers' end positions storage position
	_fseeki64((FILE*)File->GetFileHandle(),-(ZEInt64)sizeof(ZEUInt64),SEEK_END);

	//Get chunk headers' start position
	ZEUInt64 RecordsStartPosition =0;
	fread(&RecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());

	//Set current headers position to headers' start position
	ZEUInt64 NextHeaderPosition = RecordsStartPosition;

	//Control end of file is reached or data to delete is found
	while(NextHeaderPosition < EndOfFile && !WeGotDataToDelete)
	{

		//Go to current header position
		_fseeki64((FILE*)File->GetFileHandle(), NextHeaderPosition,SEEK_SET);

		//Get header from file, if cant return false
		ZEChunkHeader Header;
		if(fread(&Header,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle()) == 0 )
			return false;

		//If headers header is not equal to 'ZECH' file is corrupted return false
		if(Header.Header != 'ZECH')
			return false;

		//Control headers identifier name
		Control = true;
 		for(int i=0;i<256;i++)
		{
			if(Header.IdentifierName[i] != ChunkName[i])
				Control = false;
		}

		//If wanted header is found
		if(Control)
		{
			//Got data to delete, so break while 
			WeGotDataToDelete = true;

			//Get start position of chunk which will be deleted
			ChunkWillBeDeletedStartPoint = Header.ChunkPosition;

			//Get size of chunk which will be deleted
			ChunkWillBeDeletedSize = Header.ChunkCompressedSize;

			//Get end position of chunk which will be deleted
			ChunkWillBeDeletedEndPoint = ChunkWillBeDeletedStartPoint + ChunkWillBeDeletedSize;

			ChunkWillBeDeletedSizeD = Header.ChunkSize;

			//Get start position of header which will be deleted
			HeaderWillBeDeletedStartPoint = NextHeaderPosition;

			//Get size of header which will be deleted
			HeaderWillBeDeletedSize = sizeof(ZEChunkHeader);

			//Get end position of header which will be deleted
			HeaderWillBeDeletedEndPoint = HeaderWillBeDeletedStartPoint + HeaderWillBeDeletedSize;
		}

		//Set current header position to next one
		NextHeaderPosition += sizeof(ZEChunkHeader);

	}

	//If do not have data to delete return false
	if(!WeGotDataToDelete)
		return false;

	//if got data to delete
	else
	{
		//Copy headers to deleted chunks start position
		CopyData(File,ChunkWillBeDeletedEndPoint,EndOfFile-ChunkWillBeDeletedEndPoint,ChunkWillBeDeletedStartPoint);

		//Recalculate start and end position of header which will be deleted  
		HeaderWillBeDeletedStartPoint -= ChunkWillBeDeletedSize;
		HeaderWillBeDeletedEndPoint -= ChunkWillBeDeletedSize;

		//Copy headers that comes after header which will be deleted to their new position
		CopyData(File,HeaderWillBeDeletedEndPoint,EndOfFile - HeaderWillBeDeletedEndPoint,HeaderWillBeDeletedStartPoint);

		//Calculate new new end of file position
		NewEndOfFile = EndOfFile - HeaderWillBeDeletedSize - ChunkWillBeDeletedSize;

		//Go to new end of file
		_fseeki64((FILE*)File->GetFileHandle(),NewEndOfFile -(ZEInt64)sizeof(ZEUInt64),SEEK_SET);

		//Recalculate headers start position
		RecordsStartPosition -= ChunkWillBeDeletedSize;

		//Write headers start position to new end of file
		fwrite(&RecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());
		File->Close();

		//Create a handle of file 
		HANDLE fileHandle = CreateFile(File->GetFileName(),(GENERIC_READ | GENERIC_WRITE),
			0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		//Go to new end of file
		SetFilePointer(fileHandle,(long)NewEndOfFile,NULL,FILE_BEGIN);

		//Set new end of file
		SetEndOfFile(fileHandle);

		//close handle
		CloseHandle(fileHandle);

		//reopen file
		File->Open(File->GetFileName(),ZE_FM_READ_WRITE,true);
		File->Flush();

		//Set current header to deleted headers start position
		NextHeaderPosition = HeaderWillBeDeletedStartPoint;

		//Control if reached to end of file 
		while(NextHeaderPosition < NewEndOfFile - sizeof(EndOfFile))
		{
			//Go to current headers start position
			_fseeki64((FILE*)File->GetFileHandle(),NextHeaderPosition,SEEK_SET);

			//Get current header from file
			ZEChunkHeader header;

			//If cant read header properly return false
			if(fread(&header,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle()) == 0)
				return false;

			//If headers header is not equal to 'ZECH' , if not file if corrupted, return false
			if(header.Header != 'ZECH')
				return false;

			//Go to chunks start position
			_fseeki64((FILE*)File->GetFileHandle(),header.ChunkPosition,SEEK_SET);

			//Update temp headers chunk position
			TemporaryHeader.ChunkPosition = header.ChunkPosition - ChunkWillBeDeletedSize;

			//Get headers data to a temp variable
			TemporaryHeader.ChunkHash = header.ChunkHash;
			TemporaryHeader.Header =header.Header;
			TemporaryHeader.ChunkSize = header.ChunkSize;
			TemporaryHeader.ChunkPositionDecompressed = header.ChunkPositionDecompressed - ChunkWillBeDeletedSizeD;
			TemporaryHeader.ChunkCompressedSize = header.ChunkCompressedSize;
			strncpy(TemporaryHeader.IdentifierName,header.IdentifierName,sizeof(header.IdentifierName));

			//Go to current headers start position
			_fseeki64((FILE*)File->GetFileHandle(),NextHeaderPosition,SEEK_SET);

			//Write header to file
			fwrite(&TemporaryHeader,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle());

			//Set current headers position to next one
			NextHeaderPosition += sizeof(ZEChunkHeader);
		}
		return true;
	}
	return false;
}

//Adds a chunk 
bool ZEPack::AddChunk(const char* IdentifierName,unsigned char* Source,ZEUInt64 SourceSize)
{
	//if file is not open return false
	if( !File || !File->IsOpen())
		return false;

	//Go to headers' start position
	_fseeki64((FILE*)File->GetFileHandle(),-(ZEInt64)sizeof(ZEUInt64),SEEK_END);

	//Get chunk headers' end position
	ZEUInt64 OldRecordsEndPosition = _ftelli64((FILE*)File->GetFileHandle());
	
	//Get chunk headers' start position
	ZEUInt64 OldRecordsStartPosition = 0;

	fread(&OldRecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());

	//Calculate chunk headers' total size
	ZEUInt64 OldRecordSize = OldRecordsEndPosition - OldRecordsStartPosition;

	//Calculate new start position for chunk headers
	ZEUInt64 NewRecorsStartPosition = OldRecordsStartPosition + SourceSize;

	//Create buffer for keeping buffers at process
	unsigned char* bufferForHeaders = new unsigned char[(unsigned int)OldRecordSize];

	//Go to chunk headers' start position
	_fseeki64((FILE*)File->GetFileHandle(),OldRecordsStartPosition,SEEK_SET);

	//Copy chunk headers to buffer
	fread(bufferForHeaders, (unsigned int)OldRecordSize, 1, (FILE*)File->GetFileHandle());

	//Set new chunks start position to chunk headers' old start position
	ZEUInt64 NewChunkPosition = OldRecordsStartPosition;

	//Go to new chunks start position
	_fseeki64((FILE*)File->GetFileHandle(), NewChunkPosition, SEEK_SET);

	//Calculate total size of compressed data
	ZEUInt64 ChunkCompressedSize = File->Write(Source, SourceSize, 1);

	//Calculate where headers position will start ( just after added chunk ) 
	ZEUInt64 RecordsStartPosition = _ftelli64((FILE*)File->GetFileHandle());

	//Calculate headers total size
	ZEUInt64 RecordsEndPosition = RecordsStartPosition + OldRecordSize;

	//Go to headers start position
	_fseeki64((FILE*)File->GetFileHandle(), RecordsStartPosition, SEEK_SET);

	//Write headers from buffer 
	fwrite(bufferForHeaders, OldRecordSize, 1, (FILE*)File->GetFileHandle());

	_fseeki64((FILE*)File->GetFileHandle(), -(ZEInt64)sizeof(ZEChunkHeader),SEEK_CUR);

	ZEUInt64 NewChunkPositionDecompressed = 0;
	ZEChunkHeader TemporaryChunkHeader;
	fread(&TemporaryChunkHeader, sizeof(ZEChunkHeader), 1, (FILE*)File->GetFileHandle());
	if( TemporaryChunkHeader.Header == 'ZECH')
		NewChunkPositionDecompressed = TemporaryChunkHeader.ChunkSize + TemporaryChunkHeader.ChunkPositionDecompressed;
						
	//Create header for added chunk
	ZEChunkHeader Header;

	//Write null header 
	fwrite(&Header, sizeof(ZEChunkHeader), 1, (FILE*)File->GetFileHandle());

	//Go to null headers start position
	_fseeki64((FILE*)File->GetFileHandle(),RecordsEndPosition,SEEK_SET);

	//Set variables of header
	Header.Header = 'ZECH';
	Header.ChunkPosition = NewChunkPosition;
	Header.ChunkHash = GetHash(IdentifierName);
	Header.ChunkCompressedSize = ChunkCompressedSize;
	Header.ChunkPositionDecompressed = NewChunkPositionDecompressed;
	Header.ChunkSize = SourceSize;
	strncpy(Header.IdentifierName,IdentifierName,256);
	char ChunkName[256];
	strncpy(ChunkName,IdentifierName,256);

	//Write header over null one
	fwrite(&Header,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle());

	_fseeki64((FILE*)File->GetFileHandle(),RecordsEndPosition + sizeof(ZEChunkHeader),SEEK_SET);

	//Write start position of headers
	fwrite(&RecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());
	File->Close();

	//Calculate new end of file
	ZEUInt64 NewEndOfFile = RecordsEndPosition + sizeof(ZEChunkHeader) + (ZEInt64)sizeof(ZEUInt64);

	//Create handle
	HANDLE FileHandle;
	FileHandle = CreateFile( File->GetFileName(),( GENERIC_READ | GENERIC_WRITE),
		0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	//Set cursor position to new end of file
	SetFilePointer(FileHandle,NewEndOfFile,NULL,FILE_BEGIN);

	//Set current position as end of file
	SetEndOfFile(FileHandle);												
	CloseHandle(FileHandle);
	return true;
																						
}

//Gets a chunk
bool ZEPack::GetChunk(const char *IdentifierName, unsigned char *Data)
{
	// If file is not opened return false
	if( !File || !File->IsOpen() )
		return false;

	bool Control;

	//Get identifier to a temp variable
	char ChunkName[256];
	strncpy(ChunkName,IdentifierName,256);

	//Get hash
	ZEUInt64 Hash = GetHash(IdentifierName);

	//Go to headers' end position
	_fseeki64((FILE*)File->GetFileHandle(),-(ZEInt64)sizeof(ZEUInt64),SEEK_END);

	//Store headers' end position
	ZEUInt64 RecordsEndPosition = _ftelli64((FILE*)File->GetFileHandle());

	//Store headers' start position
	ZEUInt64 RecordsStartPosition=0;
	fread(&RecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());

	//Set current header position to headers' start position
	ZEUInt64 CurrentHeaderPosition = RecordsStartPosition;

	while(true)
	{
		//Go to current headers position
		_fseeki64((FILE*)File->GetFileHandle(),CurrentHeaderPosition,SEEK_SET);

		//Get header from file, if cant return false
		ZEChunkHeader Header;
		if(fread(&Header,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle()) == 0)
			return false;

		//If headers header is not 'ZECH' file is corrupted , return false
		if( Header.Header != 'ZECH' ) 
			return false;

		//Check identifier name
		Control = true;
		for(int i = 0; i < 256; i++)
		{
			if(Header.IdentifierName[i] != ChunkName[i])
				Control = false;
		}

		//Check hash
		if(Hash == Header.ChunkHash && Control)
		{
			File->Seek(0,ZE_SF_BEGINING);
			_fseeki64((FILE*)File->GetFileHandle(),Header.ChunkPosition,SEEK_SET);
			File->Read(Data,Header.ChunkSize,1);
			break;
		}

		//Go to next header
		CurrentHeaderPosition += sizeof(ZEChunkHeader);
	}
}



bool ZEPack::OpenChunk(ZEPartialCompressedFile& PartialFile, const char* IdentifierName)
{
	// If file is not opened return false
	if( !File || !File->IsOpen() )
		return false;

	bool Control;

	//Get identifier to a temp variable
	char ChunkName[256];
	strncpy(ChunkName,IdentifierName,256);

	//Get hash
	ZEUInt64 Hash = GetHash(IdentifierName);

	//Go to headers' end position
	_fseeki64((FILE*)File->GetFileHandle(),-(ZEInt64)sizeof(ZEUInt64),SEEK_END);

	//Store headers' end position
	ZEUInt64 RecordsEndPosition = _ftelli64((FILE*)File->GetFileHandle());

	//Store headers' start position
	ZEUInt64 RecordsStartPosition=0;
	fread(&RecordsStartPosition,(ZEInt64)sizeof(ZEUInt64),1,(FILE*)File->GetFileHandle());

	//Set current header position to headers' start position
	ZEUInt64 CurrentHeaderPosition = RecordsStartPosition;

	while(true)
	{
		//Go to current headers position
		_fseeki64((FILE*)File->GetFileHandle(),CurrentHeaderPosition,SEEK_SET);

		//Get header from file, if cant return false
		ZEChunkHeader Header;
		if(fread(&Header,sizeof(ZEChunkHeader),1,(FILE*)File->GetFileHandle()) == 0)
			return false;

		//If headers header is not 'ZECH' file is corrupted , return false
		if( Header.Header != 'ZECH' ) 
			return false;

		//Check identifier name
		Control = true;
		for(int i = 0; i < 256; i++)
		{
			if(Header.IdentifierName[i] != ChunkName[i])
				Control = false;
		}

		//Check hash
		if(Hash == Header.ChunkHash && Control)
		{
			ZEUInt64 Offset = 0;
			ZEUInt64 Size = Header.ChunkSize;
			File->Seek(Offset,ZE_SF_BEGINING);
			_fseeki64((FILE*)File->GetFileHandle(),Header.ChunkPosition,SEEK_SET);
			if(Offset + Size > Header.ChunkSize)
				Size = Header.ChunkSize - Offset;
			PartialFile.Open(File,Offset,Size);
			break;
		}
		//Go to next header
		CurrentHeaderPosition += sizeof(ZEChunkHeader);
	}

	return true;
}


bool ZEPack::IsPack(ZEString& Path)
{
	// NOT IMPLEMENTED !!!!
	return false;
	//zeAssert(true, "NOT IMPLEMENTED !!!");
}
