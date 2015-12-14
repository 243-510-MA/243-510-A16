/*******************************************************************************
  Big Integer Header File

Company:      Microchip Technology Incorported

File Name:    bigint.h

Summary:
BigInteger library Functions.

Description:
This file contains templates of the functions and structures required for performing basic operations on Big Integers.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
  Copyright (c) <year> released Microchip Technology Inc.  All rights reserved.

  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital signal
  controller that is integrated into your product or third party product
  (pursuant to the sublicense terms in the accompanying license agreement).

  You should refer to the license agreement accompanying this Software for
  additional information regarding your rights and obligations.

  SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END/
#include <stdint.h>

#ifndef __BIGINT_H_                      //avoid multiple inclusions.
#define __BIGINT_H_

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

// Big Integer result enumeration.
// Describes the result of Big Integer operations.
typedef enum
{
    BIGINT_RESULT_OK = 0x00,                                    // The operation was successful
    BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT = 0x01,     // The result buffer has insufficient length
    BIGINT_RESULT_INVALID_PARAMETER = 0x02                      // The user passed in an invalid parameter
} BIGINT_RESULT;


// *****************************************************************************
/* Big Integer Operation Structure

Summary:
Defines the structure used to access the Big Integers.

Description:
The structure has two fields. startAddress points to Byte 0 of the Big Integer.
length holds the size of the Big Integer.

Remarks:
The functions in this file support Little Endianness only. The Big Integers must be swapped to Little Endia-nness for correct functioning of the functions.
*/
typedef struct
{
    void *      startAddress; // Pointer to the least significant byte (lowest memory address)
    uint32_t    bLength;           // length in bytes 
}BIGINT_DATA;



// *****************************************************************************
// *****************************************************************************
// Section:BigInteger library Interface Routines
// *****************************************************************************
// *****************************************************************************

// Section:  Open and Configuration Functions

/*******************************************************************************
Function:

void BIGINT_Add(BIGINT_DATA *result,BIGINT_DATA *arg1, BIGINT_DATA * arg2)

Summary:
Big Integer addition routine.

Description:
This function performs the addition of  arg1 and arg2 and saves it in result.
The pointers arg1 and arg2 could either point to the same location or otherwise.

Precondition:
Result can overlap with memory pointed by either arg1 or arg2.

Parameters:
Pointers to the operands and the result

Returns:
None.

Example:
<code>
BIGINT_Add(result,arg1,arg2); 
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

BIGINT_RESULT BIGINT_Add(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

/*******************************************************************************
Function:

void BIGINT_Subtract(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

Summary:
Big Integer subtraction routine.

Description:
This function subtracts arg2 from arg2 and saves it in result.
result=arg1-arg2 
The pointers arg1, arg2 and result could either point to the same location or otherwise.

Precondition:
Result can overlap with memory pointed by either arg1 or arg2.

Parameters:
Pointers to the operands and the result

Returns:
None.

Example:
<code>
BIGINT_Subtract(result,arg1,arg2);
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

BIGINT_RESULT BIGINT_Subtract(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

/*******************************************************************************
Function:

void BIGINT_Multiply(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

Summary:
Big Integer multiplication routine.

Description:
This function performs the multiplication of arg1 and arg2 and saves it in result.
result=arg1*arg2     

Precondition:
 length of result=length of arg1 +length of arg2.
Result cannot overlap with memory pointed by either arg1 or arg2.

Parameters:
Pointers to the operands and the result

Returns:
None.

Example:
<code>
void BIGINT_Multiply((result,arg1,arg2);
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

BIGINT_RESULT BIGINT_Multiply(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

/*******************************************************************************
Function:

void BIGINT_Mod(BIGINT_DATA *result,BIGINT_DATA *arg1, BIGINT_DATA *arg2);

Summary:
Big Integer modulus routine.

Description:
This function performs the modulus of arg1 and arg2 and saves it in result.
result=arg1%arg2    

Precondition:
Result can overlap with memory pointed by either arg1 or arg2.

Parameters:
Pointers to the operands and the result

Returns:
None.

Example:
<code>
void BIGINT_Mod(result,arg1,arg2);
</code>

Remarks:
Only Little Endianness is supported.
If the operands are not in the Little Endian format,
they need to be changed to Little-Endianness format.

For now the result pointer has to be equal to arg1!
 *****************************************************************************/

