// https://raw.githubusercontent.com/wangsl/pthread-setaffinity/main/pthread-setaffinity.c

#include "cpuset.h"

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COMMIT_ID
#define COMMIT_ID "dev"
#endif

typedef int (*PthreadSetAffinity)(pthread_t thread, size_t cpusetsize,
                                  const cpu_set_t *cpuset);

static PthreadSetAffinity original_pthread_setaffinity_np = NULL;

static void *GetFunction(const char *function_name) {
  void *function = dlsym(RTLD_NEXT, function_name);
  if (function == NULL) {
    char *error = dlerror();
    fprintf(stderr, "get function failed, name: '%s', error: %s", function_name,
            error);
    exit(EXIT_FAILURE);
  }
  return function;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const cpu_set_t *cpuset) {
  if (original_pthread_setaffinity_np == NULL)
    original_pthread_setaffinity_np =
        (PthreadSetAffinity)GetFunction("pthread_setaffinity_np");
  fprintf(stdout, "commit id: %s\n", COMMIT_ID);
  int cpus[512] = {0};
  size_t cpu_count = 0;
  int err = ReadCpuSet(cpus, &cpu_count, kCpuSetPath);
  if (err != 0) {
    if (err == ENOENT) {
      return original_pthread_setaffinity_np(thread, cpusetsize, cpuset);
    }
    fprintf(stderr, "read cpuset failed, path: '%s', error: %s", kCpuSetPath,
            strerror(err));
    return err;
  }
  cpu_set_t cpuset_;
  CPU_ZERO(&cpuset_);
  for (int i = 0; i < cpu_count; ++i) {
    CPU_SET(cpus[i], &cpuset_);
  }
  for (int i = 0; i < cpusetsize; ++i) {
    if (CPU_ISSET(i, &cpuset_)) {
      CPU_SET(i, cpuset);
    } else {
      CPU_CLR(i, cpuset);
    }
  }
  return original_pthread_setaffinity_np(thread, cpusetsize, cpuset);
}
