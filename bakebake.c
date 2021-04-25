/*
 * Simulation of the miserable MOJIBAKE filenames on Windows operating system,
 * when ZIP archive files created on some particular OS are unzipped.
 * 
 * This program will at least TERRIBLY and IRREVERSIBLY CORRUPT the names of
 * all the files in the current directory. More bad things could happen as well.
 * NEVER RUN THIS PROGURAM unless a TOTAL MESS is what you really want.
 *
 * Public Domain.
 */

#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> 
#include <iconv.h>


int main(int argc, char** argv)
{
    iconv_t ic;
    DIR *d;
    struct dirent *dir;
    struct stat sb;
    char bakebake[512];
    char *inptr, *outptr;
    ssize_t inlength;
    size_t outlength;
    size_t convlength;

    ic = iconv_open("UTF-8", "WINDOWS-31J");

    d = opendir(".");
    if (d != NULL) {
        while ((dir = readdir(d))) {
            stat(dir->d_name, &sb);
            if (S_ISREG(sb.st_mode)) {
                inptr = dir->d_name;
                outptr = bakebake;
                inlength = strlen(inptr);
                outlength = sizeof(bakebake) - 1;

                while (inlength > 0) {
                    convlength = 2;
                    iconv(ic, &inptr, &convlength, &outptr, &outlength);
                    switch (convlength) {
                    case 2:
                        /* Invalid code. Emit U+30FB */
                        inptr += 2;
                        inlength -= 2;
                        if (outlength >= 3) {
                            outlength -= 3;
                            *outptr++ = 0xe3;
                            *outptr++ = 0x83;
                            *outptr++ = 0xbb;
                        }
                        break;
                    case 1:     
                        /* Only the first octet was acceptable. */
                        /* Continue next conversion from the second octet. */
                        inlength -= 1;
                        break;
                    case 0:
                        /* Both octets were accepted as Valid SJIS sequence. */
                        inlength -= 2;
                        break;                    
                    }
                }
                *outptr++ = '\0';

                rename(dir->d_name, bakebake);
                printf("rename %s -> %s\n", dir->d_name, bakebake);
            }
        }
    }

    closedir(d);
    iconv_close(ic);
    return(0);
}