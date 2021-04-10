#include "unity.h"
#include "registers.h"

void setUp(void){

}

void tearDown(void){

}

void test_hl(void){
	Registers regs;
	regs.h = 0x03;
	regs.l = 0x05;
	TEST_ASSERT_EQUAL_HEX16(0x0305,regs.hl);
	regs.hl = 0x0123;
	TEST_ASSERT_EQUAL_HEX16(0x01, regs.h);
	TEST_ASSERT_EQUAL_HEX16(0x23, regs.l);
}

void test_de(void){
	Registers regs;
	regs.d = 0x03;
	regs.e = 0x05;
	TEST_ASSERT_EQUAL_HEX16(0x0305,regs.de);
	regs.de = 0x0123;
	TEST_ASSERT_EQUAL_HEX16(0x01, regs.d);
	TEST_ASSERT_EQUAL_HEX16(0x23, regs.e);
}

void test_bc(void){
	Registers regs;
	regs.b = 0x03;
	regs.c = 0x05;
	TEST_ASSERT_EQUAL_HEX16(0x0305,regs.bc);
	regs.bc = 0x0123;
	TEST_ASSERT_EQUAL_HEX16(0x01, regs.b);
	TEST_ASSERT_EQUAL_HEX16(0x23, regs.c);
}

void test_af(void){
	Registers regs;
	regs.a = 0x03;
	regs.f = 0x05;
	TEST_ASSERT_EQUAL_HEX16(0x0305,regs.af);
	regs.af = 0x0123;
	TEST_ASSERT_EQUAL_HEX16(0x01, regs.a);
	TEST_ASSERT_EQUAL_HEX16(0x23, regs.f);
}

void test_zero_flag(void){
    Registers regs;
	TEST_ASSERT_FALSE(get_zero_flag(&regs));
	set_zero_flag(&regs, true);
	TEST_ASSERT_TRUE(get_zero_flag(&regs));
}

void test_subtraction_flag(void){
	Registers regs;
	TEST_ASSERT_FALSE(get_subtraction_flag(&regs));
	set_subtraction_flag(&regs, true);
	TEST_ASSERT_TRUE(get_subtraction_flag(&regs));
}

void test_half_carry_flag(void){
	Registers regs;
	TEST_ASSERT_FALSE(get_half_carry_flag(&regs));
	set_half_carry_flag(&regs, true);
	TEST_ASSERT_TRUE(get_half_carry_flag(&regs));
}

void test_carry_flag(void){
	Registers regs;
	TEST_ASSERT_FALSE(get_carry_flag(&regs));
	set_carry_flag(&regs, true);
	TEST_ASSERT_TRUE(get_carry_flag(&regs));
}
