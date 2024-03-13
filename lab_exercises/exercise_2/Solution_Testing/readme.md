1. Create N processes children 
2. User to send SIGUSR1, SIGUSR2, SIGTERM to father process and children processes
3. A process handles the corresponding "gate"
4. Parent takes as an input Number of gates, initial state as a string of "t" and "f" characters
5. Parent creates processes and prints initial states with pid 
6. Children Processes
7. every 15 seconds print their state with alarm and the time since their creation
8. When signal SIGUSR1 is received they print their state, (boolean ) and time since creeation
9. SIGUSR2 state gets flipped
10. SIGTERM , children processes get terminated
11. Children processes can get their signal from the parent process.
12. When the Parent Process
13. Gets SIGUSR1 , All children get SIGUSR1
14. Gets sigterm, >>
15. Checks if children are healthy, checks for SIGCHLD signal.
    If a child is killed, it waits and creates a new in its place
    if a child stopped, it makes it resume
    N child should always be active, one for every gate
16. Parent Process can get a signal from the terminal by the user

-Check for argc,argv correctness, print message
-Check for errors in syscalls
-Children code in seperate file named child.c
-Use Execv
-Makefile that produces the executables

A process can send a signal to an other process with the call kill(pid,signal)
kill(3247,sigcont) SIGCONT is sent to 3247

The command of sending a signal from the terminal is Kill -SIGNAL pid

Waitpid is used for the calling process changes of state in a child process and receive info
about the process' state that has changed