#include "stm32f10x.h"


uint32_t * c_stack_1;
uint32_t * c_stack_2;
uint32_t * psp_dummy_1;
uint32_t * psp_dummy_2;
int current_task=0;
	
void SysTick_Init(void){
	
	SysTick->CTRL|=0b101; // Clock source selection processor clock 72MHz 
	// et enable du counter
	SysTick->LOAD=71999;// 72MHz/72000 = 1kHz donc 1 ms
}

void SysTick_Int_Enable(void) {
	SysTick->CTRL|=0b10;
		// systick exception enable
}

void SysTick_Handler(void){
	//Activer les taches ne requierant pas de R4 à R11
	SCB->ICSR|=SCB_ICSR_PENDSVSET;
}

uint32_t bob1;
uint32_t bob2;

void dummy1(void){	
	bob1=1;
	__asm__ volatile("ldr r4 , [%0]" :: "r"(&bob1));
	while(1){
	}
}
void dummy2(void){
	bob2=2;
	__asm__ volatile("ldr r4 , [%0]" :: "r"(&bob2));
	while(1){
	};
}

int compteur_max=80;
int compteur=0;

void PendSV_Handler(void){
	
	if (current_task==0){
		c_stack_1= psp_dummy_1;
		c_stack_2= psp_dummy_2;
		//Init du PSP
		__set_PSP((uint32_t) c_stack_1);
		current_task=1;
	}
	else {
		compteur+=1;
		if (compteur>=compteur_max){
			compteur=0;
		}
		if (((compteur>=0) && (compteur<12))||((compteur >= 20) && (compteur<38))||((compteur >= 42) && (compteur<60))||((compteur >= 68) && (compteur<80))){
			if (current_task==1){
				__asm__ volatile("str r4 , [%0]" :: "r"(__get_PSP()-4));
				c_stack_1= (uint32_t *) __get_PSP();
				__set_PSP((uint32_t) c_stack_2);
				current_task=2;		
				__asm__ volatile("ldr r4 , [%0]" :: "r"(__get_PSP()-4));
			}
		}else{
			if (current_task==2){
				__asm__ volatile("str r4 , [%0]" :: "r"(__get_PSP()-4));
					c_stack_2=(uint32_t *) __get_PSP();
				__set_PSP((uint32_t) c_stack_1);
				current_task=1;
				__asm__ volatile("ldr r4 , [%0]" :: "r"(__get_PSP()-4));
			}
		}
	}
}

//.....................................................................................................
int main ( void ){
	//Initialisation

	
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 4) ;
	
	GPIOA->CRL &= ~(0xF<<4) ;
	GPIOA->CRL |= (0x2<<4) ; //pin 1
	
	NVIC_SetPriority ( SysTick_IRQn , 0x00 ) ;// Pour la priorité du Pendsv
	NVIC_SetPriority ( PendSV_IRQn , 0x03 ) ;
	SysTick_Init();
	
	psp_dummy_1 = (uint32_t *) 0x20001000;
	psp_dummy_2 = (uint32_t *) 0x20002000;
	
	*psp_dummy_1 = 0x00; //R0
	*(psp_dummy_1+1) = 0x00; //R1
	*(psp_dummy_1+2) = 0x00; //R2
	*(psp_dummy_1+3) = 0x00; //R3
	*(psp_dummy_1+4) = 0x00; //R12
	*(psp_dummy_1+5) = 0x00; //LR
	*(psp_dummy_1+6) = (uint32_t) dummy1; //PC
	*(psp_dummy_1+7) = 0x01000000; //xPSR
	
	*psp_dummy_2 = 0x00; //R0
	*(psp_dummy_2+1) = 0x00; //R1
	*(psp_dummy_2+2) = 0x00; //R2
	*(psp_dummy_2+3) = 0x00; //R3
	*(psp_dummy_2+4) = 0x00; //R12
	*(psp_dummy_2+5) = 0x00; //LR
	*(psp_dummy_2+6) = (uint32_t) dummy2; //PC
	*(psp_dummy_2+7) = 0x01000000; //xPSR
	//Autorisation des interruptions
	SysTick_Int_Enable();
	
	//Init du PSP
		__set_PSP(0x20003000);
	
	//Passage en mode protégé "unpriviledge" et "PSP"
	int ctrl;
	ctrl =__get_CONTROL();
	ctrl =ctrl|0b11;
	__set_CONTROL(ctrl);
	
	while (1)
	{
	}
}
