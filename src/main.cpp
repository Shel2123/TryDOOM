#include <SDL3/SDL_main.h>
#include "app.h"

int main(int, char *[])
{
    App app;
    if (!app.init())
        return 1;
    app.run();
    return 0;
}
