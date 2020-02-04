#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "FileParser.h"
#include "Circle.h"
#include <cstdlib>
const float PI = 3.141592654;

//Small letter problems
sf::Color GetRandomColor() {
	return sf::Color::Red;
	int num = rand() % 5;
	switch (num) {
	case 0:
		return sf::Color::Red;
	case 1:
		return sf::Color::Green;
	case 2:
		return sf::Color::Blue;
	case 3:
		return sf::Color::White;
	case 4:
		return sf::Color::Yellow;
	}
}

int main()
{
	//Opening image to be drawn
	ifstream inputFile;
	string fileName = "test";
	float scalFac = 1;//oct: 0.04 cat: 1.5 bird: 12
	int yOffset = 0;
	int xOffset = 0;
	inputFile.open("C:\\Users\\olabo\\OneDrive\\Pictures\\Looped Images\\" + fileName + ".svg");

	//if input file is not open, terminate
	if (!inputFile.is_open()) {
		cerr << "Input file was not opened" << endl;
		cin.get();
		return -1;
	}

	//checks the first four characters of every line for the charchters "<path" ie the instructions line
	string line;
	bool run = false;
	while (!run && getline(inputFile, line)) {//checks the first 4 letters of each line to see if has the beggining of the instructions
		string firstChars = "";
		int numOfLetters = 0; //used to count how many useful characters there have been since beg of line
		for (int i = 0; i < line.size(); i++) {
			if (!(line[i] == '	' || line[i] == ' ') && numOfLetters <= 4) {
				firstChars += line[i];
				numOfLetters++;
			}
			else if (numOfLetters > 4) {
				if (firstChars == "<path") {
					run = true;
					break;
				}
				else {
					continue;
				}
			}
		}
	}

	//Runs if instruction line is found
	if (run) {
		FileParser parser(scalFac);
		vector<Instruction> instructions = parser.parse(line);
		yOffset = -parser.meanY + 256;
		xOffset = -parser.meanX + 256;

		//print all instructions and their corresponding numbers. Used for debugging
		/*for (int i = 0; i < instructions.size(); i++) {
			cout << instructions[i].command << ": " << endl;
			for (int j = 0; j < instructions[i].cmdLoc.size(); j++) {
				cout << instructions[i].cmdLoc[j] << " ";
			}
			cout << endl;
		}*/
		
		int numOfCircles = 100;
		vector<Circle> circles;
		for (int i = -numOfCircles / 2; i < numOfCircles / 2; i++) {
			ImgNum coefficient;
			for (int j = 1; j < instructions.size(); j++) {//doesnt add first move function
				//Integrate the instruction over it's respective domain and add it to the coefficient
				coefficient.Add(instructions[j].Integrate(i, (j - 1) * 2 * PI / instructions.size(), (j) * 2 * PI / instructions.size()));
			}
		}

		/*******************************************Window Starts here **************************************************/
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(512, 512), "Epicycles-" + fileName, sf::Style::Resize | sf::Style::Close, settings);

		//print the raw image coordinates in lines, vertex array called picture
		/*
		//Draw original image
		vector<float> xVal;
		vector<float> yVal;
		for (int i = 0; i < instructions.size(); i++) {
			//cout << instructions[i].command << ": " << endl;
			for (int j = 0; j < instructions[i].cmdLoc.size(); j++) {
				//cout << instructions[i].cmdLoc[j] << " ";
				if (j % 2) {
					xVal.push_back(instructions[i].cmdLoc[j]);
				}
				else {
					yVal.push_back(instructions[i].cmdLoc[j]);
				}
			}
			//cout << endl;
		}

		sf::VertexArray picture(sf::LineStrip, xVal.size());
		for (int i = 0; i < xVal.size(); i++) {
			picture[i].position = sf::Vector2f(yVal[i]/scalFac + xOffset, xVal[i]/scalFac + yOffset);
			picture[i].color = GetRandomColor();
			//cout << yVal[i] << ", " << xVal[i] << endl;
		}
		cout << picture[xVal.size() - 1].position.x << ", " << picture[xVal.size() - 1].position.y << endl;
		picture[xVal.size() - 1].color = sf::Color::White;*/

		//print the processed image coordinates in lines, vertex array called finalPic
		Pos position;
		int steps = 10;
		sf::Color selectedColor;
		sf::VertexArray finalPic(sf::LinesStrip, steps * (instructions.size()));
		for (int i = 0; i < instructions.size(); i++) {
			for (int j = 1; j <= steps; j++) {
				float t = 1.5 / 3 * 2.f * j / steps;
				position = instructions[i].GetPos(t);
				position.x = position.x + xOffset;
				position.y = position.y + yOffset;
				finalPic[(i) * steps + j -1].position = (sf::Vector2f(position.x, position.y));
				if (j % 2 == 0) {
					selectedColor = GetRandomColor();
				}
				finalPic[(i)* steps + j - 1].color = selectedColor;
			}
		}
		
		while (window.isOpen()) {
			sf::Event evnt;
			while (window.pollEvent(evnt)) {

			}
			window.clear();
			window.draw(finalPic);
			//window.draw(picture);
			window.display();
		}

		/****************************************************************************************************************/
	}

	//If no line starting with "<path" is found...
	else {
		cerr << "File instructions not found" << endl;
		cin.get();
		return -1;
	}

	inputFile.close();
	cin.get();
	return 0;
}
