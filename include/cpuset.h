#ifndef CPUSET_H
#define CPUSET_H

#include <stdlib.h>

extern const char *kCpuSetPath;

int ReadCpuSet(int *cpus, size_t *length, const char *path);

#endif
