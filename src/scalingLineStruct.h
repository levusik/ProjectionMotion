#pragma once

// prosta struktura kt�ra przechowa wszystkie wa�ne warto�ci w dodawaniu 
// linijek [ przechowa mno�nik (skal� ), ileLinijek i zasi�g ]

struct scalingLineStruct
{

	scalingLineStruct(double Range = 0., double Scale = 0, int HowManyLines = 0)
		: range(Range), scale(Scale), howManyLines(HowManyLines)
	{}

	int howManyLines = 0;

	double scale = 0;
	double range = 0.;
};