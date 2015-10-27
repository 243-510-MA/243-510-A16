/*******************************************************************************
  Big Integer source File

  Company:      Microchip Technology Incorported
  
  iFile Name:    BIGINT.c

// DOM-IGNORE-BEGIN
******************************************************************************
Copyright © <year> released Microchip Technology Inc.  All rights reserved.

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

#include "bigint/bigint.h"
#include "bigint/src/16bit/bigint_private_16bit.h"
#include <string.h>


/*******************************************************************************
  Function:
   
   void BIGINT_Set(BIGINT_DATA *ptr, uint8_t value);

  Summary:
   Big Integer initialization  routine.

  Description:
   [*ptr:*(ptr+bLength)]=value;

  Precondition:
   None.

  Parameters:
   Pointer to the operand.

  Returns:
   None.

  Example:
    <code>
    void BIGINT_Set(ptr,0x88);
    </code>

  Remarks:
  Only Little Endianness is supported.If the operands are not in the Little Endian format,
  They need to be changed to Little-Endianness format.
 *****************************************************************************/
void BIGINT_Set(BIGINT_DATA *ptr,uint8_t value)
{
    BIGINT_DATA_TYPE val;

    val = value | (value << 8);

    _setBI(ptr,&val);
}

/*******************************************************************************
  Function:
   
   BIGINT_RESULT BIGINT_Make(BIGINT_DATA *ptr,uint8_t* value,uint32_t bLength);

  Summary:
   Big Integer initialization to point to raw data.

  Description:
   ptr->startAddress would point to value.   

  Precondition:
   None.

  Parameters:
   Pointer to the operand.

  Returns:
   None.

  Example:
    <code>
    uint8_t data[50];
    void BIGINT_Set(ptr,&data,50);
    </code>

  Remarks:
  Only Little Endianness is supported.If the operands are not in the Little Endian format,
  They need to be changed to Little-Endianness format.
 *****************************************************************************/
BIGINT_RESULT BIGINT_Make(BIGINT_DATA *ptr, void * value, uint32_t bLength)
{
    BIGINT_RESULT returnVal;

    if ((bLength % BIGINT_DATA_SIZE) != 0)
    {
        returnVal = BIGINT_RESULT_INVALID_PARAMETER;
    }
    else
    {
        ptr->startAddress = (uint8_t *)value;
        ptr->bLength = bLength;
        returnVal = BIGINT_RESULT_OK;
    }

    return returnVal;
}
    
 /*******************************************************************************
  Function:
   
   void BIGINT_Multiply(BIGINT_DATA*result, BIGINT_DATA *arg1, BIGINT_DATA *arg2 )

  Summary:
   Big Integer multiplication routine.

  Description:
    This function performs the multiplication of A and B and saves it in C.
     result=arg1*arg2;     

  Precondition:
   result is initialized to all zeros.

  Parameters:
   Pointers to the operands and the result

  Returns:
   None.

  Example:
    <code>
    void BigIntMultiply(res,arg1,arg2); </code>

  Remarks:
  Only Little Endianness is supported.If the operands are not in the Little Endian format,
  They need to be changed to Little-Endianness format.
 *****************************************************************************/
BIGINT_RESULT BIGINT_Multiply(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2)
{
    BIGINT_DATA_TYPE * a, *b;

    a = BigIntMSB(arg1);
    b = BigIntMSB(arg2);

    if ((result->bLength) < ((a - (uint16_t *)arg1->startAddress) + (b - (uint16_t *)arg2->startAddress) + 2))
        return BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;

    BIGINT_Set(result,0x00);     // clear res before performing multiplication
    _mulBI(arg1, arg2, result);     // Perform the multiplication

    return BIGINT_RESULT_OK;
}


