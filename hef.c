#ifndef HEF
#define	HEF

/* =============================================================================
 * Autor: Martin Andersen
 * Compania: IDEAA Lab ( http://www.ideaalab.com )
 * =============================================================================

 * =============================================================================
 * QUE HACE ESTA LIBRERIA?
 * 
 * Para PICs sin EEPROM interna pero con HEF, convierte read_eeprom() y
 * write_eeprom() que son funciones solo disponibles si hay EEPROM en
 * las funciones especificas para HEF read_program_memory() y
 * write_program_memory() lo que permite usar la HEF como si fuese EEPROM interna
 * Hay que tener en cuanta que la HEF es memoria de programa, y cada unidad
 * de informacion es una "palabra/word" de 13 o 15 bits por ejemplo. Nosotros
 * vamos a almacenar informacion solo en el byte bajo. Si usamos un buffer, los
 * bytes pares realmente no se usan, solo almacenamos en los impares
 * =============================================================================
 
 * =============================================================================
 * PRECAUCION
 * 
 * El PIC debe tener suficiente memoria ram si queremos escribir y conservar
 * otros datos que haya almacenados, ya que para escribir un valor hay que
 * escribir una pagina completa.
 * 
 * La HEF tiene una vida limitada y se consume solo con los BORRADOS de pagina.
 * =============================================================================

 * =============================================================================
 * COMO CONFIGURAR LA LIBRERIA?
 * 
 * + Para que la libreria funcione hay que declarar los parametros de la HEF de
 * nuestro PIC:
 * #define HEF_START_ADDRESS	0xF000	// Direccion de inicio
 * #define HEF_SIZE				128		// Tamaño de la HEF
 *
 * + Se puede configurar para que grabe sin conservar datos anteriores (no
 * requiere buffer en ram) o conservando el contenido de la pagina (requiere
 * buffer en ram del tamaño de la pagina. Declarar uno de los dos define. La 
 * configuracion por defecto mantiene los datos de la HEF cuando escribe.
 * #define HEF_LOSS_WRITE	//pierde el contenido de HEF cuando escribe
 * #define HEF_KEEP_WRITE	//mantiene el contenido de HEF cuando escribe
 * =============================================================================

 * =============================================================================
 * FUNCIONES
 * 
 * + write_eeprom(int8 address, int data)
 *		escribe en la memoria:
 *		-address: posicion de memoria (0 - HEF_SIZE)
 *		-data: valor a escribir (0 - 255)
 *
 * ========================================================================== */

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