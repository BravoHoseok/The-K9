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

**Fig.2)** shows how a touch signal(ADC) change after applying filter and PreEmphasis algorithm. If the amount of fluctuation of a noise exceeds the range (Threshold On ~ Threhosld Off), the touch system is going to bring about several malfunctions. In order to reduce the amount of fluctuation of the noise, I implemented the combination of IIR and Kalman filter with C language.

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

u32Rest indicates the remainter after division. The remainter will be added in the next loop. The source code of Kalman FIlter for ADC can be found here: https://github.com/BravoHoseok/The-K9/blob/master/src/LOGIC_Kalman.c

In these filter algorithms, a user can increase and decreases the intensity of the combined filter by changing the **DENOMINATOR** value in IIR and **LOGIC__nenKALMAN_R_CONST**, **LOGIC__nenKALMAN_Q_CONST**, **LOGIC__nenKALMAN_I_CONST** in Kalman Filter. When applying this combination filter algorithm, the noise of the original touch signal is going to be alleviated. However, the entire response time of the filtered touch signal (ADC) is slowed. A user is going to feel that the sensitivity and response of this touch system is poor. This is a side effect of filter algorithms. But this side effect can be solved by applying 'PreEmphasis' algorithm.

To implement 'PreEmphasis' algorithm, you first need to define 3 region(Noise Region, Signal Rise, Valide Signal) by monitoring the fluctuation of the touch signal. And then, set a hysterisys range of three region as shown in **Fig.3**. 

<p align="center">
<img src="./Img/RJ_PreEmpha.jpg"><br>
<strong>Fig.2) Hysterisys Table</strong>
<p>

By inputting the touch sensitivity value as the value of the X-axis and utilizing an interpolation algorithm, you can find a state of three regions (0 or 1 or 2). Now, let's assign 'Gain factors' to each region as shown below pseudocode.

>if(state == 0)//Noise State<br>
&nbsp;&nbsp;&nbsp;{<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;G_factor=Const_X;<br>
&nbsp;&nbsp;&nbsp;}<br>
else if(state == 1)//Signal Rise State<br>
&nbsp;&nbsp;&nbsp;{<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;G_factor=Const_Y;<br>
&nbsp;&nbsp;&nbsp;}<br>
&nbsp;&nbsp;&nbsp;else//Valid Signal State<br>
&nbsp;&nbsp;&nbsp;{<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;G_factor=Const_Z;<br> 
&nbsp;&nbsp;&nbsp;}<br>

'Const_X', 'Const_Y', 'Const_Z' are constant value in C programming. You can define these values by monitoring increase and decrease patterns. However, I recommend defining these constant values based on the size of the sensitivity value. For example, if the sensitivity value is stored in int_16 variable, 
```sh
$ Const_X = (inputed sensitivity value) / 32767;
$ Const_Y = {[(inputed sensitivity value) / 3276] * [(inputed sensitivity value) / 3276)]} + 1;
$ Const_Z = (inputed sensitivity value) / 3276 + 1;
``` 

You can also customize the amount of sloops of the purple line in **fig.1)**  by adjusting these gain factors to specific values. I recommend tunning these values by storing them in EEPROM of a microprocessor, performing functional safety test in a laboratory with temperature chamber and electrical equipments.

<p align="center">
<img src="./Img/RJ_Ref1.jpg"><br>
<strong>Fig.3) ADC value and Reference Value 1</strong>
<p>

Fianlly, the last stage of this combination algorithm is implementing 'algoritm tracking the touch sensitivity value rapidly'. In the world of analog signal, the ADC value will change incessantly according to temperature range. It is the property of a microprocessor. As shown Fig.3), if we set a fixed threshold(black line) about the chaning ADC value to confirm whether an object is touched or not, it is going to make malfunctions in high temperature. To prevent this situation, we set reference value that tracks the ADC value periodically(1 or 2 sec or etc..) during no touch state. In here, a 'delta value' is defined as (ADC value) - (Reference value). By defining a delta value like this, we can check the sensitivity value of the touch sensor regardless of temperature range regularly. 

<p align="center">
<img src="./Img/RJ_Ref1.jpg"><br>
<strong>Fig.4) ADC value and Reference Value 2</strong>
<p>

**Fig.4)** shows the variance of the ADC value of a touch sensor according to temperature and humidity. We assume that the variance of the ADC value is getting more and more big according to temperature range, and the reference value tracks the ADC value in a fixed value (2 value / 1 sec). And then, the difference between the ADC value and the reference value is going to be getting big more and more, ultimately surpass a threshold value and causing malfunctions such as erroneously recognized object touch. Thus, whether how properly the reference value tracks the ADC value or not determines the performance of a touch system. To implement this tracking algorithm, I used the IIR_FILTER algorithm above mentioned, and designed an algorithm that changes DENOMINATOR constant value according to sloop rate of the ADC value during no touch state. This fast tracking algorithm provided more reliable functional safety function of the touch system.
