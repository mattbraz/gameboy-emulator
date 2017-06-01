
struct op *cpu_get_op(struct gameboy *gb, struct op *op);
void cpu_exec_op(struct gameboy *gb, struct op *op);
void cpu_stack_push(struct gameboy *gb, uint16_t val);
uint16_t cpu_stack_pop(struct gameboy *gb);

void ILLEGAL_OP(struct gameboy *gb, struct op *op);

void LD_r_r();
void ADD_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void ADD_a16_a16(struct gameboy *gb, uint16_t *r1, uint16_t *r2);
void ADC_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void SUB_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void SBC_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void AND_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void XOR_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void OR_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void CP_r_r(struct gameboy *gb, uint8_t *r1, uint8_t r2);
void INC_r(struct gameboy *gb, uint8_t *r1);
void INC_r16(struct gameboy *gb, uint16_t *r1);
void DEC_r(struct gameboy *gb, uint8_t *r1);
void DEC_r16(struct gameboy *gb, uint16_t *r1);

void RLC(struct gameboy *gb, uint8_t *r1);
void RRC(struct gameboy *gb, uint8_t *r1);
void RL(struct gameboy *gb, uint8_t *r1);
void RR(struct gameboy *gb, uint8_t *r1);
void SLA(struct gameboy *gb, uint8_t *r1);
void SRA(struct gameboy *gb, uint8_t *r1);
void SWAP(struct gameboy *gb, uint8_t *r1);
void SRL(struct gameboy *gb, uint8_t *r1);
void SET_r(struct gameboy *gb, uint8_t *r1, uint8_t bit);
void BIT_r(struct gameboy *gb, uint8_t *r1, uint8_t bit);
void _BIT(struct gameboy *gb, uint8_t v, uint8_t bit);
void RES_r(struct gameboy *gb, uint8_t *r1, uint8_t bit);
void RST(struct gameboy *gb, uint8_t low);

/* --------------- */

void NOP(struct gameboy *gb, struct op *op);
void DAA(struct gameboy *gb, struct op *op);
void STOP(struct gameboy *gb, struct op *op);
void HALT(struct gameboy *gb, struct op *op);

void DI(struct gameboy *gb, struct op *op);
void EI(struct gameboy *gb, struct op *op);
void CPL(struct gameboy *gb, struct op *op);
void CCF(struct gameboy *gb, struct op *op);
void SCF(struct gameboy *gb, struct op *op);

void RLCA(struct gameboy *gb, struct op *op);
void RLA(struct gameboy *gb, struct op *op);
void RRCA(struct gameboy *gb, struct op *op);
void RRA(struct gameboy *gb, struct op *op);

void JP_a16(struct gameboy *gb, struct op *op);
void JP_HL(struct gameboy *gb, struct op *op);
void JP_NZ_a16(struct gameboy *gb, struct op *op);
void JP_Z_a16(struct gameboy *gb, struct op *op);
void JP_NC_a16(struct gameboy *gb, struct op *op);
void JP_C_a16(struct gameboy *gb, struct op *op);
void JR_r8(struct gameboy *gb, struct op *op);
void JR_NZ_r8(struct gameboy *gb, struct op *op);
void JR_Z_r8(struct gameboy *gb, struct op *op);
void JR_NC_r8(struct gameboy *gb, struct op *op);
void JR_C_r8(struct gameboy *gb, struct op *op);

void CALL_a16(struct gameboy *gb, struct op *op);
void CALL_NZ_a16(struct gameboy *gb, struct op *op);
void CALL_Z_a16(struct gameboy *gb, struct op *op);
void CALL_NC_a16(struct gameboy *gb, struct op *op);
void CALL_C_a16(struct gameboy *gb, struct op *op);

void RET(struct gameboy *gb, struct op *op);
void RET_NZ(struct gameboy *gb, struct op *op);
void RET_Z(struct gameboy *gb, struct op *op);
void RET_NC(struct gameboy *gb, struct op *op);
void RET_C(struct gameboy *gb, struct op *op);
void RETI(struct gameboy *gb, struct op *op);

