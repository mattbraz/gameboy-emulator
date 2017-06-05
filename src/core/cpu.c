#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "gameboy.h"
#include "cpu.h"

// Mnemonic, Bytes, Function
struct op_def opcl[256] = {
	// 0x00
	{"NOP",			1,	NOP},
	{"LD BC,d16",	3,	LD_BC_d16},
	{"LD (BC),A",	1,	LD_BCa_A},
	{"INC BC",		1,	INC_BC},
	{"INC B",		1,	INC_B},
	{"DEC B",		1,	DEC_B},
	{"LD B,d8",		2,	LD_B_d8},
	{"RLCA",		1,  RLCA},
	{"LD (a16),SP",	3,	LD_a16_SP},
	{"ADD HL,BC",	1,	ADD_HL_BC},
	{"LD A,(BC)",	1,	LD_A_BCa},
	{"DEC BC",		1,	DEC_BC},
	{"INC C",		1,	INC_C},
	{"DEC C",		1,	DEC_C},
	{"LD C,d8",		2,	LD_C_d8},
	{"RRCA",		1,  RRCA},
	// 0x10
	{"STOP 0",		2,  STOP},
	{"LD DE,d16",	3,	LD_DE_d16},
	{"LD (DE),A",	1,	LD_DEa_A},
	{"INC DE",		1,	INC_DE},
	{"INC D",		1,	INC_D},
	{"DEC D",		1,	DEC_D},
	{"LD D,d8",		2,	LD_D_d8},
	{"RLA",			1,  RLA},
	{"JR r8",		2,	JR_r8},
	{"ADD HL,DE",	1,	ADD_HL_DE},
	{"LD A,(DE)",	1,	LD_A_DEa},
	{"DEC DE",		1,	DEC_DE},
	{"INC E",		1,	INC_E},
	{"DEC E",		1,	DEC_E},
	{"LD E,d8",		2,	LD_E_d8},
	{"RRA",			1,  RRA},
	// 0x20
	{"JR NZ,r8",	2,	JR_NZ_r8},
	{"LD HL,d16",	3,	LD_HL_d16},
	{"LD (HL+),A",	1,	LD_I_HLa_A},
	{"INC HL",		1,	INC_HL},
	{"INC H",		1,	INC_H},
	{"DEC H",		1,	DEC_H},
	{"LD H,d8",		2,	LD_H_d8},
	{"DAA",			1,  DAA},
	{"JR Z,r8",		2,	JR_Z_r8},
	{"ADD HL,HL",	1,	ADD_HL_HL},
	{"LD A,(HL+)",	1,	LD_I_A_HLa},
	{"DEC HL",		1,	DEC_HL},
	{"INC L",		1,	INC_L},
	{"DEC L",		1,	DEC_L},
	{"LD L,d8",		2,	LD_L_d8},
	{"CPL",			1,  CPL},
	// 0x30
	{"JR NC,r8",	2,	JR_NC_r8},
	{"LD SP,d16",	3,	LD_SP_d16},
	{"LD (HL-),A",	1,	LD_D_HLa_A},
	{"INC SP",		1,	INC_SP},
	{"INC (HL)",	1,	INC_HLa},
	{"DEC (HL)",	1,	DEC_HLa},
	{"LD (HL),d8",	2,	LD_HLa_d8},
	{"SCF",			1,  SCF},
	{"JR C,r8",		2,	JR_C_r8},
	{"ADD HL,SP",	1,	ADD_HL_SP},
	{"LD A,(HL-)",	1,	LD_D_A_HLa},
	{"DEC SP",		1,	DEC_SP},
	{"INC A",		1,	INC_A},
	{"DEC A",		1,	DEC_A},
	{"LD A,d8",		2,	LD_A_d8},
	{"CCF",			1,  CCF},
	// 0x40
	{"LD B,B",		1,	LD_B_B},
	{"LD B,C",		1,	LD_B_C},
	{"LD B,D",		1,	LD_B_D},
	{"LD B,E",		1,	LD_B_E},
	{"LD B,H",		1,	LD_B_H},
	{"LD B,L",		1,	LD_B_L},
	{"LD B,(HL)",	1,	LD_B_HLa},
	{"LD B,A",		1,	LD_B_A},
	{"LD C,B",		1,	LD_C_B},
	{"LD C,C",		1,	LD_C_C},
	{"LD C,D",		1,	LD_C_D},
	{"LD C,E",		1,	LD_C_E},
	{"LD C,H",		1,	LD_C_H},
	{"LD C,L",		1,	LD_C_L},
	{"LD C,(HL)",	1,	LD_C_HLa},
	{"LD C,A",		1,	LD_C_A},
	// 0x50
	{"LD D,B",		1,	LD_D_B},
	{"LD D,C",		1,	LD_D_C},
	{"LD D,D",		1,	LD_D_D},
	{"LD D,E",		1,	LD_D_E},
	{"LD D,H",		1,	LD_D_H},
	{"LD D,L",		1,	LD_D_L},
	{"LD D,(HL)",	1,	LD_D_HLa},
	{"LD D,A",		1,	LD_D_A},
	{"LD E,B",		1,	LD_E_B},
	{"LD E,C",		1,	LD_E_C},
	{"LD E,D",		1,	LD_E_D},
	{"LD E,E",		1,	LD_E_E},
	{"LD E,H",		1,	LD_E_H},
	{"LD E,L",		1,	LD_E_L},
	{"LD E,(HL)",	1,	LD_E_HLa},
	{"LD E,A",		1,	LD_E_A},
	// 0x60
	{"LD H,B",		1,	LD_H_B},
	{"LD H,C",		1,	LD_H_C},
	{"LD H,D",		1,	LD_H_D},
	{"LD H,E",		1,	LD_H_E},
	{"LD H,H",		1,	LD_H_H},
	{"LD H,L",		1,	LD_H_L},
	{"LD H,(HL)",	1,	LD_H_HLa},
	{"LD H,A",		1,	LD_H_A},
	{"LD L,B",		1,	LD_L_B},
	{"LD L,C",		1,	LD_L_C},
	{"LD L,D",		1,	LD_L_D},
	{"LD L,E",		1,	LD_L_E},
	{"LD L,H",		1,	LD_L_H},
	{"LD L,L",		1,	LD_L_L},
	{"LD L,(HL)",	1,	LD_L_HLa},
	{"LD L,A",		1,	LD_L_A},
	// 0x70
	{"LD (HL),B",	1,	LD_HLa_B},
	{"LD (HL),C",	1,	LD_HLa_C},
	{"LD (HL),D",	1,	LD_HLa_D},
	{"LD (HL),E",	1,	LD_HLa_E},
	{"LD (HL),H",	1,	LD_HLa_H},
	{"LD (HL),L",	1,	LD_HLa_L},
	{"HALT",		1,  HALT},
	{"LD (HL),A",	1,	LD_HLa_A},
	{"LD A,B",		1,	LD_A_B},
	{"LD A,C",		1,	LD_A_C},
	{"LD A,D",		1,	LD_A_D},
	{"LD A,E",		1,	LD_A_E},
	{"LD A,H",		1,	LD_A_H},
	{"LD A,L",		1,	LD_A_L},
	{"LD A,(HL)",	1,	LD_A_HLa},
	{"LD A,A",		1,	LD_A_A},
	// 0x80
	{"ADD A,B",		1,	ADD_A_B},
	{"ADD A,C",		1,	ADD_A_C},
	{"ADD A,D",		1,	ADD_A_D},
	{"ADD A,E",		1,	ADD_A_E},
	{"ADD A,H",		1,	ADD_A_H},
	{"ADD A,L",		1,	ADD_A_L},
	{"ADD A,(HL)",	1,	ADD_A_HLa},
	{"ADD A,A",		1,	ADD_A_A},
	{"ADC A,B",		1,	ADC_A_B},
	{"ADC A,C",		1,	ADC_A_C},
	{"ADC A,D",		1,	ADC_A_D},
	{"ADC A,E",		1,	ADC_A_E},
	{"ADC A,H",		1,	ADC_A_H},
	{"ADC A,L",		1,	ADC_A_L},
	{"ADC A,(HL)",	1,	ADC_A_HLa},
	{"ADC A,A",		1,	ADC_A_A},
	// 0x90
	{"SUB A,B",		1,	SUB_A_B},
	{"SUB A,C",		1,	SUB_A_C	},
	{"SUB A,D",		1,	SUB_A_D},
	{"SUB A,E",		1,	SUB_A_E},
	{"SUB A,H",		1,	SUB_A_H},
	{"SUB A,L",		1,	SUB_A_L},
	{"SUB A,(HL)",	1,	SUB_A_HLa},
	{"SUB A,A",		1,	SUB_A_A},
	{"SBC A,B",		1,	SBC_A_B},
	{"SBC A,C",		1,	SBC_A_C},
	{"SBC A,D",		1,	SBC_A_D},
	{"SBC A,E",		1,	SBC_A_E},
	{"SBC A,H",		1,	SBC_A_H},
	{"SBC A,L",		1,	SBC_A_L},
	{"SBC A,(HL)",	1,	SBC_A_HLa},
	{"SBC A,A",		1,	SBC_A_A},
	// 0xA0
	{"AND B",		1,	AND_B},
	{"AND C",		1,	AND_C},
	{"AND D",		1,	AND_D},
	{"AND E",		1,	AND_E},
	{"AND H",		1,	AND_H},
	{"AND L",		1,	AND_L},
	{"AND (HL)",	1,	AND_HLa},
	{"AND A",		1,	AND_A},
	{"XOR B",		1,	XOR_B},
	{"XOR C",		1,	XOR_C},
	{"XOR D",		1,	XOR_D},
	{"XOR E",		1,	XOR_E},
	{"XOR H",		1,	XOR_H},
	{"XOR L",		1,	XOR_L},
	{"XOR (HL)",	1,	XOR_HLa},
	{"XOR A",		1,	XOR_A},
	// 0xB0
	{"OR B",		1,	OR_B},
	{"OR C",		1,	OR_C},
	{"OR D",		1,	OR_D},
	{"OR E",		1,	OR_E},
	{"OR H",		1,	OR_H},
	{"OR L",		1,	OR_L},
	{"OR (HL)",		1,	OR_HLa},
	{"OR A",		1,	OR_A},
	{"CP B",		1,	CP_B},
	{"CP C",		1,	CP_C},
	{"CP D",		1,	CP_D},
	{"CP E",		1,	CP_E},
	{"CP H",		1,	CP_H},
	{"CP L",		1,	CP_L},
	{"CP (HL)",		1,	CP_HLa},
	{"CP A",		1,	CP_A},
	// 0xC0
	{"RET NZ",		1,	RET_NZ},
	{"POP BC",		1,	POP_BC},
	{"JP NZ,a16",	3,	JP_NZ_a16},
	{"JP a16",		3,	JP_a16},
	{"CALL NZ,a16",	3,	CALL_NZ_a16},
	{"PUSH BC",		1,	PUSH_BC},
	{"ADD A,d8",	2,	ADD_A_d8},
	{"RST 00H",		1,  RST_00H},
	{"RET Z",		1,	RET_Z},
	{"RET",			1,	RET},
	{"JP Z,a16",	3,	JP_Z_a16},
	{"PREFIX CB",	1,  NOP},
	{"CALL Z,a16",	3,	CALL_Z_a16},
	{"CALL a16",	3,	CALL_a16},
	{"ADC A,d8",	2,	ADC_A_d8},
	{"RST 08H",		1,  RST_08H},
	// 0xD0
	{"RET NC",		1,	RET_NC},
	{"POP DE",		1,	POP_DE},
	{"JP NC,a16",	3,	JP_NC_a16},
	{"",			0,	ILLEGAL_OP},
	{"CALL NC,a16",	3,	CALL_NC_a16},
	{"PUSH DE",		1,	PUSH_DE},
	{"SUB d8",		2,	SUB_A_d8},
	{"RST 10H",		1,  RST_10H},
	{"RET C",		1,	RET_C},
	{"RETI",		1,  RETI},
	{"JP C,a16",	3,	JP_C_a16},
	{"",			0,	ILLEGAL_OP},
	{"CALL C,a16",	3,	CALL_C_a16},
	{"",			0,	ILLEGAL_OP},
	{"SBC A,d8",	2,	SBC_A_d8},
	{"RST 18H",		1,  RST_18H},
	// 0xE0
	{"LDH (a8),A",	2,	LDH_a8_A},
	{"POP HL",		1,	POP_HL},
	{"LDH (C),A",	1,	LDH_Ca_A},
	{"",			0,	ILLEGAL_OP},
	{"",			0,	ILLEGAL_OP},
	{"PUSH HL",		1,	PUSH_HL},
	{"AND d8",		2,	AND_A_d8},
	{"RST 20H",		1,  RST_20H},
	{"ADD SP,r8",	2,	ADD_SP_r8},
	{"JP (HL)",		1,	JP_HL},
	{"LD (a16),A",	3,	LD_a16_A},
	{"",			0,	ILLEGAL_OP},
	{"",			0,	ILLEGAL_OP},
	{"",			0,	ILLEGAL_OP},
	{"XOR d8",		2,	XOR_A_d8},
	{"RST 28H",		1,  RST_28H},
	// 0xF0
	{"LDH A,(a8)",	2,	LDH_A_a8},
	{"POP AF",		1,	POP_AF},
	{"LDH A,(C)",	1,	LDH_A_Ca},
	{"DI",			1,	DI},
	{"",			0,	ILLEGAL_OP},
	{"PUSH AF",		1,	PUSH_AF},
	{"OR d8",		2,	OR_A_d8},
	{"RST 30H",		1,  RST_30H},
	{"LD HL,SP+r8",	2,	LDHL_SP_r8},
	{"LD SP,HL",	1,	LD_SP_HL},
	{"LD A,(a16)",	3,	LD_A_a16},
	{"EI",			1,	EI},
	{"",			0,	ILLEGAL_OP},
	{"",			0,	ILLEGAL_OP},
	{"CP d8",		2,	CP_A_d8},
	{"RST 38H",		1,  RST_38H}
};

