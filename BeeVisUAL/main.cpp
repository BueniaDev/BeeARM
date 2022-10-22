#include <beearm.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <array>
#include <cassert>
using namespace beearm;
using namespace std;
using namespace std::placeholders;

class BeeVisUAL : public BeeARMInterface
{
    public:
	BeeVisUAL()
	{
	    core.setInterface(*this);
	}

	~BeeVisUAL()
	{

	}

	bool init()
	{
	    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	    {
		return sdl_error("SDL2 could not be initialized!");
	    }

	    window = SDL_CreateWindow("BeeVisUAL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		return sdl_error("Window could not be created!");
	    }

	    core.init();
	    return true;
	}

	void shutdown()
	{
	    core.shutdown();

	    if (window != NULL)
	    {
		SDL_DestroyWindow(window);
		window = NULL;
	    }

	    SDL_Quit();
	}

	void runcore()
	{
	    while (!quit)
	    {
		pollevents();
		runmachine();
	    }
	}

    private:
	BeeARM core;

	bool quit = false;
	SDL_Event event;

	SDL_Window *window = NULL;

	bool sdl_error(string message)
	{
	    cout << message << ": SDL_Error: " << SDL_GetError() << endl;
	    return false;
	}

	void pollevents()
	{
	    while (SDL_PollEvent(&event))
	    {
		switch (event.type)
		{
		    case SDL_QUIT: quit = true; break;
		}
	    }
	}

	void runmachine()
	{
	    core.runInstruction();
	}
};

int main(int argc, char *argv[])
{
    BeeVisUAL core;

    if (!core.init())
    {
	return 1;
    }

    core.runcore();
    core.shutdown();
    return 0;
}