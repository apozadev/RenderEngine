#include <iostream>

#include "Core/Engine.h"
#include "Core/WindowManager.h"

int main(){
  core::Engine::GetInstance()->Init();
  core::WindowManager::GetInstance()->CreateWindow(640, 420, "App");
  core::Engine::GetInstance()->Run();
  return 0;
}