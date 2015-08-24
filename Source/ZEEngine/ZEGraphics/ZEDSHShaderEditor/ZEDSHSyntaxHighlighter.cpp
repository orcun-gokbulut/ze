//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHSyntaxHighlighter.cpp
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

#include "ZEDSHSyntaxHighlighter.h"

static QRegExp RegExpCompile(const char** Keywords, size_t Count, const char* Pre, const char* Mid, const char* Post)
{
	QString Pattern = "\\b(";
	for(size_t I = 0; I < Count; I++)
	{
		Pattern += Keywords[I];
		if (Count != I + 1)
			Pattern += "|";
	}
	Pattern += ")\\b";

	return QRegExp(Pattern);
}

bool ZEDSHSyntaxElement::operator==(const ZEDSHSyntaxElement& Element)
{
	return Name == Element.Name;
}

ZEDSHSyntaxElement::ZEDSHSyntaxElement()
{
	ExpressionEnabled = false;
	ExpressionBlockEnabled = false;
}

void ZEDSHSyntaxHightlighter::HighlightElement(const QString &text, ZEDSHSyntaxElement& Element)
{
	QTextCharFormat CommentFormat;
	CommentFormat.setForeground(Element.Foreground);
	CommentFormat.setBackground(Element.Background);
	CommentFormat.setFont(Element.Font);

	if (Element.ExpressionBlockEnabled)
	{
		setCurrentBlockState(0);
		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = text.indexOf(Element.StartExpression);
		while (startIndex >= 0) 
		{
			int endIndex = text.indexOf(Element.EndExpression, startIndex);
			int commentLength;
			if (endIndex == -1) 
			{
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			} 
			else 
			{
				commentLength = endIndex - startIndex+ Element.EndExpression.matchedLength();
			}
			setFormat(startIndex, commentLength, CommentFormat);
			startIndex = text.indexOf(Element.StartExpression, startIndex + commentLength);
		}
	}

	if (Element.ExpressionEnabled)
	{
		int index = text.indexOf(Element.Expression);
		while (index >= 0) 
		{
			int length = Element.Expression.matchedLength();
			setFormat(index, length, CommentFormat);
			index = text.indexOf(Element.Expression, index + length);
		}
	}
}

void ZEDSHSyntaxHightlighter::highlightBlock(const QString &text)
{
	for (int I = 0; I < Elements.count(); I++)
		HighlightElement(text, Elements[I]);
}

const QList<ZEDSHSyntaxElement>& ZEDSHSyntaxHightlighter::GetSyntaxElements()
{
	return Elements;
}

void ZEDSHSyntaxHightlighter::AddSyntaxElement(const ZEDSHSyntaxElement& Element)
{
	Elements.append(Element);
}

void ZEDSHSyntaxHightlighter::RemoveSyntaxElement(const ZEDSHSyntaxElement& Element)
{
	Elements.removeAll(Element);
}

