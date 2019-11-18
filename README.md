# Flap Talley

## Overview:
The software is planned to help support efforts to identify avian targets via radar and through analysis of the radar signal changes, identify the rate at which the avian target is flapping its wings.


This current version of the software was developed as a proof of concept to determine if a fast, non-embedded C++ program could:
1. Obtain radar data through a data acquisition or through simulation via hardware or software.
2. Identify target in the radar data stream
3. Calculate changes in amplitude of target to find that frequency.

## Evaluation:
While the system was not able to interconnect to the hardware yet, several key elements have been able to be tested in order to validate the feasibility of the effort.
1. Can a normal Windows 10 OS based laptop computer, the target identification and analysis classes are able to operate well within the desired data acquisition and analysis rate of 2-100 kHz? Yes. The whole software package can perform quit a bit faster than is needed.
2. Could a target be identified? Yes.
3. Can a change in the target be noticed and those values recorded? Yes
4. Can the cycle of amplitude changes be analyzed to determine the flap rate? Yes.

## Software Details:
Full software docs available in docs/html/index.html  found in the repo.

The software is built on C++ using the Qt C++ Library to facilitate a cross-platform GUI and to provide useful extensions to the standard C++ library. Everything has been compiled on a Windows 10 computer using Visual Studio 2017 and the GCC compiler. In keeping with the Qt paradigm a project file (.pro) could be easily built and would contain all the necessary project information needed to facilitate compiling within another package, by using the qmake capability. This includes movement of the software to another OS (e.g. Linux)
### Other needed libraries
* Qt 5.13+    C++ Cross platform GUI and language extension   https://www.qt.io/
* FFTW        C based high speed FFT calculation               http://www.fftw.org/

## Software Operation
Software begins by starting the GUI process and placing it on the stack, which then begins to cascade starting the other elements:
1. Thread Control class starts up
2. Thread Control class initiates four additional threads and begins monitoring activity on those threads
3. Starts FT_DAQ thread - data acquisition from radar or simulation
4. Simulation thread - Controls the simulated targets. Information is fed to FT_DAQ
5. Flapping thread - Sinusoidal flapping signal is fed to Simulation thread as needed.
6. FT_Calc thread starts - Used to asynchronously handle request for signal processing.

### The GUI
The GUI consists of control elements on the left and display elements to the right.<br/>
Left Side:<br/>
Button:Start - Creates a simulated radar signal including a small noise level. visible on Incoming Signal display<br/>
Button:Add Target - Creates a signal gaussian pulse target on radar trace.<br/>
Slider:Size - Adjusts the size of the target. Can be increased or decreased until the target cannot be identified.<br/>
Button:Flap target - Causes the amplitude of signal to vary based on amplitude.<br/>
Slider:Amplitude - Causes the range of the flapping to increase or decrease<br/>
Slider:Rate - Changes the frequency of the flapping<br/>
Button:Move Target - (Not Used Yet) Will casue random movement of target along radar signal, simulating moving closer or further from source.<br/>
button:Find Target - Causes program to examine radar signal and look for targets. targets are identified by red trace on Incoming Signal display<br/>
Sliders: Lag, Threshold, Influence - Fine tunes the Target Finding algorithm<br/>
Button:Flap Rate - Initiates analysis of a targets amplitude pulses and returns found frequency.<br/>

Right Side:<br/>
Incoming Signal - Real time, raw data singal coming into the software from the radar or as a simulated signal.<br/>
Flap Simulation - Real time view of the simulated flapping signal being applied to the simulated target.<br/>
Target Flap Signal - Data buffer showing current amplitude points identified from changes in the target signal.<br/>
<br/>
### Performance characteristics:
Typical operations show no memory leaks during run. 
Base system: Intel Xenon E2176M 2.7GHz, 32 Mb RAM
Expect ~30 Mb of memory usage.
Expect ~17% of CPU usage.
Results for simulating single flapping target with target identifier and flap rate calculator on.
NOTE: Code has not been compiled as optimized.


## Continuing Work
1. Fine tune and improve FFT calculation.
2. Ability to create multiple simulated targets and identify each with own Target class instantiation.
3. Integrate multiple targets into code so that all data is carried in that class between operational threads.
4. Integrate radar hardware interface and acquire real data.
5. Change GUI so that targets are identified by ID and flap rates directly on radar trace.
6. Improve interface design.



