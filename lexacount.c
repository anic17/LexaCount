/*
    LexaCount - A lightweight command-line tool to count source lines of code.

    Description:
        LexaCount simplifies the process of analyzing and understanding the structure
        of source code in software projects. It provides various options to adapt to
        different needs, such as excluding lines with only brackets or using table output.

    Copyright (c) 2025 anic17 Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

#include "lexacount.h"

int in_string(char *s, int chr)
{
    return strrchr(s, chr) != NULL;
}

int strlen_n(const char *s)
{
    if (!s)
        return 0;
    return strlen(s);
}

void show_version()
{
    printf("LexaCount v%s - Count source lines of code.\n", lexacount_version);
}

void show_copyright()
{
    printf("Copyright (c) 2025 anic17 Software\n");
}

void help()
{
    show_version();
    printf(
        "Usage:\n"
        " lexacount [-b] [-t] [-l <list file>] [file1] [file2] ...\n\n"
        "Switches:\n"
        " -b, --bracket  Do not count lines containing only brackets or parenthesis\n"
        " -h, --help     Display this help menu\n"
        " -l, --list     Load file names from a file\n"
        " -t, --table    Print the output using tables\n"
        " -v, --version  Display program version\n"
        " -x, --exclude  Exclude files that failed to open\n\n"

    );
    show_copyright();
}

void missing_param(char *s)
{
    fprintf(stderr, "Error: Required parameter after '%s'. See 'lexacount --help' for more information.\n", s);
    exit(1);
}

size_t count_loc(char *filename, item_count *ic, text_fields *tf, param *p, int *result)
{
    ic->loc = 0;
    ic->comment = 0;
    ic->blank = 0;
    ic->bracket = 0;

    filename[strcspn(filename, "\r\n")] = '\0';
    if (filename[0] == '\0')
    {
        *result = -EINVAL;
        return 0;
    }

    FILE *count_fp = fopen(filename, "rb");
    if (!count_fp)
    {
        if (!p->excludeFiles)
            fprintf(stderr, "Error: %s: %s\n", filename, strerror(errno));
        *result = -errno;
        return 0;
    }
    bool isLineBlank = true;
    bool isLineOnlyBrackets = false;
    bool isFileEmpty = true;
    bool quoteStatus = false;
    size_t comment_idx = 0;

    size_t multi_comment_idx = 0;
    int chr = 0;

    size_t clen[3] = {strlen_n(tf->comments[0]), strlen_n(tf->comments[1]), strlen_n(tf->comments[2])};

    while ((chr = fgetc(count_fp)) != EOF)
    {

        isFileEmpty = false;
        if (in_string(tf->quotes, chr))
            quoteStatus ^= chr;

        if (isLineBlank && !quoteStatus)
        {
            if (comment_idx >= clen[0] - 1)
            {
                ic->comment++;
                comment_idx = 0;
                continue;
            }
            if (chr == tf->comments[0][comment_idx])
            {
                comment_idx++;
                continue;
            }
        }

        if ((!isspace(chr) || (!in_string(tf->brackets, chr) && p->excludeBrackets)))
        {
            if (in_string(tf->brackets, chr) && p->excludeBrackets)
            {
                isLineOnlyBrackets = true;
            }
            else
            {
                if (!isspace(chr))
                {
                    isLineBlank = false;
                    isLineOnlyBrackets = false;
                }
            }
        }
        if (chr == '\n')
        {
            if (isLineOnlyBrackets && isLineBlank)
                ic->bracket++;
            else if (isLineBlank)
                ic->blank++;

            ic->loc++;
            isLineBlank = true;
            isLineOnlyBrackets = false;
            quoteStatus = 0;
            comment_idx = 0;
        }
    }

    if (!isFileEmpty)
    {
        if (isLineOnlyBrackets && isLineBlank)
            ic->bracket++;
        else if (isLineBlank)
            ic->blank++;

        ic->loc++;
    }

    fclose(count_fp);

    *result = 1; // > 0 means success, == 0 means error
    return ic->loc;
}

void print_table_header(const char *formatspec, bool excludeBrackets)
{
    printf("| ");
    printf(formatspec, "File");
    printf(" | %-10s | %-10s | %-10s |", "Code lines", "Blank", "Comments");

    if (excludeBrackets)
        printf(" %-10s |", "Brackets");

    printf("\n");
}

void print_table_separator(size_t flen, param *p)
{
    printf("|");
    for (size_t k = 0; k < flen + 2; k++)
        putchar('-');

    printf("|------------|------------|------------|");

    if (p->excludeBrackets)
        printf("------------|");

    printf("\n");
}

void print_table_row(const char *formatspec, const char *filename, item_count *ic, param *p)
{
    printf("| ");
    printf(formatspec, filename);
    printf(" | %-10zu | %-10zu | %-10zu |", ic->loc - ic->blank - ic->comment - ic->bracket * (!!p->excludeBrackets), ic->blank, ic->comment);

    if (p->excludeBrackets)
        printf(" %-10zu |", ic->bracket);

    printf("\n");
}

void display_lines(const char *formatspec, const char *filename, item_count *ic, param *p)
{
    if (p->tableOutput)
        print_table_row(formatspec, filename, ic, p);
    else
    {
        printf("%s: %zu code lines", filename, ic->loc - ic->blank - ic->comment - ic->bracket * (!!p->excludeBrackets));

        if (ic->blank > 0 || ic->comment > 0 || ic->bracket > 0)
        {
            printf(" (and");

            if (ic->blank > 0)
                printf(" %zu blank line%s", ic->blank, (ic->blank == 1) ? "" : "s");

            if (ic->comment > 0)
                printf("%s%zu comment%s", (ic->blank > 0) ? " and " : " ", ic->comment, (ic->comment == 1) ? "" : "s");

            if (ic->bracket > 0 && p->excludeBrackets)
                printf("%s%zu line%s with only brackets", ((ic->blank > 0) || (ic->comment > 0)) ? " and " : " ", ic->bracket, (ic->bracket == 1) ? "" : "s");

            printf(")");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "/?") || !strcmp(argv[1], "-h"))
    {
        help();
        return 0;
    }
    char *loc_file = argv[1];
    FILE *list;

    char *comments[] = {"//", "/*", "*/"};

    char **fnames = calloc(argc - 1, sizeof(char *)), **list_fnames = calloc(argc - 1, sizeof(char *));
    size_t fname_count = 0, list_fname_count = 0;

    size_t longest_fname = 0;

    item_count ic = {0, 0, 0, 0}, total = {0, 0, 0, 0};

    text_fields tf = {comments, "{}[]();", "'\""};
    param p = {NULL, false, false};

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list"))
        {
            if (i + 1 < argc)
            {
                if (argv[i] != NULL)
                {
                    list_fnames[list_fname_count] = calloc(strlen_n(argv[i + 1]) + 1, sizeof(char));
                    memcpy(list_fnames[list_fname_count], argv[i + 1], strlen_n(argv[i + 1]));
                    if (strlen_n(list_fnames[list_fname_count]) > longest_fname)
                        longest_fname = strlen_n(list_fnames[list_fname_count]);

                    list_fname_count++;
                }
                i++;
            }
            else
            {
                missing_param(argv[i]);
            }
        }

        else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bracket"))
        {
            p.excludeBrackets = true;
        }
        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--table"))
        {
            p.tableOutput = true;
        }
        else if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--exclude"))
        {
            p.excludeFiles = true;
        }
        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
        {
            show_version();
            show_copyright();
            exit(0);
        }

        else
        {
            if (argv[i] != NULL)
            {
                fnames[fname_count] = calloc(strlen_n(argv[i]) + 1, sizeof(char));
                memcpy(fnames[fname_count], argv[i], strlen_n(argv[i]));
                if (strlen_n(fnames[fname_count]) > longest_fname)
                    longest_fname = strlen_n(fnames[fname_count]);

                fname_count++;
            }
        }
    }

    size_t count = 0;

    char filename[512];

    if (longest_fname < 11)
        longest_fname = 11;

    char *fspec = calloc(sizeof(char), longest_fname + 10);
    snprintf(fspec, longest_fname, "%%-%zus", longest_fname);

    int count_retcode = 0;

    for (size_t i = 0; i < list_fname_count; i++) // Load all filenames from all the list files into memory
    {
        list = fopen(list_fnames[i], "rb");
        if (!list)
        {
            if (!p.excludeFiles)
                fprintf(stderr, "Error: %s: %s", list_fnames[i], strerror(errno)); // List file error
            return -errno;
        }
        while (fgets(filename, sizeof filename, list)) // If someone knows a better way to avoid incorrect padding, let me know
        {
            if (strlen_n(filename) > longest_fname)
            {
                longest_fname = strlen_n(filename);
                snprintf(fspec, longest_fname, "%%-%zus", longest_fname);
            }
        }
        fclose(list);
    }

    if (p.tableOutput)
    {
        print_table_header(fspec, p.excludeBrackets);
        if (list_fname_count == 0)
            print_table_separator(longest_fname, &p);
    }

    for (size_t i = 0; i < list_fname_count; i++) // Process the filenames from the list files
    {
        list = fopen(list_fnames[i], "rb");
        if (!list)
        {
            if (!p.excludeFiles)
                fprintf(stderr, "Error: %s: %s", list_fnames[i], strerror(errno)); // Files inside list file error
            return errno;
        }
        if (p.tableOutput)
            print_table_separator(longest_fname, &p);

        while (fgets(filename, sizeof filename, list)) // Process each individual file from each list
        {

            count_loc(filename, &ic, &tf, &p, &count_retcode);
            if (count_retcode >= 0)
            {

                total.loc += ic.loc;
                total.comment += ic.comment;
                total.blank += ic.blank;
                total.bracket += ic.bracket;
                display_lines(fspec, filename, &ic, &p);
            }

            count++;
        }
        fclose(list);
    }

    int valid_file = 0;
    for (size_t i = 0; i < fname_count; i++) // Process normal files
    {
        if ((valid_file = count_loc(fnames[i], &ic, &tf, &p, &count_retcode)) >= 0)
            display_lines(fspec, fnames[i], &ic, &p);
        total.loc += ic.loc;
        total.comment += ic.comment;
        total.blank += ic.blank;
        total.bracket += ic.bracket;
        count++;
    }
    if (p.tableOutput)
        print_table_separator(longest_fname, &p);

    if (valid_file >= 0)
        display_lines(fspec, "Total lines", &total, &p);
    free(fspec);
    free(list_fnames);
    free(fnames);
    return 0;
}