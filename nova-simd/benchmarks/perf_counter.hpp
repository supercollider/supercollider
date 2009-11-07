//  Copyright (C) 2008 Tim Blechmann
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

#ifndef PERF_COUNTER_HPP
#define PERF_COUNTER_HPP

#include <cassert>
#include <cstring>
#include <exception>
#include <iostream>
#include <iomanip>


#include <boost/array.hpp>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>


#include <linux/types.h>

#ifdef PERF_COUNTER_H

#include <sys/perf_counter.h>
#include <linux/syscalls.h>
#else

/*
 * hw_event.type
 */
enum perf_event_types {
	PERF_TYPE_HARDWARE		= 0,
	PERF_TYPE_SOFTWARE		= 1,
	PERF_TYPE_TRACEPOINT		= 2,

	/*
	 * available TYPE space, raw is the max value.
	 */

	PERF_TYPE_RAW			= 128,
};

/*
 * Generalized performance counter event types, used by the hw_event.event_id
 * parameter of the sys_perf_counter_open() syscall:
 */
enum hw_event_ids {
	/*
	 * Common hardware events, generalized by the kernel:
	 */
	PERF_COUNT_CPU_CYCLES		= 0,
	PERF_COUNT_INSTRUCTIONS		= 1,
	PERF_COUNT_CACHE_REFERENCES	= 2,
	PERF_COUNT_CACHE_MISSES		= 3,
	PERF_COUNT_BRANCH_INSTRUCTIONS	= 4,
	PERF_COUNT_BRANCH_MISSES	= 5,
	PERF_COUNT_BUS_CYCLES		= 6,

	PERF_HW_EVENTS_MAX		= 7,
};

/*
 * Special "software" counters provided by the kernel, even if the hardware
 * does not support performance counters. These counters measure various
 * physical and sw events of the kernel (and allow the profiling of them as
 * well):
 */
enum sw_event_ids {
	PERF_COUNT_CPU_CLOCK		= 0,
	PERF_COUNT_TASK_CLOCK		= 1,
	PERF_COUNT_PAGE_FAULTS		= 2,
	PERF_COUNT_CONTEXT_SWITCHES	= 3,
	PERF_COUNT_CPU_MIGRATIONS	= 4,
	PERF_COUNT_PAGE_FAULTS_MIN	= 5,
	PERF_COUNT_PAGE_FAULTS_MAJ	= 6,

	PERF_SW_EVENTS_MAX		= 7,
};

/*
 * IRQ-notification data record type:
 */
enum perf_counter_record_type {
	PERF_RECORD_SIMPLE		= 0,
	PERF_RECORD_IRQ			= 1,
	PERF_RECORD_GROUP		= 2,
};

