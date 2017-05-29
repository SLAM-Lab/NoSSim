#include "image_file.h"
int load_file_to_memory(const char *filename, char **result)
{


        unsigned int size = 0;
        FILE *f = fopen(filename, "rb");
        if (f == NULL)
        {
                *result = NULL;
                return -1; // -1 means file opening fail 
        }
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        //printf("Size is %d \n", size);
        fseek(f, 0, SEEK_SET);
        *result = (char *)malloc(size+1);
        if (size != fread(*result, sizeof(char), size, f))
        {
                free(*result);
                return -2; // -2 means file reading fail 
        }

        fclose(f);

        (*result)[size] = 0;

        return size;
}

void dump_mem_to_file(unsigned int size, const char *filename, char **result)
{
	FILE *fp;
        //printf("Size is %d \n", size*sizeof(char));
	fp = fopen( filename , "wb" );
        //printf("Size is %d \n", fwrite(*result , 1 ,  size*sizeof(char) , fp ));
	fwrite(*result , 1 ,  size*sizeof(char) , fp );
	fclose(fp);
}

