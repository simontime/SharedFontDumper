#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
const u64 SHARED_MEMORY_SIZE = 0x1100000;

int main(int argc, char **argv)
{
  Result rc;
  FILE *outFile;
  u8 *buffer, *sharedmem_addr;

  gfxInitDefault();
  consoleInit(NULL);
  if (R_FAILED(rc = plInitialize())) {
    printf("plInitialize() failed\n0x%x\n", rc);
    return rc;
  }

  sharedmem_addr = (u8*)plGetSharedmemAddr();

  outFile = fopen ("shared_font.bin", "wb");
  printf("\nDumping shared font...\n");

  buffer = (u8*) calloc(SHARED_MEMORY_SIZE, sizeof(u8));
  memcpy (buffer, sharedmem_addr, SHARED_MEMORY_SIZE);
  if (fwrite(buffer, sizeof(u8), SHARED_MEMORY_SIZE, outFile) != SHARED_MEMORY_SIZE) {
          printf("\nFailed to write shared font.\n");
          fclose (outFile);
          gfxExit();
          plExit();
          return false;
  }
  fclose(outFile);
  
  printf("\nSuccessfully dumped to shared_font.bin!\n");
  printf("\n\n\nPress + to exit");
  int kDown;
  while(appletMainLoop()) {
    hidScanInput();
    kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    if (kDown & KEY_PLUS) break;
    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();
  }
  printf("exiting...");
  gfxExit();
  plExit();
  return 0;
}