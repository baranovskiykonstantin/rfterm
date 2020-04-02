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
	"                "
	"                "
	" ¡¢£¤¥¦§¨©ª«¬­®¯"
	"°±²³´µ¶·¸¹º»¼½¾¿"
	"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"
	"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
	"àáâãäåæçèéêëìíîï"
	"ðñòóôõö÷øùúûüýþÿ"
);

_LIT(KCodePageLatinCyrillic,
	"                "
	"                "
	" ЁЂЃЄЅІЇЈЉЊЋЌ ЎЏ"
	"АБВГДЕЖЗИЙКЛМНОП"
	"РСТУФХЦЧШЩЪЫЬЭЮЯ"
	"абвгдежзийклмноп"
	"рстуфхцчшщъыьэюя"
	"№ёђѓєѕіїјљњћќ§ўџ"
);

_LIT(KCodePage1252,
	"€ ‚ƒ„…†‡ˆ‰Š‹Œ Ž "
	" ‘’“”•–—˜™š›œ žŸ"
	" ¡¢£¤¥¦§¨©ª«¬­®¯"
	"°±²³´µ¶·¸¹º»¼½¾¿"
	"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"
	"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
	"àáâãäåæçèéêëìíîï"
	"ðñòóôõö÷øùúûüýþÿ"
);

#endif /* RFTERMCODEPAGES_H */
