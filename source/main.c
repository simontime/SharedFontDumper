#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <switch.h>

#define MEMSIZE 0x1100000
#define DIRNAME "fonts"
#define SUCCESS "\n\nSuccessfully dumped shared fonts!\n\n\n\nPress + to exit."
#define IOERROR "Error: Failed to open file for writing.\n"

int DumpMem(void) {
	FILE *outFile;
	u8 *buf = malloc(MEMSIZE);

	if (!(outFile = fopen("fonts/shared_font.bin", "wb"))) {
		fputs(IOERROR, stderr);
		goto exit;
	}

	puts("\nDumping shared font memory...\n");

	memcpy(buf, plGetSharedmemAddr(), MEMSIZE);

	fwrite(buf, MEMSIZE, 1, outFile);
	free(buf);
	
exit:
	fclose(outFile);
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	gfxWaitForVsync();

	return 0;
}

int DumpFont(PlSharedFontType type, char *name) {
	FILE *outFile;
	char *newName = malloc(strlen(DIRNAME) + strlen(name) + 1);
	
	PlFontData font;

	sprintf(newName, "%s/%s", DIRNAME, name);
	printf("Dumping %s...\n", name);
			
	if (!(outFile = fopen(newName, "wb"))) {
		fputs(IOERROR, stderr);
		goto exit;
	}
		
	plGetSharedFontByType(&font, type);

	u8 *buf = malloc(font.size);
	
	memcpy(buf, font.address, font.size);

	fwrite(buf, font.size, 1, outFile);
	free(buf);
	
exit:
	fclose(outFile);
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	gfxWaitForVsync();

	return 0;
}

int main(void) {
	int kDown, isDone = 0;
	struct stat *sb = memalign(0x1000, sizeof *sb);
	
	gfxInitDefault();
	consoleInit(NULL);
	plInitialize();

	puts("Press A to dump fonts as ttf, or press X to dump the shared font memory.\n\n");
	
	if (stat(DIRNAME, sb))
		mkdir(DIRNAME, 0777);

    while (appletMainLoop()) {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A && !isDone++) {
			DumpFont(PlSharedFontType_Standard,		"FontStandard.ttf");
			DumpFont(PlSharedFontType_ChineseSimplified, 	"FontChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ExtChineseSimplified, "FontExtendedChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ChineseTraditional, 	"FontChineseTraditional.ttf");
			DumpFont(PlSharedFontType_KO,			"FontKorean.ttf");
			DumpFont(PlSharedFontType_NintendoExt,		"FontNintendoExtended.ttf");
			puts(SUCCESS);
		}

		if (kDown & KEY_X && !isDone++) {
			DumpMem();
			puts(SUCCESS);
		}

        if (kDown & KEY_PLUS) break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
    }
    
    gfxExit();
    plExit();
    return 0;
}
