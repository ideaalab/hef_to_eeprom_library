#ifndef HEF
#define	HEF

#ifndef HEF_START_ADDRESS
#error "Hay que declarar HEF_START_ADDRESS"
#endif

#ifndef HEF_SIZE
#error "Hay que declarar HEF_SIZE"
#endif

#define HEF_WRITE_SIZE	getenv("FLASH_WRITE_SIZE")
#define HEF_ERASE_SIZE	getenv("FLASH_ERASE_SIZE")

#define EEPROM_SIZE		HEF_SIZE

/*
 * Lee un byte desde HEF
 */
int8 read_eeprom(int8 address) {
    if (address >= HEF_SIZE) return 0xFF; // Evita lecturas fuera de rango
	
    return read_program_eeprom(HEF_START_ADDRESS + address, 1);
}

/*
 * Escribe un byte en HEF
 */
void write_eeprom(int8 address, int8 data) {
    if (address >= HEF_SIZE) return; // Evita escrituras fuera de rango

    int16 page_start = (HEF_START_ADDRESS + address) & ~(FLASH_ERASE_SIZE - 1); // Calcula inicio de la página
    int8 buffer[HEF_PAGE_SIZE]; // Buffer temporal del tamaño de una página

    read_program_memory(page_start, buffer, HEF_PAGE_SIZE); // Copia la página actual
    buffer[(HEF_START_ADDRESS + address) % HEF_PAGE_SIZE] = data; // Modifica solo el dato requerido

    erase_program_memory(page_start); // Borra solo la página afectada
    write_program_memory(page_start, buffer, HEF_PAGE_SIZE); // Reescribe la página con el nuevo dato
}

#endif