#include <iostream>

#include "localcontroller.h"

#define E2 (82 * SAMPLE_RATE / N)

int main(int argc, char **argv) {
  LocalController controller;
  Pa_Sleep(1000);
  float *data = controller.GetData();
  std::cout << "#\tX\tY" << std::endl;
  for (int i = 0; i < N/2 + 1; i++) {
    std::cout << i * SAMPLE_RATE / N << "\t" << data[i] << std::endl;
  }
}
