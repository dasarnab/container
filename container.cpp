#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sched.h>
#include <sstream>

using namespace std;

int STACK_SIZE = 65536;
string path;
string displayName;
string netnsname;
string hostInterface;
string containerInterface;
string containerIp;
string hostIp;
string is_memory_limit;
string memory_limit_folder;
string memory_limit;


int child_fn(void *args){
    sleep(2);
    cout <<"Root File System Path : "<< path << endl;
    if( chdir(path.c_str()) != 0){
        perror("CHDIR ERROR");
        return -1;
    }    
 
    if(chroot(path.c_str()) != 0){
        cout << "Error in chroot" <<endl;
        return -1;
    }

    if(sethostname(displayName.c_str(),displayName.size()+1) != 0){
        cout << "Error in setting hostname"<<endl;
        return -1;
    }

    int status = mount("proc", "/proc", "proc", 0, NULL);
    if(status != 0){
        cout << "Mount Error" << endl;
        return -1;
    }
    int pid = fork();

    if(pid == 0){
        string pathOfShell = "/bin/bash";
        char *const argv[] = {
            (char *)pathOfShell.c_str(),
            NULL
        };
        // argv[0] = pathOfShell.c_str();
        if(execvp(argv[0],argv) !=0 ){
            perror("EXECV");
        }
    }else{
        // cout << "waiting for "<<pid<<endl;
        waitpid(pid,NULL,0);
        // cout << "exiting"<<endl;
    } 
    cout << "Closing Container" <<endl;
    return 0;    

}



int main(int argc, char *argv[]){
    //* create a new PID namespace
    cout <<"Container Parent PID: "<< getpid()<<endl;
    if(argc != 2){
        cout << "Enter A config File"<< endl;
        exit(1);
    }
    void *stack = malloc(STACK_SIZE);
    ifstream MyReadFile(argv[1]);
    getline (MyReadFile, path);
    getline (MyReadFile, displayName);
    getline(MyReadFile,netnsname);
    getline(MyReadFile,hostInterface);
    getline(MyReadFile,containerInterface);
    getline(MyReadFile,containerIp);
    getline(MyReadFile,hostIp);
    getline(MyReadFile,is_memory_limit);
    stringstream is_limit_set_stream(is_memory_limit);
    int is_limit_set =0;
    is_limit_set_stream >> is_limit_set;
    cout << "Is CGroup Memory Limit Set : " << is_limit_set<<endl;
    getline(MyReadFile,memory_limit_folder);
    getline(MyReadFile,memory_limit);


    


    int child_pid = clone(&child_fn,(char *)stack+STACK_SIZE,CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWUTS| SIGCHLD,NULL);
    cout << "Container Cloned child PID : "<< child_pid << endl;
    // !* with for child to end.
    // * setting up netwrok namespace;
    stringstream cmd;

    char mycmd[50];
    sprintf(mycmd,"ip netns attach %s %d",netnsname.c_str(),child_pid);
    system(mycmd);
    // cmd.clear();
    

    cmd << "ip link add "<< hostInterface <<" type veth peer name "<< containerInterface;
    system(cmd.str().c_str());
    cmd.clear();
    
    stringstream cmd1;
    cmd1 << "ip link set "<< containerInterface <<" netns "<<netnsname;
    system(cmd1.str().c_str());
    cmd1.clear();
    
    stringstream cmd2;
    cmd2 << "ip netns exec "<< netnsname <<" ifconfig "<< containerInterface <<" "<<containerIp <<" up";
    system(cmd2.str().c_str());
    cmd2.clear();

    stringstream cmd3;
    cmd3 << "ifconfig "<< hostInterface <<" "<< hostIp <<" up";
    system(cmd3.str().c_str());
    cmd3.clear();

    if(is_limit_set){

        stringstream cmd4;
        cmd4 << "mkdir /sys/fs/cgroup/memory/"<<memory_limit_folder;
        system(cmd4.str().c_str());
        cmd4.clear();

        stringstream cmd5;
        cmd5 << "echo "<< memory_limit<<" > /sys/fs/cgroup/memory/"<<memory_limit_folder<<"/memory.limit_in_bytes";
        system(cmd5.str().c_str());
        cmd5.clear();

        stringstream cmd6;
        cmd6 << "echo 0 > /sys/fs/cgroup/memory/"<<memory_limit_folder<<"/memory.swappiness";
        system(cmd6.str().c_str());
        cmd6.clear();

        stringstream cmd7;
        cmd7 << "echo "<< child_pid << " > /sys/fs/cgroup/memory/"<<memory_limit_folder<<"/tasks";
        // cout << cmd7.str()<<endl;
        system(cmd7.str().c_str());
        cmd7.clear();

    }


    waitpid(child_pid,NULL,0);
    sleep(2);

    if(is_limit_set){
        stringstream cmd8;
        cmd8 << "rmdir /sys/fs/cgroup/memory/"<<memory_limit_folder;
        // cout << cmd67str()<<endl;
        system(cmd8.str().c_str());
        cmd8.clear();
    }

    free(stack);

    stringstream cmd9;
    cmd9 << "ip netns delete "<<netnsname;
    // cout << cmd67str()<<endl;
    system(cmd9.str().c_str());
    cmd9.clear();
    
    
    cout << "Closing Containter parent" <<endl;

    return 0;
}