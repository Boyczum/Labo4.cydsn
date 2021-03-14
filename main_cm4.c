/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "params.h"
#include "queue.h"


SemaphoreHandle_t bouton_semph = NULL;

void vLedTask(){
    for(;;){
        Cy_GPIO_Write(LED_PORT,LED_NUM,1);
        vTaskDelay(pdMS_TO_TICKS(500));
        Cy_GPIO_Write(LED_PORT,LED_NUM,0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void isr_bouton(){
    
    xSemaphoreGiveFromISR(bouton_semph,NULL);
    Cy_GPIO_ClearInterrupt(Bouton_0_PORT, Bouton_0_NUM);
    NVIC_ClearPendingIRQ(Bouton_ISR_cfg.intrSrc);
}

void bouton_task(){

    for(;;){
        
        xSemaphoreTake(bouton_semph,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(20));
            
        if(Cy_GPIO_Read(Bouton_0_PORT,Bouton_0_NUM)==0){
            UART_1_PutString("Boutonappuye \n\r");
        }
        else if(Cy_GPIO_Read(Bouton_0_PORT,Bouton_0_NUM)==1){
            UART_1_PutString("Boutonrelache \n\r");
        }    
        
        bouton_semph = xSemaphoreCreateBinary();
    }
}


int main(void)
{
    bouton_semph = xSemaphoreCreateBinary();
    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_SysInt_Init(&Bouton_ISR_cfg, isr_bouton);
    NVIC_ClearPendingIRQ(Bouton_ISR_cfg.intrSrc);
    NVIC_EnableIRQ(Bouton_ISR_cfg.intrSrc);
    
    UART_1_Start();
    
    xTaskCreate(vLedTask,"BLINK LED",80,NULL,3,NULL);
    xTaskCreate(bouton_task,"BOUTON",80,NULL,3,NULL);

    vTaskStartScheduler();
    
    for(;;)
    {
        /* Place your application code here. */
    }
    
   
}

/* [] END OF FILE */
