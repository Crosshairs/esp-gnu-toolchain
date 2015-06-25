/* riscv.h.  RISC-V opcode list for GDB, the GNU debugger.
   Copyright 2011
   Free Software Foundation, Inc.
   Contributed by Andrew Waterman 

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _RISCV_H_
#define _RISCV_H_

#include "riscv-opc.h"
#include <stdlib.h>
#include <stdint.h>

/* RVC fields */

#define OP_MASK_CRS1 0x1f
#define OP_SH_CRS1 2
#define OP_MASK_CRS1S 0x7
#define OP_SH_CRS1S 2
#define OP_MASK_CRDS 0x7
#define OP_SH_CRDS 7

static const char rvc_rs1_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 7 };
#define rvc_rd_regmap rvc_rs1_regmap
#define rvc_rs2b_regmap rvc_rs1_regmap
static const char rvc_rs2_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 0 };

typedef uint64_t insn_t;

static inline unsigned int riscv_insn_length (insn_t insn)
{
  if ((insn & 0x3) != 3) /* RVC */
    return 2;
  if ((insn & 0x1f) != 0x1f) /* base ISA and extensions in 32-bit space */
    return 4;
  if ((insn & 0x3f) == 0x1f) /* 48-bit extensions */
    return 6;
  if ((insn & 0x7f) == 0x3f) /* 64-bit extensions */
    return 8;
  /* longer instructions not supported at the moment */
  return 2;
}

static const char * const riscv_rm[8] = {
  "rne", "rtz", "rdn", "rup", "rmm", 0, 0, "dyn"
};
static const char* const riscv_pred_succ[16] = {
  0,   "w",  "r",  "rw",  "o",  "ow",  "or",  "orw",
  "i", "iw", "ir", "irw", "io", "iow", "ior", "iorw",
};
static const char * const riscv_hwacha_svbits[24] = {
  "s",    "v",    0,      0,      0,      0,      0,      0,
  "ss",   "sv",   "vs",   "vv",   0,      0,      0,      0,
  "sss",  "ssv",  "svs",  "svv",  "vss",  "vsv",  "vvs",  "vvv"
};

#define RVC_JUMP_BITS 11
#define RVC_JUMP_REACH ((1ULL<<RVC_JUMP_BITS)*RISCV_JUMP_ALIGN)

#define RVC_BRANCH_BITS 8
#define RVC_BRANCH_REACH ((1ULL<<RVC_BRANCH_BITS)*RISCV_BRANCH_ALIGN)

#define RV_X(x, s, n) (((x) >> (s)) & (((insn_t)1<<(n))-1))
#define RV_IMM_SIGN(x) (-(((x) >> 31) & 1))

#define EXTRACT_ITYPE_IMM(x) \
  (RV_X(x, 20, 12) | (RV_IMM_SIGN(x) << 12))
#define EXTRACT_STYPE_IMM(x) \
  (RV_X(x, 7, 5) | (RV_X(x, 25, 7) << 5) | (RV_IMM_SIGN(x) << 12))
#define EXTRACT_SBTYPE_IMM(x) \
  ((RV_X(x, 8, 4) << 1) | (RV_X(x, 25, 6) << 5) | (RV_X(x, 7, 1) << 11) | (RV_IMM_SIGN(x) << 12))
#define EXTRACT_UTYPE_IMM(x) \
  ((RV_X(x, 12, 20) << 12) | (RV_IMM_SIGN(x) << 32))
#define EXTRACT_UJTYPE_IMM(x) \
  ((RV_X(x, 21, 10) << 1) | (RV_X(x, 20, 1) << 11) | (RV_X(x, 12, 8) << 12) | (RV_IMM_SIGN(x) << 20))
#define EXTRACT_RVC_IMM(x) \
  (RV_X(x, 2, 5) | (-RV_X(x, 12, 1) << 5))
#define EXTRACT_RVC_LW_IMM(x) \
  ((RV_X(x, 11, 1) << 2) | (RV_X(x, 5, 2) << 3) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 10, 1) << 6))
