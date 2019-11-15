// Motion control pseudocode (simple ramp acceleration)

//Definitions/declarations
double target_joint_vel = 0;																	// Joint velocity simVitro wants to command
double current_joint_vel = 0;																	// Joint velocity that will actually be used to generate this position step
double old_joint_vel = 0;																		// Joint velocity that was last used to calculate a position step
long fri_packet_interval = 2; 																	// Consider replacing in this script with timestamp subtraction for a more accurate timebase ("clock tick")

double current_position;
double delta_position; 																			// Change in joint position we want to command
double new_position;

double packet_buffer;																			// Wherever new velocity commands are coming from
double max_acceleration = 90; 																	// Velocity slope in radians/s/s. The value of 90 radians/s^2 is derived from a linear fit to the max accel measured during step testing. (-9e-05 radians/ms^2)
double min_velocity = 0.000004*500; 	 														// Friction observer threshold (aka. velocity deadband) in radians/s

int sign; //for keeping track of velocity direction. -1 if velocity is negative, 1 if velocity is positive

command (){
	
	target_joint_vel = retrieve_latest_value(packet_buffer);									// Get latest velocity command from packet buffer (or wherever velocity commands come from)
	
	if(target_joint_vel != old_joint_vel & abs((target_joint_vel - old_joint_vel)/fri_packet_interval)> max_acceleration){	//If we have a new velocity and the potential transition from the old to new velocity is greater than the max allowed accel...
		
		sign = signbit(target_joint_vel - old_joint_vel);										// Get sign of velocity change (with signbit, where 1 is returned if negative and 0 is returned if positive)
		sign = (-2*(sign))+1;																	// Convert 1 negative 0 positive to -1 negative +1 positive
		current_joint_vel = old_joint_vel + (sign * acceleration * fri_packet_interval);			// New velocity is old velocity plus the maximum allowed change in velocity (accel)
		
	}
	else{																						// If the velocity isn't new or if the change in velocity is under the permitted accel, apply the new velocity
		current_joint_vel = target_joint_vel;
	}
	
	if(current_joint_vel != 0 & abs(current_joint_vel) < min_velocity){								// If the current velocity is under the friction observer threshold, consider it to be 0.
		
		current_joint_vel = 0;
		
	}
	
	delta_position = current_joint_vel * fri_packet_interval;									// Change in position is velocity multiplied by time
	new_position = current_position + delta_position;											// New position is desired change in position added to current position
	
	
	command_position(new_position);																// Send the new position command
	old_joint_vel = current_joint_vel;															// The used velocity is now old
}