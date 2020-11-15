#include <MKL25Z4.h>


#define MASK(X) (1UL<<X)
//define intruction pins
#define RS (2) //pta2
#define E (4)//ptd4
#define D0 (12)//pta12
#define D1 (4)//pta4
#define D2 (5)//pta5
#define D3 (8)//ptc8
#define D4 (9)//ptc9
#define D5 (13)//pta13
#define D6 (5)//ptd5
#define D7 (0)//ptd0

//define instructions
#define CLEAR_DISPLAY (0x01)
#define LCD_HOME (0x02)
#define DISPLAY_ON (0x0C)
#define TOP_ROW (0x80)
#define BOTTOM_ROW (0xC0)
#define CURSOR_INCREAMENT_MODE (0x06)
#define TWO_LINE_DISPLAY (0x38)

void setup_pins(void);
void lcd_init(void);
void write_lcd(unsigned char,int);
void lcd_goto(unsigned char, unsigned char);
void clear_lcd(void);
void write_string(char *s);


int main(){
setup_pins();
lcd_init();
clear_lcd();
lcd_goto(0,0);
write_string("Ashesi Embedded");
lcd_goto(0,1);
write_string("Willakuffo");

while(1){continue;}

}



void write_string(char *s){
for(int i = 0;i<0x10000;i++);
while (*s!=0){
write_lcd(*s,1);
s++; //increase pointer address to iterate char in string
}
}

void clear_lcd(void){
for(int i = 0;i<0x10000;i++);
write_lcd(CLEAR_DISPLAY,0);
write_lcd(LCD_HOME,0);	
}

	

void lcd_goto(unsigned char column, unsigned char row){
for(int i = 0;i<0x10000;i++);
if(row==0)write_lcd(TOP_ROW + column,0);
if(row==1)write_lcd(BOTTOM_ROW+ column,0);
}

void write_lcd(unsigned char instruction,int DOI){
	for(int i = 0;i<0x10000;i++);
int bin;   
#define DATA_LEN 8 //size of data and instruction
	//check write intruction or data into IR or DR res
	if (DOI) PTA->PSOR |= MASK(RS);//set RS hight to select DR and write data
	else PTA->PCOR |= MASK(RS);//set RS low to select IR and write instruction

	PTD->PCOR |= MASK(E);//set enable low-> timing	

	//get bits
 for (int i=DATA_LEN-1;i>=0;i--){   
 if (MASK(i) & instruction ){ bin = 1; }
 else{ bin = 0; }
 switch(i){
	 case 7:
		 if(bin) PTD->PSOR |= MASK(D7);
		 else  PTD->PCOR |= MASK(D7);
		 break;
	 case 6:
		 if(bin) PTD->PSOR |= MASK(D6);
		 else PTD->PCOR |= MASK(D6);
		 break;
	 case 5:
		 if(bin) PTA->PSOR |= MASK(D5);
		 else PTA->PCOR |= MASK(D5);
     break;
	 case 4:
		 if(bin) PTC->PSOR |= MASK(D4);
		 else PTC->PCOR |= MASK(D4);
		 break;
	 case 3:
		 if(bin) PTC->PSOR |= MASK(D3);
		 else PTC->PCOR |= MASK(D3);
		 break;
	 case 2:
		 if(bin) PTA->PSOR |= MASK(D2);
		 else PTA->PCOR |= MASK(D2);
	   break;
	 case 1:
		 if(bin) PTA->PSOR |= MASK(D1);
	   else PTA->PCOR |= MASK(D1);
	   break;
	 case 0:
		 if(bin) PTA->PSOR |= MASK(D0);
		 else PTA->PCOR |= MASK(D0);
		 break;
 
 }
}
 //write intruction by generating falling edge high to low
PTD->PSOR |= MASK(E);	
PTD->PCOR |= MASK(E);	

	}

void lcd_init(void){
  for(int i = 0;i<0x10000;i++);
	write_lcd(CURSOR_INCREAMENT_MODE,0); //low for ins
	write_lcd(DISPLAY_ON,0);//display on,cursor off
	write_lcd(TWO_LINE_DISPLAY,0);//8 bit bus 2 line display
	

}




void setup_pins(){
SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK; //clock gate ports
	//configure pins
	//clear
PORTA->PCR[RS] &= ~0x700UL;
PORTD->PCR[E] &= ~0x700UL;
PORTA->PCR[D0] &= ~0x700UL;
PORTA->PCR[D1] &= ~0x700UL;
PORTA->PCR[D2] &= ~0x700UL;	
PORTC->PCR[D3] &= ~0x700UL;
PORTC->PCR[D4] &= ~0x700UL;
PORTA->PCR[D5] &= ~0x700UL;
PORTD->PCR[D6] &= ~0x700UL;
PORTD->PCR[D7] &= ~0x700UL;	
	//set mux GPIO
PORTA->PCR[RS] |= PORT_PCR_MUX(1);
PORTD->PCR[E] |= PORT_PCR_MUX(1);
PORTA->PCR[D0] |= PORT_PCR_MUX(1);
PORTA->PCR[D1] |= PORT_PCR_MUX(1);
PORTA->PCR[D2] |= PORT_PCR_MUX(1);	
PORTC->PCR[D3] |= PORT_PCR_MUX(1);
PORTC->PCR[D4] |= PORT_PCR_MUX(1);
PORTA->PCR[D5] |= PORT_PCR_MUX(1);
PORTD->PCR[D6] |= PORT_PCR_MUX(1);
PORTD->PCR[D7] |= PORT_PCR_MUX(1);	
//set DDR
PTA->PDDR |= MASK(RS);
PTD->PDDR |= MASK(E);
PTA->PDDR |= MASK(D0);
PTA->PDDR |= MASK(D1);
PTA->PDDR |= MASK(D2);	
PTC->PDDR |= MASK(D3);
PTC->PDDR |= MASK(D4);
PTA->PDDR |= MASK(D5);
PTD->PDDR |= MASK(D6);
PTD->PDDR |= MASK(D7);

PTA->PCOR |= MASK(RS);
PTD->PCOR |= MASK(E);
PTA->PCOR |= MASK(D0);
PTA->PCOR |= MASK(D1);
PTA->PCOR |= MASK(D2);	
PTC->PCOR |= MASK(D3);
PTC->PCOR |= MASK(D4);
PTA->PCOR |= MASK(D5);
PTD->PCOR |= MASK(D6);
PTD->PCOR |= MASK(D7);

}