/*******************************************************************************
  Function:
    BIGINT_RESULT BIGINT_Add(BIGINT_DATA *result, BIGINT_DATA *arg1, 
        BIGINT_DATA *arg2);

  Summary:
   Big Integer addition routine.

  Description:
    This function performs the addition of arg1 and arg2 and saves it in result.

    result=arg1+arg2; 

    If the result pointer is the same pointer as arg1 or arg2 and the result 
    buffer bLength is greater than or equal to the bLength of the largest argument,
    this function will execute much faster.  You can ensure that this is the 
    case by defining the BIGINT_USE_FAST_OPERATIONS_ONLY macro in bigint_config.h.
    If you have defined BIGINT_USE_FAST_OPERATIONS_ONLY and this function is 
    being called incorrectly or with incorrect parameters, the function will 
    loop forever; you can detect this condition while debugging your application
    and correct the function call.

  Precondition:
    None.

  Parameters:
    result - Pointer to the result of the addition.
    arg1 - The first operand to add.
    arg2 - The second operand to add.

  Returns:
   BIGINT_RESULT
    - The resultBufferLengthInsufficient flag will be set if the result buffer 
        is too short to store the result of the addition (including propagated 
        carry).  The addition will still be performed and stored up to the 
        result buffer's bLength.

  Example:
    <code>
        // Fast mode
        BIGINT_RESULT result;
        BIGINT_DATA operand1;
        BIGINT_DATA operand2;
        uint8_t buffer1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        uint8_t buffer2[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
        uint8_t resultCompare[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

        BIGINT_Make(&operand1, buffer1, sizeof(buffer1));
        BIGINT_Make(&operand2, buffer2, sizeof(buffer2));

        // For Fast mode, the bLength of the the third argument (operand2) must be
        // less than or equal to the bLength of the first argument (the result
        // buffer, operand1)
        result = BIGINT_Add(&operand1, &operand1, &operand2);

        // Note: Since buffer1 is the same size as buffer2, and the result is being 
        // stored in buffer1, result's error bit will only be set if there is a  
        // single carry bit that was truncated.  Since the addition of the most 
        // significant bytes in this example doesn't overflow (0x8 + 0x80 == 0x88) 
        // result should equal '0' in this case.

        // Check for failure
        if (!memcmp(buffer1, resultCompare, sizeof (buffer1)))
        {
            // Addition failed!
            while(1);
        }

        // Flexible argument mode (slower!)
        BIGINT_RESULT result;
        BIGINT_DATA operand1;
        BIGINT_DATA operand2;
        BIGINT_DATA operandResult;
        uint8_t buffer1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80};
        uint8_t buffer2[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
        uint8_t bufferResult[sizeof (buffer1)];
        uint8_t resultCompare[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00, 0x01};

        BIGINT_Make(&operand1, buffer1, sizeof(buffer1));
        BIGINT_Make(&operand2, buffer2, sizeof(buffer2));
        BIGINT_Make(&operandResult, bufferResult, sizeof(bufferResult));

        result = BIGINT_Add(&operandResult, &operand1, &operand2);

        // Note: In this example, the addition of the MSBs of buffer1 and buffer2 
        // will overflow.  The actual result of the addition is the value stored in 
        // resultCompare, but since bufferResult isn't large enough, the 0x01 will 
        // be truncated and the result.bits.resultBufferLengthInsufficient will
        // equal true.

        // Check for failure.
        // This check won't fail, since we are only checking based on 
        // the bLength of buffer1.
        if (!memcmp(bufferResult, resultCompare, sizeof (buffer1)))
        {
            // Addition failed!
            while(1);
        }

        // Check for failure.
        // This check will fail, since we are checking based on the 
        // bLength of the full result, which was truncated in buffer1.
        // The bLength of the longest result of an addition has the
        // potential to equal the bLength of the longest operand + 1.
        if (!memcmp(bufferResult, resultCompare, sizeof (resultCompare)))
        {
            // Addition failed!
            while(1);
        }

    </code>

  Remarks:
    Only little-endianness is supported.If the operands are not in the 
    little-endian format, they need to be changed to that format.  You can 
    use the BIGINT_EndiannessSwap function to do this.
 *****************************************************************************/
