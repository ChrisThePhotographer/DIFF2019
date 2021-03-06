//
//  main.c
//  diff
//
//  Created by Chris Ly on 4/25/19.
//  Copyright © 2019 Chris Ly. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "para.h"
#include "util.h"
#include "diff.h"

int dnormal(const char* filename1, const char* filename2) {
    if (*filename1 == *filename2) { exit(0); }else{
    }
    return 0;
}

/*
int sideside(para* p, para* q) {
    if (para_equal(p, q)) { para_print(p, printboth); return 0; }
    para_print(q, printright);
    
    while ((q = para_next(q)) != NULL && !para_equal(p,q)){ equal = para_equal(p, q); para_print(q, equal ? printboth : printright); }
    para_print((p = para_next(p)), printleft);
    
    return 0;
}
*/

int sideside(para* p, para* q) {
    para* parp = q;
    while (p != NULL) {
        parp = q;
        int match = 0;
        while (q != NULL && (match = para_equal(p, q)) == 0) { q = para_next(q); }
        q = parp;
        if (match) {
            while ((match = para_equal(p, q)) == 0) {
                para_print(q, printright);
                q = para_next(q);
                parp = q;
            }
            para_print(q, printboth);
            p = para_next(p);
            q = para_next(q);
        } else {
            para_print(p, printleft);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        para_print(q, printright);
        q = para_next(q);
    }
    return 0;
};

void brief(para* p, para* q) { if (para_equal(p, q) == 0) { printf("The files are identical.\n"); } else { return; } }

void identical(para* p, para* q) {
    if (para_equal(p, q) != 0) { printf("The files are identical.\n"); }
    else { dnormal(files[0], files[1]); }
}

void version(void) {
    printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
    printf("Copyright (C) 2014 CSUF\n");
    printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
    printf("You may redistribute copies of this program\n");
    printf("under the terms of the GNU General Public License.\n");
    printf("For more information about these matters, see the file named COPYING.\n");
    printf("Written by Chris Ly with the Help of William McCarthy\n");
}

void todo(void) {
    printf("\nTODO: check line by line in a pagraph, using '|' for differences");
    printf("\nTODO: this starter code does not yet handle printing all of fin1's pagraphs.");
    printf("\nTODO: handle the rest of diff's options");
    //printf("\nTODO: fix standard printing with no parameters");
    //printf("\nTODO: implement multiple types of parameters\n");
}

void loadfiles(const char* filename1, const char* filename2) {
    memset(buf, 0, sizeof(buf));
    memset(strings1, 0, sizeof(strings1));
    memset(strings2, 0, sizeof(strings2));
    
    FILE *fin1 = openfile(filename1, "r");
    FILE *fin2 = openfile(filename2, "r");
    
    while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
    while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
}

void print_option(const char* name, int value) { printf("%17s: %d\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
    fprintf(stderr, "diff: conflicting output style options\n");
    fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
    exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value){
    if ((strcmp(arg,s)==0) || ((t != NULL && strcmp(arg,t)==0))){
        *value =1;
    }
}

void showoptions(const char* file1, const char* file2) {
    printf("diff options...\n");
    print_option("diffnormal", diffnormal);
    print_option("show_version", showversion);
    print_option("show_brief", showbrief);
    print_option("ignorecase", ignorecase);
    print_option("report_identical", report_identical);
    print_option("show_sidebyside", showsidebyside);
    print_option("show_leftcolumn", showleftcolumn);
    print_option("suppresscommon", suppresscommon);
    print_option("showcontext", showcontext);
    print_option("show_unified", showunified);
    
    printf("file1: %s,  file2: %s\n\n\n", file1, file2);
    
    printline();
}

void init_options_files(int argc, const char* argv[]) {
    int cnt = 0;
    const char* files[2] = { NULL, NULL };
    
    while (argc-- > 0) {
        const char* arg = *argv;
        setoption(arg, "-v",       "--version",                  &showversion);
        setoption(arg, "-q",       "--brief",                    &showbrief);
        setoption(arg, "-i",       "--ignore-case",              &ignorecase);
        setoption(arg, "-s",       "--report-identical-files",   &report_identical);
        setoption(arg, "--normal", NULL,                         &diffnormal);
        setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
        setoption(arg, "--left-column", NULL,                    &showleftcolumn);
        setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
        setoption(arg, "-c",       "--context",                  &showcontext);
        setoption(arg, "-u",       "showunified",                &showunified);
        if (arg[0] != '-') {
            if (cnt == 2) {
                fprintf(stderr, "apologies, this version of diff only handles two files\n");
                fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
                exit(TOOMANYFILES_ERROR);
            } else { files[cnt++] = arg; }
        }
        ++argv;   // DEBUG only;  move increment up to top of switch at release
    }
    
    if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
        diffnormal = 1;
    }
    
    if (showversion) { version();  exit(0); }
    
    if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
        (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {
        
        diff_output_conflict_error();
    }
    
    //  showoptions(files[0], files[1]);
    loadfiles(files[0], files[1]);
}

void end(FILE* fin1, FILE* fin2) {
    fclose(fin1);
    fclose(fin2);
}

int hash(const char* s) {
    unsigned long code = 0;
    while (*s != '\0') { code = code *31 + *s++; }
    return code % HASHLEN;
}

int main(int argc, const char * argv[]) {
    init_options_files(--argc, ++argv);
    //loadfiles(files[0], files[1]);
    
    //  para_printfile(strings1, count1, printleft);
    //  para_printfile(strings2, count2, printright);
    
    para* p = para_first(strings1, count1);
    para* q = para_first(strings2, count2);
    int foundmatch = 0;
    
    para* qlast = q;
    while (p != NULL) {
        qlast = q;
        foundmatch = 0;
        while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
            q = para_next(q);
        }
        q = qlast;
        
        if (foundmatch) {
            while ((foundmatch = para_equal(p, q)) == 0) {
                para_print(q, printright);
                q = para_next(q);
                qlast = q;
            }
            para_print(q, printboth);
            p = para_next(p);
            q = para_next(q);
        } else {
            para_print(p, printleft);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        para_print(q, printright);
        q = para_next(q);
    }
    
    return 0;
}

