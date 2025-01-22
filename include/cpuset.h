#ifndef CPUSET_H
#define CPUSET_H

#include <array>
#include <optional>
#include <string>
#include <tuple>

extern const std::string kCpuSetPath;

std::tuple<std::optional<std::array<int, 512>>, int, int>
ReadCpuSet(const std::string path);

#endif
