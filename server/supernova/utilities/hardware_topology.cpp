//  parse hardware topology
//  Copyright (C) 2022 Christof Ressi
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#if defined(_WIN32)
#    include <windows.h>
#    include <malloc.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#    include <sys/sysctl.h>
#    include <errno.h>
#else /* Linux */
#    include <unistd.h>
#    include <sys/sysinfo.h>
#endif

#include <boost/thread.hpp>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#ifndef DEBUG_HARDWARE_TOPOLOGY
#    define DEBUG_HARDWARE_TOPOLOGY 0
#endif

namespace {

struct cpu_info {
    int physical_id;
    int core_id;
    int sibling_id;
    int id;

    bool operator<(const cpu_info& other) const { return to_int() < other.to_int(); }

    bool operator>(const cpu_info& other) const { return to_int() > other.to_int(); }

private:
    /* convert members to a single uint64_t to simplify comparison.
     * We try to keep siblings as far apart as possible, followed by physical
     * packages, so that cores of the same package are close to each other. */
    uint64_t to_int() const {
        return ((uint64_t)sibling_id << 24) | ((uint64_t)physical_id << 16) | ((uint64_t)core_id);
    }
};

std::vector<cpu_info> cpu_info_vec;
int core_count = 0;
int package_count = 0;

void print_cpu_info(void) {
    std::cout << "hardware topology:\n"
              << "\tlogical processors: " << cpu_info_vec.size() << "\n"
              << "\tCPU cores: " << core_count << "\n"
              << "\tphysical packages: " << package_count << "\n"
              << "\t---\n";
    for (auto& info : cpu_info_vec) {
        std::cout << "\t#" << info.id << " package: " << info.physical_id << ", core: " << info.core_id
                  << ", sibling: " << info.sibling_id << "\n";
    }
}

} /* namespace */

namespace nova {

void parse_hardware_topology(void) {
#if defined(_WIN32) /* Windows */
    typedef BOOL(WINAPI * func_type)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

    /* available since Windows XP SP3 */
    auto fn = (func_type)GetProcAddress(GetModuleHandleA("kernel32"), "GetLogicalProcessorInformation");
    if (!fn) {
        std::cout << "GetLogicalProcessorInformation() not supported" << std::endl;
        return;
    }

    /* call with size 0 to retrieve the actual size;
     * ERROR_INSUFFICIENT_BUFFER is expected. */
    DWORD err, size = 0;
    fn(NULL, &size);
    if ((err = GetLastError()) != ERROR_INSUFFICIENT_BUFFER) {
        std::cout << "GetLogicalProcessorInformation() failed (" << err << ")" << std::endl;
        return;
    }
    auto info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)alloca(size);
    if (fn(info, &size) == FALSE) {
        std::cout << "GetLogicalProcessorInformation() failed (" << GetLastError() << ")" << std::endl;
        return;
    }
    size_t n = (size_t)size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

    /* loop over CPU cores */
    for (size_t i = 0; i < n; ++i) {
        if (info[i].Relationship == RelationProcessorCore) {
            /* add all siblings to CPU list */
            int siblings = 0;
            ULONG_PTR mask = info[i].ProcessorMask;
            for (int j = 0; mask; j++, mask >>= 1) {
                if (mask & 1) {
                    cpu_info_vec.push_back({ 0, core_count, siblings, j });
                    siblings++;
                }
            }
            core_count++;
        }
    }

    /* loop again for physical packages */
    for (size_t i = 0; i < n; ++i) {
        if (info[i].Relationship == RelationProcessorPackage) {
            ULONG_PTR mask = info[i].ProcessorMask;
            /* loop over all processors and find corresponding t_cpuinfo */
            for (size_t j = 0; mask != 0; ++j, mask >>= 1) {
                if (mask & 1) {
                    for (auto& cpu : cpu_info_vec) {
                        if (cpu.id == j)
                            cpu.physical_id = package_count;
                    }
                }
            }
            package_count++;
        }
    }
#elif defined(__linux__) /* Linux */
    /* The file /proc/cpuinfo contains all logical CPUs where
     * each entry has a property "physical id" and "core id". */
    std::ifstream fs("/proc/cpuinfo");
    if (!fs) {
        std::cout << "could not open /proc/cpuinfo" << std::endl;
        return;
    }

    auto parse_line = [](const std::string& line, const char* key, int& value) {
        if (line.find(key) != std::string::npos) {
            auto pos = line.find(":");
            if (pos != std::string::npos) {
                value = std::stoi(line.substr(pos + 1));
                return true;
            } else {
                throw std::runtime_error("missing colon after key");
            }
        } else {
            return false;
        }
    };

    int physical_id = -1;
    int core_id = -1;
    try {
        for (std::string line; std::getline(fs, line);) {
            if (line.empty())
                continue;

            /* search for "physical id" and "core id" */
            if (!parse_line(line, "physical id", physical_id) && !parse_line(line, "core id", core_id)) {
                continue;
            }
            if (physical_id >= 0 && core_id >= 0) {
                int id = (int)cpu_info_vec.size();
                /* get sibling number */
                int sibling_id = 0;
                for (auto& cpu : cpu_info_vec) {
                    if ((cpu.physical_id == physical_id) && (cpu.core_id == core_id)) {
                        sibling_id++;
                    }
                }
                if (sibling_id == 0)
                    core_count++;

                /* check for new physical package */
                if (std::find_if(cpu_info_vec.begin(), cpu_info_vec.end(),
                                 [&](auto& cpu) { return cpu.physical_id == physical_id; })
                    == cpu_info_vec.end()) {
                    package_count++;
                }

                cpu_info_vec.push_back({ physical_id, core_id, sibling_id, id });

                physical_id = core_id = -1;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "could not parse /proc/cpuinfo: " << e.what() << std::endl;
        cpu_info_vec.clear();
        core_count = 0;
        package_count = 0;
        return;
    }
#else /* Apple, BSDs, etc. */
    // std::cout << "parse_hardware_topology() not implemented" << std::endl;
    return;
#endif

    /* parsing done */
#if DEBUG_HARDWARE_TOPOLOGY
    print_cpu_info(); /* print original list */
#endif
    /* sort the list so that we can simply pick
     * consecutive CPUs for effective thread pinning. */
    std::sort(cpu_info_vec.begin(), cpu_info_vec.end());
#if DEBUG_HARDWARE_TOPOLOGY
    /* print the sorted list */
    std::cout << "sorted CPU list:" << std::endl;
    for (auto& info : cpu_info_vec) {
        std::cout << "\t#" << info.id << " package: " << info.physical_id << ", core: " << info.core_id
                  << ", sibling: " << info.sibling_id << "\n";
    }
    std::cout << std::endl;
#endif
}

int get_cpu_for_thread_index(int thread_index) {
    if (!cpu_info_vec.empty()) {
        return cpu_info_vec[thread_index % cpu_info_vec.size()].id;
    } else {
        return thread_index;
    }
}

} /* namespace nova */
