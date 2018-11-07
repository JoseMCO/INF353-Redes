//Reference: https://github.com/wakwanza/leach
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include <vector>
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <ctype.h>  
#include <string.h>  
#include <time.h>  
#include "hleach.h"
#include "const.h" 
#include <fstream>
#include <iterator>
#include <iostream>
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-variable" 
#pragma GCC diagnostic ignored "-Wunused-value" 
#pragma GCC diagnostic ignored "-Wwrite-strings" 
#pragma GCC diagnostic ignored "-Wparentheses" 

using namespace std;

int NUM_NODES = 50;     // number of nodes in the network  
                        // default is 50  

int NETWORK_X = 100;    // X-size of network  
                        // default is 100  

int NETWORK_Y = 100;    // Y-size of network  
                        // default is 100  

double B_POWER = 0.75;  // initial battery power of sensors  
                        // default is 0.75 

double ALPHA_RATIO = 3.0; //

double B_POWER_ADVANCED = B_POWER * ALPHA_RATIO; // initial battery power of advanced sensors  

                // the percentage of the nodes in the   
                // network that would ideally be cluster   
                // heads during any one round of the   
                // LEACH simulation, default is 0.05  
double CLUSTER_PERCENT = 0.05; //if we change this make changes in threshold formulae and wherever we have put "20"  

                // the total rounds that the simulation  
                // should run for - the network lifetime  
                // default is 2000  
int TOTAL_ROUNDS = 200000;  

                // the distance that the advertisement  
                // phase should broadcast to, in order   
                // to alert other nodes that there is a   
                // cluster head, default is 25.0  
double LEACH_AD_DISTANCE = 25;  

                // the message length of the advertisement  
                // that there is a cluster head  
                // default is 16  
int LEACH_AD_MESSAGE = 16;  

                // the distance for the cluster head to broadcast   
                // the schedule for transmission to the other nodes   
                // in the cluster, default is 25.0  
double SCHEDULE_DISTANCE = 25;  

                // the message length of the schedule which is   
                // sent to the nodes in the cluster during the   
                // scheduling phase of the LEACH simulation,   
                // default is 16.  
int SCHEDULE_MESSAGE = 16;  

void writefile(vector<int> vec);

int BASE_STATION_X_DEFAULT = 50;

int BASE_STATION_Y_DEFAULT = 50;  

int DEAD_NODE = -2;
int MESSAGE_LENGTH = 8;

int TRIALS = 1;

int LEACH_SIMULATION = 1;
int TEEN_SIMULATION = 2;
int SEP_SIMULATION = 3;

double ADVANCED_RATIO = 0.2; // M
double TEEN_THRESHOLD = 0.65;
double TEEN_SOFT_THRESHOLD = 0.75;

struct sensor {  
  short xLoc;        // X-location of sensor  
  short yLoc;        // Y-location of sensor  
  short lPeriods;    // number of periods the sensor has been in use for  
  short ePeriods;    // expected periods the sensor must be operational for  
  double bCurrent;   // current battery power  
  double bPower;     // initial battery power  
  double pAverage;   // average power consumed per transmission period.  
  int round;         // the last round that the sensor served as a cluster head  
  int head;          // stores the index of the cluster head for the sensor to transmit to, set to -1 if the sensor is a cluster head  
  int cluster_members;   // stores the total number of nodes in the cluster, applicable only for cluster head nodes  
  int head_count;    // this contains the count of the number of times a sensor has been the head of a cluster, can be removed for optimization later  
  int advanced;
};  

struct sensor BASE_STATION;  

double computeEnergyTransmit(float distance, int messageLength);  
// computes the energy needed to transmit the message  
// inputs are the distance between the two nodes and  
// the length of the message  

double computeEnergyReceive(int messageLength);  
// computes the energy needed to receive a message   
// input is the message length  

void initializeNetwork(struct sensor network[], int networkId);  
// initializes the network; randomly places nodes within   
// the grid and sets battery power to default value  

float averageEnergy(struct sensor network[]);  
float maxEnergy(struct sensor network[]);  
float minEnergy(struct sensor network[]);  
// computes the average power of the sensors in the   
// network as a percentage of starting power  
// input is a sensor network  

struct sensor * loadConfiguration(char * filename);  
// loads values for the network initialization  
// including starting battery power, size of network,   
// and number of nodes  

int runSimulation(const struct sensor network[], int type, int trial);  

