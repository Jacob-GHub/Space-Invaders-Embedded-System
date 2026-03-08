// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 6  Exercise 2
// //    Exercise Description:
// //    I acknowledge all content contained herein, excluding template or example
// //    code, is my own original work.

// //    Demo Link: https://www.youtube.com/watch?v=ZKU8YXQZf3Y

// #include <avr/interrupt.h>
// #include <avr/io.h>
// #include "periph.h"
// #include "helper.h"
// #include "timerISR.h"

// // TODO: Complete dir array to be able to display L,r,U,d and - with outDir function
// int dir[5] = {0b0001110,  // L
//               0b0000101,  // r
//               0b0111110,  // U
//               0b0111101,  // d
//               0b0000001}; // notjing
// // a  b  c  d  e  f  g

// void outDir(int num)
// {
//     PORTD = dir[num] << 1;
//     PORTB = SetBit(PORTB, 0, dir[num] & 0x01);
// }

// int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001}; // 8 phases of the stepper motor step
// // TODO: declare variables for cross-task communication here

// int x = 0;
// int y = 0;
// int direction = 4;
// int curStep = 0;

// // TODO: declare the total number of tasks here
// #define NUM_TASKS 3 /*number of task here*/

// // Task struct for concurrent synchSMs implmentations
// typedef struct _task
// {
//     unsigned int state;        // Task's current state
//     unsigned long period;      // Task period
//     unsigned long elapsedTime; // Time elapsed since last task tick
//     int (*TickFct)(int);       // Task tick function
// } task;

// const unsigned long TASK1_PERIOD = 100;
// const unsigned long TASK2_PERIOD = 1;
// const unsigned long TASK3_PERIOD = 100;
// const unsigned long GCD_PERIOD = findGCD(TASK1_PERIOD, findGCD(TASK2_PERIOD, TASK3_PERIOD));

// task tasks[NUM_TASKS]; // declared task array with 5 tasks (2 in exercise 1)

// // TODO: Define, for each task:
// //  (1) enums and
// //  (2) tick functions

// // Example below
// // enum ABC_States {ABC_INIT, ABC_DO_STH};
// // int TickFct_ABC(int state);

// enum joystickStates
// {
//     read
// } joystickState;

// enum stepperStates
// {
//     init,
//     cw,
//     ccw
// } stepperState;

// enum ledStates
// {
//     left,
//     right,
//     none
// } ledState;

// int stepper_tick(int state);
// int joystick_tick(int state);
// int led_tick(int state);
// int sys_tick(int state);

// void TimerISR()
// {

//     for (unsigned int i = 0; i < NUM_TASKS; i++)
//     { // Iterate through each task in the task array
//         if (tasks[i].elapsedTime == tasks[i].period)
//         {                                                      // Check if the task is ready to tick
//             tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
//             tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
//         }
//         tasks[i].elapsedTime += GCD_PERIOD; // Increment the elapsed time by GCD_PERIOD
//     }
// }

// int main(void)
// {
//     // TODO: initialize all your inputs and ouputs
//     DDRC = 0b00000011;
//     PORTC = 0b11111100;

//     DDRB = 0b1111111;
//     PORTB = 0;
//     PORTB &= ~(1 << PB1);

//     DDRD = 0b11111111;
//     PORTD = 0;

//     ADC_init(); // initializes ADC

//     // TODO: Initialize tasks here
//     //  Example given below
//     //  tasks[0].period = TASK1_PERIOD;
//     //  tasks[0].state = ABC_INIT;
//     //  tasks[0].elapsedTime = TASK1_PERIOD;
//     //  tasks[0].TickFct = &TickFct_ABC;

//     tasks[0].period = TASK1_PERIOD;
//     tasks[0].state = read;
//     tasks[0].elapsedTime = 0;
//     tasks[0].TickFct = &joystick_tick;

//     tasks[1].period = TASK2_PERIOD;
//     tasks[1].state = init;
//     tasks[1].elapsedTime = 0;
//     tasks[1].TickFct = &stepper_tick;

