/*
 - You can "catch" a control C when it is "thrown" by a user typing "CTRL C".
 - The following code shows how it can be done
 - ECED 3403
 - 28 May 2023
*/


/************ Globals inside debugger ************/

#include <signal.h> /* Signal handling software */

/* 
 - volatile - do not cache or optimize, it can be changed elsewhere 
 - sig_atomic_t - shared variable - handle possible race conditions
*/

volatile sig_atomic_t ctrl_c_fnd; /* T|F - indicates whether ^C detected */


/************ Debugger support function ************/

void sigint_hdlr()
{
/*
- Invoked when SIGINT (control-C) is detected
- changes state of waiting_for_signal
- signal must be reinitialized
*/
ctrl_c_fnd = TRUE;
signal(SIGINT, (_crt_signal_t)sigint_hdlr); /* Reinitialize SIGINT */
}


/************ Debugger startup software ************/

/* Call signal() - bind sigint_hdlr to SIGINT */
ctrl_c_fnd = FALSE;
signal(SIGINT, (_crt_signal_t)sigint_hdlr);


/************ Before calling the CPU emulator *************/

void run_xm()
{
/* Run the CPU */
ctrl_c_fnd = FALSE;
cpu();
}

/************ Inside Instruction/Execution cycle software ************/

int cpu_go;
cpu_go = TRUE;


/*
while (cpu_go)
{
	fetch();
	decode();
	execute();
	/* Continue? */
	cpu_go = !ctrl_c_fnd && PC != brkpt && !single_step;
}
	
*/