#Makefile
#Author: Pham Huu Dang Nhat
#Version: 1.0
#Date: 20-4-2014

#------------------------- CONFIG INTERFACE -----------------------------------#
#location of source files and include files
SRCLOC = usartLib/
INCLOC = usartLib/

#location of binary and object files
BINDIR = bin

#------------------------------------------------------------------------------#

#name of executable file
EXEFILE = linux_zigbee_coor

#General VPATH
VPATH = ${SRCLOC}

#CC, LK and flags
CC = gcc
CXX = g++
LK = gcc

CFLAGS = $(addprefix -I, ${INCLOC}) -std=gnu99 -O0
CXXUWFLAGS = -std=gnu99
CXXEXFLAGS =
LKFLAGS =

#string array of all source files and object files
SRC = $(notdir $(wildcard *.c $(addsuffix *.c, ${SRCLOC} )))
SRCXX = $(notdir $(wildcard *.cpp $(addsuffix *.cpp, ${SRCLOC} )))

OBJC = $(SRC:%.c=${BINDIR}/%.o)
OBJCXX = $(SRCXX:%.cpp=${BINDIR}/%.o)

OBJ = $(OBJC)
OBJ += $(OBJCXX)
CPPMIX=$(if $(OBJCXX),1,)

.PHONY: all clean

all: $(OBJ)
	@echo "Linking..."
	$(if $(CPPMIX),$(CXX),$(LK)) ${LKFLAGS} $^ -o ${BINDIR}/${EXEFILE}
	@echo
	@echo "Finished"; echo
	
${OBJC}: ${BINDIR}/%.o: %.c
	@mkdir -p ${BINDIR}
	@echo "Compling $< ..."
	${CC} ${CFLAGS} -c $< -o $@
	@echo

${OBJCXX}: ${BINDIR}/%.o: %.cpp
	@mkdir -p ${BINDIR}
	@echo "Compling $< ..."
	${CXX} $(filter-out $(CXXUWFLAGS), $(CFLAGS)) $(CXXEXFLAGS) -c $< -o $@
	@echo
	
clean:
	rm -f -r ${BINDIR}
	rm -f *~
