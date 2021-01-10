#include "CPU.h"
#include "GameBoy.h"

CPU::CPU()
{
	// Init registers
	regs.af = 0x01B0;
	regs.bc = 0x0013;
	regs.de = 0x00D8;
	regs.hl = 0x014D;

	sp = 0xFFFE;
	pc = 0x0000;

	opcode = 0x00;

	HALT = false;

	cycles = 0;
	timerDiv = 0;
	timaCounter = 0;
	currentSpeed = 1024;

	frame_complete = false;

	// Opcode lookup table
	using a = CPU;
	lookup = 
	{ 
		{"NOP", &a::nop, 1},{"LD BC,d16", &a::ld_bc_nn, 3},{"LD (BC),A", &a::ld_bc_a, 1},{"INC BC", &a::inc_bc, 1},{"INC B", &a::inc_b, 1},{"DEC B", &a::dec_b, 1},{"LD B,d8", &a::ld_b_n, 2},{"RLCA", &a::rlca, 1},{"LD (a16),SP", &a::ld_nn_sp, 3},{"ADD HL,BC", &a::add_hl_bc, 1},{"LD A,(BC)", &a::ld_a_bc, 1},{"DEC BC", &a::dec_bc, 1},{"INC C", &a::inc_c, 1},{"DEC C", &a::dec_c, 1},{"LD C,d8", &a::ld_c_n, 2},{"RRCA", &a::rrca, 1},
		{"STOP 0", &a::undefined, 2},{"LD DE,d16", &a::ld_de_nn, 3},{"LD (DE),A", &a::ld_de_a, 1},{"INC DE", &a::inc_de, 1},{"INC D", &a::inc_d, 1},{"DEC D", &a::dec_d, 1},{"LD D,d8", &a::ld_d_n, 2},{"RLA", &a::rla, 1},{"JR r8", &a::jr_n, 2},{"ADD HL,DE", &a::add_hl_de, 1},{"LD A,(DE)", &a::ld_a_de, 1},{"DEC DE", &a::dec_de, 1},{"INC E", &a::inc_e, 1},{"DEC E", &a::dec_e, 1},{"LD E,d8", &a::ld_e_n, 2},{"RRA", &a::rra, 1}, 
		{"JR NZ r8", &a::jr_nz_n, 2},{"LD HL,d16", &a::ld_hl_nn, 3},{"LD (HL+),A", &a::ldi_hl, 1},{"INC HL", &a::inc_hl, 1},{"INC H", &a::inc_h, 1},{"DEC H", &a::dec_h, 1},{"LD H,d8", &a::ld_h_n, 2},{"DAA", &a::daa, 1},{"JR Z r8", &a::jr_z_n, 2},{"ADD HL,HL", &a::add_hl_hl, 1},{"LD A,(HL+)", &a::ldi_a, 1},{"DEC HL", &a::dec_hl, 1},{"INC L", &a::inc_l, 1},{"DEC L", &a::dec_l, 1},{"LD L,d8", &a::ld_l_n, 2},{"CPL", &a::cpl, 1}, 
		{"JR NC r8", &a::jr_nc, 2},{"LD SP,d16", &a::ld_sp_nn, 3},{"LD (HL-),A", &a::ldd_hl, 1},{"INC SP", &a::inc_sp, 1},{"INC (HL)", &a::inc_hla, 1},{"DEC (HL)", &a::dec_hla, 1},{"LD (HL),d8", &a::ld_hl_n, 2},{"SCF", &a::scf, 1},{"JR C r8", &a::jr_c_n, 2},{"ADD HL,SP", &a::add_hl_sp, 1},{"LD A,(HL-)", &a::ldd_a, 1},{"DEC SP", &a::dec_sp, 1},{"INC A", &a::inc_a, 1},{"DEC A", &a::dec_a, 1},{"LD A,d8", &a::ld_a_n, 2},{"CCF", &a::ccf, 1}, 
		{"LD B,B", &a::ld_b_b, 1},{"LD B,C", &a::ld_b_c, 1},{"LD B,D", &a::ld_b_d, 1},{"LD B,E", &a::ld_b_e, 1},{"LD B,H", &a::ld_b_h, 1},{"LD B,L", &a::ld_b_l, 1},{"LD B,(HL)", &a::ld_b_hl, 1},{"LD B,A", &a::ld_b_a, 1},{"LD C,B", &a::ld_c_b, 1},{"LD C,C", &a::ld_c_c, 1},{"LD C,D", &a::ld_c_d, 1},{"LD C,E", &a::ld_c_e, 1},{"LD C,H", &a::ld_c_h, 1},{"LD C,L", &a::ld_c_l, 1},{"LD C,(HL)", &a::ld_c_hl, 1},{"LD C,A", &a::ld_c_a, 1}, 
		{"LD D,B", &a::ld_d_b, 1},{"LD D,C", &a::ld_d_c, 1},{"LD D,D", &a::ld_d_d, 1},{"LD D,E", &a::ld_d_e, 1},{"LD D,H", &a::ld_d_h, 1},{"LD D,L", &a::ld_d_l, 1},{"LD D,(HL)", &a::ld_d_hl, 1},{"LD D,A", &a::ld_d_a, 1},{"LD E,B", &a::ld_e_b, 1},{"LD E,C", &a::ld_e_c, 1},{"LD E,D", &a::ld_e_d, 1},{"LD E,E", &a::ld_e_e, 1},{"LD E,H", &a::ld_e_h, 1},{"LD E,L", &a::ld_e_l, 1},{"LD E,(HL)", &a::ld_e_hl, 1},{"LD E,A", &a::ld_e_a, 1}, 
		{"LD H,B", &a::ld_h_b, 1},{"LD H,C", &a::ld_h_c, 1},{"LD H,D", &a::ld_h_d, 1},{"LD H,E", &a::ld_h_e, 1},{"LD H,H", &a::ld_h_h, 1},{"LD H,L", &a::ld_h_l , 1},{"LD H,(HL)", &a::ld_h_hl, 1},{"LD H,A", &a::ld_h_a, 1},{"LD L,B", &a::ld_l_b, 1},{"LD L,C", &a::ld_l_c, 1},{"LD L,D", &a::ld_l_d, 1},{"LD L,E", &a::ld_l_e, 1},{"LD L,H", &a::ld_l_h, 1},{"LD L,L", &a::ld_l_l , 1},{"LD L,(HL)", &a::ld_l_hl, 1},{"LD L,A", &a::ld_l_a, 1},
		{"LD (HL),B", &a::ld_hl_b, 1},{"LD (HL),C", &a::ld_hl_c, 1},{"LD (HL),D", &a::ld_hl_d, 1},{"LD (HL),E", &a::ld_hl_e, 1},{"LD (HL),H", &a::ld_hl_h, 1},{"LD (HL),L", &a::ld_hl_l, 1},{"HALT", &a::halt , 1},{"LD (HL),A", &a::ld_hl_a, 1},{"LD A,B", &a::ld_a_b, 1},{"LD A,C", &a::ld_a_c, 1},{"LD A,D", &a::ld_a_d, 1},{"LD A,E", &a::ld_a_e, 1},{"LD A,H", &a::ld_a_h, 1},{"LD A,L", &a::ld_a_l, 1},{"LD A,(HL)", &a::ld_a_hl, 1},{"LD A,A", &a::ld_a_a, 1},
		{"ADD A,B", &a::add_b, 1},{"ADD A,C", &a::add_c, 1},{"ADD A,D", &a::add_d, 1},{"ADD A,E", &a::add_e, 1},{"ADD A,H", &a::add_h, 1},{"ADD A,L", &a::add_l, 1},{"ADD A,(HL)", &a::add_hl, 1},{"ADD A,A", &a::add_a, 1},{"ADC A,B", &a::adc_b, 1},{"ADC A,C", &a::adc_c, 1},{"ADC A,D", &a::adc_d, 1},{"ADC A,E", &a::adc_e, 1},{"ADC A,H", &a::adc_h, 1},{"ADC A,L", &a::adc_l, 1},{"ADC A,(HL)", &a::adc_hl, 1},{"ADC A,A", &a::adc_a, 1},
		{"SUB B", &a::sub_b, 1},{"SUB C", &a::sub_c, 1},{"SUB D", &a::sub_d, 1},{"SUB E", &a::sub_e, 1},{"SUB H", &a::sub_h, 1},{"SUB L", &a::sub_l, 1},{"SUB (HL)", &a::sub_hl, 1},{"SUB A", &a::sub_a, 1},{"SBC A,B", &a::sbc_b, 1},{"SBC A,C", &a::sbc_c, 1},{"SBC A,D", &a::sbc_d, 1},{"SBC A,E", &a::sbc_e, 1},{"SBC A,H", &a::sbc_h, 1},{"SBC A,L", &a::sbc_l, 1},{"SBC A,(HL)", &a::sbc_hl, 1},{"SBC A,A", &a::sbc_a, 1},
		{"AND B", &a::and_b, 1},{"AND C", &a::and_c, 1},{"AND D", &a::and_d, 1},{"AND E", &a::and_e, 1},{"AND H", &a::and_h, 1},{"AND L", &a::and_l, 1},{"AND (HL)", &a::and_hl, 1},{"AND A", &a::and_a, 1},{"XOR B", &a::xor_b, 1},{"XOR C", &a::xor_c, 1},{"XOR D", &a::xor_d, 1},{"XOR E", &a::xor_e, 1},{"XOR H", &a::xor_h, 1},{"XOR L", &a::xor_l, 1},{"XOR (HL)", &a::xor_hl, 1},{"XOR A", &a::xor_a, 1},
		{"OR B", &a::or_b, 1},{"OR C", &a::or_c, 1},{"OR D", &a::or_d, 1},{"OR E", &a::or_e, 1},{"OR H", &a::or_h, 1},{"OR L", &a::or_l, 1},{"OR (HL)", &a::or_hl, 1},{"OR A", &a::or_a, 1},{"CP B", &a::cp_b, 1},{"CP C", &a::cp_c, 1},{"CP D", &a::cp_d, 1},{"CP E", &a::cp_e, 1},{"CP H", &a::cp_h, 1},{"CP L", &a::cp_l, 1},{"CP (HL)", &a::cp_hl, 1},{"CP A", &a::cp_a, 1},
		{"RET NZ", &a::ret_nz, 1},{"POP BC", &a::pop_bc, 1},{"JP NZ,a16", &a::jp_nz, 3},{"JP a16", &a::jp_nn, 3},{"CALL NZ,a16", &a::call_nz, 3},{"PUSH BC", &a::push_bc, 1},{"ADD A,d8", &a::add_n, 2},{"RST 00H", &a::rst_0, 1},{"RET Z", &a::ret_z, 1},{"RET", &a::ret, 1},{"JP Z,a16", &a::jp_z, 3},{"PREFIX CB", &a::cb, 1},{"CALL Z,a16", &a::call_z, 3},{"CALL a16", &a::call_nn, 3},{"ADC A,d8", &a::adc_n, 2},{"RST 08H", &a::rst_8, 1},
		{"RET NC", &a::ret_nc, 1},{"POP DE", &a::pop_de, 1},{"JP NC,a16", &a::jp_nc, 3},{"NOP", &a::nop, 1},{"CALL NC,a16", &a::call_nc, 3},{"PUSH DE", &a::push_de, 1},{"SUB d8", &a::sub_n, 2},{"RST 10H", &a::rst_10, 1},{"RET C", &a::ret_c, 1},{"RETI", &a::reti, 1},{"JP C,a16", &a::jp_c, 3},{"NOP", &a::nop, 1},{"CALL C,a16", &a::call_c, 1},{"NOP", &a::nop, 1},{"SBC A,d8", &a::sbc_n, 2},{"RST 18H", &a::rst_18, 1},
		{"LDH (a8),A", &a::ldh_n_a, 2},{"POP HL", &a::pop_hl, 1},{"LD(C),A", &a::ld_ff_c_a, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"PUSH HL", &a::push_hl, 1},{"AND d8", &a::and_n, 2},{"RST 20H", &a::rst_20, 1},{"ADD SP,r8", &a::add_sp_n, 2},{"JP (HL)", &a::jp_hl, 1},{"LD (a16),A", &a::ld_nn_a, 3},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"XOR d8", &a::xor_n, 2},{"RST 28H", &a::rst_28, 1},
		{"LDH A,(a8)", &a::ldh_a_n, 2},{"POP AF", &a::pop_af, 1},{"LD A,(C)", &a::ld_a_ff_c, 1},{"DI", &a::di, 1},{"NOP", &a::undefined, 1},{"PUSH AF", &a::push_af, 1},{"OR d8", &a::or_n, 2},{"RST 30H", &a::rst_30, 1},{"LD HL,SP+r8", &a::ld_hl_sp_n, 2},{"LD SP,HL", &a::ld_sp_hl, 1},{"LD A,(a16)", &a::ld_a_nn, 3},{"EI", &a::ei, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"CP d8", &a::cp_n, 2},{"RST 38H", &a::rst_38, 1}, 
	};

	extended_lookup =
	{
		{"RLC B", &a::rlc_b, 1},{"RLC C", &a::rlc_c, 1},{"RLC D", &a::rlc_d, 1},{"RLC E", &a::rlc_e, 1},{"RLC H", &a::rlc_h, 1},{"RLC L", &a::rlc_l, 1},{"RLC (HL)", &a::rlc_hl, 1},{"RLC A", &a::rlc_a, 1},{"RRC B", &a::rrc_b, 1},{"RRC C", &a::rrc_c, 1},{"RRC D", &a::rrc_d, 1},{"RRC E", &a::rrc_e, 1},{"RRC H", &a::rrc_h, 1},{"RRC L", &a::rrc_l, 1},{"RRC (HL)", &a::rrc_hl, 1},{"RRC A", &a::rrc_a, 1},
		{"RL B", &a::rl_b, 1},{"RL C", &a::rl_c, 1},{"RL D", &a::rl_d, 1},{"RL E", &a::rl_e, 1},{"RL H", &a::rl_h, 1},{"RL L", &a::rl_l, 1},{"RL (HL)", &a::rl_hl, 1},{"RL A", &a::rl_a, 1},{"RR B", &a::rr_b, 1},{"RR C", &a::rr_c, 1},{"RR D", &a::rr_d, 1},{"RR E", &a::rr_e, 1},{"RR H", &a::rr_h, 1},{"RR L", &a::rr_l, 1},{"RR (HL)", &a::rr_hl, 1},{"RR A", &a::rr_a, 1},
		{"SLA B", &a::sla_b, 1},{"SLA C", &a::sla_c , 1},{"SLA D", &a::sla_d, 1},{"SLA E", &a::sla_e, 1},{"SLA H", &a::sla_h, 1},{"SLA L", &a::sla_l, 1},{"SLA (HL)", &a::sla_hl, 1},{"SLA A", &a::sla_a, 1},{"SRA B", &a::sra_b, 1},{"SRA C", &a::sra_c, 1},{"SRA D", &a::sra_d, 1},{"SRA E", &a::sra_e, 1},{"SRA H", &a::sra_h, 1},{"SRA L", &a::sra_l, 1},{"SRA (HL)", &a::sra_hl, 1},{"SRA A", &a::sra_a, 1},
		{"SWAP B", &a::swap_b, 1},{"SWAP C", &a::swap_c, 1},{"SWAP D", &a::swap_d, 1},{"SWAP E", &a::swap_e, 1},{"SWAP H", &a::swap_h, 1},{"SWAP L", &a::swap_l, 1},{"SWAP (HL)", &a::swap_hl, 1},{"SWAP A", &a::swap_a, 1},{"SRL B", &a::srl_b , 1},{"SRL C", &a::srl_c, 1},{"SRL D", &a::srl_d, 1},{"SRL E", &a::srl_e, 1},{"SRL H", &a::srl_h, 1},{"SRL L", &a::srl_l, 1},{"SRL (HL)", &a::srl_hl, 1},{"SRL A", &a::srl_a, 1},
		{"BIT 0,B", &a::bit_0_b, 1},{"BIT 0,C", &a::bit_0_c, 1},{"BIT 0,D", &a::bit_0_d, 1},{"BIT 0,E", &a::bit_0_e, 1},{"BIT 0,H", &a::bit_0_h, 1},{"BIT 0,L", &a::bit_0_l, 1},{"BIT 0,(HL)", &a::bit_0_hl, 1},{"BIT 0,A", &a::bit_0_a, 1},{"BIT 1,B", &a::bit_1_b, 1},{"BIT 1,C", &a::bit_1_c, 1},{"BIT 1,D", &a::bit_1_d, 1},{"BIT 1,E", &a::bit_1_e, 1},{"BIT 1,H", &a::bit_1_h, 1},{"BIT 1,L", &a::bit_1_l, 1},{"BIT 1,(HL)", &a::bit_1_hl, 1},{"BIT 1,A", &a::bit_1_a, 1},
		{"BIT 2,B", &a::bit_2_b, 1},{"BIT 2,C", &a::bit_2_c, 1},{"BIT 2,D", &a::bit_2_d, 1},{"BIT 2,E", &a::bit_2_e, 1},{"BIT 2,H", &a::bit_2_h, 1},{"BIT 2,L", &a::bit_2_l, 1},{"BIT 2,(HL)", &a::bit_2_hl, 1},{"BIT 2,A", &a::bit_2_a, 1},{"BIT 3,B", &a::bit_3_b, 1},{"BIT 3,C", &a::bit_3_c, 1},{"BIT 3,D", &a::bit_3_d, 1},{"BIT 3,E", &a::bit_3_e, 1},{"BIT 3,H", &a::bit_3_h, 1},{"BIT 3,L", &a::bit_3_l, 1},{"BIT 3,(HL)", &a::bit_3_hl, 1},{"BIT 3,A", &a::bit_3_a, 1},
		{"BIT 4,B", &a::bit_4_b, 1},{"BIT 4,C", &a::bit_4_c, 1},{"BIT 4,D", &a::bit_4_d, 1},{"BIT 4,E", &a::bit_4_e, 1},{"BIT 4,H", &a::bit_4_h, 1},{"BIT 4,L", &a::bit_4_l, 1},{"BIT 4,(HL)", &a::bit_4_hl, 1},{"BIT 4,A", &a::bit_4_a, 1},{"BIT 5,B", &a::bit_5_b, 1},{"BIT 5,C", &a::bit_5_c, 1},{"BIT 5,D", &a::bit_5_d, 1},{"BIT 5,E", &a::bit_5_e, 1},{"BIT 5,H", &a::bit_5_h, 1},{"BIT 5,L", &a::bit_5_l, 1},{"BIT 5,(HL)", &a::bit_5_hl, 1},{"BIT 5,A", &a::bit_5_a, 1},
		{"BIT 6,B", &a::bit_6_b, 1},{"BIT 6,C", &a::bit_6_c, 1},{"BIT 6,D", &a::bit_6_d, 1},{"BIT 6,E", &a::bit_6_e, 1},{"BIT 6,H", &a::bit_6_h, 1},{"BIT 6,L", &a::bit_6_l, 1},{"BIT 6,(HL)", &a::bit_6_hl, 1},{"BIT 6,A", &a::bit_6_a, 1},{"BIT 7,B", &a::bit_7_b, 1},{"BIT 7,C", &a::bit_7_c, 1},{"BIT 7,D", &a::bit_7_d, 1},{"BIT 7,E", &a::bit_7_e, 1},{"BIT 7,H", &a::bit_7_h, 1},{"BIT 7,L", &a::bit_7_l, 1},{"BIT 7,(HL)", &a::bit_7_hl, 1},{"BIT 7,A", &a::bit_7_a, 1},
		{"RES 0,B", &a::res_0_b, 1},{"RES 0,C", &a::res_0_c, 1},{"RES 0,D", &a::res_0_d, 1},{"RES 0,E", &a::res_0_e, 1},{"RES 0,H", &a::res_0_h, 1},{"RES 0,L", &a::res_0_l, 1},{"RES 0,(HL)", &a::res_0_hl, 1},{"RES 0,A", &a::res_0_a, 1},{"RES 1,B", &a::res_1_b, 1},{"RES 1,C", &a::res_1_c, 1},{"RES 1,D", &a::res_1_d, 1},{"RES 1,E", &a::res_1_e, 1},{"RES 1,H", &a::res_1_h, 1},{"RES 1,L", &a::res_1_l, 1},{"RES 1,(HL)", &a::res_1_hl, 1},{"RES 1,A", &a::res_1_a, 1},
		{"RES 2,B", &a::res_2_b, 1},{"RES 2,C", &a::res_2_c, 1},{"RES 2,D", &a::res_2_d, 1},{"RES 2,E", &a::res_2_e, 1},{"RES 2,H", &a::res_2_h, 1},{"RES 2,L", &a::res_2_l, 1},{"RES 2,(HL)", &a::res_2_hl, 1},{"RES 2,A", &a::res_2_a, 1},{"RES 3,B", &a::res_3_b, 1},{"RES 3,C", &a::res_3_c, 1},{"RES 3,D", &a::res_3_d, 1},{"RES 3,E", &a::res_3_e, 1},{"RES 3,H", &a::res_3_h, 1},{"RES 3,L", &a::res_3_l, 1},{"RES 3,(HL)", &a::res_3_hl, 1},{"RES 3,A", &a::res_3_a, 1},
		{"RES 4,B", &a::res_4_b, 1},{"RES 4,C", &a::res_4_c, 1},{"RES 4,D", &a::res_4_d, 1},{"RES 4,E", &a::res_4_e, 1},{"RES 4,H", &a::res_4_h, 1},{"RES 4,L", &a::res_4_l, 1},{"RES 4,(HL)", &a::res_4_hl, 1},{"RES 4,A", &a::res_4_a, 1},{"RES 5,B", &a::res_5_b, 1},{"RES 5,C", &a::res_5_c, 1},{"RES 5,D", &a::res_5_d, 1},{"RES 5,E", &a::res_5_e, 1},{"RES 5,H", &a::res_5_h, 1},{"RES 5,L", &a::res_5_l, 1},{"RES 5,(HL)", &a::res_5_hl, 1},{"RES 5,A", &a::res_5_a, 1},
		{"RES 6,B", &a::res_6_b, 1},{"RES 6,C", &a::res_6_c, 1},{"RES 6,D", &a::res_6_d, 1},{"RES 6,E", &a::res_6_e, 1},{"RES 6,H", &a::res_6_h, 1},{"RES 6,L", &a::res_6_l, 1},{"RES 6,(HL)", &a::res_6_hl, 1},{"RES 6,A", &a::res_6_a, 1},{"RES 7,B", &a::res_7_b, 1},{"RES 7,C", &a::res_7_c, 1},{"RES 7,D", &a::res_7_d, 1},{"RES 7,E", &a::res_7_e, 1},{"RES 7,H", &a::res_7_h, 1},{"RES 7,L", &a::res_7_l, 1},{"RES 7,(HL)", &a::res_7_hl, 1},{"RES 7,A", &a::res_7_a, 1},
		{"SET 0,B", &a::set_0_b, 1},{"SET 0,C", &a::set_0_c, 1},{"SET 0,D", &a::set_0_d, 1},{"SET 0,E", &a::set_0_e, 1},{"SET 0,H", &a::set_0_h, 1},{"SET 0,L", &a::set_0_l, 1},{"SET 0,(HL)", &a::set_0_hl, 1},{"SET 0,A", &a::set_0_a, 1},{"SET 1,B", &a::set_1_b, 1},{"SET 1,C", &a::set_1_c, 1},{"SET 1,D", &a::set_1_d, 1},{"SET 1,E", &a::set_1_e, 1},{"SET 1,H", &a::set_1_h, 1},{"SET 1,L", &a::set_1_l, 1},{"SET 1,(HL)", &a::set_1_hl, 1},{"SET 1,A", &a::set_1_a, 1},
		{"SET 2,B", &a::set_2_b, 1},{"SET 2,C", &a::set_2_c, 1},{"SET 2,D", &a::set_2_d, 1},{"SET 2,E", &a::set_2_e, 1},{"SET 2,H", &a::set_2_h, 1},{"SET 2,L", &a::set_2_l, 1},{"SET 2,(HL)", &a::set_2_hl, 1},{"SET 2,A", &a::set_2_a, 1},{"SET 3,B", &a::set_3_b, 1},{"SET 3,C", &a::set_3_c, 1},{"SET 3,D", &a::set_3_d, 1},{"SET 3,E", &a::set_3_e, 1},{"SET 3,H", &a::set_3_h, 1},{"SET 3,L", &a::set_3_l, 1},{"SET 3,(HL)", &a::set_3_hl, 1},{"SET 3,A", &a::set_3_a, 1},
		{"SET 4,B", &a::set_4_b, 1},{"SET 4,C", &a::set_4_c, 1},{"SET 4,D", &a::set_4_d, 1},{"SET 4,E", &a::set_4_e, 1},{"SET 4,H", &a::set_4_h, 1},{"SET 4,L", &a::set_4_l, 1},{"SET 4,(HL)", &a::set_4_hl, 1},{"SET 4,A", &a::set_4_a, 1},{"SET 5,B", &a::set_5_b, 1},{"SET 5,C", &a::set_5_c, 1},{"SET 5,D", &a::set_5_d, 1},{"SET 5,E", &a::set_5_e, 1},{"SET 5,H", &a::set_5_h, 1},{"SET 5,L", &a::set_5_l , 1},{"SET 5,(HL)", &a::set_5_hl, 1},{"SET 5,A", &a::set_5_a, 1},
		{"SET 6,B", &a::set_6_b, 1},{"SET 6,C", &a::set_6_c, 1},{"SET 6,D", &a::set_6_d, 1},{"SET 6,E", &a::set_6_e, 1},{"SET 6,H", &a::set_6_h, 1},{"SET 6,L", &a::set_6_l, 1},{"SET 6,(HL)", &a::set_6_hl, 1},{"SET 6,A", &a::set_6_a, 1},{"SET 7,B", &a::set_7_b, 1},{"SET 7,C", &a::set_7_c, 1},{"SET 7,D", &a::set_7_d, 1},{"SET 7,E", &a::set_7_e, 1},{"SET 7,H", &a::set_7_h, 1},{"SET 7,L", &a::set_7_l , 1},{"SET 7,(HL)", &a::set_7_hl, 1},{"SET 7,A", &a::set_7_a, 1},
	};
}

