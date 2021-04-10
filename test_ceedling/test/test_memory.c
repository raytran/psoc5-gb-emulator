#include "unity.h"
#include "memory.h"
#include "stdint.h"
#include "rom.h"


void setUp(void){
	
}

void tearDown(void){

}

void test_consistent_access(void){
	Memory mem; 
	uint16_t addr;
	uint8_t data1[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
	write_mem(&mem, VRAM_END - 1 , 0xAA);
	TEST_ASSERT_EQUAL_HEX8(0xAA, fetch(&mem, VRAM_END - 1, true));
	
	for (addr=VRAM_START;addr<VRAM_END;addr++){
		write_mem(&mem, addr,  data1[addr % 5]);
		TEST_ASSERT_EQUAL_HEX8(data1[addr % 5], fetch(&mem, addr, true));
	}
	 
	uint8_t data2[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
	for (addr=WRAM_START;addr<WRAM_END;addr++){
		write_mem(&mem, addr, data2[addr % 5]);
		TEST_ASSERT_EQUAL_HEX8(data2[addr % 5], fetch(&mem, addr, true));
	}
	
}

void test_clear_memory(void){
	Memory mem; 
	uint16_t addr;
	uint8_t data1[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
	
	for (addr=VRAM_START;addr<VRAM_END;addr++){
		write_mem(&mem, addr,  data1[addr % 5]);
	}
	 
	uint8_t data2[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
	for (addr=WRAM_START;addr<WRAM_END;addr++){
		write_mem(&mem, addr, data2[addr % 5]);
	}
	
	reset_memory(&mem);
	for (addr=VRAM_START;addr<VRAM_END;addr++){
		TEST_ASSERT_EQUAL_HEX8(0x00, fetch(&mem, addr, false));
	}
	 
	for (addr=WRAM_START;addr<WRAM_END;addr++){
		TEST_ASSERT_EQUAL_HEX8(0x00, fetch(&mem, addr, false));
	}
}