#define EXTRACT_RVC_LD_IMM(x) \
  ((RV_X(x, 5, 2) << 3) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 10, 2) << 6))
#define EXTRACT_RVC_LWSP_IMM(x) \
  ((RV_X(x, 4, 3) << 2) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 2, 2) << 6))
#define EXTRACT_RVC_LDSP_IMM(x) \
  ((RV_X(x, 5, 2) << 3) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 2, 3) << 6))
#define EXTRACT_RVC_B_IMM(x) \
  ((RV_X(x, 7, 1) << 1) | (RV_X(x, 11, 1) << 2) | (RV_X(x, 5, 2) << 3) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 10, 1) << 6) | (RV_X(x, 8, 1) << 7) | (-RV_X(x, 9, 1) << 8))
#define EXTRACT_RVC_J_IMM(x) \
  ((RV_X(x, 7, 1) << 1) | (RV_X(x, 11, 1) << 2) | (RV_X(x, 5, 2) << 3) | (RV_X(x, 12, 1) << 5) | (RV_X(x, 10, 1) << 6) | (RV_X(x, 8, 2) << 7) | (RV_X(x, 2, 2) << 9) | (-RV_X(x, 4, 1) << 11))

#define ENCODE_ITYPE_IMM(x) \
  (RV_X(x, 0, 12) << 20)
#define ENCODE_STYPE_IMM(x) \
  ((RV_X(x, 0, 5) << 7) | (RV_X(x, 5, 7) << 25))
#define ENCODE_SBTYPE_IMM(x) \
  ((RV_X(x, 1, 4) << 8) | (RV_X(x, 5, 6) << 25) | (RV_X(x, 11, 1) << 7) | (RV_X(x, 12, 1) << 31))
#define ENCODE_UTYPE_IMM(x) \
  (RV_X(x, 12, 20) << 12)
#define ENCODE_UJTYPE_IMM(x) \
  ((RV_X(x, 1, 10) << 21) | (RV_X(x, 11, 1) << 20) | (RV_X(x, 12, 8) << 12) | (RV_X(x, 20, 1) << 31))
#define ENCODE_RVC_IMM(x) \
  ((RV_X(x, 0, 5) << 2) | (RV_X(x, 5, 1) << 12))
#define ENCODE_RVC_LW_IMM(x) \
  ((RV_X(x, 2, 1) << 11) | (RV_X(x, 3, 2) << 5) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 1) << 10))
#define ENCODE_RVC_LD_IMM(x) \
  ((RV_X(x, 3, 2) << 5) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 2) << 10))
#define ENCODE_RVC_LWSP_IMM(x) \
  ((RV_X(x, 2, 3) << 4) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 2) << 2))
#define ENCODE_RVC_LDSP_IMM(x) \
  ((RV_X(x, 3, 2) << 5) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 3) << 2))
#define ENCODE_RVC_B_IMM(x) \
  ((RV_X(x, 1, 1) << 7) | (RV_X(x, 2, 1) << 11) | (RV_X(x, 3, 2) << 5) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 1) << 10) | (RV_X(x, 7, 2) << 8))
#define ENCODE_RVC_J_IMM(x) \
  ((RV_X(x, 1, 1) << 7) | (RV_X(x, 2, 1) << 11) | (RV_X(x, 3, 2) << 5) | (RV_X(x, 5, 1) << 12) | (RV_X(x, 6, 1) << 10) | (RV_X(x, 7, 2) << 8) | (RV_X(x, 9, 3) << 2))
//vector imm encoding
#define ENCODE_ITYPE_VIMM(x) \
  ((insn_t)RV_X(x, 0, 32) << 32)
#define ENCODE_STYPE_VIMM(x) \
  ((RV_X(x, 0, 5) << 12) | ((insn_t)RV_X(x, 5, 7) << 36))