void CPU::dump_regs()
{
	// Decrement PC
	--pc;

	std::cout << "===========================" << std::endl;
	std::cout << "PC: " << std::hex << pc << " | OP: " << std::hex << (unsigned int)opcode << std::endl;
	std::cout << "A: " << std::hex << (unsigned int)regs.a << std::endl;
	std::cout << "F: " << std::hex << (unsigned int)regs.f << std::endl;
	std::cout << "B: " << std::hex << (unsigned int)regs.b << std::endl;
	std::cout << "C: " << std::hex << (unsigned int)regs.c << std::endl;
	std::cout << "D: " << std::hex << (unsigned int)regs.d << std::endl;
	std::cout << "E: " << std::hex << (unsigned int)regs.e << std::endl;
	std::cout << "H: " << std::hex << (unsigned int)regs.h << std::endl;
	std::cout << "L: " << std::hex << (unsigned int)regs.l << std::endl;
	std::cout << "SP: " << std::hex << sp << std::endl;

	++pc;
}

// Function that generates a log exactly like wheremyfoodat's logs for easy diff checking
void CPU::log()
{
	std::cout << "A: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.a << " ";
	std::cout << "F: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.f << " ";
	std::cout << "B: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.b << " ";
	std::cout << "C: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.c << " ";
	std::cout << "D: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.d << " ";
	std::cout << "E: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.e << " ";
	std::cout << "H: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.h << " ";
	std::cout << "L: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)regs.l << " ";
	std::cout << "SP: " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << sp << " ";
	std::cout << "PC: 00:" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << pc << " ";
	std::cout << "(" << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)gb->mmu.readByte(pc) << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)gb->mmu.readByte(pc + 1) << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)gb->mmu.readByte(pc + 2) << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (unsigned int)gb->mmu.readByte(pc + 3) << ")" << std::endl;
}

