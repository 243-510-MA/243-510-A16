/*******************************************************************************
  MPFS2 Utility for Microchip TCP/IP Stack
    Keeps all the details of a file and all files in a list.

  Company:
    Microchip Technology Inc.

  File Name:
    MPFSFileRecord.java

  Summary:
    

  Description:
    Developed in NetBeans IDE 8.0, Java version 1.8.0
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

package MicrochipMPFS;
import java.util.*;

public class MPFSFileRecord
{
    private String fileName;
    public int nameHash;
    public long fileDate;
    public byte[] data;
    //public Vector<Byte> data;
    public int locStr;
    public int locData;
    public boolean hasIndex;
    public boolean isIndex;
    public boolean isZipped;
    public int dynVarCntr=0;/*Number of Dynamic Variables in the file*/
    public Vector<Byte> dynVarOffsetAndIndexID = new Vector<Byte>(8,8);/*Location of dynamic var and its ID*/
    public int fileRecordOffset;/* Byte location in the Record file where this file record/information is written from*/
    public int fileRecordLength;/* Total length/number of bytes in this file record*/
    public int fileSizeLen;
    /// <summary>
    /// Sets up a new MPFSFileRecord
    /// </summary>
    public MPFSFileRecord()
    {
        locStr = 0;
        locData = 0;
        hasIndex = false;
        isIndex = false;
        isZipped = false;
        dynVarCntr=0;
        //data = new Vector<Byte>(0);
        //Calendar cl = Calendar.getInstance();
        //cl.getTimeInMillis();
        fileSizeLen = 0;

    }
    public String GetFileName()
    {
        return this.fileName;
    }
    public void SetFileName(String value)
    {
        this.fileName = value;
        if(value.compareTo("")==0)
            this.nameHash = 0xffff;
        else
        {
            this.nameHash = 0;
            for(byte b : value.getBytes())
            {
                nameHash += b;
                nameHash <<= 1;
            }
        }
        //System.out.println ("Vlaue : " + value + "nameHash  "+String.format("%x",nameHash));
    }
    public void SetFiledate(long value)
    {
        this.fileDate = value;
    }
}

class FileRecord implements Comparable<FileRecord>
{
    public short nameHash;
    public int fileRecordOffset;
    public int dynVarCntr=0;

    public FileRecord(short nameHash, int fileRecordOffset,int dynVarCntr)
    {
       this.nameHash = nameHash;
       this.fileRecordOffset = fileRecordOffset;
       this.dynVarCntr = dynVarCntr;
    }

    public int compareTo(FileRecord o) {
        int firstNameHash = this.nameHash&0xffff;
        int secondNameHash = o.nameHash&0xffff;
        return firstNameHash - secondNameHash ;
    }
}
