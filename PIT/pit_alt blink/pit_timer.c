#include <MKL25Z4.h>
#define ONBOARD_RED_LED (18) //ptb18
#define SIGNAL (0) //ptb0
#define MASK(X) (1UL << X)
int flag =1;
int interrupt_400ms = 0;
int interrupt_100ms = 0;

 
void setup_onboard_led(void);
void setup_PIT(void);

int main(){
setup_onboard_led();
setup_PIT();
	//PTB->PTOR |= MASK(ONBOARD_RED_LED);
	while(1){
	/*
		use flag checking
		while((PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)==0){}
		PTB->PTOR |= MASK(ONBOARD_RED_LED);
	  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;//clear flag*/
	}	
	}

void setup_onboard_led(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;//clock gate port B
  //clear and set mux
	PORTB->PCR[ONBOARD_RED_LED] &= ~0x700UL; //clear mux
	PORTB->PCR[ONBOARD_RED_LED] |= PORT_PCR_MUX(1); //set GPIO
	PORTB->PCR[SIGNAL] &= ~0x700UL; //clear mux
	PORTB->PCR[SIGNAL] |= PORT_PCR_MUX(1); //set GPIO
	
	//set DDR
	PTB->PDDR |= MASK(ONBOARD_RED_LED); //set output
	PTB->PSOR |= MASK(ONBOARD_RED_LED);
	
	PTB->PDDR |= MASK(SIGNAL); //set output
	PTB->PSOR |= MASK(SIGNAL);	
}

void setup_PIT(){
SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;//clock gate pit
	
PIT->MCR &= ~ PIT_MCR_MDIS_MASK; //enable pit module by clear

 PIT->CHANNEL[0].LDVAL =0x3FC820; //configure for 400ms
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enable timer
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;//enable interrupt

	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn, 3);
	NVIC_EnableIRQ(PIT_IRQn);

}

void PIT_IRQHandler(){
	//use interrupts
	//static int interrupt_400ms = 0;
	//static int interrupt_100ms = 0;
	//static int flag = 1; //raised when  interrupt_400ms = 5, initally configured for 400ms as was setup
	
	//for channel 0
	
	flag? interrupt_400ms ++:interrupt_100ms++; //count the interrupt
	PTB->PTOR |= MASK(ONBOARD_RED_LED);
	PTB->PTOR |= MASK(SIGNAL);
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;//clear flag
	

	if (interrupt_400ms >=10){
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disable timer
	PIT->CHANNEL[0].LDVAL =0xFF208; //configure for 100ms
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enable timer
	flag = 0;//now count for 100ms
		interrupt_400ms =0;
	}
	if (interrupt_100ms >=10){
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disable timer
	PIT->CHANNEL[0].LDVAL =0x3FC820; //configure for 400ms
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enable timer
		
	flag = 1;//now count for 400ms
		interrupt_100ms=0;
	}
	
	
	
	
}