int runLeachSimulation(const struct sensor network[]);  
// takes an initialized sensor network and simulates   
// the LEACH protocol on that network  
// The network is unchanged so that the same network   
// can be passed to other simulator functions  
// The function returns the number of rounds for which the   
// average power of the network remained above the threshold.   

int sensorTransmissionChoice(const struct sensor a);  
// using the NPP, this function determines whether sensor a  
// should transmit in the current round based on the expected  
// number of rounds the network will be in use and the average   
// energy the sensor has used per round thus far in the simulation.  
// Returns 1 if the sensor should transmit, 0 otherwise.   

int main(int argc, char * argv[]) {  
  struct sensor * network; 

  int i = 0;  
  //int j = 0;  
  double total_rounds_LEACH = 0.0;  
  double total_rounds_SEP = 0.0;  
  double total_rounds_TEEN = 0.0;  
  //double average_comparison = 0.0;  
  char* filename=new char[10]; 

  BASE_STATION.xLoc = BASE_STATION_X_DEFAULT;  
  BASE_STATION.yLoc = BASE_STATION_Y_DEFAULT;  

  cout << "proper file" << endl;   

  network = (struct sensor *) malloc(60 * sizeof(struct sensor));

  for(i = 0; i < TRIALS; i++){  
    initializeNetwork(network, i);  
    int rounds_LEACH = runSimulation(network, LEACH_SIMULATION, i);  
    int rounds_SEP = runSimulation(network, SEP_SIMULATION, i);  
    int rounds_TEEN = runSimulation(network, TEEN_SIMULATION, i);  

    total_rounds_LEACH += rounds_LEACH;
    total_rounds_SEP += rounds_SEP;
    total_rounds_TEEN += rounds_TEEN;

    printf("Trial no %d\n", i);  
    printf("The LEACH simulation was able to remain viable for %d rounds\n", rounds_LEACH);  
    printf("The SEP simulation was able to remain viable for %d rounds\n", rounds_SEP);  
    printf("The TEEN simulation was able to remain viable for %d rounds\n", rounds_TEEN);
  }  

  total_rounds_LEACH /= TRIALS;
  total_rounds_SEP /= TRIALS;
  total_rounds_TEEN /= TRIALS;

  printf("AVG results\n");  
  printf("The LEACH simulation was able to remain viable for %.2f rounds\n", total_rounds_LEACH);  
  printf("The SEP simulation was able to remain viable for %.2f rounds\n", total_rounds_SEP);  
  printf("The TEEN simulation was able to remain viable for %.2f rounds\n", total_rounds_TEEN);

  return 0;  

} // end main function  


