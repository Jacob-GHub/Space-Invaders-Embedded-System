// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 7  Exercise 2
// //    Exercise Description:
// //    I acknowledge all content contained herein, excluding template or example
// //    code, is my own original work.

// //    Demo Link: https://youtu.be/uldZP3_OOKI

// #include "timerISR.h"
// #include "helper.h"
// #include "periph.h"
// // #include "serialATmega.h"

// #define NUM_TASKS 4 // TODO: Change to the number of tasks being used

// // Task struct for concurrent synchSMs implmentations
// typedef struct _task
// {
//     signed char state;         // Task's current state
//     unsigned long period;      // Task period
//     unsigned long elapsedTime; // Time elapsed since last task tick
//     int (*TickFct)(int);       // Task tick function
// } task;

// // TODO: Define Periods for each task
// const unsigned long TASK1_PERIOD = 100;
// const unsigned long TASK2_PERIOD = 10;
// const unsigned long TASK3_PERIOD = 100;
// const unsigned long TASK4_PERIOD = 100;
// const unsigned long GCD_PERIOD = 10;

// int x = 0;
// int y = 0;
// int direction = 0;
// // int rotation_speed = 10;
// // int curStep = 0;
// int servo_angle = 0;

// task tasks[NUM_TASKS]; // declared task array with 5 tasks

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
//     light_leds
// } ledState;

// enum buttonStates
// {
//     release,
//     press
// } buttonState;

// void TimerISR()
// {
//     for (unsigned int i = 0; i < NUM_TASKS; i++)
//     { // Iterate through each task in the task array
//         if (tasks[i].elapsedTime >= tasks[i].period)
//         {                                                      // Check if the task is ready to tick
//             tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
//             tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
//         }
//         tasks[i].elapsedTime += GCD_PERIOD; // Increment the elapsed time by GCD_PERIOD
//     }
// }

// int stages[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001}; // Stepper motor phases

// // TODO: Create your tick functions for each task
// int joystick_tick(int state);
// int stepper_tick(int state);
// int led_tick(int state);
// int button_tick(int state);

// int main(void)
// {
//     DDRB = 0b11111111;
//     PORTB = 0;

//     DDRD = 0b11111111;
//     PORTD = 0;

//     DDRC = 0b00000000;
//     PORTC = 0b11111111;

//     TCCR1A |= (1 << WGM11) | (1 << COM1A1);              // COM1A1 sets it to channel A
//     TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // CS11 sets the prescaler to be 8
//     // WGM11, WGM12, WGM13 set timer to fast pwm mode

//     ICR1 = 39999; // 20ms pwm period

//     OCR1A = 3000;

//     ADC_init(); // initializes ADC
//     // serial_init(9600);

//     // TODO: Initialize the buzzer timer/pwm(timer0)

//     // TODO: Initialize the servo timer/pwm(timer1)

//     // TODO: Initialize tasks here
//     //  e.g.
//     //  tasks[0].period = ;
//     //  tasks[0].state = ;
//     //  tasks[0].elapsedTime = ;
//     //  tasks[0].TickFct = ;

//     tasks[0]
//         .period = TASK1_PERIOD;
//     tasks[0].state = read;
//     tasks[0].elapsedTime = 0;
//     tasks[0].TickFct = &joystick_tick;

//     tasks[1].period = TASK2_PERIOD;
//     tasks[1].state = init;
//     tasks[1].elapsedTime = 0;
//     tasks[1].TickFct = &stepper_tick;

//     tasks[2].period = TASK3_PERIOD;
//     tasks[2].state = light_leds;
//     tasks[2].elapsedTime = 0;
//     tasks[2].TickFct = &led_tick;

//     tasks[3].period = TASK4_PERIOD;
//     tasks[3].state = release;
//     tasks[3].elapsedTime = 0;
//     tasks[3].TickFct = &button_tick;

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
//         x = ADC_read(0);
//         y = ADC_read(1);

//         if (x < 300)
//         {
//             direction = 2;
//         }
//         else if (x > 700)
//         {
//             direction = 1;
//         }
//         else
//         {
//             direction = 0;
//         }