#define VALID_ITYPE_IMM(x) (EXTRACT_ITYPE_IMM(ENCODE_ITYPE_IMM(x)) == (x))
#define VALID_STYPE_IMM(x) (EXTRACT_STYPE_IMM(ENCODE_STYPE_IMM(x)) == (x))
#define VALID_SBTYPE_IMM(x) (EXTRACT_SBTYPE_IMM(ENCODE_SBTYPE_IMM(x)) == (x))
#define VALID_UTYPE_IMM(x) (EXTRACT_UTYPE_IMM(ENCODE_UTYPE_IMM(x)) == (x))
#define VALID_UJTYPE_IMM(x) (EXTRACT_UJTYPE_IMM(ENCODE_UJTYPE_IMM(x)) == (x))
#define VALID_RVC_IMM(x) (EXTRACT_RVC_IMM(ENCODE_RVC_IMM(x)) == (x))
#define VALID_RVC_LW_IMM(x) (EXTRACT_RVC_LW_IMM(ENCODE_RVC_LW_IMM(x)) == (x))
#define VALID_RVC_LD_IMM(x) (EXTRACT_RVC_LD_IMM(ENCODE_RVC_LD_IMM(x)) == (x))
#define VALID_RVC_LWSP_IMM(x) (EXTRACT_RVC_LWSP_IMM(ENCODE_RVC_LWSP_IMM(x)) == (x))
#define VALID_RVC_LDSP_IMM(x) (EXTRACT_RVC_LDSP_IMM(ENCODE_RVC_LDSP_IMM(x)) == (x))
#define VALID_RVC_B_IMM(x) (EXTRACT_RVC_B_IMM(ENCODE_RVC_B_IMM(x)) == (x))
#define VALID_RVC_J_IMM(x) (EXTRACT_RVC_J_IMM(ENCODE_RVC_J_IMM(x)) == (x))

