#include <MKL25Z4.h>

#define MASK(X) (1UL<<X)
#define PTD0 (0) //TPM0_CH0
#define PTD1 (1) //TPM0_CH1
#define PTD2 (2) //TPM0_CH2
void init_Timer(void);
void init_pin(void);
int main(){

	init_Timer();
	init_pin();

	while(1){}
}


void init_Timer(){
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;	//*******TPM0 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xCCD;// 

	//working wth TPM0_C0SC
						//pwm
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[0].CnV =0xA4;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[1].CnV =0xF6;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[2].CnV =0xA4;	//no atter the time, the period will be same just delayed/phase shift
	
	TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;

	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}
//TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  |TPM_SC_CMOD(1);
//ps(5) is about 100ms


void TPM0_IRQHandler(){
	//optionally check the flag
	if (TPM0->STATUS & TPM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM0->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	if (TPM0->STATUS & TPM_STATUS_CH1F_MASK){		//alternative check using the status flag
	//if (TPM0->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[1].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	if (TPM0->STATUS & TPM_STATUS_CH2F_MASK){		//alternative check using the status flag
	//if (TPM0->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		TPM0->CONTROLS[2].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}

}


void init_pin(){
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[PTD0] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[PTD0] |= PORT_PCR_MUX(4);	//setup to be GPIO
	
	PORTD->PCR[PTD1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[PTD1] |= PORT_PCR_MUX(4);	//setup to be GPIO
	
	PORTD->PCR[PTD2] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[PTD2] |= PORT_PCR_MUX(4);	//setup to be GPIO
	
}