void RST_00H(struct gameboy *gb, struct op *op);
void RST_08H(struct gameboy *gb, struct op *op);
void RST_10H(struct gameboy *gb, struct op *op);
void RST_18H(struct gameboy *gb, struct op *op);
void RST_20H(struct gameboy *gb, struct op *op);
void RST_28H(struct gameboy *gb, struct op *op);
void RST_30H(struct gameboy *gb, struct op *op);
void RST_38H(struct gameboy *gb, struct op *op);

void PUSH_BC(struct gameboy *gb, struct op *op);
void PUSH_DE(struct gameboy *gb, struct op *op);	
void PUSH_HL(struct gameboy *gb, struct op *op);
void PUSH_AF(struct gameboy *gb, struct op *op);

void POP_BC(struct gameboy *gb, struct op *op);
void POP_DE(struct gameboy *gb, struct op *op);
void POP_HL(struct gameboy *gb, struct op *op);
void POP_AF(struct gameboy *gb, struct op *op);

void ADD_A_B(struct gameboy *gb, struct op *op);
void ADD_A_C(struct gameboy *gb, struct op *op);
void ADD_A_D(struct gameboy *gb, struct op *op);
void ADD_A_E(struct gameboy *gb, struct op *op);
void ADD_A_H(struct gameboy *gb, struct op *op);
void ADD_A_L(struct gameboy *gb, struct op *op);
void ADD_A_HLa(struct gameboy *gb, struct op *op);
void ADD_A_A(struct gameboy *gb, struct op *op);
void ADD_A_d8(struct gameboy *gb, struct op *op);

void ADD_HL_BC(struct gameboy *gb, struct op *op);
void ADD_HL_DE(struct gameboy *gb, struct op *op);
void ADD_HL_HL(struct gameboy *gb, struct op *op);
void ADD_HL_SP(struct gameboy *gb, struct op *op);
void ADD_SP_r8(struct gameboy *gb, struct op *op);

void ADC_A_B(struct gameboy *gb, struct op *op);
void ADC_A_C(struct gameboy *gb, struct op *op);
void ADC_A_D(struct gameboy *gb, struct op *op);
void ADC_A_E(struct gameboy *gb, struct op *op);
void ADC_A_H(struct gameboy *gb, struct op *op);
void ADC_A_L(struct gameboy *gb, struct op *op);
void ADC_A_HLa(struct gameboy *gb, struct op *op);
void ADC_A_A(struct gameboy *gb, struct op *op);
void ADC_A_d8(struct gameboy *gb, struct op *op);

void SUB_A_B(struct gameboy *gb, struct op *op);
void SUB_A_C(struct gameboy *gb, struct op *op);
void SUB_A_D(struct gameboy *gb, struct op *op);
void SUB_A_E(struct gameboy *gb, struct op *op);
void SUB_A_H(struct gameboy *gb, struct op *op);
void SUB_A_L(struct gameboy *gb, struct op *op);
void SUB_A_HLa(struct gameboy *gb, struct op *op);
void SUB_A_A(struct gameboy *gb, struct op *op);
void SUB_A_d8(struct gameboy *gb, struct op *op);

void SBC_A_B(struct gameboy *gb, struct op *op);
void SBC_A_C(struct gameboy *gb, struct op *op);
void SBC_A_D(struct gameboy *gb, struct op *op);
void SBC_A_E(struct gameboy *gb, struct op *op);
void SBC_A_H(struct gameboy *gb, struct op *op);
void SBC_A_L(struct gameboy *gb, struct op *op);
void SBC_A_HLa(struct gameboy *gb, struct op *op);
void SBC_A_A(struct gameboy *gb, struct op *op);
void SBC_A_d8(struct gameboy *gb, struct op *op);

void AND_B(struct gameboy *gb, struct op *op);
void AND_C(struct gameboy *gb, struct op *op);
void AND_D(struct gameboy *gb, struct op *op);
void AND_E(struct gameboy *gb, struct op *op);
void AND_H(struct gameboy *gb, struct op *op);
void AND_L(struct gameboy *gb, struct op *op);
void AND_HLa(struct gameboy *gb, struct op *op);
void AND_A(struct gameboy *gb, struct op *op);
void AND_A_d8(struct gameboy *gb, struct op *op);

