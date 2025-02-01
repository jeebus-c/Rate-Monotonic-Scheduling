#include <iostream>
#include <pthread.h>
#include <vector>
#include <sstream>
#include <queue>
#include <tuple>
#include <algorithm>
#include <iomanip>
#include <cmath>
//SOURCES:
//Past Reference from PA1: 
    //https://gitgud.io/MadDawg/cosc3360_spring2024_guides/-/blob/master/assignment1guide.md?ref_type=heads#user-content-stuff-you-may-find-helpful 



//will receive input from user and return a vector of strings
std::vector<std::string> get_Inputs(){
  std::vector<std::string> inputs;
  std::string input;
  while(std::getline(std::cin, input)){
    if(!input.empty()){
      inputs.push_back(input);
    }
    else{
      return inputs;
    }
  }
  return inputs;
}

//will hold the data of our tasks, including name, WCET, and period
struct task{
  char name;
  int WCET;
  int period;
  int initial_WCET;
 // task(){}
  task(char n, int w, int p){
    name = n;
    WCET = w;
    period = p;
    initial_WCET = w;
  }
//will be used to build the priority queue 
  bool operator<(const task &other) const {
    if(initial_WCET == 0 & other.initial_WCET ==0){
      return period > other.period;
    }
    if(initial_WCET == 0){
      return initial_WCET < other.initial_WCET;
    }
    if(other.initial_WCET ==0){
      return initial_WCET < other.initial_WCET;
    }
    if(period == other.period){
      return name > other.name;
    }
    return period > other.period;
  }
};

//HELPER FUNCTIONS
//calculates threshold
double calc_threshold(std::vector<task> tasks){
  float num = tasks.size();
  float fraction = 1.0/num;
  double threshold = num *(pow(2,fraction)-1);
  return threshold;
}

//calculates utilization
double calc_utilization(std::vector<task> tasks){
  double utility = 0;
  for(int i = 0; i < tasks.size(); i++){
    double period = tasks.at(i).period;
    double WCET = tasks.at(i).WCET;
    utility=  (WCET/period)+ utility;
  }
  return utility;
}

//helper function for hyperperiod, finds the greatest common divisor
int find_gcd(int a, int b){
  if (b == 0){
    return a;
  }
  return find_gcd(b, a%b);
}

//calculates hyperperiod
int calc_hyperiod(std::vector<task> tasks){
  int hyperperiod = tasks.at(0).period;

  for(int i = 0; i < tasks.size(); i++){
    hyperperiod = (((tasks.at(i).period)* hyperperiod)/(find_gcd(tasks.at(i).period, hyperperiod)));
  }
  return hyperperiod;
}

//will create the schedule diagram-> and return it as a string
//will use priority queue to accomplish this
std::string scheduling_diagram(std::vector<task> &tasks, int hyperperiod){
  std::string diagram = ""; //will be added to based on priority_queue
  std::string answer = ""; //what will actually be returned

  std::priority_queue<task> q; 

  //push tasks into  priority_queue
  for (int i = tasks.size() - 1; i >= 0; i--) {
    q.push(tasks.at(i));
  } 

  //iterates and creates the string diagram
  for(int i = 1; i <= hyperperiod; i++){
    task cur = q.top(); 
    //Work is still needed to be done
    if(cur.initial_WCET > 0){
      diagram+= cur.name;
      cur.initial_WCET--;
      q.pop();
      q.push(cur);
      std::priority_queue<task> temp;
      while(!q.empty()){
        task cur = q.top();
        q.pop();
        if(i%cur.period == 0 && i != 1){
          cur.initial_WCET += cur.WCET;
        }
        temp.push(cur);
      }
      q = temp;
    }
      //Our idle
    else{
      diagram+= "I";
      std::priority_queue<task> temp;
      while(!q.empty()){
        task cur = q.top();
        q.pop();
        if(i%cur.period == 0 && i != 1){
          cur.initial_WCET += cur.WCET;
        }
        temp.push(cur);
      }
      q = temp;
    }
  }
  //take my string "diagram" and use that information to generate my proper scheduling diagram which will be held in answer

  int tracker = 0; //keep track of how many times a task executes
  char current = diagram[0]; 
  for(int i =0; i < diagram.size(); i++){
    if(diagram[i] == current){
      tracker++;
    }
    else{
      std::string s="";
      s= current;
      current = diagram[i];
      if(s == "I"){
        answer += "Idle(" + std::to_string(tracker) + "), ";
      }
      else{
        answer += s + "(" + std::to_string(tracker) + "), ";
      }
      tracker=1;
    }
    if(i == diagram.size()-1){
      std::string s="";
      s = current;
      current = diagram[i];
      if(s == "I"){
        answer += "Idle(" + std::to_string(tracker) + ")";  
      }
      else{
        answer += s + "(" + std::to_string(tracker) + ")";
      }
    }
  }
  return answer;
}

//will convert a stringstream into a string
//that stringstream , will contain the output of the program

