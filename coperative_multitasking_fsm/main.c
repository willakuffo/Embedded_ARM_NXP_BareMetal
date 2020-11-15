#include <MKL25Z4.h>
#define MASK(X) (1UL << X) //general mask
#define PORT_D (MASK(12))
#define PORT_A (MASK(9))
#define PORT_C (MASK(11 ))
#define PORT_B (MASK(10))
#define GPIO (MASK(8))

//define 7seg PINOUTS
#define A (1)//PTA1
#define B (2) //PTA2
#define C (4) //PTD4
#define D (12) //PTA12
#define E (4) //PTA4
#define F (5) //PTA5
#define G (13) //PTA13

//buttons
#define SW1 (2) //PDT2
#define SW2 (17) //PTC17
#define SW3 (13) //PTC13 

//onboard leds
#define BLUE (1) //PTD1
#define RED (18) //PTB18
#define GREEN (19) //PTB19

//ecternal leds
#define RED_EXT (2) //PTC2
#define YELLOW_EXT (1) //PTC1

//#define PULL_ENABLE (MASK(1)) //on PCR

//interactive global vars
int go_state = 0,stop = 1;
int speed_flag = 0;
int clockwise = 0;

enum srg_states {GC = 0,CD = 1,DE = 2,EG = 3};//not used
enum srg_states rev_next_state = EG; //not used
enum srg_states fwd_next_state = CD; //not used


enum seg {s1,s2,s3,s4}; //now used intead of above
int next_state = s2,prev_state = s4;

volatile uint32_t counter=0; //counter clock for systeick
unsigned long current_time =0u; //main clock time
unsigned long prev_time =0u; //prev time for external leds flash timing
unsigned long prev_time_onboard =0u; //prev time for onboard leds flash timing
unsigned long onboard_flash_interval =250u; // default flash interval if speed is not set to fast
unsigned long flash_interval=1000; // flash interval for external leds
int det = 0; //determine flash interval for onboard when speed_flag is set
unsigned long red_ext_flash_interval_fwd = 300u; //period is 600ms when forward, for external leds
unsigned long red_ext_prev_time = 0u; //previous flash interval for external red led
unsigned long yellow_ext_flash_interval_fwd = 1000u; //when forward for external yellow led
unsigned long yellow_ext_prev_time = 0u;
unsigned long red_ext_flash_interval_rev = 1000u; //period when reverse
unsigned long yellow_ext_flash_interval_rev = 200u; //when reverse, period of 400ms
unsigned long go_time = 200u; //normal speed
unsigned long go_prev_time = 0u;

//function prototypes
void setup(void);
void checkinput(void);
void flash_external(void);
void forward(void);//not used
void backward(void); //not used
void speed(void);
void flash_onboard(void);
void flash_white(void);
void rotate(void);//forward()and backward() condensed here
unsigned long  millis();


int main(){
	//int i;
setup();
	
	while(1){
		current_time = millis();
	  checkinput();
		
		//evaluate direction and apply proper speed
		if(current_time-go_prev_time>=go_time){
		//clockwise?forward():backward();
		rotate();
		go_prev_time = current_time;
		}
    speed();//determine speed
		flash_onboard();
		flash_external();
		
		//test inputs
		//if (go_state) PTB->PTOR |= MASK(RED);
		//if (speed_flag)PTB->PTOR |= MASK(GREEN);
		//if (clockwise) {	PTD->PTOR |= MASK(BLUE);}
		//else{forward();PTB->PTOR |= MASK(RED);}
		//test inputs
	   //checkinput();
	  //for (int i=0;i<0x35000;i++);
		//if (clockwise) PTB->PTOR|=MASK(RED);
		
}
}

void flash_blue(){
//flash onboard blue
if (current_time-prev_time_onboard>=onboard_flash_interval){
	PTD->PTOR |= MASK(BLUE);
	prev_time_onboard = current_time;
if (speed_flag) det++;// only change flash interval when in fast mode (speed flag is set)
}  
}


void flash_white(){
	//flash onboard white
if (current_time-prev_time_onboard>=onboard_flash_interval){
	PTB->PTOR |= MASK(RED);
	PTB->PTOR |= MASK(GREEN);
	PTD->PTOR |= MASK(BLUE);
	prev_time_onboard = current_time;
if (speed_flag) det++;// only change flash interval when in fast mode
}
}


void flash_onboard(){
	if (go_state){
			if (speed_flag){ //irregular flashing times for onboard
			switch (det){
				//irregular on off flash times for fast mode
				case 0: onboard_flash_interval = 100u;clockwise? flash_white():flash_blue();break; // on time
				case 1: onboard_flash_interval = 200u;clockwise? flash_white():flash_blue();break; // off time
				case 2: onboard_flash_interval = 300u;clockwise? flash_white():flash_blue();break; // on time
				case 3: onboard_flash_interval = 400u;clockwise? flash_white():flash_blue();break; // off time
				case 4: onboard_flash_interval = 500u;clockwise? flash_white():flash_blue();break; // on time..
				case 5: onboard_flash_interval = 600u;clockwise? flash_white():flash_blue(); 
				det = 0;//set det to 0 start irregular flash sequence on fast mode
				break;
		}
		}
		else{
			onboard_flash_interval = 250u; //default flash interval in normal mode
			clockwise? flash_white(): flash_blue();	
	}
	}
}



