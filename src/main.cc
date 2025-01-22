#include "cpuset.h"

#include <cstring>
#include <iostream>

extern "C" {
#include <errno.h>
}

int main(int argc, char *argv[]) {
  auto path = kCpuSetPath;
  if (argc > 1) {
    path = argv[1];
    std::cout << "path: " << path << std::endl;
  }
  auto [cpus, cpu_count, err] = ReadCpuSet(path);
  if (err != 0) {
    if (err == ENOENT) {
      std::cout << "not exist" << std::endl;
      return 0;
    }
    std::cerr << "errno: " << err << ", error: " << strerror(err) << std::endl;
    return err;
  }
  for (int i = 0; i < cpu_count; i++) {
    std::cout << "cpu: " << cpus->at(i) << std::endl;
  }
  return 0;
}