void XOR_B(struct gameboy *gb, struct op *op);
void XOR_C(struct gameboy *gb, struct op *op);
void XOR_D(struct gameboy *gb, struct op *op);
void XOR_E(struct gameboy *gb, struct op *op);
void XOR_H(struct gameboy *gb, struct op *op);
void XOR_L(struct gameboy *gb, struct op *op);
void XOR_HLa(struct gameboy *gb, struct op *op);
void XOR_A(struct gameboy *gb, struct op *op);
void XOR_A_d8(struct gameboy *gb, struct op *op);

void OR_B(struct gameboy *gb, struct op *op);
void OR_C(struct gameboy *gb, struct op *op);
void OR_D(struct gameboy *gb, struct op *op);
void OR_E(struct gameboy *gb, struct op *op);
void OR_H(struct gameboy *gb, struct op *op);
void OR_L(struct gameboy *gb, struct op *op);
void OR_HLa(struct gameboy *gb, struct op *op);
void OR_A(struct gameboy *gb, struct op *op);
void OR_A_d8(struct gameboy *gb, struct op *op);

void CP_B(struct gameboy *gb, struct op *op);
void CP_C(struct gameboy *gb, struct op *op);
void CP_D(struct gameboy *gb, struct op *op);
void CP_E(struct gameboy *gb, struct op *op);
void CP_H(struct gameboy *gb, struct op *op);
void CP_L(struct gameboy *gb, struct op *op);
void CP_HLa(struct gameboy *gb, struct op *op);
void CP_A(struct gameboy *gb, struct op *op);
void CP_A_d8(struct gameboy *gb, struct op *op);

void INC_B(struct gameboy *gb, struct op *op);
void INC_C(struct gameboy *gb, struct op *op);
void INC_D(struct gameboy *gb, struct op *op);
void INC_E(struct gameboy *gb, struct op *op);
void INC_H(struct gameboy *gb, struct op *op);
void INC_L(struct gameboy *gb, struct op *op);
void INC_HLa(struct gameboy *gb, struct op *op);
void INC_A(struct gameboy *gb, struct op *op);

void INC_BC(struct gameboy *gb, struct op *op);
void INC_DE(struct gameboy *gb, struct op *op);
void INC_HL(struct gameboy *gb, struct op *op);
void INC_SP(struct gameboy *gb, struct op *op);

void DEC_B(struct gameboy *gb, struct op *op);
void DEC_C(struct gameboy *gb, struct op *op);
void DEC_D(struct gameboy *gb, struct op *op);
void DEC_E(struct gameboy *gb, struct op *op);
void DEC_H(struct gameboy *gb, struct op *op);
void DEC_L(struct gameboy *gb, struct op *op);
void DEC_HLa(struct gameboy *gb, struct op *op);
void DEC_A(struct gameboy *gb, struct op *op);

void DEC_BC(struct gameboy *gb, struct op *op);
void DEC_DE(struct gameboy *gb, struct op *op);
void DEC_HL(struct gameboy *gb, struct op *op);
void DEC_SP(struct gameboy *gb, struct op *op);

// 8 bit 
void LD_A_A(struct gameboy *gb, struct op *op);
void LD_A_B(struct gameboy *gb, struct op *op);
void LD_A_C(struct gameboy *gb, struct op *op);
void LD_A_D(struct gameboy *gb, struct op *op);
void LD_A_E(struct gameboy *gb, struct op *op);
void LD_A_H(struct gameboy *gb, struct op *op);
void LD_A_L(struct gameboy *gb, struct op *op);
void LD_A_HLa(struct gameboy *gb, struct op *op);

void LD_B_A(struct gameboy *gb, struct op *op);
void LD_B_B(struct gameboy *gb, struct op *op);
void LD_B_C(struct gameboy *gb, struct op *op);
void LD_B_D(struct gameboy *gb, struct op *op);
void LD_B_E(struct gameboy *gb, struct op *op);
void LD_B_H(struct gameboy *gb, struct op *op);
void LD_B_L(struct gameboy *gb, struct op *op);
void LD_B_HLa(struct gameboy *gb, struct op *op);

