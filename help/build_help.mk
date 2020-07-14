# ============================================================================
#  Name	 : build_help.mk
#  Part of  : RFterm
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : RFterm
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE :
	cshlpcmp.bat RFterm_01.cshlp
	cshlpcmp.bat RFterm_16.cshlp
	cshlpcmp.bat RFterm_93.cshlp
ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
	md $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help & \
	copy out\RFterm_0xae7f53fa.h01 $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help & \
	copy out\RFterm_0xae7f53fa.h16 $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help & \
	copy out\RFterm_0xae7f53fa.h93 $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del out\RFterm_0xae7f53fa.h01
	del out\RFterm_0xae7f53fa.h01.hrh
	del out\RFterm_0xae7f53fa.h16
	del out\RFterm_0xae7f53fa.h16.hrh
	del out\RFterm_0xae7f53fa.h93
	del out\RFterm_0xae7f53fa.h93.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo RFterm_0xae7f53fa.h01
	@echo RFterm_0xae7f53fa.h16
	@echo RFterm_0xae7f53fa.h93

FINAL : do_nothing
