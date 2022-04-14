// Hadas Babayov 322807629


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 *
 * @return 1 - big endian, 0 - little endian.
 */
int is_big_endian(unsigned char first, unsigned char second) {
    if (first == 0xfe && second == 0xff) {
        return 1;
    }
    return 0;
}

/**
 * Swap between the two bytes of the buffer.
 *
 * @param buffer
 */
void swap(char *buffer) {
    char temp;
    temp = buffer[0];
    buffer[0] = buffer[1];
    buffer[1] = temp;
}

/**
 * The function copies the source file to a new destination file.
 *
 * @param inputName Name of source file.
 * @param outputName Name of destination file.
 * @param needToSwap Swap the bytes or not.
 */
void option1(char *inputName, char *outputName, int needToSwap) {
    FILE *srcFile = fopen(inputName, "rb");
    char buffer[2];
    int i;
    if (srcFile != NULL) {
        FILE *destFile = fopen(outputName, "wb");
        // copy the source file to the destination.
        if (destFile != NULL) {
            do {
                // read one character from src file.
                i = fread(buffer, 1, sizeof(buffer), srcFile);
                // swap the bytes if we need.
                if (needToSwap == 1) {
                    swap(buffer);
                }
                // write the character to dst file.
                fwrite(buffer, 1, i, destFile);
            } while (i != 0);
            fclose(srcFile);
            fclose(destFile);
        }
    }
}

/**
 * Create a new file that is encoded according to the second flag.
 * The content is the same as the source file.
 * We will perform a swap if necessary.
 *
 * @param src Name of source file.
 * @param new Name of destination file.
 * @param srcFlag Source file operating system
 * @param newFlag Destination file operating system
 * @param needToSwap Swap the bytes or not.
 */
