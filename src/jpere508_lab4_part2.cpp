// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 5  Exercise 2
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
//   return (b ? (x | (0x01 << k)) : (x & ~(0x01 << k)));
// }
// unsigned char GetBit(unsigned char x, unsigned char k)
// {
//   return ((x & (0x01 << k)) != 0);
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
//   PORTD = (PORTD & 0b01111100) | ((nums[num] & 0b01000000) << 1) |
//           (PORTD & 0b00011100);
//   PORTB = nums[num] & 0b00111111;
// }

// void ADC_init()
// {
//   ADMUX = (1 << REFS0);
//   ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
//   ADCSRB = (1 << ACME);
// }

// unsigned int ADC_read(unsigned char chnl)
// {
//   ADMUX = (ADMUX & 0xF0) | (chnl & 0x0F);
//   ADCSRA |= (1 << ADSC);

//   while ((ADCSRA >> 6) & 0x01)
//   {
//   }

//   uint8_t low = ADCL;
//   uint8_t high = ADCH;

//   return ((high << 8) | low);
// }
// long map(long x, long in_min, long in_max, long out_min, long out_max)
// {
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }
// enum states
// {
//   calibration,
//   monitor
// } state;

// static unsigned char target = 0;
// void Tick()
// {
//   static unsigned char prevBtn = 0;
//   unsigned char btn = GetBit(PINC, PC2);

//   static unsigned char blinkCounter = 0;
//   static unsigned char ledOn = 0;

//   switch (state)
//   {
//   case monitor:
//     if (btn && !prevBtn)
//       state = calibration;
//     break;
//   case calibration:
//     if (btn && !prevBtn)
//       state = monitor;
//     break;
//   default:
//     state = calibration;
//     break;
//   }

//   switch (state)
//   {
//   case monitor:
//   {
//     PORTD &= ~(1 << PD6);
//     PORTD |= (1 << PD5);
//     PORTD &= ~((1 << PD3) | (1 << PD2) | (1 << PD4));
//     int num = ADC_read(1);
//     num = map(num, 0, 1023, 0, 15);
//     target = num;
//     outNum(num);
//     // serial_println(num);
//     break;
//   }
//   case calibration:
//   {
//     PORTD &= ~(1 << PD5);
//     PORTD |= (1 << PD6);
//     int measured = ADC_read(0);
//     measured = map(measured, 0, 1023, 0, 15);
//     outNum(measured);
//     // serial_println(measured);

//     if (measured < (target - 4))
//     {
//       ledOn = !ledOn;
//       if (ledOn)
//       {
//         PORTD |= (1 << PD4);
//         PORTD &= ~((1 << PD3) | (1 << PD2));
//       }
//       else
//       {
//         PORTD &= ~(1 << PD4);
//       }
//     }
//     else if (((target - 4) <= measured) && (measured < target))
//     {
//       blinkCounter++;
//       if (blinkCounter >= 3)
//       {
//         ledOn = !ledOn;
//         blinkCounter = 0;
//       }

//       if (ledOn)
//       {
//         PORTD &= ~(1 << PD2);
//         PORTD |= (1 << PD3);
//         PORTD |= (1 << PD4);
//       }
//       else
//       {
//         PORTD &= ~(1 << PD3);
//         PORTD &= ~(1 << PD4);
//       }
//     }
//     else if (measured >= target)
//     {
//       PORTD |= (1 << PD3);
//       PORTD &= ~((1 << PD2) | (1 << PD4));
//     }
//     break;
//   }
//   default:
//     break;
//   }
// }
// int main(void)
// {
//   state = monitor;
//   DDRC = 0x00;
//   PORTC = 0x00;
//   DDRD = 0b11111100;
//   // PORTD |= (1 << PD3);
//   PORTD |= (1 << PD5);
//   DDRB = 0b11111111;
//   // PORTB = 0b11111111;
//   ADC_init();
//   // serial_init(9600);
//   TimerSet(250);
//   TimerOn();

//   while (1)
//   {
//     Tick();
//     while (!TimerFlag)
//     {
//     }
//     TimerFlag = 0;
//   }
//   return 0;
// }