struct op_def opcl_ext[256] = {
    // 0x00
    {"RLC B",			2,	RLC_B},
    {"RLC C",			2,	RLC_C},
    {"RLC D",			2,	RLC_D},
    {"RLC E",			2,	RLC_E},
    {"RLC H",			2,	RLC_H},
    {"RLC L",			2,	RLC_L},
    {"RLC (HL)",		2,	RLC_HLa},
    {"RLC A",			2,	RLC_A},
    {"RRC B",			2,	RRC_B},
    {"RRC C",			2,	RRC_C},
    {"RRC D",			2,	RRC_D},
    {"RRC E",			2,	RRC_E},
    {"RRC H",			2,	RRC_H},
    {"RRC L",			2,	RRC_L},
    {"RRC (HL)",		2,	RRC_HLa},
    {"RRC A",			2,	RRC_A},
    // 0x10
    {"RL B",			2,	RL_B},
    {"RL C",			2,	RL_C},
    {"RL D",			2,	RL_D},
    {"RL E",			2,	RL_E},
    {"RL H",			2,	RL_H},
    {"RL L",			2,	RL_L},
    {"RL (HL)",         2,	RL_HLa},
    {"RL A",			2,	RL_A},
    {"RR B",			2,	RR_B},
    {"RR C",			2,	RR_C},
    {"RR D",			2,	RR_D},
    {"RR E",			2,	RR_E},
    {"RR H",			2,	RR_H},
    {"RR L",			2,	RR_L},
    {"RR (HL)",         2,	RR_HLa},
    {"RR A",			2,	RR_A},
    // 0x20
    {"SLA B",			2,	SLA_B},
    {"SLA C",			2,	SLA_C},
    {"SLA D",			2,	SLA_D},
    {"SLA E",			2,	SLA_E},
    {"SLA H",			2,	SLA_H},
    {"SLA L",			2,	SLA_L},
    {"SLA (HL)",		2,	SLA_HLa},
    {"SLA A",			2,	SLA_A},
    {"SRA B",			2,	SRA_B},
    {"SRA C",			2,	SRA_C},
    {"SRA D",			2,	SRA_D},
    {"SRA E",			2,	SRA_E},
    {"SRA H",			2,	SRA_H},
    {"SRA L",			2,	SRA_L},
    {"SRA (HL)",		2,	SRA_HLa},
    {"SRA A",			2,	SRA_A},
    // 0x30
    {"SWAP B",          2,	SWAP_B},
    {"SWAP C",          2,	SWAP_C},
    {"SWAP D",          2,	SWAP_D},
    {"SWAP E",          2,	SWAP_E},
    {"SWAP H",          2,	SWAP_H},
    {"SWAP L",          2,	SWAP_L},
    {"SWAP (HL)",       2,	SWAP_HLa},
    {"SWAP A",          2,	SWAP_A},
    {"SRL B",           2,	SRL_B},
    {"SRL C",           2,	SRL_C},
    {"SRL D",           2,	SRL_D},
    {"SRL E",           2,	SRL_E},
    {"SRL H",           2,	SRL_H},
    {"SRL L",           2,	SRL_L},
    {"SRL (HL)",        2,	SRL_HLa},
    {"SRL A",           2,	SRL_A},
    // 0x40
    {"BIT 0,B",			2,	BIT_0_B},
    {"BIT 0,C",			2,	BIT_0_C},
    {"BIT 0,D",			2,	BIT_0_D},
    {"BIT 0,E",			2,	BIT_0_E},
    {"BIT 0,H",			2,	BIT_0_H},
    {"BIT 0,L",			2,	BIT_0_L},
    {"BIT 0,(HL)",		2,	BIT_0_HLa},
    {"BIT 0,A",			2,	BIT_0_A},
    {"BIT 1,B",			2,	BIT_1_B},
    {"BIT 1,C",			2,	BIT_1_C},
    {"BIT 1,D",			2,	BIT_1_D},
    {"BIT 1,E",			2,	BIT_1_E},
    {"BIT 1,H",			2,	BIT_1_H},
    {"BIT 1,L",			2,	BIT_1_L},
    {"BIT 1,(HL)",		2,	BIT_1_HLa},
    {"BIT 1,A",			2,	BIT_1_A},
    // 0x50
    {"BIT 2,B",			2,	BIT_2_B},
    {"BIT 2,C",			2,	BIT_2_C},
    {"BIT 2,D",			2,	BIT_2_D},
    {"BIT 2,E",			2,	BIT_2_E},
    {"BIT 2,H",			2,	BIT_2_H},
    {"BIT 2,L",			2,	BIT_2_L},
    {"BIT 2,(HL)",		2,	BIT_2_HLa},
    {"BIT 2,A",			2,	BIT_2_A},
    {"BIT 3,B",			2,	BIT_3_B},
    {"BIT 3,C",			2,	BIT_3_C},
    {"BIT 3,D",			2,	BIT_3_D},
    {"BIT 3,E",			2,	BIT_3_E},
    {"BIT 3,H",			2,	BIT_3_H},
    {"BIT 3,L",			2,	BIT_3_L},
    {"BIT 3,(HL)",		2,	BIT_3_HLa},
    {"BIT 3,A",			2,	BIT_3_A},
    // 0x60
    {"BIT 4,B",			2,	BIT_4_B},
    {"BIT 4,C",			2,	BIT_4_C},
    {"BIT 4,D",			2,	BIT_4_D},
    {"BIT 4,E",			2,	BIT_4_E},
    {"BIT 4,H",			2,	BIT_4_H},
    {"BIT 4,L",			2,	BIT_4_L},
    {"BIT 4,(HL)",		2,	BIT_4_HLa},
    {"BIT 4,A",			2,	BIT_4_A},
    {"BIT 5,B",			2,	BIT_5_B},
    {"BIT 5,C",			2,	BIT_5_C},
    {"BIT 5,D",			2,	BIT_5_D},
    {"BIT 5,E",			2,	BIT_5_E},
    {"BIT 5,H",			2,	BIT_5_H},
    {"BIT 5,L",			2,	BIT_5_L},
    {"BIT 5,(HL)",		2,	BIT_5_HLa},
    {"BIT 5,A",			2,	BIT_5_A},
    // 0x70
    {"BIT 6,B",			2,	BIT_6_B},
    {"BIT 6,C",			2,	BIT_6_C},
    {"BIT 6,D",			2,	BIT_6_D},
    {"BIT 6,E",			2,	BIT_6_E},
    {"BIT 6,H",			2,	BIT_6_H},
    {"BIT 6,L",			2,	BIT_6_L},
    {"BIT 6,(HL)",		2,	BIT_6_HLa},
    {"BIT 6,A",			2,	BIT_6_A},
    {"BIT 7,B",			2,	BIT_7_B},
    {"BIT 7,C",			2,	BIT_7_C},
    {"BIT 7,D",			2,	BIT_7_D},
    {"BIT 7,E",			2,	BIT_7_E},
    {"BIT 7,H",			2,	BIT_7_H},
    {"BIT 7,L",			2,	BIT_7_L},
    {"BIT 7,(HL)",		2,	BIT_7_HLa},
    {"BIT 7,A",			2,	BIT_7_A},
    // 0x80
    {"RES 0,B",			2,	RES_0_B},
    {"RES 0,C",			2,	RES_0_C},
    {"RES 0,D",			2,	RES_0_D},
    {"RES 0,E",			2,	RES_0_E},
    {"RES 0,H",			2,	RES_0_H},
    {"RES 0,L",			2,	RES_0_L},
    {"RES 0,(HL)",		2,	RES_0_HLa},
    {"RES 0,A",			2,	RES_0_A},
    {"RES 1,B",			2,	RES_1_B},
    {"RES 1,C",			2,	RES_1_C},
    {"RES 1,D",			2,	RES_1_D},
    {"RES 1,E",			2,	RES_1_E},
    {"RES 1,H",			2,	RES_1_H},
    {"RES 1,L",			2,	RES_1_L},
    {"RES 1,(HL)",		2,	RES_1_HLa},
    {"RES 1,A",			2,	RES_1_A},
    // 0x90
    {"RES 2,B",			2,	RES_2_B},
    {"RES 2,C",			2,	RES_2_C},
    {"RES 2,D",			2,	RES_2_D},
    {"RES 2,E",			2,	RES_2_E},
    {"RES 2,H",			2,	RES_2_H},
    {"RES 2,L",			2,	RES_2_L},
    {"RES 2,(HL)",		2,	RES_2_HLa},
    {"RES 2,A",			2,	RES_2_A},
    {"RES 3,B",			2,	RES_3_B},
    {"RES 3,C",			2,	RES_3_C},
    {"RES 3,D",			2,	RES_3_D},
    {"RES 3,E",			2,	RES_3_E},
    {"RES 3,H",			2,	RES_3_H},
    {"RES 3,L",			2,	RES_3_L},
    {"RES 3,(HL)",		2,	RES_3_HLa},
    {"RES 3,A",			2,	RES_3_A},
    // 0xA0
    {"RES 4,B",			2,	RES_4_B},
    {"RES 4,C",			2,	RES_4_C},
    {"RES 4,D",			2,	RES_4_D},
    {"RES 4,E",			2,	RES_4_E},
    {"RES 4,H",			2,	RES_4_H},
    {"RES 4,L",			2,	RES_4_L},
    {"RES 4,(HL)",		2,	RES_4_HLa},
    {"RES 4,A",			2,	RES_4_A},
    {"RES 5,B",			2,	RES_5_B},
    {"RES 5,C",			2,	RES_5_C},
    {"RES 5,D",			2,	RES_5_D},
    {"RES 5,E",			2,	RES_5_E},
    {"RES 5,H",			2,	RES_5_H},
    {"RES 5,L",			2,	RES_5_L},
    {"RES 5,(HL)",		2,	RES_5_HLa},
    {"RES 5,A",			2,	RES_5_A},
    // 0xB0
    {"RES 6,B",			2,	RES_6_B},
    {"RES 6,C",			2,	RES_6_C},
    {"RES 6,D",			2,	RES_6_D},
    {"RES 6,E",			2,	RES_6_E},
    {"RES 6,H",			2,	RES_6_H},
    {"RES 6,L",			2,	RES_6_L},
    {"RES 6,(HL)",		2,	RES_6_HLa},
    {"RES 6,A",			2,	RES_6_A},
    {"RES 7,B",			2,	RES_7_B},
    {"RES 7,C",			2,	RES_7_C},
    {"RES 7,D",			2,	RES_7_D},
    {"RES 7,E",			2,	RES_7_E},
    {"RES 7,H",			2,	RES_7_H},
    {"RES 7,L",			2,	RES_7_L},
    {"RES 7,(HL)",		2,	RES_7_HLa},
    {"RES 7,A",			2,	RES_7_A},
    // 0xC0
    {"SET 0,B",			2,	SET_0_B},
    {"SET 0,C",			2,	SET_0_C},
    {"SET 0,D",			2,	SET_0_D},
    {"SET 0,E",			2,	SET_0_E},
    {"SET 0,H",			2,	SET_0_H},
    {"SET 0,L",			2,	SET_0_L},
    {"SET 0,(HL)",		2,	SET_0_HLa},
    {"SET 0,A",			2,	SET_0_A},
    {"SET 1,B",			2,	SET_1_B},
    {"SET 1,C",			2,	SET_1_C},
    {"SET 1,D",			2,	SET_1_D},
    {"SET 1,E",			2,	SET_1_E},
    {"SET 1,H",			2,	SET_1_H},
    {"SET 1,L",			2,	SET_1_L},
    {"SET 1,(HL)",		2,	SET_1_HLa},
    {"SET 1,A",			2,	SET_1_A},
    // 0xD0
    {"SET 2,B",			2,	SET_2_B},
    {"SET 2,C",			2,	SET_2_C},
    {"SET 2,D",			2,	SET_2_D},
    {"SET 2,E",			2,	SET_2_E},
    {"SET 2,H",			2,	SET_2_H},
    {"SET 2,L",			2,	SET_2_L},
    {"SET 2,(HL)",		2,	SET_2_HLa},
    {"SET 2,A",			2,	SET_2_A},
    {"SET 3,B",			2,	SET_3_B},
    {"SET 3,C",			2,	SET_3_C},
    {"SET 3,D",			2,	SET_3_D},
    {"SET 3,E",			2,	SET_3_E},
    {"SET 3,H",			2,	SET_3_H},
    {"SET 3,L",			2,	SET_3_L},
    {"SET 3,(HL)",		2,	SET_3_HLa},
    {"SET 3,A",			2,	SET_3_A},
    // 0xE0
    {"SET 4,B",			2,	SET_4_B},
    {"SET 4,C",			2,	SET_4_C},
    {"SET 4,D",			2,	SET_4_D},
    {"SET 4,E",			2,	SET_4_E},
    {"SET 4,H",			2,	SET_4_H},
    {"SET 4,L",			2,	SET_4_L},
    {"SET 4,(HL)",		2,	SET_4_HLa},
    {"SET 4,A",			2,	SET_4_A},
    {"SET 5,B",			2,	SET_5_B},
    {"SET 5,C",			2,	SET_5_C},
    {"SET 5,D",			2,	SET_5_D},
    {"SET 5,E",			2,	SET_5_E},
    {"SET 5,H",			2,	SET_5_H},
    {"SET 5,L",			2,	SET_5_L},
    {"SET 5,(HL)",		2,	SET_5_HLa},
    {"SET 5,A",			2,	SET_5_A},
    // 0xF0
    {"SET 6,B",			2,	SET_6_B},
    {"SET 6,C",			2,	SET_6_C},
    {"SET 6,D",			2,	SET_6_D},
    {"SET 6,E",			2,	SET_6_E},
    {"SET 6,H",			2,	SET_6_H},
    {"SET 6,L",			2,	SET_6_L},
    {"SET 6,(HL)",		2,	SET_6_HLa},
    {"SET 6,A",			2,	SET_6_A},
    {"SET 7,B",			2,	SET_7_B},
    {"SET 7,C",			2,	SET_7_C},
    {"SET 7,D",			2,	SET_7_D},
    {"SET 7,E",			2,	SET_7_E},
    {"SET 7,H",			2,	SET_7_H},
    {"SET 7,L",			2,	SET_7_L},
    {"SET 7,(HL)",		2,	SET_7_HLa},
    {"SET 7,A",			2,	SET_7_A},
};


