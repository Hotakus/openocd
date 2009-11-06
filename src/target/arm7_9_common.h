/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007,2008 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
 *                                                                         *
 *   Copyright (C) 2008 by Spencer Oliver                                  *
 *   spen@spen-soft.co.uk                                                  *
 *                                                                         *
 *   Copyright (C) 2008 by Hongtao Zheng                                   *
 *   hontor@126.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ARM7_9_COMMON_H
#define ARM7_9_COMMON_H

#include "breakpoints.h"
#include "etm.h"

#define	ARM7_9_COMMON_MAGIC 0x0a790a79 /**< */

/**
 * Structure for items that are common between both ARM7 and ARM9 targets.
 */
typedef struct arm7_9_common_s
{
	uint32_t common_magic;

	arm_jtag_t jtag_info; /**< JTAG information for target */
	reg_cache_t *eice_cache; /**< Embedded ICE register cache */

	uint32_t arm_bkpt; /**< ARM breakpoint instruction */
	uint16_t thumb_bkpt; /**< Thumb breakpoint instruction */
	bool force_hw_bkpts;

	int sw_breakpoints_added; /**< Specifies which watchpoint software breakpoints are setup on */
	int sw_breakpoint_count; /**< keep track of number of software breakpoints we have set */
	int breakpoint_count; /**< Current number of set breakpoints */
	int wp_available; /**< Current number of available watchpoint units */
	int wp_available_max; /**< Maximum number of available watchpoint units */
	int wp0_used; /**< Specifies if and how watchpoint unit 0 is used */
	int wp1_used; /**< Specifies if and how watchpoint unit 1 is used */
	int wp1_used_default; /**< Specifies if and how watchpoint unit 1 is used by default */
	int dbgreq_adjust_pc; /**< Amount of PC adjustment caused by a DBGREQ */
	bool use_dbgrq; /**< Specifies if DBGRQ should be used to halt the target */
	bool need_bypass_before_restart; /**< Specifies if there should be a bypass before a JTAG restart */

	bool has_single_step;
	bool has_monitor_mode;
	bool has_vector_catch; /**< Specifies if the target has a reset vector catch */

	bool debug_entry_from_reset; /**< Specifies if debug entry was from a reset */

	bool fast_memory_access;
	bool dcc_downloads;

	etm_context_t *etm_ctx;

	struct working_area_s *dcc_working_area;

	int (*examine_debug_reason)(target_t *target); /**< Function for determining why debug state was entered */

	void (*change_to_arm)(target_t *target, uint32_t *r0, uint32_t *pc); /**< Function for changing from Thumb to ARM mode */

	void (*read_core_regs)(target_t *target, uint32_t mask, uint32_t *core_regs[16]); /**< Function for reading the core registers */
	void (*read_core_regs_target_buffer)(target_t *target, uint32_t mask, void *buffer, int size);
	void (*read_xpsr)(target_t *target, uint32_t *xpsr, int spsr); /**< Function for reading CPSR or SPSR */

	void (*write_xpsr)(target_t *target, uint32_t xpsr, int spsr); /**< Function for writing to CPSR or SPSR */
	void (*write_xpsr_im8)(target_t *target, uint8_t xpsr_im, int rot, int spsr); /**< Function for writing an immediate value to CPSR or SPSR */
	void (*write_core_regs)(target_t *target, uint32_t mask, uint32_t core_regs[16]);

	void (*load_word_regs)(target_t *target, uint32_t mask);
	void (*load_hword_reg)(target_t *target, int num);
	void (*load_byte_reg)(target_t *target, int num);

	void (*store_word_regs)(target_t *target, uint32_t mask);
	void (*store_hword_reg)(target_t *target, int num);
	void (*store_byte_reg)(target_t *target, int num);

	void (*write_pc)(target_t *target, uint32_t pc); /**< Function for writing to the program counter */
	void (*branch_resume)(target_t *target);
	void (*branch_resume_thumb)(target_t *target);

	void (*enable_single_step)(target_t *target, uint32_t next_pc);
	void (*disable_single_step)(target_t *target);

	void (*set_special_dbgrq)(target_t *target); /**< Function for setting DBGRQ if the normal way won't work */

	void (*post_debug_entry)(target_t *target); /**< Callback function called after entering debug mode */

	void (*pre_restore_context)(target_t *target); /**< Callback function called before restoring the processor context */
	void (*post_restore_context)(target_t *target); /**< Callback function called after restoring the processor context */

	armv4_5_common_t armv4_5_common;
	void *arch_info;

} arm7_9_common_t;

static inline struct arm7_9_common_s *
target_to_arm7_9(struct target_s *target)
{
	return container_of(target->arch_info, struct arm7_9_common_s,
			armv4_5_common);
}

int arm7_9_register_commands(struct command_context_s *cmd_ctx);

int arm7_9_poll(target_t *target);

int arm7_9_target_request_data(target_t *target, uint32_t size, uint8_t *buffer);

int arm7_9_setup(target_t *target);
int arm7_9_assert_reset(target_t *target);
int arm7_9_deassert_reset(target_t *target);
int arm7_9_reset_request_halt(target_t *target);
int arm7_9_early_halt(target_t *target);
int arm7_9_soft_reset_halt(struct target_s *target);
int arm7_9_prepare_reset_halt(struct target_s *target);

int arm7_9_halt(target_t *target);
int arm7_9_full_context(target_t *target);
int arm7_9_restore_context(target_t *target);
int arm7_9_resume(struct target_s *target, int current, uint32_t address, int handle_breakpoints, int debug_execution);
int arm7_9_step(struct target_s *target, int current, uint32_t address, int handle_breakpoints);
int arm7_9_read_core_reg(struct target_s *target, int num, enum armv4_5_mode mode);
int arm7_9_read_memory(struct target_s *target, uint32_t address, uint32_t size, uint32_t count, uint8_t *buffer);
int arm7_9_write_memory(struct target_s *target, uint32_t address, uint32_t size, uint32_t count, uint8_t *buffer);
int arm7_9_bulk_write_memory(target_t *target, uint32_t address, uint32_t count, uint8_t *buffer);
int arm7_9_checksum_memory(struct target_s *target, uint32_t address, uint32_t count, uint32_t* checksum);
int arm7_9_blank_check_memory(struct target_s *target, uint32_t address, uint32_t count, uint32_t* blank);

int arm7_9_run_algorithm(struct target_s *target, int num_mem_params, mem_param_t *mem_params, int num_reg_prams, reg_param_t *reg_param, uint32_t entry_point, void *arch_info);

int arm7_9_add_breakpoint(struct target_s *target, breakpoint_t *breakpoint);
int arm7_9_remove_breakpoint(struct target_s *target, breakpoint_t *breakpoint);
int arm7_9_add_watchpoint(struct target_s *target, watchpoint_t *watchpoint);
int arm7_9_remove_watchpoint(struct target_s *target, watchpoint_t *watchpoint);

void arm7_9_enable_eice_step(target_t *target, uint32_t next_pc);
void arm7_9_disable_eice_step(target_t *target);

int arm7_9_execute_sys_speed(struct target_s *target);

int arm7_9_init_arch_info(target_t *target, arm7_9_common_t *arm7_9);
int arm7_9_get_arch_pointers(target_t *target, armv4_5_common_t **armv4_5_p, arm7_9_common_t **arm7_9_p);

#endif /* ARM7_9_COMMON_H */
