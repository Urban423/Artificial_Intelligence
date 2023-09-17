#include <iostream>
#include "PNG_reader.h"
#include "Window.h"

double sigmoid(double x)
{
	return (1 / (1 + pow(2.718281828, -x)));
}

int main()
{
	PNG_reader image("test/Face.png");
	image.make_larger(5);


	Window win(image);
	while (win.isRunning());

	return 0;
}