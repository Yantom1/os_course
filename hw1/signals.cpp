#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
    cout << "smash: got ctrl-Z" << endl;
    SmallShell& smash = SmallShell::getInstance();
    if(smash.current_process != -1) {
        bool temp = smash.last_cmd_fg;
        Command* command = smash.CreateCommand(smash.current_cmd.c_str(), false);
        if(temp) { // if last command was fg so insert with already given job id
            smash.job_list.addJob(command, smash.current_process, true, true);
        } else {
            smash.job_list.addJob(command, smash.current_process, false , true);
        }
        kill(smash.current_process, SIGSTOP);
        cout << "smash: process " << smash.current_process << " was stopped" << endl;
        smash.current_process = -1;
        smash.current_cmd = "";
        delete command;
    }
}

void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C" << endl;
    SmallShell& smash = SmallShell::getInstance();
    if(smash.current_process != -1) {
        int process_num = smash.current_process;
        smash.current_process = -1;
        smash.current_cmd = "";
        kill(process_num, SIGINT);
        cout << "smash: process " << process_num << " was killed" << endl;
    }
}

void alarmHandler(int sig_num) {
    cout << "smash: got an alarm" << endl;
    SmallShell& shell = SmallShell::getInstance();
    if(shell.fg_alarm){
        kill(shell.current_process, SIGKILL);
        cout << "smash: " << shell.current_alarm_cmd << " timed out!" << endl;
        shell.fg_alarm = false;
        shell.current_alarm_cmd = "";
    } else {
        shell.alarm_list.delete_alarms();
    }
}

