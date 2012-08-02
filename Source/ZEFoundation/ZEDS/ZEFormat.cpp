//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormat.cpp
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

#include "ZEFormat.h"

#include "ZEStringWriter.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMeta/ZEObject.h"

#define ZE_FPS_IMMEDIATE					0
#define ZE_FPS_ARGUMENT_INDEX_START			1
#define ZE_FPS_ARGUMENT_INDEX				2
#define ZE_FPS_ARGUMENT_INDEX_END			3
#define ZE_FPS_ARGUMENT_FORMATTING			4
#define ZE_FPS_APPLY						5

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString** Parameters, ZESize Count)
{
	ZESize ParameterIndex = 0;

	char ParameterString[255];
	ZESize ParameterStringIndex = 0;

	const char* Current = ArgumentFormat;
	while(*Current != '\0')
	{
		if (*Current == ':')
		{
			if (ParameterIndex >= Count)
				return false;

			ParameterString[ParameterStringIndex] = '\0';
			if (ParameterStringIndex != 0)
				*Parameters[ParameterIndex] = ParameterString;
			ParameterIndex++;
			
			ParameterString[0] = '\0';
			ParameterStringIndex = 0;
		}
		else
		{
			ParameterString[ParameterStringIndex] = *Current;
			ParameterStringIndex++;
		}

		Current++;
	}

	if (ParameterStringIndex != 0)
	{
		ParameterString[ParameterStringIndex] = '\0';
		*Parameters[ParameterIndex] = ParameterString;
	}

	return true;
}

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString& Parameter0)
{
	ZEString* ArgumentFormatParameters[] = {&Parameter0};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 1);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString& Parameter0, ZEString& Parameter1)
{
	ZEString* ArgumentFormatParameters[] = {&Parameter0, &Parameter1};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 2);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString& Parameter0, ZEString& Parameter1, ZEString& Parameter2)
{
	ZEString* ArgumentFormatParameters[] = {&Parameter0, &Parameter1, &Parameter2};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 3);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString& Parameter0, ZEString& Parameter1, ZEString& Parameter2, ZEString& Parameter3)
{
	ZEString* ArgumentFormatParameters[] = {&Parameter0, &Parameter1, &Parameter2, &Parameter3};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 4);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, ZEString& Parameter0, ZEString& Parameter1, ZEString& Parameter2, ZEString& Parameter3, ZEString& Parameter4)
{
	ZEString* ArgumentFormatParameters[] = {&Parameter0, &Parameter1, &Parameter2, &Parameter3, &Parameter4};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 4);
}

static bool CheckArgumentFormat(const char* ArgumentFormat)
{
	if (ArgumentFormat == NULL)
		return true;

	while(*ArgumentFormat != 0)
	{
		if (!isdigit(*ArgumentFormat) && *ArgumentFormat != '.' && *ArgumentFormat != '#' && *ArgumentFormat != '.' && *ArgumentFormat != '-' && *ArgumentFormat != '+')
			return false;
		ArgumentFormat++;
	}

	return true;
}

static void PrintFloat(ZEStringWriter& Output, float Argument, const char* FormattingOptions)
{
	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%f");
	else
		sprintf_s(Format, 256, "%%%sf", FormattingOptions);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);
}

static bool FormatArgument(ZEStringWriter& Output, ZEInt Argument, const char* ArgumentFormat)
{
	ZEString Type = "d";
	ZEString FormattingOptions;

	if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
		return false;

	if (Type != "d" && Type != "x" && Type != "X")
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", Type.ToCString());
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions.ToCString(), Type.ToCString());
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}


