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

MAKMAKE : RFterm_0xae7f53fa.hlp
RFterm_0xae7f53fa.hlp : RFterm.xml RFterm.cshlp Custom.xml
	cshlpcmp.bat RFterm.cshlp
ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
	md $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
	copy RFterm_0xae7f53fa.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del RFterm_0xae7f53fa.hlp
	del RFterm_0xae7f53fa.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo RFterm_0xae7f53fa.hlp

FINAL : do_nothing
