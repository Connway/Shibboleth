/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include <Shibboleth_App.h>

#ifdef PLATFORM_WINDOWS
	// Force machines with integrated graphics and discrete GPUs to favor discrete GPUs.
	// https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129#39047129
	extern "C"
	{
		__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}

	int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
	{
		int32_t f = nullptr;

		Shibboleth::App app;

		//while (true) {
		//}

		if (!app.init()) {
			app.destroy();
			return -1;
		}

		app.run();
		app.destroy();

		return 0;
	}

#else
	int main(int argc, const char** argv)
	{
		Shibboleth::App app;

		if (!app.init(argc, argv)) {
			app.destroy();
			return -1;
		}

		app.run();
		app.destroy();

		return 0;
	}
#endif
