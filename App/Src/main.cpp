#include <iostream>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/WindowManager.h"
#include "Core/Exception.h"

int main(){

  try
  {

  core::Engine::GetInstance()->Initialize();
  core::WindowManager::GetInstance()->CreateNewWindow(640, 420, "App");  
  return core::Engine::GetInstance()->Run();

  }
  catch (const core::Exception& e)
  {
    MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
  }
  catch (const std::exception& e)
  {
    MessageBoxA(nullptr, e.what(), "Generic Exception", MB_OK | MB_ICONEXCLAMATION);
  }
  catch (...)
  {
    MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
  }

  return -1;

}