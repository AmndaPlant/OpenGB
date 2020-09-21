#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>

//#define LOGGING

class GameBoy;

#define FLAGS_ZERO (1 << 7)
#define FLAGS_NEGATIVE (1 << 6)
#define FLAGS_HALFCARRY (1 << 5)
#define FLAGS_CARRY (1 << 4)

#define FLAGS_ISZERO (regs.f & FLAGS_ZERO)
#define FLAGS_ISNEGATIVE (regs.f & FLAGS_NEGATIVE)
#define FLAGS_ISHALFCARRY (regs.f & FLAGS_HALFCARRY)
#define FLAGS_ISCARRY (regs.f & FLAGS_CARRY)

#define FLAGS_ISSET(x) (regs.f & (x))
#define FLAGS_SET(x) (regs.f |= (x))
#define FLAGS_CLEAR(x) (regs.f &= ~(x))

class CPU
{
	private:
		struct registers
		{
			struct
			{
				union
				{
					struct
					{
						uint8_t f;
						uint8_t a;
					};
					uint16_t af;
				};
			};

			struct
			{
				union
				{
					struct
					{
						uint8_t c;
						uint8_t b;
					};
					uint16_t bc;
				};
			};

			struct
			{
				union
				{
					struct
					{
						uint8_t e;
						uint8_t d;
					};
					uint16_t de;
				};
			};

			struct
			{
				union
				{
					struct
					{
						uint8_t l;
						uint8_t h;
					};
					uint16_t hl;
				};
			};
		} regs;

		struct instruction
		{
			std::string name;
			void (CPU::* execute)() = nullptr;
			uint8_t length;
		};

		std::vector<instruction> lookup;
		std::vector<instruction> extended_lookup;

		uint16_t sp;
		uint16_t pc;
		uint8_t opcode;

		bool HALT;
		bool interrupts_enabled;

		GameBoy *gb = nullptr;

		void cb_instr();

		uint8_t inc(uint8_t value);
		uint8_t dec(uint8_t value);

		void add(uint8_t value);
		void adc(uint8_t value);
		void sub(uint8_t value);
		void sbc(uint8_t value);
		void AND(uint8_t value);
		void XOR(uint8_t value);
		void OR(uint8_t value);
		void cp(uint8_t value);

		void add16(uint16_t value);

		void jp(uint16_t addr);
		void jr();
		void call(uint16_t addr);
		
		void push(uint16_t word);
		uint16_t pop();

		void undefined();
		void undefined_cb();

		void nop();
		void ld_bc_nn();
		void inc_bc();
		void dec_b();
		void ld_b_n();
		void ld_nn_sp();
		void add_hl_bc();
		void dec_bc();
		void dec_c();
		void ld_c_n();

		void ld_de_nn();
		void ld_de_a();
		void inc_de();
		void inc_d();
		void ld_d_n();
		void jr_n();
		void add_hl_de();
		void ld_a_de();
		void dec_de();
		void inc_e();
		void dec_e();
		void ld_e_n();
		void rra();

		void jr_nz_n();
		void ld_hl_nn();
		void ldi_hl();
		void inc_hl();
		void inc_h();
		void dec_h();
		void ld_h_n();
		void jr_z_n();
		void add_hl_hl();
		void ldi_a();
		void dec_hl();
		void inc_l();
		void dec_l();
		void ld_l_n();

		void jr_nc();
		void ld_sp_nn();
		void ldd_a();
		void inc_hla();
		void dec_hla();
		void ld_hl_n();
		void jr_c_n();
		void inc_a();
		void dec_a();
		void ld_a_n();

		void ld_b_b();
		void ld_b_c();
		void ld_b_d();
		void ld_b_e();
		void ld_b_h();
		void ld_b_l();
		void ld_b_hl();
		void ld_b_a();
		void ld_c_b();
		void ld_c_c();
		void ld_c_d();
		void ld_c_e();
		void ld_c_h();
		void ld_c_l();
		void ld_c_hl();
		void ld_c_a();

