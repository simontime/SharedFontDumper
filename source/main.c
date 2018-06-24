#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#define SHAREDMEMFONT_SIZE 0x1100000


int main(int argc, char **argv) {
    Result rc;
    FILE *outFile;
    u8 *buffer, *sharedmem_addr;
    int kDown;

    gfxInitDefault();
    consoleInit(NULL);

    if (R_FAILED(rc = plInitialize())) {
        printf("plInitialize() failed\n0x%x\n", rc);
        return rc;
    }

    sharedmem_addr = (u8*)plGetSharedmemAddr();

    outFile = fopen("shared_font.bin", "wb");
    printf("Dumping shared font...\n");

    buffer = (u8*)calloc(SHAREDMEMFONT_SIZE, sizeof(u8));

    memcpy(buffer, sharedmem_addr, SHAREDMEMFONT_SIZE);
    if (fwrite(buffer, sizeof(u8), SHAREDMEMFONT_SIZE, outFile) != SHAREDMEMFONT_SIZE) {
        printf("Failed to write shared font.\n");
        free(buffer);
        fclose(outFile);
        gfxExit();
        plExit();
        return 1;
    }

    free(buffer);
    fclose(outFile);

    printf("Successfully dumped to shared_font.bin!\n\n\n\n");
    printf("Press + to exit");

    while (appletMainLoop()) {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if (kDown & KEY_PLUS) break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }
    
    gfxExit();
    plExit();
    return 0;
}
