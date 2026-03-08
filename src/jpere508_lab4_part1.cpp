// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 5  Exercise 1
// //    Exercise Description:
// //    I acknowledge all content contained herein, excluding template or example
// //    code, is my own original work.

// //    Demo Link:
// #include "timerISR.h"
// #include <avr/interrupt.h>
// #include <avr/io.h>
// #include <util/delay.h>

// unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
// {
//     return (b ? (x | (0x01 << k)) : (x & ~(0x01 << k)));
// }
// unsigned char GetBit(unsigned char x, unsigned char k)
// {
//     return ((x & (0x01 << k)) != 0);
// }
// int nums[16] = {
//     0b0111111, // 0
//     0b0000110, // 1
//     0b1011011, // 2
//     0b1001111, // 3
//     0b1100110, // 4
//     0b1101101, // 5
//     0b1111101, // 6
//     0b0000111, // 7
//     0b1111111, // 8
//     0b1101111, // 9
//     0b1110111, // A
//     0b1111100, // b
//     0b0111001, // c
//     0b1011110, // d
//     0b1111001, // E
//     0b1110001  // F
// };

// void outNum(int num)
// {
//     PORTD = (PORTD & 0b01111100) | ((nums[num] & 0b01000000) << 1) |
//             (PORTD & 0b00011100);
//     PORTB = nums[num] & 0b00111111;
// }

// void ADC_init()
// {
//     ADMUX = (1 << REFS0);
//     ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
//     ADCSRB = (1 << ACME);
// }

// unsigned int ADC_read(unsigned char chnl)
// {
//     ADMUX = (ADMUX & 0xF0) | (chnl & 0x0F);
//     ADCSRA |= (1 << ADSC);

//     while ((ADCSRA >> 6) & 0x01)
//     {
//     }

//     uint8_t low = ADCL;
//     uint8_t high = ADCH;

//     return ((high << 8) | low);
// }
// long map(long x, long in_min, long in_max, long out_min, long out_max)
// {
//     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }
// enum states
// {
//     green,
//     yellow,
//     red
// } state;
// void Tick()
// {
//     int num = ADC_read(1);
//     num = map(num, 0, 1023, 0, 15);
//     static unsigned char blinkCounter = 0;
//     static unsigned char ledOn = 0;
//     // serial_println(num);

//     switch (state)
//     {
//     case green:
//         if (num < 13)
//         {
//             state = yellow;
//         }
//         break;
//     case yellow:
//         if (num < 7)
//         {
//             state = red;
//         }
//         else if (num >= 13)
//         {
//             state = green;
//         }
//         break;
//     case red:
//         if (num > 6)
//         {
//             state = yellow;
//         }
//         break;
//     default:
//         break;
//     }

//     switch (state)
//     {

//     case red:
//         outNum(num);
//         blinkCounter++;
//         if (blinkCounter >= 2)
//         {
//             ledOn = !ledOn;
//             blinkCounter = 0;
//         }

//         if (ledOn)
//             PORTD |= (1 << PD4);
//         else
//             PORTD &= ~(1 << PD4);

//         PORTD &= ~((1 << PD2) | (1 << PD3));
//         break;
//     case green:
//         outNum(num);
//         PORTD |= (1 << PD3);
//         PORTD &= ~((1 << PD2) | (1 << PD4));
//         break;
//     case yellow:
//         outNum(num);
//         PORTD &= ~(1 << PD2);
//         PORTD |= (1 << PD3);
//         PORTD |= (1 << PD4);
//         break;
//     default:
//         PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4));
//         break;
//     }
// }
// int main(void)
// {
//     DDRC = 0x00;
//     PORTC = 0x00;
//     DDRD = 0b11111100;
//     // PORTD |= (1 << PD3);
//     PORTD |= (1 << PD6);
//     DDRB = 0b11111111;
//     // PORTB = 0b11111111;
//     ADC_init();
//     // serial_init(9600);
//     TimerSet(500);
//     TimerOn();

//     int num = map(ADC_read(1), 0, 1023, 0, 15);
//     if (num < 7)
//         state = red;
//     else if (num < 13)
//         state = yellow;
//     else
//         state = green;

//     while (1)
//     {
//         Tick();
//         while (!TimerFlag)
//         {
//         }
//         TimerFlag = 0;
//     }
//     return 0;
// }