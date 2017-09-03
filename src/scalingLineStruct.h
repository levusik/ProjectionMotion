#pragma once

// prosta struktura która przechowa wszystkie wa¿ne wartoœci w dodawaniu 
// linijek [ przechowa mno¿nik (skalê ), ileLinijek i zasiêg ]

struct scalingLineStruct
{

	scalingLineStruct(double Range = 0., double Scale = 0, int HowManyLines = 0)
		: range(Range), scale(Scale), howManyLines(HowManyLines)
	{}

	int howManyLines = 0;

	double scale = 0;
	double range = 0.;
};