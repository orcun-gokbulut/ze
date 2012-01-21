//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAStarTests.cpp
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

#include "ZEAStar.h"
#include "ZETest.h"
#include "ZEMath/ZEVector.h"
#include <string.h>

float Cost(const ZEGraph<ZEVector2>& Graph, ZESize CurrentNodeIndex, ZESize NextNodeIndex)
{
	return 1.0f;
}

float Heuristic(const ZEGraph<ZEVector2>& Graph, ZESize CurrentNodeIndex, ZESize DestinationNodeIndex)
{
	return ZEVector2::Distance(Graph.GetNodes()[CurrentNodeIndex], Graph.GetNodes()[DestinationNodeIndex]);
}

void Visualize(const char* Tiles, ZESize Width, ZESize Height, ZEArray<const ZEVector2*>& Path, bool DiagonalMovement = false)
{
	char Buffer[32 * 1024];
	strcpy(Buffer, Tiles);
	for (ZESize I = 0; I < Path.GetCount(); I++)
		Buffer[(ZEInt)Path[I]->y * Width + (ZEInt)Path[I]->x] = 'O';

	for (ZESize y = 0; y < Height; y++)
	{
		for (ZESize x = 0; x < Width; x++)
			printf("%c", Buffer[y * Width + x]);

		printf("\n");
	}
}

void PrintSolution(ZEArray<const ZEVector2*>& Path)
{
	printf("ZEVector2 Solution[%d] = ", Path.GetCount());
	printf("\n{");
	for (ZESize I = 0; I < Path.GetCount(); I++)
	{
		if (I % 5 == 0)
			printf("\n\t");
		printf("ZEVector2(%.0f.0f, %.0f.0f), ", Path[I]->x, Path[I]->y);

	}

	printf("\n};\n\nZETestCheck(CheckSolution(Output, Solution, %d));\n\n", Path.GetCount());
}

ZEGraph<ZEVector2> GenerateGraph(const char* Tiles, ZESize Width, ZESize Height, ZESize& Start, ZESize& Destination, bool DiagonalMovement = false)
{
	ZEGraph<ZEVector2> Graph;
	for (ZESize y = 0; y < Height; y++)
		for (ZESize x = 0; x < Width; x++)
		{
			if (Tiles[y * Width + x] != 'X')
				Graph.AddNode(ZEVector2(x, y));

			if (Tiles[y * Width + x] == 'D')
				Destination = Graph.GetNodeCount() - 1;

			if (Tiles[y * Width + x] == 'S')
				Start = Graph.GetNodeCount() - 1;
		}

		for (ZESize I = 0; I < Graph.GetNodeCount(); I++)
		{
			ZEInt AdjIndex = Graph.GetNodes().FindIndex(ZEVector2(Graph.GetNodes()[I] + ZEVector2(0, 1)));
			if (AdjIndex != -1)
				Graph.AddLink(I, AdjIndex);

			AdjIndex = Graph.GetNodes().FindIndex(ZEVector2(Graph.GetNodes()[I] + ZEVector2(1, 0)));
			if (AdjIndex != -1)
				Graph.AddLink(I, AdjIndex);

			if (DiagonalMovement)
			{
				AdjIndex = Graph.GetNodes().FindIndex(ZEVector2(Graph.GetNodes()[I] + ZEVector2(1, 1)));
				if (AdjIndex != -1)
					Graph.AddLink(I, AdjIndex);

				AdjIndex = Graph.GetNodes().FindIndex(ZEVector2(Graph.GetNodes()[I] + ZEVector2(-1, 1)));
				if (AdjIndex != -1)
					Graph.AddLink(I, AdjIndex);

			}
		}

	return Graph;
}

bool CheckSolution(ZEArray<const ZEVector2*> Output, ZEVector2* Solution, ZESize SolutionSize)
{
	if (Output.GetSize() != SolutionSize)
		return false;

	for (ZEInt I = 0; I < SolutionSize; I++)
		if (*Output[I] != Solution[I])
			return false;

	return true;
}

