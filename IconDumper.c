// IconDumper.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define is ==
#define isnt !=
#define null NULL

size_t DumpIcons(FILE* stream, const char* targetExtension);
bool StreamEquals(FILE* stream, const char* target, size_t length);

#define BUFFER_SIZE 1024

int main(int argc, char** argv)
{
	if (argc <= 2)
	{
		fprintf(stdout, "Path to catalog.json must be provided usage ./IconDumper.exe \"<path>/catalog.json\" <extension>\n");
		exit(-1);
	}

	const char* path = argv[1];

	const char* targetExtension = argv[2];

	size_t extensionLength = strlen(targetExtension) + 2;

	char extension[BUFFER_SIZE];

	memcpy(extension, targetExtension, min(BUFFER_SIZE, extensionLength));

	extension[max(min(BUFFER_SIZE, extensionLength) - 1, 0)] = '\0';

	extension[max(min(BUFFER_SIZE, extensionLength) - 2, 0)] = '\"';

	FILE* stream;
	errno_t error = fopen_s(&stream, path, "rb");

	if (stream is null)
	{
		char errorBuffer[BUFFER_SIZE];
		strerror_s(errorBuffer, BUFFER_SIZE, error);

		fprintf(stderr, "Failed to open file %s\n\t%s", path, errorBuffer);
		exit(-1);
	}

	size_t count = DumpIcons(stream, extension);

	fclose(stream);

	fprintf(stdout, "Found %lli icons\n", count);
}

size_t DumpIcons(FILE* stream, const char* targetExtension)
{
	// pattern "*.png"
	
	char buffer[BUFFER_SIZE];
	size_t index = 0;

	int c;

	/*
		parse until we encounter "
		read all chars until .
		ensure ends in .png"
		if it does output to stdout
	*/

	size_t targetLength = strlen(targetExtension);

	size_t count = 0;

	bool searching = false;

	// parse until end of file
	while ((c = fgetc(stream)) != EOF)
	{
		buffer[index] = c;

		// check to see if we are looking for start of target char
		if (searching is false && c == '"')
		{
			searching = true;
			index = 0;
			continue;
		}else if(searching)
		{
			// check to see if it's the beginning of end of target
			if (c == '.' && StreamEquals(stream, targetExtension, targetLength))
			{
				// null terminate the current character
				buffer[index] = '\0';
				fprintf(stdout, "\"%s.%s\n", buffer, targetExtension);
				++count;
				index = 0;
				searching = false;
				continue;
			}

			// if .png" wasn't found but " was found we found an invalid string, start over
			if (c == '"')
			{
				index = 0;
				searching = false;
				continue;
			}
		}

		++index;

		if (index >= BUFFER_SIZE)
		{
			index = 0;
		}
	}

	return count;
}

bool StreamEquals(FILE* stream, const char* target, size_t length)
{
	size_t index = 0;
	
	int c;
	while ((c = fgetc(stream)) != EOF)
	{
		if (index > length)
		{
			return false;
		}

		if (c != target[index])
		{
			return false;
		}
		
		if(index == (length - 1))
		{
			return true;
		}

		++index;
	}

	return false;
}