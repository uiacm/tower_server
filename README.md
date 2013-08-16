Tower Animator
=============

Tower Animator is a web server that runs on a Raspberry Pi connected to the Tower Lights LED driver board. The web page
that it serves has an audio player for the music and also communicates back to the server to play the video, allowing
remote control of the tower lights. This is part of the Tower Lights project put on the by the ACM student chapter 
at the University of Idaho. Tower Lights is a project to put a remotely controlled light in each window of a one side 
of 10 residence hall on campus. See [http://towerlights.uiacm.org/](http://towerlights.uiacm.org/) for more details. 

Note: A previous incarnation of software used to control the lights based on pc to pc communication is located at 
[https://code.google.com/p/toweranimator/](https://code.google.com/p/toweranimator/)

Requirements
------------

The server is designed to run a Raspberry Pi; the board revision shouldn't matter. It requires the 
[wiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/) and [Jansson](http://www.digip.org/jansson/) libraries
to be installed. The server also uses the [Mongoose](https://github.com/valenok/mongoose) embeddable web server. This is not
an external dependency, all necessary files are included in the repository. 

Hardware set up
---------------

Becase a total of three bytes of output are needed to interface with the LED driver boards, three 595 shift
registers are used to extend the Pi's output capability. The file driver.c has comments on how pins are connected
to the Pi's GPIO pins. A premade board made by Josh Armstrong should be available. 

Software Set up
---------------

Put all the animations inside a folder called 'animations' in the 'www' folder. Each animation must be in a sub folder,
with one .tan file and one .wav file:

> - www
>   + css
>   + js
>   - animations
>      - My Animation
>          + anim.tan
>          + music.wav
>      + A second animation

To start the server, run Make and run towerserver as root (required by wiringPi). This starts serving pages on the 8080
port. Navigate there (\<ip address\>:8080/ or localhost:8080/) and you will see the Tower Animator interface. Clicking the
refresh button will enumerate the animations list with all the animations stored in the animations folder. You can play
now play any animation you want. 
