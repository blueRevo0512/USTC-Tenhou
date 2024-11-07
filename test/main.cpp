#include "MainWindow.h"
#include <random>
#include <windows.h>

std::random_device rd;
std::mt19937 gen(rd());

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	auto& window = MainWindow::getInstance();
	window.init();
	window.run();
	window.cleanup();
	return 0;
}
