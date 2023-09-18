#include <iostream>
#include "PNG_reader.h"
#include "Window.h"
#include <vector>

#include <time.h>
#include <stdlib.h>

using namespace std;

#define e 2.718281828459

struct Perceptron
{
	double value = 0;
	vector<double> weightArray;
};

struct AI
{
	vector<Perceptron> neironArray;
	vector<int> columnSizeArray;
};

double sigmoid(double x)
{
	return (1 / (1 + pow(e, -x)));
}


void savePerceptron(const Perceptron& perceptron, const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "wb");

	int size = perceptron.weightArray.size();
	fwrite(&size, sizeof(int), 1, f);
	for (int i = 0; i < perceptron.weightArray.size(); i++)
	{
		fwrite(&perceptron.weightArray[i], sizeof(double), 1, f);
	}

	fclose(f);
}

void readPerceprton(Perceptron& perceptron, const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");

	int size;
	fread(&size, sizeof(int), 1, f);
	for (int i = 0; i < size; i++)
	{
		perceptron.weightArray.push_back(0);
		fread(&perceptron.weightArray[i], sizeof(double), 1, f);
	}

	fclose(f);
}

int main()
{
	PNG_reader images[] = {
		PNG_reader("numbers/1.0.png"),
		PNG_reader("numbers/1.1.png"),
		//PNG_reader("numbers/1.2.png"),
		PNG_reader("numbers/1.3.png"),
		PNG_reader("numbers/1.4.png"),
		PNG_reader("numbers/1.5.png"),
	};
	const int images_number = 5;


	srand(time(NULL));
	Perceptron perceptron;
	//init
	for (int i = 0; i < 64; i++)
	{
		perceptron.weightArray.push_back(0);
	}

	int s = 0;
	cout << "learn? : ";
	cin >> s;
	if(s) {
		while (1)
		{
			char flag = 1;
			for (int i = 0; i < 64; i++) {
				perceptron.weightArray[i] = (double)(rand() % 10000) / 10000;
			}

			for (int y = 0; y < images_number; y++) {
				perceptron.value = 0;
				for (int i = 0; i < 64; i++) {
					perceptron.value += (double)images[y].image_data[i * images[y].bytes_per_pixel] / 255 * perceptron.weightArray[i];
				}

				perceptron.value = sigmoid(perceptron.value - 5);
				if (perceptron.value < 0.5f) {
					flag = 0;
					break;
				}
			}

			if (flag) {
				break;
			}
		}
		savePerceptron(perceptron, "./test/ai.txt");
	}
	else 
	{
		readPerceprton(perceptron, "./test/ai.txt");

		for (int y = 0; y < images_number; y++)
		{
			perceptron.value = 0;
			for (int i = 0; i < 64; i++)
			{
				perceptron.value += (double)images[y].image_data[i * images[y].bytes_per_pixel] / 255 * perceptron.weightArray[i];
			}
			perceptron.value = sigmoid(perceptron.value - 5);

			printf("%f\n", perceptron.value);
		}
	}


	system("pause");
	return 0;
}