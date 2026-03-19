#include "stm32f10x.h"
#include "systick.h"



void dummy1(void){
	while(1);
}
void dummy2(void){
	while(1);
}

int main ( void )
{
	//Initialisation
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 4) ;
	
	GPIOA->CRL &= ~(0xF<<4) ;
	GPIOA->CRL |= (0x2<<4) ; //pin 1
	
	NVIC_SetPriority ( SysTick_IRQn , 0x00 ) ;// Pour la priorité du Pendsv
	NVIC_SetPriority ( PendSV_IRQn , 0x03 ) ;
	SysTick_Init();
	
	//Addressage des fonctions dans les psp respectifs
	uint32_t * psp_dummy_1 = (uint32_t *) 0x20001000;
	*psp_dummy_1 = 0x00; //R0
	*(psp_dummy_1+1) = 0x00; //R1
	*(psp_dummy_1+2) = 0x00; //R2
	*(psp_dummy_1+3) = 0x00; //R3
	*(psp_dummy_1+4) = 0x00; //R12
	*(psp_dummy_1+5) = 0x00; //LR
	*(psp_dummy_1+6) = (uint32_t) dummy1; //PC
	*(psp_dummy_1+7) = 0x01000000; //xPSR
	
	uint32_t * psp_dummy_2 = (uint32_t *) 0x20002000;
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
	__set_PSP(0x20001000);
	
	//Passage en mode protégé "unpriviledge" et "PSP"
	int ctrl;
	ctrl =__get_CONTROL();
	ctrl =ctrl|0b11;
	__set_CONTROL(ctrl);
	
	while (1)
	{
	}
}
