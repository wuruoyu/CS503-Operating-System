/* xinu.h - include all system header files */

#include <fix16.h>
#include <limits.h>
#include <kernel.h>
#include <conf.h>
#include <process.h>
#include <gprocess.h>
#include <queue.h>
#include <resched.h>
#include <mark.h>
#include <semaphore.h>
#include <memory.h>
#include <bufpool.h>
#include <hpet.h>
#include <clock.h>
#include <ports.h>
#include <uart.h>
#include <tty.h>
#include <device.h>
#include <interrupt.h>
#include <apic.h>
#include <file.h>
#include <rfilesys.h>
#include <rdisksys.h>
#include <lfilesys.h>
#include <ether.h>
#include <net.h>
#include <ip.h>
#include <arp.h>
#include <udp.h>
#include <dhcp.h>
#include <icmp.h>
#include <tftp.h>
#include <name.h>
#include <shell.h>
#include <date.h>
#include <prototypes.h>
#include <delay.h>
#include <pci.h>
#include <quark_eth.h>
#include <quark_pdat.h>
#include <quark_irq.h>
#include <multiboot.h>
#include <stdio.h>
#include <string.h>

#define XTEST 0
#define XDEBUG 0 /* set this to 0 when submitting */
#define XTESTCASE 0 /* set this to 0 when submitting */

/* For grading */
#if XTEST
#define XTEST_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XTEST_KPRINTF(...)
#endif
/* For debugging */
#if XDEBUG
#define XDEBUG_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XDEBUG_KPRINTF(...)
#endif
/* For testcases */
#if XTESTCASE
#define XTESTCASE_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XTESTCASE_KPRINTF(...)
#endif
