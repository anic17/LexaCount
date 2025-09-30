# LexaCount

## What is LexaCount?

LexaCount is a lightweight and versatile command-line tool designed to simplify counting source lines of code in software projects. Whether you're a developer reviewing codebases, a project manager tracking progress, or simply curious about code metrics, LexaCount provides an efficient way to analyze and understand the structure of your source code with various options that adapt to your needs.

## Screenshot

![Screenshot of LexaCount](https://raw.githubusercontent.com/anic17/LexaCount/c4fdd94fc156f0e0975fba7538dfa7a6733e9d32/lexacount_2.png)  

**Screenshot of LexaCount with command line `lexacount -l lexacount.lst -b -t`**

## Usage

`lexacount [-b] [-t] [-x] [-l <list file 1>] [-l <list file 2>] ... [file1] [file2] ...`

### Switches

`-b, --bracket`  
Do not count lines containing only brackets or parentheses

`-h, --help`  
Display this help menu

`-l, --list`  
Load file names from a file

`-t, --table`  
Print the output using tables

`-v, --version`  
Display program version

`-x, --exclude`
Exclude files inside lists if they cannot be read; do not give error messages

### Example usages

`lexacount main.c main.h`  
Counts the lines of code of files `main.c` and `main.h`

`lexacount -l file_names.list -t`  
Counts the lines of all file names inside `file_names.list` in table format

`lexacount -l file_names.list include/header.h -b`  
Counts the lines of all file names inside `file_names.list` and `include/header.h`, excluding all lines containing only brackets  


### Using lists

Using a list file is a beneficial addition to LexaCount, allowing for the automation of counting lines in a large codebase with unlimited files. The list file should contain file names separated by newlines, with each line representing a single file. For example, a list file named `lines.list` would look like this:

```
src/include/main.h
src/include/input.h
src/main.c
src/input.c
make.sh
website/index.html
```
By calling `lexacount -l lines.list`, LexaCount will count the lines of code for the files listed inside `lines.list` and not the lines of the list file itself. This command loads file names from lines.list and counts the lines for each specified file, streamlining the process of analyzing multiple files.

## Compatibility

LexaCount has been written in standard C99 without making use of any platform-specific libraries. This makes LexaCount compatible with Windows, Linux, and Mac.

## Contributing

If you found a bug or want to add a new feature, don't hesitate to create a [pull request](https://github.com/anic17/LexaCount/pulls) or an [issue](https://github.com/anic17LexaCount/issues)! Contributions are greatly appreciated.

## License

LexaCount is distributed under the GNU GPL v3.0 license. See the [LICENSE](/LICENSE) file for details.

**Copyright &copy; 2025 anic17 Software**

<img src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fanic17%2FLexaCount&count_bg=%23FFFFFF&title_bg=%23FFFFFF&icon=&icon_color=%23FFFFFF&title=hits&edge_flat=false" height=0 width=0>