BIGINT_RESULT BIGINT_Add(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2)
{
    BIGINT_RESULT returnVal;

#if !defined (BIGINT_USE_FAST_OPERATIONS_ONLY)

    uint8_t addResult = true;
    int tempLength1;
    int tempLength2;
#endif

    returnVal = BIGINT_RESULT_OK;

    if ((result == arg1) && (arg1->bLength >= arg2->bLength))
    {
        // A = A + B
        addResult = _addBI(arg1,arg2);
    }
    else if ((result == arg2) && (arg2->bLength >= arg1->bLength))
    {
        // B = A + B
        addResult = _addBI(arg2,arg1);
    }
    else
    {
#if defined (BIGINT_USE_FAST_OPERATIONS_ONLY)
            // Your big integer variables are initialized incorrectly for fast operations
            while(1);
#else
        if (arg1->bLength >= arg2->bLength)
        {
            if ((result->bLength >= arg1->bLength) && (result->bLength >= arg2->bLength))
            {
                memcpy (result->startAddress, arg1->startAddress, arg1->bLength * BIGINT_DATA_SIZE);
                memset ((void *)result->startAddress + (arg1->bLength * BIGINT_DATA_SIZE), 0x00, (result->bLength - arg1->bLength) * BIGINT_DATA_SIZE);
                addResult = _addBI(result,arg2);
            }
            else
            {
                tempLength1 = arg1->bLength;
                tempLength2 = arg2->bLength;
                arg1->bLength = (arg1->bLength > result->bLength)?result->bLength:arg1->bLength;
                arg2->bLength = (arg2->bLength > result->bLength)?result->bLength:arg2->bLength;
                
                memcpy (result->startAddress, arg1->startAddress, result->bLength * BIGINT_DATA_SIZE);
                _addBI(result,arg2);
                returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;

                arg1->bLength = tempLength1;
                arg2->bLength = tempLength2;
            }
        }
        else
        {
            if ((result->bLength >= arg1->bLength) && (result->bLength >= arg2->bLength))
            {
                memcpy (result->startAddress, arg2->startAddress, arg2->bLength * BIGINT_DATA_SIZE);
                memset ((void *)result->startAddress + (arg2->bLength * BIGINT_DATA_SIZE), 0x00, (result->bLength - arg2->bLength) * BIGINT_DATA_SIZE);
                addResult = _addBI(result,arg1);
            }
            else
            {
                tempLength1 = arg1->bLength;
                tempLength2 = arg2->bLength;
                arg1->bLength = (arg1->bLength > result->bLength)?result->bLength:arg1->bLength;
                arg2->bLength = (arg2->bLength > result->bLength)?result->bLength:arg2->bLength;
                
                memcpy (result->startAddress, arg2->startAddress, result->bLength * BIGINT_DATA_SIZE);
                _addBI(result,arg1);
                returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;

                arg1->bLength = tempLength1;
                arg2->bLength = tempLength2;
            }
        }
#endif
    }

    if (!addResult)
    {
        returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
    }

    return returnVal;
}

/*******************************************************************************
  Function:
   
   void BIGINT_Subtract(BIGINT_DATA *result, BIGINT_DATA *arg1, BIGINT_DATA *arg2);

  Summary:
   Big Integer addition routine.

  Description:
    This function performs the addition of A and B and saves it in result.
     result=arg1-arg2; 
     The pointers A and C could either point to the same location or otherwise.

  Precondition:
    None.

  Parameters:
   Pointers to the operands and the result

  Returns:
   None.

  Example:
    <code>
   BIGINT_Add(arg1,arg1,arg2);
    </code>

  Remarks:
  Only Little Endianness is supported.If the operands are not in the Little Endian format,
  They need to be changed to Little-Endianness format.
 *****************************************************************************/
BIGINT_RESULT BIGINT_Subtract(BIGINT_DATA * result, BIGINT_DATA *arg1, BIGINT_DATA *arg2)
{
    BIGINT_RESULT returnVal;

#if !defined (BIGINT_USE_FAST_OPERATIONS_ONLY)
    uint8_t subResult;
    int tempLength1;
    int tempLength2;
#endif

    returnVal = BIGINT_RESULT_OK;

    if ((result == arg1) && (arg1->bLength >= arg2->bLength))
    {
        _subBI (arg1, arg2);
    }
    else
    {
#if defined (BIGINT_USE_FAST_OPERATIONS_ONLY)
        // Your big integer variables are initialized incorrectly for fast operations
        while(1);
#else
        if (arg1->bLength <= result->bLength)
        {
            // Copy the value of arg1 into result
            memcpy (result->startAddress, arg1->startAddress, arg1->bLength * BIGINT_DATA_SIZE);
            memset ((void *)result->startAddress + (arg1->bLength * BIGINT_DATA_SIZE), 0x00, (result->bLength - arg1->bLength) * BIGINT_DATA_SIZE);

            if (arg2->bLength <= result->bLength)
            {
                subResult = _subBI (result, arg2);
            }
            else
            {
                tempLength2 = arg2->bLength;
                arg2->bLength = result->bLength;
                subResult = _subBI (result, arg2);
                arg2->bLength = tempLength2;
                returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
            }
        }
        else
        {
            // The result buffer bLength is less than the argument buffer bLength
            tempLength1 = arg1->bLength;
            arg1->bLength = (arg1->bLength > result->bLength)?result->bLength:arg1->bLength;
            
            memcpy (result->startAddress, arg1->startAddress, result->bLength * BIGINT_DATA_SIZE);

            if (arg2->bLength <= result->bLength)
            {
                subResult = _subBI (result, arg2);
            }
            else
            {
                tempLength2 = arg2->bLength;
                arg2->bLength = result->bLength;
                subResult = _subBI (result, arg2);
                arg2->bLength = tempLength2;
                returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
            }

            returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
            arg1->bLength = tempLength1;
        }
#endif
    }

    if (!subResult)
    {
        returnVal |= BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
    }

    return returnVal;
}

 /*******************************************************************************
 Function:
   
   BIGINT_RESULT BIGINT_Copy(BIGINT_DATA *destination, BIGINT_DATA *source)

  Summary:
   Big Integer subtraction routine.

  Description:
    This function subtracts arg2 from arg2 and saves it in result.
     destination=source
     The pointers A and C could either point to the same location or otherwise.

  Precondition:
    None.

  Parameters:
   Pointers to the operands and the result

  Returns:
   None.

  Example:
    <code>
    BIGINT_Copy(destination,source);
    </code>

  Remarks:
  Only Little Endianness is supported.If the operands are not in the Little Endian format,
  They need to be changed to Little-Endianness format.
 *****************************************************************************/
