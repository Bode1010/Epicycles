#pragma once
#include "Instruction.h"


class FileParser
{
public:
	FileParser(float s);
	vector<Instruction> parse(string line);
	int numOfVariables(char Instruction);
	float scale = 0;
	int totalNumOfInstructions = 0;
	float meanX = 0;
	float meanY = 0;
	~FileParser();
};

