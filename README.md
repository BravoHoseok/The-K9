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
**(Step 1)** Implemented a combination of the filter algorithms (Kalman, Median) with C language. And integrated the combination into the software platform processing ADC (Analog Digital Convert) to reduce the intensity of the noise
**(Step 2)** Divided the min-max range of filtered ADC signals into the three domains (Noise, Signal Rise, Signal Detection)
and allocated three constants (Gain factors) to each domain in order to compensate for the delayed response time of the
filtered signals
**(Step 3)** Memorized each gain factor in EEPROM (Electrically Erasable Programmable Read-Only Memory)
**(Step 4)** Tuned these gain factors by using the external tool and diagnostic-CAN (Controller Area Network) in order to finalize ideal gain factors improving side effects
**(Step 5**) Devised the new algorithm that tracks the reference values (filtered ADC) at regular time interval and integrated it into the software module processing touch sensitivity, enhancing functional safety of the touch system
**(Step 6)** Optimized the added software code to meet the limited ROM size (Read Only Memory)

<p align="center">
<img src="./Img/RJ_Flow.jpg"><br>
<strong>Fig.1) the design pattern of slide(wheel touch sensors</strong>
<p>

**Fig.1)** shows flow chart of the combination algorithm. 