static bool FormatArgument(ZEStringWriter& Output, ZEUInt Argument, const char* ArgumentFormat)
{
	ZEString Type = "d";
	ZEString FormattingOptions;

	if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
		return false;

	if (Type != "d" && Type != "x" && Type != "X")
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	if (Type == "d")
		Type = "u";

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", Type.ToCString());
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions.ToCString(), Type.ToCString());
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, ZEInt64 Argument,	const char* ArgumentFormat)
{
	ZEString Type = "d";
	ZEString FormattingOptions;

	ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions);

	if (Type != "d" && Type != "x" && Type != "X")
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	const char* InnerType;
	if (Type == "d")
		InnerType = "lld";
	else if (Type == "x")
		InnerType = "llx";
	else if (Type == "X")
		InnerType = "llX";
	
	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", InnerType);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions.ToCString(), Type.ToCString());
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, ZEUInt64 Argument, const char* ArgumentFormat)
{
	ZEString Type = "d";
	ZEString FormattingOptions;

	ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions);

	if (Type != "d" && Type != "x" && Type != "X")
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;
	
	const char* InnerType;
	if (Type == "d")
		InnerType = "llu";
	else if (Type == "x")
		InnerType = "llx";
	else if (Type == "X")
		InnerType = "llX";

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", InnerType);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions.ToCString(), Type.ToCString());
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, float Argument, const char* ArgumentFormat)
{
	ZEString FormattingOptions;

	ParseArgumentFormat(ArgumentFormat, FormattingOptions);

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%f");
	else
		sprintf_s(Format, 256, "%%%sf", FormattingOptions.ToCString());
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, bool Argument, const char* ArgumentFormat)
{
	ZEString True = "true";
	ZEString False = "false";
	
	if (!ParseArgumentFormat(ArgumentFormat, True, False))
		return false;

	Output.Append(Argument ? True : False);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEVector2& Argument, const char* ArgumentFormat)
{
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString FormattingOptions = ".03";

	if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEVector3& Argument, const char* ArgumentFormat)
{
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString FormattingOptions = ".03";

	if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEVector4& Argument, const char* ArgumentFormat)
{
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString FormattingOptions = ".03";

	if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.w, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEQuaternion& Argument, const char* ArgumentFormat)
{
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString FormattingOptions = ".03";

	if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.w, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEMatrix3x3& Argument, const char* ArgumentFormat)
{
	ZEString FormattingOptions = ".03";
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString Seperator = ", ";

	if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma, Seperator))
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.M11, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M12, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M13, FormattingOptions);

	Output.Append(Seperator);

	PrintFloat(Output, Argument.M21, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M22, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M23, FormattingOptions);

	Output.Append(Seperator);

	PrintFloat(Output, Argument.M31, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M32, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M33, FormattingOptions);

	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEMatrix4x4& Argument,const char* ArgumentFormat)
{
	ZEString FormattingOptions = ".03";
	ZEString PreFix = "<";
	ZEString PostFix = ">";
	ZEString Comma = ", ";
	ZEString Seperator = ", ";

	ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma, Seperator);

	Output.Append(PreFix);
	
	PrintFloat(Output, Argument.M11, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M12, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M13, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M14, FormattingOptions);
	
	Output.Append(Seperator);
	
	PrintFloat(Output, Argument.M21, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M22, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M23, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M24, FormattingOptions);
	
	Output.Append(Seperator);
	
	PrintFloat(Output, Argument.M31, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M32, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M33, FormattingOptions);
	Output.Append(PostFix);
	PrintFloat(Output, Argument.M34, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

static bool FormatArgument(ZEStringWriter& Output, const ZEString& Argument, const char* ArgumentFormat)
{
	Output.Append(Argument);
	return true;
}

static bool FormatArgument(ZEStringWriter& Output, ZEObject* Argument,	const char* ArgumentFormat)
{
	return "";
}

static bool FormatArgumentVariant(ZEStringWriter& Output, const ZEVariant& Argument, const char* ArgumentFormat)
{
	switch(Argument.GetType())
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			return -1;

		case ZE_VRT_FLOAT:
			return FormatArgument(Output, Argument.GetFloat(), ArgumentFormat);

		case ZE_VRT_DOUBLE:
			return FormatArgument(Output, (float)Argument.GetDouble(), ArgumentFormat);

		case ZE_VRT_INTEGER_8:
			return FormatArgument(Output, (ZEInt)Argument.GetInt8(), ArgumentFormat);

		case ZE_VRT_INTEGER_16:
			return FormatArgument(Output, (ZEInt)Argument.GetInt16(), ArgumentFormat);

		case ZE_VRT_INTEGER_32:
			return FormatArgument(Output, (ZEInt)Argument.GetInt32(), ArgumentFormat);

		case ZE_VRT_INTEGER_64:
			return FormatArgument(Output, (ZEInt64)Argument.GetInt64(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return FormatArgument(Output, (ZEUInt)Argument.GetUInt8(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return FormatArgument(Output, (ZEUInt)Argument.GetUInt16(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return FormatArgument(Output, (ZEUInt)Argument.GetUInt32(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return FormatArgument(Output, (ZEUInt64)Argument.GetUInt64(), ArgumentFormat);

		case ZE_VRT_BOOLEAN:
			return FormatArgument(Output, Argument.GetBoolean(), ArgumentFormat);

		case ZE_VRT_STRING:
			return FormatArgument(Output, Argument.GetString(), ArgumentFormat);

		case ZE_VRT_QUATERNION:
			return FormatArgument(Output, Argument.GetQuaternion(), ArgumentFormat);

		case ZE_VRT_VECTOR2:
			return FormatArgument(Output, Argument.GetVector2(), ArgumentFormat);

		case ZE_VRT_VECTOR3:
			return FormatArgument(Output, Argument.GetVector3(), ArgumentFormat);

		case ZE_VRT_VECTOR4:
			return FormatArgument(Output, Argument.GetVector4(), ArgumentFormat);

		case ZE_VRT_MATRIX3X3:
			return FormatArgument(Output, Argument.GetMatrix3x3(), ArgumentFormat);

		case ZE_VRT_MATRIX4X4:
			return FormatArgument(Output, Argument.GetMatrix4x4(), ArgumentFormat);

		case ZE_VRT_CLASS:
			return FormatArgument(Output, Argument.GetClass(), ArgumentFormat);
	}
}

ZEString ZEFormat::FormatInner(const char* Input, const ZEVariant** Arguments, ZESize Count)
{
	ZEStringWriter Writer;

	int State = ZE_FPS_IMMEDIATE;

	const char* CurrentInput = Input;
	ZESize ArgumentIndex = -1;
	
	char ArgumentIndexString[2];
	char ArgumentIndexStringIndex = 0;

	char ArgumentFormat[255];
	ZESize ArgumentFormatIndex = 0;

	while(*CurrentInput != '\0')
	{
		switch(State)
		{
			case ZE_FPS_IMMEDIATE:
				if (*CurrentInput == '{')
				{
					if (*(CurrentInput + 1) != '\0' && *(CurrentInput + 1) == '{')
					{
						Writer.Append('{');
						CurrentInput++;
					}
					else
						State = ZE_FPS_ARGUMENT_INDEX_START;
				}
				else if (*CurrentInput == '}')
				{
					if (*(CurrentInput + 1) != '\0' && *(CurrentInput + 1) == '}')
					{
						Writer.Append('}');
						CurrentInput++;
					}
					else
					{
						zeError("Formatting error. Mismatch '}' character.");
						return "";
					}
				}
				else
				{
					Writer.Append(*CurrentInput);
				}

				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_INDEX_START:
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
					CurrentInput++;
				else if (isdigit(*CurrentInput))
					State = ZE_FPS_ARGUMENT_INDEX;
				else
				{
					zeError("Formatting error. Argument index must be integer.");
					return "";
				}
				break;
			
			case ZE_FPS_ARGUMENT_INDEX:
				if (*CurrentInput == ' ' || *CurrentInput == '\t' || *CurrentInput == ':' || *CurrentInput == '}')
				{
					ArgumentIndexString[ArgumentIndexStringIndex] = '\0';
					ArgumentIndex = atoi(ArgumentIndexString);
					ArgumentIndexStringIndex = 0;

					if (ArgumentIndex >= Count)
					{
						zeError("Formatting error. Argument index is out of bounds.");
						return "";
					}

					if (*CurrentInput == ':')
						State = ZE_FPS_ARGUMENT_FORMATTING;
					else if (*CurrentInput == '}')
						State = ZE_FPS_APPLY;
					else 
						State = ZE_FPS_ARGUMENT_INDEX_END;

				}
				else if (isdigit(*CurrentInput))
				{
					if (ArgumentIndexStringIndex >= 3)
					{
						zeError("Formatting error. Argument index is too big.");
						return false;
					}

					ArgumentIndexString[ArgumentIndexStringIndex] = *CurrentInput;
					ArgumentIndexStringIndex++;
				}
				else
				{
					zeError("Formatting Error. Argument index must be integer.");
					return "";
				}
				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_INDEX_END: // Argument Index End
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
				{

				}
				else if (*CurrentInput == '}')
				{
					State = ZE_FPS_APPLY;
				}
				else if (*CurrentInput == ':')
				{
					State = ZE_FPS_ARGUMENT_FORMATTING;
					CurrentInput++;
				}
				else
				{
					zeError("Formatting error. Expected ':' or '}' after argument index.");
					return "";
				}

				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_FORMATTING:
				if (*CurrentInput == '}')
				{
					State = ZE_FPS_APPLY;
				}
				else
				{
					ArgumentFormat[ArgumentFormatIndex] = *CurrentInput;
					ArgumentFormatIndex++;
				}

				CurrentInput++;
				break;
		}

		if (State == ZE_FPS_APPLY)
		{
			ArgumentFormat[ArgumentFormatIndex] = '\0';

			ZEString PrintOutput;
			if (!FormatArgumentVariant(Writer, *Arguments[ArgumentIndex], ArgumentFormat))
				return false;

			ArgumentIndex = -1;
			ArgumentFormatIndex = 0;

			State = ZE_FPS_IMMEDIATE;
		}
	}

	if (State != 0)
	{
		zeError("Formatting error. There are non-closed argument available in the format.");
		return false;
	}

	ZEString Output;
	Writer.AppendEnd();
	Writer.Output(Output);

	return Output;
}