void CPU::handle_interrupts()
{
	uint8_t fired = gb->mmu.readByte(IF);
	uint8_t ie = gb->mmu.readByte(IE);
	if (interrupts_enabled)
	{ 

		//HALT = false;

		// Check if there's any interrupts
		if (fired & ie)
		{
			interrupts_enabled = false;

			HALT = false;

			// VBlank
			if ((fired & 0x01) & (ie & 0x01))
			{
				push(pc);
				jp(VBLANK_ISR);
				gb->mmu.writeByte(0xFF0f, (fired & ~0x01));
			}

			// LCD Status
			if ((fired & 0x02) & (ie & 0x02))
			{
				push(pc);
				jp(LCD_STATUS_ISR);
				gb->mmu.writeByte(0xFF0F, (fired & ~0x02));
			}

			// TIMER
			if ((fired & 0x04) & (ie & 0x04))
			{
				push(pc);
				jp(TIMER_ISR);
				gb->mmu.writeByte(0xFF0F, (fired & ~0x04));
			}

			// SERIAL
			if ((fired & 0x08) & (ie & 0x08))
			{
				push(pc);
				jp(SERIAL_ISR);
				gb->mmu.writeByte(0xFF0F, (fired & ~0x04));
			}

			// JOYPAD
			if ((fired & 0x10) & (ie & 0x10))
			{
				push(pc);
				jp(JOYPAD_ISR);
				gb->mmu.writeByte(0xFF0F, (fired & ~0x10));
			}
		}
	}
	else // Handles the bug with halting and interrupts being disabled
	{
		if (fired & ie) HALT = false;
	}
}

void CPU::handle_timer()
{
	timerDiv += cycles;

	uint8_t timerAttrs = gb->mmu.readByte(0xFF07);
	if ((timerAttrs >> 2) & 0x1)
	{
		timaCounter += cycles * 4;
		while (timaCounter >= currentSpeed)
		{
			timaCounter -= currentSpeed;

			if (gb->mmu.readByte(0xFF05) == 0xFF)
			{
				gb->mmu.writeByte(0xFF05, gb->mmu.readByte(0xFF06));
				gb->mmu.writeByte(0xFF0F, (gb->mmu.readByte(0xFF0F) | 0x04)); // Trigger interrupt
			}
			else
			{
				gb->mmu.writeByte(0xFF05, (gb->mmu.readByte(0xFF05) + 1));
			}
		}
	}

	if (timerDiv >= 256)
	{
		timerDiv -= 256;
		gb->mmu.writeByte(0xFF04, (gb->mmu.readByte(0xFF04) + 1), true);
	}
}

uint8_t CPU::inc(uint8_t value)
{
	(value & 0x0F) == 0x0F ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	++value;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE);

	return value;
}

uint8_t CPU::dec(uint8_t value)
{
	(value & 0x0F) == 0x00 ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	--value;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_SET(FLAGS_NEGATIVE);

	return value;
}

void CPU::add(uint8_t value)
{
	((regs.a & 0x0F) + (value & 0x0F)) > 0x0F ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	uint16_t result = regs.a + value;
	result & 0xFF00 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a = (uint8_t)(result & 0x00FF);
	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

void CPU::adc(uint8_t value)
{
	uint8_t carry = FLAGS_ISCARRY ? 1 : 0;
	((regs.a & 0x0F) + (value & 0x0F) + (carry & 0x0F)) > 0x0F ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	uint16_t result = regs.a + value + carry;
	result & 0xFF00 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a = (uint8_t)(result & 0x00FF);
	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

void CPU::sub(uint8_t value)
{
	value > regs.a ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	(value & 0x0F) > (regs.a & 0x0F) ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	regs.a -= value;
	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_SET(FLAGS_NEGATIVE);
}

void CPU::sbc(uint8_t value)
{
	uint8_t carry = FLAGS_ISCARRY ? 0 : 1;
	value = ~value;
	((regs.a & 0x0F) + (value & 0x0F) + (carry & 0x0F)) > 0x0F ? FLAGS_CLEAR(FLAGS_HALFCARRY) : FLAGS_SET(FLAGS_HALFCARRY);
	uint16_t result = regs.a + value + carry;
	result & 0xFF00 ? FLAGS_CLEAR(FLAGS_CARRY) : FLAGS_SET(FLAGS_CARRY);
	regs.a = (uint8_t)(result & 0x00FF);
	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_SET(FLAGS_NEGATIVE);
}

void CPU::AND(uint8_t value)
{
	regs.a &= value;
	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_SET(FLAGS_HALFCARRY);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_CARRY);
}

void CPU::XOR(uint8_t value)
{
	regs.a ^= value;

	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY | FLAGS_CARRY);
}

void CPU::OR(uint8_t value)
{
	regs.a |= value;

	regs.a == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY | FLAGS_CARRY);
}

void CPU::cp(uint8_t value)
{
	(value == regs.a) ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	(value > regs.a) ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	((value & 0x0F) > (regs.a & 0x0F)) ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	FLAGS_SET(FLAGS_NEGATIVE);
}

