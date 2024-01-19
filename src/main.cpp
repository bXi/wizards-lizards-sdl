#include "main.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif


#include "entities/gate.h"

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
    Audio::UpdateMusicStreams();
    Window::StartFrame();

    State::Draw();

    Window::EndFrame();

}

int main(int argc, char* argv[])
{
    Window::InitWindow("Wizards & Lizards", 1280, 720);
    Audio::Init();

	Input::Init();
	ECS::init();

    BaseState* testState = new TestState;
    State::AddState("test", testState);

    BaseState* menuState = new MenuState;
    State::AddState("menu", menuState);

    BaseState* gameState = new GameState;
    State::AddState("game", gameState);

    State::Init("menu");



    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 60, 1);
    #else
    while (true) { mainloop(); }
    #endif

	return 0;
}

int WinMain(const int argc, char* argv[])
{
	return main(argc, argv);
}