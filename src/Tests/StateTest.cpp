/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include <Shibboleth_IState.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_App.h>
#include <iostream>

class TestState : public Shibboleth::IState
{
public:
	TestState(Shibboleth::App& app): _app(app) {}
	~TestState(void) {}

	bool init(unsigned int)
	{
		return true;
	}

	void enter(void)
	{
		std::cout << "Test State ENTER" << std::endl;
	}

	void update(void)
	{
		static int i = 0;
		++i;
		std::cout << "Test State UPDATE" << std::endl;

		if (i >= 1000)
			_app.quit();
	}

	void exit(void)
	{
		std::cout << "Test State EXIT" << std::endl;
	}

private:
	Shibboleth::App& _app;
};

DYNAMICEXPORT Shibboleth::IState* CreateState(Shibboleth::App& app)
{
	Shibboleth::SetAllocator(app.getAllocator());
	return app.getAllocator().template allocT<TestState>(app);
}

DYNAMICEXPORT void DestroyState(Shibboleth::IState* state)
{
	Shibboleth::GetAllocator().freeT(state);
}