BIGINT_RESULT BIGINT_Copy (BIGINT_DATA *destination, BIGINT_DATA *source)
{
    BIGINT_RESULT returnVal;

    if (destination->bLength < source->bLength)
        returnVal = BIGINT_RESULT_RESULT_BUFFER_LENGTH_INSUFFICIENT;
    else
        returnVal = BIGINT_RESULT_OK;

    _copyBI(destination,source);

    return returnVal;
}


/*********************************************************************
 * Function:        void BigIntSwapEndianness(BIGINT_DATA*a)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the BigInt
 *
 * Output:          *a: same value, with endianness swapped
 *
 * Side Effects:    None
 *
 * Overview:        Converts a big-endian data array to little-endian,
 *                    or a little-endian data array to big-endian.
 *
 * Note:            None
 ********************************************************************/
void BigIntSwapEndianness(BIGINT_DATA*a)
{
     uint8_t temp, *front, *end;
    BIGINT_DATA_TYPE ad, *address;    
    int bLength;
    address=(BIGINT_DATA_TYPE* )a->startAddress;
    bLength=a->bLength;
    ad=*address;
    front =(uint8_t*) *address;
    ad+=bLength;
    ad+=(sizeof(uint32_t) - 1);
    end=(uint8_t* )ad;
    while(front < end)
    {
        temp = *front;
        *front = *end;
        *end = temp;
        front++;
        end--;
    }    
}

