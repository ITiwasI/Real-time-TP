#include "systick.h"


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


void dummy1(void){	
	__asm__ volatile("MOV r4 , 1");
	while(1){
	}
}
void dummy2(void){
	__asm__ volatile("MOV r4 , 2");
	while(1);
}

void PendSV_Handler(void){
	//Pour les taches utilisant les registres R4 à R11
	//stack on the actual psp...
	__asm__ volatile("str r4 , [%0]" :: "r"(__get_PSP()+1));
	if (current_task==0){
		c_stack_1= *psp_dummy_1;
		c_stack_2= *psp_dummy_2;
	}
	else if (current_task==2){
		c_stack_2=__get_PSP();
		__set_PSP(c_stack_1);
		current_task=1;
	}
	else{		
		c_stack_1=__get_PSP();
		__set_PSP(c_stack_2);
		current_task=2;
	}
	// unstack the r4 in other psp for the new task:
	__asm__ volatile("ldr r4 , [%0]" :: "r"(__get_PSP()+1));
}