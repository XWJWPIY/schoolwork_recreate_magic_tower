#include "App.hpp"

#include "Core/Context.hpp"

int main(int, char **) {
  auto context = Core::Context::GetInstance();
  App app;

  while (!context->GetExit()) {
    switch (app.GetCurrentState()) {
    case App::STATE::START:
      app.Start();
      break;

    case App::STATE::UPDATE:
      app.Update();
      break;

    case App::STATE::END:
      app.End();
      context->SetExit(true);
      break;
    }
    context->Update();
  }
  return 0;
}
