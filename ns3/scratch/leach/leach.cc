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


int NUM_NODES = 200;     // number of nodes in the network  


                        // default is 50  

int NETWORK_X = 200;    // X-size of network  
                        // default is 100  

int NETWORK_Y = 200;    // Y-size of network  
                        // default is 100  

double B_POWER = 0.75;  // initial battery power of sensors  
                        // default is 0.75 

double ALPHA_RATIO = 1.333; //

double B_POWER_ADVANCED = B_POWER * ALPHA_RATIO; // initial battery power of advanced sensors  
  
double CLUSTER_PERCENT = 0.05;  
int EXPECTED_CH = NUM_NODES*0.05;  
                // the percentage of the nodes in the   
                // network that would ideally be cluster   
                // heads during any one round

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

int BASE_STATION_X_DEFAULT = NETWORK_X/2;

int BASE_STATION_Y_DEFAULT = NETWORK_Y/2;  

int DEAD_NODE = -2;
int CLUSTER_HEAD = -1;

int MESSAGE_LENGTH = 8;
int BROADCAST_MESSAGE_LENGTH = 16;

int TRIALS = 5;

int LEACH_SIMULATION = 1;
int TEEN_SIMULATION = 2;
int MOD_SIMULATION = 3;
int ADV_SIMULATION = 4;
int TAM_SIMULATION = 5;

int REDUCED_ENERGY = 1;

double ADVANCED_RATIO = 0.2; // M
double TEEN_THRESHOLD = 0.8;
double TEEN_SOFT_THRESHOLD = 0.02;

double MOD_THRESHOLD = 0.8;

int LOG_STEPS = 20;

struct Sensor {  
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
  double temperature;
};  

Sensor BASE_STATION;  

double computeEnergyTransmit(float distance, int messageLength, int reduced);  
// computes the energy needed to transmit the message  
// inputs are the distance between the two nodes and  
// the length of the message  

double computeEnergyReceive(int messageLength);  
// computes the energy needed to receive a message   
// input is the message length  

void initializeNetwork(Sensor* network, int networkId);  
// initializes the network; randomly places nodes within   
// the grid and sets battery power to default value  

int aliveCount(Sensor network[]);

float averageEnergy(Sensor network[]);  
float maxEnergy(Sensor network[]);  
float minEnergy(Sensor network[]);  
// computes the average/min/max power of the sensors in the   
// network as a percentage of starting power  
// input is a sensor network   

int runSimulation(Sensor* network, int type, int trial);    

int main(int argc, char * argv[]) {  
  Sensor network[NUM_NODES]; 

  int i = 0;  
  //int j = 0;  
  double total_rounds_LEACH = 0.0;  
  double total_rounds_MOD = 0.0;  
  double total_rounds_TEEN = 0.0;  
  double total_rounds_ADV = 0.0;  
  double total_rounds_TAM = 0.0;  
  //double average_comparison = 0.0;  

  BASE_STATION.xLoc = BASE_STATION_X_DEFAULT;  
  BASE_STATION.yLoc = BASE_STATION_Y_DEFAULT;  

  for(i = 0; i < TRIALS; i++){  
    initializeNetwork(network, i);  
    
    cout << "Running simulations" << endl; 
    
    int rounds_LEACH = runSimulation(network, LEACH_SIMULATION, i);  
    int rounds_TEEN = runSimulation(network, TEEN_SIMULATION, i);  
    int rounds_MOD = runSimulation(network, MOD_SIMULATION, i);  
    int rounds_ADV = runSimulation(network, ADV_SIMULATION, i);   
    int rounds_TAM = runSimulation(network, TAM_SIMULATION, i);   

    total_rounds_LEACH += rounds_LEACH;
    total_rounds_TEEN += rounds_TEEN;
    total_rounds_MOD += rounds_MOD;
    total_rounds_ADV += rounds_ADV; 
    total_rounds_TAM += rounds_TAM; 

    printf("Trial no %d\n", i);  
    printf("The LEACH simulation was able to remain viable for %d rounds\n", rounds_LEACH);  
    printf("The TEEN simulation was able to remain viable for %d rounds\n", rounds_TEEN);
    printf("The MOD simulation was able to remain viable for %d rounds\n", rounds_MOD);  
    printf("The ADV simulation was able to remain viable for %d rounds\n", rounds_ADV);   
    printf("The TAM simulation was able to remain viable for %d rounds\n", rounds_TAM);   
  }  

  total_rounds_LEACH /= TRIALS;
  total_rounds_TEEN /= TRIALS;
  total_rounds_MOD /= TRIALS;
  total_rounds_ADV /= TRIALS; 
  total_rounds_TAM /= TRIALS; 

  printf("AVG results\n");  
  printf("The LEACH simulation was able to remain viable for %.2f rounds\n", total_rounds_LEACH);  
  printf("The TEEN simulation was able to remain viable for %.2f rounds\n", total_rounds_TEEN);
  printf("The MOD simulation was able to remain viable for %.2f rounds\n", total_rounds_MOD);  
  printf("The ADV simulation was able to remain viable for %.2f rounds\n", total_rounds_ADV);   
  printf("The TAM simulation was able to remain viable for %.2f rounds\n", total_rounds_TAM);   

  return 0;  

} // end main function  


