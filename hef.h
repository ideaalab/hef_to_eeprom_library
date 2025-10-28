/*  hef.h - EEPROM emulation over HEF (CCS)
 *  ------------------------------------------------
 *  - Transparent read_eeprom() and write_eeprom() calls.
 *  - If the device provides real DATA EEPROM the compiler's native
 *    routines are used.
 *  - If not (e.g. PIC16F1455) the High-Endurance Flash (HEF) section is
 *    used instead.
 *
 *  Change HEF_START_ADDRESS if your device locates the HEF elsewhere.
 */

#ifndef HEF_EEPROM_H
#define HEF_EEPROM_H

// --- KNOWN HEF START ADDRESSES --------------------------------------------
// PIC10 family
#define HEF_10F320	0x00C0	// 128 B (256-word Flash)
#define HEF_10F322	0x01C0	// 128 B (512-word Flash)

// PIC12 family
#define HEF_12F1501	0x0380	// 128 B (1K-word Flash)
#define HEF_12F1571	0x0780	// 128 B (2K-word Flash)
#define HEF_12F1572	0x0780	// 128 B (2K-word Flash)
#define HEF_12F1612	0x0F80	// 128 B (4K-word Flash)

// PIC16 family (145x)
#define HEF_16F1454	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1455	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1459	0x1F80	// 128 B (8K-word Flash)

// PIC16 family (150x)
#define HEF_16F1501	0x0380	// 128 B (1K-word Flash)
#define HEF_16F1503	0x0780	// 128 B (2K-word Flash)
#define HEF_16F1507	0x0780	// 128 B (2K-word Flash)
#define HEF_16F1508	0x0F80	// 128 B (4K-word Flash)
#define HEF_16F1509	0x1F80	// 128 B (8K-word Flash)

// PIC16 family (151x)
#define HEF_16F1512	0x0780	// 128 B (2K-word Flash)
#define HEF_16F1513	0x0F80	// 128 B (4K-word Flash)
#define HEF_16F1516	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1517	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1518	0x3F80	// 128 B (16K-word Flash)
#define HEF_16F1519	0x3F80	// 128 B (16K-word Flash)

// PIC16 family (152x)
#define HEF_16F1526	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1527	0x1F80	// 128 B (8K-word Flash)

// PIC16 family (161x)
#define HEF_16F1615	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1619	0x1F80

// PIC16 family (170x)
#define HEF_16F1703	0x0780	// 128 B (2K-word Flash)
#define HEF_16F1704	0x0F80	// 128 B (4K-word Flash)
#define HEF_16F1707	0x0780	// 128 B (2K-word Flash)
#define HEF_16F1708	0x0F80	// 128 B (4K-word Flash)
#define HEF_16F1713	0x0F80	// 128 B (4K-word Flash)

// PIC16 family (171x)
#define HEF_16F1716	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1717	0x1F80	// 128 B (8K-word Flash)
#define HEF_16F1718	0x3F80	// 128 B (16K-word Flash)
#define HEF_16F1719	0x3F80	// 128 B (16K-word Flash)
// ---------------------------------------------------------------------------

// --- AUTO-DETECTION OF HEF_START_ADDRESS -----------------------------------
#ifndef HEF_START_ADDRESS
	// PIC10 family
	#if getenv("DEVICE")=="PIC10F320"
		#define HEF_START_ADDRESS HEF_10F320
	#elif getenv("DEVICE")=="PIC10F322"
		#define HEF_START_ADDRESS HEF_10F322
	// PIC12 family
	#elif getenv("DEVICE")=="PIC12F1501"
		#define HEF_START_ADDRESS HEF_12F1501
	#elif getenv("DEVICE")=="PIC12F1571"
		#define HEF_START_ADDRESS HEF_12F1571
	#elif getenv("DEVICE")=="PIC12F1572"
		#define HEF_START_ADDRESS HEF_12F1572
	#elif getenv("DEVICE")=="PIC12F1612"
		#define HEF_START_ADDRESS HEF_12F1612
	// PIC16 family (145x)
	#elif getenv("DEVICE")=="PIC16F1454"
		#define HEF_START_ADDRESS HEF_16F1454
	#elif getenv("DEVICE")=="PIC16F1455"
		#define HEF_START_ADDRESS HEF_16F1455
	#elif getenv("DEVICE")=="PIC16F1459"
		#define HEF_START_ADDRESS HEF_16F1459
	// PIC16 family (150x)
	#elif getenv("DEVICE")=="PIC16F1501"
		#define HEF_START_ADDRESS HEF_16F1501
	#elif getenv("DEVICE")=="PIC16F1503"
		#define HEF_START_ADDRESS HEF_16F1503
	#elif getenv("DEVICE")=="PIC16F1507"
		#define HEF_START_ADDRESS HEF_16F1507
	#elif getenv("DEVICE")=="PIC16F1508"
		#define HEF_START_ADDRESS HEF_16F1508
	#elif getenv("DEVICE")=="PIC16F1509"
		#define HEF_START_ADDRESS HEF_16F1509
	// PIC16 family (151x)
	#elif getenv("DEVICE")=="PIC16F1512"
		#define HEF_START_ADDRESS HEF_16F1512
	#elif getenv("DEVICE")=="PIC16F1513"
		#define HEF_START_ADDRESS HEF_16F1513
	#elif getenv("DEVICE")=="PIC16F1516"
		#define HEF_START_ADDRESS HEF_16F1516
	#elif getenv("DEVICE")=="PIC16F1517"
		#define HEF_START_ADDRESS HEF_16F1517
	#elif getenv("DEVICE")=="PIC16F1518"
		#define HEF_START_ADDRESS HEF_16F1518
	#elif getenv("DEVICE")=="PIC16F1519"
		#define HEF_START_ADDRESS HEF_16F1519
	// PIC16 family (152x)
	#elif getenv("DEVICE")=="PIC16F1526"
		#define HEF_START_ADDRESS HEF_16F1526
	#elif getenv("DEVICE")=="PIC16F1527"
		#define HEF_START_ADDRESS HEF_16F1527
	// PIC16 family (161x)
	#elif getenv("DEVICE")=="PIC16F1615"
		#define HEF_START_ADDRESS HEF_16F1615
	#elif getenv("DEVICE")=="PIC16F1619"
		#define HEF_START_ADDRESS HEF_16F1619
	// PIC16 family (170x)
	#elif getenv("DEVICE")=="PIC16F1703"
		#define HEF_START_ADDRESS HEF_16F1703
	#elif getenv("DEVICE")=="PIC16F1704"
		#define HEF_START_ADDRESS HEF_16F1704
	#elif getenv("DEVICE")=="PIC16F1707"
		#define HEF_START_ADDRESS HEF_16F1707
	#elif getenv("DEVICE")=="PIC16F1708"
		#define HEF_START_ADDRESS HEF_16F1708
	#elif getenv("DEVICE")=="PIC16F1713"
		#define HEF_START_ADDRESS HEF_16F1713
	// PIC16 family (171x)
	#elif getenv("DEVICE")=="PIC16F1716"
		#define HEF_START_ADDRESS HEF_16F1716
	#elif getenv("DEVICE")=="PIC16F1717"
		#define HEF_START_ADDRESS HEF_16F1717
	#elif getenv("DEVICE")=="PIC16F1718"
		#define HEF_START_ADDRESS HEF_16F1718
	#elif getenv("DEVICE")=="PIC16F1719"
		#define HEF_START_ADDRESS HEF_16F1719
	#endif
