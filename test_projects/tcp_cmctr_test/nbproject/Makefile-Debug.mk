#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o \
	${OBJECTDIR}/main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/tcp_cmctr_test

dist/Debug/GNU-Linux-x86/tcp_cmctr_test: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tcp_cmctr_test ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/tcp_cmctr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -g -DDEBUG -I../../PAC/uclinux/ -I../../PAC/common -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o ../../PAC/common/tcp_cmctr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/tcp_cmctr_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -g -DDEBUG -I../../PAC/uclinux/ -I../../PAC/common -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o ../../PAC/uclinux/tcp_cmctr_w750.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DDEBUG -I../../PAC/uclinux/ -I../../PAC/common -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/tcp_cmctr_test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc