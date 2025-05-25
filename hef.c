/*  hef_eeprom.h – EEPROM emulation over HEF (CCS)
 *  ------------------------------------------------
 *  • Transparent read_eeprom() and write_eeprom() calls.
 *  • If the device provides real DATA EEPROM the compiler's native
 *    routines are used.
 *  • If not (e.g. PIC16F1455) the High‑Endurance Flash (HEF) section is
 *    used instead.
 *
 *  Change HEF_START_ADDRESS if your device locates the HEF elsewhere.
 */

#ifndef HEF_EEPROM_H
#define HEF_EEPROM_H

// --- KNOWN HEF START ADDRESSES --------------------------------------------
//  (Devices without internal data‑EEPROM; all provide 128 B of HEF unless noted)
//
//  PIC10 family
#define HEF_10F320	0x00C0	// HEF 128 B: 0x00C0‑0x00FF (256‑word Flash)
#define HEF_10F322	0x01C0	// HEF 128 B: 0x01C0‑0x01FF (512‑word Flash)
//
//  PIC12 family
#define HEF_12F1572	0x0780	// HEF 128 B: 0x0780‑0x07FF (4 K‑word Flash)
//
//  PIC16 family (145x) – 14 K‑word Flash
#define HEF_16F1454	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF
#define HEF_16F1455	0x1F80
#define HEF_16F1459	0x1F80
//  PIC16 family (151x)
#define HEF_16F1516	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF (8 K‑word)
#define HEF_16F1517	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF (8 K‑word)
#define HEF_16F1518	0x3F80	// HEF 128 B: 0x3F80‑0x3FFF (16 K‑word)
#define HEF_16F1519	0x3F80	// HEF 128 B: 0x3F80‑0x3FFF (16 K‑word)
//  PIC16 family (161x)
#define HEF_16F1615	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF (8 K‑word)
#define HEF_16F1619	0x1F80
//  PIC16 family (171x)
#define HEF_16F1716	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF (8 K‑word)
#define HEF_16F1717	0x1F80	// HEF 128 B: 0x1F80‑0x1FFF (8 K‑word)
#define HEF_16F1718	0x3F80	// HEF 128 B: 0x3F80‑0x3FFF (16 K‑word)
#define HEF_16F1719	0x3F80	// HEF 128 B: 0x3F80‑0x3FFF (16 K‑word)
// ---------------------------------------------------------------------------

// --- CONFIGURATION ----------------------------------------------------------
// HEF_SIZE holds the usable byte capacity of the HEF area for the devices
// listed above. All currently supported PIC10/12/16 devices expose 128 bytes
// of high‑endurance data, so this is the default. Override from the
// application if you add a device with a different HEF capacity.
#ifndef HEF_SIZE
	#define HEF_SIZE	128
#endif

#ifndef HEF_START_ADDRESS
	#error "You have to define HEF_START_ADDRESS"
#endif

// Find the last block of our HEF
#define HEF_END_ADDRESS     (HEF_START_ADDRESS + HEF_SIZE - 1)

// Reserve flash of the HEF positions to avoid overwrite by the compiler
#org HEF_START_ADDRESS, HEF_END_ADDRESS
// ----------------------------------------------------------------------------

// --- DEVICES WITH REAL EEPROM ----------------------------------------------
#if getenv("DATA_EEPROM") > 0

	// Nothing to do: rely on the built‑in routines
	#define EEPROM_REAL	true
	#warning "Internal EEPROM is being used."

// --- DEVICES WITHOUT EEPROM (use HEF) --------------------------------------
#else
	#define EEPROM_REAL	false
	#warning "HEF is being used instead of EEPROM"

	// Block sizes retrieved from the device description
	#define HEF_WRITE_SIZE	getenv("FLASH_WRITE_SIZE")
	#define HEF_ERASE_SIZE	getenv("FLASH_ERASE_SIZE")
	
	#define EEPROM_SIZE	HEF_SIZE

	// ---------- READ (1 byte) ------------------------------------------------
	byte hef_read_byte(unsigned int16 addr) {
		int16 w = read_program_eeprom(HEF_START_ADDRESS + addr);	// full 16‑bit word
		return (byte)w;	// LSB holds the data byte
	}
	#define read_eeprom(address)	hef_read_byte((address))

	// ---------- WRITE (1 byte) ----------------------------------------------
	#if (HEF_WRITE_SIZE == HEF_ERASE_SIZE)

		// Simple case: erase size equals write size
		void hef_write_byte(unsigned int16 addr, byte value) {
			if (hef_read_byte(addr) == value) return;	// skip if already the same
			write_program_memory(HEF_START_ADDRESS + addr, &value, 1);
		}

	#else	// -------------------------------------------------------------------
			// HEF_ERASE_SIZE > HEF_WRITE_SIZE (e.g., some PIC18 devices)

		// Full read‑modify‑erase‑write cycle for the entire block
		void hef_write_byte(unsigned int16 addr, byte value) {
			unsigned int16 abs	= HEF_START_ADDRESS + addr;
			unsigned int16 blk	= abs & ~(HEF_ERASE_SIZE - 1);	// block start address
			unsigned int16 off	= abs - blk;	// byte offset within block

			static union {
				byte b[HEF_ERASE_SIZE];
				int16 w[HEF_ERASE_SIZE/2];
			} buf;

			// 1) Backup the current block to RAM
			read_program_memory(blk, buf.b, HEF_ERASE_SIZE);

			// 2) Exit early if nothing changes
			if (buf.b[off] == value) return;

			buf.b[off] = value;

			// 3) Erase the entire block
			erase_program_eeprom(blk);

			// 4) Re‑write the complete block (multiples of HEF_WRITE_SIZE)
			for (unsigned int16 i = 0; i < HEF_ERASE_SIZE; i += HEF_WRITE_SIZE) {
				write_program_memory(blk + i, &buf.b[i], HEF_WRITE_SIZE);
			}
		}
	#endif	// equal / different block sizes

	#define write_eeprom(address,val)	hef_write_byte((address),(val))

#endif	// DATA_EEPROM
// ----------------------------------------------------------------------------

#endif	// HEF_EEPROM_H
