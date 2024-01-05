# LexaCount

## What is LexaCount?

LexaCount is a lightweight and versatile command-line tool designed to simplify counting source lines of code in software projects. Whether you're a developer reviewing codebases, a project manager tracking progress, or simply curious about code metrics, LexaCount provides an efficient way to analyze and understand the structure of your source code with various options that adapt to your needs.

## Usage

lexacount [-b] [-t] [-l <list file>] [file1] [file2] ...

### Switches

`-b, --bracket`  
Do not count lines containing only brackets or parenthesis

`-h, --help`  
Display this help menu

`-l, --list`  
Load file names from a file

`-t, --table`  
Print the output using tables

`-v, --version`  
Display program version

### Example usages

`lexacount main.c main.h`
Counts the lines of code of files `main.c` and `main.h`

`lexacount -l file_names.list -t`
Counts the lines of all file names inside `file_names.list` in table format

`lexacount -l file_names.list include/header.h -b`
Counts the lines of all file names inside `file_names.list` and `include/header.h`, excluding all lines containing only brackets

**Copyright &copy; 2024 anic17 Software**

<img src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fanic17%2FLexaCount&count_bg=%23FFFFFF&title_bg=%23FFFFFF&icon=&icon_color=%23FFFFFF&title=hits&edge_flat=false" height=0 width=0>
