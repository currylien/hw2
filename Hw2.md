**main.cpp**
```c
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
int frequency[10] = {1,2,3,4,5,6,7,8,9,10};           // Array to store frequency we use (1 ~ 10Hz)
int sample = 128;                                     // Sampling frequency
float ADCdata[256];                                   // Array to store ADC data

void Print_data() {                                   // function to print ADC data
    for (int i = 128; i < 256; i++) {
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(10ms);
    }
}

int main()
{ 
    int frequency_use = 1;
    
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("* %d Hz\n", frequency[0]);           // display select menu on uLCD
    for (int i = 1; i < 10; i++) {
        uLCD.printf("  %d Hz\n", frequency[i]);
    }
    
    int i = 0;
    
    while (1) {                                       // This is the select frequency mode
        if ((button1 == 1) && (i <= 8)) {             // control symbol '*' to move down 
            i++;
            uLCD.locate(0,(i-1));
            uLCD.printf("  %d Hz", frequency[i - 1]);
            uLCD.locate(0,i);
            uLCD.printf("* %d Hz", frequency[i]);
            frequency_use = frequency[i];             // take the frequency where '*' display
        }
        if ((button3 == 1) && (i >= 1)) {             // control symbol '*' to move up
            i--;
            uLCD.locate(0, (i + 1));
            uLCD.printf("  %d Hz", frequency[i + 1]);
            uLCD.locate(0, i);
            uLCD.printf("* %d Hz", frequency[i]);
            frequency_use = frequency[i];
        }
        if (button2 == 1) break;                      // if select button = 1, finish the select frequency mode
    }
    uLCD.locate(0, 10);
    uLCD.color(RED);
    uLCD.printf("The freqency :%d Hz", frequency_use); // display the frequency we choose

    float gap1, gap2;
    gap1 = (1.0f  / (sample * 0.8f)) * frequency_use;
    gap2 = (1.0f / (sample * 0.2f)) * frequency_use;
    int k = 0;

    t.start(callback(&queue, &EventQueue::dispatch_forever)); // Build an eventqueue for the multiple task 
    sw3.rise(queue.event(Print_data));                        // if user_button = 1, start to print data
    while(1) {                                                // wave generating mode
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
```
My cut off frequency is around 7Hz, it is true that the amplitude decrease as the frequency increase.  
And the wave measured by picoscope matches the wave plot by python.  
**FFT.py**
```python
import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 128.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,1,Ts) # signal vector; create Fs samples

n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # a vector of frequencies; two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

serdev = '/dev/ttyACM2'
s = serial.Serial(serdev)
for x in range(0, int(Fs)):
    line=s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'
    # print line
    y[x] = float(line)

Y = np.fft.fft(y)/n*2 # fft computing and normalization
Y = Y[range(int(n/2))] # remove the conjugate frequency parts

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,y)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Amplitude')
ax[1].plot(frq,abs(Y),'r') # plotting the spectrum
ax[1].set_xlabel('Freq (Hz)')
ax[1].set_ylabel('|Y(freq)|')
plt.show()
s.close()
```
