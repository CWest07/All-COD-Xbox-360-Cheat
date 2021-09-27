// Read an INI file into easy-to-access name/value pairs.
#include "stdafx.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "ini.h"
#include "INIReader.h"

using std::string;

#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ini.h"

#define MAX_LINE 200
#define MAX_SECTION 50
#define MAX_NAME 50

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace(*--p))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
{
    while (*s && isspace(*s))
        s++;
    return (char*)s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
static char* find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace(*s);
        s++;
    }
    return (char*)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

/* See documentation in header file. */
int ini_parse_file(FILE* file,
                   int (*handler)(void*, const char*, const char*,
                                  const char*),
                   void* user)
{
    /* Uses a fair bit of stack (use heap instead if you need to) */
    char line[MAX_LINE];
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

    /* Scan through file line by line */
    while (fgets(line, sizeof(line), file) != NULL) {
        lineno++;
        start = lskip(rstrip(line));

        if (*start == ';' || *start == '#') {
            /* Per Python ConfigParser, allow '#' comments at start of line */
        }
#if INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            /* Non-black line with leading whitespace, treat as continuation
               of previous name's value (as per Python ConfigParser). */
            if (!handler(user, section, prev_name, start) && !error)
                error = lineno;
        }
#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            /* Not a comment, must be a name[=:]value pair */
            end = find_char_or_comment(start, '=');
            if (*end != '=') {
                end = find_char_or_comment(start, ':');
            }
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }
    }

    return error;
}

/* See documentation in header file. */
int ini_parse(const char* filename,
              int (*handler)(void*, const char*, const char*, const char*),
              void* user)
{
    FILE* file;
    int error;

    file = fopen(filename, "r");
    if (!file)
        return -1;
    error = ini_parse_file(file, handler, user);
    fclose(file);
    return error;
}

INIReader::INIReader(string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

int INIReader::ParseError()
{
    return _error;
}

string INIReader::Get(string section, string name, string default_value)
{
    string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(string section, string name, long default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

bool INIReader::GetBoolean(string section, string name, bool default_value)
{
    string valstr = Get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

string INIReader::MakeKey(string section, string name)
{
    string key = section + "." + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    INIReader* reader = (INIReader*)user;
    string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value;
    return 1;
}

/*bool INIReader::SetBoolean(string section, string name, string value, string filename)
{
	printf("Setting ini bool! \n");
	string key = MakeKey(section, name);

	FILE* file;

	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		printf("Failed to open Engine.ini! \n");
		return false;
	}
	char line[MAX_LINE];
	PCHAR fileData = (PCHAR)malloc(0x10000);
	ZeroMemory(fileData, 0x10000);
	while (fgets(line, sizeof(line), file) != NULL)
	{
		strcat(fileData, line);
	}

	//char optionString[255];
	PCHAR optionString = (PCHAR)malloc(0x100);
	ZeroMemory(optionString, 0x100);
	sprintf(optionString, "[%s]", section);
	char* option = strstr(fileData, optionString);
	if (!option)
	{
		printf("Failed to find Engine.ini section! \n");
		return false;
	}

	char modString[100];
	sprintf(modString, "%s = ", name);
	option = strstr(option, modString);
	if (!option)
	{
		printf("Failed to find Engine.ini option! \n");
		return false;
	}


	std::string final = fileData;
	final.replace((size_t)((DWORD)option - (DWORD)fileData), _values[key].length(), value);
	fclose(file);

	printf("Engine.ini closed! \n");


	file = fopen(filename.c_str(), "w");
	if (!fputs(fileData, file))
	{
		fclose(file);
		printf("Failed to write Engine.ini value! \n");
		return false;
	}
	fclose(file);
	free(fileData);
	free(optionString);
	return true;
}

void INIReader::SetInt(string section, string name, int value)
{
	string key = MakeKey(section, name);
	_values[key] = value;
}*/