int runSimulation(Sensor* network, int type, int trial){ 

  Sensor network_struct[NUM_NODES];      // wireless sensor network to run sim on  

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
  double random_number;  
  int cluster_head_count = 0;  
  double percent_found = 0.0; 
  double mid_value = 0.0;
  bool flag = 1 ;
  int msg_count = 0;
  int ovh_count = 0;
  int los_count = 0;

  //old :: network_struct = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor)); 
  // network_struct = (struct sensor *) malloc(((int)NUM_NODES) * sizeof(struct sensor));  

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
    network_struct[i].temperature = -1;
  }  


  std::string filename = "leach_simulation_";
  if (type == TEEN_SIMULATION) {
    filename = "teen_simulation_";
  }
  else if (type == MOD_SIMULATION) {
    filename = "mod_simulation_";
  }
  else if (type == ADV_SIMULATION) {
    filename = "adv_simulation_";
  }
  else if (type == TAM_SIMULATION) {
    filename = "tam_simulation_";
  }

  filename = "output/" + filename + std::to_string(trial) + ".csv";

  ofstream logFile;
  logFile.open(filename);
  logFile << "\"Round\",\"nodes alive\",\"sent\",\"overhead\",\"avg energy\",\"max energy\",\"min energy\"\n";


  //printf("\nRunning the LEACH Transmission Simulation \n");  
  // our iterating loop runs of total rounds, this is   
  // the expected lifetime of the network  

  float avgEnergy = averageEnergy(network_struct);

  while(avgEnergy > .10) {
        // here we recalculate all the variables   
        // which are round dependent
      //double check = (CLUSTER_PERCENT*float(round % 20));
      //cout << "check: " << check << endl;  
    double threshold = EXPECTED_CH/(1-(EXPECTED_CH*(round % int(NUM_NODES/EXPECTED_CH) )));
    cluster_head_count = 0;
 
    // advertisement phase  
    // we determine which nodes will be cluster heads  
    for(i = 0; i < NUM_NODES; i++){  
      if (
        (type == MOD_SIMULATION || type == TAM_SIMULATION) &&
        (network_struct[i].head == CLUSTER_HEAD) &&
        (network_struct[i].bCurrent/network_struct[i].bPower >= MOD_THRESHOLD)
      ) {
        network_struct[i].head_count++;  
        network_struct[i].round = j;  
        network_struct[i].head = CLUSTER_HEAD;  
        cluster_head_count++; 
      }
      else if(network_struct[i].head != DEAD_NODE && network_struct[i].round < (j - (int)(1/CLUSTER_PERCENT))) {  
        if (type == ADV_SIMULATION || type == TAM_SIMULATION) {
          threshold += (network_struct[i].bCurrent * EXPECTED_CH)/( network_struct[i].bPower * NUM_NODES);
        }
        random_number = .00001*(rand() % 100000);          
        if(random_number <= threshold){  
          // the random number selected is less   
          // than the threshold so the node becomes   
          // a cluster head for the round  
          network_struct[i].head_count++;  
          // update the round variable   
          // so we know that this sensor was   
          // last a cluster head at round j 
          network_struct[i].round = j;  
          network_struct[i].head = CLUSTER_HEAD;  
          // store the index of the node in the   
          // cluster_heads array  
          // increment the cluster_head_count  
          cluster_head_count++; 
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
      if(network_struct[i].head == CLUSTER_HEAD){  
        network_struct[i].bCurrent -=   
        computeEnergyTransmit(LEACH_AD_DISTANCE,  
          LEACH_AD_MESSAGE, (type==MOD_SIMULATION || type==TAM_SIMULATION));  
        msg_count++;
        ovh_count++;
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
      if((network_struct[i].head != -1) && network_struct[i].head != DEAD_NODE){  
        // if the node's round is not equal to the    
        // current round, the node is not a cluster  
        // head and we must find a cluster head for  
        // the node to transmit to  
        for(k = 0; k < NUM_NODES; k++){  
          if(network_struct[k].head == CLUSTER_HEAD && closest != -1){  
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

        if ((type == MOD_SIMULATION || type == TAM_SIMULATION) && (network_struct[i].head == closest)) {
          network_struct[closest].cluster_members++;  
        }
        else {
          network_struct[i].head = closest;  
          network_struct[closest].cluster_members++;  

          distance_X_new = network_struct[i].xLoc - network_struct[closest].xLoc;  
          distance_Y_new = network_struct[i].yLoc - network_struct[closest].yLoc;  
          distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2)));  

          network_struct[i].bCurrent -= computeEnergyTransmit(distance_new, MESSAGE_LENGTH, (type==MOD_SIMULATION || type==TAM_SIMULATION));  
          network_struct[closest].bCurrent -= computeEnergyReceive(MESSAGE_LENGTH);

          msg_count++;
          ovh_count++;
        }

      }  
    }  

    // SCHEDULE CREATION  
        // the cluster head then creates a schedule telling the nodes   
        // when they can transmit; the schedule is broadcast to the nodes  
        // but no simulation of this is neccescary outside of the loss   
        // of battery power for broadcasting the schedule, which is a constant  
    for(i = 0; i <= NUM_NODES; i++){  
      if(network_struct[i].head == CLUSTER_HEAD){  
                // if the node is going to be a cluster head, it transmits   
                // the schedule to the other nodes  
        network_struct[i].bCurrent -=   
        computeEnergyTransmit(SCHEDULE_DISTANCE, SCHEDULE_MESSAGE, (type==MOD_SIMULATION || type==TAM_SIMULATION));  

        msg_count++;
        ovh_count++;
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
        else{
          if(network_struct[i].temperature != -1 and abs(network_struct[i].temperature - r) < TEEN_SOFT_THRESHOLD){
            does_transmit = 0;
          }
        }
        network_struct[i].temperature = r;
      }


      if(network_struct[i].head != CLUSTER_HEAD && network_struct[i].head != DEAD_NODE && does_transmit){  
        distance_X_new = network_struct[i].xLoc - network_struct[network_struct[i].head].xLoc;  
        distance_Y_new = network_struct[i].yLoc - network_struct[network_struct[i].head].yLoc;  
        distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2)));  
        network_struct[i].bCurrent -= computeEnergyTransmit(distance_new, MESSAGE_LENGTH, (type==MOD_SIMULATION || type==TAM_SIMULATION));  
        network_struct[network_struct[i].head].bCurrent -= computeEnergyReceive(MESSAGE_LENGTH);

        msg_count++; 
      }
      else if(network_struct[i].head == CLUSTER_HEAD) {  
        distance_X_new = network_struct[i].xLoc - BASE_STATION.xLoc;  
        distance_Y_new = network_struct[i].yLoc - BASE_STATION.yLoc;  
        distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2))); 

        for (int i = network_struct[i].cluster_members; i >= 0; --i) {
          double req_energy = computeEnergyTransmit(distance_new, MESSAGE_LENGTH*i, FALSE);
          if (network_struct[i].bCurrent - req_energy >= 0) {
            network_struct[i].bCurrent -= req_energy;
            los_count += network_struct[i].cluster_members - i;
            if (i > 0) {
              msg_count++;
            }
            break;
          }
        }
      } 
    }  

    // round has completed, increment the round count  
    for(i = 0; i <= NUM_NODES; i++){  
      network_struct[i].cluster_members = 0;  
      if(network_struct[i].bCurrent <= 0.0) network_struct[i].head = DEAD_NODE;  
    }  

    cluster_head_count = 0;  

    avgEnergy = averageEnergy(network_struct);
    if (round%LOG_STEPS == 0){
      int aliveC = aliveCount(network_struct);
      double mxEnergy = maxEnergy(network_struct);
      double mnEnergy = minEnergy(network_struct);
      // logFile << "round,alive,sent,overhead,avg energy,max energy,min energy\n";
      logFile << std::to_string(round)+",";
      logFile << std::to_string(aliveC)+",";
      logFile << std::to_string(msg_count)+",";
      logFile << std::to_string(ovh_count)+",";
      logFile << std::to_string(avgEnergy)+",";
      logFile << std::to_string(mxEnergy)+",";
      logFile << std::to_string(mnEnergy)+"\n";
    }

    round += 1;  
    j = round;
  } 

  // free(network_struct);

  logFile.close();
  
  return round;  
} 


