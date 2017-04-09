//Code for electronic chimes playing stand

#include <MIDI.h>

//Variable initialization

int note_vector[] = {used for associating inputs with midi notes};
boolean flag_vector_one[] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
boolean flag_vector_two[] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);

int pin_list[] = {whatever the rest of the input pins are};

void setup(){
	
	MIDI.begin(1);
	Serial.begin(115200);
	
	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	
	for (int pin = 0; pin<21; pin++){
		pinMode(pin_list[pin], INPUT);
	}
	
	
	
	
	
}


void loop(){
	
	//Initial pin clearing
	digitalWrite(1,LOW);
	digitalWrite(2,LOW);
	
	//First switch scan (output pin 1)
	digitalWrite(1,HIGH);
	for (int pin = 0; pin<21; pin++){
		if(digitalRead(pin_list[pin])==HIGH&flag_vector_one[pin]==FALSE){
			time_vector[pin] = millis();
			flag_vector_one[pin] = TRUE;
		}
		if(digitalRead(pin_list[pin])==LOW){
			flag_vector_one[pin] = FALSE;
		}
	}
	
	digitalWrite (1, LOW):
	
	//Second switch scan (output pin 2)
	digitalWrite(2, HIGH);
	for (int pin = 0; pin<21; pin ++){
		if(digitalRead(pin_list[pin])==HIGH&flag_vector_two[pin]==FALSE){
			velocity_vector[pin] = millis() - time_vector[pin];
			MIDI.sendNoteON(pin, insert map function here, 1);//midi output using note_vector[pin] for note and velocity_vector[pin] and map() for velocity
			flag_vector_two[pin] = TRUE;//some sort of flagging stuff here for anti-bounce
		}
		if(digitalRead(pin_list[pin])==LOW){
			flag_vector_two[pin] = FALSE;
		}
		
	}
	
	
}