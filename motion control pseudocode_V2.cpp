// Motion control pseudocode V2 (sinusoidal S-curve velocity profile)

//Definitions/declarations
double target_joint_vel = 0;																	// Joint velocity simVitro wants to command
double old_target_joint_vel = 0;																// Previously received joint velocity command
double current_joint_vel = 0;																	// Joint velocity that will actually be used to generate this position step
double old_joint_vel = 0;																		// Joint velocity that was last used to calculate a position step
long fri_packet_interval = 0.002; 																// Expected "clock ticks" timing of the controller sending/requesting a new packet (in actual code use KUKA API to get this number as SECONDS)

double current_position;
double delta_position; 																			// Change in joint position we want to command
double new_position;
double vel_gap = 0;																				// Gap between old and new velocity (NOTE: HAS SIGN!)
double gap_time;																				// Time to bridge vel_gap using max_acceleration assuming straight ramp
long intervals;																					// Number of packet cycles needed to bridge vel_gap
double packet_buffer;																			// Wherever new velocity commands are coming from
double max_acceleration = 90; 																	// Velocity slope in radians/s/s. The value of 90 radians/s^2 is derived from a linear fit to the max accel measured during step testing. (-9e-05 radians/ms^2)
double min_velocity = 0.000004*500; 	 														// Friction observer threshold (aka. velocity deadband)

double path_vector[] = NULL;																	// Array for storing sinusoidal velocity trajectory 
int path_vector_counter = 0;																	// Counter for keeping track of read position when reading the velocity trajectory array

command (){
	
	target_joint_vel = retrieve_latest_value(packet_buffer);									// Get latest velocity command from packet buffer (or wherever velocity commands come from)
	
	vel_gap = target_joint_vel - old_joint_vel;													// Figure out the difference between the latest velocity command and the old velocity
	
	if(target_joint_vel == old_joint_vel){														// If target is equal to old velocity, apply target velocity (remembering to reset stuff just in case)
		current_joint_vel = target_joint_vel;
		path_vector = {target_joint_vel};
		path_vector_counter = 0;
	}
	
	else{																						// If target is NOT equal to old velocity:
		
		if(target_joint_vel == old_target_joint_vel){											// Check if the target velocity has changed from before. If not, apply next velocity in the generated velocity trajectory array and increment the counter.
			current_joint_vel = path_vector[path_vector_counter];
			path_vector_counter ++;
		}
		
		else{																					// If the target velocity is a new target, check if difference between old velocity and new target exceeds max acceleration.
			if((abs(vel_gap))/fri_packet_interval) > max_acceleration){							// If so, generate sinusoidal velocity trajectory array to transition gracefully from old velocity to target velocity:
				
				gap_time = abs(vel_gap)/max_acceleration;										// Estimate transition time using trapezoidal trajectory
				adjusted_gap_time = (pi/2)*gap_time;											// Adjust transition time estimation by a factor of 2*pi (ratio between [slope of direct line between peaks of a given sinusoid] and [max slope of said sinusoid between peaks])
				intervals = ceil(adjusted_gap_time/fri_packet_interval);						// Break up transistion time into chunks by clock tick intervals
				path_vector[] = NULL;															// Clear out array just in case it gets filled by unexpected behavior
				path_vector_counter = 0;														// Reset path_vector_counter just in case
				
				for (i in 0:intervals){
					path_vector[i] = -(vel_gap/2)*cos((pi)*(i/intervals))+(0.5*vel_gap)+old_vel	// Generate a velocity for each clock tick in the transition using cosine and put in velocity trajectory array
				}
				
				current_joint_vel = path_vector[0];												// Apply the first velocity in the trajectory array
				path_vector_counter ++;															// Increment array read position counter
			}
			else{																				// If the target velocity is a new target but the difference between the old and target velocities are below the acceleration limit...
				current_joint_vel = target_joint_vel;											// Just set the current velocity right to the target velocity
				path_vector = {target_joint_vel};												// And set the velocity trajectory vector all to the target velocity 
				path_vector_counter = 0;														// Reset counter just in case
			}
		}
	}
	
	
	if(current_joint_vel != 0 & abs(current_joint_vel) < min_velocity){							// If the current velocity is under the friction observer threshold, consider it to be 0.
		current_joint_vel = 0;
	}
	
	delta_position = current_joint_vel * fri_packet_interval;									// Change in position is velocity multiplied by time
	new_position = current_position + delta_position;											// New position is desired change in position added to current position
	
	command_position(new_position);																// Send the new position command
	
	old_joint_vel = current_joint_vel;															// The used velocity is now old
	old_target_joint_vel = target_joint_vel;													// The used target velocity is now old
	

}