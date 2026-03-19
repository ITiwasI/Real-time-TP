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

void PendSV_Handler(void){
	//Pour les taches utilisant les registres R4 à R11
	
	// stack r4 in the first psp
	__asm__ volatile("push{R4 , R5}"); //same stack than psp one? is the start at psp+8?
	
	// move the psp / save to mem?
	if (__get_PSP()!=0x20001000){
		__set_PSP(0x20001000);
	}
	else{
	__set_PSP(0x20002000);
	}
	// unstack the r4 in other psp for the new task:
	__asm__ volatile("pop {R4, R5}");  // first unstack pb ?
}