struct op *cpu_get_op(struct gameboy *gb, struct op *op) {
    uint8_t opcd;
    opcd = mem_read_u8(gb, gb->cpu.PC);
    if (opcd == 0xCB) {
        opcd = mem_read_u8(gb, gb->cpu.PC+1);
        op->op_def = &opcl_ext[opcd];
        op->opcode = (0xCB << 8) | opcd;
        // Extended op codes have no operands
    } else {
        op->op_def = &opcl[opcd];
        op->opcode = (0x00 << 8) | opcd;
        // Operands
        if (op->op_def->bytes == 2) {
            op->immediate.byte = mem_read_u8(gb, gb->cpu.PC+1);
        } else if (op->op_def->bytes == 3) {
            op->immediate.word = mem_read_u16(gb, gb->cpu.PC+1);
        }
    }
    
    return op;
}

void cpu_exec_op(struct gameboy *gb, struct op *op) {
    gb->cpu.PC = gb->cpu.PC + op->op_def->bytes;
    
    (*(op->op_def->op_func_ptr))(gb, op);
}

void cpu_stack_push(struct gameboy *gb, uint16_t val) {
    mem_write_u8(gb, gb->cpu.SP-1, (val >> 8) & 0xFF);
    mem_write_u8(gb, gb->cpu.SP-2, (val >> 0) & 0xFF);
    gb->cpu.SP -= 2;
}

