#include "cpuset.h"

#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  const char *path = kCpuSetPath;
  if (argc > 1) {
    path = argv[1];
    puts(path);
  }
  int cpus[512] = {0};
  size_t cpu_count = 0;
  int err = ReadCpuSet(cpus, &cpu_count, path);
  if (err != 0) {
    if (err == ENOENT) {
      puts("not exists");
      return 0;
    }
    printf("errno: %d\n", err);
    return err;
  }
  for (int i = 0; i < cpu_count; i++) {
    printf("cpu: %d\n", cpus[i]);
  }
}
