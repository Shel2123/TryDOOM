#include "app.h"

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
