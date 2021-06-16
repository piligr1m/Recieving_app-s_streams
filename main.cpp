#include <iostream>
#include <fstream>
#include <unistd.h>
#include <wait.h>
#include <exception>
void help()
{
    std::cout << "usage: myprog stdin_option stdout_option stderr_option OBJECT" << std::endl;
    std::cout << "where OBJECT := { program }" << std::endl;
    std::cout << "      stdin_option := { -c[onsole] | -f[ile] }" << std::endl;
    std::cout << "      stdout_option := { -c[onsole] | -f[ile] }" << std::endl;
    std::cout << "      stderr_option := { -c[onsole] | -f[ile] }" << std::endl;
}

void stdin_redirect(std::string stdin_arg) // stdin redirect
{
    std::string stdinArg = stdin_arg;
    if (stdinArg == "-f") {
        FILE* stdinFile = std::fopen("stdin.txt", "r");
        dup2(fileno(stdinFile), 0);
        std::fclose(stdinFile);
    }
    else if (stdinArg == "-c") {
        dup2(0, 0);
    }
    else {
        throw std::invalid_argument("Error! Use help to show help.\n");
    }
}

void stdout_redirect(std::string stdout_arg) // stdout redirect
{
    std::string stdoutArg = stdout_arg;
    if (stdoutArg == "-f") {
        FILE* stdoutFile = fopen("stdout.txt", "a+");
        dup2(fileno(stdoutFile), 1);
        std::fclose(stdoutFile);
    }
    else if (stdoutArg == "-c") {
        dup2(1, 1);
    }
    else {
       throw std::invalid_argument("Error! Use help to show help.\n");
    }
}
void stderr_redirect(std::string stderr_arg) // stderr redirect
{
    std::string stderrArg = stderr_arg;
    if (stderrArg == "-f") {
        FILE* stderrFile = fopen("stderr.txt", "a+");
        dup2(fileno(stderrFile), 2);
        std::fclose(stderrFile);
    }
    else if (stderrArg == "-c") {
        dup2(1, 2);
    }
    else {
        throw std::invalid_argument("Error! Use help to show help.\n");
    }
}
int exec_prog(int argc, char* argv[], std::string prog)
{
    char* args[argc - 4];
    std::string program = prog;
    if (argc ==5)
    {
        return execl(program.c_str(), nullptr);
    }
    else
        for (size_t i{0U}; i < argc - 4; ++i) {
            args[i] = argv[4 + i];
        }
         return execv(program.c_str(), args);
}

int main(int argc, char* argv[])
{
    try{
    if (argc !=  5) {
        if (argc == 2 && std::string{argv[1]} == std::string{"help"}) {
            help();
        }
        else {
            throw std::logic_error("Error! Use help to show help.\n");
        }
	return 0;
    }
    std::string stdinArg{argv[1]};
    std::string stdoutArg{argv[2]};
    std::string stderrArg{argv[3]};
    std::string program{argv[4]};

    pid_t pid = fork();
    if (pid == -1) {
        throw std::logic_error("Impossible to fork program!\n");
	return -1;
    }
    else if (pid == 0) {
        // stdin redirect
        stdin_redirect(stdinArg);
        // stdout redirect
        stdout_redirect(stdoutArg);
        // stderr redirect
        stderr_redirect(stderrArg);
        // exec program
        exec_prog(argc, &argv[4], program);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Return code: " << status << std::endl;
    }
    }
    catch (const std::exception& e){
        std::cout <<e.what();
    }
    return 0;
}
