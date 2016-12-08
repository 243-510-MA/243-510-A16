#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-miwikit_pic18f46j50_24j40.mk)" "nbproject/Makefile-local-miwikit_pic18f46j50_24j40.mk"
include nbproject/Makefile-local-miwikit_pic18f46j50_24j40.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=miwikit_pic18f46j50_24j40
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c ../../../../../../framework/miwi/src/miwi_mesh.c ../../../../../../framework/miwi/src/miwi_nvm.c ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c ../src/system_config/miwikit_pic18f46j50_24j40/system.c ../src/system_config/miwikit_pic18f46j50_24j40/delay.c ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c ../src/system_config/miwikit_pic18f46j50_24j40/button.c ../src/system_config/miwikit_pic18f46j50_24j40/spi.c ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c ../src/main.c ../src/door_unlock.c ../src/pan.c ../src/student.c ../src/teacher.c ../src/projector_screen.c ../src/network.c ../src/computer_control.c ../src/demo_pan.c ../src/demo_mouvement.c ../src/demo_911.c ../src/soft_uart.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1 ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1 ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1 ${OBJECTDIR}/_ext/1255583909/lcd.p1 ${OBJECTDIR}/_ext/1255583909/serial_flash.p1 ${OBJECTDIR}/_ext/1255583909/system.p1 ${OBJECTDIR}/_ext/1255583909/delay.p1 ${OBJECTDIR}/_ext/1255583909/symbol.p1 ${OBJECTDIR}/_ext/1255583909/button.p1 ${OBJECTDIR}/_ext/1255583909/spi.p1 ${OBJECTDIR}/_ext/1255583909/eeprom.p1 ${OBJECTDIR}/_ext/1360937237/main.p1 ${OBJECTDIR}/_ext/1360937237/door_unlock.p1 ${OBJECTDIR}/_ext/1360937237/pan.p1 ${OBJECTDIR}/_ext/1360937237/student.p1 ${OBJECTDIR}/_ext/1360937237/teacher.p1 ${OBJECTDIR}/_ext/1360937237/projector_screen.p1 ${OBJECTDIR}/_ext/1360937237/network.p1 ${OBJECTDIR}/_ext/1360937237/computer_control.p1 ${OBJECTDIR}/_ext/1360937237/demo_pan.p1 ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1 ${OBJECTDIR}/_ext/1360937237/demo_911.p1 ${OBJECTDIR}/_ext/1360937237/soft_uart.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d ${OBJECTDIR}/_ext/1255583909/lcd.p1.d ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d ${OBJECTDIR}/_ext/1255583909/system.p1.d ${OBJECTDIR}/_ext/1255583909/delay.p1.d ${OBJECTDIR}/_ext/1255583909/symbol.p1.d ${OBJECTDIR}/_ext/1255583909/button.p1.d ${OBJECTDIR}/_ext/1255583909/spi.p1.d ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d ${OBJECTDIR}/_ext/1360937237/main.p1.d ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d ${OBJECTDIR}/_ext/1360937237/pan.p1.d ${OBJECTDIR}/_ext/1360937237/student.p1.d ${OBJECTDIR}/_ext/1360937237/teacher.p1.d ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d ${OBJECTDIR}/_ext/1360937237/network.p1.d ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1 ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1 ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1 ${OBJECTDIR}/_ext/1255583909/lcd.p1 ${OBJECTDIR}/_ext/1255583909/serial_flash.p1 ${OBJECTDIR}/_ext/1255583909/system.p1 ${OBJECTDIR}/_ext/1255583909/delay.p1 ${OBJECTDIR}/_ext/1255583909/symbol.p1 ${OBJECTDIR}/_ext/1255583909/button.p1 ${OBJECTDIR}/_ext/1255583909/spi.p1 ${OBJECTDIR}/_ext/1255583909/eeprom.p1 ${OBJECTDIR}/_ext/1360937237/main.p1 ${OBJECTDIR}/_ext/1360937237/door_unlock.p1 ${OBJECTDIR}/_ext/1360937237/pan.p1 ${OBJECTDIR}/_ext/1360937237/student.p1 ${OBJECTDIR}/_ext/1360937237/teacher.p1 ${OBJECTDIR}/_ext/1360937237/projector_screen.p1 ${OBJECTDIR}/_ext/1360937237/network.p1 ${OBJECTDIR}/_ext/1360937237/computer_control.p1 ${OBJECTDIR}/_ext/1360937237/demo_pan.p1 ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1 ${OBJECTDIR}/_ext/1360937237/demo_911.p1 ${OBJECTDIR}/_ext/1360937237/soft_uart.p1

