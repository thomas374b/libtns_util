
####
## universal Makefile by new_make (pantecs Makefile templater)
##

TARGET =  tns_util
LIBS   =  -lstdc++
DEFS =  

EXTRA_INC =  -I.  
EXTRA_LIB =  

BINDIR = bin
# where to install, choose one of bin, sbin or libexec

SUBPREFIX = /Pantec
# directory relative to your rootprefix (defaults to /opt/Pantec ) 

ifneq (_$(DEBUG_FLAGS)_,__)
GDBPREFIX = G
endif

CROSS = 
	# exec prefix for cross compiler e.g. arm-v4l-linux-
	
OPTS	= -O3 # -funroll-loops 
WARN	= -Wall -Wno-unused
DEBUG	= -ggdb -DDEBUG
CODE	= -fpic # -fpcc-struct-return 

MAJOR =  0
MINOR =  9
PATCHLEVEL = 1
VERSION = $(MAJOR).$(MINOR).$(PATCHLEVEL)

include .cache-$(shell uname -n)
include built/.versions

ifeq (_$(UNAME)$(MACHINE)_,__)
silent:
	exit
else

UMA = $(UNAME)-$(MACHINE)

ifeq (_$(MACHINE)_,_strongarm_)
	# zaurus (all models, kernel), use original Sharp compiler
	CROSS = arm-linux-
else
ifeq (_$(MACHINE)_,_armv5te_)
		# optimized for zaurus SLC-400 and higher, applications only
		CROSS = arm-v5te-linux-
else
ifeq (_$(MACHINE)_,_etrax100lx_)
			# FOX board
			CROSS = cris-axis-linux-gnu-
endif
endif	
endif	

CC	= $(CROSS)gcc
LD	= $(CROSS)ld
AR	= $(CROSS)ar
STRIP = $(CROSS)strip
CC_LINK	= $(CROSS)gcc
CC_VERSION = $(shell $(CC) -dumpversion)

SBIN_TARGET = $(shell test -f man/man8/$(TARGET).8 && echo "s")

#################################################################
#		first fence
#################################################################

# enable flags for various toolkits
WITH_XAW =  
WITH_MOTIF =  
WITH_XFORMS =  
WITH_MESAGL = 
WITH_QTOPIA =  
WITH_WXWIDGETS = 
WITH_X = 
WITH_XINC = 

# REVISION = $(shell date +%Y/%m/%d,%H:%M)
UID = $(shell id |sed -e 's/[0-9][0-9]*/ & /g'|cut -f2 -d' ')

ifeq (_${PREFIX}_,__)
	OPTROOT = /opt
		# choose one of /Software, /public, /usr/local, /opt or whatever your prefix is 
ifeq (_$(UID)_,_0_)
		PREFIX = $(OPTROOT)$(SUBPREFIX)
ifeq (_$(MACHINE)_,_strongarm_)
			PREFIX=/mnt/SD/armv4l
else
ifeq (_$(MACHINE)_,_armv5te_)
				PREFIX=/mnt/SD/armv5te
else
ifeq (_$(MACHINE)_,_etrax100lx_)
					PREFIX=/mnt/SD/etrax
endif
endif	
endif	

ifeq (_$(CROSS)_,__)
			PKGLOG = /var/log/packages
else	
			PKGLOG = $(OPTROOT)/$(UMA)/.packages
endif
else
		PREFIX = ${HOME}
		PKGLOG = ${HOME}/.packages
		# private users installation goes to your homedir
endif
else
	PREFIX = ${PREFIX}
		# use prefix variable from environment
endif

ifeq (_$(UID)_,_0_)
	# root is compiling
	CODEPREFIX=$(PREFIX)
else
	ifeq (_$(PREFIX)_,_/usr_)
		# a debian package is being built
		CODEPREFIX=$(PREFIX)
	else
		# machine dependent prefix for user home installation
		CODEPREFIX=$(PREFIX)/$(UMA)
	endif
endif

ifneq (_$(CROSS)_,__)
ifeq (_$(MACHINE)_,_strongarm_)
		CODEPREFIX=/mnt/SD/armv4l
else
ifeq (_$(MACHINE)_,_armv5te_)
			CODEPREFIX=/mnt/SD/armv5te
else
ifeq (_$(MACHINE)_,_etrax100lx_)
				CODEPREFIX=/mnt/SD/etrax
else
				CODEPREFIX=$(PREFIX)/$(UMA)	
endif
endif	
endif	
endif	


ifeq ($(WITH_QTOPIA),y)
	# if Qtopia toolkit is used
ifeq (_$(CROSS)_,__)	
	QPEDIR = /opt/Qt-2.3.3
	QTDIR = $(QPEDIR)/$(UMA)
else
	QPEDIR = /home/QtPalmtop
	QTDIR = $(QPEDIR)
endif

