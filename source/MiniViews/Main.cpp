#include "../MiniViewsLib/MiniViews.h"

#ifdef _WIN32
#ifdef _DEBUG
#include "../MiniViewsLib/CLI.h"
CLI cli;
#endif
#endif

MiniViews miniViews; // The main instance of MiniViews

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
    return miniViews.Run(nCmdShow);
}
