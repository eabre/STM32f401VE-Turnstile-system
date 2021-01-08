The project has 3 states: start, pause and stop. LEDs for each situation (if the start button is pressed, the green led blinks,
if the pause button is pressed, the yellow led blinks, if the stop button is pressed, the red led blinks) works at 100ms intervals.
On the other hand, the number of passing people is collected in 5s and displayed on the LCD(16x2) and if the pause button is pressed,
the total number of people passing is displayed on the LCD.

Components of project; I used 3 external interrupts and 1 timer interrupt. And thanks to algorithm I developed, my delay function and
the static 100ms periodic square wave generator work separately at the same timer. I think that's why project is so efficient.
ps: In my project, I had to developed my own callback functions ,my own delay function due to proteus errors. Besides, since the
project is a simulation, I designed my own random signal generator to simulate human passings.

Those who want to run the project already in proteus can find the project's hex file and proteus file in the debug folder.

