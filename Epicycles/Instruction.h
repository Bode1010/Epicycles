#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "Circle.h"

using namespace std;

struct Pos {
	float x;
	float y;
};

class Instruction
{
public:
	Instruction();
	char command = '1';
	vector<float> cmdLoc;
	Pos GetPos(float time);
	ImgNum Integrate(int circleIndex, float domainBeg, float domainEnd);
	~Instruction();
};

