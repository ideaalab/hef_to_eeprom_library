#include <16F1455.h>

#device ADC=10

#FUSES NOWDT                    
#FUSES NOPUT                    
#FUSES NOMCLR                   
#FUSES NOPROTECT
#FUSES NOBROWNOUT               
#FUSES NOCLKOUT                 
#FUSES NOIESO                   
#FUSES NOFCMEN                  
#FUSES NOWRT                    
#FUSES NOSTVREN                 
#FUSES NOLVP                    
#FUSES NODEBUG                  

#use delay(internal=16MHz)
#use rs232(baud=115200, xmit=PIN_C4, ERRORS)

// --- Definiciones de pines ---
#define P_BTN	PIN_A3	// I


// --- Configuración de TRIS y Weak Pull-ups ---
#define TRIS_A  0b00111111  // Entradas/Salidas para PORTA
#define WPU_A   0b00001000  // Weak pull-up PORTA

// --- Constantes ---
#define PULSADO		0
#define NO_PULSADO	(!PULSADO)

#include "..\hef.c"

void print_hef_bytes(void){

	//imprimimos cabeceras de columna
	printf("    ");
	for(int x = 0; x < 8; x++){
		printf(" %02u", x);
	}
	printf("\r\n");
	
	//imprimimos valores
	for(long x = 0; x < DATA_EEPROM_SIZE; x++){
		//imprimo cambio de linea en los multiplos de 8
		if(x%8 == 0){
			//imprimo cambio de linea en los multiplos de 64
			if((x%64 == 0) && (x != 0)){
				printf("\r\n");
			}
			
			//imprimimos numero de linea
			printf("\r\n%02LX:  ", x);
		}
		
		printf("%02X ", read_eeprom(x));	//imprimo valor
	}
	
	printf("\r\n\r\n");
}

void print_hef_words(void){
	//imprimimos cabeceras de columna
	printf("    ");
	for(int x = 0; x < 8; x++){
		printf("   %02u", x);
	}
	printf("\r\n");
	
	//imprimimos valores
	for(long x = 0; x < DATA_EEPROM_SIZE; x++){
		//imprimo cambio de linea en los multiplos de 8
		if(x%8 == 0){
			//imprimo cambio de linea en los multiplos de 64
			if((x%64 == 0) && (x != 0)){
				printf("\r\n");
			}
			
			//imprimimos numero de linea
			printf("\r\n%02LX:  ", x);
		}
		
		printf("%04LX ", hef_read_word(x));	//imprimo valor
	}
	
	printf("\r\n\r\n");
}

void main() {
	//config
	disable_interrupts(GLOBAL);
	set_tris_a(TRIS_A);
	port_a_pullups(WPU_A);
	
    delay_ms(100);
	
	//print_hef();
	//print_hef_words();
	
	do{
		if(input_state(P_BTN) == PULSADO){
			printf("\r\n\r\n--- HEF EEPROM Test ---\r\n");
			printf("HEF size: %u words / %u-bit word\r\n", DATA_EEPROM_SIZE, HEF_WORD_SIZE);
			printf("Write size: %u / Erase size: %u\r\n\r\n", HEF_WRITE_SIZE, HEF_ERASE_SIZE);

			/*
			 * --- writes incremental bytes to the first half of HEF --- 
			 * MSB is not written, so when read the whole word MSB is trash
			 */
			for(int8 i = 0; i < 0x20; i++) {
				write_eeprom(i, i);
				hef_write_word(i+32, i);
			}
			
			//reads byte and increment/decrement one
			int16 b1 = read_eeprom(0x40)+1;
			int16 b2 = read_eeprom(0x47)-1;
			
			//reads word and increment/decrement one
			int16 w1 = hef_read_word(0x48)+1;
			int16 w2 = hef_read_word(0x4F)-1;
			
			int16 block[8] = {0, 1, 2, 3, 4, 5, 6, 7};
			
			//write LSB of the word
			write_eeprom(0x40, b1);
			write_eeprom(0x47, b2);
			
			//writes 12-14bit word
			hef_write_word(0x48, w1);
			hef_write_word(0x4F, w2);
			
			//writes block
			write_program_memory(HEF_START_ADDRESS + 0x50, block, 16);
			hef_read_block(HEF_START_ADDRESS + 0x50, block, 16);
			
			printf("Pos 0x00-0x1F: write_eeprom() (MSB is garbage)\r\n");
			printf("Pos 0x20-0x3F: hef_write_word()\r\n\r\n");
			
			
			printf("write_eeprom() / read_eeprom()\r\n");
			printf("Pos 0x40: W: %04LX / R: %04LX\r\n", b1, read_eeprom(0x40));
			printf("Pos 0x47: W: %04LX / R: %04LX\r\n\r\n", b2, read_eeprom(0x47));
			
			printf("hef_write_word() / hef_read_word()\r\n");
			printf("Pos 0x48: W: %04LX / R: %04LX\r\n", w1, hef_read_word(0x48));
			printf("Pos 0x4F: W: %04LX / R: %04LX\r\n\r\n", w2, hef_read_word(0x4F));
			
			print_hef_bytes();
			print_hef_words();

			delay_ms(100);
			
			//print_hef();
			//printf("Word: %Lu\r\n", hef_read_word(DATA_EEPROM_SIZE-1));
			
			while(input_state(P_BTN) == PULSADO){delay_ms(20);}	//antirebote
		}
	}while(true);
}