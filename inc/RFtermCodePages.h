/*
 ============================================================================
 Name        : RFtermCodePages.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Characters from range 128-255 of different code pages.
 ============================================================================
 */

#ifndef RFTERMCODEPAGES_H
#define RFTERMCODEPAGES_H

const TUint KCodePageSize = 256;

_LIT(KCodePageLatin1,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/" ¡¢£¤¥¦§¨©ª«¬­®¯"
/*B*/"°±²³´µ¶·¸¹º»¼½¾¿"
/*C*/"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"
/*D*/"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
/*E*/"àáâãäåæçèéêëìíîï"
/*F*/"ðñòóôõö÷øùúûüýþÿ"
);

_LIT(KCodePageLatin2,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/" Ą˘Ł¤ĽŚ§¨ŠŞŤŹ­ŽŻ"
/*B*/"°ą˛ł´ľśˇ¸šşťź˝žż"
/*C*/"ŔÁÂĂÄĹĆÇČÉĘËĚÍÎĎ"
/*D*/"ĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢß"
/*E*/"ŕáâăäĺćçčéęëěíîď"
/*F*/"đńňóôőö÷řůúűüýţ˙"
);

_LIT(KCodePageLatin3,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/" Ħ˘£¤¥Ĥ§¨İŞĞĴ­®Ż"
/*B*/"°ħ²³´µĥ·¸ışğĵ½¾ż"
/*C*/"ÀÁÂ ÄĊĈÇÈÉÊËÌÍÎÏ"
/*D*/" ÑÒÓÔĠÖ×ĜÙÚÛÜŬŜß"
/*E*/"àáâ äċĉçèéêëìíîï"
/*F*/" ñòóôġö÷ĝùúûüŭŝ˙"
);

_LIT(KCodePageLatin4,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/" ĄĸŖ¤ĨĻ§¨ŠĒĢŦ­Ž¯"
/*B*/"°ą˛ŗ´ĩļˇ¸šēģŧŊžŋ"
/*C*/"ĀÁÂÃÄÅÆĮČÉĘËĖÍÎĪ"
/*D*/"ĐŅŌĶÔÕÖ×ØŲÚÛÜŨŪß"
/*E*/"āáâãäåæįčéęëėíîī"
/*F*/"đņōķôõö÷øųúûüũū˙"
);

_LIT(KCodePageLatinCyrillic,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/" ЁЂЃЄЅІЇЈЉЊЋЌ ЎЏ"
/*B*/"АБВГДЕЖЗИЙКЛМНОП"
/*C*/"РСТУФХЦЧШЩЪЫЬЭЮЯ"
/*D*/"абвгдежзийклмноп"
/*E*/"рстуфхцчшщъыьэюя"
/*F*/"№ёђѓєѕіїјљњћќ§ўџ"
);

_LIT(KCodePage1251,
/*    0123456789ABCDEF*/
/*8*/"ЂЃ‚ѓ„…†‡€‰Љ‹ЊЌЋЏ"
/*9*/"ђ‘’“”•–—™ љ›њќћџ"
/*A*/" ЎўЈ¤Ґ¦§Ё©Є«¬­®Ї"
/*B*/"°±Ііґµ¶·ё№є»јЅѕї"
/*C*/"АБВГДЕЖЗИЙКЛМНОП"
/*D*/"РСТУФХЦЧШЩЪЫЬЭЮЯ"
/*E*/"абвгдежзийклмноп"
/*F*/"рстуфхцчшщъыьэюя"
);

_LIT(KCodePage1252,
/*    0123456789ABCDEF*/
/*8*/"€ ‚ƒ„…†‡ˆ‰Š‹Œ Ž "
/*9*/" ‘’“”•–—˜™š›œ žŸ"
/*A*/" ¡¢£¤¥¦§¨©ª«¬­®¯"
/*B*/"°±²³´µ¶·¸¹º»¼½¾¿"
/*C*/"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"
/*D*/"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
/*E*/"àáâãäåæçèéêëìíîï"
/*F*/"ðñòóôõö÷øùúûüýþÿ"
);

_LIT(KCodePageKOI8,
/*    0123456789ABCDEF*/
/*8*/"                "
/*9*/"                "
/*A*/"                "
/*B*/"                "
/*C*/"юабцдефгхийклмно"
/*D*/"пярстужвьызшэщчъ"
/*E*/"ЮАБЦДЕФГХИЙКЛМНО"
/*F*/"ПЯРСТУЖВЬЫЗШЭЩЧ "
);


#endif /* RFTERMCODEPAGES_H */