void CPU::add16(uint16_t value)
{
	((regs.hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	uint32_t result = regs.hl + value;
	result & 0xFFFF0000 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.hl = (uint16_t)(result & 0x0000FFFF);
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

void CPU::jp(uint16_t addr)
{
	pc = addr;
}

void CPU::jr()
{
	pc += (int8_t)(gb->mmu.readByte(pc)) + 1;
}

void CPU::call(uint16_t addr)
{
	push(pc + 2);
	jp(addr);
}

void CPU::push(uint16_t word)
{
	sp -= 2;
	gb->mmu.writeShort(sp, word);
}

uint16_t CPU::pop()
{
	uint16_t value = gb->mmu.readShort(sp);
	sp += 2;

	return value;
}

void CPU::undefined()
{
#ifndef LOGGING
	instruction i = lookup[gb->mmu.readByte(pc - 1)];
	std::cout << "Undefined instruction: " << i.name;
	switch (i.length)
	{
		case 2:
			std::cout << ", ARGS: 0x" << std::setfill('0') << std::hex << std::setw(2) << (unsigned int) gb->mmu.readByte(pc) << std::endl;
			break;
		case 3:
			std::cout << ", ARGS: 0x" << std::setfill('0') << std::hex << std::setw(4) << gb->mmu.readShort(pc) << std::endl;
			break;
		default:
			std::cout << std::endl;
	}
	dump_regs();
#endif
	exit(1);
}

void CPU::undefined_cb()
{
	instruction i = extended_lookup[gb->mmu.readByte(pc - 1)];
	std::cout << "Undefined instruction: " << i.name << std::endl;
	dump_regs();
	exit(1);
}

// 0x00
void CPU::nop()
{
	cycles = 1;
}

// 0x01
void CPU::ld_bc_nn()
{
	regs.bc = gb->mmu.readShort(pc);
	pc += 2;
	cycles = 3;
}

// 0x02
void CPU::ld_bc_a()
{
	gb->mmu.writeByte(regs.bc, regs.a);
	cycles = 2;
}

// 0x03
void CPU::inc_bc()
{
	++regs.bc;
	cycles = 2;
}

// 0x04
void CPU::inc_b()
{
	regs.b = inc(regs.b);
	cycles = 1;
}

// 0x05
void CPU::dec_b()
{
	regs.b = dec(regs.b);
	cycles = 1;
}

// 0x06
void CPU::ld_b_n()
{
	regs.b = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x07
void CPU::rlca()
{
	uint8_t carry = (regs.a & 0x80) >> 7;
	carry ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a <<= 1;
	regs.a += carry;
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x08
void CPU::ld_nn_sp()
{
	gb->mmu.writeShort(gb->mmu.readShort(pc), sp);
	pc += 2;
	cycles = 5;
}

// 0x09
void CPU::add_hl_bc()
{
	add16(regs.bc);
	cycles = 2;
}

// 0x0A
void CPU::ld_a_bc()
{
	regs.a = gb->mmu.readByte(regs.bc);
	cycles = 2;
}

// 0x0B
void CPU::dec_bc()
{
	--regs.bc;
	cycles = 2;
}

// 0x0C
void CPU::inc_c()
{
	regs.c = inc(regs.c);
	cycles = 1;
}

// 0x0D
void CPU::dec_c()
{
	regs.c = dec(regs.c);
	cycles = 2;
}

// 0x0E:
void CPU::ld_c_n()
{
	regs.c = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x0F
void CPU::rrca()
{
	uint8_t carry = (regs.a & 0x01);
	carry ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a >>= 1;
	regs.a += (carry << 7);
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x11
void CPU::ld_de_nn()
{
	regs.de = gb->mmu.readShort(pc);
	pc += 2;
	cycles = 3;
}

// 0x12
void CPU::ld_de_a()
{
	gb->mmu.writeByte(regs.de, regs.a);
	cycles = 2;
}

// 0x13
void CPU::inc_de()
{
	++regs.de;
	cycles = 2;
}

// 0x14:
void CPU::inc_d()
{
	regs.d = inc(regs.d);
	cycles = 1;
}

// 0x15
void CPU::dec_d()
{
	regs.d = dec(regs.d);
	cycles = 1;
}

// 0x16
void CPU::ld_d_n()
{
	regs.d = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x17
void CPU::rla()
{
	uint8_t carry = FLAGS_ISCARRY ? 1 : 0;
	regs.a & 0x80 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a <<= 1;
	regs.a += carry;
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x18
void CPU::jr_n()
{
	jr();
	cycles = 3;
}

// 0x19
void CPU::add_hl_de()
{
	add16(regs.de);
	cycles = 2;
}

// 0x1A
void CPU::ld_a_de()
{
	regs.a = gb->mmu.readByte(regs.de);
	cycles = 2;
}

// 0x1B
void CPU::dec_de()
{
	--regs.de;
	cycles = 2;
}

// 0x1C
void CPU::inc_e()
{
	regs.e = inc(regs.e);
	cycles = 1;
}

// 0x1D
void CPU::dec_e()
{
	regs.e = dec(regs.e);
	cycles = 1;
}

// 0x1E
void CPU::ld_e_n()
{
	regs.e = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x1F
void CPU::rra()
{
	int old_carry = (FLAGS_ISCARRY ? 1 : 0) << 7;
	regs.a & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a >>= 1;
	regs.a += old_carry;
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x20
void CPU::jr_nz_n()
{
	if (!FLAGS_ISZERO)
	{
		jr();
		cycles = 3;
	}
	else
	{
		++pc;
		cycles = 2;
	}
}

// 0x21
void CPU::ld_hl_nn()
{
	regs.hl = gb->mmu.readShort(pc);
	pc += 2;
	cycles = 3;
}

// 0x22
void CPU::ldi_hl()
{
	gb->mmu.writeByte(regs.hl++, regs.a);
	cycles = 2;
}

// 0x23
void CPU::inc_hl()
{
	++regs.hl;
	cycles = 2;
}

// 0x24
void CPU::inc_h()
{
	regs.h = inc(regs.h);
	cycles = 1;
}

// 0x25
void CPU::dec_h()
{
	regs.h = dec(regs.h);
	cycles = 1;
}

// 0x26
void CPU::ld_h_n()
{
	regs.h = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x27
void CPU::daa()
{
	uint16_t temp = regs.a;
	if (FLAGS_ISNEGATIVE)
	{
		if (FLAGS_ISHALFCARRY) temp = (temp - 0x06) & 0xFF;
		if (FLAGS_ISCARRY) temp -= 0x60;
	}
	else
	{
		if (FLAGS_ISHALFCARRY || (temp & 0xF) > 9) temp += 0x06;
		if (FLAGS_ISCARRY || temp > 0x9F) temp += 0x60;
	}
	regs.a = static_cast<uint8_t>(temp);
	FLAGS_CLEAR(FLAGS_HALFCARRY);
	(regs.a == 0) ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	if (temp >= 0x100) FLAGS_SET(FLAGS_CARRY);
	cycles = 1;
}

// 0x28
void CPU::jr_z_n()
{
	if (FLAGS_ISZERO)
	{
		jr();
		cycles = 3;
	}
	else
	{
		++pc;
		cycles = 2;
	}
}

// 0x29
void CPU::add_hl_hl()
{
	add16(regs.hl);
	cycles = 2;
}

// 0x2A
void CPU::ldi_a()
{
	regs.a = gb->mmu.readByte(regs.hl++);
	cycles = 2;
}

// 0x2B
void CPU::dec_hl()
{
	--regs.hl;
	cycles = 2;
}

// 0x2C
void CPU::inc_l()
{
	regs.l = inc(regs.l);
	cycles = 1;
}

// 0x2D
void CPU::dec_l()
{
	regs.l = dec(regs.l);
	cycles = 1;
}

// 0x2E
void CPU::ld_l_n()
{
	regs.l = gb->mmu.readByte(pc++);
	cycles = 2;
}

// 0x2F
void CPU::cpl()
{
	regs.a = ~regs.a;
	FLAGS_SET(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x30
void CPU::jr_nc()
{
	if (!FLAGS_ISCARRY)
	{
		jr();
		cycles = 3;
	}
	else
	{
		++pc;
		cycles = 2;
	}
}

// 0x31
void CPU::ld_sp_nn()
{
	sp = gb->mmu.readShort(pc);
	pc += 2;
	cycles = 3;
}

// 0x32
void CPU::ldd_hl ()
{
	gb->mmu.writeByte(regs.hl--, regs.a);
	cycles = 2;
}

// 0x33
void CPU::inc_sp()
{
	++sp;
	cycles = 2;
}

// 0x34
void CPU::inc_hla()
{
	gb->mmu.writeByte(regs.hl, inc(gb->mmu.readByte(regs.hl)));
	cycles = 3;
}

// 0x35
void CPU::dec_hla()
{
	gb->mmu.writeByte(regs.hl, dec(gb->mmu.readByte(regs.hl)));
	cycles = 3;
}

// 0x36
void CPU::ld_hl_n()
{
	gb->mmu.writeByte(regs.hl, gb->mmu.readByte(pc++));
	cycles = 3;
}

// 0x37
void CPU::scf()
{
	FLAGS_SET(FLAGS_CARRY);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x38
void CPU::jr_c_n()
{
	if (FLAGS_ISCARRY)
	{
		jr();
		cycles = 3;
	}
	else
	{
		++pc;
		cycles = 2;
	}
}

// 0x39
void CPU::add_hl_sp()
{
	add16(sp);
	cycles = 2;
}

// 0x3A
void CPU::ldd_a()
{
	regs.a = gb->mmu.readByte(regs.hl--);
	cycles = 2;
}

// 0x3B
void CPU::dec_sp()
{
	--sp;
	cycles = 2;
}

// 0x3C
void CPU::inc_a()
{
	regs.a = inc(regs.a);
	cycles = 1;
}

// 0x3D
void CPU::dec_a()
{
	regs.a = dec(regs.a);
	cycles = 1;
}

// 0x3E
void CPU::ld_a_n()
{
	regs.a = gb->mmu.readByte(pc++);
	cycles = 2;
}

void CPU::ccf()
{
	FLAGS_ISCARRY ? FLAGS_CLEAR(FLAGS_CARRY) : FLAGS_SET(FLAGS_CARRY);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	cycles = 1;
}

// 0x40
void CPU::ld_b_b()
{
	regs.b = regs.b;
	cycles = 1;
}

// 0x41
void CPU::ld_b_c()
{
	regs.b = regs.c;
	cycles = 1;
}

// 0x42
void CPU::ld_b_d()
{
	regs.b = regs.d;
	cycles = 1;
}

// 0x43
void CPU::ld_b_e()
{
	regs.b = regs.e;
	cycles = 1;
}

// 0x44
void CPU::ld_b_h()
{
	regs.b = regs.h;
	cycles = 1;
}

// 0x45
void CPU::ld_b_l()
{
	regs.b = regs.l;
	cycles = 1;
}

// 0x46
void CPU::ld_b_hl()
{
	regs.b = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x47
void CPU::ld_b_a()
{
	regs.b = regs.a;
	cycles = 1;
}

// 0x48
void CPU::ld_c_b()
{
	regs.c = regs.b;
	cycles = 1;
}

// 0x49
void CPU::ld_c_c()
{
	regs.c = regs.c;
	cycles = 1;
}

// 0x4A
void CPU::ld_c_d()
{
	regs.c = regs.d;
	cycles = 1;
}

// 0x4B
void CPU::ld_c_e()
{
	regs.c = regs.e;
	cycles = 1;
}

// 0x4C
void CPU::ld_c_h()
{
	regs.c = regs.h;
	cycles = 1;
}

// 0x4D
void CPU::ld_c_l()
{
	regs.c = regs.l;
	cycles = 1;
}

// 0x4E
void CPU::ld_c_hl()
{
	regs.c = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x4F
void CPU::ld_c_a()
{
	regs.c = regs.a;
	cycles = 1;
}

// 0x50
void CPU::ld_d_b()
{
	regs.d = regs.b;
	cycles = 1;
}

// 0x51
void CPU::ld_d_c()
{
	regs.d = regs.c;
	cycles = 1;
}

// 0x52
void CPU::ld_d_d()
{
	regs.d = regs.d;
	cycles = 1;
}

// 0x53
void CPU::ld_d_e()
{
	regs.d = regs.e;
	cycles = 1;
}

// 0x54
void CPU::ld_d_h()
{
	regs.d = regs.h;
	cycles = 1;
}

// 0x55
void CPU::ld_d_l()
{
	regs.d = regs.l;
	cycles = 1;
}


// 0x56
void CPU::ld_d_hl()
{
	regs.d = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x57
void CPU::ld_d_a()
{
	regs.d = regs.a;
	cycles = 1;
}


// 0x58
void CPU::ld_e_b()
{
	regs.e = regs.b;
	cycles = 1;
}

// 0x59
void CPU::ld_e_c()
{
	regs.e = regs.c;
	cycles = 1;
}

// 0x5A
void CPU::ld_e_d()
{
	regs.e = regs.d;
	cycles = 1;
}

// 0x5B
void CPU::ld_e_e()
{
	regs.e = regs.e;
	cycles = 1;
}

// 0x5C
void CPU::ld_e_h()
{
	regs.e = regs.h;
	cycles = 1;
}

// 0x5D
void CPU::ld_e_l()
{
	regs.e = regs.l;
	cycles = 1;
}

// 0x5E
void CPU::ld_e_hl()
{
	regs.e = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x5F
void CPU::ld_e_a()
{
	regs.e = regs.a;
	cycles = 1;
}

// 0x60
void CPU::ld_h_b()
{
	regs.h = regs.b;
	cycles = 1;
}

// 0x61
void CPU::ld_h_c()
{
	regs.h = regs.c;
	cycles = 1;
}

// 0x62
void CPU::ld_h_d()
{
	regs.h = regs.d;
	cycles = 1;
}

// 0x63
void CPU::ld_h_e()
{
	regs.h = regs.e;
	cycles = 1;
}

// 0x64
void CPU::ld_h_h()
{
	regs.h = regs.h;
	cycles = 1;
}

// 0x65
void CPU::ld_h_l()
{
	regs.h = regs.l;
	cycles = 1;
}

// 0x66
void CPU::ld_h_hl()
{
	regs.h = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x67
void CPU::ld_h_a()
{
	regs.h = regs.a;
	cycles = 1;
}

// 0x68
void CPU::ld_l_b()
{
	regs.l = regs.b;
	cycles = 1;
}

// 0x69
void CPU::ld_l_c()
{
	regs.l = regs.c;
	cycles = 1;
}

// 0x6A
void CPU::ld_l_d()
{
	regs.l = regs.d;
	cycles = 1;
}

// 0x6B
void CPU::ld_l_e()
{
	regs.l = regs.e;
	cycles = 1;
}

// 0x6C
void CPU::ld_l_h()
{
	regs.l = regs.h;
	cycles = 1;
}

// 0x6D
void CPU::ld_l_l()
{
	regs.l = regs.l;
	cycles = 1;
}

// 0x6E
void CPU::ld_l_hl()
{
	regs.l = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x6F
void CPU::ld_l_a()
{
	regs.l = regs.a;
	cycles = 1;
}

// 0x70
void CPU::ld_hl_b()
{
	gb->mmu.writeByte(regs.hl, regs.b);
	cycles = 2;
}

// 0x71
void CPU::ld_hl_c()
{
	gb->mmu.writeByte(regs.hl, regs.c);
	cycles = 2;
}

// 0x72
void CPU::ld_hl_d()
{
	gb->mmu.writeByte(regs.hl, regs.d);
	cycles = 2;

}

// 0x73
void CPU::ld_hl_e()
{
	gb->mmu.writeByte(regs.hl, regs.e);
	cycles = 2;
}

// 0x74
void CPU::ld_hl_h()
{
	gb->mmu.writeByte(regs.hl, regs.h);
	cycles = 2;
}

// 0x75
void CPU::ld_hl_l()
{
	gb->mmu.writeByte(regs.hl, regs.l);
	cycles = 2;
}

// 0x76
void CPU::halt()
{
	HALT = true;
	cycles = 1;
}

// 0x77
void CPU::ld_hl_a()
{
	gb->mmu.writeByte(regs.hl, regs.a);
	cycles = 2;
}

// 0x78
void CPU::ld_a_b()
{
	regs.a = regs.b;
	cycles = 1;
}

// 0x79
void CPU::ld_a_c()
{
	regs.a = regs.c;
	cycles = 1;
}

// 0x7A
void CPU::ld_a_d()
{
	regs.a = regs.d;
	cycles = 1;
}

// 0x7B
void CPU::ld_a_e()
{
	regs.a = regs.e;
	cycles = 1;
}

// 0x7C
void CPU::ld_a_h()
{
	regs.a = regs.h;
	cycles = 1;
}

// 0x7D
void CPU::ld_a_l()
{
	regs.a = regs.l;
	cycles = 1;
}

// 0x7E
void CPU::ld_a_hl()
{
	regs.a = gb->mmu.readByte(regs.hl);
	cycles = 2;
}

// 0x7F
void CPU::ld_a_a()
{
	regs.a = regs.a;
	cycles = 1;
}

// 0x80
void CPU::add_b()
{
	add(regs.b);
	cycles = 1;
}

// 0x81
void CPU::add_c()
{
	add(regs.c);
	cycles = 1;
}

// 0x82
void CPU::add_d()
{
	add(regs.d);
	cycles = 1;
}

// 0x83
void CPU::add_e()
{
	add(regs.e);
	cycles = 1;
}

// 0x84
void CPU::add_h()
{
	add(regs.h);
	cycles = 1;
}

// 0x85
void CPU::add_l()
{
	add(regs.l);
	cycles = 1;
}

// 0x86
void CPU::add_hl()
{
	add(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0x87
void CPU::add_a()
{
	add(regs.a);
	cycles = 1;
}

// 0x88
void CPU::adc_b()
{
	adc(regs.b);
	cycles = 1;
}

// 0x89
void CPU::adc_c()
{
	adc(regs.c);
	cycles = 1;
}

// 0x8A
void CPU::adc_d()
{
	adc(regs.d);
	cycles = 1;
}

// 0x8B
void CPU::adc_e()
{
	adc(regs.e);
	cycles = 1;
}

// 0x8C
void CPU::adc_h()
{
	adc(regs.h);
	cycles = 1;
}

// 0x8D
void CPU::adc_l()
{
	adc(regs.l);
	cycles = 1;
}

// 0x8E
void CPU::adc_hl()
{
	adc(gb->mmu.readByte(regs.hl));
	cycles = 1;
}

// 0x8F
void CPU::adc_a()
{
	adc(regs.a);
	cycles = 1;
}

// 0x90
void CPU::sub_b()
{
	sub(regs.b);
	cycles = 1;
}

// 0x91
void CPU::sub_c()
{
	sub(regs.c);
	cycles = 1;
}

// 0x92
void CPU::sub_d()
{
	sub(regs.d);
	cycles = 1;
}

// 0x93
void CPU::sub_e()
{
	sub(regs.e);
	cycles = 1;
}

// 0x94
void CPU::sub_h()
{
	sub(regs.h);
	cycles = 1;
}

// 0x95
void CPU::sub_l()
{
	sub(regs.l);
	cycles = 1;
}

// 0x96
void CPU::sub_hl()
{
	sub(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0x97
void CPU::sub_a()
{
	sub(regs.a);
	cycles = 1;
}

// 0x98
void CPU::sbc_b()
{
	sbc(regs.b);
	cycles = 1;
}

// 0x99
void CPU::sbc_c()
{
	sbc(regs.c);
	cycles = 1;
}

// 0x9A
void CPU::sbc_d()
{
	sbc(regs.d);
	cycles = 1;
}

// 0x9B
void CPU::sbc_e()
{
	sbc(regs.e);
	cycles = 1;
}

// 0x9C
void CPU::sbc_h()
{
	sbc(regs.h);
	cycles = 1;
}

// 0x9D
void CPU::sbc_l()
{
	sbc(regs.l);
	cycles = 1;
}

// 0x9E
void CPU::sbc_hl()
{
	sbc(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0x9F
void CPU::sbc_a()
{
	sbc(regs.a);
	cycles = 1;
}

// 0xA0
void CPU::and_b()
{
	AND(regs.b);
	cycles = 1;
}

// 0xA1
void CPU::and_c()
{
	AND(regs.c);
	cycles = 1;
}

// 0xA2
void CPU::and_d()
{
	AND(regs.d);
	cycles = 1;
}

// 0xA3
void CPU::and_e()
{
	AND(regs.e);
	cycles = 1;
}

// 0xA4
void CPU::and_h()
{
	AND(regs.h);
	cycles = 1;
}

// 0xA5
void CPU::and_l()
{
	AND(regs.l);
	cycles = 1;
}

// 0xA6
void CPU::and_hl()
{
	AND(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0xA7
void CPU::and_a()
{
	AND(regs.a);
	cycles = 1;
}

// 0xA8
void CPU::xor_b()
{
	XOR(regs.b);
	cycles = 1;
}

// 0xA9
void CPU::xor_c()
{
	XOR(regs.c);
	cycles = 1;
}

// 0xAA
void CPU::xor_d()
{
	XOR(regs.d);
	cycles = 1;
}

// 0xAB
void CPU::xor_e()
{
	XOR(regs.e);
	cycles = 1;
}

// 0xAC
void CPU::xor_h()
{
	XOR(regs.h);
	cycles = 1;
}

// 0xAD
void CPU::xor_l()
{
	XOR(regs.l);
	cycles = 1;
}

// 0xAE
void CPU::xor_hl()
{
	XOR(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0xAF
void CPU::xor_a()
{
	XOR(regs.a);
	cycles = 1;
}

// 0xB0
void CPU::or_b()
{
	OR(regs.b);
	cycles = 1;
}

// 0xB1
void CPU::or_c()
{
	OR(regs.c);
	cycles = 1;
}

// 0xB2
void CPU::or_d()
{
	OR(regs.d);
	cycles = 1;
}

// 0xB3
void CPU::or_e()
{
	OR(regs.e);
	cycles = 1;
}

// 0xB4
void CPU::or_h()
{
	OR(regs.h);
	cycles = 1;
}

// 0xB5
void CPU::or_l()
{
	OR(regs.l);
	cycles = 1;
}

// 0xB6
void CPU::or_hl()
{
	OR(gb->mmu.readByte(regs.hl));
	cycles = 2;
}

// 0xB7
void CPU::or_a()
{
	OR(regs.a);
	cycles = 1;
}

// 0xB8
void CPU::cp_b()
{
	cp(regs.b);
	cycles = 1;
}

// 0xB9
void CPU::cp_c()
{
	cp(regs.c);
	cycles = 1;
}

// 0xBA
void CPU::cp_d()
{
	cp(regs.d);
	cycles = 1;
}

// 0xBB
void CPU::cp_e()
{
	cp(regs.e);
	cycles = 1;
}

// 0xBC
void CPU::cp_h()
{
	cp(regs.h);
	cycles = 1;
}


// 0xBD
void CPU::cp_l()
{
	cp(regs.l);
	cycles = 1;
}


// 0xBE
void CPU::cp_hl ()
{
	cp(gb->mmu.readByte(regs.hl));
	cycles = 2;
}


// 0xBF
void CPU::cp_a()
{
	cp(regs.a);
	cycles = 1;
}


// 0xC0
void CPU::ret_nz()
{
	if (!FLAGS_ISZERO)
	{
		jp(pop());
		cycles = 5;
	}
	else cycles = 2;
}

// 0xC1
void CPU::pop_bc()
{
	regs.bc = pop();
	cycles = 3;
}

// 0xC2
void CPU::jp_nz()
{
	if (!FLAGS_ISZERO)
	{
		jp(gb->mmu.readShort(pc));
		cycles = 4;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xC3
void CPU::jp_nn()
{
	jp(gb->mmu.readShort(pc));
	cycles = 4;
}

void CPU::call_nz()
{
	if (!FLAGS_ISZERO)
	{
		call(gb->mmu.readShort(pc));
		cycles = 6;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xC5
void CPU::push_bc()
{
	push(regs.bc);
	cycles = 4;
}

// 0xC6
void CPU::add_n()
{
	add(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xC7
void CPU::rst_0()
{
	push(pc);
	jp(0x00);
	cycles = 4;
}

// 0xC8
void CPU::ret_z()
{
	if (FLAGS_ISZERO)
	{
		jp(pop());
		cycles = 5;
	} 
	else cycles = 2;
}

// 0xC9
void CPU::ret()
{
	jp(pop());
	cycles = 4;
}

// 0xCA
void CPU::jp_z()
{
	if (FLAGS_ISZERO)
	{
		jp(gb->mmu.readShort(pc));
		cycles = 4;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xCB
void CPU::cb()
{
	cb_instr();
}

// 0xCC
void CPU::call_z()
{
	if (FLAGS_ISZERO)
	{
		call(gb->mmu.readShort(pc));
		cycles = 6;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xCD
void CPU::call_nn()
{
	call(gb->mmu.readShort(pc));
	cycles = 6;
}

// 0xCE
void CPU::adc_n()
{
	adc(gb->mmu.readByte(pc++));
	cycles = 2;
}

void CPU::rst_8()
{
	push(pc);
	jp(0x08);
	cycles = 4;
}

// 0xD0
void CPU::ret_nc()
{
	if (!FLAGS_ISCARRY)
	{
		jp(pop());
		cycles = 5;
	}
	else cycles = 2;
}

// 0xD1
void CPU::pop_de()
{
	regs.de = pop();
	cycles = 3;
}

// 0xD2
void CPU::jp_nc()
{
	if (!FLAGS_ISCARRY)
	{
		jp(gb->mmu.readShort(pc));
		cycles = 4;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xD4
void CPU::call_nc()
{
	if (!FLAGS_ISCARRY)
	{
		call(gb->mmu.readShort(pc));
		cycles = 6;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xD5
void CPU::push_de()
{
	push(regs.de);
	cycles = 4;
}

// 0xD6
void CPU::sub_n()
{
	sub(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xD7
void CPU::rst_10()
{
	push(pc);
	jp(0x10);
	cycles = 4;
}

// 0xD8
void CPU::ret_c()
{
	if (FLAGS_ISCARRY)
	{
		jp(pop());
		cycles = 5;
	}
	else cycles = 2;
}

// 0xD9
void CPU::reti()
{
	jp(pop());
	interrupts_enabled = true;
	cycles = 4;
}

// 0xDA
void CPU::jp_c()
{
	if (FLAGS_ISCARRY)
	{
		jp(gb->mmu.readShort(pc));
		cycles = 4;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xDB
void CPU::call_c()
{
	if (FLAGS_ISCARRY)
	{
		call(gb->mmu.readShort(pc));
		cycles = 6;
	}
	else
	{
		pc += 2;
		cycles = 3;
	}
}

// 0xDE
void CPU::sbc_n()
{
	sbc(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xDF
void CPU::rst_18()
{
	push(pc);
	jp(0x18);
	cycles = 4;
}

// 0xE0
void CPU::ldh_n_a()
{
	uint16_t addr = 0xFF00 + gb->mmu.readByte(pc++);
	gb->mmu.writeByte(addr, regs.a);
	cycles = 3;
}

// 0xE1
void CPU::pop_hl()
{
	regs.hl = pop();
	cycles = 3;
}

// 0xE2
void CPU::ld_ff_c_a()
{
	uint16_t addr = 0xFF00 + regs.c;
	gb->mmu.writeByte(addr, regs.a);
	cycles = 2;
}

// 0xE5
void CPU::push_hl()
{
	push(regs.hl);
	cycles = 4;
}

// 0xE6
void CPU::and_n()
{
	AND(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xE7
void CPU::rst_20()
{
	push(pc);
	jp(0x20);
	cycles = 4;
}

// 0xE8
void CPU::add_sp_n()
{
	int8_t value = static_cast<int8_t>(gb->mmu.readByte(pc++));
	int16_t sum = sp + value;
	(sp ^ value ^ sum) & 0x10 ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	(sp ^ value ^ sum) & 0x100 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);
	sp = static_cast<uint16_t>(sum);
	cycles = 4;
}

// 0xE9
void CPU::jp_hl()
{
	jp(regs.hl);
	cycles = 1;
}

// 0xEA
void CPU::ld_nn_a()
{
	gb->mmu.writeByte(gb->mmu.readShort(pc), regs.a);
	pc += 2;
	cycles = 4;
}

// 0xEE
void CPU::xor_n()
{
	XOR(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xEF
void CPU::rst_28()
{
	push(pc);
	jp(0x28);
	cycles = 4;
}

// 0xF0
void CPU::ldh_a_n()
{
	uint16_t addr = 0xFF00 + gb->mmu.readByte(pc++);
	regs.a = gb->mmu.readByte(addr);
	cycles = 3;
}

// 0xF1
void CPU::pop_af()
{
	regs.af = pop();
	regs.f &= 0xF0;
	cycles = 3;
}

// 0xF2
void CPU::ld_a_ff_c()
{
	uint16_t addr = 0xFF00 + regs.c;
	regs.a = gb->mmu.readByte(addr);
	cycles = 2;
}

// 0xF3
void CPU::di()
{
	interrupts_enabled = false;
	cycles = 1;
}

// 0xF5
void CPU::push_af()
{
	push(regs.af);
	cycles = 4;
}

// 0xF6
void CPU::or_n()
{
	OR(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xF7
void CPU::rst_30()
{
	push(pc);
	jp(0x30);
	cycles = 4;
}

// 0xF8
void CPU::ld_hl_sp_n()
{
	int8_t value = static_cast<int8_t>(gb->mmu.readByte(pc++));
	int16_t sum = sp + value;
	(sp ^ value ^ sum) & 0x10 ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	(sp ^ value ^ sum) & 0x100 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);
	regs.hl = static_cast<uint16_t>(sum);
	cycles = 3;
}

// 0xF9
void CPU::ld_sp_hl()
{
	sp = regs.hl;
	cycles = 2;
}

// 0xFA
void CPU::ld_a_nn()
{
	uint16_t addr = gb->mmu.readShort(pc);
	regs.a = gb->mmu.readByte(addr);
	pc += 2;
	cycles = 4;
}

// 0xFB
void CPU::ei()
{
	interrupts_enabled = true;
	cycles = 1;
}

// 0xFE
void CPU::cp_n()
{
	cp(gb->mmu.readByte(pc++));
	cycles = 2;
}

// 0xFF
void CPU::rst_38()
{
	push(pc);
	jp(0x38);
	cycles = 4;
}

uint8_t CPU::rlc(uint8_t value)
{
	int carry = (value & 0x80) >> 7;
	value & 0x80 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value <<= 1;
	value += carry;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::rrc(uint8_t value)
{
	int carry = (value & 0x01);
	value >>= 1;
	if (carry)
	{
		FLAGS_SET(FLAGS_CARRY);
		value |= 0x80;
	}
	else
	{
		FLAGS_CLEAR(FLAGS_CARRY);
	}

	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::rl(uint8_t value)
{
	int old_carry = FLAGS_ISCARRY ? 1 : 0;
	value & 0x80 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value <<= 1;
	value += old_carry;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::rr(uint8_t value)
{
	int old_carry = (FLAGS_ISCARRY ? 1 : 0) << 7;
	value & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value >>= 1;
	value += old_carry;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::sla(uint8_t value)
{
	value & 0x80 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value <<= 1;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::sra(uint8_t value)
{
	value & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value = (value & 0x80) | value >> 1;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

uint8_t CPU::swap(uint8_t value)
{
	value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY | FLAGS_CARRY);

	return value;
}

uint8_t CPU::srl(uint8_t value)
{
	value & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value >>= 1;
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

void CPU::bit(uint8_t value, uint8_t bit)
{
	((value & (1 << bit)) == 0) ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_SET(FLAGS_HALFCARRY);
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

uint8_t CPU::res(uint8_t value, uint8_t bit)
{
	return (value & ~(1 << bit));
}

uint8_t CPU::set(uint8_t value, uint8_t bit)
{
	return (value | (1 << bit));
}

// 0x00
void CPU::rlc_b()
{
	regs.b = rlc(regs.b);
	cycles = 2;
}

// 0x01
void CPU::rlc_c()
{
	regs.c = rlc(regs.c);
	cycles = 2;
}

// 0x02
void CPU::rlc_d()
{
	regs.d = rlc(regs.d);
	cycles = 2;
}

// 0x03
void CPU::rlc_e()
{
	regs.e = rlc(regs.e);
	cycles = 2;
}

// 0x04
void CPU::rlc_h()
{
	regs.h = rlc(regs.h);
	cycles = 2;
}

// 0x05
void CPU::rlc_l()
{
	regs.l = rlc(regs.l);
	cycles = 2;
}

// 0x06
void CPU::rlc_hl()
{
	gb->mmu.writeByte(regs.hl, rlc(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x07
void CPU::rlc_a()
{
	regs.a = rlc(regs.a);
	cycles = 2;
}

// 0x08
void CPU::rrc_b()
{
	regs.b = rrc(regs.b);
	cycles = 2;
}

// 0x09
void CPU::rrc_c()
{
	regs.c = rrc(regs.c);
	cycles = 2;
}

// 0x0A
void CPU::rrc_d()
{
	regs.d = rrc(regs.d);
	cycles = 2;
}

// 0x0B
void CPU::rrc_e()
{
	regs.e = rrc(regs.e);
	cycles = 2;
}

// 0x0C
void CPU::rrc_h()
{
	regs.h = rrc(regs.h);
	cycles = 2;
}

// 0x0D
void CPU::rrc_l()
{
	regs.l = rrc(regs.l);
	cycles = 2;
}

// 0x0E
void CPU::rrc_hl()
{
	gb->mmu.writeByte(regs.hl, rrc(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x0F
void CPU::rrc_a()
{
	regs.a = rrc(regs.a);
	cycles = 2;
}

// 0x10
void CPU::rl_b()
{
	regs.b = rl(regs.b);
	cycles = 2;
}

// 0x11
void CPU::rl_c()
{
	regs.c = rl(regs.c);
	cycles = 2;
}

// 0x12
void CPU::rl_d()
{
	regs.d = rl(regs.d);
	cycles = 2;
}

// 0x13
void CPU::rl_e()
{
	regs.e = rl(regs.e);
	cycles = 2;
}

// 0x14
void CPU::rl_h()
{
	regs.h = rl(regs.h);
	cycles = 4;
}

// 0x15
void CPU::rl_l()
{
	regs.l = rl(regs.l);
	cycles = 2;
}

// 0x16
void CPU::rl_hl()
{
	gb->mmu.writeByte(regs.hl, rl(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x17
void CPU::rl_a()
{
	regs.a = rl(regs.a);
	cycles = 2;
}

// 0x18
void CPU::rr_b()
{
	regs.b = rr(regs.b);
	cycles = 2;
}

// 0x19
void CPU::rr_c()
{
	regs.c = rr(regs.c);
	cycles = 2;
}

// 0x1A
void CPU::rr_d()
{
	regs.d = rr(regs.d);
	cycles = 2;
}

// 0x1B
void CPU::rr_e()
{
	regs.e = rr(regs.e);
	cycles = 2;
}

// 0x1C
void CPU::rr_h()
{
	regs.h = rr(regs.h);
	cycles = 2;
}

// 0x1D
void CPU::rr_l()
{
	regs.l = rr(regs.l);
	cycles = 2;
}

// 0x1E
void CPU::rr_hl()
{
	gb->mmu.writeByte(regs.hl, rr(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x1F
void CPU::rr_a()
{
	regs.a = rr(regs.a);
	cycles = 2;
}

// 0x20
void CPU::sla_b()
{
	regs.b = sla(regs.b);
	cycles = 2;
}

// 0x21
void CPU::sla_c()
{
	regs.c = sla(regs.c);
	cycles = 2;
}

// 0x22
void CPU::sla_d()
{
	regs.d = sla(regs.d);
	cycles = 2;
}

// 0x23
void CPU::sla_e()
{
	regs.e = sla(regs.e);
	cycles = 2;
}

// 0x24
void CPU::sla_h()
{
	regs.h = sla(regs.h);
	cycles = 2;
}

// 0x25
void CPU::sla_l()
{
	regs.l = sla(regs.l);
	cycles = 2;
}

// 0x26
void CPU::sla_hl()
{
	gb->mmu.writeByte(regs.hl, sla(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x27
void CPU::sla_a()
{
	regs.a = sla(regs.a);
	cycles = 2;
}

// 0x28
void CPU::sra_b()
{
	regs.b = sra(regs.b);
	cycles = 2;
}

// 0x29
void CPU::sra_c()
{
	regs.c = sra(regs.c);
	cycles = 2;
}

// 0x2A
void CPU::sra_d()
{
	regs.d = sra(regs.d);
	cycles = 2;
}

// 0x2B
void CPU::sra_e()
{
	regs.e = sra(regs.e);
	cycles = 2;
}

// 0x2C
void CPU::sra_h()
{
	regs.h = sra(regs.h);
	cycles = 2;
}

// 0x2D
void CPU::sra_l()
{
	regs.l = sra(regs.l);
	cycles = 2;
}

// 0x2E
void CPU::sra_hl()
{
	gb->mmu.writeByte(regs.hl, sra(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x2F
void CPU::sra_a()
{
	regs.a = sra(regs.a);
	cycles = 2;
}

// 0x30
void CPU::swap_b()
{
	regs.b = swap(regs.b);
	cycles = 2;
}

// 0x31
void CPU::swap_c()
{
	regs.c = swap(regs.c);
	cycles = 2;
}

// 0x32
void CPU::swap_d()
{
	regs.d = swap(regs.d);
	cycles = 2;
}

// 0x33
void CPU::swap_e()
{
	regs.e = swap(regs.e);
	cycles = 2;
}

// 0x34
void CPU::swap_h()
{
	regs.h = swap(regs.h);
	cycles = 2;
}

// 0x35
void CPU::swap_l()
{
	regs.l = swap(regs.l);
	cycles = 2;
}

// 0x36
void CPU::swap_hl()
{
	gb->mmu.writeByte(regs.hl, swap(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x37
void CPU::swap_a()
{
	regs.a = swap(regs.a);
	cycles = 2;
}

// 0x38
void CPU::srl_b()
{
	regs.b = srl(regs.b);
	cycles = 2;
}

// 0x39
void CPU::srl_c()
{
	regs.c = srl(regs.c);
	cycles = 2;
}

// 0x3A
void CPU::srl_d()
{
	regs.d = srl(regs.d);
	cycles = 2;
}

// 0x3B
void CPU::srl_e()
{
	regs.e = srl(regs.e);
	cycles = 2;
}

// 0x3C
void CPU::srl_h()
{
	regs.h = srl(regs.h);
	cycles = 2;
}

// 0x3D
void CPU::srl_l()
{
	regs.l = srl(regs.l);
	cycles = 2;
}

// 0x3E
void CPU::srl_hl()
{
	gb->mmu.writeByte(regs.hl, srl(gb->mmu.readByte(regs.hl)));
	cycles = 4;
}

// 0x3F
void CPU::srl_a()
{
	regs.a = srl(regs.a);
	cycles = 2;
}

// 0x40
void CPU::bit_0_b()
{
	bit(regs.b, 0);
	cycles = 2;
}

// 0x41
void CPU::bit_0_c()
{
	bit(regs.c, 0);
	cycles = 2;
}

// 0x42
void CPU::bit_0_d()
{
	bit(regs.d, 0);
	cycles = 2;
}

// 0x43
void CPU::bit_0_e()
{
	bit(regs.e, 0);
	cycles = 2;
}

// 0x44
void CPU::bit_0_h()
{
	bit(regs.h, 0);
	cycles = 2;
}

// 0x45
void CPU::bit_0_l()
{
	bit(regs.l, 0);
	cycles = 2;
}

// 0x46
void CPU::bit_0_hl()
{
	bit(gb->mmu.readByte(regs.hl), 0);
	cycles = 4;
}

// 0x47
void CPU::bit_0_a()
{
	bit(regs.a, 0);
	cycles = 2;
}

// 0x48
void CPU::bit_1_b()
{
	bit(regs.b, 1);
	cycles = 2;
}

// 0x49
void CPU::bit_1_c()
{
	bit(regs.c, 1);
	cycles = 2;
}

// 0x4A
void CPU::bit_1_d()
{
	bit(regs.d, 1);
	cycles = 2;
}

// 0x4B
void CPU::bit_1_e()
{
	bit(regs.e, 1);
	cycles = 2;
}

// 0x4C
void CPU::bit_1_h()
{
	bit(regs.h, 1);
	cycles = 2;
}

// 0x4D
void CPU::bit_1_l()
{
	bit(regs.l, 1);
	cycles = 2;
}

// 0x4E
void CPU::bit_1_hl()
{
	bit(gb->mmu.readByte(regs.hl), 1);
	cycles = 4;
}

// 0x4F
void CPU::bit_1_a()
{
	bit(regs.a, 1);
	cycles = 2;
}

// 0x50
void CPU::bit_2_b()
{
	bit(regs.b, 2);
	cycles = 2;
}

// 0x51
void CPU::bit_2_c()
{
	bit(regs.c, 2);
	cycles = 2;
}

// 0x52
void CPU::bit_2_d()
{
	bit(regs.d, 2);
	cycles = 2;
}

// 0x53
void CPU::bit_2_e()
{
	bit(regs.e, 2);
	cycles = 2;
}

// 0x54
void CPU::bit_2_h()
{
	bit(regs.h, 2);
	cycles = 2;
}

// 0x55
void CPU::bit_2_l()
{
	bit(regs.l, 2);
	cycles = 2;
}

// 0x56
void CPU::bit_2_hl()
{
	bit(gb->mmu.readByte(regs.hl), 2);
	cycles = 4;
}

// 0x57
void CPU::bit_2_a()
{
	bit(regs.a, 2);
	cycles = 2;
}

// 0x58
void CPU::bit_3_b()
{
	bit(regs.b, 3);
	cycles = 2;
}

// 0x59
void CPU::bit_3_c()
{
	bit(regs.c, 3);
	cycles = 2;
}

// 0x5A
void CPU::bit_3_d()
{
	bit(regs.d, 3);
	cycles = 2;
}

// 0x5B
void CPU::bit_3_e()
{
	bit(regs.e, 3);
	cycles = 2;
}

// 0x5C
void CPU::bit_3_h()
{
	bit(regs.h, 3);
	cycles = 2;
}

// 0x5D
void CPU::bit_3_l()
{
	bit(regs.l, 3);
	cycles = 2;
}

// 0x5E
void CPU::bit_3_hl()
{
	bit(gb->mmu.readByte(regs.hl), 3);
	cycles = 4;
}

// 0x5F
void CPU::bit_3_a()
{
	bit(regs.a, 3);
	cycles = 2;
}

// 0x60
void CPU::bit_4_b()
{
	bit(regs.b, 4);
	cycles = 2;
}

// 0x61
void CPU::bit_4_c()
{
	bit(regs.c, 4);
	cycles = 2;
}

// 0x62
void CPU::bit_4_d()
{
	bit(regs.d, 4);
	cycles = 2;
}

// 0x63
void CPU::bit_4_e()
{
	bit(regs.e, 4);
	cycles = 2;
}

// 0x64
void CPU::bit_4_h()
{
	bit(regs.h, 4);
	cycles = 2;
}

// 0x65
void CPU::bit_4_l()
{
	bit(regs.l, 4);
	cycles = 2;
}

// 0x66
void CPU::bit_4_hl()
{
	bit(gb->mmu.readByte(regs.hl), 4);
	cycles = 4;
}

// 0x67
void CPU::bit_4_a()
{
	bit(regs.a, 4);
	cycles = 2;
}

// 0x68
void CPU::bit_5_b()
{
	bit(regs.b, 5);
	cycles = 2;
}

// 0x69
void CPU::bit_5_c()
{
	bit(regs.c, 5);
	cycles = 2;
}

// 0x6A
void CPU::bit_5_d()
{
	bit(regs.d, 5);
	cycles = 2;
}

// 0x6B
void CPU::bit_5_e()
{
	bit(regs.e, 5);
	cycles = 2;
}

// 0x6C
void CPU::bit_5_h()
{
	bit(regs.h, 5);
	cycles = 2;
}

// 0x6D
void CPU::bit_5_l()
{
	bit(regs.l, 5);
	cycles = 2;
}

// 0x6E
void CPU::bit_5_hl()
{
	bit(gb->mmu.readByte(regs.hl), 5);
	cycles = 4;
}

// 0x6F
void CPU::bit_5_a()
{
	bit(regs.a, 5);
	cycles = 2;
}

// 0x70
void CPU::bit_6_b()
{
	bit(regs.b, 6);
	cycles = 2;
}

// 0x71
void CPU::bit_6_c()
{
	bit(regs.c, 6);
	cycles = 2;
}

// 0x72
void CPU::bit_6_d()
{
	bit(regs.d, 6);
	cycles = 2;
}

// 0x73
void CPU::bit_6_e()
{
	bit(regs.e, 6);
	cycles = 2;
}

// 0x74
void CPU::bit_6_h()
{
	bit(regs.h, 6);
	cycles = 2;
}

// 0x75
void CPU::bit_6_l()
{
	bit(regs.l, 6);
	cycles = 2;
}

// 0x76
void CPU::bit_6_hl()
{
	bit(gb->mmu.readByte(regs.hl), 6);
	cycles = 4;
}

// 0x77
void CPU::bit_6_a()
{
	bit(regs.a, 6);
	cycles = 2;
}

// 0x78
void CPU::bit_7_b()
{
	bit(regs.b, 7);
	cycles = 2;
}

// 0x79
void CPU::bit_7_c()
{
	bit(regs.c, 7);
	cycles = 2;
}

// 0x7A
void CPU::bit_7_d()
{
	bit(regs.d, 7);
	cycles = 2;
}

// 0x7B
void CPU::bit_7_e()
{
	bit(regs.e, 7);
	cycles = 2;
}

// 0x7C
void CPU::bit_7_h()
{
	bit(regs.h, 7);
	cycles = 2;
}

// 0x7D
void CPU::bit_7_l()
{
	bit(regs.l, 7);
	cycles = 2;
}

// 0x7E
void CPU::bit_7_hl()
{
	bit(gb->mmu.readByte(regs.hl), 7);
	cycles = 4;
}

// 0x7F
void CPU::bit_7_a()
{
	bit(regs.a, 7);
	cycles = 2;
}

// 0x80
void CPU::res_0_b()
{
	regs.b = res(regs.b, 0);
	cycles = 2;
}

// 0x81
void CPU::res_0_c()
{
	regs.c = res(regs.c, 0);
	cycles = 2;
}

// 0x82
void CPU::res_0_d()
{
	regs.d = res(regs.d, 0);
	cycles = 2;
}

// 0x83
void CPU::res_0_e()
{
	regs.e = res(regs.e, 0);
	cycles = 2;
}

// 0x84
void CPU::res_0_h()
{
	regs.h = res(regs.h, 0);
	cycles = 2;
}

// 0x85
void CPU::res_0_l()
{
	regs.l = res(regs.l, 0);
	cycles = 2;
}

// 0x86
void CPU::res_0_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 0));
	cycles = 2;
}

// 0x87
void CPU::res_0_a()
{
	regs.a = res(regs.a, 0);
	cycles = 2;
}

// 0x88
void CPU::res_1_b()
{
	regs.b = res(regs.b, 1);
	cycles = 2;
}

// 0x89
void CPU::res_1_c()
{
	regs.c = res(regs.c, 1);
	cycles = 2;
}

// 0x8A
void CPU::res_1_d()
{
	regs.d = res(regs.d, 1);
	cycles = 2;
}

// 0x8B
void CPU::res_1_e()
{
	regs.e = res(regs.e, 1);
	cycles = 2;
}

// 0x8C
void CPU::res_1_h()
{
	regs.h = res(regs.h, 1);
	cycles = 2;
}

// 0x8D
void CPU::res_1_l()
{
	regs.l = res(regs.l, 1);
	cycles = 2;
}

// 0x8E
void CPU::res_1_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 1));
	cycles = 4;
}

// 0x8F
void CPU::res_1_a()
{
	regs.a = res(regs.a, 1);
	cycles = 2;
}

// 0x90
void CPU::res_2_b()
{
	regs.b = res(regs.b, 2);
	cycles = 2;
}

// 0x91
void CPU::res_2_c()
{
	regs.c = res(regs.c, 2);
	cycles = 2;
}

// 0x92
void CPU::res_2_d()
{
	regs.d = res(regs.d, 2);
	cycles = 2;
}

// 0x93
void CPU::res_2_e()
{
	regs.e = res(regs.e, 2);
	cycles = 2;
}

// 0x94
void CPU::res_2_h()
{
	regs.h = res(regs.h, 2);
	cycles = 2;
}

// 0x95
void CPU::res_2_l()
{
	regs.l = res(regs.l, 2);
	cycles = 2;
}

// 0x96
void CPU::res_2_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 2));
	cycles = 2;
}

// 0x97
void CPU::res_2_a()
{
	regs.a = res(regs.a, 2);
	cycles = 2;
}

// 0x98
void CPU::res_3_b()
{
	regs.b = res(regs.b, 3);
	cycles = 2;
}

// 0x99
void CPU::res_3_c()
{
	regs.c = res(regs.c, 3);
	cycles = 2;
}

// 0x9A
void CPU::res_3_d()
{
	regs.d = res(regs.d, 3);
	cycles = 2;
}

// 0x9B
void CPU::res_3_e()
{
	regs.e = res(regs.e, 3);
	cycles = 2;
}

// 0x9C
void CPU::res_3_h()
{
	regs.h = res(regs.h, 3);
	cycles = 2;
}

// 0x9D
void CPU::res_3_l()
{
	regs.l = res(regs.l, 3);
	cycles = 2;
}

// 0x9E
void CPU::res_3_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 3));
	cycles = 4;
}

// 0x9F
void CPU::res_3_a()
{
	regs.a = res(regs.a, 3);
	cycles = 2;
}

// 0xA0
void CPU::res_4_b()
{
	regs.b = res(regs.b, 4);
	cycles = 2;
}

// 0xA1
void CPU::res_4_c()
{
	regs.c = res(regs.c, 4);
	cycles = 2;
}

// 0xA2
void CPU::res_4_d()
{
	regs.d = res(regs.d, 4);
	cycles = 2;
}

// 0xA3
void CPU::res_4_e()
{
	regs.e = res(regs.e, 4);
	cycles = 2;
}

// 0xA4
void CPU::res_4_h()
{
	regs.h = res(regs.h, 4);
	cycles = 2;
}

// 0xA5
void CPU::res_4_l()
{
	regs.l = res(regs.l, 4);
	cycles = 2;
}

// 0xA6
void CPU::res_4_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 4));
	cycles = 4;
}

// 0xA7
void CPU::res_4_a()
{
	regs.a = res(regs.a, 4);
	cycles = 2;
}

// 0xA8
void CPU::res_5_b()
{
	regs.b = res(regs.b, 5);
	cycles = 2;
}

// 0xA9
void CPU::res_5_c()
{
	regs.c = res(regs.c, 5);
	cycles = 2;
}

// 0xAA
void CPU::res_5_d()
{
	regs.d = res(regs.d, 5);
	cycles = 2;
}

// 0xAB
void CPU::res_5_e()
{
	regs.e = res(regs.e, 5);
	cycles = 2;
}

// 0xAC
void CPU::res_5_h()
{
	regs.h = res(regs.h, 5);
	cycles = 2;
}

// 0xAD
void CPU::res_5_l()
{
	regs.l = res(regs.l, 5);
	cycles = 2;
}

// 0xAE
void CPU::res_5_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 5));
	cycles = 4;
}

// 0xAF
void CPU::res_5_a()
{
	regs.a = res(regs.a, 5);
	cycles = 2;
}

// 0xB0
void CPU::res_6_b()
{
	regs.b = res(regs.b, 6);
	cycles = 2;
}

// 0xB1
void CPU::res_6_c()
{
	regs.c = res(regs.c, 6);
	cycles = 2;
}

// 0xB2
void CPU::res_6_d()
{
	regs.d = res(regs.d, 6);
	cycles = 2;
}

// 0xB3
void CPU::res_6_e()
{
	regs.e = res(regs.e, 6);
	cycles = 2;
}

// 0xB4
void CPU::res_6_h()
{
	regs.h = res(regs.h, 6);
	cycles = 2;
}

// 0xB5
void CPU::res_6_l()
{
	regs.l = res(regs.l, 6);
	cycles = 2;
}

// 0xB6
void CPU::res_6_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 6));
	cycles = 4;
}

// 0xB7
void CPU::res_6_a()
{
	regs.a = res(regs.a, 6);
	cycles = 2;
}

// 0xB8
void CPU::res_7_b()
{
	regs.b = res(regs.b, 7);
	cycles = 2;
}

// 0xB9
void CPU::res_7_c()
{
	regs.c = res(regs.c, 7);
	cycles = 2;
}

// 0xBA
void CPU::res_7_d()
{
	regs.d = res(regs.d, 7);
	cycles = 2;
}

// 0xBB
void CPU::res_7_e()
{
	regs.e = res(regs.e, 7);
	cycles = 2;
}

// 0xBC
void CPU::res_7_h()
{
	regs.h = res(regs.h, 7);
	cycles = 2;
}

// 0xBD
void CPU::res_7_l()
{
	regs.l = res(regs.l, 7);
	cycles = 2;
}

// 0xBE
void CPU::res_7_hl()
{
	gb->mmu.writeByte(regs.hl, res(gb->mmu.readByte(regs.hl), 7));
	cycles = 4;
}

// 0xBF
void CPU::res_7_a()
{
	regs.a = res(regs.a, 7);
	cycles = 2;
}

// 0xC0
void CPU::set_0_b()
{
	regs.b = set(regs.b, 0);
	cycles = 2;
}

// 0xC1
void CPU::set_0_c()
{
	regs.c = set(regs.c, 0);
	cycles = 2;
}

// 0xC2
void CPU::set_0_d()
{
	regs.d = set(regs.d, 0);
	cycles = 2;
}

// 0xC3
void CPU::set_0_e()
{
	regs.e = set(regs.e, 0);
	cycles = 2;
}

// 0xC4
void CPU::set_0_h()
{
	regs.h = set(regs.h, 0);
	cycles = 2;
}

// 0xC5
void CPU::set_0_l()
{
	regs.l = set(regs.l, 0);
	cycles = 2;
}

// 0xC6
void CPU::set_0_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 0));
	cycles = 4;
}

// 0xC7
void CPU::set_0_a()
{
	regs.a = set(regs.a, 0);
	cycles = 2;
}

// 0xC8
void CPU::set_1_b()
{
	regs.b = set(regs.b, 1);
	cycles = 2;
}

// 0xC9
void CPU::set_1_c()
{
	regs.c = set(regs.c, 1);
	cycles = 2;
}

// 0xCA
void CPU::set_1_d()
{
	regs.d = set(regs.d, 1);
	cycles = 2;
}

// 0xCB
void CPU::set_1_e()
{
	regs.e = set(regs.e, 1);
	cycles = 2;
}

// 0xCC
void CPU::set_1_h()
{
	regs.h = set(regs.h, 1);
	cycles = 2;
}

// 0xCD
void CPU::set_1_l()
{
	regs.l = set(regs.l, 1);
	cycles = 2;
}

// 0xCE
void CPU::set_1_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 1));
	cycles = 4;
}

// 0xCF
void CPU::set_1_a()
{
	regs.a = set(regs.a, 1);
	cycles = 2;
}

// 0xD0
void CPU::set_2_b()
{
	regs.b = set(regs.b, 2);
	cycles = 2;
}

// 0xD1
void CPU::set_2_c()
{
	regs.c = set(regs.c, 2);
	cycles = 2;
}

// 0xD2
void CPU::set_2_d()
{
	regs.d = set(regs.d, 2);
	cycles = 2;
}

// 0xD3
void CPU::set_2_e()
{
	regs.e = set(regs.e, 2);
	cycles = 2;
}

// 0xD4
void CPU::set_2_h()
{
	regs.h = set(regs.h, 2);
	cycles = 2;
}

// 0xD5
void CPU::set_2_l()
{
	regs.l = set(regs.l, 2);
	cycles = 2;
}

// 0xD6
void CPU::set_2_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 2));
	cycles = 4;
}

// 0xD7
void CPU::set_2_a()
{
	regs.a = set(regs.a, 2);
	cycles = 2;
}

// 0xD8
void CPU::set_3_b()
{
	regs.b = set(regs.b, 3);
	cycles = 2;
}

// 0xD9
void CPU::set_3_c()
{
	regs.c = set(regs.c, 3);
	cycles = 2;
}

// 0xDA
void CPU::set_3_d()
{
	regs.d = set(regs.d, 3);
	cycles = 2;
}

// 0xDB
void CPU::set_3_e()
{
	regs.e = set(regs.e, 3);
	cycles = 2;
}

// 0xDC
void CPU::set_3_h()
{
	regs.h = set(regs.h, 3);
	cycles = 2;
}

// 0xDD
void CPU::set_3_l()
{
	regs.l = set(regs.l, 3);
	cycles = 2;
}

// 0xDE
void CPU::set_3_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 3));
	cycles = 4;
}

// 0xDF
void CPU::set_3_a()
{
	regs.a = set(regs.a, 3);
	cycles = 2;
}

// 0xE0
void CPU::set_4_b()
{
	regs.b = set(regs.b, 4);
	cycles = 2;
}

// 0xE1
void CPU::set_4_c()
{
	regs.c = set(regs.c, 4);
	cycles = 2;
}

// 0xE2
void CPU::set_4_d()
{
	regs.d = set(regs.d, 4);
	cycles = 2;
}

// 0xE3
void CPU::set_4_e()
{
	regs.e = set(regs.e, 4);
	cycles = 2;
}

// 0xE4
void CPU::set_4_h()
{
	regs.h = set(regs.h, 4);
	cycles = 2;
}

// 0xE5
void CPU::set_4_l()
{
	regs.l = set(regs.l, 4);
	cycles = 2;
}

// 0xE6
void CPU::set_4_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 4));
	cycles = 4;
}

// 0xE7
void CPU::set_4_a()
{
	regs.a = set(regs.a, 4);
	cycles = 2;
}

// 0xE8
void CPU::set_5_b()
{
	regs.b = set(regs.b, 5);
	cycles = 2;
}

// 0xE9
void CPU::set_5_c()
{
	regs.c = set(regs.c, 5);
	cycles = 2;
}

// 0xEA
void CPU::set_5_d()
{
	regs.d = set(regs.d, 5);
	cycles = 2;
}

// 0xEB
void CPU::set_5_e()
{
	regs.e = set(regs.e, 5);
	cycles = 2;
}

// 0xEC
void CPU::set_5_h()
{
	regs.h = set(regs.h, 5);
	cycles = 2;
}

// 0xED
void CPU::set_5_l()
{
	regs.l = set(regs.l, 5);
	cycles = 2;
}

// 0xEE
void CPU::set_5_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 5));
	cycles = 4;
}

// 0xEF
void CPU::set_5_a()
{
	regs.a = set(regs.a, 5);
	cycles = 2;
}

// 0xF0
void CPU::set_6_b()
{
	regs.b = set(regs.b, 6);
	cycles = 2;
}

// 0xF1
void CPU::set_6_c()
{
	regs.c = set(regs.c, 6);
	cycles = 2;
}

// 0xF2
void CPU::set_6_d()
{
	regs.d = set(regs.d, 6);
	cycles = 2;
}

// 0xF3
void CPU::set_6_e()
{
	regs.e = set(regs.e, 6);
	cycles = 2;
}

// 0xF4
void CPU::set_6_h()
{
	regs.h = set(regs.h, 6);
	cycles = 2;
}

// 0xF5
void CPU::set_6_l()
{
	regs.l = set(regs.l, 6);
	cycles = 2;
}

// 0xF6
void CPU::set_6_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 6));
	cycles = 4;
}

// 0xF7
void CPU::set_6_a()
{
	regs.a = set(regs.a, 6);
	cycles = 2;
}

// 0xF8
void CPU::set_7_b()
{
	regs.b = set(regs.b, 7);
	cycles = 2;
}

// 0xF9
void CPU::set_7_c()
{
	regs.c = set(regs.c, 7);
	cycles = 2;
}

// 0xFA
void CPU::set_7_d()
{
	regs.d = set(regs.d, 7);
	cycles = 2;
}

// 0xFB
void CPU::set_7_e()
{
	regs.e = set(regs.e, 7);
	cycles = 2;
}

// 0xFC
void CPU::set_7_h()
{
	regs.h = set(regs.h, 7);
	cycles = 2;
}

// 0xFD
void CPU::set_7_l()
{
	regs.l = set(regs.l, 7);
	cycles = 2;
}

// 0xFE
void CPU::set_7_hl()
{
	gb->mmu.writeByte(regs.hl, set(gb->mmu.readByte(regs.hl), 7));
	cycles = 4;
}

// 0xFF
void CPU::set_7_a()
{
	regs.a = set(regs.a, 7);
	cycles = 2;
}


void CPU::cb_instr()
{
	opcode = gb->mmu.readByte(pc++);

	(this->*extended_lookup[opcode].execute)();
}

void CPU::cpu_step()
{
#ifdef  LOGGING
	log();
#endif //  LOGGING

	handle_interrupts();
	handle_timer();

	if (!HALT)
	{
		opcode = gb->mmu.readByte(pc++);

		(this->*lookup[opcode].execute)();
	}
	else
	{
		cycles = 1;
	}

	total_cycles += cycles;
	if (total_cycles >= 17556)
	{
		total_cycles = 0;
		frame_complete = true;
	}
}