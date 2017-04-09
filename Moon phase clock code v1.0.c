#include <Time.h>
#include <TimeLib.h>

//Make sure clock movement minute hand is at the new moon position before applying power to the system

//Variable initialization
int phase = 0;
float jy;
float jm;
int Year;
int Month;
int Day;
int current_location;
long delay_time;
long jul;
long movement_time;
long wasted_time;
long time_now;
float ja;
float n;
float RAD;
float t;
float t2;
float as;
float am;
float xtra;
float i;
float j1;
float jd;


int output_pin = 13; //Relay control output pin


void setup(){
	pinMode(output_pin, OUTPUT);
	digitalWrite(output_pin, LOW); //Assuming relay requires HIGH to turn on. If not, make sure to reverse stuff below too!
	Serial.begin(9600);
	setTime(19,00,00,27,02,2017);  //Set current time in hr,min,sec,Day,mo,yr format
	Serial.println("Lunar Clock Program Started");
}



void loop(){
	
	time_now = now(); //Get the time now
	Day = day(time_now);
	Month = month(time_now);
	Year = year(time_now);
	
	phase = Trig2(Year,Month,Day); // What is toDay's phase? (int 0-29)
	
	// Take deep breath after tedious floating point trig
	
	// Figure out how long to move hand to get to the right phase, based on where it is right now
	if (phase > current_location){ //If desired phase is in front of current location
		movement_time = (phase - current_location)*2*60*1000L;
	}
	if (phase < current_location){ //If desired phase is behind current location
		movement_time = ((30-current_location)+phase)*2*60*1000L;
	}
	if (phase == current_location){ //If current location is same as desired phase
		movement_time = 0;
	}
	
	Serial.print("Phase: "); //Output variables to serial connection for debugging
	Serial.println(phase);
	Serial.print("Date: ");
	Serial.print(month(time_now));
	Serial.print("/");
	Serial.print(day(time_now));
	Serial.print("/");
	Serial.println(year(time_now));
	Serial.print("Time to move: ");
	Serial.print(movement_time/60000);
	Serial.println(" minutes");
	Serial.println("Movement Begun");
	
	digitalWrite(output_pin, HIGH); //Time to move!
	delay(movement_time);
	digitalWrite(output_pin, LOW); //Stop
	
	Serial.print("Movement Finished");
	
	current_location = phase; //Reset current location memory
	
	wasted_time = (now() - time_now)*1000L; //Take time spent doing floating point math and moving the hand into account (milsecs)
	
	delay_time = 86400000L-(wasted_time); //Really really try to stay on time please xD
	delay(delay_time); //delay 24 hours (minus the movement running time and program running time) before running the program again
}




long julday(int Year, int Month, int Day){//Calculate julian Day number with horrid floating point math
	jy = Year;
	jm = Month+1;
  
	if (Month <= 2) {
		jy = jy-1;	
		jm = jm + 12;	
	} 
  
	jul = floor(365.25 *jy) + floor(30.6001 * jm) + (Day) + 1720995L;
	
	if (Day+31*(Month+12*Year) >= (15+31*(10+12*1582))) {
		ja = floor(0.01 * jy);
		jul = jul + 2 - ja + floor(0.25 * ja);
	}
	
	return(jul);
}


int Trig2(int Year,int Month,int Day){ //Calculate moon phase with more horrid floating point math
	n = floor(12.37 * (Year -1900 + ((1.0 * Month - 0.5)/12.0)));
	RAD = 3.14159/180.0;
	t = n / 1236.85;
	t2 = t * t;
	as = 359.224 + 29.10536 * n;
	am = 306.0253 + 385.817 * n + 0.010730 * t2;
	xtra = 0.75933 + 1.530589 * n + ((1.178e-4) - (1.55e-7) * t) * t2;
	xtra = xtra + ((0.1734 - 3.93e-4 * t) * sin(RAD * as) - 0.4068 * sin(RAD * am));

	if(xtra > 0.0) {
		i= floor(xtra);
	}
	else {
		ceil(xtra - 1.0);
	}
  
	j1 = julday(Year,Month,Day);
	jd = (2415020L + 28 * n) + i;
	return(((int)(j1-jd) + 30)%30);
}


