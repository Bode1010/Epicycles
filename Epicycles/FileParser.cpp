#include "FileParser.h"



FileParser::FileParser(float s)
{
	scale = s;
}

int FileParser::numOfVariables(char instruction) {
	if (instruction == 'c' || instruction == 'C')
		return 8;
	else if (instruction == 'h' || instruction == 'H')
		return 4;
	else if (instruction == 'l' || instruction == 'L')
		return 4;
	else if (instruction == 's' || instruction == 'S')//if the last instruction was a c, dont use the first variables as part of execution
		return 8;
	else if (instruction == 'q' || instruction == 'Q') {
		return 6;
	}
	else if (instruction == 'v' || instruction == 'V')
		return 1;
}

vector<Instruction> FileParser::parse(string line) {
	//Breaks up instuction "chunk" into their individual instrcutions and puts them in a string vector(array)
	char loopChar;
	int mCounter = 0; //count the number of move instructions
	bool startInstructionSearch = false; // this is used to check if we are past the <path d=" point

	string numberInString = ""; //current number being scanned in string form
	float number = 0; //current number being scanned
	vector<Instruction> instructions;
	int instructionCounter = 0; //places loop in current instruction
	int instructionNumberCounter = 0;
	float readX = 0;
	float readY = 0;
	float isReadingX = true;

	bool smallLetter = true;

	for (int i = 0; i < line.size(); i++) {
		loopChar = line[i];
		if (loopChar == '"') {
			startInstructionSearch = !startInstructionSearch;
			i++;
			loopChar = line[i];
		}

		//WARNING: This entire process will fall apart if I encounter an instruction command that i've not prepared for
		if (startInstructionSearch) {// if we are parsing through the instructions
			if (loopChar == 'm') {
				instructions.push_back(Instruction());
				instructions[instructionCounter].command = loopChar;

				numberInString = "";
				smallLetter = true;
				mCounter++;
			}
			else if (loopChar == 'M') {
				instructions.push_back(Instruction());
				instructions[instructionCounter].command = loopChar;

				numberInString = "";
				smallLetter = false;
				mCounter++;
			}
			else if (loopChar == 'c' || loopChar == 'h' || loopChar == 'z' || loopChar == 'q' || loopChar == 'v' || loopChar == 'l' || loopChar == 's') {//if the current character is an instruction
				
				//add new number
				if (numberInString != "") {
					totalNumOfInstructions++;
					stringstream numConv(numberInString);
					numConv >> number;
					number /= scale;

					instructions[instructionCounter].cmdLoc.push_back(float());
					if (smallLetter) {
						//Checks whether number that is currently being pushed into array is x or y and adding accordingly
						if (instructions[instructionCounter].command == 'h') {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = true;
						}
						else if (instructions[instructionCounter].command == 'v') {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = true;
						}
						else if (isReadingX) {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = !isReadingX;
						}
						else {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = !isReadingX;
						}
					}

					else {
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = number;
						if (isReadingX) {
							meanX += number;
							isReadingX = !isReadingX;
						}
						else {
							meanY += number;
							isReadingX = !isReadingX;
						}
					}
					instructionNumberCounter++;
				}

				smallLetter = true;

				//if the instruction we just finished was a h, tag a zero at the end
				if (instructions[instructionCounter].command == 'h') {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = 0;
					instructionNumberCounter++;
				}

				//create new instruction
				instructions.push_back(Instruction());
				instructionCounter++;
				instructions[instructionCounter].command = loopChar;
				instructionNumberCounter = 0;
				numberInString = "";

				//if s is following a c or another s, copy the last 4 numbers
				if (instructions[instructionCounter].command == 's' && (instructions[instructionCounter - 1].command == 's' || instructions[instructionCounter - 1].command == 'c' || instructions[instructionCounter - 1].command == 'S' || instructions[instructionCounter - 1].command == 'C')) {
					//3rd vertex in past c
					float pastCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 4];
					float pastCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 3];
					//4th vertex in past c
					float curCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
					float curCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];

					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos;
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos;
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos + (curCyPos - pastCyPos);
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos + (curCxPos - pastCxPos);
					instructionNumberCounter++;
				}

				//else copy the last 2 numbers
				else {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];
					instructionNumberCounter++;
				}

				//if its z, push back the first cursor position, ie draw a line to the beg
				if (instructions[instructionCounter].command == 'z') {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[0].cmdLoc[0];
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[0].cmdLoc[1];
					instructionNumberCounter++;
				}

				//if the instruction we are coming to is a v, tag a zero at the beginning
				if (instructions[instructionCounter].command == 'v') {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = 0;
					instructionNumberCounter++;
				}


			}

			//if the current character is an instruction wt cap letters
			else if (loopChar == 'C' || loopChar == 'Z' || loopChar == 'Q' || loopChar == 'L' || loopChar == 'S') {
				
				//add new number if there is any
				if (numberInString != "") {
					totalNumOfInstructions++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					stringstream numConv(numberInString);
					numConv >> number;
					number /= scale;

					if (smallLetter) {
						//Checks whether number that is currently being pushed into array is x or y and adding accordingly
						if (instructions[instructionCounter].command == 'h') {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = true;
						}
						else if (instructions[instructionCounter].command == 'v') {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = true;
						}
						else if (isReadingX) {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = !isReadingX;
						}
						else {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = !isReadingX;
						}
					}

					else {
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = number;
						if (isReadingX) {
							meanX += number;
							isReadingX = !isReadingX;
						}
						else {
							meanY += number;
							isReadingX = !isReadingX;
						}
					}
					instructionNumberCounter++;
				}

				smallLetter = false;

				//create new instructions
				instructions.push_back(Instruction());
				instructionCounter++;
				instructions[instructionCounter].command = loopChar;
				instructionNumberCounter = 0;
				numberInString = "";

				//if s is following a c or another s, copy the last 4 numbers
				if (instructions[instructionCounter].command == 'S' && (instructions[instructionCounter - 1].command == 's' || instructions[instructionCounter - 1].command == 'c' || instructions[instructionCounter - 1].command == 'S' || instructions[instructionCounter - 1].command == 'C')) {
					//3rd vertex in past c
					float pastCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 4];
					float pastCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 3];
					//4th vertex in past c
					float curCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
					float curCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];

					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos;
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos;
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos + (curCxPos - pastCxPos);
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos + (curCyPos - pastCyPos);
					instructionNumberCounter++;
				}

				//else copy last 2 numbers
				else {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];
					instructionNumberCounter++;
				}

				//if its z, push back the first cursor position, ie draw a line to the beg
				if (instructions[instructionCounter].command == 'Z') {
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[0].cmdLoc[0];
					instructionNumberCounter++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[0].cmdLoc[1];
					instructionNumberCounter++;
				}

			}

			else if (loopChar == ' ' || loopChar == ',') {
				// if loopChar is a space or a minus sign, its a new number
				//Break up batch instructions into single instructions:
				if (instructionNumberCounter >= numOfVariables(instructions[instructionCounter].command)) {
					instructions.push_back(Instruction());
					instructionCounter++;
					instructions[instructionCounter].command = instructions[instructionCounter - 1].command;
					instructionNumberCounter = 0;

					//if s is following a c or another s, copy the last 4 numbers
					if ((instructions[instructionCounter].command == 's' || instructions[instructionCounter].command == 'S') && (instructions[instructionCounter - 1].command == 's' || instructions[instructionCounter - 1].command == 'c' || instructions[instructionCounter - 1].command == 'S' || instructions[instructionCounter - 1].command == 'C')) {
						//3rd vertex in past c
						float pastCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 4];
						float pastCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 3];
						//4th vertex in past c
						float curCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
						float curCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];

						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos;
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos;
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos + (curCyPos - pastCyPos);
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos + (curCxPos - pastCxPos);
						instructionNumberCounter++;
					}

					//else copy the last 2 numbers
					else {
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];
						instructionNumberCounter++;
					}
				}

				//add new number
				if (numberInString != "") {
					totalNumOfInstructions++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					stringstream numConv(numberInString);
					numConv >> number;
					number /= scale;

					if (smallLetter) {
						//Checks whether number that is currently being pushed into array is x or y and adding accordingly
						if (instructions[instructionCounter].command == 'h') {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = true;
						}
						else if (instructions[instructionCounter].command == 'v') {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = true;
						}
						else if (isReadingX) {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = !isReadingX;
						}
						else {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = !isReadingX;
						}
					}

					else {
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = number;
						if (isReadingX) {
							isReadingX = !isReadingX;
							meanX += number;
						}
						else {
							isReadingX = !isReadingX;
							meanY += number;
						}
					}
					instructionNumberCounter++;

					numberInString = "";
				}
			}

			else if (loopChar == '-') {
				//Break up batch instructions into single instructions:
				if (instructionNumberCounter >= numOfVariables(instructions[instructionCounter].command)) {
					instructions.push_back(Instruction());
					instructionCounter++;
					instructions[instructionCounter].command = instructions[instructionCounter - 1].command;
					instructionNumberCounter = 0;

					//if s is following a c or another s, copy the last 4 numbers
					if ((instructions[instructionCounter].command == 's' || instructions[instructionCounter].command == 'S') && (instructions[instructionCounter - 1].command == 's' || instructions[instructionCounter - 1].command == 'c' || instructions[instructionCounter - 1].command == 'S' || instructions[instructionCounter - 1].command == 'C')) {
						//3rd vertex in past c
						float pastCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 4];
						float pastCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 3];
						//4th vertex in past c
						float curCyPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
						float curCxPos = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];

						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos;
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos;
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCyPos + (curCyPos - pastCyPos);
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = curCxPos + (curCxPos - pastCxPos);
						instructionNumberCounter++;
					}

					//else copy the last 2 numbers
					else {
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 2];
						instructionNumberCounter++;
						instructions[instructionCounter].cmdLoc.push_back(float());
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = instructions[instructionCounter - 1].cmdLoc[instructions[instructionCounter - 1].cmdLoc.size() - 1];
						instructionNumberCounter++;
					}
				}

				//add new number
				if (numberInString != "") {
					totalNumOfInstructions++;
					instructions[instructionCounter].cmdLoc.push_back(float());
					stringstream numConv(numberInString);
					numConv >> number;
					number /= scale;
					if (smallLetter) {
						//Checks whether number that is currently being pushed into array is x or y and adding accordingly
						if (instructions[instructionCounter].command == 'h') {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = true;
						}
						else if (instructions[instructionCounter].command == 'v') {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = true;
						}
						else if (isReadingX) {
							readX = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readX + number;
							meanX += readX;
							isReadingX = !isReadingX;
						}
						else {
							readY = instructions[instructionCounter].cmdLoc[instructionNumberCounter] = readY + number;
							meanY += readY;
							isReadingX = !isReadingX;
						}
					}

					else {
						instructions[instructionCounter].cmdLoc[instructionNumberCounter] = number;
						if (isReadingX) {
							meanX += readX;
							isReadingX = !isReadingX;
						}
						else {
							meanY += readY;
							isReadingX = !isReadingX;
						}
					}
					instructionNumberCounter++;
				}
				numberInString = loopChar;
			}

			else {
				numberInString += loopChar;
			}

		}
	}

	if (mCounter > 1) {
		cerr << "More than one move function, not a perfect loop, cannot replicate" << endl;
		cin.get();
		exit(1);
	}

	meanX = meanX / (totalNumOfInstructions * 0.5);
	meanY = meanY / (totalNumOfInstructions * 0.5);

	return instructions;

	//calculate the mean position and make that the origin

	//adjust each "instruction" value to new value relative to the mean position

	//reads from the file and puts each "instruction" into its own function that can be followed by the epicycle

	//Follows instuctions to ensure the right image is constructed from extracted instructions

}

FileParser::~FileParser()
{
}
