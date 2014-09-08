CC := g++
LD := g++
RM := rm -rf
LIBFOLDER := ./shared_objects/
USER_OBJS := $(LIBFOLDER)libgnsdk_manager.so.3.07.0 $(LIBFOLDER)libgnsdk_dsp.so.3.07.0 $(LIBFOLDER)libgnsdk_storage_sqlite.so.3.07.0 $(LIBFOLDER)libgnsdk_lookup_local.so.3.07.0 $(LIBFOLDER)libgnsdk_musicid.so.3.07.0 $(LIBFOLDER)libgnsdk_musicid_stream.so.3.07.0
LIBS := -lm -lstdc++ -lpthread -ldl
CFLAGS := -O2 -g3 -Wall -c -fmessage-length=0 -v -fPIC -MMD -MP
CDEFS := -D_THREAD_SAFE -D_REENTRANT -DGCSL_STRICT_HANDLES -DGNSSDK_STRICT_HANDLES -DGNSDK_DSP -DGNSDK_MUSICID -DGNSDK_STORAGE_SQLITE -DGNSDK_LOOKUP_LOCAL

CPP_SRCS += \
GNMusicService.cpp \
main.cpp 

OBJS += \
GNMusicService.o \
main.o 

CPP_DEPS += \
GNMusicService.d \
main.d 

all:	musicident

musicident: $(OBJS) $(USER_OBJS) musicident
	$(LD) -L"./shared_objects/" -Wl,-rpath=$(LIBFOLDER) -o "musicident" $(OBJS) $(USER_OBJS) $(LIBS)
	
%.o: ./%.cpp
	$(CC) $(CDEF) -I"./" -I"./include" $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) musicident
	