		void ld_d_b();
		void ld_d_c();
		void ld_d_d();
		void ld_d_e();
		void ld_d_h();
		void ld_d_l();
		void ld_d_hl();
		void ld_d_a();
		void ld_e_b();
		void ld_e_c();
		void ld_e_d();
		void ld_e_e();
		void ld_e_h();
		void ld_e_l();
		void ld_e_hl();
		void ld_e_a();

		void ld_h_b();
		void ld_h_c();
		void ld_h_d();
		void ld_h_e();
		void ld_h_h();
		void ld_h_l();
		void ld_h_hl();
		void ld_h_a();
		void ld_l_b();
		void ld_l_c();
		void ld_l_d();
		void ld_l_e();
		void ld_l_h();
		void ld_l_l();
		void ld_l_hl();
		void ld_l_a();

		void ld_hl_b();
		void ld_hl_c();
		void ld_hl_d();
		void ld_hl_e();
		void ld_hl_h();
		void ld_hl_l();
		void ld_hl_a();
		void ld_a_b();
		void ld_a_c();
		void ld_a_d();
		void ld_a_e();
		void ld_a_h();
		void ld_a_l();
		void ld_a_hl();
		void ld_a_a();

		void or_b();
		void xor_c();
		void xor_e();
		void xor_l();
		void xor_hl();
		void xor_a();

		void or_c();
		void or_hl();
		void or_a();

		void ret_nz();
		void pop_bc();
		void jp_nz();
		void jp_nn();
		void call_nz();
		void push_bc();
		void add_n();
		void rst_0();
		void ret_z();
		void ret();
		void jp_z();
		void cb();
		void call_z();
		void call_nn();
		void adc_n();
		void rst_8();

		void ret_nc();
		void pop_de();
		void jp_nc();
		void call_nc();
		void push_de();
		void sub_n();
		void rst_10();
		void ret_c();
		void reti();
		void jp_c();
		void call_c();
		void sbc_n();
		void rst_18();

		void ldh_n_a();
		void pop_hl();
		void ld_ff_c_a();
		void push_hl();
		void and_n();
		void rst_20();
		void jp_hl();
		void ld_nn_a();
		void xor_n();
		void rst_28();

		void ldh_a_n();
		void pop_af();
		void ld_a_ff_c();
		void di();
		void push_af();
		void or_n();
		void rst_30();
		void ld_sp_hl();
		void ld_a_nn();
		void cp_n();
		void rst_38();

		// Extended instructions
		uint8_t rr(uint8_t value);
		uint8_t swap(uint8_t value);
		uint8_t srl(uint8_t value);
		void bit(uint8_t value, uint8_t bit);
		uint8_t res(uint8_t value, uint8_t bit);
		uint8_t set(uint8_t value, uint8_t bit);

		void rr_c();
		void rr_d();
		void rr_e();

		void swap_a();
		void srl_b();

		void bit_0_b();
		void bit_0_c();
		void bit_0_d();
		void bit_0_e();
		void bit_0_h();
		void bit_0_l();
		void bit_0_hl();
		void bit_0_a();
		void bit_1_b();
		void bit_1_c();
		void bit_1_d();
		void bit_1_e();
		void bit_1_h();
		void bit_1_l();
		void bit_1_hl();
		void bit_1_a();

		void bit_2_b();
		void bit_2_c();
		void bit_2_d();
		void bit_2_e();
		void bit_2_h();
		void bit_2_l();
		void bit_2_hl();
		void bit_2_a();
		void bit_3_b();
		void bit_3_c();
		void bit_3_d();
		void bit_3_e();
		void bit_3_h();
		void bit_3_l();
		void bit_3_hl();
		void bit_3_a();

