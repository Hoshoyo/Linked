#include "Display.h"
#include <time.h>

//#include <vld.h>

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	Display* display = new Display(&argc, argv, "Linked", 1200, 675);
	delete display;
	return 0;
}