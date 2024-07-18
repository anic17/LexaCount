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

#ifndef _LEXACOUNT_H_
#define _LEXACOUNT_H_

#define is_bracket is_quote

const char lexacount_version[] = "1.2";


int is_quote(char *quotes_str, size_t qlen, int chr);
void show_version();
void help();
void missing_param(char *s);
size_t count_loc(char *filename, char **comments, char *quotes, char *brackets, size_t *file_loc, size_t *file_comment, size_t *file_blank, size_t *file_bracket, bool excludeBrackets);
void print_table_header(const char *formatspec, bool excludeBrackets);
void print_table_separator(size_t flen, bool excludeBrackets);
void print_table_row(const char *formatspec, const char *filename, size_t lines, size_t blanks, size_t comments, size_t brackets, bool excludeBrackets);
void display_lines(const char *formatspec, const char *filename, size_t file_loc, size_t file_blank, size_t file_comment, size_t file_bracket, bool tableOutput, bool excludeBrackets);

#endif