double computeEnergyTransmit(float distance, int messageLength, int reduced){  
// Preconditions:   distance contains the distance between the transmitting   
//                  node and the receiving node. messageLength contains   
//                  the length of the message in bits.  
// Postconditions:  the total energy consumed by the transmission is   
//                  returned  

  float E_elec = 50 * pow(10,-9);  
  if (reduced) {
    E_elec*=0.1;
  }
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


void initializeNetwork(Sensor* network, int networkId) {  
// Preconditions:   network is an unitialized sensor network.  
// Postconditions:  network is an initialized sensor network   
//                  whose values are loaded from global varibles   
//          which are set using the loadConfiguration   
//          function or the defaults are used  

  // std::string filename = "Network_";
  // filename += std::to_string(networkId);
  // filename += ".xml";
  // ns3::AnimationInterface anim (filename); 

  cout << "initializing: " << networkId << endl; 

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

int aliveCount(Sensor network[]) { 
  int count = -1;

  for(int i = 0; i <= NUM_NODES; i++) {  
    if(network[i].head != DEAD_NODE){
      count++;
    }
  }  

  return count;
}

float averageEnergy(Sensor network[]) {  
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
    if (network[i].head != DEAD_NODE){
      current_power += network[i].bCurrent;  
    }
    starting_power += network[i].bPower;  
  }  

  return current_power/starting_power;  
}           // end averageEnergy function 

float maxEnergy(Sensor network[]) { 
  float max = 0.0;   

  for(int i = 0; i <= NUM_NODES; i++) {  
    double pwr = network[i].bCurrent/network[i].bPower;
    if (pwr > max && pwr <= 1){
      max = pwr;
    }
  }  

  return (int)(max * 1000.0)/1000.0;  
}           // end maxEnergy function 

float minEnergy(Sensor network[]) { 
  float min = B_POWER_ADVANCED+1;   

  for(int i = 0; i <= NUM_NODES; i++) {  
    if (network[i].head == DEAD_NODE) {
      return 0.0;
    }
    double pwr = network[i].bCurrent/network[i].bPower;
    if (pwr < min && pwr >= 0){
      min = pwr;
    }
  }  

  return (int)(min * 1000.0)/1000.0;  
}           // end minEnergy function  