#define __PERF_COUNTER_MASK(name) 			\
	(((1ULL << PERF_COUNTER_##name##_BITS) - 1) <<	\
	 PERF_COUNTER_##name##_SHIFT)

#define PERF_COUNTER_RAW_BITS		1
#define PERF_COUNTER_RAW_SHIFT		63
#define PERF_COUNTER_RAW_MASK		__PERF_COUNTER_MASK(RAW)

#define PERF_COUNTER_CONFIG_BITS	63
#define PERF_COUNTER_CONFIG_SHIFT	0
#define PERF_COUNTER_CONFIG_MASK	__PERF_COUNTER_MASK(CONFIG)

#define PERF_COUNTER_TYPE_BITS		7
#define PERF_COUNTER_TYPE_SHIFT		56
#define PERF_COUNTER_TYPE_MASK		__PERF_COUNTER_MASK(TYPE)

#define PERF_COUNTER_EVENT_BITS		56
#define PERF_COUNTER_EVENT_SHIFT	0
#define PERF_COUNTER_EVENT_MASK		__PERF_COUNTER_MASK(EVENT)

/*
 * Bits that can be set in hw_event.read_format to request that
 * reads on the counter should return the indicated quantities,
 * in increasing order of bit value, after the counter value.
 */
enum perf_counter_read_format {
	PERF_FORMAT_TOTAL_TIME_ENABLED	=  1,
	PERF_FORMAT_TOTAL_TIME_RUNNING	=  2,
};

/*
 * Hardware event to monitor via a performance monitoring counter:
 */
struct perf_counter_hw_event {
	/*
	 * The MSB of the config word signifies if the rest contains cpu
	 * specific (raw) counter configuration data, if unset, the next
	 * 7 bits are an event type and the rest of the bits are the event
	 * identifier.
	 */
	__u64			config;

	__u64			irq_period;
	__u64			record_type;
	__u64			read_format;

	__u64			disabled       :  1, /* off by default        */
				nmi	       :  1, /* NMI sampling          */
				inherit	       :  1, /* children inherit it   */
				pinned	       :  1, /* must always be on PMU */
				exclusive      :  1, /* only group on PMU     */
				exclude_user   :  1, /* don't count user      */
				exclude_kernel :  1, /* ditto kernel          */
				exclude_hv     :  1, /* ditto hypervisor      */
				exclude_idle   :  1, /* don't count when idle */
				include_tid    :  1, /* include the tid */

				__reserved_1   : 54;

	__u32			extra_config_len;
	__u32			__reserved_4;

	__u64			__reserved_2;
	__u64			__reserved_3;
};

/*
 * Ioctls that can be done on a perf counter fd:
 */
#define PERF_COUNTER_IOC_ENABLE		_IO('$', 0)
#define PERF_COUNTER_IOC_DISABLE	_IO('$', 1)

/*
 * Structure of the page that can be mapped via mmap
 */
struct perf_counter_mmap_page {
	__u32	version;		/* version number of this structure */
	__u32	compat_version;		/* lowest version this is compat with */
	__u32	lock;			/* seqlock for synchronization */
	__u32	index;			/* hardware counter identifier */
	__s64	offset;			/* add to hardware counter value */

	__u32   data_head;		/* head in the data section */
};

struct perf_event_header {
	__u32	type;
	__u32	size;
};

enum perf_event_type {
	PERF_EVENT_IP		= 0,
	PERF_EVENT_GROUP	= 1,

	__PERF_EVENT_TID	= 0x100,
};

/*
 * prctl(PR_TASK_PERF_COUNTERS_DISABLE) will (cheaply) disable all
 * counters in the current task.
 */
#define PR_TASK_PERF_COUNTERS_DISABLE   31
#define PR_TASK_PERF_COUNTERS_ENABLE    32

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define rdclock()                                       \
({                                                      \
        struct timespec ts;                             \
                                                        \
        clock_gettime(CLOCK_MONOTONIC, &ts);            \
        ts.tv_sec * 1000000000ULL + ts.tv_nsec;         \
})

#endif

/*
 * Pick up some kernel type conventions:
 */
typedef __u64 u64;


#define __user
#define asmlinkage

#ifdef __x86_64__
#define __NR_perf_counter_open 298
#define rmb()		asm volatile("lfence" ::: "memory")
#define cpu_relax()	asm volatile("rep; nop" ::: "memory");
#endif

#ifdef __i386__
#define __NR_perf_counter_open 336
#define rmb()		asm volatile("lfence" ::: "memory")
#define cpu_relax()	asm volatile("rep; nop" ::: "memory");
#endif

#ifdef __powerpc__
#define __NR_perf_counter_open 319
#define rmb() 		asm volatile ("sync" ::: "memory")
#define cpu_relax()	asm volatile ("" ::: "memory");
#endif

#define unlikely(x)	__builtin_expect(!!(x), 0)
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

asmlinkage int sys_perf_counter_open(
        struct perf_counter_hw_event    *hw_event_uptr          __user,
        pid_t                           pid,
        int                             cpu,
        int                             group_fd,
        unsigned long                   flags)
{
        return syscall(
                __NR_perf_counter_open, hw_event_uptr, pid, cpu, group_fd, flags);
}

#define MAX_COUNTERS			64
#define MAX_NR_CPUS			256

#define EID(type, id) (((__u64)(type) << PERF_COUNTER_TYPE_SHIFT) | (id))

static int			run_perfstat			=  0;
static int			system_wide			=  0;

static int			nr_counters			=  0;
static __u64			event_id[MAX_COUNTERS]		= {
	EID(PERF_TYPE_SOFTWARE, PERF_COUNT_TASK_CLOCK),
	EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CONTEXT_SWITCHES),
	EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CPU_MIGRATIONS),
	EID(PERF_TYPE_SOFTWARE, PERF_COUNT_PAGE_FAULTS),

	EID(PERF_TYPE_HARDWARE, PERF_COUNT_CPU_CYCLES),
	EID(PERF_TYPE_HARDWARE, PERF_COUNT_INSTRUCTIONS),
	EID(PERF_TYPE_HARDWARE, PERF_COUNT_CACHE_REFERENCES),
	EID(PERF_TYPE_HARDWARE, PERF_COUNT_CACHE_MISSES),
};
static int			default_interval = 100000;
static int			event_count[MAX_COUNTERS];
static int			fd[MAX_NR_CPUS][MAX_COUNTERS];

static __u64			count_filter		       = 100;

static int			tid				= -1;
static int			profile_cpu			= -1;
static int			nr_cpus				=  0;
static int			nmi				=  1;
static int			group				=  0;
static unsigned int		page_size;
static unsigned int		mmap_pages			=  16;

static char			*vmlinux;

static char			*sym_filter;
static unsigned long		filter_start;
static unsigned long		filter_end;

static int			delay_secs			=  2;
static int			zero;
static int			dump_symtab;

struct source_line {
	uint64_t		EIP;
	unsigned long		count;
	char			*line;
	struct source_line	*next;
};

static struct source_line	*lines;
static struct source_line	**lines_tail;

const unsigned int default_count[] = {
	1000000,
	1000000,
	  10000,
	  10000,
	1000000,
	  10000,
};

static const char *hw_event_names[] = {
	"CPU cycles",
	"instructions",
	"cache references",
	"cache misses",
	"branches",
	"branch misses",
	"bus cycles",
};

static const char *sw_event_names[] = {
	"cpu clock ticks",
	"task clock ticks",
	"pagefaults",
	"context switches",
	"CPU migrations",
	"minor faults",
	"major faults",
};

struct event_symbol {
	__u64 event;
	const char *symbol;
};

static struct event_symbol event_symbols[] = {
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_CPU_CYCLES),		"cpu-cycles",		},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_CPU_CYCLES),		"cycles",		},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_INSTRUCTIONS),		"instructions",		},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_CACHE_REFERENCES),		"cache-references",	},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_CACHE_MISSES),		"cache-misses",		},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_BRANCH_INSTRUCTIONS),	"branch-instructions",	},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_BRANCH_INSTRUCTIONS),	"branches",		},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_BRANCH_MISSES),		"branch-misses",	},
	{EID(PERF_TYPE_HARDWARE, PERF_COUNT_BUS_CYCLES),		"bus-cycles",		},

	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CPU_CLOCK),			"cpu-clock",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_TASK_CLOCK),		"task-clock",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_PAGE_FAULTS),		"page-faults",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_PAGE_FAULTS),		"faults",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_PAGE_FAULTS_MIN),		"minor-faults",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_PAGE_FAULTS_MAJ),		"major-faults",		},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CONTEXT_SWITCHES),		"context-switches",	},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CONTEXT_SWITCHES),		"cs",			},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CPU_MIGRATIONS),		"cpu-migrations",	},
	{EID(PERF_TYPE_SOFTWARE, PERF_COUNT_CPU_MIGRATIONS),		"migrations",		},
};

