xall:
	@echo Enter VAR name
VAR10:
	gcc keypad.c -o keypad_v10 -D VAR10 -lbcm2835
clean:
	rm -f *.o
stepmotor:
	gcc step_motor.c -o step_motor_Stepanova -lbcm2835
gp2y:
	gcc rangefinder_gp2y.c -o rangefinder_gp2y_Stepanova  -lwiringPi -lm -Wall