void option2(char *src, char *new, char *srcFlag, char *newFlag, int needToSwap) {
    // If the flags are the same, we'll copy without changing the encoding.
    if (strcmp(srcFlag, newFlag) == 0) {
        option1(src, new, needToSwap);
        return;
    }

    FILE *srcFile = fopen(src, "rb");
    if (srcFile != NULL) {
        char readBuffer[2];
        int i, isBigEndian;
        // Check the BOM, swap (if necessary) and write it to destination file.
        i = fread(readBuffer, 1, sizeof(readBuffer), srcFile);
        isBigEndian = is_big_endian(readBuffer[0], readBuffer[1]);
        FILE *destFile = fopen(new, "wb");
        if (needToSwap == 1) {
            swap(readBuffer);
        }
        fwrite(readBuffer, 1, i, destFile);
        // We'll read a character and write to the destination file according to the flags.
        if (destFile != NULL) {
            i = fread(readBuffer, 1, sizeof(readBuffer), srcFile);
            while (i != 0) {
                // We'll change it if we write to the destination file.
                int bol = 0;
                // We'll change it if we want to save the next character to the next iteration.
                int saveBuf = 0;
                // big endian
                if (isBigEndian == 1) {
                    // source file encoded by mac
                    if (strcmp(srcFlag, "-mac") == 0 && readBuffer[1] == '\r' && readBuffer[0] == 0) {
                        // destination file encoded by unix
                        if (strcmp(newFlag, "-unix") == 0) {
                            readBuffer[1] = '\n';
                        }
                        // destination file encoded by windows
                        if (strcmp(newFlag, "-win") == 0) {
                            bol = 1;
                            if (needToSwap == 0) {
                                char winBuf[4] = {0, '\r', 0, '\n'};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            if (needToSwap == 1) {
                                char winBuf[4] = {'\r', 0, '\n', 0};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                        }
                    }
                    // source file encoded by windows
                    if (strcmp(srcFlag, "-win") == 0 && readBuffer[1] == '\r' && readBuffer[0] == 0) {
                        char winBuf[2];
                        int check = fread(winBuf, 1, sizeof(winBuf), srcFile);
                        if (check != 0 && winBuf[1] == '\n' && winBuf[0] == 0) {
                            // destination file encoded by unix
                            if (strcmp(newFlag, "-unix") == 0) {
                                bol = 1;
                                if (needToSwap == 1) {
                                    winBuf[0] = '\n';
                                    winBuf[1] = 0;
                                }
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            // destination file encoded by mac
                            if (strcmp(newFlag, "-mac") == 0) {
                                bol = 1;
                                if (needToSwap == 1) {
                                    readBuffer[0] = '\r';
                                    readBuffer[1] = 0;
                                }
                                fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            }
                        } else {
                            saveBuf = 1;
                            if (needToSwap == 1){
                                swap(readBuffer);
                            }
                            fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            readBuffer[0] = winBuf[0];
                            readBuffer[1] = winBuf[1];
                        }
                    }
                    // source file encoded by unix
                    if (strcmp(srcFlag, "-unix") == 0 && readBuffer[1] == '\n' && readBuffer[0] == 0) {
                        // destination file encoded by mac
                        if (strcmp(newFlag, "-mac") == 0) {
                            readBuffer[1] = '\r';
                        }
                        // destination file encoded by windows
                        if (strcmp(newFlag, "-win") == 0) {
                            bol = 1;
                            readBuffer[1] = '\r';
                            if (needToSwap == 1) {
                                readBuffer[0] = '\r';
                                readBuffer[1] = 0;
                            }
                            fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            if (needToSwap == 0) {
                                char winBuf[2] = {0, '\n'};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            if (needToSwap == 1) {
                                char winBuf[2] = {'\n', 0};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                        }
                    }
                }
                // little endian
                if (isBigEndian == 0) {
                    // source file encoded by mac
                    if (strcmp(srcFlag, "-mac") == 0 && readBuffer[0] == '\r' && readBuffer[1] == 0) {
                        // destination file encoded by unix
                        if (strcmp(newFlag, "-unix") == 0) {
                            readBuffer[0] = '\n';
                        }
                        // destination file encoded by windows
                        if (strcmp(newFlag, "-win") == 0) {
                            bol = 1;
                            if (needToSwap == 0) {
                                char winBuf[4] = {'\r', 0, '\n', 0};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            if (needToSwap == 1) {
                                char winBuf[4] = {0, '\r', 0, '\n'};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                        }
                    }
                    // source file encoded by windows
                    if (strcmp(srcFlag, "-win") == 0 && readBuffer[0] == '\r' && readBuffer[1] == 0) {
                        char winBuf[2];
                        int check = fread(winBuf, 1, sizeof(winBuf), srcFile);
                        if (check != 0 && winBuf[0] == '\n' && winBuf[1] == 0) {
                            // destination file encoded by unix
                            if (strcmp(newFlag, "-unix") == 0) {
                                bol = 1;
                                if (needToSwap == 1) {
                                    winBuf[1] = '\n';
                                    winBuf[0] = 0;
                                }
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            // destination file encoded by mac
                            if (strcmp(newFlag, "-mac") == 0) {
                                bol = 1;
                                if (needToSwap == 1) {
                                    readBuffer[0] = 0;
                                    readBuffer[1] = '\r';
                                }
                                fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            }
                        } else {
                            saveBuf = 1;
                            if (needToSwap == 1){
                                swap(readBuffer);
                            }
                            fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            readBuffer[0] = winBuf[0];
                            readBuffer[1] = winBuf[1];
                        }
                    }
                    // source file encoded by unix
                    if (strcmp(srcFlag, "-unix") == 0 && readBuffer[0] == '\n' && readBuffer[1] == 0) {
                        // destination file encoded by mac
                        if (strcmp(newFlag, "-mac") == 0) {
                            readBuffer[0] = '\r';
                        }
                        // destination file encoded by win
                        if (strcmp(newFlag, "-win") == 0) {
                            bol = 1;
                            readBuffer[0] = '\r';
                            if (needToSwap == 1) {
                                readBuffer[1] = '\r';
                                readBuffer[0] = 0;
                            }
                            fwrite(readBuffer, 1, sizeof(readBuffer), destFile);
                            if (needToSwap == 1) {
                                char winBuf[2] = {0, '\n'};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                            if (needToSwap == 0) {
                                char winBuf[2] = {'\n', 0};
                                fwrite(winBuf, 1, sizeof(winBuf), destFile);
                            }
                        }
                    }
                }
                // Save the window buffer to the next iteration.
                if (saveBuf == 1) {
                    continue;
                }
                // Swap the bytes if necessary.
                if (needToSwap == 1) {
                    swap(readBuffer);
                }
                // If we write to the destination file, we read another character and continue to check it.
                if (bol == 1) {
                    i = fread(readBuffer, 1, sizeof(readBuffer), srcFile);
                    continue;
                }
                fwrite(readBuffer, 1, i, destFile);
                i = fread(readBuffer, 1, sizeof(readBuffer), srcFile);
            }
        }
        // close the files.
        fclose(srcFile);
        fclose(destFile);
    }
}

/**
 * Check the flag (keep \ swap) - according to the flag, the arguments are sent to option2.
 *
 * @param srcName of source file.
 * @param new Name of destination file.
 * @param srcFlag Source file operating system
 * @param newFlag Destination file operating system.
 * @param flag keep-> don't swap the bytes, swap -> swap the bytes.
 */
void option3(char *src, char *new, char *srcFlag, char *newFlag, char *flag) {
    // Check if the input is valid.
    if (strcmp(srcFlag, "-win") != 0 && strcmp(srcFlag, "-mac") != 0 && strcmp(srcFlag, "-unix") != 0) {
        return;
    }
    if (strcmp(newFlag, "-win") != 0 && strcmp(newFlag, "-mac") != 0 && strcmp(newFlag, "-unix") != 0) {
        return;
    }

    if (strcmp(flag, "-keep") == 0) {
        option2(src, new, srcFlag, newFlag, 0);
    }
    if (strcmp(flag, "-swap") == 0) {
        option2(src, new, srcFlag, newFlag, 1);
    }
}

/**
 * Create a new file according to information received in the arguments.
 *
 * @param argc size of argv.
 * @param argv array of strings.
 * @return 0
 */
int main(int argc, char *argv[]) {
    
    if (argc == 3) {
        option1(argv[1], argv[2], 0);
    }
    if (argc == 5) {
        // Check if the input is valid.
        if (strcmp(argv[3], "-win") != 0 && strcmp(argv[3], "-mac") != 0 && strcmp(argv[3], "-unix") != 0) {
            return 0;
        }
        if (strcmp(argv[4], "-win") != 0 && strcmp(argv[4], "-mac") != 0 && strcmp(argv[4], "-unix") != 0) {
            return 0;
        }
        option2(argv[1], argv[2], argv[3], argv[4], 0);
    }
    if (argc == 6) {
        option3(argv[1], argv[2], argv[3], argv[4], argv[5]);
    }
    return 0;
}