std::string part(std::string& input, int iteration){

  std::stringstream input_string(input); //stringstream input
  std:: stringstream ans_sstr; //will hold our output
  std::vector<task> tasks; //will hold our tasks

  //as the name suggests
  char task_name;
  int task_wcet;
  int task_period;
  double threshold;
  double utilization;
  int hyperperiod;

  //will assign name, wcet, and period to a new task in the tasks vector
  while(input_string >> task_name >> task_wcet >> task_period){
    tasks.emplace_back(task_name, task_wcet, task_period); 
  }

  //will hold the values of periods
  std::vector<int> temp;

  //will contain tasks sorted by period <think of this as priority queue>
  std::vector<task> sortedTasks;
  for(int i =0; i < tasks.size(); i++){
    temp.push_back(tasks[i].period);
  }
  //sort periods
  std::sort(temp.begin(), temp.end());

  // based on sorted periods we will sort our tasks appropiately
  //sortedTasks
  for(int i =0; i < temp.size(); i++){
    for(int j = 0; j < temp.size(); j++){
      if(temp[i] == tasks.at(j).period){
        sortedTasks.emplace_back(tasks.at(j).name, tasks.at(j).WCET, tasks.at(j).period);
      }
    }
  }
//calculating all of our...
  threshold = calc_threshold(tasks);
  utilization = calc_utilization(tasks);
  hyperperiod = calc_hyperiod(sortedTasks);

  //The MEAT of the program
  ans_sstr << "CPU " << iteration << std::endl;
  ans_sstr << "Task scheduling information: ";
  for(int i =0; i < tasks.size(); i++){
    if(i != tasks.size()-1){
    ans_sstr << tasks.at(i).name << " (WCET: " << tasks.at(i).WCET << ", Period: " << tasks.at(i).period << "), ";
    }
    else{
      ans_sstr << tasks.at(i).name << " (WCET: " << tasks.at(i).WCET << ", Period: " << tasks.at(i).period << ")";
    }
  }
  ans_sstr << std::endl;
  ans_sstr << "Task set utilization: " << std::fixed << std::setprecision(2) << utilization << std::endl;
  ans_sstr << "Hyperperiod: " << hyperperiod << std::endl;
  ans_sstr << "Rate Monotonic Algorithm execution for CPU" << iteration << ": " << std::endl;

  //Run through the scenarios where a scheduling diagram wouldn't be possible, if none apply, then we'll run the scheduling algorithm
  if(utilization > 1){
    ans_sstr << "The task set is not schedulable" << std::endl;
  }
  else if(utilization > threshold && utilization <=1){
    ans_sstr << "Task set schedulability is unknown" << std::endl;
  }
  else{
    ans_sstr << "Scheduling Diagram for CPU " << iteration << ": "; 
    ans_sstr << scheduling_diagram(tasks,hyperperiod);
    ans_sstr << std::endl;
  }
    return ans_sstr.str();
}

//will hold the data that'll get passed to the pthread_join function in main
struct Arguments{
  std::string input; //line of input
  int iteration; //iteration of the thread
  pthread_mutex_t *bsem1; //mutex semaphore 
  pthread_mutex_t *bsem2; //mutex semaphore
  pthread_cond_t *Wait_turn; //conditional semaphore
  int *turn; 

  Arguments(){
    input = "";
    iteration = 0;
    turn = nullptr;
  }
  Arguments(std::string in, int iter, int *turn, pthread_cond_t *Wait_turn, pthread_mutex_t *bsem1, pthread_mutex_t *bsem2  ){
    input = in;
    iteration = iter;
    this->turn = turn;
    this->Wait_turn = Wait_turn;
    this->bsem1 = bsem1;
    this->bsem2 = bsem2;
  }
};

//function will pass in pthread_join, turns a void pointer into an arguments pointer
void* thread_fun(void * ptr){
  //still inside critical section
  Arguments arg = *(Arguments*) ptr; 
  std::string local_input = arg.input;
  int local_iter = arg.iteration;
  //int *turn = arg.turn;
  
  pthread_mutex_unlock(arg.bsem1);
  std::string output = part(local_input, arg.iteration+1); //create our output
  pthread_mutex_lock(arg.bsem2); //enter critical section
  //if its not your turn you will wait
    while(local_iter != (*arg.turn)){
      pthread_cond_wait(arg.Wait_turn, arg.bsem2);
    }
  pthread_mutex_unlock(arg.bsem2); //exit critical section
  std::cout << output; //output our answer
  pthread_mutex_lock(arg.bsem2); //back to critical section
  (*arg.turn)++; //increase value of turn
  pthread_cond_broadcast(arg.Wait_turn); //unblock remaining threads
  pthread_mutex_unlock(arg.bsem2); //exit critical section
  return nullptr;
}

int main() {
  //declare and initialize 2 mutex semaphores
  pthread_mutex_t bsem1;
  pthread_mutex_t bsem2;
  
  pthread_mutex_init(&bsem1,NULL); //will lock in main, unlock in void function
  pthread_mutex_init(&bsem2,NULL); //will lock and unlock in void function
  static pthread_cond_t Wait_turn; //will be used to have processes wait their turn
  static int turn = 0;
  std::vector<std::string> inputs = get_Inputs(); //will hold all of our inputs
  std::vector<pthread_t> tid(inputs.size()); //will hold n number of threads
  Arguments arg; 

  //create threads-- pass arguments into the threads 
  for(int i =0; i < inputs.size(); i++){
    pthread_mutex_lock(&bsem1); //first critical section
    arg.input = inputs[i];
    arg.iteration = i;
    arg.turn = &turn;
    arg.bsem1 = &bsem1;
    arg.bsem2 = &bsem2;
    arg.Wait_turn = &Wait_turn;
    pthread_create(&tid[i], nullptr, thread_fun, &arg);
  }
  //call on pthread join
  for(int i =0; i < inputs.size(); i++){
    pthread_join(tid[i], nullptr);
  }
}