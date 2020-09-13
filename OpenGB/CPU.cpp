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
	pc = 0x0100;

	opcode = 0x00;

	HALT = false;

	// Opcode lookup table
	using a = CPU;
	lookup = 
	{ 
		{"NOP", &a::nop, 1},{"LD BC,d16", &a::ld_bc_nn, 3},{"LD (BC),A", &a::undefined, 1},{"INC BC", &a::inc_bc, 1},{"INC B", &a::undefined, 1},{"DEC B", &a::dec_b, 1},{"LD B,d8", &a::ld_b_n, 2},{"RLCA", &a::undefined, 1},{"LD (a16),SP", &a::undefined, 3},{"ADD HL,BC", &a::undefined, 1},{"LD A,(BC)", &a::undefined, 1},{"DEC BC", &a::undefined, 1},{"INC C", &a::undefined, 1},{"DEC C", &a::dec_c, 1},{"LD C,d8", &a::ld_c_n, 2},{"RRCA", &a::undefined, 1},
		{"STOP 0", &a::undefined, 2},{"LD DE,d16", &a::ld_de_nn, 3},{"LD (DE),A", &a::ld_de_a, 1},{"INC DE", &a::inc_de, 1},{"INC D", &a::inc_d, 1},{"DEC D", &a::undefined, 1},{"LD D,d8", &a::undefined, 2},{"RLA", &a::undefined, 1},{"JR r8", &a::jr_n, 2},{"ADD HL,DE", &a::undefined, 1},{"LD A,(DE)", &a::ld_a_de, 1},{"DEC DE", &a::undefined, 1},{"INC E", &a::inc_e, 1},{"DEC E", &a::dec_e, 1},{"LD E,d8", &a::undefined, 2},{"RRA", &a::rra, 1}, 
		{"JR NZ r8", &a::jr_nz_n, 2},{"LD HL,d16", &a::ld_hl_nn, 3},{"LD (HL+),A", &a::ldi_hl, 1},{"INC HL", &a::inc_hl, 1},{"INC H", &a::inc_h, 1},{"DEC H", &a::dec_h, 1},{"LD H,d8", &a::ld_h_n, 2},{"DAA", &a::undefined, 1},{"JR Z r8", &a::jr_z_n, 2},{"ADD HL,HL", &a::add_hl_hl, 1},{"LD A,(HL+)", &a::ldi_a, 1},{"DEC HL", &a::undefined, 1},{"INC L", &a::inc_l, 1},{"DEC L", &a::dec_l, 1},{"LD L,d8", &a::ld_l_n, 2},{"CPL", &a::undefined, 1}, 
		{"JR NC r8", &a::jr_nc, 2},{"LD SP,d16", &a::ld_sp_nn, 3},{"LD (HL-),A", &a::ldd_a, 1},{"INC SP", &a::undefined, 1},{"INC (HL)", &a::undefined, 1},{"DEC (HL)", &a::dec_hla, 1},{"LD (HL),d8", &a::undefined, 2},{"SCF", &a::undefined, 1},{"JR C r8", &a::jr_c_n, 2},{"ADD HL,SP", &a::undefined, 1},{"LD A,(HL-)", &a::undefined, 1},{"DEC SP", &a::undefined, 1},{"INC A", &a::inc_a, 1},{"DEC A", &a::dec_a, 1},{"LD A,d8", &a::ld_a_n, 2},{"CCF", &a::undefined, 1}, 
		{"LD B,B", &a::ld_b_b, 1},{"LD B,C", &a::ld_b_c, 1},{"LD B,D", &a::ld_b_d, 1},{"LD B,E", &a::ld_b_e, 1},{"LD B,H", &a::ld_b_h, 1},{"LD B,L", &a::ld_b_l, 1},{"LD B,(HL)", &a::ld_b_hl, 1},{"LD B,A", &a::ld_b_a, 1},{"LD C,B", &a::ld_c_b, 1},{"LD C,C", &a::ld_c_c, 1},{"LD C,D", &a::ld_c_d, 1},{"LD C,E", &a::ld_c_e, 1},{"LD C,H", &a::ld_c_h, 1},{"LD C,L", &a::ld_c_l, 1},{"LD C,(HL)", &a::ld_c_hl, 1},{"LD C,A", &a::ld_c_a, 1}, 
		{"LD D,B", &a::ld_d_b, 1},{"LD D,C", &a::ld_d_c, 1},{"LD D,D", &a::ld_d_d, 1},{"LD D,E", &a::ld_d_e, 1},{"LD D,H", &a::ld_d_h, 1},{"LD D,L", &a::ld_d_l, 1},{"LD D,(HL)", &a::ld_d_hl, 1},{"LD D,A", &a::ld_d_a, 1},{"LD E,B", &a::ld_e_b, 1},{"LD E,C", &a::ld_e_c, 1},{"LD E,D", &a::ld_e_d, 1},{"LD E,E", &a::ld_e_e, 1},{"LD E,H", &a::ld_e_h, 1},{"LD E,L", &a::ld_e_l, 1},{"LD E,(HL)", &a::ld_e_hl, 1},{"LD E,A", &a::ld_e_a, 1}, 
		{"LD H,B", &a::ld_h_b, 1},{"LD H,C", &a::ld_h_c, 1},{"LD H,D", &a::ld_h_d, 1},{"LD H,E", &a::ld_h_e, 1},{"LD H,H", &a::ld_h_h, 1},{"LD H,L", &a::ld_h_l , 1},{"LD H,(HL)", &a::ld_h_hl, 1},{"LD H,A", &a::ld_h_a, 1},{"LD L,B", &a::ld_l_b, 1},{"LD L,C", &a::ld_l_c, 1},{"LD L,D", &a::ld_l_d, 1},{"LD L,E", &a::ld_l_e, 1},{"LD L,H", &a::ld_l_h, 1},{"LD L,L", &a::ld_l_l , 1},{"LD L,(HL)", &a::ld_l_hl, 1},{"LD L,A", &a::ld_l_a, 1},
		{"LD (HL),B", &a::ld_hl_b, 1},{"LD (HL),C", &a::ld_hl_c, 1},{"LD (HL),D", &a::ld_hl_d, 1},{"LD (HL),E", &a::ld_hl_e, 1},{"LD (HL),H", &a::ld_hl_h, 1},{"LD (HL),L", &a::ld_hl_l, 1},{"HALT", &a::undefined, 1},{"LD (HL),A", &a::ld_hl_a, 1},{"LD A,B", &a::ld_a_b, 1},{"LD A,C", &a::ld_a_c, 1},{"LD A,D", &a::ld_a_d, 1},{"LD A,E", &a::ld_a_e, 1},{"LD A,H", &a::ld_a_h, 1},{"LD A,L", &a::ld_a_l, 1},{"LD A,(HL)", &a::ld_a_hl, 1},{"LD A,A", &a::ld_a_a, 1}, 
		{"ADD A,B", &a::undefined, 1},{"ADD A,C", &a::undefined, 1},{"ADD A,D", &a::undefined, 1},{"ADD A,E", &a::undefined, 1},{"ADD A,H", &a::undefined, 1},{"ADD A,L", &a::undefined, 1},{"ADD A,(HL)", &a::undefined, 1},{"ADD A,A", &a::undefined, 1},{"ADC A,B", &a::undefined, 1},{"ADC A,C", &a::undefined, 1},{"ADC A,D", &a::undefined, 1},{"ADC A,E", &a::undefined, 1},{"ADC A,H", &a::undefined, 1},{"ADC A,L", &a::undefined, 1},{"ADC A,(HL)", &a::undefined, 1},{"ADC A,A", &a::undefined, 1}, 
		{"SUB B", &a::undefined, 1},{"SUB C", &a::undefined, 1},{"SUB D", &a::undefined, 1},{"SUB E", &a::undefined, 1},{"SUB H", &a::undefined, 1},{"SUB L", &a::undefined, 1},{"SUB (HL)", &a::undefined, 1},{"SUB A", &a::undefined, 1},{"SBC A,B", &a::undefined, 1},{"SBC A,C", &a::undefined, 1},{"SBC A,D", &a::undefined, 1},{"SBC A,E", &a::undefined, 1},{"SBC A,H", &a::undefined, 1},{"SBC A,L", &a::undefined, 1},{"SBC A,(HL)", &a::undefined, 1},{"SBC A,A", &a::undefined, 1},
		{"AND B", &a::undefined, 1},{"AND C", &a::undefined, 1},{"AND D", &a::undefined, 1},{"AND E", &a::undefined, 1},{"AND H", &a::undefined, 1},{"AND L", &a::undefined, 1},{"AND (HL)", &a::undefined, 1},{"AND A", &a::undefined, 1},{"XOR B", &a::undefined, 1},{"XOR C", &a::xor_c, 1},{"XOR D", &a::undefined, 1},{"XOR E", &a::undefined, 1},{"XOR H", &a::undefined, 1},{"XOR L", &a::xor_l, 1},{"XOR (HL)", &a::xor_hl, 1},{"XOR A", &a::xor_a, 1},
		{"OR B", &a::or_b, 1},{"OR C", &a::or_c, 1},{"OR D", &a::undefined, 1},{"OR E", &a::undefined, 1},{"OR H", &a::undefined, 1},{"OR L", &a::undefined, 1},{"OR (HL)", &a::or_hl, 1},{"OR A", &a::or_a, 1},{"CP B", &a::undefined, 1},{"CP C", &a::undefined, 1},{"CP D", &a::undefined, 1},{"CP E", &a::undefined, 1},{"CP H", &a::undefined, 1},{"CP L", &a::undefined, 1},{"CP (HL)", &a::undefined, 1},{"CP A", &a::undefined, 1},
		{"RET NZ", &a::undefined, 1},{"POP BC", &a::pop_bc, 1},{"JP NZ,a16", &a::undefined, 3},{"JP a16", &a::jp_nn, 3},{"CALL NZ,a16", &a::call_nz, 3},{"PUSH BC", &a::push_bc, 1},{"ADD A,d8", &a::add_n, 2},{"RST 00H", &a::undefined, 1},{"RET Z", &a::ret_z, 1},{"RET", &a::ret, 1},{"JP Z,a16", &a::undefined, 3},{"PREFIX CB", &a::cb, 1},{"CALL Z,a16", &a::undefined, 3},{"CALL a16", &a::call_nn, 3},{"ADC A,d8", &a::adc_n, 2},{"RST 08H", &a::undefined, 1},
		{"RET NC", &a::ret_nc, 1},{"POP DE", &a::pop_de, 1},{"JP NC,a16", &a::undefined, 3},{"NOP", &a::nop, 1},{"CALL NC,a16", &a::undefined, 3},{"PUSH DE", &a::push_de, 1},{"SUB d8", &a::sub_n, 2},{"RST 10H", &a::undefined, 1},{"RET C", &a::ret_c, 1},{"RETI", &a::undefined, 1},{"JP C,a16", &a::undefined, 3},{"NOP", &a::nop, 1},{"CALL C,a16", &a::undefined, 1},{"NOP", &a::nop, 1},{"SBC A,d8", &a::undefined, 2},{"RST 18H", &a::undefined, 1},
		{"LDH (a8),A", &a::ldh_n_a, 2},{"POP HL", &a::pop_hl, 1},{"LD(C),A", &a::undefined, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"PUSH HL", &a::push_hl, 1},{"AND d8", &a::and_n, 2},{"RST 20H", &a::undefined, 1},{"ADD SP,r8", &a::undefined, 2},{"JP (HL)", &a::jp_hl, 1},{"LD (a16),A", &a::ld_nn_a, 3},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"XOR d8", &a::xor_n, 2},{"RST 28H", &a::undefined, 1},
		{"LDH A,(a8)", &a::ldh_a_n, 2},{"POP AF", &a::pop_af, 1},{"LD A,(C)", &a::undefined, 1},{"DI", &a::di, 1},{"NOP", &a::undefined, 1},{"PUSH AF", &a::push_af, 1},{"OR d8", &a::undefined, 2},{"RST 30H", &a::undefined, 1},{"LD HL,SP+r8", &a::undefined, 2},{"LD SP,HL", &a::undefined, 1},{"LD A,(a16)", &a::ld_a_nn, 3},{"EI", &a::undefined, 1},{"NOP", &a::nop, 1},{"NOP", &a::nop, 1},{"CP d8", &a::cp_n, 2},{"RST 38H", &a::undefined, 1}, 
	};

	extended_lookup =
	{
		{"RLC B", &a::undefined_cb, 1},{"RLC C", &a::undefined_cb, 1},{"RLC D", &a::undefined_cb, 1},{"RLC E", &a::undefined_cb, 1},{"RLC H", &a::undefined_cb, 1},{"RLC L", &a::undefined_cb, 1},{"RLC (HL)", &a::undefined_cb, 1},{"RLC A", &a::undefined_cb, 1},{"RRC B", &a::undefined_cb, 1},{"RRC C", &a::undefined_cb, 1},{"RRC D", &a::undefined_cb, 1},{"RRC E", &a::undefined_cb, 1},{"RRC H", &a::undefined_cb, 1},{"RRC L", &a::undefined_cb, 1},{"RRC (HL)", &a::undefined_cb, 1},{"RRC A", &a::undefined_cb, 1},
		{"RL B", &a::undefined_cb, 1},{"RL C", &a::undefined_cb, 1},{"RL D", &a::undefined_cb, 1},{"RL E", &a::undefined_cb, 1},{"RL H", &a::undefined_cb, 1},{"RL L", &a::undefined_cb, 1},{"RL (HL)", &a::undefined_cb, 1},{"RL A", &a::undefined_cb, 1},{"RR B", &a::undefined_cb, 1},{"RR C", &a::rr_c, 1},{"RR D", &a::rr_d, 1},{"RR E", &a::rr_e, 1},{"RR H", &a::undefined_cb, 1},{"RR L", &a::undefined_cb, 1},{"RR (HL)", &a::undefined_cb, 1},{"RR A", &a::undefined_cb, 1},
		{"SLA B", &a::undefined_cb, 1},{"SLA C", &a::undefined_cb, 1},{"SLA D", &a::undefined_cb, 1},{"SLA E", &a::undefined_cb, 1},{"SLA H", &a::undefined_cb, 1},{"SLA L", &a::undefined_cb, 1},{"SLA (HL)", &a::undefined_cb, 1},{"SLA A", &a::undefined_cb, 1},{"SRA B", &a::undefined_cb, 1},{"SRA C", &a::undefined_cb, 1},{"SRA D", &a::undefined_cb, 1},{"SRA E", &a::undefined_cb, 1},{"SRA H", &a::undefined_cb, 1},{"SRA L", &a::undefined_cb, 1},{"SRA (HL)", &a::undefined_cb, 1},{"SRA A", &a::undefined_cb, 1},
		{"SWAP B", &a::undefined_cb, 1},{"SWAP C", &a::undefined_cb, 1},{"SWAP D", &a::undefined_cb, 1},{"SWAP E", &a::undefined_cb, 1},{"SWAP H", &a::undefined_cb, 1},{"SWAP L", &a::undefined_cb, 1},{"SWAP (HL)", &a::undefined_cb, 1},{"SWAP A", &a::swap_a, 1},{"SRL B", &a::srl_b , 1},{"SRL C", &a::undefined_cb, 1},{"SRL D", &a::undefined_cb, 1},{"SRL E", &a::undefined_cb, 1},{"SRL H", &a::undefined_cb, 1},{"SRL L", &a::undefined_cb, 1},{"SRL (HL)", &a::ld_a_n, 1},{"SRL A", &a::undefined_cb, 1},
		{"BIT 0,B", &a::undefined_cb, 1},{"BIT 0,C", &a::undefined_cb, 1},{"BIT 0,D", &a::undefined_cb, 1},{"BIT 0,E", &a::undefined_cb, 1},{"BIT 0,H", &a::undefined_cb, 1},{"BIT 0,L", &a::undefined_cb, 1},{"BIT 0,(HL)", &a::undefined_cb, 1},{"BIT 0,A", &a::undefined_cb, 1},{"BIT 1,B", &a::undefined_cb, 1},{"BIT 1,C", &a::undefined_cb, 1},{"BIT 1,D", &a::undefined_cb, 1},{"BIT 1,E", &a::undefined_cb, 1},{"BIT 1,H", &a::undefined_cb, 1},{"BIT 1,L", &a::undefined_cb, 1},{"BIT 1,(HL)", &a::undefined_cb, 1},{"BIT 1,A", &a::undefined_cb, 1},
		{"BIT 2,B", &a::undefined_cb, 1},{"BIT 2,C", &a::undefined_cb, 1},{"BIT 2,D", &a::undefined_cb, 1},{"BIT 2,E", &a::undefined_cb, 1},{"BIT 2,H", &a::undefined_cb, 1},{"BIT 2,L", &a::undefined_cb, 1},{"BIT 2,(HL)", &a::undefined_cb, 1},{"BIT 2,A", &a::undefined_cb, 1},{"BIT 3,B", &a::undefined_cb, 1},{"BIT 3,C", &a::undefined_cb, 1},{"BIT 3,D", &a::undefined_cb, 1},{"BIT 3,E", &a::undefined_cb, 1},{"BIT 3,H", &a::undefined_cb, 1},{"BIT 3,L", &a::undefined_cb, 1},{"BIT 3,(HL)", &a::undefined_cb, 1},{"BIT 3,A", &a::undefined_cb, 1},
		{"BIT 4,B", &a::undefined_cb, 1},{"BIT 4,C", &a::undefined_cb, 1},{"BIT 4,C", &a::undefined_cb, 1},{"BIT 4,E", &a::undefined_cb, 1},{"BIT 4,H", &a::undefined_cb, 1},{"BIT 4,L", &a::undefined_cb, 1},{"BIT 4,(HL)", &a::undefined_cb, 1},{"BIT 4,A", &a::undefined_cb, 1},{"BIT 5,B", &a::undefined_cb, 1},{"BIT 5,C", &a::undefined_cb, 1},{"BIT 5,D", &a::undefined_cb, 1},{"BIT 5,E", &a::undefined_cb, 1},{"BIT 5,H", &a::undefined_cb, 1},{"BIT 5,L", &a::undefined_cb, 1},{"BIT 5,(HL)", &a::undefined_cb, 1},{"BIT 5,A", &a::undefined_cb, 1},
		{"BIT 6,B", &a::undefined_cb, 1},{"BIT 6,C", &a::undefined_cb, 1},{"BIT 6,D", &a::undefined_cb, 1},{"BIT 6,E", &a::undefined_cb, 1},{"BIT 6,H", &a::undefined_cb, 1},{"BIT 6,L", &a::undefined_cb, 1},{"BIT 6,(HL)", &a::undefined_cb, 1},{"BIT 6,A", &a::undefined_cb, 1},{"BIT 7,B", &a::undefined_cb, 1},{"BIT 7,C", &a::undefined_cb, 1},{"BIT 7,D", &a::undefined_cb, 1},{"BIT 7,E", &a::undefined_cb, 1},{"BIT 7,H", &a::undefined_cb, 1},{"BIT 7,L", &a::undefined_cb, 1},{"BIT 7,(HL)", &a::undefined_cb, 1},{"BIT 7,A", &a::undefined_cb, 1},
		{"RES 0,B", &a::undefined_cb, 1},{"RES 0,C", &a::undefined_cb, 1},{"RES 0,D", &a::undefined_cb, 1},{"RES 0,E", &a::undefined_cb, 1},{"RES 0,H", &a::undefined_cb, 1},{"RES 0,L", &a::undefined_cb, 1},{"RES 0,(HL)", &a::undefined_cb, 1},{"RES 0,A", &a::undefined_cb, 1},{"RES 1,B", &a::undefined_cb, 1},{"RES 1,C", &a::undefined_cb, 1},{"RES 1,D", &a::undefined_cb, 1},{"RES 1,E", &a::undefined_cb, 1},{"RES 1,H", &a::undefined_cb, 1},{"RES 1,L", &a::undefined_cb, 1},{"RES 1,(HL)", &a::undefined_cb, 1},{"RES 1,A", &a::undefined_cb, 1},
		{"RES 2,B", &a::undefined_cb, 1},{"RES 2,C", &a::undefined_cb, 1},{"RES 2,D", &a::undefined_cb, 1},{"RES 2,E", &a::undefined_cb, 1},{"RES 2,H", &a::undefined_cb, 1},{"RES 2,L", &a::undefined_cb, 1},{"RES 2,(HL)", &a::undefined_cb, 1},{"RES 2,A", &a::undefined_cb, 1},{"RES 3,B", &a::undefined_cb, 1},{"RES 3,C", &a::undefined_cb, 1},{"RES 3,D", &a::undefined_cb, 1},{"RES 3,E", &a::undefined_cb, 1},{"RES 3,H", &a::undefined_cb, 1},{"RES 3,L", &a::undefined_cb, 1},{"RES 3,(HL)", &a::undefined_cb, 1},{"RES 3,A", &a::undefined_cb, 1},
		{"RES 4,B", &a::undefined_cb, 1},{"RES 4,C", &a::undefined_cb, 1},{"RES 4,C", &a::undefined_cb, 1},{"RES 4,E", &a::undefined_cb, 1},{"RES 4,H", &a::undefined_cb, 1},{"RES 4,L", &a::undefined_cb, 1},{"RES 4,(HL)", &a::undefined_cb, 1},{"RES 4,A", &a::undefined_cb, 1},{"RES 5,B", &a::undefined_cb, 1},{"RES 5,C", &a::undefined_cb, 1},{"RES 5,D", &a::undefined_cb, 1},{"RES 5,E", &a::undefined_cb, 1},{"RES 5,H", &a::undefined_cb, 1},{"RES 5,L", &a::undefined_cb, 1},{"RES 5,(HL)", &a::undefined_cb, 1},{"RES 5,A", &a::undefined_cb, 1},
		{"RES 6,B", &a::undefined_cb, 1},{"RES 6,C", &a::undefined_cb, 1},{"RES 6,D", &a::undefined_cb, 1},{"RES 6,E", &a::undefined_cb, 1},{"RES 6,H", &a::undefined_cb, 1},{"RES 6,L", &a::undefined_cb, 1},{"RES 6,(HL)", &a::undefined_cb, 1},{"RES 6,A", &a::undefined_cb, 1},{"RES 7,B", &a::undefined_cb, 1},{"RES 7,C", &a::undefined_cb, 1},{"RES 7,D", &a::undefined_cb, 1},{"RES 7,E", &a::undefined_cb, 1},{"RES 7,H", &a::undefined_cb, 1},{"RES 7,L", &a::undefined_cb, 1},{"RES 7,(HL)", &a::undefined_cb, 1},{"RES 7,A", &a::undefined_cb, 1},
		{"SET 0,B", &a::undefined_cb, 1},{"SET 0,C", &a::undefined_cb, 1},{"SET 0,D", &a::undefined_cb, 1},{"SET 0,E", &a::undefined_cb, 1},{"SET 0,H", &a::undefined_cb, 1},{"SET 0,L", &a::undefined_cb, 1},{"SET 0,(HL)", &a::undefined_cb, 1},{"SET 0,A", &a::undefined_cb, 1},{"SET 1,B", &a::undefined_cb, 1},{"SET 1,C", &a::undefined_cb, 1},{"SET 1,D", &a::undefined_cb, 1},{"SET 1,E", &a::undefined_cb, 1},{"SET 1,H", &a::undefined_cb, 1},{"SET 1,L", &a::undefined_cb, 1},{"SET 1,(HL)", &a::undefined_cb, 1},{"SET 1,A", &a::undefined_cb, 1},
		{"SET 2,B", &a::undefined_cb, 1},{"SET 2,C", &a::undefined_cb, 1},{"SET 2,D", &a::undefined_cb, 1},{"SET 2,E", &a::undefined_cb, 1},{"SET 2,H", &a::undefined_cb, 1},{"SET 2,L", &a::undefined_cb, 1},{"SET 2,(HL)", &a::undefined_cb, 1},{"SET 2,A", &a::undefined_cb, 1},{"SET 3,B", &a::undefined_cb, 1},{"SET 3,C", &a::undefined_cb, 1},{"SET 3,D", &a::undefined_cb, 1},{"SET 3,E", &a::undefined_cb, 1},{"SET 3,H", &a::undefined_cb, 1},{"SET 3,L", &a::undefined_cb, 1},{"SET 3,(HL)", &a::undefined_cb, 1},{"SET 3,A", &a::undefined_cb, 1},
		{"SET 4,B", &a::undefined_cb, 1},{"SET 4,C", &a::undefined_cb, 1},{"SET 4,C", &a::undefined_cb, 1},{"SET 4,E", &a::undefined_cb, 1},{"SET 4,H", &a::undefined_cb, 1},{"SET 4,L", &a::undefined_cb, 1},{"SET 4,(HL)", &a::undefined_cb, 1},{"SET 4,A", &a::undefined_cb, 1},{"SET 5,B", &a::undefined_cb, 1},{"SET 5,C", &a::undefined_cb, 1},{"SET 5,D", &a::undefined_cb, 1},{"SET 5,E", &a::undefined_cb, 1},{"SET 5,H", &a::undefined_cb, 1},{"SET 5,L", &a::undefined_cb, 1},{"SET 5,(HL)", &a::undefined_cb, 1},{"SET 5,A", &a::undefined_cb, 1},
		{"SET 6,B", &a::undefined_cb, 1},{"SET 6,C", &a::undefined_cb, 1},{"SET 6,D", &a::undefined_cb, 1},{"SET 6,E", &a::undefined_cb, 1},{"SET 6,H", &a::undefined_cb, 1},{"SET 6,L", &a::undefined_cb, 1},{"SET 6,(HL)", &a::undefined_cb, 1},{"SET 6,A", &a::undefined_cb, 1},{"SET 7,B", &a::undefined_cb, 1},{"SET 7,C", &a::undefined_cb, 1},{"SET 7,D", &a::undefined_cb, 1},{"SET 7,E", &a::undefined_cb, 1},{"SET 7,H", &a::undefined_cb, 1},{"SET 7,L", &a::undefined_cb, 1},{"SET 7,(HL)", &a::undefined_cb, 1},{"SET 7,A", &a::undefined_cb, 1},
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
	value += FLAGS_ISCARRY ? 1 : 0;
	add(value);
}