uint16_t cpu_stack_pop(struct gameboy *gb) {
    uint16_t val = 0;
    val |= mem_read_u8(gb, gb->cpu.SP+0) << 0;
    val |= mem_read_u8(gb, gb->cpu.SP+1) << 8;
    gb->cpu.SP += 2;
    return val;
}

//---------------------------------------------

void ILLEGAL_OP(struct gameboy *gb, struct op *op) {
	printf("ILLEGAL OP %04X\n", op->opcode);
	exit(1);
}

void DAA(struct gameboy *gb, struct op *op) {
    int16_t A = gb->cpu.A;
    
    if(gb->cpu.flags.N) {
        /* SUB */
        if (gb->cpu.flags.H)
            A = (A - 0x06) & 0xFF;
        if (gb->cpu.flags.C)
            A -= 0x60;
    } else {
        /* ADD */
        if (gb->cpu.flags.H || (A & 0x0F) > 0x9)
            A += 0x06;
        if(gb->cpu.flags.C || A > 0x9F)
            A += 0x60;
    }
    gb->cpu.A = A & 0xFF;
    
    if (A & 0x100) gb->cpu.flags.C = 1;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.Z = (!gb->cpu.A);
    
    gb_add_clocks(gb, 4);
}

void NOP(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 4);
}
void DI(struct gameboy *gb, struct op *op) {
    gb->ime = 0;
    gb->ime_delay = 0;
    gb_add_clocks(gb, 4);
}
void EI(struct gameboy *gb, struct op *op) {
    //gb->ime = 1;
    gb->ime_delay = 1;
    gb_add_clocks(gb, 4);
}
void STOP(struct gameboy *gb, struct op *op) {
    gb->oscillator_stopped = 1;
    gb_add_clocks(gb, 4);
}
void HALT(struct gameboy *gb, struct op *op) {
    gb->clock_stopped = 1;
    gb_add_clocks(gb, 4);
}
void CPL(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.N = 1;
    gb->cpu.flags.H = 1;
    gb->cpu.A = ~gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void SCF(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.N = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.C = 1;
    gb_add_clocks(gb, 4);
}
void CCF(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.N = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.C ^= 1;
    gb_add_clocks(gb, 4);
}

void JP_a16(struct gameboy *gb, struct op *op) {
	gb->cpu.PC = op->immediate.word;
    gb_add_clocks(gb, 16);
}
void JP_HL(struct gameboy *gb, struct op *op) {
	gb->cpu.PC = gb->cpu.HL;
    gb_add_clocks(gb, 4);
}
void JP_NZ_a16(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.Z) {
		gb->cpu.PC = op->immediate.word;
        gb_add_clocks(gb, 16);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void JP_Z_a16(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.Z) {
		gb->cpu.PC = op->immediate.word;
        gb_add_clocks(gb, 16);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void JP_NC_a16(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.C) {
		gb->cpu.PC = op->immediate.word;
        gb_add_clocks(gb, 16);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void JP_C_a16(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.C) {
		gb->cpu.PC = op->immediate.word;
        gb_add_clocks(gb, 16);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void JR_r8(struct gameboy *gb, struct op *op) {
	int8_t ofs = op->immediate.lo;
	gb->cpu.PC += ofs;
    gb_add_clocks(gb, 12);
}
void JR_NZ_r8(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.Z) {
		int8_t ofs = op->immediate.lo;
		gb->cpu.PC += ofs;
        gb_add_clocks(gb, 12);
    } else {
        gb_add_clocks(gb, 8);
    }
}
void JR_Z_r8(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.Z) {
		int8_t ofs = op->immediate.lo;
		gb->cpu.PC += ofs;
        gb_add_clocks(gb, 12);
    } else {
        gb_add_clocks(gb, 8);
    }
}
void JR_NC_r8(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.C) {
		int8_t ofs = op->immediate.lo;
		gb->cpu.PC += ofs;
        gb_add_clocks(gb, 12);
    } else {
        gb_add_clocks(gb, 8);
    }
}
void JR_C_r8(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.C) {
		int8_t ofs = op->immediate.lo;
		gb->cpu.PC += ofs;
        gb_add_clocks(gb, 12);
    } else {
        gb_add_clocks(gb, 8);
    }
}

void CALL_a16(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 4);
    gb_add_clocks(gb, 4);
    uint16_t addr;
    addr  = mem_read_u8(gb, gb->cpu.PC-2);
    gb_add_clocks(gb, 4);
    addr |= mem_read_u8(gb, gb->cpu.PC-1) << 8;
    gb_add_clocks(gb, 4);
    
    mem_write_u8(gb, gb->cpu.SP-1, (gb->cpu.PC >> 8) & 0xFF);
    gb_add_clocks(gb, 4);
    mem_write_u8(gb, gb->cpu.SP-2, (gb->cpu.PC >> 0) & 0xFF);
    gb->cpu.SP -= 2;

    gb_add_clocks(gb, 4);
    gb->cpu.PC = addr;
}
void CALL_NZ_a16(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.Z) {
		CALL_a16(gb, op);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void CALL_Z_a16(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.Z) {
		CALL_a16(gb, op);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void CALL_NC_a16(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.C) {
		CALL_a16(gb, op);
    } else {
        gb_add_clocks(gb, 12);
    }
}
void CALL_C_a16(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.C) {
		CALL_a16(gb, op);
    } else {
        gb_add_clocks(gb, 12);
    }
}

