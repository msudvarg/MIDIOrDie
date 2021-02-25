#include <fstream>
#include <iostream>

#include <unistd.h>

#include "localcontroller.h"

#define E2 (82 * SAMPLE_RATE / N)

int main(int argc, char **argv) {
  LocalController controller;

  int p[2];
  pipe(p);
  int pid = fork();
  if (pid == 0) {
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);
    execl("/usr/bin/gnuplot", "gnuplot", 0);
  } else {
    close(1);
    dup(p[1]);
    close(p[0]);
    close(p[1]);
  }
  for (;;) { // Forever
    Pa_Sleep(10);
    std::ofstream fout { "plot.data" };
    float *data = controller.GetData();
    fout << "#\tX\tY" << std::endl;
    for (int i = 0; i < 200; i++) {
      fout << i * SAMPLE_RATE / N << "\t" << data[i] << std::endl;
    }
    std::cout << "plot 'plot.data' with lines" << std::endl;
  }
}