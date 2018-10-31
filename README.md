# kennel_fan_controller
This is a Linux application that controls a battery powered fan to cool a dog kennel.
The major framework is Qt. I'm using signals and slots to connect the hardware together.
The functional version is running on a pi-zero-w board.  I've also run most of the code on a pi-3 and on a beaglebone green wireles.
I still have to finish setting up the Avahi stuff to work with an Android App.

The hardware is a 4 pin PWM 200mm fan, a Sunfounder Rotary encoder module, a Sunfounder relay module to turn the fan 12 Volt supply on and off, a cpt DC/DC converter to take 12 Volt from the battery pack and supply 5 Volt USB power to the pi-zero-w, and a Lithium Polymer 11.1V 55C 5000mAh battery pack.

I still need to figure out a battery management system to go between the battery pack and the DC/DC converter.

