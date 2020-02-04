#include "Instruction.h"



Instruction::Instruction()
{
}

//returns the position of any instruction when given a time from zero to one
Pos Instruction::GetPos(float t) {
	Pos finalPos;
	finalPos.x = 0;
	finalPos.y = 0;

	if (command == '1') {//this is checking for the number one(1)
		cout << "error, command for this instruction has not been assigned" << endl;
		return finalPos;
	}
	else {
		if (command == 'c' || command == 's' || command == 'C' || command == 'S') {
			finalPos.x = (1 - t)*((1-t)*((1 - t)*cmdLoc[1] + (t)*(cmdLoc[3])) + (t)*((1 - t)*cmdLoc[3] + (t)*cmdLoc[5])) 
				+ (t)*((1-t)*((1 - t)*cmdLoc[3] + (t)*(cmdLoc[5])) + (t)*((1 - t)*cmdLoc[5] + (t)*(cmdLoc[7])));
			finalPos.y = (1 - t)*((1 - t)*((1 - t)*cmdLoc[0] + (t)*(cmdLoc[2])) + (t)*((1 - t)*cmdLoc[2] + (t)*(cmdLoc[4])))
				+ (t)*((1 - t)*((1 - t)*cmdLoc[2] + (t)*(cmdLoc[4])) + (t)*((1 - t)*cmdLoc[4] + (t)*(cmdLoc[6])));
		}
		else if (command == 'l' || command == 'h' || command == 'v' || command == 'z' || command == 'L' || command == 'Z') {// this is checking for the letter l
			finalPos.x = (1 - t)*(cmdLoc[1]) + (t)*(cmdLoc[3]);
			finalPos.y = (1 - t)*(cmdLoc[0]) + (t)*(cmdLoc[2]);
		}
		else if (command == 'm' || command == 'M') {
			finalPos.x = cmdLoc[1];
			finalPos.y = cmdLoc[0];
		}
		else {
			cout << "Didnt account for command " << command << endl;
		}
	}
	float temp = finalPos.x;
	finalPos.x = finalPos.y;
	finalPos.y = temp;
	return finalPos;
}

ImgNum Instruction::Integrate(int circIndex, float domainBeg, float domainEnd) {
	ImgNum result;
	if (command == 'c' || command == 's' || command == 'C' || command == 'S') {
		result.num = 0;//will use imaginary number calculator >:)
		result.imgNum = 0;
	}
	else if (command == 'l' || command == 'h' || command == 'v' || command == 'z' || command == 'L' || command == 'Z') {
		result.num = 0;
		result.imgNum = 0;
	}
	else {
		cout << "Didnt account for command " << command  << " while integrating" << endl;
	}
	return result;
}

Instruction::~Instruction()
{
}
