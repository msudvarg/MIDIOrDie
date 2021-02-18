#include "input.h"

#include <iostream>

int main() {
  KeyEventSender kes;

  try {
    kes.Connect();
    kes.Buffer(KEY_F1);
    kes.Send();
    kes.Disconnect();
  } catch (const char *err) {
    std::cerr << err << std::endl;
  }
}
