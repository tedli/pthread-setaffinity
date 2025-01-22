#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

def parse_session_options(cpu_set_path = "/sys/fs/cgroup/cpuset.cpus"):
    cs = cpu_set(cpu_set_path)
    print(cs)

def cpu_set(cpu_set_path = "/sys/fs/cgroup/cpuset.cpus"):
    try:
        with open(cpu_set_path, 'r') as cpu_set:
            cpus = cpu_set.read().rstrip()
    except FileNotFoundError:
        return []
    except:
        return []
    else:
        cpu_set = []
        for part in cpus.split(","):
            begin_end = part.split("-")
            if len(begin_end) == 1:
                cpu_set.append(int(begin_end[0]))
            else:
                for i in range(int(begin_end[0]), int(begin_end[1])+1):
                    cpu_set.append(i)
        return [i + 1 for i in cpu_set]


def session_options(cpu_set_path = "/sys/fs/cgroup/cpuset.cpus"):
    options = ort.SessionOptions()
    try:
        with open(cpu_set_path, 'r') as cpu_set:
            cpus = cpu_set.read().rstrip()
    except FileNotFoundError:
        return options
    except:
        return options
    else:
        cpu_set = []
        for part in cpus.split(","):
            begin_end = part.split("-")
            if len(begin_end) == 1:
                cpu_set.append(int(begin_end[0]))
            else:
                for i in range(int(begin_end[0]), int(begin_end[1])+1):
                    cpu_set.append(i)
        options.intra_op_num_threads = len(cpu_set) + 1
        options.add_session_config_entry('session.intra_op_thread_affinities', ";".join([str(i + 1) for i in cpu_set]))
        return options


if __name__=="__main__":
    parse_session_options(sys.argv[1])
