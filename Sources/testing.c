#include <windows.h>
#include <stdio.h>
#include <unistd.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

int main() {
    int c, result = 0;
    result = PlaySound(TEXT("Battle.wav"), NULL, SND_ASYNC|SND_LOOP);
    printf("%d", result);
    while ((c = getchar()) != EOF) {
        putc(c, stdout);
        if (c == 'a')
            PlaySound(NULL, NULL, 0);
    }
    sleep(15);
}