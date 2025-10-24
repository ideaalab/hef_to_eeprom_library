#include "hef.h"

#if EEPROM_REAL == false
/*
 * hef_read_byte()
 * ---------------
 * Reads a single byte from HEF memory.
 * 
 * Parameters:
 *   addr - Address offset within HEF memory (0 to HEF_SIZE-1)
 * 
 * Returns:
 *   The byte value stored at the specified address
 */
byte hef_read_byte(unsigned int16 addr) {
	int16 w = read_program_eeprom(HEF_START_ADDRESS + addr);	// full 12/14 bit word
	return (byte)w;	// LSB holds the data byte
}

/*
 * hef_read_word()
 * ---------------
 * Reads a single word (12 or 14 bits) from HEF memory.
 * 
 * Parameters:
 *   addr - Address offset within HEF memory (0 to HEF_SIZE-1)
 * 
 * Returns:
 *   The word value stored at the specified address, masked to valid bits
 *   (12 bits for PIC10/12 baseline, 14 bits for PIC12/16F enhanced mid-range)
 */

int16 hef_read_word(unsigned int16 addr) {
	int16 w = read_program_eeprom(HEF_START_ADDRESS + addr);	// full 12/14bit word
	return w & HEF_WORD_MASK;
}

/*
 * hef_write_byte()
 * ----------------
 * Writes a single byte to HEF memory.
 * Function only valid for PIC10/12/16 where HEF_WRITE_SIZE = HEF_ERASE_SIZE.
 * Skips write if value is already stored. Disables interrupts during write.
 * 
 * Parameters:
 *   addr  - Address offset within HEF memory (0 to HEF_SIZE-1)
 *   value - Byte value to write (0-255)
 */

void hef_write_byte(unsigned int16 addr, byte value) {
	if (hef_read_byte(addr) == value) return;
	disable_interrupts(GLOBAL);
	write_program_eeprom(HEF_START_ADDRESS + addr, value);
	enable_interrupts(GLOBAL);
}

/*
 * hef_write_word()
 * ----------------
 * Writes a single word (12 or 14 bits) to HEF memory.
 * Function only valid for PIC10/12/16 where HEF_WRITE_SIZE = HEF_ERASE_SIZE.
 * Skips write if value is already stored. Disables interrupts during write.
 * 
 * Parameters:
 *   addr  - Address offset within HEF memory (0 to HEF_SIZE-1)
 *   value - Word value to write, will be masked to valid bits
 *           (12 bits for PIC10/12 baseline, 14 bits for PIC12/16F enhanced mid-range)
 */
void hef_write_word(unsigned int16 addr, int16 value) {
	value &= HEF_WORD_MASK;
	if (hef_read_word(addr) == value) return;
	disable_interrupts(GLOBAL);
	write_program_memory(HEF_START_ADDRESS + addr, (byte*)&value, 2);
	enable_interrupts(GLOBAL);
}

/*
 * hef_read_block()
 * ----------------
 * Reads multiple bytes from HEF memory into a buffer.
 * 
 * Parameters:
 *   addr  - Starting address offset within HEF memory (0 to HEF_SIZE-1)
 *   ptr   - Pointer to buffer where data will be stored
 *   count - Number of bytes to read (1 to HEF_SIZE)
 */
void hef_read_block(unsigned int16 addr, byte *ptr, byte count){
	read_program_memory(HEF_START_ADDRESS + addr, ptr, count);
}

/*
 * hef_write_block()
 * -----------------
 * Writes multiple bytes from a buffer to HEF memory.
 * Disables interrupts during the entire write operation.
 * 
 * Parameters:
 *   addr  - Starting address offset within HEF memory (0 to HEF_SIZE-1)
 *   ptr   - Pointer to buffer containing data to write
 *   count - Number of bytes to write (1 to HEF_SIZE)
 */
void hef_write_block(unsigned int16 addr, byte *ptr, byte count) {
	disable_interrupts(GLOBAL);
	write_program_memory(HEF_START_ADDRESS + addr, ptr, count);
	enable_interrupts(GLOBAL);
}
#endif