#include <MKL25Z4.h>
#define ONBOARD_RED_LED 18
#define MASK(X) (1UL << X)

void setup_onboard_led(void);
void setup_PIT(void);

int main(){
setup_onboard_led();
setup_PIT();
	//PTB->PTOR |= MASK(ONBOARD_RED_LED);
	while(1){
	/*while((PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)==0){}
		PTB->PTOR |= MASK(ONBOARD_RED_LED);
	  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;//clear flag*/
	}	
	}

void setup_onboard_led(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;//clock gate port B
  //clear and set mux
	PORTB->PCR[ONBOARD_RED_LED] &= ~0x700UL; //clear mux
	PORTB->PCR[ONBOARD_RED_LED] |= PORT_PCR_MUX(1); //set GPIO
	//set DDR
	PTB->PDDR |= MASK(ONBOARD_RED_LED); //set output
	PTB->PSOR |= MASK(ONBOARD_RED_LED);	
}

void setup_PIT(){
SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;//clock gate pit
	
PIT->MCR &= ~ PIT_MCR_MDIS_MASK; //enable pit module by clear
PIT->CHANNEL[0].LDVAL =0x75300; //configure for 100ms
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enable timer
PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;//enable interrupt
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn, 3);
	NVIC_EnableIRQ(PIT_IRQn);

}

void PIT_IRQHandler(){
	PTB->PTOR |= MASK(ONBOARD_RED_LED);
	  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;//clear flag
}
