#include "mbed.h"
#include "uLCD_4DGL.h"
DigitalIn button1(D3);
DigitalIn button2(D6);
DigitalIn button3(D5);
uLCD_4DGL uLCD(D1, D0, D2);
AnalogOut Aout(PA_4);
AnalogIn Ain(A0);
int frequency[10] = {1,2,3,4,5,6,7,8,9,10};
int sample = 128;
float ADCdata[128];

int main()
{ 
    int frequency_use = 1;
    uLCD.cls();
    uLCD.printf("* %d Hz\n", frequency[0]);
    for (int i = 1; i <10; i++) {
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
            uLCD.locate(0,(i+1));
            uLCD.printf("  %d Hz", frequency[i + 1]);
            uLCD.locate(0,i);
            uLCD.printf("* %d Hz", frequency[i]);
            frequency_use = frequency[i];
        }
        if (button2 == 1) break;
    }
    uLCD.locate(0, 10);
    uLCD.color(RED);
    uLCD.printf("The freqency :%d Hz", frequency_use);
    float period;
    float gap1, gap2;
    period = 1.0f / 1;
    gap1 = (1.0f  / (sample * 0.8f*period));
    gap2 = (1.0f / (sample * 0.2f*period));
    int k = 0;
    while(1) {
        
        for (float j = 0; j <= 1; j = j + gap1) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
        for (float j = 1; j >= 0; j = j - gap2) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    }
}