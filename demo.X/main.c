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

// --- Pin definition ---
#define P_BTN	PIN_A3	// I


// --- TRIS and Weak Pull-ups config ---
#define TRIS_A  0b00111111  // I/O PORTA
#define WPU_A   0b00001000  // WPU PORTA

// --- Constants ---
#define PRESSED		0
#define NOT_PRESSED	(!PRESSED)

#include "..\hef.c"

void print_hef_bytes(void){

	//print column headers
	printf("    ");
	for(int x = 0; x < 16; x++){
		printf(" %02X", x);
	}
	printf("\r\n");
	
	//print values
	for(long x = 0; x < DATA_EEPROM_SIZE; x++){
		//print new line every 16 bytes
		if(x%16 == 0){
			//print new line every 32 bytes
			if((x%32 == 0) && (x != 0)){
				printf("\r\n");
			}
			
			//print line number
			printf("\r\n%02LX:  ", x);
		}
		
		printf("%02X ", read_eeprom(x));	//print value
	}
	
	printf("\r\n\r\n");
}

void print_hef_words(void){
	//print column headers
	printf("    ");
	for(int x = 0; x < 16; x++){
		printf("   %02X", x);
	}
	printf("\r\n");
	
	//print values
	for(long x = 0; x < DATA_EEPROM_SIZE; x++){
		//print new line every 16 words
		if(x%16 == 0){
			//print new line every 32 words
			if((x%32 == 0) && (x != 0)){
				printf("\r\n");
			}
			
			//print line number
			printf("\r\n%02LX:  ", x);
		}
		
		printf("%04LX ", read_eeprom_word(x));	//print value
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
		if(input_state(P_BTN) == PRESSED){
			printf("\r\n\r\n--- HEF EEPROM Test ---\r\n");
			printf("HEF size: %u words / %u-bit word\r\n", DATA_EEPROM_SIZE, HEF_WORD_SIZE);
			printf("Write block size: %u / Erase block size: %u\r\n\r\n", HEF_WRITE_SIZE, HEF_ERASE_SIZE);

			/*
			 * --- writes incremental bytes to the first half of HEF --- 
			 * MSB is not written, so when read the whole word MSB is trash
			 */
			for(int8 i = 0; i < 0x10; i++) {
				write_eeprom(i, i);
				write_eeprom_word(i+16, i);
			}
			
			//reads byte and increment/decrement one
			int16 b1 = read_eeprom(0x20)+1;
			int16 b2 = read_eeprom(0x21)-1;
			
			//reads word and increment/decrement one
			int16 w1 = read_eeprom_word(0x30)+1;
			int16 w2 = read_eeprom_word(0x31)-1;
			
			byte block_b[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
			int16 block_w[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
			
			//write LSB of the word
			write_eeprom(0x20, b1);
			write_eeprom(0x21, b2);
			
			//writes 12-14bit word
			write_eeprom_word(0x30, w1);
			write_eeprom_word(0x31, w2);
			
			//writes byte block
			write_eeprom_block(0x40, block_b, 16);
			read_eeprom_block(0x40, block_b, 16);
			
			//writes word block
			write_eeprom_block_word(0x50, block_w, 16);
			read_eeprom_block_word(0x50, block_w, 16);
			
			printf("Pos 0x00-0x0F: write_eeprom() (MSB is garbage)\r\n");
			printf("Pos 0x10-0x1F: write_eeprom_word()\r\n\r\n");
			
			
			printf("write_eeprom() / read_eeprom()\r\n");
			printf("Pos 0x20: W: %04LX / R: %04LX\r\n", b1, read_eeprom(0x20));
			printf("Pos 0x21: W: %04LX / R: %04LX\r\n\r\n", b2, read_eeprom(0x21));
			
			printf("write_eeprom_word() / read_eeprom_word()\r\n");
			printf("Pos 0x30: W: %04LX / R: %04LX\r\n", w1, read_eeprom_word(0x30));
			printf("Pos 0x31: W: %04LX / R: %04LX\r\n\r\n", w2, read_eeprom_word(0x31));
			
			printf("write_eeprom_block() / read_eeprom_block()\r\n");
			printf("Pos 0x40: R:\r\n");
			for(int8 i = 0x40; i<0x50; i++){
				printf("0x%02X ", block_b[i-0x40]);
				if(i==0x47){
					printf("\r\n");
				}
			}
			printf("\r\n\r\n");
			
			printf("write_eeprom_block_word() / read_eeprom_block_word()\r\n");
			printf("Pos 0x50: R:\r\n");
			for(int8 i = 0x50; i<0x60; i++){
				printf("0x%04X ", block_w[i-0x50]);
				if(i==0x57){
					printf("\r\n");
				}
			}
			printf("\r\n\r\n");
			
			print_hef_bytes();
			printf("\r\n");
			print_hef_words();

			delay_ms(100);
			
			while(input_state(P_BTN) == PRESSED){delay_ms(20);}	//debounce
		}
	}while(true);
}