ZETestSuite(ZEAIAStar)
{
	ZETest("No Solution")
	{
		ZETestCase("NoSolutionMaze")
		{
			char Maze[] =
				"XXXXXXXXXXXXXXXXXXX"
				"X     X           X"
				"X  D  X           X"
				"X     X           X"
				"XXXXXXX           X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X            S    X"
				"X                 X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZETestCheck(Output.GetCount() == 0);
		}
	}

	ZETest("Solvable Maze")
	{
		ZETestCase("VeryEasyMaze")
		{
			char Maze[] =
				"XXXXXXXXXXXXXXXXXXX"
				"X                 X"
				"X  D              X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X            S    X"
				"X                 X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[18] =
			{
				ZEVector2(12.0f, 10.0f), ZEVector2(11.0f, 10.0f), ZEVector2(11.0f, 9.0f), ZEVector2(11.0f, 8.0f), ZEVector2(10.0f, 8.0f),
				ZEVector2(9.0f, 8.0f), ZEVector2(8.0f, 8.0f), ZEVector2(8.0f, 7.0f), ZEVector2(8.0f, 6.0f), ZEVector2(8.0f, 5.0f),
				ZEVector2(7.0f, 5.0f), ZEVector2(6.0f, 5.0f), ZEVector2(5.0f, 5.0f), ZEVector2(5.0f, 4.0f), ZEVector2(5.0f, 3.0f),
				ZEVector2(4.0f, 3.0f), ZEVector2(3.0f, 3.0f), ZEVector2(3.0f, 2.0f)
			};

			ZETestCheck(CheckSolution(Output, Solution, 18));
		}

		ZETestCase("EasyMaze")
		{
			char Maze[] =
				"XSXXXXXXXXXXXXXXXXX"
				"X                 X"
				"X  D              X"
				"X                 X"
				"XXXXXXXXXXXXXX    X"
				"X                 X"
				"X                 X"
				"X                 X"
				"X   XXXXXXXXXXXXXXX"
				"X                 X"
				"X              S  X"
				"X                 X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[42] =
			{
				ZEVector2(14.0f, 10.0f), ZEVector2(13.0f, 10.0f), ZEVector2(12.0f, 10.0f), ZEVector2(11.0f, 10.0f), ZEVector2(10.0f, 10.0f),
				ZEVector2(9.0f, 10.0f), ZEVector2(9.0f, 9.0f), ZEVector2(8.0f, 9.0f), ZEVector2(7.0f, 9.0f), ZEVector2(6.0f, 9.0f),
				ZEVector2(5.0f, 9.0f), ZEVector2(4.0f, 9.0f), ZEVector2(3.0f, 9.0f), ZEVector2(3.0f, 8.0f), ZEVector2(3.0f, 7.0f),
				ZEVector2(3.0f, 6.0f), ZEVector2(3.0f, 5.0f), ZEVector2(4.0f, 5.0f), ZEVector2(5.0f, 5.0f), ZEVector2(6.0f, 5.0f),
				ZEVector2(7.0f, 5.0f), ZEVector2(8.0f, 5.0f), ZEVector2(9.0f, 5.0f), ZEVector2(10.0f, 5.0f), ZEVector2(11.0f, 5.0f),
				ZEVector2(12.0f, 5.0f), ZEVector2(13.0f, 5.0f), ZEVector2(14.0f, 5.0f), ZEVector2(14.0f, 4.0f), ZEVector2(14.0f, 3.0f),
				ZEVector2(13.0f, 3.0f), ZEVector2(12.0f, 3.0f), ZEVector2(11.0f, 3.0f), ZEVector2(10.0f, 3.0f), ZEVector2(9.0f, 3.0f),
				ZEVector2(8.0f, 3.0f), ZEVector2(7.0f, 3.0f), ZEVector2(6.0f, 3.0f), ZEVector2(5.0f, 3.0f), ZEVector2(4.0f, 3.0f),
				ZEVector2(3.0f, 3.0f), ZEVector2(3.0f, 2.0f),
			};

			ZETestCheck(CheckSolution(Output, Solution, 42));
		}

		ZETestCase("IntermediateMaze")
		{
			char Maze[] =
				"XXXXXXXXXXXXXXXXXXX"
				"X   D             X"
				"X     X   X  X    X"
				"XXX XXXX  X  XXXX X"
				"X         X  X    X"
				"X    XXXXXXXX     X"
				"X    X      X XXXXX"
				"X    X      X     X"
				"X    X      XXXXX X"
				"X    X      X     S"
				"X    XXXXXXXX     X"
				"X                 X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[28] =
			{
				ZEVector2(17.0f, 9.0f), ZEVector2(16.0f, 9.0f), ZEVector2(15.0f, 9.0f), ZEVector2(14.0f, 9.0f), ZEVector2(13.0f, 9.0f),
				ZEVector2(13.0f, 10.0f), ZEVector2(13.0f, 11.0f), ZEVector2(12.0f, 11.0f), ZEVector2(11.0f, 11.0f), ZEVector2(10.0f, 11.0f),
				ZEVector2(9.0f, 11.0f), ZEVector2(8.0f, 11.0f), ZEVector2(7.0f, 11.0f), ZEVector2(6.0f, 11.0f), ZEVector2(5.0f, 11.0f),
				ZEVector2(4.0f, 11.0f), ZEVector2(4.0f, 10.0f), ZEVector2(4.0f, 9.0f), ZEVector2(4.0f, 8.0f), ZEVector2(4.0f, 7.0f),
				ZEVector2(4.0f, 6.0f), ZEVector2(4.0f, 5.0f), ZEVector2(3.0f, 5.0f), ZEVector2(3.0f, 4.0f), ZEVector2(3.0f, 3.0f),
				ZEVector2(3.0f, 2.0f), ZEVector2(3.0f, 1.0f), ZEVector2(4.0f, 1.0f),
			};

			ZETestCheck(CheckSolution(Output, Solution, 28));
		}

		ZETestCase("UpperIntermediateMase")
		{
			char Maze[] =
				"XSXXXXXXXXXXXXXXXXX"
				"X       X         X"
				"XXXXXXX X XXXXXXX X"
				"X               X X"
				"X X XXXXXXXXXXXXX X"
				"X X       X X     X"
				"X XXXXXXXXX X XXXXX"
				"X     X           X"
				"XXXXX XXXXX XXXXXXX"
				"X X         X    DX"
				"X XXXXX XXXXXXX XXX"
				"X                 X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[41] =
			{
				ZEVector2(1.0f, 1.0f), ZEVector2(2.0f, 1.0f), ZEVector2(3.0f, 1.0f), ZEVector2(4.0f, 1.0f), ZEVector2(5.0f, 1.0f),
				ZEVector2(6.0f, 1.0f), ZEVector2(7.0f, 1.0f), ZEVector2(7.0f, 2.0f), ZEVector2(7.0f, 3.0f), ZEVector2(6.0f, 3.0f),
				ZEVector2(5.0f, 3.0f), ZEVector2(4.0f, 3.0f), ZEVector2(3.0f, 3.0f), ZEVector2(2.0f, 3.0f), ZEVector2(1.0f, 3.0f),
				ZEVector2(1.0f, 4.0f), ZEVector2(1.0f, 5.0f), ZEVector2(1.0f, 6.0f), ZEVector2(1.0f, 7.0f), ZEVector2(2.0f, 7.0f),
				ZEVector2(3.0f, 7.0f), ZEVector2(4.0f, 7.0f), ZEVector2(5.0f, 7.0f), ZEVector2(5.0f, 8.0f), ZEVector2(5.0f, 9.0f),
				ZEVector2(6.0f, 9.0f), ZEVector2(7.0f, 9.0f), ZEVector2(7.0f, 10.0f), ZEVector2(7.0f, 11.0f), ZEVector2(8.0f, 11.0f),
				ZEVector2(9.0f, 11.0f), ZEVector2(10.0f, 11.0f), ZEVector2(11.0f, 11.0f), ZEVector2(12.0f, 11.0f), ZEVector2(13.0f, 11.0f),
				ZEVector2(14.0f, 11.0f), ZEVector2(15.0f, 11.0f), ZEVector2(15.0f, 10.0f), ZEVector2(15.0f, 9.0f), ZEVector2(16.0f, 9.0f),
				ZEVector2(17.0f, 9.0f),
			};
			ZETestCheck(CheckSolution(Output, Solution, 41));
	}

		ZETestCase("HardMaze")
		{
			char Maze[] =
				"XXXXXXXXXXXXXXXXXXX"
				"X       DX        X"
				"X  XXXXXXX XXXXX XX"
				"X        X     X  X"
				"X  XXXX XXXXXX XX X"
				"X     X        X  X"
				"XXXXXXXXXXXXXXXX  X"
				"X    SX    X   X XX"
				"X  XXXX  X XX XX  X"
				"X     X  X  X  XX X"
				"X        X        X"
				"X        X        X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[75] =
			{
				ZEVector2(4.0f, 7.0f), ZEVector2(3.0f, 7.0f), ZEVector2(2.0f, 7.0f), ZEVector2(2.0f, 8.0f), ZEVector2(2.0f, 9.0f),
				ZEVector2(3.0f, 9.0f), ZEVector2(4.0f, 9.0f), ZEVector2(5.0f, 9.0f), ZEVector2(5.0f, 10.0f), ZEVector2(6.0f, 10.0f),
				ZEVector2(7.0f, 10.0f), ZEVector2(7.0f, 9.0f), ZEVector2(7.0f, 8.0f), ZEVector2(7.0f, 7.0f), ZEVector2(8.0f, 7.0f),
				ZEVector2(9.0f, 7.0f), ZEVector2(10.0f, 7.0f), ZEVector2(10.0f, 8.0f), ZEVector2(10.0f, 9.0f), ZEVector2(11.0f, 9.0f),
				ZEVector2(11.0f, 10.0f), ZEVector2(12.0f, 10.0f), ZEVector2(13.0f, 10.0f), ZEVector2(14.0f, 10.0f), ZEVector2(15.0f, 10.0f),
				ZEVector2(16.0f, 10.0f), ZEVector2(17.0f, 10.0f), ZEVector2(17.0f, 9.0f), ZEVector2(17.0f, 8.0f), ZEVector2(16.0f, 8.0f),
				ZEVector2(16.0f, 7.0f), ZEVector2(16.0f, 6.0f), ZEVector2(17.0f, 6.0f), ZEVector2(17.0f, 5.0f), ZEVector2(17.0f, 4.0f),
				ZEVector2(17.0f, 3.0f), ZEVector2(16.0f, 3.0f), ZEVector2(16.0f, 2.0f), ZEVector2(16.0f, 1.0f), ZEVector2(15.0f, 1.0f),
				ZEVector2(14.0f, 1.0f), ZEVector2(13.0f, 1.0f), ZEVector2(12.0f, 1.0f), ZEVector2(11.0f, 1.0f), ZEVector2(10.0f, 1.0f),
				ZEVector2(10.0f, 2.0f), ZEVector2(10.0f, 3.0f), ZEVector2(11.0f, 3.0f), ZEVector2(12.0f, 3.0f), ZEVector2(13.0f, 3.0f),
				ZEVector2(14.0f, 3.0f), ZEVector2(14.0f, 4.0f), ZEVector2(14.0f, 5.0f), ZEVector2(13.0f, 5.0f), ZEVector2(12.0f, 5.0f),
				ZEVector2(11.0f, 5.0f), ZEVector2(10.0f, 5.0f), ZEVector2(9.0f, 5.0f), ZEVector2(8.0f, 5.0f), ZEVector2(7.0f, 5.0f),
				ZEVector2(7.0f, 4.0f), ZEVector2(7.0f, 3.0f), ZEVector2(6.0f, 3.0f), ZEVector2(5.0f, 3.0f), ZEVector2(4.0f, 3.0f),
				ZEVector2(3.0f, 3.0f), ZEVector2(2.0f, 3.0f), ZEVector2(2.0f, 2.0f), ZEVector2(2.0f, 1.0f), ZEVector2(3.0f, 1.0f),
				ZEVector2(4.0f, 1.0f), ZEVector2(5.0f, 1.0f), ZEVector2(6.0f, 1.0f), ZEVector2(7.0f, 1.0f), ZEVector2(8.0f, 1.0f),
			};

			ZETestCheck(CheckSolution(Output, Solution, 75));
		}

		ZETestCase("VeryHardMaze")
		{
			char Maze[] =
				"XSXXXXXXXXXXXXXXXXX"
				"X X X   X X XXD   X"
				"X X X X X XXX XXX X"
				"X X X X X   X X X X"
				"X X X X XXX X X X X"
				"X X X X X X X X X X"
				"X X X X X X X X X X"
				"X X X X X X X X X X"
				"X X X X X X X X X X"
				"X X X X X   X X   X"
				"X X X X X X X XXX X"
				"X     X   X       X"
				"XXXXXXXXXXXXXXXXXXX";

			ZESize Start, Destination;
			ZEGraph<ZEVector2> MazeGraph = GenerateGraph(Maze, 19, 13, Start, Destination);
			ZEArray<const ZEVector2*> Output = ZEAIAStar::Process(MazeGraph, Start, Destination, Cost, Heuristic);

			ZEVector2 Solution[64] =
			{
				ZEVector2(1.0f, 1.0f), ZEVector2(1.0f, 2.0f), ZEVector2(1.0f, 3.0f), ZEVector2(1.0f, 4.0f), ZEVector2(1.0f, 5.0f),
				ZEVector2(1.0f, 6.0f), ZEVector2(1.0f, 7.0f), ZEVector2(1.0f, 8.0f), ZEVector2(1.0f, 9.0f), ZEVector2(1.0f, 10.0f),
				ZEVector2(1.0f, 11.0f), ZEVector2(2.0f, 11.0f), ZEVector2(3.0f, 11.0f), ZEVector2(4.0f, 11.0f), ZEVector2(5.0f, 11.0f),
				ZEVector2(5.0f, 10.0f), ZEVector2(5.0f, 9.0f), ZEVector2(5.0f, 8.0f), ZEVector2(5.0f, 7.0f), ZEVector2(5.0f, 6.0f),
				ZEVector2(5.0f, 5.0f), ZEVector2(5.0f, 4.0f), ZEVector2(5.0f, 3.0f), ZEVector2(5.0f, 2.0f), ZEVector2(5.0f, 1.0f),
				ZEVector2(6.0f, 1.0f), ZEVector2(7.0f, 1.0f), ZEVector2(7.0f, 2.0f), ZEVector2(7.0f, 3.0f), ZEVector2(7.0f, 4.0f),
				ZEVector2(7.0f, 5.0f), ZEVector2(7.0f, 6.0f), ZEVector2(7.0f, 7.0f), ZEVector2(7.0f, 8.0f), ZEVector2(7.0f, 9.0f),
				ZEVector2(7.0f, 10.0f), ZEVector2(7.0f, 11.0f), ZEVector2(8.0f, 11.0f), ZEVector2(9.0f, 11.0f), ZEVector2(9.0f, 10.0f),
				ZEVector2(9.0f, 9.0f), ZEVector2(10.0f, 9.0f), ZEVector2(11.0f, 9.0f), ZEVector2(11.0f, 10.0f), ZEVector2(11.0f, 11.0f),
				ZEVector2(12.0f, 11.0f), ZEVector2(13.0f, 11.0f), ZEVector2(14.0f, 11.0f), ZEVector2(15.0f, 11.0f), ZEVector2(16.0f, 11.0f),
				ZEVector2(17.0f, 11.0f), ZEVector2(17.0f, 10.0f), ZEVector2(17.0f, 9.0f), ZEVector2(17.0f, 8.0f), ZEVector2(17.0f, 7.0f),
				ZEVector2(17.0f, 6.0f), ZEVector2(17.0f, 5.0f), ZEVector2(17.0f, 4.0f), ZEVector2(17.0f, 3.0f), ZEVector2(17.0f, 2.0f),
				ZEVector2(17.0f, 1.0f), ZEVector2(16.0f, 1.0f), ZEVector2(15.0f, 1.0f), ZEVector2(14.0f, 1.0f),
			};

			ZETestCheck(CheckSolution(Output, Solution, 64));
		}
	}
}