int runSimulation(const struct sensor network[], int type, int trial){  
  // Preconditions:   the network variable contains an initiailized    
  //          sensor network and all global variables have been   
  //          set using the loadConfiguration function. The simulation   
  //          runs a number of times equal to the TOTAL_ROUNDS variable.  
  // Postconditions:      the LEACH protocol simulation has been   
  //              run on the supplied network although the instance   
  //          of that variable within main memory has NOT been   
  //          modified. Output of statistics is currently to the   
  //          screen.  

  struct sensor * network_struct;      // wireless sensor network to run sim on  

  int i = 0;              // indexing variables  
  int j = 0;  
  int k = 0;  
  int closest = 0;  

  int round = 0;              // current round  
  int failed_transmit = 0;        // round where a failed transmission occurred  

  int testing = 0;            // testing variable, TO BE REMOVED  
  int bits_transmitted = 0;       // count of bits transmitted  
  int power = FALSE;  
  int temp_cluster_members = 0;  

  int sent = 0;

  double average_energy = 0.0;  
  double distance_X_old = 0.0;  
  double distance_Y_old = 0.0;  
  double distance_old = 0.0;  
  double distance_X_new = 0.0;  
  double distance_Y_new = 0.0;  
  double distance_new = 0.0;  
  int recent_round = 20;  
  double threshold = CLUSTER_PERCENT/(1-(CLUSTER_PERCENT*(round % 20)));
  double random_number;  
  int cluster_head_count = 0;  
  double percent_found = 0.0; 
  double mid_value = 0.0;
  bool flag = 1 ;
  vector<double>energy;
  vector<double>profile;

  //old :: network_struct = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor)); 
  network_struct = (struct sensor *) malloc(60 * sizeof(struct sensor));  
  //network_struct = new struct sensor[NUM_NODES];  
  // copy the contents of the passed network to a temporary   
  // network so the same network can be passed to different   
  // protocol simulations  

  for(i = 0; i  < NUM_NODES; i++){  
    network_struct[i].xLoc = network[i].xLoc;  
    network_struct[i].yLoc = network[i].yLoc;   
    network_struct[i].cluster_members = 0;  
    network_struct[i].ePeriods = network[i].ePeriods;   
    network_struct[i].bCurrent = network[i].bCurrent;   
    network_struct[i].bPower = network[i].bPower;   
    network_struct[i].pAverage = network[i].pAverage;   
    network_struct[i].advanced = network[i].advanced;   
  }  


  std::string filename = "leach_simulation_";
  if (type == SEP_SIMULATION) {
    filename = "sep_simulation_";
  }
  else if (type == TEEN_SIMULATION) {
    filename = "teen_simulation_";
  }

  filename = "output/" + filename + std::to_string(round) + ".csv";

  ofstream logFile;
  logFile.open(filename);
  logFile << "Round,nodes alive,sent,avg energy,max energy,min energy\n";


  //printf("\nRunning the LEACH Transmission Simulation \n");  
  // our iterating loop runs of total rounds, this is   
  // the expected lifetime of the network  

  float avgEnergy = averageEnergy(network_struct);

  while(avgEnergy > .10) {
        // here we recalculate all the variables   
        // which are round dependent
      //double check = (CLUSTER_PERCENT*float(round % 20));
      //cout << "check: " << check << endl;  
    double threshold = CLUSTER_PERCENT/(1-(CLUSTER_PERCENT*(round % int(1.0/CLUSTER_PERCENT) )));
    cluster_head_count = 0;

    double P_nrm = CLUSTER_PERCENT/(1+ ALPHA_RATIO * ADVANCED_RATIO);
    double P_adv = P_nrm * (1 + ALPHA_RATIO);

    double threshold_nrm = P_nrm/(1-(P_nrm*(round % int(1.0/P_nrm))));
    double threshold_adv = P_adv/(1-(P_adv*(round % int(1.0/P_adv))));

      //cout << "round: " << round << " threshold: "  << threshold << endl;  
        // advertisement phase  
        // we determine which nodes will be cluster heads  
    for(i = 0; i < NUM_NODES; i++){  
      if(network_struct[i].round < (j - recent_round) || (j - recent_round == 0)){  
        //cout << "loop1 " << endl;
        if(network_struct[i].head != DEAD_NODE){  
          random_number = .00001*(rand() % 100000);          
          //cout << "random_number " << random_number << endl;
          if (type == SEP_SIMULATION){
            if (network_struct[i].advanced) {
              if(random_number <= threshold_adv) {  
                // the random number selected is less   
                // than the threshold so the node becomes   
                // a cluster head for the round  
                network_struct[i].head_count++;  
                // update the round variable   
                // so we know that this sensor was   
                // last a cluster head at round i  
                network_struct[i].round = j;  
                network_struct[i].head = -1;  
                // store the index of the node in the   
                // cluster_heads array  
                // increment the cluster_head_count  
                cluster_head_count++;
              }
              else{
                if(random_number <= threshold_nrm){  
                  // the random number selected is less   
                  // than the threshold so the node becomes   
                  // a cluster head for the round  
                  network_struct[i].head_count++;  
                  // update the round variable   
                  // so we know that this sensor was   
                  // last a cluster head at round i  
                  network_struct[i].round = j;  
                  network_struct[i].head = -1;  
                  // store the index of the node in the   
                  // cluster_heads array  
                  // increment the cluster_head_count  
                  cluster_head_count++;
                }
              }
            }
          }
          else{
            if(random_number <= threshold){  
              // the random number selected is less   
              // than the threshold so the node becomes   
              // a cluster head for the round  
              network_struct[i].head_count++;  
              // update the round variable   
              // so we know that this sensor was   
              // last a cluster head at round i  
              network_struct[i].round = j;  
              network_struct[i].head = -1;  
              // store the index of the node in the   
              // cluster_heads array  
              // increment the cluster_head_count  
              cluster_head_count++; 
            }
          }  
        }  
      }  
    } 

    percent_found += (double)cluster_head_count/(double)NUM_NODES;

    // now the cluster heads must transmit the fact that they   
    // are cluster heads to the network, this will be a constant   
    // transmit energy, during this period the other nodes must   
    // keep their receivers on - which has an energy cost, again   
    // this is constant  
    for(i = 0; i  < NUM_NODES; i++){  
      if(network_struct[i].head == -1){  
        network_struct[i].bCurrent -=   
        computeEnergyTransmit(LEACH_AD_DISTANCE,  
          LEACH_AD_MESSAGE);  
      }  
      else{  
        network_struct[i].bCurrent -=   
        computeEnergyReceive(LEACH_AD_MESSAGE);  
      }  
    }  



    // CLUSTER SET-UP PHASE  
        // for this phase, all non-cluster heads determine to which   
        // cluster they will broadcast and alert the cluster head to that  


    for(i = 0; i  < NUM_NODES; i++){  
      closest = -1;  
      if((network_struct[i].head != -1) &&   
        network_struct[i].head != DEAD_NODE){  
                            // if the node's round is not equal to the    
                            // current round, the node is not a cluster  
                            // head and we must find a cluster head for  
                            // the node to transmit to  
        for(k = 0; k < NUM_NODES; k++){  
          if(network_struct[k].head == -1 && closest != -1){  
            distance_X_old = network_struct[i].xLoc - network_struct[closest].xLoc;  
            distance_Y_old = network_struct[i].yLoc - network_struct[closest].yLoc;  
            distance_old = sqrt(pow(distance_X_old, 2) + pow(distance_Y_old, 2));  
            distance_X_new = network_struct[i].xLoc - network_struct[k].xLoc;  
            distance_Y_new = network_struct[i].yLoc - network_struct[k].yLoc;  
            distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
            if(distance_new < distance_old)  
              closest = k;  
          }  
          else if(network_struct[k].head == -1 && closest == -1){  
            closest = k;  
          }  
        }  

        network_struct[i].head = closest;  
        network_struct[closest].cluster_members++;  
      }  
    }  

    // SCHEDULE CREATION  
        // the cluster head then creates a schedule telling the nodes   
        // when they can transmit; the schedule is broadcast to the nodes  
        // but no simulation of this is neccescary outside of the loss   
        // of battery power for broadcasting the schedule, which is a constant  
    for(i = 0; i <= NUM_NODES; i++){  
      if(network_struct[i].head == -1){  
                // if the node is going to be a cluster head, it transmits   
                // the schedule to the other nodes  
        network_struct[i].bCurrent -=   
        computeEnergyTransmit(SCHEDULE_DISTANCE, SCHEDULE_MESSAGE);  
      }  
      else  
        network_struct[i].bCurrent -=   
      computeEnergyReceive(SCHEDULE_MESSAGE);  
    }  



    // DATA TRANSMISSION  
        // non cluster heads send their data to the cluster heads who then   
        // broadcast the data to the base station  
    for(i = 0; i < NUM_NODES; i++){  
      network_struct[i].lPeriods++;  
      int does_transmit = 1;
      if (type == TEEN_SIMULATION){
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        if (r < TEEN_THRESHOLD){
          does_transmit = 0;
        }
      }

      if(network_struct[i].head != -1 && network_struct[i].head != DEAD_NODE && does_transmit){  
        distance_X_new = network_struct[i].xLoc - network_struct[network_struct[i].head].xLoc;  
        distance_Y_new = network_struct[i].yLoc - network_struct[network_struct[i].head].yLoc;  
        distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2)));  
        network_struct[i].bCurrent -= computeEnergyTransmit(distance_new, MESSAGE_LENGTH);  
        network_struct[network_struct[i].head].bCurrent -= computeEnergyReceive(MESSAGE_LENGTH);

        if(network_struct[i].bCurrent < 0.0 && network_struct[i].head != -1){  
          network_struct[i].head = DEAD_NODE;  
        }  
      } 
    }  


    for(i = 0; i <= NUM_NODES; i++){  
      if(network_struct[i].head == -1){  
        distance_X_new = network_struct[i].xLoc - BASE_STATION.xLoc;  
        distance_Y_new = network_struct[i].yLoc - BASE_STATION.yLoc;  
        distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
        double energy_enough = network_struct[i].bCurrent - computeEnergyTransmit(distance_new, (MESSAGE_LENGTH * (network_struct[i].cluster_members+1)));

    //            network_struct[i].bCurrent -= computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * (network_struct[i].cluster_members+1)));  
    //            if(network_struct[i].bCurrent > 0.0){  
        if(round < 2001){
          double required_energy = computeEnergyTransmit(distance_new, MESSAGE_LENGTH);
          network_struct[network_struct[i].head].bCurrent -= computeEnergyReceive(MESSAGE_LENGTH);
          energy.push_back(required_energy);
        }
        if(energy_enough > 0.0){
          network_struct[i].bCurrent -= computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * (network_struct[i].cluster_members+1)));
          bits_transmitted += (MESSAGE_LENGTH * (network_struct[i].cluster_members+1));  
        }  
        else{  
          failed_transmit++;  
          //  network_struct[i].head = DEAD_NODE;
          if(flag){
            //cout << "modified_LEACH's first node dies: " << round << endl;
            flag = 0;
          }
          temp_cluster_members = network_struct[i].cluster_members + 1;  
          network_struct[i].bCurrent += computeEnergyTransmit(distance_new, (MESSAGE_LENGTH * (network_struct[i].cluster_members+1)));  
          while(power == FALSE){  
            if((network_struct[i].bCurrent - computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * temp_cluster_members))) > 0){  
              bits_transmitted += (MESSAGE_LENGTH * temp_cluster_members);  
              power = TRUE;  
            }  
            else {
              temp_cluster_members--;  
            }
            if(temp_cluster_members == 0){
              power == TRUE;  
            } 
          }  
        }  
      }  
    }  
    double max = 0.0000;
    for(unsigned int i = 0; i < energy.size(); i++){
      if ((energy[i]) > max) max =  energy[i];
    }
      //cout << max  << endl; 
    profile.push_back(max);

        // round has completed, increment the round count  
    for(i = 0; i <= NUM_NODES; i++){  
      network_struct[i].cluster_members = 0;  
      if(network_struct[i].bCurrent > 0.0) network_struct[i].head = 0;  
    }  

    cluster_head_count = 0;  


    avgEnergy = averageEnergy(network_struct);
    if (round%10 == 0){
      double mxEnergy = maxEnergy(network_struct);
      double mnEnergy = minEnergy(network_struct);
      // logFile << "round,alive,sent,avg energy,max energy,min energy\n";
      logFile << (std::to_string(round)+",0,0,"+std::to_string(avgEnergy)+","+std::to_string(mxEnergy)+","+std::to_string(mnEnergy)+"\n");
    }

    round += 1;  
    j = round;
  } 

  logFile.close();
  
  return round;  
} // end runLeachSimulation function 


