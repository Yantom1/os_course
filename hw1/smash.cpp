#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    struct sigaction action;
    memset(&action, '\0', sizeof(action));
    action.sa_flags = SA_RESTART;
    action.sa_handler = &alarmHandler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGALRM, &action, NULL);


    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        smash.is_pipe = false;
        std::cout << smash.smash_prompt <<"> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str());
        if(smash.is_pipe)
            break;
    }
    return 0;
}