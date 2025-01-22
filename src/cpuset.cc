#include "cpuset.h"

#include <array>
#include <charconv>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

extern "C" {
#include <errno.h>
}

const std::string kCpuSetPath = "/sys/fs/cgroup/cpuset.cpus";

static const std::string kHyphen = "-";
static const std::string kComma = ",";

static std::tuple<int, int, bool> GetBeginEnd(const std::string_view part) {
  auto begin = 0, end = 0;
  auto length = part.find(kHyphen);
  auto is_range = false;
  if (length != std::string::npos) {
    auto part_end = part.data() + length;
    std::from_chars(part.data(), part_end, begin);
    std::from_chars(part_end + 1, part.data() + part.size(), end);
    is_range = true;
  } else {
    std::from_chars(part.data(), part.data() + part.size(), begin);
  }
  return std::make_tuple(begin, end, is_range);
}

static std::tuple<std::array<int, 512>, int> ReadCpus(std::string &line) {
  auto buffer = std::string_view(line);
  auto cpus = std::array<int, 512>();
  auto cpu_count = 0;
  auto fill = [&](std::string_view part) {
    auto [begin, end, is_range] = GetBeginEnd(part);
    if (is_range) {
      for (int i = begin; i <= end; ++i) {
        cpus[cpu_count++] = i;
      }
    } else {
      cpus[cpu_count++] = begin;
    }
  };
  auto content = line.c_str();
  for (auto length = buffer.find(kComma); length != std::string::npos;
       length = buffer.find(kComma)) {
    auto part = std::string_view(content, length);
    fill(part);
    content += length + 1;
    buffer = std::string_view(content);
  }
  if (buffer.empty())
    return std::make_tuple(std::move(cpus), cpu_count);
  fill(std::string_view(content, buffer.find_last_not_of(" \n\r\t") + 1));
  return std::make_tuple(std::move(cpus), cpu_count);
}

std::tuple<std::optional<std::array<int, 512>>, int, int>
ReadCpuSet(const std::string path) {
  auto cpuset = std::ifstream(path);
  if (cpuset.fail())
    return std::make_tuple(std::nullopt, 0, errno);
  auto line = std::string(std::istreambuf_iterator<char>(cpuset),
                          std::istreambuf_iterator<char>());
  auto [cpus, cpu_count] = ReadCpus(line);
  return std::make_tuple(std::move(cpus), cpu_count, 0);
}