void RET(struct gameboy *gb, struct op *op) {
    uint16_t addr = cpu_stack_pop(gb);
    gb->cpu.PC = addr;
    gb_add_clocks(gb, 16);
}
void RET_NZ(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.Z) {
		RET(gb, op);
        gb_add_clocks(gb, 4);  /* Plus the 16 in RET() */
    } else {
        gb_add_clocks(gb, 8);
    }
}
void RET_Z(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.Z) {
		RET(gb, op);
        gb_add_clocks(gb, 4);  /* Plus the 16 in RET() */
    } else {
        gb_add_clocks(gb, 8);
    }
}
void RET_NC(struct gameboy *gb, struct op *op) {
	if (!gb->cpu.flags.C) {
		RET(gb, op);
        gb_add_clocks(gb, 4);  /* Plus the 16 in RET() */
    } else {
        gb_add_clocks(gb, 8);
    }
}
void RET_C(struct gameboy *gb, struct op *op) {
	if (gb->cpu.flags.C) {
		RET(gb, op);
        gb_add_clocks(gb, 4);  /* Plus the 16 in RET() */
    } else {
        gb_add_clocks(gb, 8);
    }
}
void RETI(struct gameboy *gb, struct op *op) {
    RET(gb, op);
    gb->ime = 1;
    gb_add_clocks(gb, 0);
}

void RST(struct gameboy *gb, uint8_t low) {
    cpu_stack_push(gb, gb->cpu.PC);
    gb->cpu.PC = 0x0000 | low;
    gb_add_clocks(gb, 16);
}

void RST_00H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x00);
}
void RST_08H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x08);
}
void RST_10H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x10);
}
void RST_18H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x18);
}
void RST_20H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x20);
}
void RST_28H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x28);
}
void RST_30H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x30);
}
void RST_38H(struct gameboy *gb, struct op *op) {
    RST(gb, 0x38);
}

void PUSH_BC(struct gameboy *gb, struct op *op) {
    cpu_stack_push(gb, gb->cpu.BC);
    gb_add_clocks(gb, 16);
}
void PUSH_DE(struct gameboy *gb, struct op *op) {
    cpu_stack_push(gb, gb->cpu.DE);
    gb_add_clocks(gb, 16);
}
void PUSH_HL(struct gameboy *gb, struct op *op) {
    cpu_stack_push(gb, gb->cpu.HL);
    gb_add_clocks(gb, 16);
}
void PUSH_AF(struct gameboy *gb, struct op *op) {
    cpu_stack_push(gb, gb->cpu.AF);
    gb_add_clocks(gb, 16);
}

void POP_BC(struct gameboy *gb, struct op *op) {
    gb->cpu.B = mem_read_u8(gb, gb->cpu.SP+1);
    gb->cpu.C = mem_read_u8(gb, gb->cpu.SP);
	gb->cpu.SP += 2;
    gb_add_clocks(gb, 12);
}
void POP_DE(struct gameboy *gb, struct op *op) {
    gb->cpu.D = mem_read_u8(gb, gb->cpu.SP+1);
    gb->cpu.E = mem_read_u8(gb, gb->cpu.SP);
	gb->cpu.SP += 2;
    gb_add_clocks(gb, 12);
}
void POP_HL(struct gameboy *gb, struct op *op) {
    gb->cpu.H = mem_read_u8(gb, gb->cpu.SP+1);
    gb->cpu.L = mem_read_u8(gb, gb->cpu.SP);
	gb->cpu.SP += 2;
    gb_add_clocks(gb, 12);
}
void POP_AF(struct gameboy *gb, struct op *op) {
    gb->cpu.A = mem_read_u8(gb, gb->cpu.SP+1);
    gb->cpu.F = mem_read_u8(gb, gb->cpu.SP);
	gb->cpu.SP += 2;
    /* Special case - lower nibble of F is always 0 */
    gb->cpu.F &= ~0xF;
    gb_add_clocks(gb, 12);
}