//     tasks[2].period = TASK3_PERIOD;
//     tasks[2].state = none;
//     tasks[2].elapsedTime = 0;
//     tasks[2].TickFct = &led_tick;

//     TimerSet(GCD_PERIOD);
//     TimerOn();

//     while (1)
//     {
//     }

//     return 0;
// }

// int joystick_tick(int state)
// {
//     switch (state)
//     {
//     case read:
//     {
//         x = ADC_read(2);
//         y = ADC_read(3);

//         if (y < 300)
//         {
//             direction = 0;
//             outDir(0);
//         }
//         else if (y > 700)
//         {
//             direction = 1;
//             outDir(1);
//         }
//         else if (x > 700)
//         {
//             direction = 2;
//             outDir(2);
//         }
//         else if (x < 300)
//         {
//             direction = 3;
//             outDir(3);
//         }
//         else
//         {
//             direction = 4;
//             outDir(4);
//         }
//         break;
//     }

//     default:

//         break;
//     }

//     return state;
// }

// int stepper_tick(int state)
// {
//     static int i = 0;

//     switch (state)
//     {
//     case init:

//     {
//         if (direction == 1 && curStep < 128)
//         {
//             state = cw;
//         }
//         else if (direction == 0 && curStep > -128)
//         {
//             state = ccw;
//         }
//         else
//         {
//             state = init;
//         }

//         break;
//     }

//     case cw:
//     {

//         if (direction == 1 && curStep < 128)
//         {
//             state = cw;
//         }
//         else if (direction == 0 && curStep > -128)
//         {
//             state = ccw;
//         }
//         else
//         {
//             state = init;
//         }

//         break;
//     }

//     case ccw:
//     {

//         if (direction == 0 && curStep > -128)
//         {
//             state = ccw;
//         }
//         else if (direction == 1 && curStep < 128)
//         {
//             state = cw;
//         }
//         else
//         {
//             state = init;
//         }

//         break;
//     }

//     default:
//         state = init;
//         curStep = 0;
//         break;
//     }

//     switch (state)
//     {

//     case cw:
//     {
//         PORTB = (PORTB & 0x03) | (phases[i] << 2);
//         i += 1;
//         if (i > 7)
//         {
//             i = 0;
//             curStep += 1;
//         }
//         break;
//     }

//     case ccw:
//     {
//         PORTB = (PORTB & 0x03) | (phases[i] << 2);
//         i--;
//         if (i < 0)
//         {
//             i = 7;
//             curStep -= 1;
//         }
//         break;
//     }

//     default:
//         break;
//     }

//     return state;
// }

// int led_tick(int state)
// {
//     // serial_println(curStep);
//     switch (state)
//     {
//     case none:
//     {
//         if (curStep <= -128)
//         {
//             state = left;
//         }
//         else if (curStep >= 128)
//         {
//             state = right;
//         }
//         else
//         {
//             state = none;
//         }
//         break;
//     }

//     case left:
//     {
//         if (curStep >= 128)
//         {
//             state = right;
//         }
//         else if (curStep > -128)
//         {
//             state = none;
//         }
//         else
//         {
//             state = left;
//         }
//         break;
//     }

//     case right:
//     {
//         if (curStep <= -128)
//         {
//             state = left;
//         }
//         else if (curStep < 128)
//         {
//             state = none;
//         }
//         else
//         {
//             state = right;
//         }
//         break;
//     }

//     default:
//     {
//         state = none;
//         break;
//     }
//     }

//     switch (state)
//     {
//     case left:
//     {
//         PORTC = SetBit(PORTC, PC1, 1);
//         PORTC = SetBit(PORTC, PC0, 0);
//         break;
//     }
//     case right:
//     {
//         PORTC = SetBit(PORTC, PC1, 0);
//         PORTC = SetBit(PORTC, PC0, 1);
//         break;
//     }
//     case none:
//     {
//         PORTC = SetBit(PORTC, PC1, 0);
//         PORTC = SetBit(PORTC, PC0, 0);
//         break;
//     }
//     default:
//         break;
//     }

//     return state;
// }
