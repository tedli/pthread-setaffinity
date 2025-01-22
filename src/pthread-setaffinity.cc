// https://raw.githubusercontent.com/wangsl/pthread-setaffinity/main/pthread-setaffinity.c

#include "cpuset.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>

extern "C" {
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const cpu_set_t *cpuset) throw();
}

using PthreadSetAffinity = int (*)(pthread_t thread, size_t cpusetsize,
                                   const cpu_set_t *cpuset) throw();

static void *GetFunction(const char *function_name) {
  void *function = dlsym(RTLD_NEXT, function_name);
  if (function == NULL) {
    std::cerr << "get function failed, name: " << function_name
              << ", error: " << dlerror() << std::endl;
    exit(EXIT_FAILURE);
  }
  return function;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const cpu_set_t *cpuset) throw() {
  static const auto original_pthread_setaffinity_np =
      reinterpret_cast<PthreadSetAffinity>(
          GetFunction("pthread_setaffinity_np"));
  auto [cpus, cpu_count, err] = ReadCpuSet(kCpuSetPath);
  if (err != 0) {
    if (err == ENOENT) {
      std::cout << "no need to patch" << std::endl;
      return original_pthread_setaffinity_np(thread, cpusetsize, cpuset);
    }
    std::cerr << "read cpuset failed, path: " << kCpuSetPath
              << ", error: " << strerror(err) << std::endl;
    return err;
  }
  std::cout << "patching cpuset" << std::endl;
  cpu_set_t index;
  CPU_ZERO(&index);
  for (int i = 0; i < cpu_count; ++i) {
    CPU_SET(cpus->at(i), &index);
  }
  for (int i = 0; i < cpusetsize; ++i) {
    if (CPU_ISSET(i, &index)) {
      CPU_SET(i, cpuset);
    } else {
      CPU_CLR(i, cpuset);
    }
  }
  return original_pthread_setaffinity_np(thread, cpusetsize, cpuset);
}
