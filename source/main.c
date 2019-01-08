#include "text.h"

void DumpMem() {
	FILE *outFile;
	u8 *buf = malloc(MEMSIZE);

	if (!(outFile = fopen("fonts/shared_font.bin", "wb"))) {
		draw_text(0x10, 0x60, RED, IOERROR);
		goto exit;
	}

	draw_text(0x10, 0x60, SKYBLUE, "Dumping shared font memory...");
	text_update();

	memcpy(buf, plGetSharedmemAddr(), MEMSIZE);

	fwrite(buf, sizeof(u8), MEMSIZE, outFile);
	free(buf);
	
exit:
	fclose(outFile);
}

int Dumped = 0;

void DumpFont(PlSharedFontType type, char *name) {
	FILE *outFile;

	char *newName = malloc(strlen(DIRNAME) + strlen(name) + 2),
		 *str 	  = malloc(strlen(name) + 0x10);
	
	PlFontData font;

	sprintf(newName, "%s/%s", DIRNAME, name);

	if (!(outFile = fopen(newName, "wb"))) {
		draw_text(0x10, 0x60, RED, IOERROR);
		goto exit;
	}

	sprintf(str, "Dumping %s...\n", name);

	draw_text(0x10, 0x60 + 0x20 * Dumped++, SKYBLUE, str);
	text_update();

	plGetSharedFontByType(&font, type);

	u8 *buf = malloc(font.size);
	
	memcpy(buf, font.address, font.size);

	fwrite(buf, sizeof(u8), font.size, outFile);
	free(buf);
	
exit:
	free(newName);
	free(str);
	fclose(outFile);
}

void main() {
	int kDown, isDone = 0;
	struct stat *sb = memalign(0x1000, sizeof *sb);
	
	plInitialize();	
	text_init();

	if (stat(DIRNAME, sb))
		mkdir(DIRNAME, 0777);

	draw_text(0x10, 0x20, WHITE, "Press A to dump fonts as ttf, or press X to dump the shared font memory.");
	text_update();

    while (appletMainLoop()) {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A && !isDone++) {
			DumpFont(PlSharedFontType_Standard, 			"FontStandard.ttf");
			DumpFont(PlSharedFontType_ChineseSimplified, 	"FontChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ExtChineseSimplified, "FontExtendedChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ChineseTraditional, 	"FontChineseTraditional.ttf");
			DumpFont(PlSharedFontType_KO, 					"FontKorean.ttf");
			DumpFont(PlSharedFontType_NintendoExt, 			"FontNintendoExtended.ttf");
			draw_text(0x10, 0x80 + 0x20 * Dumped, GREEN, SUCCESS);
		}

		if (kDown & KEY_X && !isDone++) {
			DumpMem();
			draw_text(0x10, 0xa0, GREEN, SUCCESS);
		}

        if (kDown & KEY_PLUS) break;

		text_update();
    }

	free(sb);

	text_exit();
    plExit();
}
