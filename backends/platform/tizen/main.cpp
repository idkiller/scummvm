#include <app.h>
#include <Elementary.h>

#include <memory>

#include "backends/platform/tizen/application.h"

void appcontrol_cb(app_control_h handle, void *data)
{
  ScummVM *vm = static_cast<ScummVM*>(data);
}

int main (int argc, char *argv[])
{
  std::unique_ptr<ScummVM> scummVM(new ScummVM());

  ui_app_lifecycle_callback_s events;

#define VM(data) (static_cast<ScummVM*>(data))
  events.create = [](void *data) -> bool { return VM(data)->onCreate(); };
  events.pause = [](void *data) { VM(data)->onResume(); };
  events.resume = [](void *data) { VM(data)->onResume(); };
  events.terminate = [](void *data) { VM(data)->onTerminate(); };
  events.app_control = appcontrol_cb;
#undef VM

  return 0;
}