int runLeachSimulation(const struct sensor network[]){  
  // Preconditions:   the network variable contains an initiailized    
  //          sensor network and all global variables have been   
  //          set using the loadConfiguration function. The simulation   
  //          runs a number of times equal to the TOTAL_ROUNDS variable.  
  // Postconditions:      the LEACH protocol simulation has been   
  //              run on the supplied network although the instance   
  //          of that variable within main memory has NOT been   
  //          modified. Output of statistics is currently to the   
  //          screen.  

  struct sensor * network_LEACH;      // wireless sensor network to run sim on  

  int i = 0;              // indexing variables  
  int j = 0;  
  int k = 0;  
  int closest = 0;  

  int round = 0;              // current round  
  int failed_transmit = 0;        // round where a failed transmission occurred  

  int testing = 0;            // testing variable, TO BE REMOVED  
  int bits_transmitted = 0;       // count of bits transmitted  
  int power = FALSE;  
  int temp_cluster_members = 0;  

  double average_energy = 0.0;  
  double distance_X_old = 0.0;  
  double distance_Y_old = 0.0;  
  double distance_old = 0.0;  
  double distance_X_new = 0.0;  
  double distance_Y_new = 0.0;  
  double distance_new = 0.0;  
  int recent_round = 20;  
  double threshold = CLUSTER_PERCENT/(1-(CLUSTER_PERCENT*(round % 20)));
  double random_number;  
  int cluster_head_count = 0;  
  double percent_found = 0.0; 
  double mid_value = 0.0;
  bool flag = 1 ;
  vector<double>energy;
  vector<double>profile;
  vector<double> dir;
  //old :: network_LEACH = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor)); 
  network_LEACH = (struct sensor *) malloc(60 * sizeof(struct sensor));  
  //network_LEACH = new struct sensor[NUM_NODES];  
  // copy the contents of the passed network to a temporary   
  // network so the same network can be passed to different   
  // protocol simulations  


  for(i = 0; i  < NUM_NODES; i++){  
    network_LEACH[i].bPower = network[i].bPower;  
    network_LEACH[i].xLoc = network[i].xLoc;  
    network_LEACH[i].cluster_members = 0;  
    network_LEACH[i].yLoc = network[i].yLoc;   
    network_LEACH[i].ePeriods = network[i].ePeriods;   
    network_LEACH[i].bCurrent = network[i].bCurrent;   
    network_LEACH[i].bPower = network[i].bPower;   
    network_LEACH[i].pAverage = network[i].pAverage;   
  }  


  //printf("\nRunning the LEACH Transmission Simulation \n");  
  // our iterating loop runs of total rounds, this is   
  // the expected lifetime of the network  

  while(averageEnergy(network_LEACH) > .10) {
        // here we recalculate all the variables   
        // which are round dependent
      //double check = (CLUSTER_PERCENT*float(round % 20));
      //cout << "check: " << check << endl;  
    double threshold = CLUSTER_PERCENT/(1-(CLUSTER_PERCENT*(round % 20)));
    cluster_head_count = 0;  
      //cout << "round: " << round << " threshold: "  << threshold << endl;  
        // advertisement phase  
        // we determine which nodes will be cluster heads  
    for(i = 0; i < NUM_NODES; i++){  
      if(network_LEACH[i].round < (j - recent_round) || (j - recent_round == 0)){  
        //cout << "loop1 " << endl;
        if(network_LEACH[i].head != DEAD_NODE){  
          random_number = .00001*(rand() % 100000);          
          //cout << "random_number " << random_number << endl;
          if(random_number <= threshold){  
            // the random number selected is less   
            // than the threshold so the node becomes   
            // a cluster head for the round  
            network_LEACH[i].head_count++;  
            // update the round variable   
            // so we know that this sensor was   
            // last a cluster head at round i  
            network_LEACH[i].round = j;  
            network_LEACH[i].head = -1;  
            // store the index of the node in the   
            // cluster_heads array  
            // increment the cluster_head_count  
            cluster_head_count++; 
          }  
        }  
      }  
    }  

    percent_found += (double)cluster_head_count/(double)NUM_NODES;

    // now the cluster heads must transmit the fact that they   
    // are cluster heads to the network, this will be a constant   
    // transmit energy, during this period the other nodes must   
    // keep their receivers on - which has an energy cost, again   
    // this is constant  
    for(i = 0; i  < NUM_NODES; i++){  
      if(network_LEACH[i].head == -1){  
        network_LEACH[i].bCurrent -=   
        computeEnergyTransmit(LEACH_AD_DISTANCE,  
          LEACH_AD_MESSAGE);  
      }  
      else{  
        network_LEACH[i].bCurrent -=   
        computeEnergyReceive(LEACH_AD_MESSAGE);  
      }  
    }  



    // CLUSTER SET-UP PHASE  
        // for this phase, all non-cluster heads determine to which   
        // cluster they will broadcast and alert the cluster head to that  


    for(i = 0; i  < NUM_NODES; i++){  
      closest = -1;  
      if((network_LEACH[i].head != -1) &&   
        network_LEACH[i].head != DEAD_NODE){  
                            // if the node's round is not equal to the    
                            // current round, the node is not a cluster  
                            // head and we must find a cluster head for  
                            // the node to transmit to  
        for(k = 0; k < NUM_NODES; k++){  
          if(network_LEACH[k].head == -1 && closest != -1){  
            distance_X_old = network_LEACH[i].xLoc - network_LEACH[closest].xLoc;  
            distance_Y_old = network_LEACH[i].yLoc - network_LEACH[closest].yLoc;  
            distance_old = sqrt(pow(distance_X_old, 2) + pow(distance_Y_old, 2));  
            distance_X_new = network_LEACH[i].xLoc - network_LEACH[k].xLoc;  
            distance_Y_new = network_LEACH[i].yLoc - network_LEACH[k].yLoc;  
            distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
            if(distance_new < distance_old)  
              closest = k;  
          }  
          else if(network_LEACH[k].head == -1 && closest == -1){  
            closest = k;  
          }  
        }  

        network_LEACH[i].head = closest;  
        network_LEACH[closest].cluster_members++;  
      }  
    }  

    // SCHEDULE CREATION  
        // the cluster head then creates a schedule telling the nodes   
        // when they can transmit; the schedule is broadcast to the nodes  
        // but no simulation of this is neccescary outside of the loss   
        // of battery power for broadcasting the schedule, which is a constant  
    for(i = 0; i <= NUM_NODES; i++){  
      if(network_LEACH[i].head == -1){  
                // if the node is going to be a cluster head, it transmits   
                // the schedule to the other nodes  
        network_LEACH[i].bCurrent -=   
        computeEnergyTransmit(SCHEDULE_DISTANCE, SCHEDULE_MESSAGE);  
      }  
      else  
        network_LEACH[i].bCurrent -=   
      computeEnergyReceive(SCHEDULE_MESSAGE);  
    }  



    // DATA TRANSMISSION  
        // non cluster heads send their data to the cluster heads who then   
        // broadcast the data to the base station  
    for(i = 0; i < NUM_NODES; i++){  
      network_LEACH[i].lPeriods++;  
      if(network_LEACH[i].head != -1 && network_LEACH[i].head != DEAD_NODE){  
        distance_X_new = network_LEACH[i].xLoc - network_LEACH[network_LEACH[i].head].xLoc;  
        distance_Y_new = network_LEACH[i].yLoc - network_LEACH[network_LEACH[i].head].yLoc;  
        distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2)));  
        network_LEACH[i].bCurrent -=   
        computeEnergyTransmit(distance_new, MESSAGE_LENGTH);  
        network_LEACH[network_LEACH[i].head].bCurrent -=  
        computeEnergyReceive(MESSAGE_LENGTH);

        if(network_LEACH[i].bCurrent < 0.0 && network_LEACH[i].head != -1){  
          network_LEACH[i].head = DEAD_NODE;  
        }  
      }  
    }  


    for(i = 0; i <= NUM_NODES; i++){  
      if(network_LEACH[i].head == -1){  
        distance_X_new = network_LEACH[i].xLoc - BASE_STATION.xLoc;  
        distance_Y_new = network_LEACH[i].yLoc - BASE_STATION.yLoc;  
        distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
        double energy_enough = network_LEACH[i].bCurrent - computeEnergyTransmit(distance_new, (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));

    //            network_LEACH[i].bCurrent -= computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));  
    //            if(network_LEACH[i].bCurrent > 0.0){  
        if(round < 2001){
          double required_energy = computeEnergyTransmit(distance_new, MESSAGE_LENGTH);
          network_LEACH[network_LEACH[i].head].bCurrent -= computeEnergyReceive(MESSAGE_LENGTH);
          energy.push_back(required_energy);
        }
        if(energy_enough > 0.0){
          network_LEACH[i].bCurrent -= computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));
          bits_transmitted += (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1));  
        }  
        else{  
          failed_transmit++;  
          //  network_LEACH[i].head = DEAD_NODE;
          if(flag){
            //cout << "modified_LEACH's first node dies: " << round << endl;
            flag = 0;
          }
          temp_cluster_members = network_LEACH[i].cluster_members + 1;  
          network_LEACH[i].bCurrent += computeEnergyTransmit(distance_new, (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));  
          while(power == FALSE){  
            if((network_LEACH[i].bCurrent - computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * temp_cluster_members))) > 0){  
              bits_transmitted += (MESSAGE_LENGTH * temp_cluster_members);  
              power = TRUE;  
            }  
            else {
              temp_cluster_members--;  
            }
            if(temp_cluster_members == 0){
              power == TRUE;  
            } 
          }  
        }  
      }  
    }  
    double max = 0.0000;
    for(unsigned int i = 0; i < energy.size(); i++){
      if ((energy[i]) > max) max =  energy[i];
    }
      //cout << max  << endl; 
    profile.push_back(max);

        // round has completed, increment the round count  
    for(i = 0; i <= NUM_NODES; i++){  
      network_LEACH[i].cluster_members = 0;  
      if(network_LEACH[i].bCurrent > 0.0) network_LEACH[i].head = 0;  
    }  

    cluster_head_count = 0;  

    dir.push_back(averageEnergy(network_LEACH));
        //if(round == 1000 ) cout  << "network_pwr: " << averageEnergy(network_LEACH) << endl;
    round += 1;  
    j = round;
  } 
  
  ofstream d_stats;
  d_stats.open("leach_simulation.txt");
  //d_stats << "leach_simulation" << endl;
  for(unsigned int i = 0; i < dir.size(); i++) d_stats << dir[i] << endl;//d_stats << dir[i] << endl ;
    //cout << dir.size() << endl;


        //}
  //cout << "LEACH: " << round << endl;
  //free(network_LEACH);  
  //printf("The average energy of the network remained above 10%% for %d tranmission periods\n", round);  

  return round;  
} // end runLeachSimulation function 


