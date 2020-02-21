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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/332fd38b/reader.o \
	${OBJECTDIR}/Channel/Channel.o \
	${OBJECTDIR}/DataFromFFMpeg/DataFromFFMpeg.o \
	${OBJECTDIR}/MetaData/MetaData.o \
	${OBJECTDIR}/Net/Net.o \
	${OBJECTDIR}/Settings/Settings.o \
	${OBJECTDIR}/StatusPrint/StatusPrint.o \
	${OBJECTDIR}/VideoFragment/VideoFileFragment.o \
	${OBJECTDIR}/netMonitor/netMonitor.o \
	${OBJECTDIR}/printer/printer.o \
	${OBJECTDIR}/writer.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nvr4.0

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nvr4.0: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nvr4.0 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/332fd38b/reader.o: /home/tenevoi/NetBeansProjects/nvr4.0/MetaData/reader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/332fd38b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/332fd38b/reader.o /home/tenevoi/NetBeansProjects/nvr4.0/MetaData/reader.cpp

${OBJECTDIR}/Channel/Channel.o: Channel/Channel.cpp
	${MKDIR} -p ${OBJECTDIR}/Channel
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Channel/Channel.o Channel/Channel.cpp

${OBJECTDIR}/DataFromFFMpeg/DataFromFFMpeg.o: DataFromFFMpeg/DataFromFFMpeg.cpp
	${MKDIR} -p ${OBJECTDIR}/DataFromFFMpeg
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DataFromFFMpeg/DataFromFFMpeg.o DataFromFFMpeg/DataFromFFMpeg.cpp

${OBJECTDIR}/MetaData/MetaData.o: MetaData/MetaData.cpp
	${MKDIR} -p ${OBJECTDIR}/MetaData
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MetaData/MetaData.o MetaData/MetaData.cpp

${OBJECTDIR}/Net/Net.o: Net/Net.cpp
	${MKDIR} -p ${OBJECTDIR}/Net
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Net/Net.o Net/Net.cpp

${OBJECTDIR}/Settings/Settings.o: Settings/Settings.cpp
	${MKDIR} -p ${OBJECTDIR}/Settings
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Settings/Settings.o Settings/Settings.cpp

${OBJECTDIR}/StatusPrint/StatusPrint.o: StatusPrint/StatusPrint.cpp
	${MKDIR} -p ${OBJECTDIR}/StatusPrint
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StatusPrint/StatusPrint.o StatusPrint/StatusPrint.cpp

${OBJECTDIR}/VideoFragment/VideoFileFragment.o: VideoFragment/VideoFileFragment.cpp
	${MKDIR} -p ${OBJECTDIR}/VideoFragment
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VideoFragment/VideoFileFragment.o VideoFragment/VideoFileFragment.cpp

${OBJECTDIR}/netMonitor/netMonitor.o: netMonitor/netMonitor.cpp
	${MKDIR} -p ${OBJECTDIR}/netMonitor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/netMonitor/netMonitor.o netMonitor/netMonitor.cpp

${OBJECTDIR}/printer/printer.o: printer/printer.cpp
	${MKDIR} -p ${OBJECTDIR}/printer
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/printer/printer.o printer/printer.cpp

${OBJECTDIR}/writer.o: writer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/writer.o writer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