void LD_C_A(struct gameboy *gb, struct op *op);
void LD_C_B(struct gameboy *gb, struct op *op);
void LD_C_C(struct gameboy *gb, struct op *op);
void LD_C_D(struct gameboy *gb, struct op *op);
void LD_C_E(struct gameboy *gb, struct op *op);
void LD_C_H(struct gameboy *gb, struct op *op);
void LD_C_L(struct gameboy *gb, struct op *op);
void LD_C_HLa(struct gameboy *gb, struct op *op);

void LD_D_A(struct gameboy *gb, struct op *op);
void LD_D_B(struct gameboy *gb, struct op *op);
void LD_D_C(struct gameboy *gb, struct op *op);
void LD_D_D(struct gameboy *gb, struct op *op);
void LD_D_E(struct gameboy *gb, struct op *op);
void LD_D_H(struct gameboy *gb, struct op *op);
void LD_D_L(struct gameboy *gb, struct op *op);
void LD_D_HLa(struct gameboy *gb, struct op *op);

void LD_E_A(struct gameboy *gb, struct op *op);
void LD_E_B(struct gameboy *gb, struct op *op);
void LD_E_C(struct gameboy *gb, struct op *op);
void LD_E_D(struct gameboy *gb, struct op *op);
void LD_E_E(struct gameboy *gb, struct op *op);
void LD_E_H(struct gameboy *gb, struct op *op);
void LD_E_L(struct gameboy *gb, struct op *op);
void LD_E_HLa(struct gameboy *gb, struct op *op);

void LD_H_A(struct gameboy *gb, struct op *op);
void LD_H_B(struct gameboy *gb, struct op *op);
void LD_H_C(struct gameboy *gb, struct op *op);
void LD_H_D(struct gameboy *gb, struct op *op);
void LD_H_E(struct gameboy *gb, struct op *op);
void LD_H_H(struct gameboy *gb, struct op *op);
void LD_H_L(struct gameboy *gb, struct op *op);
void LD_H_HLa(struct gameboy *gb, struct op *op);

void LD_L_A(struct gameboy *gb, struct op *op);
void LD_L_B(struct gameboy *gb, struct op *op);
void LD_L_C(struct gameboy *gb, struct op *op);
void LD_L_D(struct gameboy *gb, struct op *op);
void LD_L_E(struct gameboy *gb, struct op *op);
void LD_L_H(struct gameboy *gb, struct op *op);
void LD_L_L(struct gameboy *gb, struct op *op);
void LD_L_HLa(struct gameboy *gb, struct op *op);

void LD_BCa_A(struct gameboy *gb, struct op *op);
void LD_DEa_A(struct gameboy *gb, struct op *op);
void LD_I_HLa_A(struct gameboy *gb, struct op *op);
void LD_D_HLa_A(struct gameboy *gb, struct op *op);

void LD_A_BCa(struct gameboy *gb, struct op *op);
void LD_A_DEa(struct gameboy *gb, struct op *op);
void LD_I_A_HLa(struct gameboy *gb, struct op *op);
void LD_D_A_HLa(struct gameboy *gb, struct op *op);

void LD_HLa_A(struct gameboy *gb, struct op *op);
void LD_HLa_B(struct gameboy *gb, struct op *op);
void LD_HLa_C(struct gameboy *gb, struct op *op);
void LD_HLa_D(struct gameboy *gb, struct op *op);
void LD_HLa_E(struct gameboy *gb, struct op *op);
void LD_HLa_H(struct gameboy *gb, struct op *op);
void LD_HLa_L(struct gameboy *gb, struct op *op);

void LD_A_d8(struct gameboy *gb, struct op *op);
void LD_B_d8(struct gameboy *gb, struct op *op);
void LD_C_d8(struct gameboy *gb, struct op *op);
void LD_D_d8(struct gameboy *gb, struct op *op);
void LD_E_d8(struct gameboy *gb, struct op *op);
void LD_H_d8(struct gameboy *gb, struct op *op);
void LD_L_d8(struct gameboy *gb, struct op *op);
void LD_HLa_d8(struct gameboy *gb, struct op *op);

void LDH_a8_A(struct gameboy *gb, struct op *op);
void LDH_A_a8(struct gameboy *gb, struct op *op);
void LDH_Ca_A(struct gameboy *gb, struct op *op);
void LDH_A_Ca(struct gameboy *gb, struct op *op);
void LD_a16_A(struct gameboy *gb, struct op *op);
void LD_A_a16(struct gameboy *gb, struct op *op);

