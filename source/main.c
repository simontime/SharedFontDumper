#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#define MemSize 0x1100000
#define ErrStr "Failed to dump %s, please check your SD card storage space.\n"
#define SuccessStr "\n\nSuccessfully dumped shared fonts!\n\n\nPress + to exit."

int main() {
	int kDown;
	int Done = 0;
	Result rc;
	gfxInitDefault();
	consoleInit(NULL);

	FILE *outFile;
	u8 *buffer;
	PlFontData font;

	printf("Press A to dump fonts as ttf, or press X to dump the shared font memory.\n\n\n");

	void closeall(u8 *mem, FILE *out) {
		free(mem);
		fclose(out);
	}

	void clrfrmbuf() {
		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

    if (R_FAILED(rc = plInitialize())) {
        printf("plInitialize() failed\n0x%x\n", rc);
        return rc;
    }

	int DumpMem() {
		char *Name = "shared_font.bin";

		outFile = fopen(Name, "wb");

		printf("Dumping shared font memory...\n");

		buffer = malloc(MemSize * sizeof(u8));
		memcpy(buffer, plGetSharedmemAddr(), MemSize);

		if (fwrite(buffer, sizeof(u8), MemSize, outFile) != MemSize) {
			printf(ErrStr, Name);
			closeall(buffer, outFile);
			gfxExit();
			plExit();
			return 1;
		}

		closeall(buffer, outFile);
		clrfrmbuf();
		return 0;
	}

	int DumpFont(PlSharedFontType type, char *Name) {
		outFile = fopen(Name, "wb");

		printf("Dumping %s...\n", Name);

		plGetSharedFontByType(&font, type);

		u32 size = font.size;

		buffer = malloc(size * sizeof(u8));
		memcpy(buffer, font.address, size);

		if (fwrite(buffer, sizeof(u8), size, outFile) != size) {
			printf(ErrStr, Name);
			closeall(buffer, outFile);
			gfxExit();
			plExit();
			return 1;
		}

		closeall(buffer, outFile);
		clrfrmbuf();
		return 0;
	}

    while (appletMainLoop()) {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A && !Done) {
			DumpFont(PlSharedFontType_Standard, "FontStandard.ttf");
			DumpFont(PlSharedFontType_ChineseSimplified, "FontChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ExtChineseSimplified, "FontExtendedChineseSimplified.ttf");
			DumpFont(PlSharedFontType_ChineseTraditional, "FontChineseTraditional.ttf");
			DumpFont(PlSharedFontType_KO, "FontKorean.ttf");
			DumpFont(PlSharedFontType_NintendoExt, "FontNintendoExtended.ttf");
			printf(SuccessStr);
			Done++;
		}

		if (kDown & KEY_X && !Done) {
			DumpMem();
			printf(SuccessStr);
			Done++;
		}

        if (kDown & KEY_PLUS) break;

		clrfrmbuf();
    }
    
    gfxExit();
    plExit();
    return 0;
}
