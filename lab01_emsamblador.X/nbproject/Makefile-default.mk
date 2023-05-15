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
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=lab01_emsamblador.asm

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/lab01_emsamblador.o
POSSIBLE_DEPFILES=${OBJECTDIR}/lab01_emsamblador.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/lab01_emsamblador.o

# Source Files
SOURCEFILES=lab01_emsamblador.asm


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18f4550
MP_LINKER_DEBUG_OPTION= 
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/lab01_emsamblador.o: lab01_emsamblador.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lab01_emsamblador.o.d 
	@${RM} ${OBJECTDIR}/lab01_emsamblador.o 
	@${FIXDEPS} dummy.d -e "/media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.ERR" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_SIMULATOR=1 -q -p$(MP_PROCESSOR_OPTION) -u  $(ASM_OPTIONS)   \\\"/media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.asm\\\" 
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.O ${OBJECTDIR}/lab01_emsamblador.o
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.ERR ${OBJECTDIR}/lab01_emsamblador.o.err
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.LST ${OBJECTDIR}/lab01_emsamblador.o.lst
	@${RM}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.HEX 
	@${DEP_GEN} -d "${OBJECTDIR}/lab01_emsamblador.o"
	@${FIXDEPS} "${OBJECTDIR}/lab01_emsamblador.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
else
${OBJECTDIR}/lab01_emsamblador.o: lab01_emsamblador.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lab01_emsamblador.o.d 
	@${RM} ${OBJECTDIR}/lab01_emsamblador.o 
	@${FIXDEPS} dummy.d -e "/media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.ERR" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  $(ASM_OPTIONS)   \\\"/media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.asm\\\" 
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.O ${OBJECTDIR}/lab01_emsamblador.o
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.ERR ${OBJECTDIR}/lab01_emsamblador.o.err
	@${MV}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.LST ${OBJECTDIR}/lab01_emsamblador.o.lst
	@${RM}  /media/esteban/Esteban/UN/Microcontroladores/MPLAB_X_IDE/lab01_emsamblador.X/lab01_emsamblador.HEX 
	@${DEP_GEN} -d "${OBJECTDIR}/lab01_emsamblador.o"
	@${FIXDEPS} "${OBJECTDIR}/lab01_emsamblador.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION)  -w -x -u_DEBUG -z__ICD2RAM=1 -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"   -z__MPLAB_BUILD=1  -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_SIMULATOR=1 $(MP_LINKER_DEBUG_OPTION) -odist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
else
dist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"   -z__MPLAB_BUILD=1  -odist/${CND_CONF}/${IMAGE_TYPE}/lab01_emsamblador.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
