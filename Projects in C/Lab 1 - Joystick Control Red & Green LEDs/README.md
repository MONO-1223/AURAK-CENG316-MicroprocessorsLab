ECE 271 Lab Report (Spring 2016)
=============

Name
-----------
```
Ethan Knowles
```

Lab Status
-------
completed with small "cool thing": each button press affects the LEDs differently: one toggles the red, one toggles the green, one toggles both,
one turns both on, and the last turns both off.

After-lab Questions
-------
The hardware debouncing circuit works by adding in a parallel capacitor to the push button, which absobs any rapid fluctuations in voltage
that may occur when the push button is pushed, resulting in a smoother voltage change.
One solution for software debouncing is to set up a piece of code that, when checking if a button has been pushed, checks multiple times
quickly and only passes on that the button has been pushed if it receives a positive result n times in a row.

Suggestions and Comments
-------

no comments of suggestions


Lab Credits
-------
Did you received any help from someone other than Prof. Zhu and lab teaching assistants?
My roommate Nikolaus Josephson suggested to add a while loop inside my if statements when trying to get the program to only count each button press once
