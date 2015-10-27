/********************************************************************
// Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/
#include "system.h"
#include "system_config.h"

#if defined(ENABLE_NVM)

    #include "miwi/miwi_nvm.h"


    
    extern void MacroNop(void);
            
    #if defined(USE_EXTERNAL_EEPROM) || defined(USE_DATA_EEPROM)
    
        uint16_t        nvmMyPANID;
        uint16_t        nvmCurrentChannel;
        uint16_t        nvmConnMode;
        uint16_t        nvmConnectionTable;
        uint16_t        nvmOutFrameCounter;
      
        #if defined(PROTOCOL_MIWI)
            uint16_t        nvmMyShortAddress;
            uint16_t        nvmMyParent;
            
            #ifdef NWK_ROLE_COORDINATOR
                uint16_t    nvmRoutingTable;
                uint16_t    nvmKnownCoordinators;
                uint16_t    nvmRole;
            #endif
        #endif
        
        #if defined(PROTOCOL_MIWI_PRO)
            uint16_t        nvmMyShortAddress;
            uint16_t        nvmMyParent;
            
            #ifdef NWK_ROLE_COORDINATOR
                uint16_t    nvmRoutingTable;
                uint16_t    nvmNeighborRoutingTable;
                uint16_t    nvmFamilyTree;
                uint16_t    nvmRole;
            #endif
        #endif
        
    #else

#if 0
        ROM uint8_t filler[ERASE_BLOCK_SIZE]  = {0x00};

        ROM uint8_t                nvmCurrentChannel;
        ROM API_UINT16_UNION            nvmMyPANID;
        ROM uint8_t                nvmConnMode;
        ROM CONNECTION_ENTRY    nvmConnectionTable[CONNECTION_SIZE];
        ROM API_UINT16_UNION           nvmOutFrameCounter;

        #if defined(PROTOCOL_MIWI)

            ROM API_UINT16_UNION        nvmMyShortAddress;
            ROM uint8_t         nvmMyParent;

            #if defined(NWK_ROLE_COORDINATOR)
                ROM uint8_t        nvmRoutingTable[8];
                ROM uint8_t        nvmKnownCoordinators;
                ROM uint8_t        nvmRole;
            #endif

        #endif

        #if defined(PROTOCOL_MIWI_PRO)

            ROM API_UINT16_UNION        nvmMyShortAddress;
            ROM uint8_t            nvmMyParent;

            #if defined(NWK_ROLE_COORDINATOR)
                ROM uint8_t        nvmRoutingTable[NUM_COORDINATOR/8];
                ROM uint8_t        nvmNeighborRoutingTable[NUM_COORDINATOR][NUM_COORDINATOR/8];
                ROM uint8_t        nvmFamilyTree[NUM_COORDINATOR];
                ROM uint8_t        nvmRole;
            #endif

        #endif

#else


#define BASE_ADDR   0x9000
        ROM uint8_t filler[ERASE_BLOCK_SIZE] @ BASE_ADDR = {0x00};

#define START_ADDR (BASE_ADDR+ERASE_BLOCK_SIZE)

#if defined(PROTOCOL_P2P)
    #define CONNECTION_ELEMENT_SIZE (MY_ADDRESS_LENGTH+ADDITIONAL_NODE_ID_SIZE+1)
#elif defined(PROTOCOL_MIWI) || defined(PROTOCOL_MIWI_PRO)
    #define CONNECTION_ELEMENT_SIZE (MY_ADDRESS_LENGTH+ADDITIONAL_NODE_ID_SIZE+5)
#else
    #error A Protocol must be defined in ConfigApp.h
#endif
        ROM uint8_t                nvmCurrentChannel                       @ START_ADDR;
        ROM API_UINT16_UNION            nvmMyPANID                              @ (START_ADDR+1);
        ROM uint8_t                nvmConnMode                             @ (START_ADDR+3);
        ROM CONNECTION_ENTRY    nvmConnectionTable[CONNECTION_SIZE]     @ (START_ADDR+4);
        ROM API_UINT16_UNION           nvmOutFrameCounter                      @ (START_ADDR+4+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE);

        #if defined(PROTOCOL_P2P)
            #define NVM_END     (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+8)
        #endif

        #if defined(PROTOCOL_MIWI)
            
            ROM API_UINT16_UNION        nvmMyShortAddress                       @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+8);
            ROM uint8_t            nvmMyParent                             @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+10);
            
            #if defined(NWK_ROLE_COORDINATOR)
                ROM uint8_t        nvmRoutingTable[8];                     @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+11);
                ROM uint8_t        nvmKnownCoordinators;                   @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+19);
                ROM uint8_t        nvmRole;                                @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+20);
                #define         NVM_END     (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+21)
            #else
                #define         NVM_END     (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+11)
            #endif

        #endif
        
        #if defined(PROTOCOL_MIWI_PRO)
            
            ROM API_UINT16_UNION        nvmMyShortAddress;                      @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+8);
            ROM uint8_t            nvmMyParent;                            @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+10);
            
            #if defined(NWK_ROLE_COORDINATOR)
                ROM uint8_t        nvmRoutingTable[NUM_COORDINATOR/8];     @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+11);
                ROM uint8_t        nvmNeighborRoutingTable[NUM_COORDINATOR][NUM_COORDINATOR/8];    @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+(NUM_COORDINATOR/8)+11);
                ROM uint8_t        nvmFamilyTree[NUM_COORDINATOR];         @ (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+(NUM_COORDINATOR/8)+NUM_COORDINATOR*(NUM_COORDINATOR/8)+11);
                ROM uint8_t        nvmRole;
                #define         NVM_END     (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+(NUM_COORDINATOR/8)+NUM_COORDINATOR*(NUM_COORDINATOR/8)+NUM_COORDINATOR+11)
            #else
                #define         NVM_END     (START_ADDR+CONNECTION_SIZE*CONNECTION_ELEMENT_SIZE+11)
            #endif
            
        #endif
#endif
                
        ROM uint8_t filler2[ERASE_BLOCK_SIZE] @ NVM_END = {0x00};
    #endif
    
    #if defined(USE_EXTERNAL_EEPROM)
        #if defined(EEPROM_SHARE_SPI)
        
            #define EESPIPut( x )       SPIPut( x )
            #define EESPIGet()          SPIGet()
            
        #else
            void SPIPut2(uint8_t);
            uint8_t SPIGet2(void);
            
            #define EESPIPut( x )       SPIPut2( x )
            #define EESPIGet()          SPIGet2()
  
        #endif
    #endif
    
    
    #if defined(USE_EXTERNAL_EEPROM)
        
        /*********************************************************************
        * Function:         void NVMRead(uint8_t *dest, uint8_t addr, uint8_t count)
        *
        * PreCondition:     SPI port has been initialized
        *
        * Input:		    dest -  pointer to the buffer to hold the read data
        *                   addr -  starting address for the read
        *                   count - total number of bytes to be read
        *
        * Output:		    none
        *
        * Side Effects:	    none
        *
        * Overview:		    This function will read count byte of data starting
        *                   from address addr, and store the read data to the
        *                   buffer starting from *dest.
        *
        * Note:			    None
        ********************************************************************/        
    	void NVMRead(uint8_t *dest, uint16_t addr, uint16_t count)
        {
            #if defined(__18CXX)
                uint8_t oldGIEH = INTCONbits.GIEH;
                
                INTCONbits.GIEH = 0;
            #else
                uint8_t oldRFIE = RFIE;
                
                RFIE = 0;
            #endif
            
            EE_nCS = 0;
            
            #if MCHP_EEPROM < MCHP_4KBIT
                EESPIPut(SPI_READ);
                EESPIPut(addr);
            #elif MCHP_EEPROM == MCHP_4KBIT
                if( addr > 0xFF )
                {
                    EESPIPut(SPI_READ | 0x08);
                }
                else
                {
                    EESPIPut(SPI_READ);
                }
                EESPIPut(addr);
            #elif MCHP_EEPROM < MCHP_1MBIT
                EESPIPut(SPI_READ);
                EESPIPut(addr>>8);
                EESPIPut(addr);
            #endif
            
        	while( count > 0 )
            {
                *dest++ = EESPIGet();
                count--;
            }
            EE_nCS = 1;
            
            #if defined(__18CXX)
                INTCONbits.GIEH = oldGIEH;
            #else
                RFIE = oldRFIE;
            #endif
        }

    	void NVMWrite(uint8_t *source, uint16_t addr, uint16_t count)
        {
            uint8_t PageCounter = 0;
            
            #if defined(__18CXX)
                uint8_t oldGIEH = INTCONbits.GIEH;
                INTCONbits.GIEH = 0;
            #else
                uint8_t oldRFIE = RFIE;
                RFIE = 0;
            #endif
           
            
EEPROM_NEXT_PAGE:
            do
            {
                EE_nCS = 0;
                EESPIPut(SPI_RD_STATUS);
                PageCounter = EESPIGet();
                EE_nCS = 1;
                MacroNop();
            } while(PageCounter & 0x01 );
    
            EE_nCS = 0;
            EESPIPut(SPI_EN_WRT);
            EE_nCS = 1;
            MacroNop();
            EE_nCS = 0;
            #if MCHP_EEPROM < MCHP_4KBIT
                EESPIPut(SPI_WRITE);
                EESPIPut(addr);
            #elif MCHP_EEPROM == MCHP_4KBIT
                if( addr > 0xFF )
                {
                    EESPIPut(SPI_WRITE | 0x08);
                }
                else
                {
                    EESPIPut(SPI_WRITE);
                }
                EESPIPut(addr);
            #elif MCHP_EEPROM < MCHP_1MBIT
                EESPIPut(SPI_WRITE);
                EESPIPut(addr>>8);
                EESPIPut(addr);
            #endif
            PageCounter = 0;
        	while( count > 0 )
            {
                EESPIPut(*source++);
                count--;
                PageCounter++;
                if( ((addr + PageCounter) & (NVM_PAGE_SIZE-1)) == 0 )
                {
                    EE_nCS = 1;
                    addr += PageCounter;
                    goto EEPROM_NEXT_PAGE;
                }
            }
            EE_nCS = 1;
            
            #if defined(__18CXX)
                INTCONbits.GIEH = oldGIEH;
            #else
                RFIE = oldRFIE;
            #endif
            
        }
    #endif
 
 
    #if defined(USE_PROGRAMMING_SPACE)

        #if defined(__XC8) && !defined(_PIC18)

        #include <htc.h>
