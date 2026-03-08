// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 5  Exercise 1
// //    Exercise Description:
// //    I acknowledge all content contained herein, excluding template or example
// //    code, is my own original work.

// //    Demo Link: https://www.youtube.com/watch?v=275irPQjmOM

// #include "timerISR.h"
// #include "helper.h"
// #include "periph.h"
// // Exercise 1: 3 tasks
// #define NUM_TASKS 3

// typedef struct _task
// {
//     int state;                 // Task's current state
//     unsigned long period;      // Task period
//     unsigned long elapsedTime; // Time elapsed since last task tick
//     int (*TickFct)(int);       // Task tick function
// } task;

// const unsigned long TASK1_PERIOD = 1000; // 1000ms
// const unsigned long TASK2_PERIOD = 1;    // 1ms
// const unsigned long TASK3_PERIOD = 1;    // 10ms
// const unsigned long GCD_PERIOD = findGCD(TASK1_PERIOD, findGCD(TASK2_PERIOD, TASK3_PERIOD));
// int distance = 0;

// task tasks[NUM_TASKS];

// enum sonarStates
// {
//     listening,
//     waiting
// } sonarState;

// enum numStates
// {
//     single_digit,
//     double_digit,
//     full
// } numState;

// enum ledStates
// {
//     lit
// } ledState;

// int num_tick(int state)
// {
//     switch (state)
//     {
//     case single_digit:
//     {
//         if (distance >= 10)
//         {
//             state = double_digit;
//         }
//         else
//         {
//             state = single_digit;
//         }
//         break;
//     }
//     case double_digit:
//     {
//         if (distance < 10)
//         {
//             state = single_digit;
//         }
//         else if (distance >= 10 && distance <= 25)
//         {
//             state = double_digit;
//         }
//         else
//         {
//             state = full;
//         }
//         break;
//     }
//     case full:
//     {
//         if (distance < 25)
//         {
//             state = double_digit;
//         }
//         else
//         {
//             state = full;
//         }
//         break;
//     }
//     default:
//         break;
//     }
//     switch (state)
//     {
//     case single_digit:
//     {
//         DDRB &= ~((1 << PB5) | (1 << PB4) | (1 << PB3));
//         DDRB |= (1 << PB2);
//         outNum(distance);
//         break;
//     }
//     case double_digit:
//     {
//         int rhs = distance % 10;
//         int lhs = distance / 10;
//         static int toggle = 0;

//         if (toggle == 0)
//         {
//             DDRB &= ~(1 << PB3);
//             DDRB |= (1 << PB2);
//             outNum(rhs);
//         }
//         else
//         {
//             DDRB &= ~(1 << PB2);
//             DDRB |= (1 << PB3);
//             outNum(lhs);
//         }
//         toggle = !toggle;
//         break;
//     }
//     case full:
//     {
//         static int toggle = 0;
//         switch (toggle)
//         {
//         case 0:
//         {
//             DDRB |= (1 << PB5);
//             DDRB &= ~(1 << PB4 | (1 << PB3) | (1 << PB2));
//             outNum(15);
//             break;
//         }
//         case 1:
//         {
//             DDRB |= (1 << PB4);
//             DDRB &= ~(1 << PB5 | (1 << PB3) | (1 << PB2));
//             outNum(16);
//             break;
//         }
//         case 2:
//         {
//             DDRB |= (1 << PB3);
//             DDRB &= ~(1 << PB4 | (1 << PB5) | (1 << PB2));
//             outNum(17);
//             break;
//         }
//         case 3:
//         {
//             DDRB |= (1 << PB2);
//             DDRB &= ~(1 << PB5 | (1 << PB3) | (1 << PB4));
//             outNum(17);
//             break;
//         }
//         }
//         toggle = (toggle + 1) % 4;
//         break;
//     }
//     default:
//         break;
//     }
//     return state;
// }
// int sonar_tick(int state)
// {
//     switch (state)
//     {
//     case listening:
//         state = waiting;
//         break;

//     case waiting:
//         state = listening;
//         break;

//     default:
//         break;
//     }

//     switch (state)
//     {

//     case listening:
//     {
//         distance = int(sonar_read());
//         break;
//     }

//     default:
//         break;
//     }

//     return state;
// }

// int led_tick(int state)
// {
//     switch (state)
//     {
//     case lit:
//     {
//         state = lit;
//     }
//     default:
//         break;
//     }
//     switch (state)
//     {
//     case lit:
//     {
//         DDRC |= ((1 << PC3) | (1 << PC5));
//         static int counter = 0;
//         int pwm_period = 10;
//         int duty = 5;

//         if (counter < duty)
//         {
//             PORTC |= (1 << PC3);
//             PORTC |= (1 << PC5);
//         }
//         else
//         {
//             PORTC &= ~(1 << PC3);
//             PORTC &= ~(1 << PC5);
//         }
//         counter = (counter + 1) % pwm_period;
//     }

//     default:
//         break;
//     }
//     return state;
// }

// void TimerISR()
// {

//     // TODO: sample inputs here

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

//     ADC_init();   // initializes ADC
//     sonar_init(); // initializes sonar
//     DDRC |= (1 << DDC2);
//     DDRB = 0b00000110;
//     DDRD = 0b11111111;

//     // TODO: Initialize tasks here
//     //  e.g. tasks[0].period = TASK1_PERIOD
//     //  tasks[0].state = ...
//     //  tasks[0].elapsedTime = ...
//     //  tasks[0].TickFct = &task1_tick_function;

//     tasks[0].period = 1000;
//     tasks[0].state = listening;
//     tasks[0].elapsedTime = 0;
//     tasks[0].TickFct = &sonar_tick;

//     tasks[1].period = 1;
//     tasks[1].state = single_digit;
//     tasks[1].elapsedTime = 0;
//     tasks[1].TickFct = &num_tick;

//     tasks[2].period = 1;
//     tasks[2].state = lit;
//     tasks[2].elapsedTime = 0;
//     tasks[2].TickFct = &led_tick;

//     TimerSet(GCD_PERIOD);
//     TimerOn();

//     while (1)
//     {
//     }

//     return 0;
// }