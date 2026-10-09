#include "pid.h"

void PID_Init(PIDControls *pid, float p, float i, float d, float iLim, float sLim){
	pid->kp = p;
	pid->ki = i;
	pid->kd = d;

	pid->itg = 0.0f;
	pid->prevErr = 0.0f;

	pid->iLimit = iLim;
	pid->servoLimit = sLim;

}

void PID_Compute(PIDControls *pid, float desired, float actual, float dt){
	float err = desired - actual;

	//Bierzemy proporcje do błędu (P)
	float p_out = pid->kp * error;

	//Sumujemy błąd w czasie (I)
	pid->itg += error * dt;

	//Blokada przed zbytnim wychyleniem itg
	if(pid->itg > pid->iLimit) pid->itg = pid->iLimit;
	else if(pid -> itg < -pid -> ilimit) pid -> itg = pid -> iLimit;

	//Proporcjowanie itg
	float i_out = pid-> itg * pid->ki;

	//Zmiana błedu w czasie
	float dtv = (err - pid -> prevErr)/dt;
	float d_out = dtv * pid->kd;

	pid -> prevErr = err;

	float total_out = p_out + i_out + d_out;

	//Blokada przed zbytnim wychyleniem SERWA
	if(total_out > pid->serwoLimit) total_out = pid->serwoLimit;
	else if(total_out < -pid -> serwoLimit) total_out= pid -> serwoLimit;

	return total_out;

}
