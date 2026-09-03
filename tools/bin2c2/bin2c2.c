#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

unsigned char *buffer;

void print_hex_big(FILE* outfile, unsigned char * buffer, int bytesPer)
{
    fprintf(dest, "0x");
    for(i = 0; i < bytesPer; i++)
    {
        fprintf(dest, "%02x", buffer[i]);
    }
    fprintf(dest, ", ");
}

// untested
void print_hex_little(FILE* outfile, unsigned char * buffer, int bytesPer)
{
    fprintf(dest, "0x");
    for(i = bytesPer; i > 0; i++)
    {
        fprintf(dest, "%02x", buffer[i-1]);
    }
    fprintf(dest, ", ");
}

int main(int argc, char *argv[])
{
	int fd_size;
	int bytesize = 1;
	int linelm = 16, lines;
    unsigned int ar_sz = 0;
	char bytearg;
	FILE *source,*dest;
	int i;

	if(argc < 4)
	{
		printf("bin2c2\n"
			   "Usage: bin2c2.exe infile outfile label {bytesize}\n\n");
		return 1;
	}

	if((source=fopen( argv[1], "rb")) == NULL)
	{
		printf("Error opening %s for reading.\n",argv[1]);
		return 1;
	}

	if(argc == 5)
	{
		bytearg = argv[4][0];
		switch(bytearg)
		{
			case '1':			// unsigned char
				bytesize = 1;
				linelm = 16;
				break;
			case '2':			// unsigned short
				bytesize = 2;
				linelm = 12;
				break;
			case '4':			// unsigned long/unsigned int
				bytesize = 4;
				linelm = 8;
				break;
			case '8':			// unsigned int64
				bytesize = 8;
				linelm = 4;
				break;
			default:
				printf("Error %c is not a valid size! Must be one of {1,2,4,8}\n");
                return 1;
				break;
		};
	}

	fseek(source, 0, SEEK_END);
	fd_size = ftell(source);
	fseek(source, 0, SEEK_SET);

	if((fd_size%bytesize) != 0)
	{
		printf("Size %x is not a multiple of bytesize %x.\n", fd_size, bytesize);
        fclose(source);
		return 1;
	}

	buffer = (unsigned char *)malloc(fd_size);
	if(buffer == NULL)
	{
		printf("Failed to allocate memory.\n");
        fclose(source);
		return 1;
	}

	if(fread(buffer, 1, fd_size, source) != fd_size)
	{
		printf("Failed to read file.\n");
        fclose(source);
		return 1;
	}
	fclose(source);

	if((dest = fopen(argv[2], "w+")) == NULL)
	{
		printf("Failed to open/create %s.\n",argv[2]);
		return 1;
	}

	fprintf(dest, "#ifndef __%s__\n", argv[3]);
	fprintf(dest, "#define __%s__\n\n", argv[3]);
    ar_sz = fd_size/bytesize;
	fprintf(dest, "static unsigned int %s_byte_size = 0x%x;\n", argv[3], fd_size);
	fprintf(dest, "static unsigned int %s_size = 0x%x;\n", argv[3], (fd_size/bytesize));

	switch(bytesize)
	{
		case 2:			// unsigned short
			fprintf(dest, "static unsigned short %s[0x%x] = {\n\t", argv[3], ar_sz);
			break;
		case 4:			// unsigned long
			fprintf(dest, "static unsigned long %s[0x%x] = {\n\t", argv[3], ar_sz);
			break;
		case 8:			// unsigned int64
			fprintf(dest, "static unsigned int64 %s[0x%x] = {\n\t", argv[3], ar_sz);  // int64 <-- probably need to change this, these days
			break;
		default:			// unsigned char
			fprintf(dest, "static unsigned char %s[0x%x] = {\n\t", argv[3], ar_sz);
			break;
	};

    // tackling this as unsigned char, presuming target endianess is big for larger than char[] groupings
    // see print_hex_little() for a little endian producing version
	for(i = 0, lines = 0;i < fd_size;i += bytesize, lines++)
	{
		if(lines == linelm)
		{
			fprintf(dest, "\n\t");
			lines = 0;
		}
        print_hex_buffer(dest, &buffer[i], bytesize);
	}

	fprintf(dest, "\n};\n\n#endif\n");
	fclose(dest);

	return 0;
}
