#include <fstream>
#include <iostream>

#include <unistd.h>

#include "localcontroller.h"

#define E2 (82 * SAMPLE_RATE / N)

int main(int argc, char **argv) {
  double fft_data[N];
  float raw_audio[N];
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
  std::cout << "set term x11;" << std::endl;
  for (;;) { // Forever
    std::cout << "unset multiplot;" << std::endl;
    Pa_Sleep(10);
    std::cout << "set multiplot layout 1,2;" << std::endl;
    controller.GetData(fft_data, raw_audio);
    std::cout << "$raw_audio << EOD" << std::endl;
    for (int i = 0; i < N; i++) {
      std::cout << i << "\t" << raw_audio[i] << std::endl;
    }
    std::cout << "EOD\n" << std::endl;
    std::cout << "$fft_data << EOD" << std::endl;
    for (int i = 0; i < N / 2 + 1; i++) {
      std::cout << i * SAMPLE_RATE / N << "\t" << fft_data[i] << std::endl;
    }
    std::cout << "EOD\n" << std::endl;
    std::cout << "set xrange [0:1024];" << std::endl;
    std::cout << "set yrange [-0.05:0.05];" << std::endl;
    std::cout << "plot $raw_audio with lines;" << std::endl;
    std::cout << "set xrange[150:400];" << std::endl;
    std::cout << "set yrange[-100:40];" << std::endl;
    std::cout << "plot $fft_data with lines;" << std::endl;
  }
}
