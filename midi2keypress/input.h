#ifndef INPUT_H
#define INPUT_H

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

class KeyEventSender {
public:
  KeyEventSender();
  KeyEventSender(const char *name);
  void Connect();
  void Buffer(int keys);
  void Send();
  void Disconnect();
private:
  const char *name;
  struct libevdev *dev;
  struct libevdev_uinput *uidev;
};

#endif /* INPUT_H */