void LD_BC_d16(struct gameboy *gb, struct op *op);
void LD_DE_d16(struct gameboy *gb, struct op *op);
void LD_HL_d16(struct gameboy *gb, struct op *op);
void LD_SP_d16(struct gameboy *gb, struct op *op);
void LD_a16_SP(struct gameboy *gb, struct op *op);
void LD_SP_HL(struct gameboy *gb, struct op *op);
void LDHL_SP_r8(struct gameboy *gb, struct op *op);


void RLC_B(struct gameboy *gb, struct op *op);
void RLC_C(struct gameboy *gb, struct op *op);
void RLC_D(struct gameboy *gb, struct op *op);
void RLC_E(struct gameboy *gb, struct op *op);
void RLC_H(struct gameboy *gb, struct op *op);
void RLC_L(struct gameboy *gb, struct op *op);
void RLC_HLa(struct gameboy *gb, struct op *op);
void RLC_A(struct gameboy *gb, struct op *op);

void RRC_B(struct gameboy *gb, struct op *op);
void RRC_C(struct gameboy *gb, struct op *op);
void RRC_D(struct gameboy *gb, struct op *op);
void RRC_E(struct gameboy *gb, struct op *op);
void RRC_H(struct gameboy *gb, struct op *op);
void RRC_L(struct gameboy *gb, struct op *op);
void RRC_HLa(struct gameboy *gb, struct op *op);
void RRC_A(struct gameboy *gb, struct op *op);

void RL_B(struct gameboy *gb, struct op *op);
void RL_C(struct gameboy *gb, struct op *op);
void RL_D(struct gameboy *gb, struct op *op);
void RL_E(struct gameboy *gb, struct op *op);
void RL_H(struct gameboy *gb, struct op *op);
void RL_L(struct gameboy *gb, struct op *op);
void RL_HLa(struct gameboy *gb, struct op *op);
void RL_A(struct gameboy *gb, struct op *op);

void RR_B(struct gameboy *gb, struct op *op);
void RR_C(struct gameboy *gb, struct op *op);
void RR_D(struct gameboy *gb, struct op *op);
void RR_E(struct gameboy *gb, struct op *op);
void RR_H(struct gameboy *gb, struct op *op);
void RR_L(struct gameboy *gb, struct op *op);
void RR_HLa(struct gameboy *gb, struct op *op);
void RR_A(struct gameboy *gb, struct op *op);

void SLA_B(struct gameboy *gb, struct op *op);
void SLA_C(struct gameboy *gb, struct op *op);
void SLA_D(struct gameboy *gb, struct op *op);
void SLA_E(struct gameboy *gb, struct op *op);
void SLA_H(struct gameboy *gb, struct op *op);
void SLA_L(struct gameboy *gb, struct op *op);
void SLA_HLa(struct gameboy *gb, struct op *op);
void SLA_A(struct gameboy *gb, struct op *op);

void SRA_B(struct gameboy *gb, struct op *op);
void SRA_C(struct gameboy *gb, struct op *op);
void SRA_D(struct gameboy *gb, struct op *op);
void SRA_E(struct gameboy *gb, struct op *op);
void SRA_H(struct gameboy *gb, struct op *op);
void SRA_L(struct gameboy *gb, struct op *op);
void SRA_HLa(struct gameboy *gb, struct op *op);
void SRA_A(struct gameboy *gb, struct op *op);

void SWAP_B(struct gameboy *gb, struct op *op);
void SWAP_C(struct gameboy *gb, struct op *op);
void SWAP_D(struct gameboy *gb, struct op *op);
void SWAP_E(struct gameboy *gb, struct op *op);
void SWAP_H(struct gameboy *gb, struct op *op);
void SWAP_L(struct gameboy *gb, struct op *op);
void SWAP_HLa(struct gameboy *gb, struct op *op);
void SWAP_A(struct gameboy *gb, struct op *op);

