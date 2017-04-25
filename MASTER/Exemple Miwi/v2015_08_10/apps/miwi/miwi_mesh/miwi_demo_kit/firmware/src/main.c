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
#include "computer_control.h"
#include "demo_pan.h"
#include "demo_mouvement.h"
#include "demo_911.h"



#define PAN 0                       //Controls the projector, red light, buzzer and motion sensor (such many thing, wow)
#define DOOR_UNLOCK 1               //Unlock the door (duh)
#define PROJECTOR_SCREEN 2          //DROP DA PROJECTOR
#define TEACHER 3                   //Mista ze teacha
#define STUDENT 4                   //You little shit
#define COMPUTER_CONTROL 5          //Boop Beep Boop
#define MOVEMENT 6                  //Beep Boop Boop
#define DEMO_PAN 7
#define DEMO_911 8                  //Can jet fuel melt steel beams?

//*************************************************************************

#define DEVICEMODE STUDENT
// Choisir ici

//*************************************************************************

//bool presence = true;
 
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
    
    if(DEVICEMODE == COMPUTER_CONTROL)
    {
        ComputerControl();
    }
    
    if(DEVICEMODE == MOVEMENT)
    {
        MovementDetector();
    }
    
    if(DEVICEMODE == DEMO_PAN)
    {
       Demo_Pan();      
    }
    
    if(DEVICEMODE == DEMO_911)
    {
       Demo_911();
        
    }
}