void speed(){
	go_time = speed_flag? 100u:200u;//double slow or double fast
}


void rotate(){
if (go_state){
if (clockwise){
		switch(next_state){
			case s1: 
				PTA->PTOR |=MASK(E);PTD->PTOR |=MASK(C);
			   next_state=s2;
					prev_state = s4;
					break; 
				case s2: 
				PTA->PTOR |=MASK(G);PTA->PTOR |=MASK(D);
			   next_state=s3;
					prev_state = s1;
					break;
				case s3:
				PTA->PTOR |=MASK(E);PTD->PTOR |=MASK(C);
			   next_state=s4;
				prev_state = s2;	
				break;
				case s4: 
				PTA->PTOR |=MASK(G);PTA->PTOR |=MASK(D);
			   next_state=s1;
					prev_state = s3;
					break;
				}
			}
		else{
		switch(prev_state){
			case s1: 
				PTA->PTOR |=MASK(D);PTA->PTOR |=MASK(G);
			   next_state=s2;
					prev_state = s4;
					break; 
				case s2: 
				PTA->PTOR |=MASK(E);PTD->PTOR |=MASK(C);
			   next_state=s3;
					prev_state = s1;
					break;
				case s3:
				PTA->PTOR |=MASK(D);PTA->PTOR |=MASK(G);
			   next_state=s4;
				prev_state = s2;	
				break;
				case s4: 
				PTD->PTOR |=MASK(C);PTA->PTOR |=MASK(E);
			   next_state=s1;
					prev_state = s3;
					break;
				}
	}
}	

}

void backward(){
 if (go_state){
 //if (clockwise == 0){
 switch(rev_next_state){
		case GC:
			PTA->PTOR |= MASK(E);PTD->PTOR |= MASK(C);
		  rev_next_state = EG;
			fwd_next_state = CD;break;
		case CD:
			PTA->PTOR |= MASK(G);PTA->PTOR |= MASK(D);
			rev_next_state = GC;
			fwd_next_state = DE;break;
		case DE:
			PTD->PTOR |= MASK(C);PTA->PTOR |= MASK(E);
		  rev_next_state = CD;
			fwd_next_state = EG;break;
		case EG:
			PTA->PTOR |= MASK(G);PTA->PTOR |= MASK(D);
			rev_next_state = DE;
			fwd_next_state = GC;break;
		 }
 //}
}
}


void forward(){
	if (go_state){
		//if (clockwise){
	switch(fwd_next_state){
		case GC:
			PTA->PTOR |= MASK(E);PTD->PTOR |= MASK(C);
			fwd_next_state = CD;
			rev_next_state = EG;
			break;
		case CD:
			PTA->PTOR |= MASK(G);PTA->PTOR |= MASK(D);
			fwd_next_state = DE;
			rev_next_state = GC;
			break;
		case DE:
			PTD->PTOR |= MASK(C);PTA->PTOR |= MASK(E);
			fwd_next_state = EG;
			rev_next_state = CD;       
			break;
		case EG:
			PTA->PTOR |= MASK(G);PTA->PTOR |= MASK(D);
			fwd_next_state = GC;
			rev_next_state = DE;
			break;	  
	//}
	}
	}
	}


void flash_external(){

	static int mode = 0;
	if (stop){
		if (current_time-prev_time >= flash_interval){
			switch(mode){
				case 0:
   PTC-> PCOR |= MASK(RED_EXT);
	 PTC-> PSOR |= MASK(YELLOW_EXT);
	 prev_time = current_time;
   mode = 1;break;
				case 1:
	 PTC-> PSOR |= MASK(RED_EXT);
	 PTC-> PCOR |= MASK(YELLOW_EXT);
	 prev_time = current_time;
	 mode = 0;break;
		}
	}
 }
	else {
		if (clockwise){
		if (current_time - yellow_ext_prev_time>= yellow_ext_flash_interval_fwd ){
		PTC->PTOR |= MASK(YELLOW_EXT);
			yellow_ext_prev_time = current_time;
		} //flash yellow external each sec
		
		if (current_time - red_ext_prev_time>= red_ext_flash_interval_fwd ){
		PTC->PTOR |= MASK(RED_EXT);
			red_ext_prev_time = current_time;
		} //flash red external 
		
		
		}
		else{
		
		if (current_time - yellow_ext_prev_time>= yellow_ext_flash_interval_rev ){
		PTC->PTOR |= MASK(YELLOW_EXT);
			yellow_ext_prev_time = current_time;
		} 
		
		if (current_time - red_ext_prev_time>= red_ext_flash_interval_rev ){
		PTC->PTOR |= MASK(RED_EXT);
			red_ext_prev_time = current_time;
		}  
		
		}
		
		}
	//PTC-> PCOR |= MASK(RED_EXT);
	 //PTC-> PCOR |= MASK(YELLOW_EXT);
	
}