		void bit_4_b();
		void bit_4_c();
		void bit_4_d();
		void bit_4_e();
		void bit_4_h();
		void bit_4_l();
		void bit_4_hl();
		void bit_4_a();
		void bit_5_b();
		void bit_5_c();
		void bit_5_d();
		void bit_5_e();
		void bit_5_h();
		void bit_5_l();
		void bit_5_hl();
		void bit_5_a();

		void bit_6_b();
		void bit_6_c();
		void bit_6_d();
		void bit_6_e();
		void bit_6_h();
		void bit_6_l();
		void bit_6_hl();
		void bit_6_a();
		void bit_7_b();
		void bit_7_c();
		void bit_7_d();
		void bit_7_e();
		void bit_7_h();
		void bit_7_l();
		void bit_7_hl();
		void bit_7_a();

		void res_0_b();
		void res_0_c();
		void res_0_d();
		void res_0_e();
		void res_0_h();
		void res_0_l();
		void res_0_hl();
		void res_0_a();
		void res_1_b();
		void res_1_c();
		void res_1_d();
		void res_1_e();
		void res_1_h();
		void res_1_l();
		void res_1_hl ();
		void res_1_a();

		void res_2_b();
		void res_2_c();
		void res_2_d();
		void res_2_e();
		void res_2_h();
		void res_2_l();
		void res_2_hl();
		void res_2_a();
		void res_3_b();
		void res_3_c();
		void res_3_d();
		void res_3_e();
		void res_3_h();
		void res_3_l();
		void res_3_hl();
		void res_3_a();

		void res_4_b();
		void res_4_c();
		void res_4_d();
		void res_4_e();
		void res_4_h();
		void res_4_l();
		void res_4_hl();
		void res_4_a();
		void res_5_b();
		void res_5_c();
		void res_5_d();
		void res_5_e();
		void res_5_h();
		void res_5_l();
		void res_5_hl();
		void res_5_a();

		void res_6_b();
		void res_6_c();
		void res_6_d();
		void res_6_e();
		void res_6_h();
		void res_6_l();
		void res_6_hl();
		void res_6_a();
		void res_7_b();
		void res_7_c();
		void res_7_d();
		void res_7_e();
		void res_7_h();
		void res_7_l();
		void res_7_hl();
		void res_7_a();

		void set_0_b();
		void set_0_c();
		void set_0_d();
		void set_0_e();
		void set_0_h();
		void set_0_l();
		void set_0_hl();
		void set_0_a();
		void set_1_b();
		void set_1_c();
		void set_1_d();
		void set_1_e();
		void set_1_h();
		void set_1_l();
		void set_1_hl();
		void set_1_a();

		void set_2_b();
		void set_2_c();
		void set_2_d();
		void set_2_e();
		void set_2_h();
		void set_2_l();
		void set_2_hl();
		void set_2_a();
		void set_3_b();
		void set_3_c();
		void set_3_d();
		void set_3_e();
		void set_3_h();
		void set_3_l();
		void set_3_hl();
		void set_3_a();

		void set_4_b();
		void set_4_c();
		void set_4_d();
		void set_4_e();
		void set_4_h();
		void set_4_l();
		void set_4_hl();
		void set_4_a();
		void set_5_b();
		void set_5_c();
		void set_5_d();
		void set_5_e();
		void set_5_h();
		void set_5_l();
		void set_5_hl();
		void set_5_a();

		void set_6_b();
		void set_6_c();
		void set_6_d();
		void set_6_e();
		void set_6_h();
		void set_6_l();
		void set_6_hl();
		void set_6_a();
		void set_7_b();
		void set_7_c();
		void set_7_d();
		void set_7_e();
		void set_7_h();
		void set_7_l();
		void set_7_hl();
		void set_7_a();

		void log();
		

	public:
		CPU();

		void connectGB(GameBoy *g) { gb = g; }

		void dump_regs();
		void cpu_step();

		bool is_halted() { return HALT; };
};

