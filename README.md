# The K9 RJ

## 1. Project Overview
- This project item is made of 2 parts: a HVAC controller panel and proximity touch system
- I was in charge of the entire software development of the proximity touch system, applications and the HVAC controller panel 

## 2. Development Environment
-  Software Platform developed internally(Adpated OS platform)
-  Compiler: IAR
-  mPD78F1845 16-bit Microprocessor | PIC16F18875 8-bit Microprocessor
-  Git, Jira, Script Tool

## 3. Challenging issues / Research and Solution / Result
This section demonstrates what was challenging issues, how to research them, and how to solve them

---
### Challenge #1 - Improving the performance of a touch system by devising software algorithms
When implementing a proximity function with a touch system, there were a lot of challenging issues. The technical design was fully changed from an infrared type to a touch system. In addition, Since the PCB (Printed Circuit Board) had limited physical space, it was difficult for our hardware team to alleviate the noise of touch signals by changing the PCB design. To solve this issue, I introduced new software algorithms, carefully addressing side effects, such as delayed response time, low touch sensitivity, functional problem in humid environments, and a lack of ROM size of the 8-bit microprocessor.

### Research and Solution #1
**(Step 1)** Implemented a combination of the filter algorithms (Kalman, Median) with C language. And integrated the combination into the software platform processing ADC (Analog Digital Convert) to reduce the intensity of the noise<br>
**(Step 2)** Divided the min-max range of filtered ADC signals into the three domains (Noise, Signal Rise, Signal Detection) and allocated three constants (Gain factors) to each domain in order to compensate for the delayed response time of the filtered signals<br>
**(Step 3)** Memorized each gain factor in EEPROM (Electrically Erasable Programmable Read-Only Memory)<br>
**(Step 4)** Tuned these gain factors by using the external tool and diagnostic-CAN (Controller Area Network) in order to finalize ideal gain factors improving side effects<br>
**(Step 5**) Devised the new algorithm that tracks the reference values (filtered ADC) at regular time interval and integrated it into the software module processing touch sensitivity, enhancing functional safety of the touch system<br>
**(Step 6)** Optimized the added software code to meet the limited ROM size (Read Only Memory)<br>

<p align="center">
<img src="./Img/RJ_Flow.jpg"><br>
<strong>Fig.1) the entire block diagram of the combination algorithm</strong>
<p>

**Fig.1)** shows the entire block diagram of the touch signal processing. 3 purple blocks with star shape are complemented to improve the performance of the proximity touch system (noise, response time, sensitivity). This combination algorithm (3 purple blocks) was mandatory, because the PCB was intolerant to noisy environment caused by signals with various frequencies and the proximity touch sensor was very small (Width: 57.4mm / Front Height: 3mm / Upper Height: 3.5mm). This tiny sensor had to recognize the 3D range (Width: 250mm, Height: 50mm, Depth: 10mm). Since our OEM did not approve changing our product's design to increase the physical area of a touch sensor, I had to overcome these limitations with software algorithms.

<p align="center">
<img src="./Img/RJ_Filter.jpg"><br>
<strong>Fig.2) Touch signal(ADC) behavior according to algorithms</strong>
<p>

**Fig.2)** shows how a touch signal(ADC) change after applying filter and PreEmphasis algorithm. In order to reduce the amount of fluctuation of the touch signal(ADC), I implemented the combination of IIR and Kalman filter with C language.

>void IIR_FILTER(Xtype* prox)<br>
{<br>
    //Calculation<br>
    //New = ((N-1)*Old + New)/N<br>
&nbsp;&nbsp;&nbsp;uint32_t u32CalTemp;<br>
&nbsp;&nbsp;&nbsp;u32CalTemp = u32Old * DENOMINATOR;//DENOMINATOR=8(changable)<br>
&nbsp;&nbsp;&nbsp;u32CalTemp -= u32Old;<br>
&nbsp;&nbsp;&nbsp;u32CalTemp += u32Rest;<br>
&nbsp;&nbsp;&nbsp;u32CalTemp += (uint32_t)Xtype->PresentADCValue;<br>
&nbsp;&nbsp;&nbsp;<br>
&nbsp;&nbsp;&nbsp;u32FilteredVal = u32CalTemp / DENOMINATOR;<br>
&nbsp;&nbsp;&nbsp;u32Rest = u32CalTemp % DENOMINATOR;<br>
&nbsp;&nbsp;&nbsp;Xtype->PresentADCValue = u32FilteredVal;<br>
&nbsp;&nbsp;&nbsp;u32Old = u32FilteredVal;<br>
}<br>

By changing the DENOMINATOR value, a user increases and decreases the intensity of IIR filter. In this program, u32Rest indicates the remainter after division. The remainter will be added in the next loop.