void SRL_B(struct gameboy *gb, struct op *op);
void SRL_C(struct gameboy *gb, struct op *op);
void SRL_D(struct gameboy *gb, struct op *op);
void SRL_E(struct gameboy *gb, struct op *op);
void SRL_H(struct gameboy *gb, struct op *op);
void SRL_L(struct gameboy *gb, struct op *op);
void SRL_HLa(struct gameboy *gb, struct op *op);
void SRL_A(struct gameboy *gb, struct op *op);

void SET_0_B(struct gameboy *gb, struct op *op);
void SET_0_C(struct gameboy *gb, struct op *op);
void SET_0_D(struct gameboy *gb, struct op *op);
void SET_0_E(struct gameboy *gb, struct op *op);
void SET_0_H(struct gameboy *gb, struct op *op);
void SET_0_L(struct gameboy *gb, struct op *op);
void SET_0_HLa(struct gameboy *gb, struct op *op);
void SET_0_A(struct gameboy *gb, struct op *op);

void SET_1_B(struct gameboy *gb, struct op *op);
void SET_1_C(struct gameboy *gb, struct op *op);
void SET_1_D(struct gameboy *gb, struct op *op);
void SET_1_E(struct gameboy *gb, struct op *op);
void SET_1_H(struct gameboy *gb, struct op *op);
void SET_1_L(struct gameboy *gb, struct op *op);
void SET_1_HLa(struct gameboy *gb, struct op *op);
void SET_1_A(struct gameboy *gb, struct op *op);

void SET_2_B(struct gameboy *gb, struct op *op);
void SET_2_C(struct gameboy *gb, struct op *op);
void SET_2_D(struct gameboy *gb, struct op *op);
void SET_2_E(struct gameboy *gb, struct op *op);
void SET_2_H(struct gameboy *gb, struct op *op);
void SET_2_L(struct gameboy *gb, struct op *op);
void SET_2_HLa(struct gameboy *gb, struct op *op);
void SET_2_A(struct gameboy *gb, struct op *op);

void SET_3_B(struct gameboy *gb, struct op *op);
void SET_3_C(struct gameboy *gb, struct op *op);
void SET_3_D(struct gameboy *gb, struct op *op);
void SET_3_E(struct gameboy *gb, struct op *op);
void SET_3_H(struct gameboy *gb, struct op *op);
void SET_3_L(struct gameboy *gb, struct op *op);
void SET_3_HLa(struct gameboy *gb, struct op *op);
void SET_3_A(struct gameboy *gb, struct op *op);

void SET_4_B(struct gameboy *gb, struct op *op);
void SET_4_C(struct gameboy *gb, struct op *op);
void SET_4_D(struct gameboy *gb, struct op *op);
void SET_4_E(struct gameboy *gb, struct op *op);
void SET_4_H(struct gameboy *gb, struct op *op);
void SET_4_L(struct gameboy *gb, struct op *op);
void SET_4_HLa(struct gameboy *gb, struct op *op);
void SET_4_A(struct gameboy *gb, struct op *op);

void SET_5_B(struct gameboy *gb, struct op *op);
void SET_5_C(struct gameboy *gb, struct op *op);
void SET_5_D(struct gameboy *gb, struct op *op);
void SET_5_E(struct gameboy *gb, struct op *op);
void SET_5_H(struct gameboy *gb, struct op *op);
void SET_5_L(struct gameboy *gb, struct op *op);
void SET_5_HLa(struct gameboy *gb, struct op *op);
void SET_5_A(struct gameboy *gb, struct op *op);

void SET_6_B(struct gameboy *gb, struct op *op);
void SET_6_C(struct gameboy *gb, struct op *op);
void SET_6_D(struct gameboy *gb, struct op *op);
void SET_6_E(struct gameboy *gb, struct op *op);
void SET_6_H(struct gameboy *gb, struct op *op);
void SET_6_L(struct gameboy *gb, struct op *op);
void SET_6_HLa(struct gameboy *gb, struct op *op);
void SET_6_A(struct gameboy *gb, struct op *op);

void SET_7_B(struct gameboy *gb, struct op *op);
void SET_7_C(struct gameboy *gb, struct op *op);
void SET_7_D(struct gameboy *gb, struct op *op);
void SET_7_E(struct gameboy *gb, struct op *op);
void SET_7_H(struct gameboy *gb, struct op *op);
void SET_7_L(struct gameboy *gb, struct op *op);
void SET_7_HLa(struct gameboy *gb, struct op *op);
void SET_7_A(struct gameboy *gb, struct op *op);