BIGINT_RESULT BIGINT_Mod(BIGINT_DATA *result , BIGINT_DATA *arg1, BIGINT_DATA * arg2);

/*******************************************************************************
Function:

void BIGINT_Copy(BIGINT_DATA *destination, BIGINT_DATA*source);

Summary:
Big Integer copy routine.

Description:
This function copies destination from source.
destination=source;     

Precondition:
None.

Parameters:
Pointers to the operands and the result

Returns:
None.

Example:
<code>
void BIGINT_Copy(destination,source);
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

BIGINT_RESULT BIGINT_Copy(BIGINT_DATA *destination, BIGINT_DATA *source);

/*******************************************************************************
Function:

void BIGINT_Set(BIGINT_DATA *ptr,uint8_t value);

Summary:
Big Integer initialization to value.

Description:
[*ptr:*(ptr+length)]=value;     

Precondition:
None.

Parameters:
Pointer to the operand.

Returns:
None.

Example:
<code>
void BIGINT_Set(ptr,0x00);
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

void BIGINT_Set(BIGINT_DATA *ptr,uint8_t value);
/*******************************************************************************
Function:

void BIGINT_Make(BIGINT_DATA *bigIntData,uint32_t * startingAddress,uint32_t length);

Summary:
Big Integer initialization to point to raw data.

Description:
ptr->startAddress would point to value.   

Precondition:
None.

Parameters:


Returns:
None.

Example:
<code>
BIGINT_DATA bigIntData;
uint8_t data[50];
BIGINT_Make(&bigIntData,data,sizeof(data));
</code>

Remarks:
Only Little Endianness is supported.If the operands are not in the Little Endian format,
They need to be changed to Little-Endianness format.
 *****************************************************************************/

BIGINT_RESULT BIGINT_Make (BIGINT_DATA *bigIntData, void * startingAddress, uint32_t length);

/*******************************************************************************
Function:

void BIGINT_EndiannessSwap(BIGINT_DATA *ptr);

Summary:
Big Integer swap Endianness.

Description:
The function can be used to swap from little Endian to Big Endian format and  vice-versa    

Precondition:
None.

Parameters:
Pointer to the BIGINT_DATA to be swapped for endianness.

Returns:
None.

Example:
<code>
BIGINT_EndiannessSwap(ptr);
</code>

Remarks:/
 *****************************************************************************/

void BIGINT_EndiannessSwap(BIGINT_DATA *ptr);

/*********************************************************************
 * Function:        int BIGINT_MagnitudeDifference(BIGINT_DATA *a, BIGINT_DATA *b)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the first number
 *                    *b: a pointer to the second number
 *                    
 * Output:          Returns the magnitude of difference in bytes
 *
 * Side Effects:    None
 *
 * Overview:        Helps to quickly determine a byte shift for operations
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/

int  BIGINT_MagnitudeDifference(BIGINT_DATA *a, BIGINT_DATA *b);

/*********************************************************************
 * Function:        WORD BIGINT_Magnitude(BIGINTGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *                    
 * Output:          Returns the number of significant words in the data, less one (ex: 0x12, has zero magnitude)
 *
 * Side Effects:    None
 *
 * Overview:        Helps to quickly determine the magnitude of the number
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/

int  BIGINT_Magnitude(BIGINT_DATA *n);


/*********************************************************************
 * Function:        int BIGINT_Compare(BIGINTGINT *a, BIGINTGINT *b)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the first number
 *                    *b: a pointer to the second number
 *
 * Output:          0 if a == b
 *                    1 if a > b
 *                    -1 if a < b
 *
 * Side Effects:    None
 *
 * Overview:        Determines if a > b, a < b, or a == b
 *
 * Note:            Supports at least 2048 bits.
 *                    
 ********************************************************************/

int  BIGINT_Compare(BIGINT_DATA *a, BIGINT_DATA *b);


#endif  // __BIGINT_H_

