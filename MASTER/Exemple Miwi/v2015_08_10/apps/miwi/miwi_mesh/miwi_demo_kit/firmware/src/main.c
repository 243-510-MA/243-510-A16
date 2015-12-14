//MAIN

#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "network.h"
#include "pan.h"
#include "student.h"
#include "teacher.h"
#include "door_unlock.h"
#include "projector_screen.h"


#define PAN 0                       //Controls the projector, red light, buzzer and motion sensor (such many thing, wow)
#define DOOR_UNLOCK 1               //Unlock the door (duh)
#define PROJECTOR_SCREEN 2          //DROP DA PROJECTOR
#define TEACHER 3                   //Mista ze teacha
#define STUDENT 4                   //You little shit

//*************************************************************************

#define DEVICEMODE PAN // Choisir ici

//*************************************************************************

void main(void)
{
    Network(DEVICEMODE);
    
    LED0 = LED1 = LED2 = 0;
    

    
    if(DEVICEMODE == PAN)
    {
        Pan();     
    }   


    if(DEVICEMODE == DOOR_UNLOCK)
    {
        DoorUnlock();
    }


    if(DEVICEMODE == PROJECTOR_SCREEN)
    {
        ProjectorScreen();
    }


    if(DEVICEMODE == TEACHER)
    {
        Teacher();
    }


    if(DEVICEMODE == STUDENT)
    {
        Student();
    }
}