void BIT_0_B(struct gameboy *gb, struct op *op);
void BIT_0_C(struct gameboy *gb, struct op *op);
void BIT_0_D(struct gameboy *gb, struct op *op);
void BIT_0_E(struct gameboy *gb, struct op *op);
void BIT_0_H(struct gameboy *gb, struct op *op);
void BIT_0_L(struct gameboy *gb, struct op *op);
void BIT_0_HLa(struct gameboy *gb, struct op *op);
void BIT_0_A(struct gameboy *gb, struct op *op);

void BIT_1_B(struct gameboy *gb, struct op *op);
void BIT_1_C(struct gameboy *gb, struct op *op);
void BIT_1_D(struct gameboy *gb, struct op *op);
void BIT_1_E(struct gameboy *gb, struct op *op);
void BIT_1_H(struct gameboy *gb, struct op *op);
void BIT_1_L(struct gameboy *gb, struct op *op);
void BIT_1_HLa(struct gameboy *gb, struct op *op);
void BIT_1_A(struct gameboy *gb, struct op *op);

void BIT_2_B(struct gameboy *gb, struct op *op);
void BIT_2_C(struct gameboy *gb, struct op *op);
void BIT_2_D(struct gameboy *gb, struct op *op);
void BIT_2_E(struct gameboy *gb, struct op *op);
void BIT_2_H(struct gameboy *gb, struct op *op);
void BIT_2_L(struct gameboy *gb, struct op *op);
void BIT_2_HLa(struct gameboy *gb, struct op *op);
void BIT_2_A(struct gameboy *gb, struct op *op);

void BIT_3_B(struct gameboy *gb, struct op *op);
void BIT_3_C(struct gameboy *gb, struct op *op);
void BIT_3_D(struct gameboy *gb, struct op *op);
void BIT_3_E(struct gameboy *gb, struct op *op);
void BIT_3_H(struct gameboy *gb, struct op *op);
void BIT_3_L(struct gameboy *gb, struct op *op);
void BIT_3_HLa(struct gameboy *gb, struct op *op);
void BIT_3_A(struct gameboy *gb, struct op *op);

void BIT_4_B(struct gameboy *gb, struct op *op);
void BIT_4_C(struct gameboy *gb, struct op *op);
void BIT_4_D(struct gameboy *gb, struct op *op);
void BIT_4_E(struct gameboy *gb, struct op *op);
void BIT_4_H(struct gameboy *gb, struct op *op);
void BIT_4_L(struct gameboy *gb, struct op *op);
void BIT_4_HLa(struct gameboy *gb, struct op *op);
void BIT_4_A(struct gameboy *gb, struct op *op);

void BIT_5_B(struct gameboy *gb, struct op *op);
void BIT_5_C(struct gameboy *gb, struct op *op);
void BIT_5_D(struct gameboy *gb, struct op *op);
void BIT_5_E(struct gameboy *gb, struct op *op);
void BIT_5_H(struct gameboy *gb, struct op *op);
void BIT_5_L(struct gameboy *gb, struct op *op);
void BIT_5_HLa(struct gameboy *gb, struct op *op);
void BIT_5_A(struct gameboy *gb, struct op *op);

void BIT_6_B(struct gameboy *gb, struct op *op);
void BIT_6_C(struct gameboy *gb, struct op *op);
void BIT_6_D(struct gameboy *gb, struct op *op);
void BIT_6_E(struct gameboy *gb, struct op *op);
void BIT_6_H(struct gameboy *gb, struct op *op);
void BIT_6_L(struct gameboy *gb, struct op *op);
void BIT_6_HLa(struct gameboy *gb, struct op *op);
void BIT_6_A(struct gameboy *gb, struct op *op);

void BIT_7_B(struct gameboy *gb, struct op *op);
void BIT_7_C(struct gameboy *gb, struct op *op);
void BIT_7_D(struct gameboy *gb, struct op *op);
void BIT_7_E(struct gameboy *gb, struct op *op);
void BIT_7_H(struct gameboy *gb, struct op *op);
void BIT_7_L(struct gameboy *gb, struct op *op);
void BIT_7_HLa(struct gameboy *gb, struct op *op);
void BIT_7_A(struct gameboy *gb, struct op *op);