/*********************************************************************
 * Function:        void BIGINTMod(BIGINT_DATA *n, BIGINT_DATA *m)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *                    *m: a pointer to the modulus
 *                    
 * Output:          *n contains the modded number
 *                     i.e: *n = *n % *m
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntMod() to calculate the modulus of two
 *                    really big numbers.
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/
BIGINT_RESULT BIGINT_Mod(BIGINT_DATA *result, BIGINT_DATA *a, BIGINT_DATA *b)
{
    BIGINT_DATA_TYPE    *_iB, *_xB, *_iR, _wC;
    BIGINT_DATA_TYPE    *ptrMSBa, MSBb;
    BIGINT_DATA_TYPE_2  qHatInt;
    BIGINT_DATA         temp1;
    uint32_t            tempLen;

    union
    {
        BIGINT_DATA_TYPE v[2];
        BIGINT_DATA_TYPE_2 Val;
    } topTwoWords;

    if (result == b)
    {
        // We can't store the result in m
        return BIGINT_RESULT_INVALID_PARAMETER;
    }
    else if (result != a)
    {
        // The result buffer is a separate buffer from n
        BIGINT_Copy (result, a);
    }


    // Set up assembly pointers for m
    // _iB and _xB are limiters in the _mas function
    _iB = b->startAddress;
    _xB = BigIntMSB(b);

    // Find the starting MSBs
    ptrMSBa = BigIntMSB(result);
    MSBb = *_xB;

    temp1.startAddress = result->startAddress;
    temp1.bLength = result->bLength;

    // Find out how many bytes we need to shift and move the LSB up
    _iR = result->startAddress + ((BigIntMagnitudeDifference(result, b) - 1) * BIGINT_DATA_SIZE);

    // This loops while the order of magnitude (in words) of n > m
    // Each iteration modulos off one word of magnitude from n
    while(_iR >= (BIGINT_DATA_TYPE *)result->startAddress)
    {
        // Find qHat = MSBn:MSBn-1/MSBb
        topTwoWords.Val = *((BIGINT_DATA_TYPE_2*)(ptrMSBa - 1));
        qHatInt = topTwoWords.Val / MSBb;
        if(qHatInt > BIGINT_DATA_MAX)  
            qHatInt = BIGINT_DATA_MAX;

        // Once qHat is determined, we multiply M by qHat, shift it up
        // as many bytes as possible, and subtract the result.
        // In essence, qHat is a rough estimation of the quotient, divided
        // by a power of 2^8 (PIC18) or 2^16 (PIC24/dsPIC)
        
        // This implementation multiplies and subtracts in the same step
        // using a _mas function which saves about 30% of clock cycles.

        // Save the old MSB and set up the ASM pointers
        _wC = (BIGINT_DATA_TYPE)qHatInt;

        // Do the multiply and subtract
        // Occassionally this results in underflow...this is solved below.
        _masBI(_xB, _iB, _wC, _iR);

        // qHat may have been 1 or 2 greater than possible.  If so,
        // the new MSB will be greater than the old one, so we *add*
        // M back to N in the shifted position until overflow occurs
        // and this case corrects itself.
        while(topTwoWords.v[1] < *ptrMSBa)
        {
            tempLen = result->bLength;
            result->bLength = (BigIntMSB(result) - _iR + 1) * BIGINT_DATA_SIZE;
            result->startAddress = _iR;

            _addBI(result,b);

            result->startAddress = temp1.startAddress;
            result->bLength = tempLen;
        }

        // We should have modulated off a word (or two if we were lucky),
        // so move our MSB and LSB pointers as applicable
        while(*ptrMSBa == 0x0u)
        {
            _iR--;
            result->bLength -= 2;
            ptrMSBa--;
        }
    }

    // Iteration of the _mas function can only handle full-byte orders
    // of magnitude.  The result may still be a little larger, so this
    // cleans up the last few multiples with simple subtraction.
    while(BIGINT_Compare(result, b) >= 0)
    {
//        _iA = result->startAddress;
//        _xA = result->startAddress + result->bLength - 1;

        _subBI(result,b);
    }

    result->startAddress = temp1.startAddress;
    result->bLength = temp1.bLength;

    return BIGINT_RESULT_OK;
}

/*********************************************************************
 * Function:        CHAR BIGINT_Compare(BIGINTGINT *a, BIGINTGINT *b)
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
int BIGINT_Compare(BIGINT_DATA *a, BIGINT_DATA *b)
{
    uint32_t                magA, magB;
    BIGINT_DATA_TYPE         valA, valB;
    BIGINT_DATA_TYPE         *ptrA;
    BIGINT_DATA_TYPE         *ptrB;
    
    magA = BigIntMSB(a) - (BIGINT_DATA_TYPE *)a->startAddress;
    magB = BigIntMSB(b) - (BIGINT_DATA_TYPE *)b->startAddress;

    if(magA > magB)
    {
        
        return 1;
    }
    else if(magA < magB)
    {
        return -1;
    }

 // Loop through all words, looking for a non-equal word
    ptrA = BigIntMSB(a);
    ptrB = BigIntMSB(b);
    while(ptrA >= (BIGINT_DATA_TYPE *)a->startAddress)    // Magnitude is same, no need to check ptrB bounds
    {
        valA = *ptrA--;
        valB = *ptrB--;
        
        if(valA > valB)
        {
                    
            return 1;
        }
        else if(valA < valB)
        {
                    
            return -1;
        }    
    }

    // All words were exactly identical, return match
    return 0;
}

/*********************************************************************
 * Function:        int BigIntMagnitudeDifference(const BIGINTGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the first number
 *                    *b: a pointer to the second number
 *                    
 * Output:          Returns the magnitude of difference in zero-bytes
 *
 * Side Effects:    None
 *
 * Overview:        Helps to quickly determine a byte shift for operations
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/

int BigIntMagnitudeDifference(BIGINT_DATA *a, BIGINT_DATA *b)
{
    return BigIntMagnitude(a) - BigIntMagnitude(b);
}

/*********************************************************************
 * Function:        WORD BigIntMagnitude(BIGINTGINT *n)
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

uint32_t BigIntMagnitude(BIGINT_DATA *n)
{
    return BigIntMSB(n) - (BIGINT_DATA_TYPE *)n->startAddress;
}

/*********************************************************************
 * Function:        static BIGINT_DATA_TYPE* BigIntMSB(const BIGINTGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *                    
 * Output:          n->ptrMSB points to the MSB of n
 *
 * Side Effects:    None
 *
 * Overview:        Updates the ptrMSB.  Use after an operation in which
 *                    the new MSB cannot be estimated.
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/

BIGINT_DATA_TYPE* BigIntMSB(BIGINT_DATA *n)
{
    return _msbBI(n);
}