# Source Files
SOURCEFILES=../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c ../../../../../../framework/miwi/src/miwi_mesh.c ../../../../../../framework/miwi/src/miwi_nvm.c ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c ../src/system_config/miwikit_pic18f46j50_24j40/system.c ../src/system_config/miwikit_pic18f46j50_24j40/delay.c ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c ../src/system_config/miwikit_pic18f46j50_24j40/button.c ../src/system_config/miwikit_pic18f46j50_24j40/spi.c ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c ../src/main.c ../src/door_unlock.c ../src/pan.c ../src/student.c ../src/teacher.c ../src/projector_screen.c ../src/network.c ../src/computer_control.c ../src/demo_pan.c ../src/demo_mouvement.c ../src/demo_911.c ../src/soft_uart.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-miwikit_pic18f46j50_24j40.mk dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J50
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1: ../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1308774647" 
	@${RM} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1  ../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c 
	@-${MV} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.d ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/916281452/miwi_mesh.p1: ../../../../../../framework/miwi/src/miwi_mesh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/916281452" 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/916281452/miwi_mesh.p1  ../../../../../../framework/miwi/src/miwi_mesh.c 
	@-${MV} ${OBJECTDIR}/_ext/916281452/miwi_mesh.d ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/916281452/miwi_nvm.p1: ../../../../../../framework/miwi/src/miwi_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/916281452" 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/916281452/miwi_nvm.p1  ../../../../../../framework/miwi/src/miwi_nvm.c 
	@-${MV} ${OBJECTDIR}/_ext/916281452/miwi_nvm.d ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/lcd.p1: ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/lcd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/lcd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/lcd.p1  ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/lcd.d ${OBJECTDIR}/_ext/1255583909/lcd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/lcd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/serial_flash.p1: ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/serial_flash.p1  ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/serial_flash.d ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/system.p1: ../src/system_config/miwikit_pic18f46j50_24j40/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/system.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/system.p1  ../src/system_config/miwikit_pic18f46j50_24j40/system.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/system.d ${OBJECTDIR}/_ext/1255583909/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/delay.p1: ../src/system_config/miwikit_pic18f46j50_24j40/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/delay.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/delay.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/delay.p1  ../src/system_config/miwikit_pic18f46j50_24j40/delay.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/delay.d ${OBJECTDIR}/_ext/1255583909/delay.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/delay.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/symbol.p1: ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/symbol.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/symbol.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/symbol.p1  ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/symbol.d ${OBJECTDIR}/_ext/1255583909/symbol.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/symbol.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/button.p1: ../src/system_config/miwikit_pic18f46j50_24j40/button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/button.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/button.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/button.p1  ../src/system_config/miwikit_pic18f46j50_24j40/button.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/button.d ${OBJECTDIR}/_ext/1255583909/button.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/button.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/spi.p1: ../src/system_config/miwikit_pic18f46j50_24j40/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/spi.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/spi.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/spi.p1  ../src/system_config/miwikit_pic18f46j50_24j40/spi.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/spi.d ${OBJECTDIR}/_ext/1255583909/spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/eeprom.p1: ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/eeprom.p1  ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/eeprom.d ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/main.p1: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/main.p1  ../src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/main.d ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/door_unlock.p1: ../src/door_unlock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/door_unlock.p1  ../src/door_unlock.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/door_unlock.d ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/pan.p1: ../src/pan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pan.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pan.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/pan.p1  ../src/pan.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/pan.d ${OBJECTDIR}/_ext/1360937237/pan.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/pan.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/student.p1: ../src/student.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/student.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/student.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/student.p1  ../src/student.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/student.d ${OBJECTDIR}/_ext/1360937237/student.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/student.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/teacher.p1: ../src/teacher.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/teacher.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/teacher.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/teacher.p1  ../src/teacher.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/teacher.d ${OBJECTDIR}/_ext/1360937237/teacher.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/teacher.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/projector_screen.p1: ../src/projector_screen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/projector_screen.p1  ../src/projector_screen.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/projector_screen.d ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/network.p1: ../src/network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/network.p1  ../src/network.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/network.d ${OBJECTDIR}/_ext/1360937237/network.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/network.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/computer_control.p1: ../src/computer_control.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/computer_control.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/computer_control.p1  ../src/computer_control.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/computer_control.d ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_pan.p1: ../src/demo_pan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_pan.p1  ../src/demo_pan.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_pan.d ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1: ../src/demo_mouvement.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1  ../src/demo_mouvement.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.d ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_911.p1: ../src/demo_911.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_911.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_911.p1  ../src/demo_911.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_911.d ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/soft_uart.p1: ../src/soft_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/soft_uart.p1  ../src/soft_uart.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/soft_uart.d ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1: ../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1308774647" 
	@${RM} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1  ../../../../../../framework/driver/mrf_miwi/src/drv_mrf_miwi_24j40.c 
	@-${MV} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.d ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1308774647/drv_mrf_miwi_24j40.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/916281452/miwi_mesh.p1: ../../../../../../framework/miwi/src/miwi_mesh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/916281452" 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/916281452/miwi_mesh.p1  ../../../../../../framework/miwi/src/miwi_mesh.c 
	@-${MV} ${OBJECTDIR}/_ext/916281452/miwi_mesh.d ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/916281452/miwi_mesh.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/916281452/miwi_nvm.p1: ../../../../../../framework/miwi/src/miwi_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/916281452" 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/916281452/miwi_nvm.p1  ../../../../../../framework/miwi/src/miwi_nvm.c 
	@-${MV} ${OBJECTDIR}/_ext/916281452/miwi_nvm.d ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/916281452/miwi_nvm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/lcd.p1: ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/lcd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/lcd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/lcd.p1  ../src/system_config/miwikit_pic18f46j50_24j40/lcd.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/lcd.d ${OBJECTDIR}/_ext/1255583909/lcd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/lcd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/serial_flash.p1: ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/serial_flash.p1  ../src/system_config/miwikit_pic18f46j50_24j40/serial_flash.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/serial_flash.d ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/serial_flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/system.p1: ../src/system_config/miwikit_pic18f46j50_24j40/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/system.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/system.p1  ../src/system_config/miwikit_pic18f46j50_24j40/system.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/system.d ${OBJECTDIR}/_ext/1255583909/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/delay.p1: ../src/system_config/miwikit_pic18f46j50_24j40/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/delay.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/delay.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/delay.p1  ../src/system_config/miwikit_pic18f46j50_24j40/delay.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/delay.d ${OBJECTDIR}/_ext/1255583909/delay.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/delay.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/symbol.p1: ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/symbol.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/symbol.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/symbol.p1  ../src/system_config/miwikit_pic18f46j50_24j40/symbol.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/symbol.d ${OBJECTDIR}/_ext/1255583909/symbol.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/symbol.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/button.p1: ../src/system_config/miwikit_pic18f46j50_24j40/button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/button.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/button.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/button.p1  ../src/system_config/miwikit_pic18f46j50_24j40/button.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/button.d ${OBJECTDIR}/_ext/1255583909/button.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/button.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/spi.p1: ../src/system_config/miwikit_pic18f46j50_24j40/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/spi.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/spi.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/spi.p1  ../src/system_config/miwikit_pic18f46j50_24j40/spi.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/spi.d ${OBJECTDIR}/_ext/1255583909/spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1255583909/eeprom.p1: ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1255583909" 
	@${RM} ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1255583909/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1255583909/eeprom.p1  ../src/system_config/miwikit_pic18f46j50_24j40/eeprom.c 
	@-${MV} ${OBJECTDIR}/_ext/1255583909/eeprom.d ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1255583909/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/main.p1: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/main.p1  ../src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/main.d ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/door_unlock.p1: ../src/door_unlock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/door_unlock.p1  ../src/door_unlock.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/door_unlock.d ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/door_unlock.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/pan.p1: ../src/pan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pan.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pan.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/pan.p1  ../src/pan.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/pan.d ${OBJECTDIR}/_ext/1360937237/pan.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/pan.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/student.p1: ../src/student.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/student.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/student.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/student.p1  ../src/student.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/student.d ${OBJECTDIR}/_ext/1360937237/student.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/student.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/teacher.p1: ../src/teacher.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/teacher.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/teacher.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/teacher.p1  ../src/teacher.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/teacher.d ${OBJECTDIR}/_ext/1360937237/teacher.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/teacher.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/projector_screen.p1: ../src/projector_screen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/projector_screen.p1  ../src/projector_screen.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/projector_screen.d ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/projector_screen.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/network.p1: ../src/network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/network.p1  ../src/network.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/network.d ${OBJECTDIR}/_ext/1360937237/network.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/network.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/computer_control.p1: ../src/computer_control.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/computer_control.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/computer_control.p1  ../src/computer_control.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/computer_control.d ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/computer_control.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_pan.p1: ../src/demo_pan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_pan.p1  ../src/demo_pan.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_pan.d ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_pan.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1: ../src/demo_mouvement.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1  ../src/demo_mouvement.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.d ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_mouvement.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/demo_911.p1: ../src/demo_911.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/demo_911.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/demo_911.p1  ../src/demo_911.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/demo_911.d ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/demo_911.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/soft_uart.p1: ../src/soft_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1360937237/soft_uart.p1  ../src/soft_uart.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/soft_uart.d ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/soft_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.map  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"       --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.map  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=pro -P -N255 -I"../src" -I"../../src" -I"../../../../../../framework" -I"../src/system_config/miwikit_pic18f46j50_24j40" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/miwi_demo_kit.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/miwikit_pic18f46j50_24j40
	${RM} -r dist/miwikit_pic18f46j50_24j40

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
