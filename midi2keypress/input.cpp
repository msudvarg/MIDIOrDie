#include "input.h"

#include <stdlib.h>
#include <unistd.h>

KeyEventSender::KeyEventSender() {
  name = "Test Device";
}

KeyEventSender::KeyEventSender(const char *name) {
  this->name = name;
}

void KeyEventSender::Connect() {
  int err;

  dev = libevdev_new();
  libevdev_set_name(dev, name);
  libevdev_enable_event_type(dev, EV_KEY);
  libevdev_enable_event_code(dev, EV_KEY, KEY_ENTER, NULL);
  libevdev_enable_event_code(dev, EV_KEY, KEY_F1, NULL);
  libevdev_enable_event_code(dev, EV_KEY, KEY_F2, NULL);
  libevdev_enable_event_code(dev, EV_KEY, KEY_F3, NULL);
  libevdev_enable_event_code(dev, EV_KEY, KEY_F4, NULL);
  libevdev_enable_event_code(dev, EV_KEY, KEY_F5, NULL);

  err = libevdev_uinput_create_from_device(dev,
					   LIBEVDEV_UINPUT_OPEN_MANAGED,
					   &uidev);

  if (err != 0) {
    throw "Could not create a new device. (Do you need \"modprobe uinput\"?)";
  }
    
  sleep(1); // HACK system needs time for device enrollment
}

void KeyEventSender::Buffer(int key, char code) {
  libevdev_uinput_write_event(uidev, EV_KEY, key, code);
}

void KeyEventSender::Send() {
  libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
}

void KeyEventSender::Disconnect() {
  libevdev_uinput_destroy(uidev);
  libevdev_free(dev);
}
