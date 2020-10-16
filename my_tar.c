#include "my_tar.h"

int format_error ( char option, char* file_f, int files_count )
{
    switch ( option )
    {
    case 'c': break;
    case 't': break;
    case 'r': break;
    case 'u': break;
    case 'x': break;
    default :
        printf("You shall specify one of the -ctrux flags.\n");
        return 2;
    }

    if ( !file_f[0] )
    {
        printf("You need to use -f option.\n");
        return 3;
    }

    if ( !files_count && option != 't' && option != 'x' )
    {
        printf("You need to specify file used by -ctrux command.\n");
        return 4;
    }

    return 0;
}

int run_option(char option, char* file_f, char** archive_files, int files_count)
{
    switch ( option )
    {
    case 'c':
        my_tar_c(file_f, archive_files, files_count);
        break;
    case 't':
        my_tar_t(file_f);
        break;
    case 'r':
        my_tar_r(file_f, archive_files, files_count);
        break;
    case 'u':
        my_tar_u(file_f, archive_files, files_count);
        break;
    case 'x':
        my_tar_x(file_f);
        break;
    default:
        return 1;
    }

    return 0;
}

int main (int argc, char** argv)
{
    char file_f[100] = {'\0'};
    char* archive_files[10];
    int files_count = 0, out;
    char option = '\0';

    //check format
    for (int i = 1; i < argc; i++)
    {
        //if is option
        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                if ( argv[i][j] == 'f' && file_f[0] == '\0' )
                {
                    if (argv[i][j + 1] != '\0')
                    {
                        my_strcpy(file_f, &argv[i][j + 1]);
                    }
                    else if ( i + 1 < argc )
                    {
                        my_strcpy(file_f, argv[++i]);
                    }
                    break;
                }
                else if ( argv[i][j] == 'f' && file_f[0] != '\0' )
                {
                    printf("You should use only one -f file.\n");
                    return 5;
                }
                else if ( option == '\0')
                {
                    option = argv[i][j];
                }
                else if ( option != argv[i][j] )
                {
                    printf("You should use only one -ctrux option.\n");
                    return 1;
                }
            }
        }
        //if is filename
        else
        {
            archive_files[files_count] = malloc(sizeof(char) * ( my_strlen(argv[i]) + 1) );
            my_strcpy (archive_files[files_count++], argv[i]);
        }
    }

    if ( ( out = format_error ( option, file_f, files_count ) ) == 0 )
    {
        out = run_option (option, file_f, archive_files, files_count);
    }

    for (int i = 0; i < files_count; i++)
    {
        free(archive_files[i]);
    }

    return out;
}
