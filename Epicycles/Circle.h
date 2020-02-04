#pragma once
struct ImgNum {
	float num;
	float imgNum;

	void Add(ImgNum other) {
		num = num + other.num;
		imgNum = imgNum + other.imgNum;
	}
};

class Circle
{
public:
	Circle();
	float radSize = 0;
	float angle = 0;
	ImgNum coefficient;
	~Circle();
};

