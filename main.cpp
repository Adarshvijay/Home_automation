/***************************************************
  COURSEWORK : EMBEDDED SYSTEM PROGRAMING 

  TITLE : SMART HOME USING MQ135,LDR AND IR SENSORS

  DEVELOPED BY :   ADARSH VIJAY (11556493)
  
 ****************************************************/


#include "mbed.h"  
#include "MQ135.h"
#include <LCD_DISCO_F469NI.h>

uint8_t text[30];
AnalogIn IR(PB_1); // assigning ldr as analog input A0
AnalogIn ldr(PC_2); // assigning ldr as analog input A1
DigitalOut buz(PG_9); // DigitalOut buzzer(D0);
DigitalOut myled(PD_4); // DigitalOut myled(LED2);
AnalogIn MQ(PC_3);   // assigning Analog A2 pin for MQ135 

LCD_DISCO_F469NI lcd;

static BufferedSerial pc(USBTX, USBRX, 9600); // tx, rx, baud rate

uint8_t ADC_to_cm(uint16_t ADC_value);

const uint16_t Look_up_Table[8][3]={
    {10,46670,3177},
    {15,33761,2582},
    {20,25817,1589},
    {30,18866,1390},
    {50,10923,794},
    {70,7944,149},
    {80,7547,20}
};

FileHandle *mbed::mbed_override_console(int fd)
{
    return &pc;
} 
int main(void)
{
       while(1)
    {

        unsigned short IR_ADC = 17300;              // ADC value for 43 cm 
        unsigned short ldrout=ldr.read_u16();         //read ldr value  
        lcd.Clear(LCD_COLOR_BLACK);
        lcd.SetFont(&Font20);
        lcd.SetBackColor(LCD_COLOR_BLACK);
        lcd.SetTextColor(LCD_COLOR_WHITE);

        float adcRaw= MQ.read_u16()/64;
        //Clean air
        if(adcRaw <= 55)
        {
        printf("Clean air \n\r");
        printf("\n\r");
            
        lcd.DisplayStringAt(0,LINE(7),(uint8_t *)"Clean Air",CENTER_MODE);

        }
        
        //Air with CO2 (Respirable by human)
        if( adcRaw >= 180 && adcRaw <= 400)
        {
        printf("Air with CO2 \n\r");
        printf("\n\r");

        lcd.DisplayStringAt(0,LINE(7),(uint8_t *)"Air with CO2",CENTER_MODE);
        }
        
        //Oxygen in Air
        if( adcRaw >= 74 && adcRaw <= 180)
        {
            printf("Breathable Air \n\r");
            printf("\n\r");

            lcd.DisplayStringAt(0,LINE(7),(uint8_t *)"Breathable Air",CENTER_MODE);
        }
        
        //propane and butane gas
        if(adcRaw >= 400)
        {
            printf("Propane and butane \n\r");
            printf("\n\r");

            lcd.DisplayStringAt(0,LINE(7),(uint8_t *)"Propane and butane",CENTER_MODE);
        }

        if(ldrout>48000)  //if luminescence value is greater than Roomlight, Orange LED turns on 
	    {     
                                      
            printf("LDR Output :Night mode  %d\n ",ldrout);       //print ldr value on LCD
            myled = 0;                                             //LED OFF            

            lcd.DisplayStringAt(0,LINE(6),(uint8_t *)"LDR Output :Night mode",CENTER_MODE);                                           
         } 
	    else      
	    {   
            printf("LDR Output : Normal light %d\n ",ldrout);
            myled = 1;  

            lcd.DisplayStringAt(0,LINE(6),(uint8_t *)"LDR Output : Normal light",CENTER_MODE);                          
             
        } 
        if (IR.read_u16() < IR_ADC)
        { 
            printf("IR Distance in cm %d \n", ADC_to_cm(IR.read_u16()));   

            sprintf((char*)text, "IR Distance in cm: %d",ADC_to_cm(IR.read_u16()));
            lcd.DisplayStringAt(0, LINE(5), (uint8_t *)&text, CENTER_MODE);   
        }          
        else
        { 
            buz=1;
            printf("Intruder alert \n");     
            lcd.DisplayStringAt(0,LINE(5),(uint8_t *)"Intruder alert",CENTER_MODE);           
            buz=0;
        }
        wait_us(100000); 
    } 
}
    
 uint8_t ADC_to_cm(uint16_t ADC_value)
{
    uint8_t index;
   for(index=0;index<7;index++)
   {
      if(ADC_value>Look_up_Table[index][1])
      {
        return(Look_up_Table[index][0]-(ADC_value-Look_up_Table[index][1])/Look_up_Table[index][2]);
      }
        
    } 
   return(0); 
}   