double computeEnergyTransmit(float distance, int messageLength){  
// Preconditions:   distance contains the distance between the transmitting   
//                  node and the receiving node. messageLength contains   
//                  the length of the message in bits.  
// Postconditions:  the total energy consumed by the transmission is   
//                  returned  

  float E_elec = 50 * pow(10,-9);  
  float epsilon_amp = 100 * pow(10,-12);  
  double EnergyUse = 0.00;  

  EnergyUse = (messageLength * E_elec) +   
  (messageLength * epsilon_amp * pow(distance,2));   

  return EnergyUse;  

}           // end computeEnergyTransmit function  

double computeEnergyReceive(int messageLength) {  
// Preconditions:   messageLength contains the length of the  
//                  message in bits to be received by the station.  
// Postconditions:  the total energy consumed by the reception   
//              of the transmission is returned.      
  return (messageLength * (50 * pow(10,-9)));  
}           // end computeEnergyReceive function  


void initializeNetwork(struct sensor network[], int networkId) {  
// Preconditions:   network is an unitialized sensor network.  
// Postconditions:  network is an initialized sensor network   
//                  whose values are loaded from global varibles   
//          which are set using the loadConfiguration   
//          function or the defaults are used  

  // std::string filename = "Network_";
  // filename += std::to_string(networkId);
  // filename += ".xml";
  // ns3::AnimationInterface anim (filename); 

  int i = 0;  
  srand((unsigned int) time(0));  

  for(i = 0; i < NUM_NODES; i++) {  
    network[i].xLoc = rand() % NETWORK_X;  
    network[i].yLoc = rand() % NETWORK_Y;  
    network[i].lPeriods = 0;  
    network[i].ePeriods = TOTAL_ROUNDS;  
    network[i].bCurrent = B_POWER;  
    network[i].bPower = B_POWER;  
    network[i].pAverage = 0.00;  
    network[i].round = FALSE;  
    network[i].head = FALSE;  
    network[i].advanced = FALSE;   

    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    if (r < ADVANCED_RATIO){
      network[i].bCurrent = B_POWER_ADVANCED;  
      network[i].bPower = B_POWER_ADVANCED;  
      network[i].advanced = TRUE;   
    }
  }  

}           // end initializeNetwork function  

