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
		void dec_c();
		void ld_c_n();

		void ld_de_nn();
		void ld_de_a();
		void inc_de();
		void inc_d();
		void jr_n();
		void ld_a_de();
		void inc_e();
		void dec_e();
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
		void inc_l();
		void dec_l();
		void ld_l_n();

		void jr_nc();
		void ld_sp_nn();
		void ldd_a();
		void dec_hla();
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
		void xor_l();
		void xor_hl();
		void xor_a();

		void or_c();
		void or_hl();
		void or_a();

		void pop_bc();
		void jp_nn();
		void call_nz();
		void push_bc();
		void add_n();
		void ret_z();
		void ret();
		void cb();
		void call_nn();
		void adc_n();

		void ret_nc();
		void pop_de();
		void push_de();
		void sub_n();
		void ret_c();

		void ldh_n_a();
		void pop_hl();
		void ld_ff_c_a();
		void push_hl();
		void and_n();
		void jp_hl();
		void ld_nn_a();
		void xor_n();

		void ldh_a_n();
		void pop_af();
		void ld_a_ff_c();
		void di();
		void push_af();
		void ld_sp_hl();
		void ld_a_nn();
		void cp_n();

		// Extended instructions
		uint8_t srl(uint8_t value);
		uint8_t rr(uint8_t value);
		uint8_t swap(uint8_t value);

		void rr_c();
		void rr_d();
		void rr_e();

		void swap_a();
		void srl_b();

		void log();
		

	public:
		CPU();

		void connectGB(GameBoy *g) { gb = g; }

		void dump_regs();
		void cpu_step();

		bool is_halted() { return HALT; };
};