UISRC = $(shell ls *.ui 2>/dev/null |sed s/\.ui$$//g)
MOCSRC = $(shell grep -l Q_OBJECT *.h 2>/dev/null | sed s/\.h$$//g)

ifeq (_$(CROSS)_,_arm-v5te-linux-_)	
v5teerr:
	@echo -n "target qtopia libs can't be linked properly with gcc 3.3.2. "
	@echo "Use arm-linux-g++"
else
	LIBS += -lqte # -lcrypt -ldl 
	DEFS += -DQWS 
#			EXTRA_LIB += -L$(QTDIR)/lib
ifeq (_$(CROSS)_,__)
	LIBS += -lstdc++
	CODEPREFIX=$(PREFIX)
	PREFIX = $(QTDIR)
else
	DEFS += -DQT_NO_PROPERTIES
	LIBS += -lqpe -ljpeg # -lcrypt -ldl 
	EXTRA_INC += -I$(CODEPREFIX)/include
	EXTRA_LIB += -L$(QPEDIR)/lib
	PREFIX = $(QPEDIR)
endif
	# Meta-object compiler                                                                          
	MOC = $(QTDIR)/bin/moc
	
	# UI compiler                                                                                   
	UIC = $(QTDIR)/bin/uic
	
	UMAUIC = $(OPA)/.uic

ifneq (_$(UISRC)_,__)
		EXTRA_INC += -I$(UMAUIC)
endif

	OPTS +=	-pipe -fno-exceptions -fno-rtti

#	DEFS += -DQT_NO_DRAGANDDROP -DQT_NO_PROPERTIES 
#	LINK_OPTS = -Wl,-rpath,$(OPTROOT)/xscale/lib/gcc-lib/arm-v5te-linux/2.95.3	
endif
endif	# ifeq WITH_QTOPIA

MANSUFFIX = 1

GPREFIX=g
# on Solaris, HP-UX or IBM AIX gnu utils are prefixed like "gmake", "ginstall", "gtar" etc.
ifeq (_$(UNAME)_,_Linux_)
	# native
	GPREFIX=
endif
ifeq (_$(UNAME)_,_cygwin_)
	GPREFIX=
endif

TAR = $(GPREFIX)tar
INSTALL = $(GPREFIX)install
 
ifeq (_$(UNAME)_,_Linux_)
ifeq (_$(MACHINE)_,_sparc64_)
		OPTS += -msupersparc
		LIBS += -lNoVersion
else
ifeq (_$(MACHINE)_,_strongarm_)
			# special optimizations for xscale cpu
			OPTS += -mtune=$(MACHINE) 
else
ifeq (_$(MACHINE)_,_armv5te_)
			# special optimizations for xscale cpu
			OPTS += -march=$(MACHINE) -Wa,-mfpu=fpa
else
ifeq (_$(LIBC)_,_5_)
			OPTS += -m486
				DEFS += -DLIBC$(LIBC)
else
				OPTS += -march=$(MACHINE) 
endif	
endif	
endif
endif	
endif	

ifeq (_$(UNAME)_,_SunOS_)
	EXTRA_INC += -I/usr/openwin/include # -I/usr/ucbinclude 
	LD      = /usr/ccs/bin/ld
	STRIP   = /usr/ccs/bin/strip
	OPTS += -msupersparc
	LIBS += -lsocket -lnsl
endif

ifeq ($(WITH_QTOPIA),y)
	SHARES_PATH = $(QTDIR)/share/$(TARGET)
else
	SHARES_PATH = $(PREFIX)/share/$(TARGET)
endif


DEFS += -D$(UNAME) -D__$(MACHINE)__ 
DEFS += -DCOMPILER_HOST=\"$(shell hostname)\"
DEFS += -DCOMPILER_EXE=\"$(CC)\"
DEFS += -DCOMPILER_VERSION=\"$(CC_VERSION)\"


TARGETPREFIX=$(CODEPREFIX)

ifeq (_$(UID)_,_0_)
ifneq (_$(CROSS)_,__)
ifeq (_$(MACHINE)_,_etrax100lx_)
			TARGETPREFIX = /mnt/flash
endif
		SHARES_PATH = $(TARGETPREFIX)/share/$(TARGET)
endif
endif	

DEFS += -DPREFIX=\"$(TARGETPREFIX)\" 
DEFS += -DSHARES_PATH=\"$(SHARES_PATH)\"
INCLDIR = -I$(PREFIX)/include $(EXTRA_INC)
#LIBDIR	= -L$(PREFIX)/$(UMA)/lib 
LIBDIR= -L$(CODEPREFIX)/lib$(GDBPREFIX) $(EXTRA_LIB)

ifeq (_$(UNAME)_,_hpux_)
	SHARED_LIB = -b +b $(CODEPREFIX)/lib -B immediate -B nonfatal                                       
	LINK_OPTS = -Wl,+b,$(CODEPREFIX)/lib,-B,immediate,-B,nonfatal                                       
	MACHINE = $(shell uname -m|sed -e "s/\//\./g")
endif

ifeq ($(WITH_MESAGL),y)
	LIBS += -lMesaGL -lpthread 
	LIBDIR += -L$(OPTROOT)/lib/MesaGL
endif

ifeq ($(WITH_WXWIDGETS),y)
	LIBDIR	+= $(shell wx-config --libs)
	INCLDIR += $(shell wx-config --cflags)
endif

ifeq ($(WITH_MOTIF),y)
ifeq (_$(UNAME)_,_SunOS_)  
		INCLDIR += -I/usr/dt/include
		LIBDIR  += -L/usr/dt/lib
else
ifeq (_$(UNAME)_,_Linux_)  
#			INCLDIR += -I$(OPTROOT)/LessTif/Motif1.2/include
			INCLDIR += -I$(OPTROOT)/include/Lesstif-2.1
#			LIBDIR  += -L$(OPTROOT)/LessTif/Motif1.2/lib
else
errortarget:
			echo not defined for $(UNAME)
endif	
endif

	LIBS += -lXm -lXmu -lXt 
	WITH_X=y
endif

ifeq (_$(MACHINE)_,_etrax100lx_)
	LIBS += -lstdc++
	LIBDIR	+= -L/opt/cris/cris-axis-linux-gnu/lib
	INCLDIR += -I/opt/cris/cris-axis-linux-gnu/sys-include
#	DEFS += -DQT_NO_PROPERTIES
endif

ifeq ($(WITH_XFORMS),y)
	LIBS += -L$(OPTROOT)/lib -lforms -lm -lXpm
ifeq (_$(LIBC)_,_5_)
		LIBS += -lMesaGL -L$(OPTROOT)/lib/MesaGL -lXext
endif	
	WITH_X=y
endif

ifeq ($(WITH_XAW),y)
  LIBS += -lXmu -lXaw3d -lXt
  WITH_X=y
endif  


XINCLDIR =
XLIBDIR =
XLIBPATH =

ifeq (_$(UNAME)_,_SunOS_)  
  XINCLDIR += -I/usr/openwin/include/X11
  XLIBDIR  += -L/usr/openwin/lib
  XLIBPATH = /usr/openwin/lib/X11
else
ifeq (_$(UNAME)_,_Linux_)
  XINCLDIR += -I/opt/Xorg/include -I/usr/X11R6/include
  XLIBDIR  += -L/opt/Xorg/lib -L/usr/X11R6/lib
  XLIBPATH = /opt/Xorg/lib/X11
else
# ifeq (_$(UNAME)_,_hpux_)                                                                         
  XINCLDIR += -I/usr/include/X11R5                                                              
  XLIBDIR  += -L/usr/lib/X11R5     
  XLIBPATH = /usr/lib/X11
# else
#  echo X-includes not defined for $(UNAME)
# endif
endif
endif


ifeq ($(WITH_X),y)
  WITH_XINC=y
 	
  LIBS += $(XLIBDIR)	
  LIBS += -lX11 # -lXext # -lXt -lXmu -lSM -lICE 

###### the following X-libs might be useful, so remember to edit 
#  LIBS	  += -lXm -lXaw3d -lXintl -lforms -lXpm -lMesaGL

  BINDIR_ = $(BINDIR)/X11
else
  BINDIR_ = $(SBIN_TARGET)$(BINDIR)
ifeq (_$(SBIN_TARGET)_,_s_)
  MANSUFFIX = 8
endif
endif

ifeq ($(WITH_XINC),y)
  INCLDIR += $(XINCLDIR)
  DEFS += -DXLIBPATH=\"$(XLIBPATH)\"
endif

DYNLINK=0
ifeq (_$(UNAME)_,_Linux_)
	DYNLINK=1
else
ifeq (_$(UNAME)_,_SunOS_)
	DYNLINK=1
else
ifeq (_$(UNAME)_,_hpux_)                                                                         
	DYNLINK=1
else                                                                                            
endif   # hpux                                                                                        
endif	# SunOS
endif	# Linux

DEVLIBSYM = lib$(TARGET).so
SONAME = lib$(TARGET).so.$(MAJOR)
LIBNAME = lib$(TARGET)-$(VERSION).so

INCNAME = $(TARGET)
KMOD	= module/$(TARGET).o

ifeq (_$(DYNLINK)_,_0_)
	LIBNAME = lib$(TARGET).a
endif

include .headers
include .sources
include .defines
DEFS += $(DEFINES)

CFLAGS	= $(OPTS) $(WARN) $(DEFS) $(DEBUG_FLAGS) 

BINTARGET = $(TARGET)
ifeq (_$(UNAME)_,_cygwin_)
	BINTARGET = $(TARGET).exe	
endif

MAKE_T=$(LIBNAME)
      # only  $(BINTARGET) or $(LIBNAME) or $(KMOD)   allowed
      # $(LIBNAME) means we are compiling a library	
	  # $(KMOD) means we are compiling a kernel module


ifeq ($(MAKE_T),$(KMOD))
	INCLDIR = -I/usr/src/linux-$(KERNEL_VERSION)/include
	CFLAGS = -O6 -DMODULE -D__KERNEL__ -Wstrict-prototypes -fomit-frame-pointer $(DEFINES) $(WARN) $(DEBUG_FLAGS) $(EXTRA_INC)
	TARGETNAME = $(TARGET)
	UMA = $(UNAME)-$(MACHINE)-$(KERNEL_VERSION)
	OPA = built/$(UMA)
	OPA_M = $(OPA)/module
	KMODDIR = /lib/modules/$(KERNEL_VERSION)/vendor
	MANSUFFIX = 8
else
	OPA = built/$(UMA)
	OPA_M = $(OPA)
ifeq ($(MAKE_T),$(LIBNAME))
	CFLAGS += -I. $(CODE)
	TARGETNAME = $(SONAME)
	MANSUFFIX = 3
else
	TARGETNAME = $(TARGET)
endif
ifeq (_${LOGNAME}_,_root_)
ifeq (_$(CROSS)_,__)
	INST_PREFIX = $(PREFIX)
else
		INST_PREFIX = $(CODEPREFIX)
endif
else
	INST_PREFIX = $(PREFIX)/$(UMA)
endif
endif

include $(OPA)/.objects

CFLAGS += -DVERSION=\"$(VERSION)\" 
CFLAGS += -DBUILDDATE=\"$(shell date +%Y/%m/%d,%H:%M)\"
CFLAGS += -DTARGETNAME=\"$(TARGETNAME)\"

DEPEND=$(OPA)/.depend
O_MAKE_T=$(OPA)/$(MAKE_T)

O_TARGET=$(OPA)/$(BINTARGET)
O_LIBNAME=$(OPA)/$(LIBNAME)
O_KMOD=$(OPA)/$(KMOD)

ifeq ($(MAKE_T),$(BINTARGET)) 
	# we are going to compile a ordinary binary
	INSTALL_HELP = install_bin
else
ifeq ($(MAKE_T),$(KMOD)) 
	# we are going to compile a kernel module
	INSTALL_HELP = install_mod
else
	# we are going to compile a library
	INSTALL_HELP = install_lib, static_lib
endif
endif

ifeq ($(MAKE_T),$(LIBNAME))
	ifeq (_$(MACHINE)_,_etrax100lx_)
		# the cris architecture requires the -fPIC flag for libraries
		CFLAGS += -fPIC
	endif
endif

all: $(DEPEND) $(O_MAKE_T) 
	@ls -l $(O_MAKE_T)          

help:
	@(echo "useful targets are"			;\
	echo "		$(O_MAKE_T) (default target)"	;\
	echo "		install:"			;\
	echo "		$(INSTALL_HELP):"		;\
	echo "		clean:"				;\
	echo "		tgz:"				;\
	echo "		depend:"			;\
	echo "		gdb:"				;\
	echo " press [ENTER] to continue:"		;\
	read A )

ifeq ($(WITH_QTOPIA),y)
$(UMAUIC):
	mkdir -p $(UMAUIC)
	
.uic: $(UMAUIC)	
	rm -f .uic
	ln -s $(UMAUIC) .uic

$(UMAUIC)/.depend: .$(UMA)_o .uic
	@>$(UMAUIC)/.depend
ifneq (_$(UISRC)_,__)
	@echo generating uic dependencies
	@(for i in $(UISRC) 	;\
	do	echo create $(UMAUIC)/$$i.h >&2 ;\
		$(UIC) $$i.ui -o $(UMAUIC)/$$i.h	;\
		echo create uic_$$i.cpp >&2 ;\
		$(UIC) $$i.ui -i $(UMAUIC)/$$i.h -o uic_$$i.cpp	;\
		echo $(UMAUIC)/$$i.h: $$i.ui ;\
		echo "	$(UIC) $$i.ui -o $(UMAUIC)/$$i.h"	;\
		echo ;\
		echo uic_$$i.cpp: $$i.ui ;\
		echo "	$(UIC) $$i.ui -i $(UMAUIC)/$$i.h -o uic_$$i.cpp" ;\
		echo ;\
		echo ;\
		$(MOC) $(UMAUIC)/$$i.h -o moc_$$i.cpp ;\
		echo moc_$$i.cpp: $(UMAUIC)/$$i.h ;\
		echo "	$(MOC) $(UMAUIC)/$$i.h -o moc_$$i.cpp" ;\
		echo ;\
	done) >>$(UMAUIC)/.depend
endif	
ifneq (_$(MOCSRC)_,__)
	@echo generating moc dependencies
	@(for i in $(MOCSRC) 	;\
	do	echo create moc_$$i.cpp >&2 ;\
		$(MOC) $$i.h -o moc_$$i.cpp ;\
		echo moc_$$i.cpp: $$i.h  ;\
		echo "	$(MOC) $$i.h -o moc_$$i.cpp" ;\
		echo ;\
	done) >>$(UMAUIC)/.depend	
endif	
	@echo touch $(UMAUIC)/.depend	

#moc_%.cpp: %.h $(UMAUIC)/%.ui $(UMAUIC)
#	$(MOC) $(UMAUIC)/$$i.h -o moc_$$i.cpp	

$(OPA)/%.dep: $(UMAUIC)/.depend
else
$(OPA)/%.dep: 
endif
	@echo "refreshing dependency $@" 	
	@$(CC) $(CFLAGS) -DMODNAME=\"$*\" $(INCLDIR) -M $*  		|\
	sed -e "s/^.*\.o\:\ /built\/$(UMA)\/&/g;s/\: $*/& built\/.versions/g"  	>$@
	rm -f built/.versions

$(OPA)/%.o: 
	@echo [CC] $< \-\> $@
	@$(CC) $(CFLAGS) -DMODNAME=\"$*\" -DREVISION=\"$($<.version)\" $(INCLDIR) -c $< -o $@

.$(UMA)_o: 
	@(echo "making target directory $(UMA)" ;\
	 rm -rf $(OPA) ;\
	 mkdir -p $(OPA_M) ;\
	 touch .$(UMA)_o ;\
	 $(MAKE) help     )

cflag_info:
	@echo using flags $(CFLAGS) $(INCLDIR)
	@echo

$(TARGET)/Makefile:
	@if test -f $(TARGET)/Makefile; then echo ok; else ln -s . $(TARGET); fi

clean:
	@rm -f a.out $(OPA)/*.o core $(OPA)/lib*.so* $(OPA)/lib*.a $(TARGET).pc $(O_MAKE_T) 	

distclean:
	if test -L $(TARGET); then rm $(TARGET); fi
	rm -rf ./built moc_*.cpp uic_*.cpp .*_o core a.out *.bck .cache-* *.bak .*.bak .*.bck Makefile-* .hc~*~ $(DEBRT)
	chmod 440 .headers .sources .defines

$(O_KMOD): cflag_info $(OBJS)
ifeq (_$(UNAME)_,_Linux_)
	@echo kernel version $(KERNEL_VERSION)
	$(LD) -S -r -o $(O_KMOD) $(OBJS)
else
	@echo I will make kernel modules on linux systems only
endif


ifeq (_$(DYNLINK)_,_1_)
$(O_LIBNAME): cflag_info $(OBJS)
ifeq (_$(UNAME)_,_Linux_)
	$(CC_LINK) $(LIBDIR) -shared -Wl,-soname,$(SONAME) -o $(O_LIBNAME) $(OBJS) $(LIBS)
else
ifeq (_$(UNAME)_,_SunOS_)
	$(LD) -B dynamic -G -h $(SONAME) -o $(O_LIBNAME) $(OBJS) 
else
ifeq (_$(UNAME)_,_hpux_)                                                                         
	$(LD) $(SHARED_LIB) -o $(O_LIBNAME) $(OBJS)                                               
else                                                                                            
	@echo dynamic linking not implemented for $(UNAME)                                           
endif   # hpux                                                                                        
endif	# SunOS
endif	# Linux
else
$(O_LIBNAME): static_lib
	@echo dynamic linking not defined for $(UNAME)                                           
endif

static_lib: cflag_info $(OBJS)
	$(AR) -rc $(OPA)/lib$(TARGET).a $(OBJS)

$(O_TARGET): cflag_info $(OBJS)
	$(CC) $(LINK_OPTS) $(LIBDIR) -o $(O_TARGET) $(OBJS) $(LIBS)	

$(TARGET).pc:
	@(echo prefix=$(PREFIX);\
	echo exec_prefix=$(PREFIX);\
	echo libdir=$(INST_PREFIX)/lib;\
	echo includedir=$(PREFIX)/include/$(TARGET);\
	echo "";\
	echo Name: $(TARGET);\
	echo Description: $(TARGET) library;\
	echo Requires:;\
	echo Version: $(VERSION);\
	echo Libs: -L\$\{libdir\} -l$(TARGET);\
	echo Cflags: -I\$\{includedir\};\
	) >	$(TARGET).pc
		
install_pkg_conf: $(TARGET).pc
	@if test -d $(PREFIX)/lib/pkgconfig;\
	then $(INSTALL) -m 644 $(TARGET).pc $(PREFIX)/lib/pkgconfig;\
	fi
		
ifeq (_$(DYNLINK)_,_0_)
install_lib: install_pkg_conf $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX)
	$(INSTALL) -m 644 $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX)	
else
install_lib: install_pkg_conf $(O_LIBNAME) $(INST_PREFIX)/lib$(GDBPREFIX)
ifeq (_$(DEBUG_FLAGS)_,__)	
	$(STRIP) $(O_LIBNAME) 
endif
	@if test -r $(OPA)/lib$(TARGET).a ; then $(INSTALL) -m 644 $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX) ; fi
	$(INSTALL) -m 755 $(O_LIBNAME) $(INST_PREFIX)/lib$(GDBPREFIX)
ifeq (_${LOGNAME}_,_root_)
ifeq (_$(CROSS)$(DEBUG_FLAGS)_,__)
	ldconfig
else
	@(cd $(INST_PREFIX)/lib$(GDBPREFIX);\
	rm -f $(DEVLIBSYM) $(SONAME);\
	ln -s $(INST_PREFIX)/lib$(GDBPREFIX)/$(LIBNAME) $(SONAME);\
	ln -s $(INST_PREFIX)/lib$(GDBPREFIX)/$(LIBNAME) $(DEVLIBSYM) )
endif	
else
	@(cd $(INST_PREFIX)/lib$(GDBPREFIX);\
	rm -f $(DEVLIBSYM) $(SONAME);\
	ln -s $(INST_PREFIX)/lib$(GDBPREFIX)/$(LIBNAME) $(SONAME);\
	ln -s $(INST_PREFIX)/lib$(GDBPREFIX)/$(LIBNAME) $(DEVLIBSYM) )
endif
endif

install_inc: $(PREFIX)/include/$(TARGET)
	@(SRCPATH=`pwd` ;\
	  cd $(PREFIX)/include/$(TARGET) ; \
	  for I in $(HDRS) ;\
	    do rm -f $$I ;\
	    ln -s $$SRCPATH/$$I $$I ;\
	  done ) 		

compact: $(O_TARGET)
	$(STRIP) $(O_TARGET)
	ls -l $(O_MAKE_T)
	
ifeq ($(MAKE_T),$(KMOD))
$(KMODDIR):
	mkdir -m 755 -p $(KMODDIR) 
	
install_bin: $(O_KMOD) $(KMODDIR)
	@if test -w $(KMODDIR); \
	then $(INSTALL) -m 755 $(O_KMOD) $(KMODDIR); \
	ls -l $(KMODDIR)/$(TARGET).o				;\
	else echo must be root to write in $(KMODDIR); \
	fi
ifeq (_$(UID)_,_0_)
	depmod -a
else
	@echo must be root to write dependancies
endif
else
install_bin: compact  $(INST_PREFIX)/$(BINDIR_)    # 
	$(INSTALL) -m 755 $(O_TARGET) $(INST_PREFIX)/$(BINDIR_)
endif

install_mod: $(O_KMOD)
ifeq (_$(UID)_,_0_)
	@if [ "`lsmod |grep $(TARGETNAME)`" != "" ]; then rmmod $(TARGETNAME); fi
	insmod $(O_KMOD)
else
	@echo must be root to (un)load modules
endif

$(INST_PREFIX)/$(BINDIR_):
	mkdir -m 755 -p $(INST_PREFIX)/$(BINDIR_)

$(INST_PREFIX)/lib$(GDBPREFIX):
	mkdir -m 755 -p $(INST_PREFIX)/lib$(GDBPREFIX)

$(PREFIX)/share/$(TARGET):
	mkdir -p $(PREFIX)/share/$(TARGET)

$(PREFIX)/include/$(TARGET):
	mkdir -m 755 -p $(PREFIX)/include/$(TARGET)

$(PREFIX)/man:
	mkdir -m 755 -p $(PREFIX)/man

$(PREFIX)/doc/$(TARGET):
	mkdir -m 755 -p $(PREFIX)/doc/$(TARGET)

share:
	mkdir -m 755 -p ./share
	
doc:
	mkdir -m 755 -p ./doc	

htmldoc: doc built/.versions 	
	@(if test -r man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) ; then \
	nroff -man <man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) | man2html >doc/$(TARGET).html ;\
	fi )

install_doc: $(PREFIX)/doc/$(TARGET) htmldoc
	@(if test -r man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) ; then \
	cp doc/$(TARGET).html >$(PREFIX)/doc/$(TARGET)/$(TARGET).html ;\
	echo $(PREFIX)/doc/$(TARGET)/$(TARGET).html >>$(PKGLOG)/$(TARGET) ;\
	fi )
#	cd $(PREFIX)/doc/$(TARGET) && ln -sf $(PREFIX)/share/$(TARGET)/doc/* .	

$(PKGLOG):
	mkdir -m 755 -p $(PKGLOG)
	
sort_pkglog:
	@(if test -f $(PKGLOG)/$(TARGET) ; then \
	  sort $(PKGLOG)/$(TARGET) | egrep -v "/(.svn|CVS)/" | uniq >/tmp/.pkg.$(TARGET)	;\
	  mv /tmp/.pkg.$(TARGET) $(PKGLOG)/$(TARGET)	; fi )

install_share: $(PREFIX)/share/$(TARGET) 
	@(if test -d ./share ; then 	\
	  umask 022			;\
	  (cd ./share; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(PREFIX)/share/$(TARGET); $(TAR) -xf -) ;\
	fi)
	
install_man: $(PREFIX)/man $(PKGLOG)
	@(if test -d ./man ; then 	\
	  umask 022			;\
	  LIST=`ls *.[1-9] 2>/dev/null |\
	  sed -e "s/\ [A-Za-z0-9\._-]*.//g" `	;\
	  (cd ./man; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(PREFIX)/man; $(TAR) -xf -) ;\
	  find man -type f | egrep -v "CVS|\.svn" |\
	  awk '{print "$(PREFIX)/" $$0 }' >>$(PKGLOG)/$(TARGET)	;\
	  fi )

ifeq ($(MAKE_T),$(KMOD))
install: install_bin install_mod
else
ifeq ($(MAKE_T),$(BINTARGET))
install: install_bin install_share install_man install_doc 
	@(echo $(INST_PREFIX)/$(BINDIR_)/$(BINTARGET)	;\
	find $(PREFIX)/share/$(TARGET) | egrep -v "/(CVS|\.svn)/" ) >>$(PKGLOG)/$(TARGET)
	@$(MAKE) sort_pkglog
	@echo target was _$(MAKE_T)_ _$(BINTARGET)_ 
else
install: install_inc install_lib
	echo target was _$(MAKE_T)_ _$(TARGET)_
ifeq (_$(UNAME)_,_AIX_)                                                                         
else
endif
endif
endif


uninstall:
	rm -rf `cat $(PKGLOG)/$(TARGET)` 
	rm -f $(PKGLOG)/$(TARGET)
	
tgz:
	@($(MAKE) distclean	;\
	  inode=`ls -id . | awk '{print $$1}' `; \
	  cd .. ; \
	  ZIPDIR=`ls -ia | grep $$inode | awk '{print $$2}' ` ; \
	  $(TAR) --exclude CVS --exclude .svn --exclude .cvsignore -cvzf $$ZIPDIR-$(VERSION).tgz $$ZIPDIR $$ZIPDIR/.svn/entries ) 	

ifeq ($(MAKE_T),$(BINTARGET))
gdb: 
	@$(MAKE) DEBUG_FLAGS="$(DEBUG)" WARN="-Wall" OPTS="" && gdb $(O_TARGET)
else
gdb: 
	@$(MAKE) DEBUG_FLAGS="$(DEBUG)" WARN="-Wall" OPTS="" && make DEBUG_FLAGS="$(DEBUG)" install_lib
endif

gdball:  clean gdb


dep:	depend
depend:
	rm $(OPA)/*.dep $(OPA)/.depend
	$(MAKE) $(DEPEND)

ifeq ($(MAKE_T),$(LIBNAME))
$(DEPEND): $(TARGET)/Makefile .sources .headers .$(UMA)_o
else
$(DEPEND): .sources .headers .$(UMA)_o
endif
ifeq (_$(SRCS)_,__)
	$(MAKE) $(DEPEND)
else	
	@(echo creating new dependency list for $(UMA)  >&2 ; \
	    for i in $(SRCS) $(UISRCS) $(MOCSRCS)		; \
	    do echo "$$i" >&2                  	; \
	    echo include built/$(UMA)/$$i.dep	; \
	    done ) >$(DEPEND)
endif



$(OPA)/.objects: .sources .$(UMA)_o 
	@(echo creating .objects >&2; \
	cat .sources |\
	sed -e "s/\.[cC][pc]*/\.o/g;s/SRCS/OBJS/g;s/[-0-9A-Za-z_]*\.o/built\/$(UMA)\/&/g" >$(OPA)/.objects )

ifeq ($(WITH_QTOPIA),y)
.sources: $(UMAUIC)/.depend
else
.sources: 
endif
	@(echo " " | awk '{printf("SRCS = ")}' 		;\
	ls *.cc *.cpp *.C *.c 2>/dev/null	|		 \
	awk '{printf("\\"); printf("\n\t%s\t",$$1); }'	;\
	echo "	"	) >.sources 

.headers:
	@(echo " " | awk '{printf("HDRS = ")}'   	;\
	ls *.h *.H 2>/dev/null 		|		 \
	awk '{printf("\\"); printf("\n\t%s\t",$$1)}' 	;\
	echo "	"	) >.headers 


link:	$(OBJS) .rmlink $(O_TARGET)
.rmlink:
	rm -f $(O_TARGET)

forcedep: .rmdep $(DEPEND)
.rmdep:
	rm -f $(DEPEND)

srcs: .rmsrcs $(OPA)/.objects .headers forcedep
.rmsrc:
	rm -f .sources .headers	$(OPA)/.objects 

.defines: 		# $(HDRS) $(SRCS)
	@(echo scanning for additional defines >&2 ; \
	egrep -v '^//' *.[cChH] *.cc *.cpp 2>/dev/null	|\
	egrep '\#define [A-Z][A-Z_]*[ \	]*(\"[0-9A-Za-z_/\\]*\"|[0-9][0-9]*[ ]*)$$' |\
	awk '{for (i=2; i<=NF; i++) { \
	if (i==2) printf("// DEFINES += -D%s=",$$i);	\
	else printf("%s",$$i); } printf("\n");}' 	|\
	sed -e 's/\"/\\&/g' >>.defines )


## dependencies follow

include $(DEPEND)

ifeq (_$(WITH_QTOPIA)_,_y_)
include $(UMAUIC)/.depend
endif 

endif	# i f e q (_$(UNAME)$(MACHINE)_,__)

.cache-$(shell uname -n):
	@(echo UNAME = $(shell uname|sed -e "s/HP\-UX/hpux/g;s/CYGWIN_NT-\([45]\).\([01]\)/cygwin/g"); \
	if [ "$(CROSS)" = "" ]; then echo MACHINE = $(shell uname -m|sed -e "s/0000[0-9A-F]*00/rs6000/g"); \
	if [ "$(shell uname)" != "Linux" ]; then echo KERNEL_VERSION = "$(shell uname -r)"; else 	\
	echo KERNEL_VERSION = $(shell grep UTS_RELEASE /usr/src/linux/include/linux/*.h|cut -d\" -f 2|head -1);	\
	fi; else echo MACHINE = strongarm; \
	echo KERNEL_VERSION = 2.4.18-rmk7-pxa3-embedix;	\
	fi	;\
	echo LIBC = $(shell ldd /bin/ls | grep libc.so | cut -f1 -d" " | cut -f3 -d"." | head -1); \
	) >.cache-$(shell uname -n)



ifeq (_$(SRCS)_,__)
built/.versions:
	@mkdir -p built ; touch built/.versions 
else	
SRCREGEX = $(shell echo `cat .sources `|sed 's/\\//g;s/^.*=[ ]*//g;s/  /|/g')
built/.versions: $(DEPEND)
	@mkdir -p built ; \
	(echo creating new ls_fulltime version list >&2 ; \
	    for i in $(SRCS)		; \
	    do echo "$$i" >&2                  	; \
		echo "$${i}.version=$${i}@@unknown,`ls --full-time $$i | awk '{print $$6"T"$$7"Z"}' | sed 's/-//g'`"; \
	    done; \
		echo "# end-of-versions created from ls --full-time"; \
		if test -f .svn/entries; \
		then \
			(echo creating new version list >&2 ; \
				sed 's/<//g;s/>//g;s/-//g;s/ //g;s/=/ /g;s/"//g' .svn/entries |	awk '{\
					if (oldfmt != 1) {\
						if (($$1 == "entry") && (NF == 1)) { printf("\n"); } else {\
						if ($$1 == "committeddate") { printf("%s.version=%s@@%s,%s\n",cname,cname,cversion,$$2); }\
						if ($$1 == "committedrev") { cversion=$$2 }\
						if ($$1 == "name") { cname=$$2 } } \
					} else {\
						n++;if (n == 7) {cdate = $$1}; if (n == 8) {cversion = $$1}; \
					} \
					if ((($$1 == "file") || ($$1 == "hasprops")) && (NF == 1)) {\
						printf("%s.version=%s@@%s,%s\n",cname,cname,cversion,cdate); \
						cname=lasts;	oldfmt = 1;	n = 0;\
					};\
					lasts=$$1; \
				}' | egrep "$(SRCREGEX)"; \
			);		\
		fi; \
		echo "# end-of-versions created from .svn/entries";	\
	) >built/.versions
endif

# debian package stuff, maintainers only
DEBRT = $(OPA)/debpackage

DEBSECTION = B
DEBPKG = $(shell echo $(TARGET) | sed "s/_//g;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/")

ifeq ($(MAKE_T),$(KMOD))
	# kernel module
	DEBSECTION = K
else
ifeq ($(MAKE_T),$(BINTARGET))
ifeq ($(SBIN_TARGET),_s_)
	# admin 
	DEBSECTION = M
endif
else
	DEBSECTION = L
	DEBPKG = $(shell echo lib$(TARGET) | sed "s/_//g;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/")
endif
endif

DEBUSR = $(DEBRT)/usr
DEBETC = $(DEBRT)/etc
DEBBIN = $(DEBRT)/usr/$(BINDIR_)
DEBLIB = $(DEBRT)/usr/lib
DEBSHARE = $(DEBUSR)/share/$(TARGET)
DEBINC = $(DEBUSR)/include/$(TARGET)
DEBDOC = $(DEBUSR)/share/doc/$(DEBPKG)
DEBMAN = $(DEBUSR)/share/man
DEBCTRL = $(DEBRT)/DEBIAN
DEBMOD = $(DEBRT)/$(KMODDIR)
DEBVERS = $(VERSION)-1

$(DEBRT):
	mkdir -m 755 -p ./$@

$(DEBMAN):
	mkdir -m 755 -p ./$@

$(DEBSHARE):
	mkdir -m 755 -p ./$@

$(DEBCTRL):
	mkdir -m 755 -p ./$@
	
$(DEBUSR):
	mkdir -m 755 -p ./$@

$(DEBBIN):
	mkdir -m 755 -p ./$@

ifeq (_$(DEBSECTION)_,_K_)
$(DEBMOD):
	mkdir -m 755 -p ./$@
endif

ifeq (_$(DEBSECTION)_,_L_)
$(DEBLIB):
	mkdir -m 755 -p ./$@

$(DEBINC):
	mkdir -m 755 -p ./$@
endif

$(DEBDOC):
	mkdir -m 755 -p ./$@

contrib/DEBIAN:
	mkdir -m 755 -p ./contrib/DEBIAN

contrib/DEBIAN/control:	debutils
contrib/DEBIAN/copyright:	debutils
contrib/DEBIAN/prerm:	debutils
contrib/DEBIAN/postinst:	debutils

ifeq (_$(isDAEMON)_,_d_)
$(DEBETC)/init.d:
	mkdir -m 755 -p ./$@

contrib/DEBIAN/initd_rc:	debutils

$(DEBETC)/init.d/$(TARGET):	contrib/DEBIAN/initd_rc $(DEBETC)/init.d
	cp contrib/DEBIAN/initd_rc $(DEBETC)/init.d/$(TARGET)
endif

debutils: contrib/DEBIAN
	mkDeb.sh -p $(DEBPKG) -t $(DEBSECTION)

contrib/DEBIAN/changelog: contrib/DEBIAN
	svn log -v >contrib/DEBIAN/changelog
	
contrib/DEBIAN/changelog.Debian: contrib/DEBIAN 
	@(echo "created by template Makefile";\
	svn ls -v Makefile) >contrib/DEBIAN/changelog.Debian

$(DEBCTRL)/p%: debutils $(DEBCTRL)
#	echo 1 $< 2 $@ 3 $*
	cp contrib/DEBIAN/p$* $@
 
debfiles: $(DEBCTRL)/postinst $(DEBCTRL)/prerm contrib/DEBIAN/changelog contrib/DEBIAN/changelog.Debian
	chmod 755 contrib/DEBIAN/prerm contrib/DEBIAN/postinst	 

ifeq ($(MAKE_T),$(KMOD))
	# kernel module	
install_deb: $(DEBMOD) $(O_KMOD)
	$(INSTALL) -m 755 $(O_KMOD) $(DEBMOD)
else
ifeq ($(MAKE_T),$(BINTARGET))
	# binary
DEBDEPEND = $(shell mk_shlibs.sh -o $(O_TARGET))

install_deb: $(O_TARGET) $(DEBBIN) $(DEBSHARE)
	$(STRIP) -v --strip-unneeded -R .comment $(O_TARGET) -o $(DEBBIN)/$(TARGET)
	chmod 755 $(DEBBIN)/$(TARGET)
	@(if test -d ./share ; then 	\
	  umask 022			;\
	  (cd ./share; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(DEBSHARE); $(TAR) -xf -) ;\
	fi)
else
	# library

#DEBDEPLIBS = $(shell ldd $(O_LIBNAME) | awk '{print $$1}')
#DEBDEPITEMS = $(shell (for i in $(DEBDEPLIBS); do dpkg -S $$i 2>/dev/null; done) | awk '{print $$1}' | sort -u | cut -f1 -d: )	
#DEBDEPEND = $(shell (for item in $(DEBDEPITEMS); do grep $$item /var/lib/dpkg/info/$$item.shlibs | awk '{for (j=3; j<=NF; j++) printf("%s ",$$j); printf(",\n");}' | sort -u; done) | awk '{printf("%s ",$$0)}' | sed "s/ ,/,/g;s/, $$//g")

## DEBDEPEND = $(shell	dpkg-shlibdeps -O $(O_LIBNAME) | cut -f2,3,4,5,6,7,8,9,10,11,12 -d=)

DEBDEPEND = $(shell mk_shlibs.sh -o $(O_LIBNAME))

install_deb: $(DEBLIB) $(DEBINC) $(DEBCTRL) $(O_LIBNAME)
	$(STRIP) -v --strip-unneeded -R .comment $(O_LIBNAME) -o $(DEBLIB)/$(LIBNAME)
#	@echo LIBS $(DEBDEPLIBS)
#	@echo ITEMS $(DEBDEPITEMS)
#	@echo DEPEND "$(DEBDEPEND)"
	@echo "lib$(TARGET) $(MAJOR) $(DEBDEPEND)" >$(DEBCTRL)/shlibs
	@(cd $(DEBLIB); ln -s $(LIBNAME) $(DEVLIBSYM); ln -s $(LIBNAME) $(SONAME); )
	@(SRCPATH=`pwd` ;\
	  cd $(DEBINC) ; \
	  for I in $(HDRS) ;\
	    do rm -f $$I ;\
	    $(INSTALL) -m 644 $$SRCPATH/$$I $$I ;\
	  done ) 		
endif
endif

debclean:
	rm -rf $(DEBRT)

DEBDESCHEAD = $(shell head -1 doc/description)
DEBDESCTAIL = $(shell wc doc/description |awk '{print $$1-1}')

DEBSIZE = $(shell du -s $(DEBRT) | awk '{print $$1}')
	
debcopy: $(DEBMAN) $(DEBDOC) htmldoc install_deb debfiles 
	@(if test -d ./man ; then 	\
	  (cd ./man; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(DEBMAN); $(TAR) -xf -) ;\
	  (cd $(DEBMAN); gzip -9 */*);\
	fi)
	@(if test -d ./doc ; then 	\
	  (cd ./doc; $(TAR) --exclude README --exclude description --exclude CVS --exclude .svn -cf - .)|(cd $(DEBDOC); $(TAR) -xf -) ;\
	fi)
	cp contrib/DEBIAN/copyright $(DEBDOC)
	@(cat doc/description; echo ""; cat doc/README) >$(DEBDOC)/README
	gzip -c9 <contrib/DEBIAN/changelog >$(DEBDOC)/changelog.gz
	gzip -c9 <contrib/DEBIAN/changelog.Debian >$(DEBDOC)/changelog.Debian.gz
	@(cd ./$(DEBRT); md5sum `find * -type f|egrep -v DEBIAN/`) >$(DEBCTRL)/md5sums

deb: debclean clean all debcopy
	@(echo "Package: $(DEBPKG)";\
	echo "Version: $(DEBVERS)";\
	echo "Depends: $(DEBDEPEND)";\
	echo "Installed-Size: $(DEBSIZE)";\
	sed "s/_ARCH_/$(MACHINE)/g;s/i[3456]86/i386/g;s/_DESC_/$(DEBDESCHEAD)/g" <contrib/DEBIAN/control;\
	tail -$(DEBDESCTAIL) doc/description) >$(DEBCTRL)/control	
	@(find $(DEBRT)/ -type d -empty -exec rmdir {} \; 2>/dev/null; echo "" >/dev/null; )
	chmod -R g-s $(DEBRT)
	fakeroot dpkg-deb --build $(DEBRT)
	lintian $(DEBRT).deb
	mv $(DEBRT).deb $(OPA)/$(DEBPKG)_$(DEBVERS)_$(MACHINE).deb && $(MAKE) debclean



