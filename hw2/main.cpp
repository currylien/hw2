#include "mbed.h"
#include "uLCD_4DGL.h"

DigitalIn button1(D3);
DigitalIn button2(D6);
DigitalIn button3(D5);
InterruptIn sw3(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
uLCD_4DGL uLCD(D1, D0, D2);
AnalogOut Aout(D7);
AnalogIn Ain(A0);

Thread t;
int frequency[10] = {1,2,3,4,5,6,7,8,9,10};
int sample = 128;
float ADCdata[256];

void Print_data() {
    for (int i = 128; i < 256; i++){
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(10ms);
    }  
}


int main()
{ 
    int frequency_use = 1;
    
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("* %d Hz\n", frequency[0]);
    for (int i = 1; i < 10; i++) {
        uLCD.printf("  %d Hz\n", frequency[i]);
    }
    
    int i = 0;
    
    while (1) {
        if ((button1 == 1) && (i <= 8)) {
            i++;
            uLCD.locate(0,(i-1));
            uLCD.printf("  %d Hz", frequency[i - 1]);
            uLCD.locate(0,i);
            uLCD.printf("* %d Hz", frequency[i]);
            frequency_use = frequency[i];
        }
        if ((button3 == 1) && (i >= 1)) {
            i--;
            uLCD.locate(0, (i + 1));
            uLCD.printf("  %d Hz", frequency[i + 1]);
            uLCD.locate(0, i);
            uLCD.printf("* %d Hz", frequency[i]);
            frequency_use = frequency[i];
        }
        if (button2 == 1) break;
    }
    uLCD.locate(0, 10);
    uLCD.color(RED);
    uLCD.printf("The freqency :%d Hz", frequency_use);

    float gap1, gap2;
    gap1 = (1.0f  / (sample * 0.8f)) * frequency_use;
    gap2 = (1.0f / (sample * 0.2f)) * frequency_use;
    int k = 0;

    t.start(callback(&queue, &EventQueue::dispatch_forever));
    sw3.rise(queue.event(Print_data));
    while(1) {
        for (float j = 0; j <= 1; j = j + gap1) {
            Aout = j;
            if (k < 2 * sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
        for (float j = 1; j >= 0; j = j - gap2) {
            Aout = j;
            if (k < 2 * sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    }
}