void RES_0_B(struct gameboy *gb, struct op *op);
void RES_0_C(struct gameboy *gb, struct op *op);
void RES_0_D(struct gameboy *gb, struct op *op);
void RES_0_E(struct gameboy *gb, struct op *op);
void RES_0_H(struct gameboy *gb, struct op *op);
void RES_0_L(struct gameboy *gb, struct op *op);
void RES_0_HLa(struct gameboy *gb, struct op *op);
void RES_0_A(struct gameboy *gb, struct op *op);

void RES_1_B(struct gameboy *gb, struct op *op);
void RES_1_C(struct gameboy *gb, struct op *op);
void RES_1_D(struct gameboy *gb, struct op *op);
void RES_1_E(struct gameboy *gb, struct op *op);
void RES_1_H(struct gameboy *gb, struct op *op);
void RES_1_L(struct gameboy *gb, struct op *op);
void RES_1_HLa(struct gameboy *gb, struct op *op);
void RES_1_A(struct gameboy *gb, struct op *op);

void RES_2_B(struct gameboy *gb, struct op *op);
void RES_2_C(struct gameboy *gb, struct op *op);
void RES_2_D(struct gameboy *gb, struct op *op);
void RES_2_E(struct gameboy *gb, struct op *op);
void RES_2_H(struct gameboy *gb, struct op *op);
void RES_2_L(struct gameboy *gb, struct op *op);
void RES_2_HLa(struct gameboy *gb, struct op *op);
void RES_2_A(struct gameboy *gb, struct op *op);

void RES_3_B(struct gameboy *gb, struct op *op);
void RES_3_C(struct gameboy *gb, struct op *op);
void RES_3_D(struct gameboy *gb, struct op *op);
void RES_3_E(struct gameboy *gb, struct op *op);
void RES_3_H(struct gameboy *gb, struct op *op);
void RES_3_L(struct gameboy *gb, struct op *op);
void RES_3_HLa(struct gameboy *gb, struct op *op);
void RES_3_A(struct gameboy *gb, struct op *op);

void RES_4_B(struct gameboy *gb, struct op *op);
void RES_4_C(struct gameboy *gb, struct op *op);
void RES_4_D(struct gameboy *gb, struct op *op);
void RES_4_E(struct gameboy *gb, struct op *op);
void RES_4_H(struct gameboy *gb, struct op *op);
void RES_4_L(struct gameboy *gb, struct op *op);
void RES_4_HLa(struct gameboy *gb, struct op *op);
void RES_4_A(struct gameboy *gb, struct op *op);

void RES_5_B(struct gameboy *gb, struct op *op);
void RES_5_C(struct gameboy *gb, struct op *op);
void RES_5_D(struct gameboy *gb, struct op *op);
void RES_5_E(struct gameboy *gb, struct op *op);
void RES_5_H(struct gameboy *gb, struct op *op);
void RES_5_L(struct gameboy *gb, struct op *op);
void RES_5_HLa(struct gameboy *gb, struct op *op);
void RES_5_A(struct gameboy *gb, struct op *op);

void RES_6_B(struct gameboy *gb, struct op *op);
void RES_6_C(struct gameboy *gb, struct op *op);
void RES_6_D(struct gameboy *gb, struct op *op);
void RES_6_E(struct gameboy *gb, struct op *op);
void RES_6_H(struct gameboy *gb, struct op *op);
void RES_6_L(struct gameboy *gb, struct op *op);
void RES_6_HLa(struct gameboy *gb, struct op *op);
void RES_6_A(struct gameboy *gb, struct op *op);

void RES_7_B(struct gameboy *gb, struct op *op);
void RES_7_C(struct gameboy *gb, struct op *op);
void RES_7_D(struct gameboy *gb, struct op *op);
void RES_7_E(struct gameboy *gb, struct op *op);
void RES_7_H(struct gameboy *gb, struct op *op);
void RES_7_L(struct gameboy *gb, struct op *op);
void RES_7_HLa(struct gameboy *gb, struct op *op);
void RES_7_A(struct gameboy *gb, struct op *op);