void checkinput(){
	//pull douwn default state 0
		//start/stop
		if(PTD ->PDIR & MASK(SW1)){ //check sw1
			//PTB->PTOR |= MASK(RED);
			go_state = 1;
			stop = 0;
		}
		else {
    go_state = 0;
		stop = 1;
		}
		
		//fast/slow
		if(PTC ->PDIR & MASK(SW2)){ //check sw1
		//	PTB->PTOR |= MASK(GREEN);
			speed_flag = 1;
		
		}
		else {
    speed_flag = 0;}
		
		//direction
		if(PTC ->PDIR & MASK(SW3)){ //check sw1
			clockwise = 1;
		//PTD->PTOR |= MASK(BLUE);
		}
		else {
		clockwise = 0;
		}
		
		
}

void setup(){
//clock gate ports to use
	SIM->SCGC5 |= PORT_A |PORT_D |PORT_C |PORT_B; 
	
//clear and set port mux as gpio segments
	PORTA->PCR[A] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[B] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	
	PORTD->PCR[C] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[D] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[E] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[F] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[G] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	
	//clear buttons mux
	PORTD->PCR[SW1] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[SW2] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[SW3] &= ~0x700;//PORT_PCR_MUX_MASK;	//Clear mux
	
	//LEDS
	//onboard
	PORTB->PCR[RED] &= ~ 0x700;
	PORTB->PCR[GREEN] &= ~ 0x700;
	PORTD->PCR[BLUE] &= ~ 0x700;
	
	//external
	PORTC->PCR[RED_EXT] &= ~0x700;
	PORTC->PCR[YELLOW_EXT] &= ~0x700;
	
	//not used. External pull ups configured
	//set pull enable on portD for buttons
  /*PORTD->PCR[SW1] |= PULL_ENABLE; 
	PORTC->PCR[SW2] |= PULL_ENABLE;
	PORTA->PCR[SW3] |= PULL_ENABLE;*/
	
	//set as GPIO mux
	PORTC->PCR[RED_EXT] |= GPIO; //leds ext
	PORTC->PCR[YELLOW_EXT] |= GPIO;
	
	PORTB->PCR[RED] |= GPIO; //onboard leds
	PORTB->PCR[GREEN] |= GPIO;
	PORTD->PCR[BLUE] |= GPIO;
	
	PORTD->PCR[SW1] |= GPIO; // buttons as gpio
	PORTC->PCR[SW2] |= GPIO;
	PORTC->PCR[SW3] |= GPIO;
	
	PORTA->PCR[A] |= GPIO; //segments
	PORTA->PCR[B] |= GPIO; 
	
	PORTD->PCR[C] |= GPIO;
	PORTA->PCR[D] |= GPIO;
	PORTA->PCR[E] |= GPIO;
	PORTA->PCR[F] |= GPIO;
	PORTA->PCR[G] |= GPIO;
	
	//set port ddr
	PTD->PDDR |= MASK(BLUE); //onboard
	PTB->PDDR |= MASK(RED);
	PTB->PDDR |= MASK(GREEN);
	
	PTC->PDDR |= MASK(RED_EXT); //leds ext
	PTC->PDDR |= MASK(YELLOW_EXT);
	
	PTD -> PDDR &= ~MASK(SW1); // set buttons as input
	PTC -> PDDR &= ~MASK(SW2);
	PTC -> PDDR &= ~MASK(SW3);
	
	
	PTA -> PDDR |= MASK(A); //segments
	PTA -> PDDR |= MASK(B);
	PTD -> PDDR |= MASK(C);
	PTA -> PDDR |= MASK(D);
	PTA -> PDDR |= MASK(E);
	PTA -> PDDR |= MASK(F);
	PTA -> PDDR |= MASK(G);
	
	//initially set all indicators to Low
	PTA -> PCOR |= MASK(A);
	PTA -> PCOR |= MASK(B);
	PTD -> PCOR |= MASK(C);
	PTA -> PCOR |= MASK(D);
	PTA -> PCOR |= MASK(E);
	PTA -> PCOR |= MASK(F);
	PTA -> PCOR |= MASK(G);
	
	PTD->PSOR |= MASK(BLUE);
	PTB->PSOR |= MASK(RED);
	PTB->PSOR |= MASK(GREEN);
	
	PTC->PCOR |= MASK(RED_EXT); 
	PTC->PCOR |= MASK(YELLOW_EXT);
	
	
	//init segment indicator ==set current state GC therefore next state 
	PTA -> PSOR |= MASK(G);
	PTD -> PSOR |= MASK(C);
	
	//setup clock
	SysTick->LOAD = (20971520u/1000u)-1 ;  //configure for every half sec restart.
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
    		SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
	
}

void SysTick_Handler(){
	counter++;
}

unsigned long  millis(){
	return (unsigned long)counter;
}