void ADD_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.N = 0;
	uint16_t result = *r1 + r2;
    gb->cpu.flags.C = result >> 8;
    gb->cpu.flags.H = ((*r1 & 0x0F) + (r2 & 0x0F) > 0x0F);
	*r1 = result;
    if (!*r1) gb->cpu.flags.Z = 1;
}
void ADD_A_B(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void ADD_A_C(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void ADD_A_D(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void ADD_A_E(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void ADD_A_H(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void ADD_A_L(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void ADD_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    ADD_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void ADD_A_A(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void ADD_A_d8(struct gameboy *gb, struct op *op) {
	ADD_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}
void ADD_a16_a16(struct gameboy *gb, uint16_t *r1, uint16_t *r2) {
	gb->cpu.flags.N = 0;
    uint32_t result = *r1 + *r2;
	/* Bit 15 carry */
    gb->cpu.flags.C = result >> 16;
    /* Bit 11 carry */
    gb->cpu.flags.H = ((*r1 & 0x0FFF) + (*r2 & 0x0FFF) > 0x0FFF);
    *r1 = result;
}
void ADD_HL_BC(struct gameboy *gb, struct op *op) {
	ADD_a16_a16(gb, &gb->cpu.HL, &gb->cpu.BC);
    gb_add_clocks(gb, 8);
}
void ADD_HL_DE(struct gameboy *gb, struct op *op) {
	ADD_a16_a16(gb, &gb->cpu.HL, &gb->cpu.DE);
    gb_add_clocks(gb, 8);
}
void ADD_HL_HL(struct gameboy *gb, struct op *op) {
	ADD_a16_a16(gb, &gb->cpu.HL, &gb->cpu.HL);
    gb_add_clocks(gb, 8);
}
void ADD_HL_SP(struct gameboy *gb, struct op *op) {
	ADD_a16_a16(gb, &gb->cpu.HL, &gb->cpu.SP);
    gb_add_clocks(gb, 8);
}
void ADD_SP_r8(struct gameboy *gb, struct op *op) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.N = 0;
    int8_t d = op->immediate.lo;
    int32_t result = gb->cpu.SP + d;
	/* Bit 7 carry */
    gb->cpu.flags.C = ((gb->cpu.SP & 0x0FF) + ((int8_t) op->immediate.lo & 0x0FF) > 0x0FF);
    /* Bit 3 carry */
    gb->cpu.flags.H = ((gb->cpu.SP & 0x0F) + ((int8_t) op->immediate.lo & 0x0F) > 0x0F);
    gb->cpu.SP = result;
    gb_add_clocks(gb, 16);
}

void ADC_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.N = 0;
    uint16_t result = *r1 + r2 + gb->cpu.flags.C;
    gb->cpu.flags.H = ((*r1 & 0x0F) + (r2 & 0x0F) + gb->cpu.flags.C > 0x0F);
    gb->cpu.flags.C = result >> 8;
    *r1 = result;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void ADC_A_B(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void ADC_A_C(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void ADC_A_D(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void ADC_A_E(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void ADC_A_H(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void ADC_A_L(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void ADC_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    ADC_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void ADC_A_A(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void ADC_A_d8(struct gameboy *gb, struct op *op) {
	ADC_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void SUB_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.N = 1;
    uint16_t result = *r1 - r2;
    gb->cpu.flags.C = result >> 8;
    gb->cpu.flags.H = ((*r1 & 0x0F) - (r2 & 0x0F) < 0);
    *r1 = result;
	if (!*r1) gb->cpu.flags.Z = 1;
}

void SUB_A_B(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void SUB_A_C(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void SUB_A_D(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void SUB_A_E(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void SUB_A_H(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void SUB_A_L(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void SUB_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    SUB_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void SUB_A_A(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void SUB_A_d8(struct gameboy *gb, struct op *op) {
	SUB_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void SBC_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.N = 1;
    uint16_t result = *r1 - r2 - gb->cpu.flags.C;
    gb->cpu.flags.H = ((*r1 & 0x0F) - (r2 & 0x0F) - gb->cpu.flags.C < 0);
    gb->cpu.flags.C = result >> 8;
    *r1 = result;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void SBC_A_B(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void SBC_A_C(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void SBC_A_D(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void SBC_A_E(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void SBC_A_H(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void SBC_A_L(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void SBC_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    SBC_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void SBC_A_A(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void SBC_A_d8(struct gameboy *gb, struct op *op) {
	SBC_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void AND_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.H = 1;
	gb->cpu.flags.N = 0;
	gb->cpu.flags.C = 0;
	*r1 &= r2;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void AND_B(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void AND_C(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void AND_D(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void AND_E(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void AND_H(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void AND_L(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void AND_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    AND_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void AND_A(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void AND_A_d8(struct gameboy *gb, struct op *op) {
	AND_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void XOR_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.H = 0;
	gb->cpu.flags.N = 0;
	gb->cpu.flags.C = 0;
	*r1 ^= r2;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void XOR_B(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void XOR_C(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void XOR_D(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void XOR_E(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void XOR_H(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void XOR_L(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void XOR_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    XOR_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void XOR_A(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void XOR_A_d8(struct gameboy *gb, struct op *op) {
	XOR_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void OR_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.H = 0;
	gb->cpu.flags.N = 0;
	gb->cpu.flags.C = 0;
	*r1 |= r2;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void OR_B(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void OR_C(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void OR_D(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void OR_E(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void OR_H(struct gameboy *gb, struct op *op) {
    OR_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void OR_L(struct gameboy *gb, struct op *op) {
    OR_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void OR_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    OR_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void OR_A(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void OR_A_d8(struct gameboy *gb, struct op *op) {
	OR_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void CP_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2) {
    int16_t res = *r1 - r2;
	gb->cpu.flags.Z = ((uint8_t) res == 0);
    gb->cpu.flags.N = 1;
	gb->cpu.flags.H = ((*r1 & 0x0F) - (r2 & 0x0F) < 0);
	gb->cpu.flags.C = res >> 8;
}
void CP_B(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void CP_C(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void CP_D(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void CP_E(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void CP_H(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void CP_L(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void CP_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    CP_r_r(gb, &gb->cpu.A, mem_read_u8(gb, gb->cpu.HL));
}
void CP_A(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void CP_A_d8(struct gameboy *gb, struct op *op) {
	CP_r_r(gb, &gb->cpu.A, op->immediate.lo);
    gb_add_clocks(gb, 8);
}

void INC_r(struct gameboy *gb, uint8_t *r1) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.N = 0;
    gb->cpu.flags.H = ((*r1 & 0x0F) == 0x0F);
	(*r1)++;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void INC_B(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void INC_C(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void INC_D(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void INC_E(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void INC_H(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void INC_L(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void INC_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    INC_r(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void INC_A(struct gameboy *gb, struct op *op) {
	INC_r(gb, &gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void INC_r16(struct gameboy *gb, uint16_t *r1) {
    /* Does not set any flags */
	(*r1)++;
}
void INC_BC(struct gameboy *gb, struct op *op) {
	INC_r16(gb, &gb->cpu.BC);
    gb_add_clocks(gb, 8);
}
void INC_DE(struct gameboy *gb, struct op *op) {
	INC_r16(gb, &gb->cpu.DE);
    gb_add_clocks(gb, 8);
}
void INC_HL(struct gameboy *gb, struct op *op) {
	INC_r16(gb, &gb->cpu.HL);
    gb_add_clocks(gb, 8);
}
void INC_SP(struct gameboy *gb, struct op *op) {
	INC_r16(gb, &gb->cpu.SP);
    gb_add_clocks(gb, 8);
}

void DEC_r(struct gameboy *gb, uint8_t *r1) {
	gb->cpu.flags.Z = 0;
	gb->cpu.flags.N = 1;
	gb->cpu.flags.H = ((*r1 & 0x0F) == 0x00);
	(*r1)--;
	if (!*r1) gb->cpu.flags.Z = 1;
}
void DEC_B(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.B);
    gb_add_clocks(gb, 4);
}
void DEC_C(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.C);
    gb_add_clocks(gb, 4);
}
void DEC_D(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.D);
    gb_add_clocks(gb, 4);
}
void DEC_E(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.E);
    gb_add_clocks(gb, 4);
}
void DEC_H(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.H);
    gb_add_clocks(gb, 4);
}
void DEC_L(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.L);
    gb_add_clocks(gb, 4);
}
void DEC_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    DEC_r(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void DEC_A(struct gameboy *gb, struct op *op) {
	DEC_r(gb, &gb->cpu.A);
    gb_add_clocks(gb, 4);
}
void DEC_r16(struct gameboy *gb, uint16_t *r1) {
    /* Does not set any flags */
	(*r1)--;
}
void DEC_BC(struct gameboy *gb, struct op *op) {
	DEC_r16(gb, &gb->cpu.BC);
    gb_add_clocks(gb, 8);
}
void DEC_DE(struct gameboy *gb, struct op *op) {
	DEC_r16(gb, &gb->cpu.DE);
    gb_add_clocks(gb, 8);
}
void DEC_HL(struct gameboy *gb, struct op *op) {
	DEC_r16(gb, &gb->cpu.HL);
    gb_add_clocks(gb, 8);
}
void DEC_SP(struct gameboy *gb, struct op *op) {
	DEC_r16(gb, &gb->cpu.SP);
    gb_add_clocks(gb, 8);
}

void LD_A_A(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_A_B(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_A_C(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_A_D(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_A_E(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_A_H(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_A_L(struct gameboy *gb, struct op *op) {
	gb->cpu.A = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.A = mem_read_u8(gb, gb->cpu.HL);
}

void LD_B_A(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_B_B(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_B_C(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_B_D(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_B_E(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_B_H(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_B_L(struct gameboy *gb, struct op *op) {
	gb->cpu.B = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_B_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.B = mem_read_u8(gb, gb->cpu.HL);
}

void LD_C_A(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_C_B(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_C_C(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_C_D(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_C_E(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_C_H(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_C_L(struct gameboy *gb, struct op *op) {
	gb->cpu.C = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_C_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.C = mem_read_u8(gb, gb->cpu.HL);
}

void LD_D_A(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_D_B(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_D_C(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_D_D(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_D_E(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_D_H(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_D_L(struct gameboy *gb, struct op *op) {
	gb->cpu.D = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_D_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.D = mem_read_u8(gb, gb->cpu.HL);
}

void LD_E_A(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_E_B(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_E_C(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_E_D(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_E_E(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_E_H(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_E_L(struct gameboy *gb, struct op *op) {
	gb->cpu.E = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_E_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.E = mem_read_u8(gb, gb->cpu.HL);
}

void LD_H_A(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_H_B(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_H_C(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_H_D(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_H_E(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_H_H(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_H_L(struct gameboy *gb, struct op *op) {
	gb->cpu.H = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_H_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.H = mem_read_u8(gb, gb->cpu.HL);
}

void LD_L_A(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.A;
    gb_add_clocks(gb, 4);
}
void LD_L_B(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.B;
    gb_add_clocks(gb, 4);
}
void LD_L_C(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.C;
    gb_add_clocks(gb, 4);
}
void LD_L_D(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.D;
    gb_add_clocks(gb, 4);
}
void LD_L_E(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.E;
    gb_add_clocks(gb, 4);
}
void LD_L_H(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.H;
    gb_add_clocks(gb, 4);
}
void LD_L_L(struct gameboy *gb, struct op *op) {
	gb->cpu.L = gb->cpu.L;
    gb_add_clocks(gb, 4);
}
void LD_L_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    gb->cpu.L = mem_read_u8(gb, gb->cpu.HL);
}


void LD_BCa_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.BC, gb->cpu.A);
}
void LD_DEa_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.DE, gb->cpu.A);
}
void LD_I_HLa_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.HL++;
    mem_write_u8(gb, addr, gb->cpu.A);
}
void LD_D_HLa_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.HL--;
    mem_write_u8(gb, addr, gb->cpu.A);
}

void LD_A_BCa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.BC;
    gb->cpu.A = mem_read_u8(gb, addr);
}
void LD_A_DEa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.DE;
    gb->cpu.A = mem_read_u8(gb, addr);
}
void LD_I_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.HL++;
    gb->cpu.A = mem_read_u8(gb, addr);
}
void LD_D_A_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = gb->cpu.HL--;
    gb->cpu.A = mem_read_u8(gb, addr);
}

void LD_HLa_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.A);
}
void LD_HLa_B(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.B);
}
void LD_HLa_C(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.C);
}
void LD_HLa_D(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.D);
}
void LD_HLa_E(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.E);
}
void LD_HLa_H(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.H);
}
void LD_HLa_L(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
    mem_write_u8(gb, gb->cpu.HL, gb->cpu.L);
}

void LD_A_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.A = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_B_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.B = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_C_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.C = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_D_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.D = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_E_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.E = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_H_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.H = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_L_d8(struct gameboy *gb, struct op *op) {
	gb->cpu.L = op->immediate.lo;
    gb_add_clocks(gb, 8);
}
void LD_HLa_d8(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    mem_write_u8(gb, gb->cpu.HL, op->immediate.lo);
}

void LDH_a8_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 4);
    gb_add_clocks(gb, 4);
    gb_add_clocks(gb, 4);
	uint16_t addr = 0xFF00 + op->immediate.lo;
    mem_write_u8(gb, addr, gb->cpu.A);
}
void LDH_A_a8(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
	uint16_t addr = 0xFF00 + op->immediate.lo;
    gb->cpu.A = mem_read_u8(gb, addr);
}
void LDH_Ca_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = 0xFF00 + gb->cpu.C;
    mem_write_u8(gb, addr, gb->cpu.A);
}
void LDH_A_Ca(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 8);
	uint16_t addr = 0xFF00 + gb->cpu.C;
    gb->cpu.A = mem_read_u8(gb, addr);
}

void LD_a16_A(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 16);
	uint16_t addr = op->immediate.word;
    mem_write_u8(gb, addr, gb->cpu.A);
}
void LD_A_a16(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 16);
	uint16_t addr = op->immediate.word;
    gb->cpu.A = mem_read_u8(gb, addr);
}

void LD_BC_d16(struct gameboy *gb, struct op *op) {
	gb->cpu.BC = op->immediate.word;
    gb_add_clocks(gb, 12);
}
void LD_DE_d16(struct gameboy *gb, struct op *op) {
	gb->cpu.DE = op->immediate.word;
    gb_add_clocks(gb, 12);
}
void LD_HL_d16(struct gameboy *gb, struct op *op) {
	gb->cpu.HL = op->immediate.word;
    gb_add_clocks(gb, 12);
}
void LD_SP_d16(struct gameboy *gb, struct op *op) {
	gb->cpu.SP = op->immediate.word;
    gb_add_clocks(gb, 12);
}
void LD_a16_SP(struct gameboy *gb, struct op *op) {
	uint16_t addr = op->immediate.word;
    mem_write_u8(gb, addr, gb->cpu.SP & 0xFF);
    mem_write_u8(gb, addr+1, gb->cpu.SP >> 8);
    gb_add_clocks(gb, 20);
}
void LD_SP_HL(struct gameboy *gb, struct op *op) {
	gb->cpu.SP = gb->cpu.HL;
    gb_add_clocks(gb, 8);
}
void LDHL_SP_r8(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.N = 0;
    int8_t d = op->immediate.lo;
    int32_t result = gb->cpu.SP + d;
    /* Bit 7 carry */
    gb->cpu.flags.C = ((gb->cpu.SP & 0x0FF) + ((int8_t) op->immediate.lo & 0x0FF) > 0x0FF);
    /* Bit 3 carry */
    gb->cpu.flags.H = ((gb->cpu.SP & 0x0F) + ((int8_t) op->immediate.lo & 0x0F) > 0x0F);
    gb->cpu.HL = result;
    gb_add_clocks(gb, 12);
}

void RLCA(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = 0;
    uint8_t old = gb->cpu.A;
    gb->cpu.A = gb->cpu.A << 1;
    /* Bit 7 carry */
    if (old & 0x80) {
        gb->cpu.A |= 0x01;
        gb->cpu.flags.C = 1;
    }
    gb_add_clocks(gb, 4);
}

void RLA(struct gameboy *gb, struct op *op) {
    uint8_t C_tmp = gb->cpu.flags.C;
    gb->cpu.flags.C = ((gb->cpu.A & 0x80) != 0);
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.A = (gb->cpu.A << 1) | C_tmp;
    gb_add_clocks(gb, 4);
}

void RRCA(struct gameboy *gb, struct op *op) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = 0;
    uint8_t old = gb->cpu.A;
    gb->cpu.A = gb->cpu.A >> 1;
    /* Bit 0 carry */
    if (old & 0x01) {
        gb->cpu.A |= 0x80;
        gb->cpu.flags.C = 1;
    }
    gb_add_clocks(gb, 4);
}

void RRA(struct gameboy *gb, struct op *op) {
    uint8_t C_tmp = gb->cpu.flags.C;
    gb->cpu.flags.C = ((gb->cpu.A & 0x01) != 0);
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.A = (gb->cpu.A >> 1) | (C_tmp << 7);
    gb_add_clocks(gb, 4);
}

void RLC(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.C = ((*r1 & 0x80) != 0);
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    *r1 = (*r1 << 1) | gb->cpu.flags.C;
    gb->cpu.flags.Z = (*r1 == 0);
}
void RRC(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.C = ((*r1 & 0x01) != 0);
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    *r1 = (*r1 >> 1) | (gb->cpu.flags.C << 7);
    gb->cpu.flags.Z = (*r1 == 0);
}

void RLC_B(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void RLC_C(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void RLC_D(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void RLC_E(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void RLC_H(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void RLC_L(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void RLC_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RLC(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RLC_A(struct gameboy *gb, struct op *op) {
    RLC(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void RRC_B(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void RRC_C(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void RRC_D(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void RRC_E(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void RRC_H(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void RRC_L(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void RRC_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RRC(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RRC_A(struct gameboy *gb, struct op *op) {
    RRC(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void RL(struct gameboy *gb, uint8_t *r1) {
    uint8_t C_tmp = gb->cpu.flags.C;
    gb->cpu.flags.C = ((*r1 & 0x80) != 0);
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    *r1 = (*r1 << 1) | C_tmp;
    gb->cpu.flags.Z = (*r1 == 0);
}
void RR(struct gameboy *gb, uint8_t *r1) {
    uint8_t C_tmp = gb->cpu.flags.C;
    gb->cpu.flags.C = ((*r1 & 0x01) != 0);
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    *r1 = (*r1 >> 1) | (C_tmp << 7);
    gb->cpu.flags.Z = (*r1 == 0);
}

void RL_B(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void RL_C(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void RL_D(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void RL_E(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void RL_H(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void RL_L(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void RL_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RL(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RL_A(struct gameboy *gb, struct op *op) {
    RL(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void RR_B(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void RR_C(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void RR_D(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void RR_E(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void RR_H(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void RR_L(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void RR_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RR(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RR_A(struct gameboy *gb, struct op *op) {
    RR(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void SLA(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = ((*r1 & 0x80) != 0);
    *r1 = *r1 << 1;
    gb->cpu.flags.Z = (*r1 == 0);
}
void SLA_B(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void SLA_C(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void SLA_D(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void SLA_E(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void SLA_H(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void SLA_L(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void SLA_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SLA(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SLA_A(struct gameboy *gb, struct op *op) {
    SLA(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void SRA(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = ((*r1 & 0x01) != 0);
    *r1 = (*r1 >> 1) | (*r1 & 0x80);
    gb->cpu.flags.Z = (*r1 == 0);
}
void SRA_B(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void SRA_C(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void SRA_D(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void SRA_E(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void SRA_H(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void SRA_L(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void SRA_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SRA(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SRA_A(struct gameboy *gb, struct op *op) {
    SRA(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}

void SWAP(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = 0;
    *r1 = ((*r1 >> 4) & 0x0F0F0F0F) | ((*r1 & 0x0F0F0F0F) << 4);
    if (!*r1) gb->cpu.flags.Z = 1;
}
void SWAP_B(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void SWAP_C(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void SWAP_D(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void SWAP_E(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void SWAP_H(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void SWAP_L(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void SWAP_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SWAP(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SWAP_A(struct gameboy *gb, struct op *op) {
    SWAP(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}


void SRL(struct gameboy *gb, uint8_t *r1) {
    gb->cpu.flags.Z = 0;
    gb->cpu.flags.H = 0;
    gb->cpu.flags.N = 0;
    gb->cpu.flags.C = *r1 & 0x1;
    *r1 = *r1 >> 0x1;
    if (!*r1) gb->cpu.flags.Z = 1;
}
void SRL_B(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.B);
    gb_add_clocks(gb, 8);
}
void SRL_C(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.C);
    gb_add_clocks(gb, 8);
}
void SRL_D(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.D);
    gb_add_clocks(gb, 8);
}
void SRL_E(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.E);
    gb_add_clocks(gb, 8);
}
void SRL_H(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.H);
    gb_add_clocks(gb, 8);
}
void SRL_L(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.L);
    gb_add_clocks(gb, 8);
}
void SRL_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SRL(gb, &val);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SRL_A(struct gameboy *gb, struct op *op) {
    SRL(gb, &gb->cpu.A);
    gb_add_clocks(gb, 8);
}


void SET_r(struct gameboy *gb, uint8_t *r1, uint8_t bit) {
    *r1 |= 0x1 << bit;
}
void SET_0_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 0);
    gb_add_clocks(gb, 8);
}
void SET_0_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 0);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_0_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 0);
    gb_add_clocks(gb, 8);
}
void SET_1_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 1);
    gb_add_clocks(gb, 8);
}
void SET_1_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 1);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_1_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 1);
    gb_add_clocks(gb, 8);
}
void SET_2_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 2);
    gb_add_clocks(gb, 8);
}
void SET_2_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 2);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_2_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 2);
    gb_add_clocks(gb, 8);
}
void SET_3_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 3);
    gb_add_clocks(gb, 8);
}
void SET_3_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 3);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_3_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 3);
    gb_add_clocks(gb, 8);
}
void SET_4_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 4);
    gb_add_clocks(gb, 8);
}
void SET_4_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 4);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_4_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 4);
    gb_add_clocks(gb, 8);
}
void SET_5_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 5);
    gb_add_clocks(gb, 8);
}
void SET_5_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 5);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_5_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 5);
    gb_add_clocks(gb, 8);
}
void SET_6_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 6);
    gb_add_clocks(gb, 8);
}
void SET_6_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 6);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_6_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 6);
    gb_add_clocks(gb, 8);
}
void SET_7_B(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.B, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_C(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.C, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_D(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.D, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_E(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.E, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_H(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.H, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_L(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.L, 7);
    gb_add_clocks(gb, 8);
}
void SET_7_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    SET_r(gb, &val, 7);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void SET_7_A(struct gameboy *gb, struct op *op) {
    SET_r(gb, &gb->cpu.A, 7);
    gb_add_clocks(gb, 8);
}

void BIT_r(struct gameboy *gb, uint8_t *r1, uint8_t bit) {
    gb->cpu.flags.N = 0;
    gb->cpu.flags.H = 1;
    gb->cpu.flags.Z = !(*r1 & (0x1 << bit));
}
void _BIT(struct gameboy *gb, uint8_t v, uint8_t bit) {
    gb->cpu.flags.N = 0;
    gb->cpu.flags.H = 1;
    gb->cpu.flags.Z = !(v & (0x1 << bit));
}
void BIT_0_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 0);
    gb_add_clocks(gb, 8);
}
void BIT_0_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 0);
}
void BIT_0_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 0);
    gb_add_clocks(gb, 8);
}
void BIT_1_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 1);
    gb_add_clocks(gb, 8);
}
void BIT_1_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 1);
}
void BIT_1_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 1);
    gb_add_clocks(gb, 8);
}
void BIT_2_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 2);
    gb_add_clocks(gb, 8);
}
void BIT_2_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 2);
}
void BIT_2_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 2);
    gb_add_clocks(gb, 8);
}
void BIT_3_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 3);
    gb_add_clocks(gb, 8);
}
void BIT_3_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 3);
}
void BIT_3_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 3);
    gb_add_clocks(gb, 8);
}
void BIT_4_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 4);
    gb_add_clocks(gb, 8);
}
void BIT_4_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 4);
}
void BIT_4_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 4);
    gb_add_clocks(gb, 8);
}
void BIT_5_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 5);
    gb_add_clocks(gb, 8);
}
void BIT_5_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 5);
}
void BIT_5_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 5);
    gb_add_clocks(gb, 8);
}
void BIT_6_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 6);
    gb_add_clocks(gb, 8);
}
void BIT_6_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 6);
}
void BIT_6_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 6);
    gb_add_clocks(gb, 8);
}
void BIT_7_B(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.B, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_C(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.C, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_D(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.D, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_E(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.E, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_H(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.H, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_L(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.L, 7);
    gb_add_clocks(gb, 8);
}
void BIT_7_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    _BIT(gb, mem_read_u8(gb, gb->cpu.HL), 7);
}
void BIT_7_A(struct gameboy *gb, struct op *op) {
    BIT_r(gb, &gb->cpu.A, 7);
    gb_add_clocks(gb, 8);
}

void RES_r(struct gameboy *gb, uint8_t *r1, uint8_t bit) {
    *r1 &= ~(0x1 << bit);
}
void RES_0_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 0);
    gb_add_clocks(gb, 8);
}
void RES_0_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 0);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_0_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 0);
    gb_add_clocks(gb, 8);
}
void RES_1_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 1);
    gb_add_clocks(gb, 8);
}
void RES_1_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 1);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_1_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 1);
    gb_add_clocks(gb, 8);
}
void RES_2_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 2);
    gb_add_clocks(gb, 8);
}
void RES_2_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 2);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_2_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 2);
    gb_add_clocks(gb, 8);
}
void RES_3_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 3);
    gb_add_clocks(gb, 8);
}
void RES_3_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 3);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_3_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 3);
    gb_add_clocks(gb, 8);
}
void RES_4_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 4);
    gb_add_clocks(gb, 8);
}
void RES_4_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 4);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_4_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 4);
    gb_add_clocks(gb, 8);
}
void RES_5_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 5);
    gb_add_clocks(gb, 8);
}
void RES_5_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 5);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_5_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 5);
    gb_add_clocks(gb, 8);
}
void RES_6_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 6);
    gb_add_clocks(gb, 8);
}
void RES_6_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 6);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_6_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 6);
    gb_add_clocks(gb, 8);
}
void RES_7_B(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.B, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_C(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.C, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_D(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.D, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_E(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.E, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_H(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.H, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_L(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.L, 7);
    gb_add_clocks(gb, 8);
}
void RES_7_HLa(struct gameboy *gb, struct op *op) {
    gb_add_clocks(gb, 12);
    uint8_t val = mem_read_u8(gb, gb->cpu.HL);
    gb_add_clocks(gb, 4);
    RES_r(gb, &val, 7);
    mem_write_u8(gb, gb->cpu.HL, val);
}
void RES_7_A(struct gameboy *gb, struct op *op) {
    RES_r(gb, &gb->cpu.A, 7);
    gb_add_clocks(gb, 8);
}