Kibbie Dome Code
================

Things are a little messy until I can actually set up the system to test it. So here's the rundown:

"JordanLights.c" is the actual .c file that needs to be compiled the makefile won't do that it compiles "dynamicLights.c". "dynamicLights.c" will later be renamed to "dynamicLights.h" once the code is done and the main() will be removed out. It's currently like this because I needed to compile it's code to make sure everything works but I can't actually run "JordanLights.c" because I do not have all the hardware (light bars, controller boards ect.) so once it's time to run this code make sure you re-name dynamicLights.c to have a .h suffix.

Running code and show
---------------------

Once you have renamed the suffix as mentioned above you should make sure and compile JordanLights.c and to check for errors. Then upload it to whatever micro-controller we happen to be using at the time. dynamicLights.c is build to handle a zigbee network and make lightbars dynamic and let them enter and leave and does so based on what "route" the bar is assigned to once it initially enters which is assigned from a 64 bit permanent address assigned to it's hardware, and then addresses it from then on by it's route number.
