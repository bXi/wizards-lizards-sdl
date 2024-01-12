#include "main.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif


static void mainloop()
{
    if (Window::ShouldQuit()) {
        State::Unload();
        Window::Close();

        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();  /* this should "kill" the app. */
        #else
        exit(0);
        #endif
    }

    Window::StartFrame();

    State::Draw();

    Window::EndFrame();

}

int main(int argc, char* argv[])
{
    Audio::Init();
    Window::InitWindow("Wizards & Lizards", 1280, 720);
	Input::Init();

    BaseState* testState = new TestState;
    State::AddState("test", testState);

    BaseState* menuState = new MenuState;
    State::AddState("menu", menuState);

    BaseState* gameState = new GameState;
    State::AddState("game", gameState);

    State::Init("game");

	ECS::init();

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
    #else
    while (1) { mainloop(); }
    #endif

	return 0;
}

int WinMain(const int argc, char* argv[])
{
	return main(argc, argv);
}