#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <cstring>
#include <string>
#include <ctime>
#include <sys/wait.h>


#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS 20

class Command {
protected:
    const char* cmd_line;
public:
    explicit Command(const char *cmd_line);

    virtual ~Command() = default;;

    virtual void execute() = 0;
    const char* get_cmd_line(){return cmd_line;}
};

class BuiltInCommand : public Command {
public:
    explicit BuiltInCommand(const char *cmd_line);

    virtual ~BuiltInCommand() {
        delete this->cmd_line;
    }
};

class ExternalCommand : public Command {
    bool is_alarm;
public:
    ExternalCommand(const char *cmd_line, bool is_alarm);

    virtual ~ExternalCommand() = default;

    void execute() override;
};

class PipeCommand : public Command {
    std::string command1;
    std::string command2;
    std::string delimiter;
public:
    explicit PipeCommand(const char *cmd_line);

    virtual ~PipeCommand() = default;

    void execute() override;
};

class RedirectionCommand : public Command {
    char* command;
    char* filename;
    bool append;
    int stdout_copy;
    int fd;
    bool redirect_success;
public:
    explicit RedirectionCommand(const char *cmd_line);

    virtual ~RedirectionCommand() = default;

    void execute() override;
    void prepare();
    void cleanup();
};

class ChpromptCommand : public BuiltInCommand {
public:
    explicit ChpromptCommand(const char *cmd_line);

    virtual  ~ChpromptCommand() = default;

    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
public:
    char **plastPwd;
    ChangeDirCommand(const char *cmd_line, char **plastPwd);

    virtual ~ChangeDirCommand() = default;

    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    explicit GetCurrDirCommand(const char *cmd_line);

    virtual ~GetCurrDirCommand() = default;

    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    explicit ShowPidCommand(const char *cmd_line);

    virtual ~ShowPidCommand() = default;

    void execute() override;
};

class JobsList;

class QuitCommand : public BuiltInCommand {
public:

    explicit QuitCommand(const char *cmd_line);

    virtual ~QuitCommand() = default;

    void execute() override;
};

class AlarmList{
public:
    class AlarmEntry{
    public:
        std::string command;
        time_t time_created;
        time_t duration;
        time_t time_limit;
        pid_t pid;
        AlarmEntry(std::string command, time_t time_created, time_t duration, pid_t pid);
        ~AlarmEntry() = default;
    };

    std::vector<AlarmEntry> alarms;

    AlarmList();
    void add_alarm(std::string command, time_t duration, pid_t pid);
    void delete_alarms();
};

class JobsList {
public:
    class JobEntry {
    public:
        int job_id;
        pid_t job_pid;
        time_t time_created;
        std::string command;
        bool isStopped;

        JobEntry(int job_id, pid_t job_pid, time_t time_created, std::string& command, bool isStopped);
    };

    std::vector<JobEntry> job_list;
    int max_job_id;

    JobsList();

    ~JobsList() = default;

    void addJob(Command *cmd, pid_t pid, bool last_cmd_fg = false ,bool isStopped = false);

    void killAllJobs();

    void removeFinishedJobs();

    JobEntry *getJobById(int jobId);

    void removeJobById(int jobId);

    JobEntry *getLastJob(int *lastJobId);

    JobEntry *getLastStoppedJob(int *jobId);
};

class JobsCommand : public BuiltInCommand {

public:
    explicit JobsCommand(const char *cmd_line);

    virtual ~JobsCommand() = default;

    void execute() override;
};

class KillCommand : public BuiltInCommand {

public:
    explicit KillCommand(const char *cmd_line);

    virtual ~KillCommand() = default;

    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {

public:
    explicit ForegroundCommand(const char *cmd_line);

    virtual ~ForegroundCommand() = default;

    void execute() override;
};


class BackgroundCommand : public BuiltInCommand {

public:
    explicit BackgroundCommand(const char *cmd_line);

    virtual ~BackgroundCommand() = default;

    void execute() override;
};

class HeadCommand : public BuiltInCommand {
public:
    explicit HeadCommand(const char *cmd_line);

    virtual ~HeadCommand() = default;

    void execute() override;
};

class TimeoutCommand: public Command{
public:
    explicit TimeoutCommand(const char* cmd_line);

    virtual ~TimeoutCommand() = default;

    void execute() override;
};


class SmallShell {
private:

    SmallShell();

public:
    static std::string smash_prompt;
    static pid_t pid;
    char* last_directory;
    static JobsList job_list;
    bool is_pipe;
    pid_t current_process;
    int job_id_fg;
    bool last_cmd_fg;
    std::string current_cmd;

    AlarmList alarm_list;
    std::string current_alarm_cmd;
    bool fg_alarm;
    pid_t alarm_pid;
    time_t current_duration;

    Command *CreateCommand(const char *cmd_line, bool is_alarm);

    SmallShell(SmallShell const &) = delete; // disable copy ctor
    void operator=(SmallShell const &) = delete; // disable = operator
    static SmallShell &getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell();

    void executeCommand(const char *cmd_line, bool alarm=false);
    void setLastDirectory(char *dir);
};

#endif //SMASH_COMMAND_H_
