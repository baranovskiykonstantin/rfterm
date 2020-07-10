# ============================================================================
#  Name	 : Icons_scalable_dc.mk
#  Part of  : RFterm
#
#  Description: This is file for creating .mif file (scalable icon)
# 
# ============================================================================


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\RFterm_0xae7f53fa.mif

HEADERDIR=$(EPOCROOT)epoc32\include
HEADERFILENAME=$(HEADERDIR)\RFterm_0xae7f53fa.mbg

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	@echo ...Deleting $(ICONTARGETFILENAME)
	del /q /f $(ICONTARGETFILENAME)
	@echo ...Deleting $(HEADERFILENAME)
	del /q /f $(HEADERFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE: if you have JUSTINTIME enabled for your S60 3rd FP1 or newer SDK
# and this command crashes, consider adding "/X" to the command line.
# See <http://forum.nokia.com/document/Forum_Nokia_Technical_Library_v1_35/contents/FNTL/Build_process_fails_at_mif_file_creation_in_S60_3rd_Ed_FP1_SDK.htm>
# ----------------------------------------------------------------------------

RESOURCE : $(ICONTARGETFILENAME)

# Animated flag ( /A ) must be set otherwise the icon will be shown a bit smaller!
$(ICONTARGETFILENAME) : $(ICONDIR)\qgn_menu_RFterm.svg
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/A /c32 $(ICONDIR)\qgn_menu_RFterm.svg \
		/c32,8 $(ICONDIR)\signal_off.svg \
		/c32,8 $(ICONDIR)\signal_on.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