#define RISCV_RTYPE(insn, rd, rs1, rs2) \
  ((MATCH_ ## insn) | ((rd) << OP_SH_RD) | ((rs1) << OP_SH_RS1) | ((rs2) << OP_SH_RS2))
#define RISCV_ITYPE(insn, rd, rs1, imm) \
  ((MATCH_ ## insn) | ((rd) << OP_SH_RD) | ((rs1) << OP_SH_RS1) | ENCODE_ITYPE_IMM(imm))
#define RISCV_STYPE(insn, rs1, rs2, imm) \
  ((MATCH_ ## insn) | ((rs1) << OP_SH_RS1) | ((rs2) << OP_SH_RS2) | ENCODE_STYPE_IMM(imm))
#define RISCV_SBTYPE(insn, rs1, rs2, target) \
  ((MATCH_ ## insn) | ((rs1) << OP_SH_RS1) | ((rs2) << OP_SH_RS2) | ENCODE_SBTYPE_IMM(target))
#define RISCV_UTYPE(insn, rd, bigimm) \
  ((MATCH_ ## insn) | ((rd) << OP_SH_RD) | ENCODE_UTYPE_IMM(bigimm))
#define RISCV_UJTYPE(insn, rd, target) \
  ((MATCH_ ## insn) | ((rd) << OP_SH_RD) | ENCODE_UJTYPE_IMM(target))

#define RISCV_NOP RISCV_ITYPE(ADDI, 0, 0, 0)
#define RVC_NOP MATCH_C_ADDI

#define RISCV_CONST_HIGH_PART(VALUE) \
  (((VALUE) + (RISCV_IMM_REACH/2)) & ~(RISCV_IMM_REACH-1))
#define RISCV_CONST_LOW_PART(VALUE) ((VALUE) - RISCV_CONST_HIGH_PART (VALUE))
#define RISCV_PCREL_HIGH_PART(VALUE, PC) RISCV_CONST_HIGH_PART((VALUE) - (PC))
#define RISCV_PCREL_LOW_PART(VALUE, PC) RISCV_CONST_LOW_PART((VALUE) - (PC))

/* RV fields */

#define OP_MASK_OP		0x7f
#define OP_SH_OP		0
#define OP_MASK_RS2		0x1f
#define OP_SH_RS2		20
#define OP_MASK_RS1		0x1f
#define OP_SH_RS1		15
#define OP_MASK_RS3		0x1f
#define OP_SH_RS3		27
#define OP_MASK_RD		0x1f
#define OP_SH_RD		7
#define OP_MASK_SHAMT		0x3f
#define OP_SH_SHAMT		20
#define OP_MASK_SHAMTW		0x1f
#define OP_SH_SHAMTW		20
#define OP_MASK_RM		0x7
#define OP_SH_RM		12
#define OP_MASK_PRED		0xf
#define OP_SH_PRED		24
#define OP_MASK_SUCC		0xf
#define OP_SH_SUCC		20
#define OP_MASK_AQ		0x1
#define OP_SH_AQ		26
#define OP_MASK_RL		0x1
#define OP_SH_RL		25

/*control thread vector instructions*/
#define OP_MASK_SVARD		0x1f
#define OP_SH_SVARD		7
#define OP_MASK_SVSRDLO		0x1f
#define OP_SH_SVSRDLO		7
#define OP_MASK_SVSRDHI		0x7
#define OP_SH_SVSRDHI		20

/*microthread vector instructions*/
#define OP_MASK_VRD		0xff
#define OP_SH_VRD		16
#define OP_MASK_VRS		0xff
#define OP_SH_VRS		24
#define OP_MASK_VRT		0xff
#define OP_SH_VRT		33
#define OP_MASK_VRR		0xff
#define OP_SH_VRR		41
#define OP_MASK_VPD		0xf
#define OP_SH_VPD		16
#define OP_MASK_VPS		0xf
#define OP_SH_VPS		24
#define OP_MASK_VPT		0xf
#define OP_SH_VPT		33
#define OP_MASK_VPR		0xf
#define OP_SH_VPR		41
#define OP_MASK_VAS		0x1f
#define OP_SH_VAS		24
#define OP_MASK_VAT		0x1f
#define OP_SH_VAT		33
#define OP_MASK_VPRED 0xf
#define OP_SH_VPRED 12
#define OP_MASK_VN 0x1
#define OP_SH_VN 32
#define OP_MASK_VD 0x1
#define OP_SH_VD 63
#define OP_MASK_VS1 0x1
#define OP_SH_VS1 62
#define OP_MASK_VS2 0x1
#define OP_SH_VS2 61
#define OP_MASK_VS3 0x1
#define OP_SH_VS3 60
#define OP_MASK_VCOND 0x3
#define OP_SH_VCOND 33
#define OP_MASK_VPFUNC 0xff
#define OP_SH_VPFUNC 50

#define OP_MASK_VIMM 0xffffffff
#define OP_SH_VIMM 32
#define OP_MASK_VCIMM 0x1fffffff
#define OP_SH_VCIMM 35
#define OP_MASK_VSHAMT 0x3f
#define OP_SH_VSHAMT 32 
#define OP_MASK_VSHAMTW 0x1f 
#define OP_SH_VSHAMTW 32
#define OP_MASK_VAQ 0x1
#define OP_SH_VAQ  54
#define OP_MASK_VRL 0x1 
#define OP_SH_VRL 53 
#define OP_MASK_VRM 0x7 
#define OP_SH_VRM 50 
#define OP_MASK_VPREV		0xf
#define OP_SH_VPREV		20
#define OP_MASK_VSUCC		0xf
#define OP_SH_VSUCC		16

#define OP_MASK_IMMNGPR         0xff
#define OP_SH_IMMNGPR           20
#define OP_MASK_IMMNPPR         0xf
#define OP_SH_IMMNPPR           28
#define OP_MASK_IMMSEGNELM      0x7
#define OP_SH_IMMSEGNELM        45
#define OP_MASK_CUSTOM_IMM      0x7f
#define OP_SH_CUSTOM_IMM        25
#define OP_MASK_CSR             0xfff
#define OP_SH_CSR               20

#define X_RA 1
#define X_SP 2
#define X_GP 3
#define X_TP 4
#define X_T0 5
#define X_T1 6
#define X_T2 7
#define X_T3 28

#define NGPR 32
#define NFPR 32
#define NVGPR 256
#define NVSPR 256
#define NVAPR 32
#define NVPPR 16

#define RISCV_JUMP_BITS RISCV_BIGIMM_BITS
#define RISCV_JUMP_ALIGN_BITS 1
#define RISCV_JUMP_ALIGN (1 << RISCV_JUMP_ALIGN_BITS)
#define RISCV_JUMP_REACH ((1ULL<<RISCV_JUMP_BITS)*RISCV_JUMP_ALIGN)

#define RISCV_IMM_BITS 12
#define RISCV_BIGIMM_BITS (32-RISCV_IMM_BITS)
#define RISCV_IMM_REACH (1LL<<RISCV_IMM_BITS)
#define RISCV_BIGIMM_REACH (1LL<<RISCV_BIGIMM_BITS)
#define RISCV_RVC_IMM_REACH (1LL<<6)
#define RISCV_BRANCH_BITS RISCV_IMM_BITS
#define RISCV_BRANCH_ALIGN_BITS RISCV_JUMP_ALIGN_BITS
#define RISCV_BRANCH_ALIGN (1 << RISCV_BRANCH_ALIGN_BITS)
#define RISCV_BRANCH_REACH (RISCV_IMM_REACH*RISCV_BRANCH_ALIGN)

/* This structure holds information for a particular instruction.  */

struct riscv_opcode
{
  /* The name of the instruction.  */
  const char *name;
  /* The ISA subset name (I, M, A, F, D, Xextension). */
  const char *subset;
  /* A string describing the arguments for this instruction.  */
  const char *args;
  /* The basic opcode for the instruction.  When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is 0.  */
  insn_t match;
  /* If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */
  insn_t mask;
  /* A function to determine if a word corresponds to this instruction.
     Usually, this computes ((word & mask) == match). */
  int (*match_func)(const struct riscv_opcode *op, insn_t word);
  /* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */
  unsigned long pinfo;
};

#define INSN_WRITE_GPR_D            0x00000001
#define INSN_WRITE_GPR_RA           0x00000004
#define INSN_WRITE_FPR_D            0x00000008
#define INSN_READ_GPR_S             0x00000040
#define INSN_READ_GPR_T             0x00000080
#define INSN_READ_FPR_S             0x00000100
#define INSN_READ_FPR_T             0x00000200
#define INSN_READ_FPR_R        	    0x00000400
/* Instruction is a simple alias (I.E. "move" for daddu/addu/or) */
#define	INSN_ALIAS		    0x00001000
/* Instruction is actually a macro.  It should be ignored by the
   disassembler, and requires special treatment by the assembler.  */
#define INSN_MACRO                  0xffffffff

/* This is a list of macro expanded instructions.

   _I appended means immediate
   _A appended means address
   _AB appended means address with base register
   _D appended means 64 bit floating point constant
   _S appended means 32 bit floating point constant.  */

enum
{
  M_LA,
  M_LLA,
  M_LA_TLS_GD,
  M_LA_TLS_IE,
  M_LB,
  M_LBU,
  M_LH,
  M_LHU,
  M_LW,
  M_LWU,
  M_LD,
  M_SB,
  M_SH,
  M_SW,
  M_SD,
  M_FLW,
  M_FLD,
  M_FSW,
  M_FSD,
  M_CALL,
  M_J,
  M_LI,
  M_VF,
  M_NUM_MACROS
};


extern const char * const riscv_gpr_names_numeric[NGPR];
extern const char * const riscv_gpr_names_abi[NGPR];
extern const char * const riscv_fpr_names_numeric[NFPR];
extern const char * const riscv_fpr_names_abi[NFPR];
extern const char * const riscv_vec_gpr_names[NVGPR];
extern const char * const riscv_vec_spr_names[NVSPR];
extern const char * const riscv_vec_apr_names[NVAPR];
extern const char * const riscv_vec_ppr_names[NVPPR];

extern const struct riscv_opcode riscv_builtin_opcodes[];
extern const int bfd_riscv_num_builtin_opcodes;
extern struct riscv_opcode *riscv_opcodes;
extern int bfd_riscv_num_opcodes;
#define NUMOPCODES bfd_riscv_num_opcodes

#endif /* _RISCV_H_ */