float averageEnergy(struct sensor network[]) {  
// Preconditions:   network is an initialized sensor network  
// Postconditions:  the average percentage of power in the   
//                  batteries across the network is returned.  
//                  i.e. 0.90 means that the average battery   
//                  in the network is at 90% of its original   
//                  power  
  float average = 0.0;  
  float starting_power = 0.00;  
  float current_power = 0.00;  
  int i = 0;  

  for(i = 0; i <= NUM_NODES; i++) {  
    starting_power += network[i].bPower;  
    current_power += network[i].bCurrent;  
  }  

  return current_power/starting_power;  
}           // end averageEnergy function 

float maxEnergy(struct sensor network[]) { 
  float max = 0.0;   

  for(int i = 0; i <= NUM_NODES; i++) {  
    double pwr = network[i].bCurrent/network[i].bPower;
    if (pwr > max)
    {
      max = pwr;
    }
  }  

  return max;  
}           // end maxEnergy function 

float minEnergy(struct sensor network[]) { 
  float min = B_POWER_ADVANCED+1;   

  for(int i = 0; i <= NUM_NODES; i++) {  
    if (network[i].head == DEAD_NODE) {
      return 0.0;
    }
    double pwr = network[i].bCurrent/network[i].bPower;
    if (pwr < min && pwr >= 0)
    {
      min = pwr;
    }
  }  

  return min;  
}           // end minEnergy function  

int sensorTransmissionChoice(const struct sensor a){  
// Preconditions:   a is an initialized sensor  
// Postconditions:  1 is returned if a should transmit for   
//          the current round, 0 otherwise.  
  int remaining_periods = 0;  

  remaining_periods = a.ePeriods - a.lPeriods;  
  if((remaining_periods * a.pAverage) > a.bCurrent)  
    return 0;  
  else  
    return 1;  
}