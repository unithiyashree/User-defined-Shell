#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int get_words();
void check_commands();
void pwd_command();
void echo_command();
void cd_command( int status);
void psinfo_command();
void system_commands();

char *a[1000];
char command_line[1024];
char home[1024];
char *user;
char host[1024];
//This is the flag with sets 1 when directory is changed
int set = 0;

int main()
{
	while(1)
	{
		if(set == 0)
		{
			//Getting the current directory where the shell is located
			getcwd( home, 1024);
			//printf("%s\n", home );

			// Getting the username for the current user
			//char *user;
			user = getlogin();

			//Getting the hostname for the current user
			//char host[1024];
			gethostname(host, 1023);
			host[1023] = '\0';

			sprintf(command_line, "<%s@%s:~>", user, host);
			printf("<%s@%s:~>", user, host);

			// Scaning the input as a string and parsing it
			char c;
			char input[500];
			//scanf("%[^\n]", input);
			bzero( input,sizeof( input));
			while( ( c = getchar()) != '\n')
			{
				strncat( input, &c, 1);
			}

			// numof_parsed give the total no. of words after parsing
			int numof_parsed;
			numof_parsed = get_words( input);
			check_commands();
		}
		else
		{

			printf("%s", command_line);

			// Scaning the input as a string and parsing it
			char c;
			char input[500];
			//scanf("%[^\n]", input);
			bzero(input,sizeof( input));
			while((c=getchar())!='\n')
			{
				strncat( input,&c,1);
			}

			// numof_parsed give the total no. of words after parsing
			int numof_parsed;
			numof_parsed = get_words(input);
			check_commands();
		}
	}
	return 0;

}

int get_words(char str[])
{

	const char s[2] = " ";
	char *token;

	// get the first token 
	token = strtok(str, s);
	int i = 0, k;
	//char *a[1000];

	// walk through other tokens 
	while( token != NULL ) 
	{
		a[i] = token;
		//printf("%s \n", token );
		token = strtok(NULL, s);
		i++;
	}
	a[i] = token;
	return i;
}

// This function helps to check which is the command to be executed
void check_commands()
{	
	int status;
	if(strcmp(a[0],"quit")==0)
	{
		exit(0);
	}
	else if(strcmp(a[0],"pwd")==0)
	{
		pwd_command();
	}
	else if(strcmp(a[0],"echo")==0)
	{
		echo_command();
	}
	else if(strcmp(a[0],"cd")==0)
	{
		if( a[1] == NULL)
		{
			status = chdir(getenv("HOME"));
		}
		else
		{
			status = chdir( a[1]);
		}
		cd_command(status);
	}
	else if(strcmp(a[0],"psinfo")==0)
	{
		psinfo_command();
	}
	else
	{
		system_commands();
	}

}

// This function implements PWD command using system calls
void pwd_command()
{
	char *current_dir = NULL;
	int buff = 300;

	//Getting the present directory
	current_dir = getcwd(current_dir, 300);

	printf("%s\n", current_dir);
	//printf("%s",command_line);
}

// This function implements ECHO command using system calls
void echo_command()
{
	int i;

	for(i=1;a[i]!= '\0';i++)
	{
		printf("%s ", a[i] );
	}
	printf("\n");
	//printf("%s",command_line);
}

// This function implements CD command using system calls
void cd_command(int status)
{
	// If there is an error in the command format
	if( status != 0)
	{
		perror("Error: Couldn't execute command.\n");
	}
	else
	{

		int command_len;
		char current_dir[1024];
		getcwd(current_dir,1024);
		command_len = (strlen(current_dir)) - (strlen(home));
		if( command_len < 0)
		{
			sprintf( command_line, "<%s@%s:%.*s>", user, host, -command_len, home +  strlen(home) + command_len);
			//printf("%s", command_line);
			set = 1;
		}
		else if( command_line > 0)
		{
			sprintf( command_line, "<%s@%s:~%s>",user, host, current_dir + strlen(current_dir) - command_len);	
			//printf("%s", command_line);
			set = 1;
		}

		else
		{
			sprintf( command_line, "<%s@%s:~>", user, host);
			//printf("%s", command_line);
			set = 1;
		}
	}

} 

// This function implements and prints the Pid information
void psinfo_command()
{	

	if( a[1] == NULL)
	{
		FILE * file;
		char buff[1024] = "~/a.out";
		char status[100];
		char memory[100];
		char process_status[100];
		int current_pid;

		//Opening the file to get the ps info
		file = fopen ("/proc/self/status", "r");
		char act_mem[100];
		int temp = 0;
		while(temp != 1)
		{
			fgets(status,100,file);
			temp++;
		}
		fscanf(file,"%s %s",status,process_status);
		while(1)
		{
			fgets(memory,100,file);
			if(strstr(memory,"VmPeak") != NULL)
			{
				break;
			}
		}
		fscanf(file, "%s %s",memory,act_mem);

		//Printing the expected data
		// Getting the current pid
		current_pid = getpid();
		printf("Pid -- %d\n", current_pid);
		printf("Process Status -- %s\n", process_status);
		printf("memory -- %s\n", act_mem);
		printf("Executable path -- %s\n", buff);
		fclose(file);
		//printf("%s",command_line);

	}
	else
	{
		char temp1[1024] = "/proc/";
		char temp2[1024] = "/proc/";
		char temp3[1024] = "/status";
		char temp4[1024] = "/cmdline";
		char mem[100];
		char process_status[100];
		char act_mem[100];
		char status[100];
		size_t buflen = 1024;
		char buff[buflen];



		strcat( temp1, a[1]);
		strcat( temp2, a[1]);
		strcat( temp1, temp3);
		strcat( temp2, temp4);

		FILE * file;
		file = fopen( temp2,"r");
		fgets(buff,1024,file);
		fclose(file); 
		file = fopen (temp1, "r");

		int temp = 0; 
		while(temp != 1)
		{
			fgets( status, 100, file);
			temp++;
		}
		fscanf( file, "%s %s", mem, process_status);
		while(1)
		{
			fgets( mem, 100, file);
			if( strstr( mem, "VmPeak" ) != NULL)
			{
				break;
			}
		}
		fscanf(file,"%s %s", status, act_mem);

		printf("Pid -- %s\n", a[1]);
		printf("Process Status -- %s\n", process_status);
		printf("memory -- %s\n", act_mem);
		printf("Executable path -- %s\n", buff);
		fclose(file);
		//printf("%s", command_line);
	} 
}

void system_commands()
{
	pid_t pid;
	pid = fork();
	if( pid < 0)
	{
		// child proc not created
		perror("Child Proc. not created\n");
		_exit(-1);
	}
	else if(pid == 0)
	{
		int ret;
		//ret=execvp(*arr,arr);
		if( ret < 0)
		{
			perror("Error : Exec failed\n");
			_exit(-1);
		}
		else if(pid == 0)
		{
			int ret = 0;
			ret = execvp( a[0], a);
			if(ret < 0)
			{
				perror("Error: couldn't execute command. \n");
				_exit(-1);
			}
			printf("Giddy! Up..\n");
			_exit(0);
		}
	}
	wait();
} 