//         if (y < 300)
//         {
//             servo_angle = map_value(0, 300, -90, -45, y);
//             OCR1A = map_value(0, 300, 999, 1750, y);
//         }
//         else if (y < 700)
//         {
//             servo_angle = map_value(300, 700, -45, 45, y);
//             OCR1A = map_value(300, 700, 1750, 3250, y);
//         }
//         else
//         {
//             servo_angle = map_value(700, 1023, 45, 90, y);
//             OCR1A = map_value(700, 1023, 3250, 4999, y);
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
//     case ccw:
//     case cw:
//     {

//         if (direction == 1)
//         {
//             state = cw;
//         }
//         else if (direction == 2)
//         {
//             state = ccw;
//         }
//         else
//         {
//             state = init;
//         }
//         break;
//     }

//     default:
//     {
//         state = init;
//         break;
//     }
//     }

//     switch (state)
//     {

//     case cw:
//     {
//         PORTB = (PORTB & 0x03) | (stages[i] << 2);
//         i += 1;
//         if (i > 7)
//         {
//             i = 0;
//             // curStep += 1;
//         }
//         break;
//     }

//     case ccw:
//     {
//         PORTB = (PORTB & 0x03) | (stages[i] << 2);
//         i--;
//         if (i < 0)
//         {
//             i = 7;
//             // curStep -= 1;
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
//     switch (state)
//     {
//     case light_leds:
//     {
//         PORTD &= 0b01000011;
//         PORTB &= 0b11111110;

//         if (direction == 1)
//         {
//             PORTD = SetBit(PORTD, PD4, 1);
//             PORTD = SetBit(PORTD, PD3, 0);
//             PORTD = SetBit(PORTD, PD2, 0);
//         }
//         else if (direction == 2)
//         {
//             PORTD = SetBit(PORTD, PD4, 0);
//             PORTD = SetBit(PORTD, PD3, 0);
//             PORTD = SetBit(PORTD, PD2, 1);
//         }
//         else
//         {
//             PORTD = SetBit(PORTD, PD4, 0);
//             PORTD = SetBit(PORTD, PD3, 1);
//             PORTD = SetBit(PORTD, PD2, 0);
//         }

//         if (servo_angle >= 45 && servo_angle <= 90)
//         {
//             PORTD = SetBit(PORTD, PD5, 1);
//             PORTB = SetBit(PORTB, PB0, 0);
//             PORTD = SetBit(PORTD, PD7, 0);
//         }
//         else if (servo_angle >= -45 && servo_angle < 45)
//         {
//             PORTD = SetBit(PORTD, PD5, 0);
//             PORTB = SetBit(PORTB, PB0, 0);
//             PORTD = SetBit(PORTD, PD7, 1);
//         }
//         else if (servo_angle >= -90 && servo_angle < -45)
//         {
//             PORTD = SetBit(PORTD, PD5, 0);
//             PORTB = SetBit(PORTB, PB0, 1);
//             PORTD = SetBit(PORTD, PD7, 0);
//         }

//         break;
//     }
//     default:
//         break;
//     }
//     return state;
// }

// int button_tick(int state)
// {
//     int left_button = GetBit(PINC, PC3);
//     int right_button = GetBit(PINC, PC4);

//     static int left_prev = 1;
//     static int right_prev = 1;

//     // static int first_tick = 1;
//     // if (first_tick)
//     // {
//     //     left_prev = left_button;
//     //     right_prev = right_button;
//     //     first_tick = 0;
//     //     return state;
//     // }

//     switch (state)
//     {
//     case release:
//     {
//         if ((!left_button && left_prev) || (!right_button && right_prev))
//         {
//             state = press;
//         }
//         break;
//     }

//     case press:
//     {
//         if (left_button && right_button)
//         {
//             state = release;
//         }
//         break;
//     }

//     default:
//     {
//         state = release;
//         break;
//     }
//     }
//     switch (state)
//     {
//     case press:
//     {
//         if (!left_button && left_prev)
//         {
//             if (tasks[1].period < 30)
//             {
//                 tasks[1].period += 5;
//                 tasks[1].elapsedTime = 0;
//                 // serial_println(tasks[1].period);
//             }
//         }
//         else if (!right_button && right_prev)
//         {
//             if (tasks[1].period > 5)
//             {
//                 tasks[1].period -= 5;
//                 tasks[1].elapsedTime = 0;
//                 // serial_println(tasks[1].period);
//             }
//         }
//         break;
//     }
//     default:
//         break;
//     }

//     left_prev = left_button;
//     right_prev = right_button;

//     return state;
// }