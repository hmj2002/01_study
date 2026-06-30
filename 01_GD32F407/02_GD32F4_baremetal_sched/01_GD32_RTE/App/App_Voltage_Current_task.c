#include "App.h"

void App_Voltage_task() {
	float vol=Voltage_get();
	printf("Vol=%.2f\n",vol);

}

void App_Current_task() {
	float cur=Current_get();
	printf("cur=%.2f\n",cur);
}