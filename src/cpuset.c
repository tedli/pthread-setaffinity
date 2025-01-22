#include "cpuset.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *kCpuSetPath = "/sys/fs/cgroup/cpuset.cpus";

static const char kEol = '\0';
static const char kHyphen = '-';
static const char kComma = ',';
static const char kNewLine = '\n';
static const char kCarriageReturn = '\r';
static const char kLiteralZero = '0';

static void FillCpus(int *cpus, size_t *length, int begin, int end,
                     bool is_range) {
  if (is_range) {
    for (int i = begin; i <= end; ++i) {
      cpus[(*length)++] = i;
    }
  } else {
    cpus[(*length)++] = begin;
  }
}

static int ParseUnsignedDecimal(char *const begin, char *const end) {
  int value = 0;
  for (char *i = begin; i != end; ++i) {
    value = value * 10 + (*i - kLiteralZero);
  }
  return value;
}

static void ParseRange(char *const part_begin, char *const part_end,
                       char *const hyphen_index, int *cpus, size_t *length,
                       void(callback)(int *, size_t *, int, int, bool)) {
  bool is_range = hyphen_index != NULL;
  int begin = 0;
  int end = 0;
  if (is_range) {
    begin = ParseUnsignedDecimal(part_begin, hyphen_index);
    end = ParseUnsignedDecimal(hyphen_index + 1, part_end);
  } else {
    begin = ParseUnsignedDecimal(part_begin, part_end);
  }
  callback(cpus, length, begin, end, is_range);
}

static void ParseLine(char *const line_begin, char *const line_end, int *cpus,
                      size_t *length,
                      void(callback)(int *, size_t *, int, int, bool)) {
  char *part_begin = line_begin;
  char *comma_index = NULL;
  char *hyphen_index = NULL;
  char *i = NULL;
  int begin = 0;
  int end = 0;
  for (i = part_begin; i != line_end; ++i) {
    if (*i == kComma) {
      ParseRange(part_begin, i, hyphen_index, cpus, length, callback);
      part_begin = i + 1;
      hyphen_index = NULL;
    } else if (*i == kHyphen) {
      hyphen_index = i;
    } else if (*i == kNewLine || *i == kEol || *i == kCarriageReturn) {
      ParseRange(part_begin, i, hyphen_index, cpus, length, callback);
      break;
    }
  }
}

int ReadCpuSet(int *cpus, size_t *length, const char *path) {
  FILE *cpu_set_file = fopen(path, "r");
  if (cpu_set_file == NULL) {
    return errno;
  }
  fseek(cpu_set_file, 0L, SEEK_END);
  int content_length = ftell(cpu_set_file);
  fseek(cpu_set_file, 0L, SEEK_SET);
  char line[1024] = {0};
  fread(&line, sizeof(char), content_length, cpu_set_file);
  fclose(cpu_set_file);
  ParseLine(line, line + content_length, cpus, length, FillCpus);
  return 0;
}
