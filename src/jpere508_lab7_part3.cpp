// //    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
// //    Discussion Section: 021
// //    Assignment: Lab 7  Exercise 3
// //    Exercise Description:
// //    I acknowledge all content contained herein, excluding template or example
// //    code, is my own original work.

// //    Demo Link: https://youtu.be/ZhcHTXUHe0o

// #include "timerISR.h"
// #include "helper.h"
// #include "periph.h"
// // #include "serialATmega.h"

// #define NUM_TASKS 5 // TODO: Change to the number of tasks being used

// // Task struct for concurrent synchSMs implmentations
// // typedef struct _task
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
// const unsigned long TASK5_PERIOD = 100;
// const unsigned long GCD_PERIOD = 10;

// int x = 0;
// int y = 0;
// int direction = 0;
// // int rotation_speed = 10;
// // int curStep = 0;
// int servo_angle = 0;

// int cur_sound = 0;
// unsigned long idle_time = 0;

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

// enum buzzerStates
// {
//     off,
//     note1,
//     note2,
//     note3,
//     pause
// } buzzerState;

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
// int buzzer_tick(int state);

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

//     ICR1 = 39999;
//     OCR1A = 3000;

//     OCR0A = 255;
//     TCCR0A |= (1 << COM0A1);
//     TCCR0A |= (1 << WGM01) | (1 << WGM00);

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

//     tasks[4].period = TASK5_PERIOD;
//     tasks[4].state = off;
//     tasks[4].elapsedTime = 0;
//     tasks[4].TickFct = &buzzer_tick;

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
//     case init:
//     {
//         idle_time += tasks[1].period;
//         break;
//     }

//     case cw:
//     {
//         PORTB = (PORTB & 0x03) | (stages[i] << 2);
//         i += 1;
//         if (i > 7)
//         {
//             i = 0;
//             // curStep += 1;
//         }
//         idle_time = 0;
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
//         idle_time = 0;
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
//                 cur_sound = 2;
//             }
//         }
//         else if (!right_button && right_prev)
//         {
//             if (tasks[1].period > 5)
//             {
//                 tasks[1].period -= 5;
//                 tasks[1].elapsedTime = 0;
//                 // serial_println(tasks[1].period);
//                 cur_sound = 1;
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

// int buzzer_tick(int state)
// {
//     static unsigned long timer = 0;

//     if (idle_time > 10000)
//     {
//         if (cur_sound == 0)
//         {
//             timer += TASK5_PERIOD;
//             if (timer >= 5000)
//             {
//                 cur_sound = 3;
//                 timer = 0;
//             }
//         }
//     }
//     else
//     {
//         timer = 0;
//     }

//     switch (state)
//     {
//     case off:
//     {
//         if (cur_sound == 1 || cur_sound == 2 || cur_sound == 3)
//         {
//             state = note1;
//         }
//         break;
//     }

//     case note1:
//     {
//         state = note2;
//         break;
//     }

//     case note2:
//     {

//         if (cur_sound == 3)
//         {
//             state = note3;
//         }
//         else
//         {
//             state = off;
//             cur_sound = 0;
//         }

//         break;
//     }

//     case note3:
//     {
//         state = off;
//         cur_sound = 0;
//         break;
//     }

//     default:
//     {
//         state = off;
//         break;
//     }
//     }

//     switch (state)
//     {
//     case note1:
//     {
//         OCR0A = 128;
//         if (cur_sound == 1)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x02;
//         }
//         else if (cur_sound == 2)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x04;
//         }
//         else if (cur_sound == 3)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x03;
//         }
//         break;
//     }

//     case note2:
//     {
//         OCR0A = 128;
//         if (cur_sound == 1)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x04;
//         }
//         else if (cur_sound == 2)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x02;
//         }
//         else if (cur_sound == 3)
//         {
//             TCCR0B = (TCCR0B & 0xF8) | 0x04;
//         }
//         break;
//     }

//     case note3:
//     {
//         OCR0A = 128;
//         TCCR0B = (TCCR0B & 0xF8) | 0x02;
//         break;
//     }

//     default:
//     {
//         OCR0A = 255;
//         break;
//     }
//     }

//     return state;
// }
