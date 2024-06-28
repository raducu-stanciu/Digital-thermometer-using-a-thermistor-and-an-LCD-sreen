
#include <mega164a.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alcd_twi.h>
//#include <stdout>
#include <i2c.h>
#include <stdio.h>
#include <math.h>
#define PCF8574_I2C_ADDRESS 0x27 
#define degree_sysmbol 0xdf
#define BUTON_PIN PIND.5
   
  




void init_ADC() {
    // Setarea ADC pentru a citi de pe pinul ADC0 (Port A0)
    ADMUX |= (1<<REFS0); // Tensiunea de referinta este AVCC cu capat inferior la GND
    ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // Activeaza ADC si seteaza factorul de divizare a frecventei de ceas la 128
}
// Functia de citire a valorii ADC de pe termistor
int ADC_Read(char channel)
{
    int Ain,AinLow;
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);    /* Set input channel to read */
    ADCSRA = (ADCSRA & 0xF8) | 0x05;
    ADCSRA |= (1<<ADSC);        /* Start conversion */
    while((ADCSRA&(1<<ADIF))==0);    /* Monitor end of conversion interrupt */
    delay_us(10);
    AinLow = (int)ADCL;        /* Read lower byte*/
    Ain = (int)ADCH*256;        /* Read higher 2 bits and 
                    Multiply with weight */
    Ain = Ain + AinLow;                
    return(Ain);            /* Return digital value*/
}
void display_temperature_bar_graph(float temperature) {
    int i = 0      ;
    int bar_height = (temperature * 16) / 38; // Map temperature to bar height (adjust max temp based on sensor range)
   
    
    for ( i ; i < bar_height; i++) {   
        
         lcd_printfxy(i,1,"*"); // Print filled blocks for 
        } 
        }

  
void main(void)
{
twi_master_init(100);  
#asm("sei")    
lcd_twi_init(PCF8574_I2C_ADDRESS,16);
init_ADC() ;
DDRA &= ~(1<<DDA0);
DDRB |= (1<<DDB0);
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
PORTB=(1<<PORTB7) | (1<<PORTB6) | (1<<PORTB5) | (1<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);
//DDRB |= (1<<DDB0);
PORTD=   (1<<PORTD5) ;

while (1)
    {       
    int value_adc = ADC_Read(0);
    /*float voltage = (value_adc / 1023.0) *2.56;    */  
    //float adc=(float)value_adc;
    float voltage = (value_adc / 1023.0) *5;
    float Rt = 9980 * voltage / (5 - voltage);
    float temperature_K = 1 / (1/(273.15+25) + log(Rt/10000)/3950.0  );
    float temperature_C = temperature_K - 273.15;
    float temperature_F=  (temperature_C-2) *1.80 +32; 
    int volt= (int)voltage;   
    int temp_C= (int)(temperature_C-2);
    int temp_K= (int)(temperature_K-2);
    int temp_F= (int)temperature_F; 
    
       
    if(temp_C>25){ 
    PORTB=(1<<PORTB7) | (1<<PORTB6) | (1<<PORTB5) | (1<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);      
   

    lcd_printfxy(0,0,"Temperatura %d%cC",temp_C,degree_sysmbol );         
    lcd_printfxy(0,1," Ridicata         "); 
    if(BUTON_PIN==0){
    lcd_printfxy(0,0," %dK  %dF         ",temp_K ,temp_F );
    display_temperature_bar_graph(temperature_C-2);
    //lcd_printfxy(0,1," Ridicata         "); 
    }
    }else {    
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
     
    lcd_printfxy(0,0,"Temperatura %d%cC ",temp_C,degree_sysmbol );         
    lcd_printfxy(0,1," Normala        "); 
    if(BUTON_PIN==0){
    lcd_printfxy(0,0," %dK  %dF         ",temp_K ,temp_F );     
    display_temperature_bar_graph(temperature_C-2);
    //lcd_printfxy(0,1," Normala ");
    }
    }
    
    
     /* 500ms delay */ 
    delay_ms(500);
    }
}






