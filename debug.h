// dgen debugger
// (C) 2012, Edd Barrett <vext01@gmail.com>

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __AMIGAOS4__
#ifndef AMIGAOS4_DEBUG_H
#define AMIGAOS4_DEBUG_H
#include <proto/exec.h>
#endif /* AMIGAOS4 */

#include <stdint.h>

#ifdef __AMIGAOS4__
#if defined(DEBUG)
	#define DBUG(x,...) DebugPrintF("[%s:%-4ld] "x ,__FILE__,__LINE__, ##__VA_ARGS__)
	#define IDBUG(x,...) IExec->DebugPrintF("[%s:%-4ld] "x ,__FILE__,__LINE__, ##__VA_ARGS__)
//	#define MDBUG(x,.)  DebugPrintF("[%s] "x, ##__VA_ARGS__)
#else
	#define DBUG(x,...)
	#define IDBUG(x,...)
#endif
#endif
#endif /* AMIGAOS4 */

/** Maximum number of beakpoints supported. */
#define MAX_BREAKPOINTS			64
/** Maximum number of watchpoints supported. */
#define MAX_WATCHPOINTS			64
/** Maximum number of tokens on the debugger command line. */
#define MAX_DEBUG_TOKS			8
/** Default number of instructions to disassemble. */
#define DEBUG_DFLT_DASM_LEN		16
/** Default number of bytes to display while dumping memory. */
#define DEBUG_DFLT_MEMDUMP_LEN		128

#define DBG_CONTEXT_M68K		0
#define DBG_CONTEXT_Z80			1
#define DBG_CONTEXT_YM2612		2
#define DBG_CONTEXT_SN76489		3

/** Breakpoint structure. */
struct dgen_bp {
	uint32_t	addr; /**< Address to break on. */
#define BP_FLAG_USED		(1<<0) /**< Breakpoint enabled. */
#define BP_FLAG_FIRED		(1<<1) /**< Set when breakpoint fires. */
	uint32_t	flags; /**< Flags for breakpoint, see BP_FLAG*. */
};

/** Watchpoint structure. */
struct dgen_wp {
	uint32_t	 start_addr; /**< Address to start watching from. */
	uint32_t	 end_addr;   /**< Address to stop watching to. */
#define WP_FLAG_USED		(1<<0) /**< Watchpoint enabled. */
#define WP_FLAG_FIRED		(1<<1) /**< Set when watchpoint fires. */
	uint32_t	 flags;     /**< Flags for watchpoint, see WP_FLAG*. */
	unsigned char	*bytes;
};

extern "C" void		debug_show_ym2612_regs(void); // fm.c

#endif
