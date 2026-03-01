This repo is about ONLY the FreeRTOS Linux port that you can use right away, it doesn't include anything else. Take into account that it 
isn't real time, but you can try and learn about this OS as fast as possible. Happy tasking! 

DISCLAIMER: Neither I'm the owner nor I'm tightly involved in the FreeRTOS project, but I love it. I'm just packing it for 
my self of the future and your convenience and nothing more. Any questions about the kernel, the ports, the memory models and 
the libraries write them down in the official forums.


How to use it.

1 Clon or download the repo (copy and paste in a terminal):

git clone https://fjrg76@bitbucket.org/fjrg76/freertosv202107.00_linux_port_only.git

Or just choose "Download repository" (look for the 3 points in a row just at
the right of the Clone button) and uncompressed it wherever you want.


2. Navigate to the my_demo demo:

FreeRTOS/ -> Demo/ -> my_demo/


3. Open a terminal pointing to the project folder and build it ($ is the prompt, don't type it):

$ make


4. Then run it:

$ ./build/posix_demo 


5. Edit the contents of the file main_blink.c and enjoy it!

* Don't touch the file main.c, unless you know what you're doing.
* Whatever you want to do, do it inside the main_blink.c file or create your
  owns, but don't touch main.c (again).


6. To clean up the project type in the terminal:

$ make clean


7. To change the project's name, enter into the Makefile file, look for the
line that starts with BIN and write the new binary name after the := symbol,
for example:

BIN := my_awesome_first_FreeRTOS_project


8. You can find the official (and (more) complex) examples inside the folder
POSIX_GCC. The building process is the same.



