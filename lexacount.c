/*
    LexaCount - A lightweight command-line tool to count source lines of code.

    Description:
        LexaCount simplifies the process of analyzing and understanding the structure
        of source code in software projects. It provides various options to adapt to
        different needs, such as excluding lines with only brackets or using table output.
    
    Copyright (c) 2024 anic17 Software

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

int is_quote(char *quotes_str, size_t qlen, int chr)
{
    for (size_t i = 0; i < qlen; i++)
    {
        if (chr == quotes_str[i])
            return true;
    }
    return false;
}

void show_version()
{
    printf("LexaCount v%s - Count source lines of code.\n", lexacount_version);
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
        " -v, --version  Display program version\n\n"
        "Copyright (c) 2024 anic17 Software\n");
}

void missing_param(char *s)
{
    fprintf(stderr, "Error: Required parameter after '%s'. See 'lexacount --help' for more information.\n", s);
    exit(1);
}

size_t count_loc(char *filename, char **comments, char *quotes, char *brackets, size_t *file_loc, size_t *file_comment, size_t *file_blank, size_t *file_bracket, bool excludeBrackets)
{
    *file_loc = 0;
    *file_comment = 0;
    *file_blank = 0;
    *file_bracket = 0;
    filename[strcspn(filename, "\r\n")] = '\0';
    if (filename[0] == '\0')
    {
        return 0;
    }
    FILE *count_fp = fopen(filename, "rb");
    if (!count_fp)
    {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        return 0;
    }
    bool isLineBlank = true;
    bool isLineOnlyBrackets = false;
    bool isFileEmpty = true;
    int chr = 0;
    size_t comment_idx = 0;
    int quoteStatus = 0; // Status of the quotes
    size_t qlen = strlen(quotes);
    size_t blen = strlen(brackets);
    size_t clen = strlen(comments[0]);

    while ((chr = fgetc(count_fp)) != EOF)
    {
        
        isFileEmpty = false;
        if (is_quote(quotes, qlen, chr))
        {
            quoteStatus ^= chr;
        }
        if (isLineBlank && !quoteStatus)
        {
            if (comment_idx >= clen - 1)
            {
                (*file_comment)++;
                comment_idx = 0;
                continue;
            }
            if (chr == comments[0][comment_idx])
            {
                comment_idx++;
                continue;
            }
        }

        if ((!isspace(chr) || (!is_bracket(brackets, blen, chr) && excludeBrackets)))
        {
            if (is_bracket(brackets, blen, chr) && excludeBrackets)
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
            {
                (*file_bracket)++;
            }
            else if (isLineBlank)
            {
                (*file_blank)++;
            }

            (*file_loc)++;
            isLineBlank = true;
            isLineOnlyBrackets = false;
            quoteStatus = 0;
            comment_idx = 0;
        }
    }

    if (!isFileEmpty)
    {
        if (isLineOnlyBrackets && isLineBlank)
        {
            (*file_bracket)++;
        }
        else if (isLineBlank)
        {
            (*file_blank)++;
        }
        (*file_loc)++;
    }

    fclose(count_fp);
    return *file_loc;
}

void print_table_header(const char *formatspec, bool excludeBrackets)
{
    printf("| ");
    printf(formatspec, "File");
    printf(" | %-10s | %-10s | %-10s |", "Code lines", "Blank", "Comments");

    if (excludeBrackets)
    {
        printf(" %-15s |", "Brackets only");
    }

    printf("\n");
}

void print_table_separator(size_t flen, bool excludeBrackets)
{
    printf("|");
    for (size_t k = 0; k < flen + 2; k++)
    {
        printf("-");
    }
    printf("|------------|------------|------------|");

    if (excludeBrackets)
    {
        printf("-----------------|");
    }
    printf("\n");
}

void print_table_row(const char *formatspec, const char *filename, size_t lines, size_t blanks, size_t comments, size_t brackets, bool excludeBrackets)
{
    printf("| ");
    printf(formatspec, filename);
    printf(" | %-10zu | %-10zu | %-10zu |", excludeBrackets ? (lines - blanks - comments - brackets) : (lines - blanks - comments), blanks, comments);

    if (excludeBrackets)
    {
        printf(" %-15zu |", brackets);
    }
    printf("\n");
}

void display_lines(const char *formatspec, const char *filename, size_t file_loc, size_t file_blank, size_t file_comment, size_t file_bracket, bool tableOutput, bool excludeBrackets)
{
    if (tableOutput)
    {
        print_table_row(formatspec, filename, file_loc, file_blank, file_comment, file_bracket, excludeBrackets);
    }
    else
    {
        printf("%s: %zu lines", filename, excludeBrackets ? (file_loc - file_blank - file_comment - file_bracket) : (file_loc - file_blank - file_comment));

        if (file_blank > 0 || file_comment > 0 || file_bracket > 0)
        {
            printf(" (excluding");

            if (file_blank > 0)
            {
                printf(" %zu blank line%s", file_blank, (file_blank == 1) ? "" : "s");
            }

            if (file_comment > 0)
            {
                printf("%s%zu comment%s", (file_blank > 0) ? " and " : " ", file_comment, (file_comment == 1) ? "" : "s");
            }

            if (file_bracket > 0 && excludeBrackets)
            {
                printf("%s%zu line%s with only brackets", ((file_blank > 0) || (file_comment > 0)) ? " and " : " ", file_bracket, (file_bracket == 1) ? "" : "s");
            }

            printf(")");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    char quotechars[] = "'\"";
    char bracketchars[] = "{}[]()";
    if (argc < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "/?"))
    {
        help();
        return 0;
    }
    char *loc_file = argv[1];
    FILE *list;

    char *comments[] = {"//"};

    char **fnames = calloc(argc - 1, sizeof(char *)), **list_fnames = calloc(argc - 1, sizeof(char *));
    size_t fname_count = 0, list_fname_count = 0;

    size_t longest_fname = 0;

    bool excludeBrackets = false;
    bool tableOutput = false;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list"))
        {
            if (i + 1 < argc)
            {
                if (argv[i] != NULL)
                {
                    list_fnames[list_fname_count] = calloc(strlen(argv[i + 1]) + 1, sizeof(char));
                    memcpy(list_fnames[list_fname_count], argv[i + 1], strlen(argv[i + 1]));
                    if (strlen(list_fnames[list_fname_count]) > longest_fname)
                    {
                        longest_fname = strlen(list_fnames[list_fname_count]);
                    }
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
            excludeBrackets = true;
        }
        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--table"))
        {
            tableOutput = true;
        }
        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
        {
            show_version();
            printf("\nCopyright (c) 2024 anic17 Software\n");
            exit(0);
        }

        else
        {
            if (argv[i] != NULL)
            {
                fnames[fname_count] = calloc(strlen(argv[i]) + 1, sizeof(char));
                memcpy(fnames[fname_count], argv[i], strlen(argv[i]));
                if (strlen(fnames[fname_count]) > longest_fname)
                {
                    longest_fname = strlen(fnames[fname_count]);
                }
                fname_count++;
            }
        }
    }

    size_t total_loc = 0, total_comment_loc = 0, total_blank_loc = 0, total_bracket_loc = 0;
    size_t file_loc = 0, file_comment_loc = 0, file_blank_loc = 0, file_bracket_loc = 0;
    size_t file_count = 0;

    char filename[512];

    if (longest_fname < 11)
    {
        longest_fname = 11;
    }

    char *fspec = calloc(sizeof(char), longest_fname + 10);
    snprintf(fspec, longest_fname, "%%-%zus", longest_fname);

    for (size_t i = 0; i < list_fname_count; i++)
    {
        list = fopen(list_fnames[i], "rb");
        if (!list)
        {
            fprintf(stderr, "%s: %s", loc_file, strerror(errno));
            return errno;
        }
        while (fgets(filename, sizeof filename, list)) // If someone knows a better way to avoid incorrect padding, let me know
        {
            if (strlen(filename) > longest_fname)
            {
                longest_fname = strlen(filename);
                snprintf(fspec, longest_fname, "%%-%zus", longest_fname);
            }
        }
        fclose(list);
    }

    if (tableOutput)
    {
        print_table_header(fspec, excludeBrackets);
        if (list_fname_count == 0)
            print_table_separator(longest_fname, excludeBrackets);
    }

    for (size_t i = 0; i < list_fname_count; i++)
    {
        list = fopen(list_fnames[i], "rb");
        if (!list)
        {
            fprintf(stderr, "%s: %s", loc_file, strerror(errno));
            return errno;
        }
        print_table_separator(longest_fname, excludeBrackets);

        while (fgets(filename, sizeof filename, list))
        {
            count_loc(filename, comments, quotechars, bracketchars, &file_loc, &file_comment_loc, &file_blank_loc, &file_bracket_loc, excludeBrackets);
            display_lines(fspec, filename, file_loc, file_blank_loc, file_comment_loc, file_bracket_loc, tableOutput, excludeBrackets);
            file_count++;
            total_loc += file_loc;
            total_comment_loc += file_comment_loc;
            total_blank_loc += file_blank_loc;
            total_bracket_loc += file_bracket_loc;
        }
        fclose(list);
    }

    for (size_t i = 0; i < fname_count; i++)
    {
        count_loc(fnames[i], comments, quotechars, bracketchars, &file_loc, &file_comment_loc, &file_blank_loc, &file_bracket_loc, excludeBrackets);
        display_lines(fspec, fnames[i], file_loc, file_blank_loc, file_comment_loc, file_bracket_loc, tableOutput, excludeBrackets);
        file_count++;
        total_loc += file_loc;
        total_comment_loc += file_comment_loc;
        total_blank_loc += file_blank_loc;
        total_bracket_loc += file_bracket_loc;
    }
    if (tableOutput)
    {
        print_table_separator(longest_fname, excludeBrackets);
    }
    display_lines(fspec, "Total lines", total_loc, total_blank_loc, total_comment_loc, total_bracket_loc, tableOutput, excludeBrackets);
    free(fspec);
    free(list_fnames);
    free(fnames);
    return 0;
}