void CPU::sub(uint8_t value)
{
	value > regs.a ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	(value & 0x0F) > (regs.a & 0x0F) ? FLAGS_SET(FLAGS_HALFCARRY) : FLAGS_CLEAR(FLAGS_HALFCARRY);
	regs.a -= value;
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
{}

// 0x01
void CPU::ld_bc_nn()
{
	regs.bc = gb->mmu.readShort(pc);
	pc += 2;
}

// 0x03
void CPU::inc_bc()
{
	++regs.bc;
}

// 0x05
void CPU::dec_b()
{
	regs.b = dec(regs.b);
}

// 0x06
void CPU::ld_b_n()
{
	regs.b = gb->mmu.readByte(pc++);
}

// 0x0D
void CPU::dec_c()
{
	regs.c = dec(regs.c);
}

// 0x0E:
void CPU::ld_c_n()
{
	regs.c = gb->mmu.readByte(pc++);
}

// 0x11
void CPU::ld_de_nn()
{
	regs.de = gb->mmu.readShort(pc);
	pc += 2;
}

// 0x12
void CPU::ld_de_a()
{
	gb->mmu.writeByte(regs.de, regs.a);
}

// 0x13
void CPU::inc_de()
{
	++regs.de;
}

// 0x14:
void CPU::inc_d()
{
	regs.d = inc(regs.d);
}

// 0x18
void CPU::jr_n()
{
	jr();
}

// 0x1A
void CPU::ld_a_de()
{
	regs.a = gb->mmu.readByte(regs.de);
}

// 0x1C
void CPU::inc_e()
{
	regs.e = inc(regs.e);
}

// 0x1D
void CPU::dec_e()
{
	regs.e = dec(regs.e);
}

// 0x1F
void CPU::rra()
{
	int old_carry = (FLAGS_ISCARRY ? 1 : 0) << 7;
	regs.a & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	regs.a >>= 1;
	regs.a += old_carry;
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

// 0x20
void CPU::jr_nz_n()
{
	if (!FLAGS_ISZERO)
	{
		jr();
	}
	else
	{
		++pc;
	}
}

// 0x21
void CPU::ld_hl_nn()
{
	regs.hl = gb->mmu.readShort(pc);
	pc += 2;
}

// 0x22
void CPU::ldi_hl()
{
	gb->mmu.writeByte(regs.hl++, regs.a);
}

// 0x23
void CPU::inc_hl()
{
	++regs.hl;
}

// 0x24
void CPU::inc_h()
{
	regs.h = inc(regs.h);
}

// 0x25
void CPU::dec_h()
{
	regs.h = dec(regs.h);
}

// 0x26
void CPU::ld_h_n()
{
	regs.h = gb->mmu.readByte(pc++);
}

// 0x28
void CPU::jr_z_n()
{
	if (FLAGS_ISZERO)
	{
		jr();
	}
	else
	{
		++pc;
	}
}

// 0x29
void CPU::add_hl_hl()
{
	add16(regs.hl);
}

// 0x2A
void CPU::ldi_a()
{
	regs.a = gb->mmu.readByte(regs.hl++);
}

// 0x2C
void CPU::inc_l()
{
	regs.l = inc(regs.l);
}

// 0x2D
void CPU::dec_l()
{
	regs.l = dec(regs.l);
}

// 0x2E
void CPU::ld_l_n()
{
	regs.l = gb->mmu.readByte(pc++);
}

// 0x30
void CPU::jr_nc()
{
	if (!FLAGS_ISCARRY)
	{
		jr();
	}
	else
	{
		++pc;
	}
}

// 0x31
void CPU::ld_sp_nn()
{
	sp = gb->mmu.readShort(pc);
	pc += 2;
}

// 0x32
void CPU::ldd_a()
{
	gb->mmu.writeByte(regs.hl--, regs.a);
}

// 0x35
void CPU::dec_hla()
{
	gb->mmu.writeByte(regs.hl, dec(gb->mmu.readByte(regs.hl)));
}

// 0x38
void CPU::jr_c_n()
{
	if (FLAGS_CARRY)
	{
		jr();
	}
	else
	{
		++pc;
	}
}

void CPU::inc_a()
{
	regs.a = inc(regs.a);
}

// 0x3D
void CPU::dec_a()
{
	regs.a = dec(regs.a);
}

// 0x3E
void CPU::ld_a_n()
{
	regs.a = gb->mmu.readByte(pc++);
}

// 0x40
void CPU::ld_b_b()
{
	regs.b = regs.b;
}

// 0x41
void CPU::ld_b_c()
{
	regs.b = regs.c;
}

// 0x42
void CPU::ld_b_d()
{
	regs.b = regs.d;
}

// 0x43
void CPU::ld_b_e()
{
	regs.b = regs.e;
}

// 0x44
void CPU::ld_b_h()
{
	regs.b = regs.h;
}

// 0x45
void CPU::ld_b_l()
{
	regs.b = regs.l;
}

// 0x46
void CPU::ld_b_hl()
{
	regs.b = gb->mmu.readByte(regs.hl);
}

// 0x47
void CPU::ld_b_a()
{
	regs.b = regs.a;
}

// 0x48
void CPU::ld_c_b()
{
	regs.c = regs.b;
}

// 0x49
void CPU::ld_c_c()
{
	regs.c = regs.c;
}

// 0x4A
void CPU::ld_c_d()
{
	regs.c = regs.d;
}

// 0x4B
void CPU::ld_c_e()
{
	regs.c = regs.e;
}

// 0x4C
void CPU::ld_c_h()
{
	regs.c = regs.h;
}

// 0x4D
void CPU::ld_c_l()
{
	regs.c = regs.l;
}

// 0x4E
void CPU::ld_c_hl()
{
	regs.c = gb->mmu.readByte(regs.hl);
}

// 0x4F
void CPU::ld_c_a()
{
	regs.c = regs.a;
}

// 0x50
void CPU::ld_d_b()
{
	regs.d = regs.b;
}

// 0x51
void CPU::ld_d_c()
{
	regs.d = regs.c;
}

// 0x52
void CPU::ld_d_d()
{
	regs.d = regs.d;
}

// 0x53
void CPU::ld_d_e()
{
	regs.d = regs.e;
}

// 0x54
void CPU::ld_d_h()
{
	regs.d = regs.h;
}

// 0x55
void CPU::ld_d_l()
{
	regs.d = regs.l;
}


// 0x56
void CPU::ld_d_hl()
{
	regs.d = gb->mmu.readByte(regs.hl);
}

// 0x57
void CPU::ld_d_a()
{
	regs.d = regs.a;
}


// 0x58
void CPU::ld_e_b()
{
	regs.e = regs.b;
}

// 0x59
void CPU::ld_e_c()
{
	regs.e = regs.c;
}

// 0x5A
void CPU::ld_e_d()
{
	regs.e = regs.d;
}

// 0x5B
void CPU::ld_e_e()
{
	regs.e = regs.e;
}

// 0x5C
void CPU::ld_e_h()
{
	regs.e = regs.h;
}

// 0x5D
void CPU::ld_e_l()
{
	regs.e = regs.l;	
}

// 0x5E
void CPU::ld_e_hl()
{
	regs.e = gb->mmu.readByte(regs.hl);
}

// 0x5F
void CPU::ld_e_a()
{
	regs.e = regs.a;
}

// 0x60
void CPU::ld_h_b()
{
	regs.h = regs.b;
}

// 0x61
void CPU::ld_h_c()
{
	regs.h = regs.c;
}

// 0x62
void CPU::ld_h_d()
{
	regs.h = regs.d;
}

// 0x63
void CPU::ld_h_e()
{
	regs.h = regs.e;
}

// 0x64
void CPU::ld_h_h()
{
	regs.h = regs.h;
}

// 0x65
void CPU::ld_h_l()
{
	regs.h = regs.l;
}

// 0x66
void CPU::ld_h_hl()
{
	regs.h = gb->mmu.readByte(regs.hl);
}

// 0x67
void CPU::ld_h_a()
{
	regs.h = regs.a;
}

// 0x68
void CPU::ld_l_b()
{
	regs.l = regs.b;
}

// 0x69
void CPU::ld_l_c()
{
	regs.l = regs.c;
}

// 0x6A
void CPU::ld_l_d()
{
	regs.l = regs.d;
}

// 0x6B
void CPU::ld_l_e()
{
	regs.l = regs.e;
}

// 0x6C
void CPU::ld_l_h()
{
	regs.l = regs.h;
}

// 0x6D
void CPU::ld_l_l()
{
	regs.l = regs.l;
}

// 0x6E
void CPU::ld_l_hl()
{
	regs.l = gb->mmu.readByte(regs.hl);
}

// 0x6F
void CPU::ld_l_a()
{
	regs.l = regs.a;
}

// 0x70
void CPU::ld_hl_b()
{
	gb->mmu.writeByte(regs.hl, regs.b);
}

// 0x71
void CPU::ld_hl_c()
{
	gb->mmu.writeByte(regs.hl, regs.c);
}

// 0x72
void CPU::ld_hl_d()
{
	gb->mmu.writeByte(regs.hl, regs.d);

}

// 0x73
void CPU::ld_hl_e()
{
	gb->mmu.writeByte(regs.hl, regs.e);
}

// 0x74
void CPU::ld_hl_h()
{
	gb->mmu.writeByte(regs.hl, regs.h);
}

// 0x75
void CPU::ld_hl_l()
{
	gb->mmu.writeByte(regs.hl, regs.l);
}

// 0x77
void CPU::ld_hl_a()
{
	gb->mmu.writeByte(regs.hl, regs.a);
}

// 0x78
void CPU::ld_a_b()
{
	regs.a = regs.b;
}

// 0x79
void CPU::ld_a_c()
{
	regs.a = regs.c;
}

// 0x7A
void CPU::ld_a_d()
{
	regs.a = regs.d;
}

// 0x7B
void CPU::ld_a_e()
{
	regs.a = regs.e;
}

// 0x7C
void CPU::ld_a_h()
{
	regs.a = regs.h;
}

// 0x7D
void CPU::ld_a_l()
{
	regs.a = regs.l;
}

// 0x7E
void CPU::ld_a_hl()
{
	regs.a = gb->mmu.readByte(regs.hl);
}

// 0x7F
void CPU::ld_a_a()
{
	regs.a = regs.a;
}

// 0xA9
void CPU::xor_c()
{
	XOR(regs.c);
}

// 0xAD
void CPU::xor_l()
{
	XOR(regs.l);
}

// 0xAE
void CPU::xor_hl()
{
	XOR(gb->mmu.readByte(regs.hl));
}

// 0xAF
void CPU::xor_a()
{
	XOR(regs.a);
}

// 0xB0
void CPU::or_b()
{
	OR(regs.b);
}

// 0xB1
void CPU::or_c()
{
	OR(regs.c);
}

// 0xB6
void CPU::or_hl()
{
	OR(gb->mmu.readByte(regs.hl));
}

// 0xB7
void CPU::or_a()
{
	OR(regs.a);
}

// 0xC1
void CPU::pop_bc()
{
	regs.bc = pop();
}

// 0xC3
void CPU::jp_nn()
{
	jp(gb->mmu.readShort(pc));
}

void CPU::call_nz()
{
	if (!FLAGS_ISZERO)
	{
		call(gb->mmu.readShort(pc));
	}
	else
	{
		pc += 2;
	}
}

// 0xC5
void CPU::push_bc()
{
	push(regs.bc);
}

// 0xC6
void CPU::add_n()
{
	add(gb->mmu.readByte(pc++));
}

// 0xC8
void CPU::ret_z()
{
	if (FLAGS_ISZERO)
	{
		jp(pop());
	}
}

// 0xC9
void CPU::ret()
{
	jp(pop());
}

// 0xCB
void CPU::cb()
{
	cb_instr();
}

// 0xCD
void CPU::call_nn()
{
	call(gb->mmu.readShort(pc));
}

// 0xCE
void CPU::adc_n()
{
	adc(gb->mmu.readByte(pc++));
}

// 0xD0
void CPU::ret_nc()
{
	if (!FLAGS_ISCARRY)
	{
		jp(pop());
	}
}



// 0xD1
void CPU::pop_de()
{
	regs.de = pop();
}

// 0xD5
void CPU::push_de()
{
	push(regs.de);
}

// 0xD6
void CPU::sub_n()
{
	sub(gb->mmu.readByte(pc++));
}

void CPU::ret_c()
{
	if (FLAGS_ISCARRY)
	{
		jp(pop());
	}
}

// 0xE0
void CPU::ldh_n_a()
{
	uint16_t addr = 0xFF00 + gb->mmu.readByte(pc++);
	gb->mmu.writeByte(addr, regs.a);
}

// 0xE1
void CPU::pop_hl()
{
	regs.hl = pop();
}

// 0xE5
void CPU::push_hl()
{
	push(regs.hl);
}

// 0xE6
void CPU::and_n()
{
	AND(gb->mmu.readByte(pc++));
}

// 0xE9
void CPU::jp_hl()
{
	jp(regs.hl);
}

// 0xEA
void CPU::ld_nn_a()
{
	gb->mmu.writeByte(gb->mmu.readShort(pc), regs.a);
	pc += 2;
}

// 0xEE
void CPU::xor_n()
{
	XOR(gb->mmu.readByte(pc++));
}

// 0xF0
void CPU::ldh_a_n()
{
	uint16_t addr = 0xFF00 + gb->mmu.readByte(pc++);
	regs.a = gb->mmu.readByte(addr);
}

// 0xF3
void CPU::pop_af()
{
	regs.af = pop();
}

// 0xF3
void CPU::di()
{
	interrupts_enabled = false;
}

// 0xF5
void CPU::push_af()
{
	push(regs.af);
}

// 0xFA
void CPU::ld_a_nn()
{
	uint16_t addr = gb->mmu.readShort(pc);
	regs.a = gb->mmu.readByte(addr);
	pc += 2;
}

// 0xFE
void CPU::cp_n()
{
	cp(gb->mmu.readByte(pc++));
}

uint8_t CPU::srl(uint8_t value)
{
	value & 0x01 ? FLAGS_SET(FLAGS_CARRY) : FLAGS_CLEAR(FLAGS_CARRY);
	value >>= 1;
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

uint8_t CPU::swap(uint8_t value)
{
	value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
	value == 0 ? FLAGS_SET(FLAGS_ZERO) : FLAGS_CLEAR(FLAGS_ZERO);
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY | FLAGS_CARRY);

	return value;
}

// 0x19
void CPU::rr_c()
{
	regs.c = rr(regs.c);
}

// 0x1A
void CPU::rr_d()
{
	regs.d = rr(regs.d);
}

// 0x1B
void CPU::rr_e()
{
	regs.e = rr(regs.e);
}

// 0x37
void CPU::swap_a()
{
	regs.a = swap(regs.a);
}

// 0x38
void CPU::srl_b()
{
	regs.b = srl(regs.b);
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
	opcode = gb->mmu.readByte(pc++);

	(this->*lookup[opcode].execute)();
}