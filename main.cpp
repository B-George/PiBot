// Benjamin George & Cameron Carlson
// Seattle University
// ECEGR Junior Lab Robot Code
// 2015

#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LM 26		// Right motor test control ---------------- pin 32
#define RM 23		// Left motor test control ----------------- pin 33
#define R_PWM 26 	// Left motor speed control GPIO 26 PWM 0 -- pin 32
#define L_PWM 23 	// Right motor speed control GPIO 23 PWM 1 - pin 33
#define R_REV 27 	// Left motor direction control GPIO 27 ---- pin 36
#define L_REV 25  	// Right motor direction control GPIO 25 --- pin 37
#define RUN_SW 1  	// Run switch control ---------------------- pin 12
#define SD_SW 6  	// Shutdown switch control ----------------- pin 22
#define US_TRIG 0 	// Ultrasound trigger ---------------------- pin 11
#define US_ECHO_1 2	// Ultrasound echo 1 ----------------------- pin 13
#define US_ECHO_2 3 	// Ultrasound echo 2 ----------------------- pin 15
#define IR_SEND 4	// IR out ---------------------------------- pin 16
#define IR_REC 5	// IR in ----------------------------------- pin 18
#define LED_PIN 15	// LED tester ------------------------------ pin 08

#define SPEED 1000	// speed for our PWM
#define STOP 0		// speed == 0

#define REP(a, b)	for(int i = a; i < b; i++) // define for loop -- remove ;

#define ORIENT 0	// orientation phase
#define MOVING 1	// movement phase
#define ZFOUND 2	// confront zombie phase
#define ZKILL 3		// kill zombie phase

using namespace std;

// motor movement functions
void turn_left();
void turn_right();
void spin_left();
void spin_right();
void move_fwd();
void move_bwd();
void full_stop();
void move_fwd_direct();
void stop_direct();

// sets all pins as defined above
void setPins();

// ultrasonic sensor timing
void pulse_start_ISR(void);
void pulse_end_ISR(void);

// IR  fxn
void IRrec();
void IRtrn();

struct timeval start, end;
double intervals[2048];
int i = 0;
int count = 0;
int twiddle, pfds[2], report;
char parityt, parityr, bit, buf[2];
char letter = 0x42; // ASCII for letter 'B'



int main()
{
/*
pid_t parent pid, child_pid;
parent_pid = getpid();

child_pid = fork();
if(child_pid == 0) { // child pid -- go to transmit fxn
	child_pid = getpid();
	printf("Imma child, going to IRtrn");
	IRtrn();
}else{ // parent_pid
	while(1){
		
*/
// US sensor test code

	int num;
	FILE *fp;

	wiringPiSetup();
	setPins();
	wiringPiISR( US_ECHO_1, INT_EDGE_RISING, &pulse_start_ISR );
	wiringPiISR( US_ECHO_2, INT_EDGE_FALLING, &pulse_end_ISR );

	full_stop();
	delay(1);

	spin_left();
	
//	while(1)
	REP(0, 512)
	{
		delay(30);
		digitalWrite(US_TRIG, HIGH);
		usleep(10);
		digitalWrite(US_TRIG, LOW);	
	}
	delay(100);
	full_stop();

	fp = fopen("lab5data.bin", "wb");
	num = fwrite(intervals, 8, 512, fp);
	fclose(fp);
	printf("wrote %d intervals\n", num);
	REP(0,20)
	{
		printf("%f0\n", intervals[i]);
	} 

	cout << "program complete";

	

// test code for finding H(s)
/*
	wiringPiSetup();
	setPins();
	cout << "stop all for 10 sec\n\n";
	full_stop();
	delay(20000);
	digitalWrite(LED_PIN, HIGH);
	cout << "move fwd .5 sec\n\n";
	move_fwd();
	delay(500);
	digitalWrite(LED_PIN, LOW);
	full_stop();
	cout << "stop all\n\nend program";

*/

// motor function test code

/*	turn_left();
	std::cout << "turn left .5 sec\n";
	delay(500);
	std::cout << "wait .5 sec\n";
	full_stop();
	delay(500);
	turn_right();
	std::cout << "turn right .5 sec\n";
	delay(500);
	full_stop();
	std::cout << "wait .5 sec\n";
	delay(500);
*/


	return 0;	
 
}

void setPins()
{
		// outputs
		pinMode(L_PWM, PWM_OUTPUT);
		pinMode(R_PWM, PWM_OUTPUT);
		//pinMode(RM, OUTPUT);
		//pinMode(LM, OUTPUT);
		pinMode(L_REV, OUTPUT);
		pinMode(R_REV, OUTPUT);
		pinMode(IR_SEND, OUTPUT);
		pinMode(US_TRIG, OUTPUT);
		pinMode(LED_PIN, OUTPUT);
		// inputs
		pinMode(RUN_SW, INPUT);
		pinMode(SD_SW, INPUT);
		pinMode(US_ECHO_1, INPUT);
		pinMode(US_ECHO_2, INPUT);
		pinMode(IR_REC, INPUT);
		
//		pullUpDncontrol(2, PUD_UP);
	std::cout << "pins set\n\n";
}

void turn_left()
{
	pwmWrite(L_PWM, SPEED);
	pwmWrite(R_PWM, STOP);
	digitalWrite(L_REV, LOW); 
}

void turn_right()
{
	pwmWrite(L_PWM, STOP);
	pwmWrite(R_PWM, SPEED);
	digitalWrite(R_REV, LOW); 
}

void spin_left()
{
	pwmWrite(L_PWM, SPEED);
	pwmWrite(R_PWM, SPEED);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_REV, LOW);
}

void spin_right()
{
	pwmWrite(L_PWM, SPEED);
	pwmWrite(R_PWM, SPEED);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_REV, LOW);
}

void move_fwd()
{
	pwmWrite(L_PWM, SPEED);
	pwmWrite(R_PWM, SPEED);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_REV, LOW);
}

void move_bwd()
{
	pwmWrite(L_PWM, SPEED);
	pwmWrite(R_PWM, SPEED);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_REV, HIGH);
}

void full_stop()
{
	pwmWrite(L_PWM, STOP);
	pwmWrite(R_PWM, STOP); 
}

void move_fwd_direct()
{
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_REV, HIGH);
	digitalWrite(RM, HIGH);
	digitalWrite(LM, LOW);
}

void stop_direct()
{
	digitalWrite(RM, LOW);
	digitalWrite(LM, HIGH);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_REV, LOW);
}


void pulse_start_ISR(void)
{
	gettimeofday(&start, NULL);
	return;
}

void pulse_end_ISR(void)
{
	double t_start, t_end;

	gettimeofday(&end, NULL);
	t_start = start.tv_sec + (start.tv_usec/1000000.0);
	t_end =  end.tv_sec + (end.tv_usec/1000000.0);
	intervals[i] = t_end - t_start;
	return;
}

void IRrec()
{
//	pid_t parent_pid, child_pid;
//	parent_pid = getpid();
	
}

void IRtrn()
{

}
