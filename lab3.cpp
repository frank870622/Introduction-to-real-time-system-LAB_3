#include <iostream>
#include <algorithm>
#include <memory.h>
#include <vector>
#include <string>
using namespace std;

class Task //class of each task
{
  public:
    int ID;
    int release_time; //arrival time
    int execution_time;
    int deadline;
    int period;       //waiting time
    int preemption;
    int task;
    int suspend = 0;
};

class Processor //class of each processor
{
  public:
    int id;
    int ability;
    Task *task;
};

bool sort_execution_time(Task, Task); //sort vector's execute time from small to big
bool sort_realease_time(Task, Task);  //sort vector's release time(arrival time) from small to big
bool running_vector_empty(const vector<Task> *);    //check all running queue is empty of not

int main()
{
    int process_time = 0; //time of schedule
    float hit = 0;          //task terminal before deadline
    float miss = 0;         //task terminal after deadline
    float allWaitTime = 0;  //waiting time of all task

    freopen("input.txt", "r", stdin); //set input.txt as the stdin
    int processor_num, task_num;
    cin >> processor_num >> task_num; //input processor number & task number

    vector<Task> readyvector;              //vector to store ready task
    vector<Processor> provector;           //vector to store processor
    vector<Task> waitvector;               //vector to store waiting task
    vector<Task> runvector[processor_num]; //vector to store running task
    vector<Task> tervector;                //vector to store terminal task

    for (int i = 0; i < processor_num; i++)
    {
        Processor processor;                      //initial each processor
        cin >> processor.id >> processor.ability; //input each processor's ID & ability
        provector.push_back(processor);           //push processor into vector
    }

    for (int i = 0; i < task_num; i++)
    {
        Task task;
        //initial each task
        cin >>
            task.ID; //input each task's parameter
        cin >> task.release_time;
        cin >> task.execution_time;
        cin >> task.deadline;
        cin >> task.period;
        cin >> task.preemption;
        cin >> task.task;
        waitvector.push_back(task); //push task into vector
    }

    //sort readyvector by it's release time to get the first task
    sort(waitvector.begin(), waitvector.end(), sort_realease_time);

    //create string for every process
    string outputstring[processor_num];
    for (int i = 0; i < processor_num; i++) //initial the output string of each processor
    {
        outputstring[i] = outputstring[i] + "Processor: " + to_string(provector[i].id) + "\n";
    }

    //job schedule
    while (1)
    {
        //if ready,wait,run queue is empty -> all task is terminate -> program end
        if (readyvector.size() <= 0 && waitvector.size() <= 0 && running_vector_empty(runvector))
            break;

        //cout << "release time schedule \n";
        if (waitvector.size() > 0)
        {
            //if release time < process time,is means the task is arrived ,so move the task from waitingqueue to readyqueue
            if (waitvector.begin()->release_time <= process_time)
            {
                readyvector.push_back(waitvector[0]);       //move task to ready
                waitvector.erase(waitvector.begin());       //remove task from waiting
                sort(readyvector.begin(), readyvector.end(), sort_execution_time);      //sort the ready queue by execute time
            }
        }

        //working for all processor
        for (int i = 0; i < processor_num; i++)
        {
            //if the run queue of a processor is empty, move a ready task to it
            if (runvector[i].size() <= 0)
            {
                if (readyvector.size() > 0)
                {
                    runvector[i].push_back(readyvector[0]);     //move task to run queue
                    readyvector.erase(readyvector.begin());     //remove task from ready

                    if (runvector[i][0].suspend == 1)           //if suspend is i, it means this task has been remove from run queue, this situation need context switch 
                    {                                           //during context switch, the task whick need to be move to run queue 's waiting time need to be added
                        runvector[i][0].period++;
                        outputstring[i] += "\t" + to_string(process_time) + " Context Switch " + to_string(process_time + 1) + "\n";        //context switch 
                        outputstring[i] += "\t" + to_string(process_time + 1) + " Task" + to_string(runvector[i].begin()->ID) + " ";        //the task move to run queue
                        continue;       //because context switch ,at this moment, this processor can't do anyting else
                    }
                    else
                    {
                        outputstring[i] += "\t" + to_string(process_time) + " Task" + to_string(runvector[i].begin()->ID) + " ";            //else, just move to run queue
                    }
                }
            }

            //context switch
            if (runvector[i].size() > 0)
            {
                //context switch
                if (runvector[i].begin()->execution_time > readyvector.begin()->execution_time + 1)     
                {
                    runvector[i][0].suspend = 1; //set context switch flag
                    readyvector.push_back(runvector[i][0]);         //move running task to ready queue
                    runvector[i].erase(runvector[i].begin());       //remove running task
                    outputstring[i] += to_string(process_time) + "\n\t" + to_string(process_time) + " Context Switch " + to_string(process_time + 1) + "\n";    //output context switch
                    sort(readyvector.begin(), readyvector.end(), sort_execution_time);              //sort ready queue by time

                    runvector[i].push_back(readyvector[0]);                                         //move another to run queue
                    readyvector.erase(readyvector.begin());                                         //remove from ready queue
                    runvector[i][0].period++;      //even if the task has been move to running queue, the task has not been execute yet, so the moved task's waiting time need to be added 
                    outputstring[i] += "\t" + to_string(process_time + 1) + " Task" + to_string(runvector[i].begin()->ID) + " ";            //task 
                    continue;           //because context switch ,at this moment, this processor can't do anyting else
                }
            }

            //task excuting
            if (runvector[i].size() > 0)
            {
                //cout << "excute\n";
                runvector[i][0].execution_time -= 1;        //execute all task in running queue
                if (runvector[i][0].execution_time == 0)    //task terminal
                {
                    if (runvector[i][0].deadline < process_time)    //check if this task terminal before deadline of not
                        ++miss;
                    else
                        ++hit;

                    //cout << "a task terminal\n";
                    tervector.push_back(runvector[i][0]);           //move task to terminal queue
                    runvector[i].erase(runvector[i].begin());       //remove task from running queue
                    outputstring[i] += to_string(process_time + 1) + "\n";      //output task
                }
            }
        }
        //add waiting time
        for (int i = 0; i < readyvector.size(); i++)    
        {
            ++readyvector[i].period;
        }
        //add process time
        ++process_time;
    }

    for (int i = 0; i < processor_num; i++) //print the output string
    {
        cout << outputstring[i];
    }

    //add all waiting time of all task in ready queue
    for (int i = 0; i < tervector.size(); i++)
    {
        allWaitTime += tervector[i].period;
    }

    cout << "Average Waiting Time: " << allWaitTime / (float)task_num << endl;
    cout << "Hit Rate: " << hit / (hit + miss) << endl;

    return 0;
}

bool sort_execution_time(Task task1, Task task2)
{
    return task1.execution_time < task2.execution_time;
}
bool sort_realease_time(Task task1, Task task2)
{
    return task1.release_time < task2.release_time;
}
bool running_vector_empty(const vector<Task> *runvector)
{
    bool ans = true;
    for (int i = 0; i < runvector->size(); i++)
    {
        if (runvector[i].size() > 0)
            ans = false;
    }
    return ans;
}