int format_error ( char option, char* file_f)
{
    if ( !option )
    {
        printf("You shall specify one of the -ctrux flags.");
        return 2;
    }
    else if ( option )
    {
        switch option
        {
        case 'c': break;
        case 't': break;
        case 'r': break;
        case 'u': break;
        case 'x': break;
        default : return 3;
        }
    }

    if ( !file_f[0] ) //Not sure
    {
        printf("You need to use -f option.");
        return 4;
    }

    return 0;
}

int run_option(char option, char* file_f, char** archive_files, int files_count)
{
    switch option
    {
    case c:
        my_tar_c(file_f, archive_files, files_count);
        break;
    // case t:
    //     my_tar_t();
    //     break;
    // case r:
    //     my_tar_r();
    //     break;
    // case u:
    //     my_tar_u();
    //     break;
    // case x:
    //     my_tar_x();
    //     break;
    default:
        return 1;
        break;
    }

    return 0;
}

int main (int argc, char** argv)
{
    char file_f[100];
    char* archive_files[10]; //We might need to malloc
    int check, files_count = 0;
    char option;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                if ( argv[i][j] == 'f' )
                {
                    if (argv[i][++j] != ' ')
                        strcpy(file_f, &argv[i][j]); //Not sure
                    else
                        strcpy(file_f, argv[++i]);
                }
                else if ( option == '\0')
                    option = argv[i][j];
                else if ( option != argv[i][j] )
                {
                    printf("You should use only one -ctrux option")
                    return 1;
                }
            }
        }
        else
        {
            // archive_files[k] = malloc(sizeof(char)*my_strlen(arvg[i])); Not Sure
            strcpy (archive_files[files_count++], argv[i]);
        }
    }

    if ( (int out = format_error ( option, file_f )) != 0)
        return out;

    if ( (int out = run_option (option, file_f, archive_files, files_count)) != 0) )
        return out;

    return 0;
}