#define __PERF_COUNTER_FIELD(config, name) \
	((config & PERF_COUNTER_##name##_MASK) >> PERF_COUNTER_##name##_SHIFT)

#define PERF_COUNTER_RAW(config)	__PERF_COUNTER_FIELD(config, RAW)
#define PERF_COUNTER_CONFIG(config)	__PERF_COUNTER_FIELD(config, CONFIG)
#define PERF_COUNTER_TYPE(config)	__PERF_COUNTER_FIELD(config, TYPE)
#define PERF_COUNTER_ID(config)		__PERF_COUNTER_FIELD(config, EVENT)


class perf_counter
{
    boost::array<int, 6> fd;
/*     boost::array<int, 6> sw_ct; */

    static int open_performance_counter(int type)
    {
        perf_counter_hw_event hw_event;
        memset(&hw_event, 0, sizeof(hw_event));
        hw_event.config		= event_id[type];
        hw_event.record_type	= PERF_RECORD_SIMPLE;
        hw_event.nmi		= 1;

        int ret = sys_perf_counter_open(&hw_event, 0, -1, -1, 0);

        if (ret < 0)
            throw std::exception();
        return ret;
    }

public:
    perf_counter(void)
    {
        for (int i = 0; i != 6; ++i)
            fd[i] = open_performance_counter(i);

/*         for (int i = 0; i != 2; ++i) */
/*             sw_ct[i] = open_performance_counter(-1 - i); */
    }

    ~perf_counter(void)
    {
        for (int i = 0; i != 6; ++i)
            close(fd[i]);

/*         for (int i = 0; i != 2; ++i) */
/*             close(sw_ct[i]); */
    }

    void reset(void)
    {}

    void start(void)
    {
        prctl(PR_TASK_PERF_COUNTERS_ENABLE);
    }

    void stop(void)
    {
        prctl(PR_TASK_PERF_COUNTERS_DISABLE);
    }

    void dump(bool verbose = true)
    {
        static const char *hw_event_names [] = {
            "cycles",
            "instructions",
            "cache references",
            "cache misses",
            "branches",
            "branch misses"
        };

        static const char *sw_event_names [] = {
            "cpu clock ticks",
            "task clock ticks",
            "pagefaults",
            "context switches",
            "CPU migrations",
        };

        boost::array<u64, 6> count;
        using namespace std;

        for (int i = 0; i != 6; ++i)
        {
            u64 cnt;
            ssize_t res = read(fd[i], (char*)&cnt, sizeof(cnt));
            assert(res == sizeof(cnt));
            count[i] = cnt;
        }

/*         boost::array<u64, 6> soft_count; */
/*         for (int i = 0; i != 2; ++i) */
/*         { */
/*             u64 cnt; */
/*             ssize_t res = read(sw_ct[i], (char*)&cnt, sizeof(cnt)); */
/*             assert(res == sizeof(cnt)); */
/*             soft_count[i] = cnt; */
/*         } */

        if (verbose)
        {
            for (int i = 0; i != 6; ++i)
                cout << hw_event_names[i] << ": " << count[i] << endl;
            cout << endl;
        }
        else
        {
            cout << setw(12) << setfill(' ') << count[0] << " "
                 << setw(12) << setfill(' ') << count[1] << " "
                 << setw(7)  << setfill(' ') << count[2] << " "
                 << setw(5)  << setfill(' ') << count[3] << " "
                 << setw(10) << setfill(' ') << count[4] << " "
                 << setw(8)  << setfill(' ') << count[5] << " "
/*                  << setw(8)  << setfill(' ') << soft_count[0] << " " */
/*                  << setw(8)  << setfill(' ') << soft_count[1] */
                 << endl;
        }
    }
};

#undef unlikely
#endif /* PERF_COUNTER_HPP */
