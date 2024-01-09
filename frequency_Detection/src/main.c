/* Name: 2.4.2.c
 * Author: Tejendra Patel
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h"
#define Timer_Freq 15625UL      //16MHz/1024          

int a;       //firstrising_edge
int b;       //firstfalling_edge
int c;       //secondfalling_edge
int freq = -1;

int readICR3() {
    while (!bit_is_set(TIFR3, ICF3)){
    } 
    return ICR3;
}

void clearICF3Flag() {
    set(TIFR3, ICF3);
}

// void updateFrequency(int a, int b, int c) {
//     if (a < b && b < c) {               // Check for valid condition
//         int period = c - a;            // Time between two rising edge
//         freq = Timer_Freq / period;   

//         if (freq > 500 && freq < 600) {
//             set(PORTB, 6);                // Green Led On at 662 hz
//             clear(PORTB, 5);            
//         } 
//         else if (freq > 18 && freq < 28) {
//             clear(PORTB, 6);                
//             set(PORTB, 5);             // Red Led On at 25 Hz
//         } 
//         else if (freq==-1) {
//             clear(PORTB, 6);
//             clear(PORTB, 5);
//         }
//     }
// }


int main(void)
{
    _clockdivide(0); // Set the clock speed to 16MHz
    clear(DDRC, 7);    // Set pin 7 of port B as an output
    set(DDRB, 6); 
    set(DDRB, 5); 
    m_usb_init();  


    // 1024 Prescaler
    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30); 


    for (;;)
    { 

        set(TCCR3B,ICES3);          //store timer value on rising edge
		clear(PORTB, 6);
    	clear(PORTB, 5);
        a=readICR3();               
        clearICF3Flag();

        clear(TCCR3B,ICES3);        //store timer value on falling edge 
        b=readICR3();   
        clearICF3Flag(); 

        set(TCCR3B,ICES3);          //store timer value on rising edge
        c=readICR3();
        clearICF3Flag(); 
        

        int period = c - a;            // Time between two rising edge
        freq = Timer_Freq / period;   

        if (freq > 500 && freq < 600) {
            set(PORTB, 6);                // Green Led On at 662 hz
            clear(PORTB, 5);            
        } 
        else if (freq > 18 && freq < 28) {
            clear(PORTB, 6);                
            set(PORTB, 5);             // Red Led On at 25 Hz
        } 
        else{
            clear(PORTB, 6);
            clear(PORTB, 5);
        }
        
        // updateFrequency(a,b,c); 

        m_usb_tx_uint(freq);
        m_usb_tx_string("\n"); 

		_delay_ms(100);

        // m_usb_tx_string("start");
        
       
    }

    return 0;   /* Never reached */

}

//Frequency Detection Circuit
