#include "app.h"
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[])
{

  App app;
  if(constexpr AppConfig cfg; !app.init(cfg))
    {
      return 1;
    }
  app.run();
  app.shutdown();
  return 0;
}