ZEDSHSyntaxHightlighter::ZEDSHSyntaxHightlighter(QTextDocument* Document) : QSyntaxHighlighter(Document)
{
	const char* Keywords[] =
	{
		"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer",	
		"case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment", "CompileShader", "const",
		"continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView",
		"discard", "do", "double", "DomainShader", "dword", "else", "export", "extern", "false", "float", "for", "fxgroup",
		"GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int",
		"interface", "line", "lineadj", "linear", "LineStream", "matrix", "min16float", "min10float", "min16int",
		"min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL,out", "OutputPatch",
		"packoffset", "pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState",
		"RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
		"RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "(s|S)ampler(|2D|3D|Cube)",
		"SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state", "static",
		"string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11",
		"texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS", "Texture2DMSArray",
		"Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream",
		"uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment", "VertexShader", "void", "volatilewhile",
		"float([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"int([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"uint([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"bool([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min10float([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min16float([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min12int([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min16int([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min12int([2|3|4]|[2|3|4]x[2|3|4])",
		"min16float([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"min16uint([1|2|3|4]|[1|2|3|4]x[1|2|3|4])",
		"half([1|2|3|4]|[1|2|3|4]x[1|2|3|4])"
	};

	ZEDSHSyntaxElement KeywordElements;
	KeywordElements.Name = "Keywords";
	KeywordElements.Font = QFont("Consolas", 10);
	KeywordElements.Font.setBold(true);
	KeywordElements.Foreground = QColor(Qt::darkBlue);
	KeywordElements.ExpressionEnabled = true;
	KeywordElements.Expression = RegExpCompile(Keywords, sizeof(Keywords) / sizeof(const char*), "\\b(", "|", ")\\b");
	AddSyntaxElement(KeywordElements);

	const char* Semantics[] =
	{
		"BINORMAL\\d+",	"BLENDINDICES\\d+",	"BLENDWEIGHT\\d+", "COLOR\\d+", "DEPTH\\d+", "NORMAL\\d+",
		"POSITION\\d+", "POSITIONT", "PSIZE\\d+", "TANGENT\\d+", "TEXCOORD\\d+", "COLOR\\d+", "FOG",
		"TESSFACTOR\\d+" "VFACE", "VPOS",
		"SV_ClipDistance\\d+", "SV_CullDistance\\d+", "SV_Coverage", "SV_Depth", "SV_DepthGreaterEqual(\\d+)",
		"SV_DepthLessEqual(\\d+)", "SV_DispatchThreadID", "SV_DomainLocation", "SV_GroupID", "SV_GroupIndex", "SV_GroupThreadID", 
		"SV_GSInstanceID", "SV_InnerCoverage", "SV_InsideTessFactor", "SV_InstanceID", "SV_IsFrontFace", "SV_OutputControlPointID",
		"SV_Position", "SV_PrimitiveID", "SV_RenderTargetArrayIndex", "SV_SampleIndex", "SV_StencilRef", "SV_Target\\d+,", 
		"SV_TessFactor", "SV_VertexID", "SV_ViewportArrayIndex"
	};

	ZEDSHSyntaxElement SemanticsElements;
	SemanticsElements.Name = "Semantics";
	SemanticsElements.Font = QFont("Consolas", 10);
	SemanticsElements.Foreground = QColor(Qt::darkMagenta);
	SemanticsElements.ExpressionEnabled = true;
	SemanticsElements.Expression = RegExpCompile(Semantics, sizeof(Semantics) / sizeof(const char*), "\\b(", "|", ")\\b");
	AddSyntaxElement(SemanticsElements);

	const char* Intrinsics[] =
	{
		"abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint",
		"asint", "asuint", "asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross",
		"D3DCOLORtoUBYTE4", "ddx", "ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier",
		"DeviceMemoryBarrierWithGroupSync", "distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample",
		"EvaluateAttributeSnapped", "exp", "exp2", "f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma",
		"fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount", "GetRenderTargetSamplePosition", "GroupMemoryBarrier",
		"GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange", "InterlockedCompareStore",
		"InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan", "ldexp",
		"length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf", 
		"Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors",
		"ProcessQuadTessFactorsAvg", "ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg",
		"ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin", "radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt",
		"saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step", "tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod",
		"tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj", "tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod",
		"tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc", "Expand"
	};

	ZEDSHSyntaxElement IntrinsicsElements;
	IntrinsicsElements.Name = "Intrinsics";
	IntrinsicsElements.Font = QFont("Consolas", 10);
	IntrinsicsElements.Foreground = QColor(Qt::darkBlue);
	IntrinsicsElements.ExpressionEnabled = true;
	IntrinsicsElements.Expression = RegExpCompile(Intrinsics, sizeof(Intrinsics) / sizeof(const char*), "\\b(", "|", ")\\b");
	AddSyntaxElement(IntrinsicsElements);


	const char* Preporcessor[] =
	{
		"\\#define", "\\#undef", "\\#include", "\\#if", "\\#else", "\\#elif", "\\#endif", "\\#pragma", "\\#line", "\\#error"
	};

	ZEDSHSyntaxElement PreprocessorElements;
	PreprocessorElements.Name = "Preporcessor";
	PreprocessorElements.Font = QFont("Consolas", 10);
	PreprocessorElements.Font.setBold(true);
	PreprocessorElements.Foreground = QColor(Qt::darkBlue);
	PreprocessorElements.ExpressionEnabled = true;
	PreprocessorElements.Expression = RegExpCompile(Preporcessor, sizeof(Preporcessor) / sizeof(const char*), "\\s(", "|", ")\\s");
	AddSyntaxElement(PreprocessorElements);

	ZEDSHSyntaxElement CommentElements;
	CommentElements.Name = "Comments";
	CommentElements.Font = QFont("Consolas", 10);
	CommentElements.Foreground = QColor(Qt::darkGreen);
	CommentElements.Expression = QRegExp("//.*$");
	CommentElements.ExpressionEnabled = true;
	CommentElements.ExpressionBlockEnabled = true;
	CommentElements.StartExpression = QRegExp("/\\*");
	CommentElements.EndExpression = QRegExp("\\*/");
	AddSyntaxElement(CommentElements);
}
