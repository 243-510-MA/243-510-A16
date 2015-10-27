/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_colors.h

  Summary:
    The header file contains default color definitions for each color depth.

  Description:
    This header file provides default color definitions as well as color
    conversion macros. Application may or may not use this file. Application
    may replace this file with new color definitions that fits the application
    needs.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END


#ifndef _GFX_COLORS_H
// DOM-IGNORE-BEGIN
    #define _GFX_COLORS_H
// DOM-IGNORE-END

/*********************************************************************
* Section: Includes
*********************************************************************/
#include <stdint.h>
#include "system_config.h"

/*********************************************************************
* Overview: Basic colors definitions.
*           The basic colors defined in this section are defined
*           for basic demo requirements. End application can define additional
*           colors or override existing default colors. If overriding an
*           existing default color, define the new color value before
*           the #include of Graphics.h to avoid the compile time warning.
*           When using palette, a different file 'PaletteColorDefines.h' has
*           to be used instead.
*********************************************************************/
#ifdef GFX_CONFIG_PALETTE_DISABLE
	
	#if (GFX_CONFIG_COLOR_DEPTH == 1)

		#ifndef BLACK
			#define BLACK	0   
		#endif
		#ifndef WHITE
			#define WHITE	1    
		#endif       	    
			
	#elif (GFX_CONFIG_COLOR_DEPTH == 4)

		#ifndef BLACK
			#define BLACK   0
		#endif
		#ifndef GRAY000
			#define GRAY000 0
		#endif
		#ifndef GRAY001
			#define GRAY001 1
		#endif
		#ifndef GRAY002
			#define GRAY002 2
		#endif
		#ifndef GRAY003
			#define GRAY003 3 
		#endif
		#ifndef GRAY004
			#define GRAY004 4 
		#endif
		#ifndef GRAY005
			#define GRAY005 5 
		#endif
		#ifndef GRAY006
			#define GRAY006 6
		#endif
		#ifndef GRAY007
			#define GRAY007 7
		#endif
		#ifndef GRAY008
			#define GRAY008 8 
		#endif
		#ifndef GRAY009
			#define GRAY009 9 
		#endif
		#ifndef GRAY010
			#define GRAY010 10 
		#endif
		#ifndef GRAY011
			#define GRAY011 11 
		#endif
		#ifndef GRAY012
			#define GRAY012 12 
		#endif
		#ifndef GRAY013
			#define GRAY013 13 
		#endif
		#ifndef GRAY014
			#define GRAY014 14 
		#endif
		#ifndef GRAY015
			#define GRAY015 15 
		#endif
		#ifndef WHITE
			#define WHITE   15
		#endif

		#ifndef LIGHTGRAY
			#define LIGHTGRAY      GRAY012
		#endif	
		#ifndef DARKGRAY
			#define DARKGRAY        GRAY004
		#endif	
		
	#elif (GFX_CONFIG_COLOR_DEPTH == 8) || (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)

		#ifndef BLACK
			#define BLACK           GFX_RGBConvert(0, 0, 0)
		#endif	
		#ifndef BRIGHTBLUE
			#define BRIGHTBLUE      GFX_RGBConvert(0, 0, 255)
		#endif	
		#ifndef BRIGHTGREEN
			#define BRIGHTGREEN     GFX_RGBConvert(0, 255, 0)
		#endif	
		#ifndef BRIGHTCYAN
			#define BRIGHTCYAN      GFX_RGBConvert(0, 255, 255)
		#endif	
		#ifndef BRIGHTRED
			#define BRIGHTRED       GFX_RGBConvert(255, 0, 0)
		#endif	
		#ifndef BRIGHTMAGENTA
			#define BRIGHTMAGENTA   GFX_RGBConvert(255, 0, 255)
		#endif	
		#ifndef YELLOW
			#define YELLOW          GFX_RGBConvert(255, 255, 128)
		#endif	
		#ifndef BRIGHTYELLOW
			#define BRIGHTYELLOW    GFX_RGBConvert(255, 255, 0)
		#endif	
		#ifndef LIGHTYELLOW
			#define LIGHTYELLOW     GFX_RGBConvert(255, 255, 150)
		#endif	
		#ifndef GOLD
			#define GOLD            GFX_RGBConvert(255, 215, 0)
		#endif
		#ifndef BLUE
			#define BLUE            GFX_RGBConvert(0, 0, 128)
		#endif	
		#ifndef GREEN
			#define GREEN           GFX_RGBConvert(0, 128, 0)
		#endif	
		#ifndef CYAN
			#define CYAN            GFX_RGBConvert(0, 128, 128)
		#endif	
		#ifndef RED
			#define RED             GFX_RGBConvert(128, 0, 0)
		#endif	
		#ifndef MAGENTA
			#define MAGENTA         GFX_RGBConvert(128, 0, 128)
		#endif	
		#ifndef BROWN
			#define BROWN           GFX_RGBConvert(255, 128, 0)
		#endif	
		#ifndef LIGHTGRAY
			#define LIGHTGRAY       GFX_RGBConvert(128, 128, 128)
		#endif	
		#ifndef DARKGRAY
			#define DARKGRAY        GFX_RGBConvert(64, 64, 64)
		#endif	
		#ifndef LIGHTBLUE
			#define LIGHTBLUE       GFX_RGBConvert(128, 128, 255)
		#endif	
		#ifndef LIGHTGREEN
			#define LIGHTGREEN      GFX_RGBConvert(128, 255, 128)
		#endif	
		#ifndef LIGHTCYAN
			#define LIGHTCYAN       GFX_RGBConvert(128, 255, 255)
		#endif	
		#ifndef LIGHTRED
			#define LIGHTRED        GFX_RGBConvert(255, 128, 128)
		#endif	
		#ifndef LIGHTMAGENTA
			#define LIGHTMAGENTA    GFX_RGBConvert(255, 128, 255)
		#endif	
		#ifndef WHITE
			#define WHITE           GFX_RGBConvert(255, 255, 255)
		#endif	
		#ifndef SADDLEBROWN
			#define SADDLEBROWN 	GFX_RGBConvert(139, 69, 19)
		#endif	
		#ifndef SIENNA
			#define SIENNA      	GFX_RGBConvert(160, 82, 45)
		#endif	
		#ifndef PERU
			#define PERU        	GFX_RGBConvert(205, 133, 63)
		#endif	
		#ifndef BURLYWOOD
			#define BURLYWOOD  	 	GFX_RGBConvert(222, 184, 135)
		#endif	
		#ifndef WHEAT
			#define WHEAT       	GFX_RGBConvert(245, 245, 220)
		#endif	
		#ifndef TAN
			#define TAN         	GFX_RGBConvert(210, 180, 140)
		#endif	
		#ifndef ORANGE
			#define ORANGE         	GFX_RGBConvert(255, 187, 76)
		#endif	
		#ifndef DARKORANGE
			#define DARKORANGE      GFX_RGBConvert(255, 140, 0)
		#endif	
		#ifndef LIGHTORANGE
			#define LIGHTORANGE     GFX_RGBConvert(255, 200, 0)
		#endif		
		#ifndef GRAY242
			#define GRAY242      	GFX_RGBConvert(242, 242, 242)    
		#endif	
		#ifndef GRAY229
			#define GRAY229      	GFX_RGBConvert(229, 229, 229)    
		#endif	
		#ifndef GRAY224
			#define GRAY224         GFX_RGBConvert(224, 224, 224)
		#endif	
		#ifndef GRAY204
			#define GRAY204      	GFX_RGBConvert(204, 204, 204)    
		#endif	
		#ifndef GRAY192
			#define GRAY192         GFX_RGBConvert(192, 192, 192)
		#endif	
		#ifndef GRAY160
			#define GRAY160         GFX_RGBConvert(160, 160, 160)
		#endif	
		#ifndef GRAY128
			#define GRAY128         GFX_RGBConvert(128, 128, 128)
		#endif	
		#ifndef GRAY096
			#define GRAY096          GFX_RGBConvert(96, 96, 96)
		#endif	
		#ifndef GRAY032
			#define GRAY032          GFX_RGBConvert(32, 32, 32)
		#endif 
		#ifndef GRAY010
			#define GRAY010          GFX_RGBConvert(10, 10, 10)
		#endif
		
        //DOM-IGNORE-BEGIN
        //Following color definitions are deprecated and should not be used in new projects.
		#ifndef GRAY95
			#define GRAY95      	GFX_RGBConvert(242, 242, 242)
		#endif	
		#ifndef GRAY90
			#define GRAY90      	GFX_RGBConvert(229, 229, 229)
		#endif		
		#ifndef GRAY0
			#define GRAY0           GFX_RGBConvert(224, 224, 224)
		#endif	
		#ifndef GRAY80
			#define GRAY80      	GFX_RGBConvert(204, 204, 204)
		#endif	
		#ifndef GRAY1
			#define GRAY1           GFX_RGBConvert(192, 192, 192)
		#endif	
		#ifndef GRAY2
			#define GRAY2           GFX_RGBConvert(160, 160, 160)
		#endif	
		#ifndef GRAY3
			#define GRAY3           GFX_RGBConvert(128, 128, 128)
		#endif	
		#ifndef GRAY4
			#define GRAY4           GFX_RGBConvert(96, 96, 96)
		#endif	
		#ifndef GRAY5
			#define GRAY5           GFX_RGBConvert(64, 64, 64)
		#endif	
		#ifndef GRAY6
			#define GRAY6           GFX_RGBConvert(32, 32, 32)
		#endif
        //DOM-IGNORE-END		
	#endif

#endif 

#endif  // #ifndef _GFX_COLORS_H