#asm
#include <caspic.h>
#ifndef	RD_bit
#error	"Selected device not supported, device cannot self-read/write"
#endif
#ifdef	WR_bit
#define	has_WR
#endif
#ifdef	FREE_bit
#define has_FREE
#endif
#ifdef	LWLO_bit
#define has_LWLO
#endif
#ifdef	CFGS_bit
#define has_CFGS
#endif
#ifdef	EEPGD_bit
#define has_EEPGD
#endif
#endasm

#if _EEPROMSIZE > 0
#define FLASHCON1bits EECON1bits
#define FLASHCON2 EECON2
#define FLASHADRL EEADRL
#define FLASHADRH EEADRH
#define FLASHDATL EEDATL
#define FLASHDATH EEDATH
#else
#define FLASHCON1bits PMCON1bits
#define FLASHCON2 PMCON2
#define FLASHADRL PMADRL
#define FLASHADRH PMADRH
#define FLASHDATL PMDATL
#define FLASHDATH PMDATH
#endif

void required_sequence(void)
#ifndef has_WR
{}
#else
{
	FLASHCON1bits.WREN = 1;						// Enable write/erase
	#asm
	#include <caspic.h>
		//BCF		CARRY_bit						; Clear C, we will use it to save GIE
		//BTFSC	GIE_bit							; Test GIE
		//BSF		CARRY_bit						; If set, save it off
		//BCF		GIE_bit							; Disable interrupts
		MOVLW	0x55
		MOVWF	BANKMASK(FLASHCON2)
		MOVLW	0xAA
		MOVWF	BANKMASK(FLASHCON2)
		BSF		WR_bit
		//BTFSC	CARRY_bit						; If GIE was set before sequence
		//BSF		GIE_bit							; Re-enable it
	#endasm
        NOP();
        NOP();
	FLASHCON1bits.WREN = 0;						// Disable writes
}
#endif


        void NVMRead(uint8_t *dest, ROM uint8_t *addr, uint16_t count)
        {
            uint8_t i;
            
            for(i = 0; i < count; i++)
            {
                #ifdef has_EEPGD
                    FLASHCON1bits.EEPGD = 1;					// Access flash memory
                #endif
                #ifdef has_CFGS
                    FLASHCON1bits.CFGS = 0;						// Access flash memory
                #endif
                FLASHADRL = (unsigned int)addr+i;           //(char)(addr+i);					// Load address
                FLASHADRH = ((unsigned int)addr+i)>>8;                                //(char)((addr+i) >> 8);			// Load address high
                FLASHCON1bits.RD = 1;						// Initiate read
                NOP();
                NOP();
                dest[i] = FLASHDATL;
            }
        }

        uint8_t memBlock[ERASE_BLOCK_SIZE];
        void NVMWrite(uint8_t *src, ROM uint8_t* dest, uint16_t count)
        {
            ROM char *pEraseBlock;
            uint8_t *pMemBlock;
            uint8_t writeIndex;
            uint8_t writeStart;
            uint8_t writeCount;
            uint8_t oldGIEH;

            oldGIEH = INTCONbits.GIE;
            INTCONbits.GIE = 0;

            #if defined(VERIFY_WRITE)
                while( memcmppgm2ram( src, (MEM_MODEL ROM void *)dest, count))
            #else
                //if (memcmppgm2ram( src, (far ROM void *)dest, count ))
            #endif
            {
                // First of all get nearest "left" erase block boundary
                //pEraseBlock = (ROM char*)((int)dest & (int)(~(ERASE_BLOCK_SIZE-1)));
                //writeStart = (uint8_t)((uint8_t)dest & (uint8_t)(ERASE_BLOCK_SIZE-1));
                    pEraseBlock = (unsigned int)dest - ((unsigned int)dest % ERASE_BLOCK_SIZE);
                    writeStart = (uint8_t)((unsigned int)dest - (unsigned int)pEraseBlock);

                while( count )
                {
                    // Now read the entire erase block size into RAM.
                    NVMRead(memBlock, (far ROM void*)pEraseBlock, ERASE_BLOCK_SIZE);

                    #ifdef has_EEPGD
                        FLASHCON1bits.EEPGD = 1;					// Access flash memory
                    #endif
                    #ifdef has_CFGS
                        FLASHCON1bits.CFGS = 0;						// Access flash memory
                    #endif
                    #ifdef has_FREE
                        FLASHCON1bits.FREE = 1;						// Signify erase instead of write
                    #endif
                    FLASHADRL = (unsigned int)pEraseBlock;					// Load address
                    FLASHADRH = ((unsigned int)pEraseBlock)>>8;			// Load address high

                    required_sequence();						// Required sequence to initiate erase

                    // Modify 64-byte block of RAM buffer as per what is required.
                    pMemBlock = &memBlock[writeStart];
                    while( (writeStart < ERASE_BLOCK_SIZE) && count )
                    {
                        *pMemBlock++ = *src++;
                        count--;
                        writeStart++;
                    }

                    // After first block write, next start would start from 0.
                    writeStart = 0;

                    // Now write entire 64 byte block in one write block at a time.
                    writeIndex = 0; //ERASE_BLOCK_SIZE / WRITE_BLOCK_SIZE;
                    pMemBlock = memBlock;
                    while( writeIndex < (ERASE_BLOCK_SIZE / WRITE_BLOCK_SIZE) )
                    {
                        #ifdef has_EEPGD
                            FLASHCON1bits.EEPGD = 1;					// Access flash memory
                        #endif
                        #ifdef has_CFGS
                            FLASHCON1bits.CFGS = 0;						// Access flash memory
                        #endif
                        #ifdef has_LWLO
                            FLASHCON1bits.LWLO = 1;						// Load write latches only
                        #endif
                        FLASHADRL = (char)((unsigned int)pEraseBlock+writeIndex*WRITE_BLOCK_SIZE);					// Load address
                        FLASHADRH = (char)(((unsigned int)pEraseBlock+writeIndex*WRITE_BLOCK_SIZE)>>8);			// Load address high

                        writeCount = WRITE_BLOCK_SIZE-1;
                        while(writeCount--)
                        {
                            FLASHDATL = *pMemBlock++;				// Load data
                            FLASHDATH = 0x34;
                            required_sequence();					// Required sequence for loading latches
                            FLASHADRL++;							// Increment write latch address
                        }
                        #ifdef has_LWLO
                            FLASHCON1bits.LWLO = 0;						// Last latch will perform write
                        #endif
                        FLASHDATL = *pMemBlock++;					// Load data
                        FLASHDATH = 0x34;
                        required_sequence();						// Required sequence for write

                        // One less block to write
                        writeIndex++;
                    }

                    // Go back and do it all over again until we write all
                    // data bytes - this time the next block.
                    pEraseBlock += ERASE_BLOCK_SIZE; 

                }
            }

            INTCONbits.GIE = oldGIEH;
        }

        #else
    	void NVMWrite(uint8_t *src, ROM uint8_t * dest, uint16_t count)
        {
            ROM char *pEraseBlock;
            static uint8_t memBlock[ERASE_BLOCK_SIZE];
            uint8_t *pMemBlock;
            uint8_t writeIndex;
            uint8_t writeStart;
            uint8_t writeCount;
            uint8_t oldGIEH;
            uint16_t oldTBLPTR;
        
            #if defined(VERIFY_WRITE)
                while( memcmppgm2ram( src, (MEM_MODEL ROM void *)dest, count))
            #elif defined(CHECK_BEFORE_WRITE)
                if (memcmppgm2ram( src, (MEM_MODEL ROM void *)dest, count ))
            #endif
            {
                // First of all get nearest "left" erase block boundary
                pEraseBlock = (ROM char*)((long)dest & (long)(~(ERASE_BLOCK_SIZE-1)));
                writeStart = (uint8_t)((uint8_t)dest & (uint8_t)(ERASE_BLOCK_SIZE-1));

                while( count )
                {
                    // Now read the entire erase block size into RAM.
                    NVMRead(memBlock, (far ROM void*)pEraseBlock, ERASE_BLOCK_SIZE);
                                
                    // Erase the block.
                    // Erase flash memory, enable write control.
                    EECON1 = 0x94;
                    
                    oldGIEH = INTCONbits.GIEH;
                    INTCONbits.GIEH = 0;
                    
                    #if defined(__18CXX) 
                        TBLPTR = (unsigned short long)pEraseBlock;
                    #endif

                    EECON2 = 0x55;
                    EECON2 = 0xaa;
                    EECON1bits.WR = 1;
                    MacroNop();
        
                    EECON1bits.WREN = 0;
        
                    oldTBLPTR = TBLPTR;

                    INTCONbits.GIEH = oldGIEH;
        
                    // Modify 64-byte block of RAM buffer as per what is required.
                    pMemBlock = &memBlock[writeStart];
                    while( writeStart < ERASE_BLOCK_SIZE && count )
                    {
                        *pMemBlock++ = *src++;
        
                        count--;
                        writeStart++;
                    }
        
                    // After first block write, next start would start from 0.
                    writeStart = 0;
        
                    // Now write entire 64 byte block in one write block at a time.
                    writeIndex = ERASE_BLOCK_SIZE / WRITE_BLOCK_SIZE;
                    pMemBlock = memBlock;
                    while( writeIndex )
                    {
        
                        oldGIEH = INTCONbits.GIEH;
                        INTCONbits.GIEH = 0;
        
                        TBLPTR = oldTBLPTR;
        
                        // Load individual block
                        writeCount = WRITE_BLOCK_SIZE;
                        while( writeCount-- )
                        {
                            TABLAT = *pMemBlock++;
        
                            //TBLWTPOSTINC();
                            _asm tblwtpostinc _endasm
                        }
        
                        // Start the write process: reposition tblptr back into memory block that we want to write to.
                        #if defined(__18CXX) 
                            _asm tblrdpostdec _endasm
                        #endif
        
                        // Write flash memory, enable write control.
                        EECON1 = 0x84;
        
                        EECON2 = 0x55;
                        EECON2 = 0xaa;
                        EECON1bits.WR = 1;
                        MacroNop();
                        EECON1bits.WREN = 0;
        
                        // One less block to write
                        writeIndex--;
        
                        TBLPTR++;
        
                        oldTBLPTR = TBLPTR;

                        INTCONbits.GIEH = oldGIEH;
                    }
        
                    // Go back and do it all over again until we write all
                    // data bytes - this time the next block.
                    #if !defined(WIN32)
                        pEraseBlock += ERASE_BLOCK_SIZE;
                    #endif
                   
                }
            }
        }
        #endif
    #endif 
 
    #if defined(USE_DATA_EEPROM)
    
    	void NVMRead(uint8_t *dest, uint16_t addr, uint16_t count)
        {
            while( count )
            {
                EEADRH = addr >> 8;
                EEADR = addr;
                EECON1bits.EEPGD = 0;
                EECON1bits.CFGS = 0;
                EECON1bits.RD = 1;   
                MacroNop();
                *dest++ = EEDATA;
                count--;
                addr++;
            }            
        }
        
    	void NVMWrite(uint8_t *source, uint16_t addr, uint16_t count)
        {
            uint8_t oldGIEH;
#if defined(__XC8) && !defined(_PIC18)
    #define GIE INTCONbits.GIE
#else
    #define GIE INTCONbits.GIEH
#endif
            while(count)
            {   
                EEADRH = addr >> 8;
                EEADR = addr;
                EEDATA = *source++;   
                EECON1bits.EEPGD = 0;
                EECON1bits.CFGS = 0;
                EECON1bits.WREN = 1;
                oldGIEH = GIE;
                GIE = 0;
                EECON2 = 0x55;
                EECON2 = 0xAA;
                EECON1bits.WR = 1;
                GIE = oldGIEH;
                while(EECON1bits.WR) ;
                EECON1bits.WREN = 0;
                count--;
                addr++;
            }
#undef GIE

        }
    #endif
 
    
    #if defined(USE_DATA_EEPROM) || defined(USE_EXTERNAL_EEPROM)
        
    	uint16_t nextEEPosition;
        bool NVMalloc(uint16_t size, uint16_t *location)
        {
            //WORD retval;
    
            if ((nextEEPosition + size) > TOTAL_NVM_BYTES)
            {
                return false;
            }
    
            *location = nextEEPosition;
            nextEEPosition += size;
            return true;
        }
        
    
        bool NVMInit(void)
        {
            bool result = true;
            
            nextEEPosition = 0;
            
            result &= NVMalloc(2, &nvmMyPANID);
            result &= NVMalloc(1, &nvmCurrentChannel);
            result &= NVMalloc(1, &nvmConnMode);
            result &= NVMalloc(sizeof(CONNECTION_ENTRY) * CONNECTION_SIZE, &nvmConnectionTable);
            result &= NVMalloc(4, &nvmOutFrameCounter);
            
            #if defined(PROTOCOL_MIWI)

                result &= NVMalloc(2, &nvmMyShortAddress);
                result &= NVMalloc(1, &nvmMyParent);
                
                #if defined(NWK_ROLE_COORDINATOR)
                    result &= NVMalloc(8, &nvmRoutingTable);
                    result &= NVMalloc(1, &nvmKnownCoordinators);
                    result &= NVMalloc(1, &nvmRole);
                #endif
            #endif
            
            #if defined(PROTOCOL_MIWI_PRO)

                result &= NVMalloc(2, &nvmMyShortAddress);
                result &= NVMalloc(1, &nvmMyParent);
                
                #if defined(NWK_ROLE_COORDINATOR)
                    result &= NVMalloc((NUM_COORDINATOR/8), &nvmRoutingTable);
                    result &= NVMalloc(((uint16_t)NUM_COORDINATOR/8*(uint16_t)NUM_COORDINATOR), &nvmNeighborRoutingTable);
                    result &= NVMalloc(NUM_COORDINATOR, &nvmFamilyTree);
                    result &= NVMalloc(1, &nvmRole);
                #endif
            #endif
            
            return result;
        }
        
    #endif 
 
 
 
 
#else
    extern char bogusVar;
    
#endif   