#endif

#ifndef HEF_START_ADDRESS
	#error "HEF_START_ADDRESS not defined. Define it manually or use a supported PIC."
#endif
// ----------------------------------------------------------------------------

// --- CONFIGURATION ----------------------------------------------------------
// HEF_SIZE holds the usable byte capacity of the HEF area for the devices
// listed above. All currently supported PIC10/12/16 devices expose 128 words
// of high-endurance data, so this is the default. Override from the
// application if you add a device with a different HEF capacity.
#ifndef HEF_SIZE
	#define HEF_SIZE	128
#endif

// HEF word size
#if defined(__PCB__)  // PIC10/12 baseline
	#define HEF_WORD_MASK 0x0FFF  // 12 bits
#else
	#define HEF_WORD_MASK 0x3FFF  // 14 bits
#endif

// Find the last block of our HEF
#define HEF_END_ADDRESS     (HEF_START_ADDRESS + HEF_SIZE - 1)
// ----------------------------------------------------------------------------

// --- DEVICES WITH REAL EEPROM ----------------------------------------------
#if getenv("DATA_EEPROM") > 0

	// Nothing to do: rely on the built-in routines
	#define EEPROM_REAL	true
	#warning "Internal EEPROM is being used."

// --- DEVICES WITHOUT EEPROM (use HEF) --------------------------------------
#else
	#define EEPROM_REAL	false
	#warning "HEF is being used instead of EEPROM"

	// Block sizes retrieved from the device description
	#define HEF_WRITE_SIZE	getenv("FLASH_WRITE_SIZE")
	#define HEF_ERASE_SIZE	getenv("FLASH_ERASE_SIZE")
	#define HEF_WORD_SIZE	getenv("BITS_PER_INSTRUCTION")
	
	#define DATA_EEPROM_SIZE	HEF_SIZE

	// Reserve flash of the HEF positions to avoid overwrite by the compiler
	#org HEF_START_ADDRESS, HEF_END_ADDRESS {}

	// Prototypes
	// read
	byte hef_read_byte(unsigned int16 addr);
	int16 hef_read_word(unsigned int16 addr);
	void hef_read_block(unsigned int16 addr, byte *ptr, byte count);
	void hef_read_block_word(unsigned int16 addr, int16 *ptr, byte count);
	// write
	void hef_write_byte(unsigned int16 addr, byte value);
	void hef_write_word(unsigned int16 addr, int16 value);
	void hef_write_block(unsigned int16 addr, byte *ptr, byte count);
	void hef_write_block_word(unsigned int16 addr, int16 *ptr, byte count);
	
	// Compatibility macros to work as classic EEPROM
	#define read_eeprom(address) hef_read_byte((address))
	#define read_eeprom_word(address) hef_read_word((address))
	#define read_eeprom_block(address,ptr,count) hef_read_block((address),(ptr),(count))
	#define read_eeprom_block_word(address,ptr,count) hef_read_block_word((address),(ptr),(count))
	
	#define write_eeprom(address,val) hef_write_byte((address),(val))
	#define write_eeprom_word(address,val) hef_write_word((address),(val))
	#define write_eeprom_block(address,ptr,count) hef_write_block((address),(ptr),(count))
	#define write_eeprom_block_word(address,ptr,count) hef_write_block_word((address),(ptr),(count))

#endif	// DATA_EEPROM
// ----------------------------------------------------------------------------

#endif	// HEF_EEPROM_H
