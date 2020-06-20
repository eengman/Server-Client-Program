#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <limits.h>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

/*
Eric Engman
Lab 4
CS 470
40855107
*/

int numClients = 0;
long sendNum;
string result = " ";
int numFactors = 0;
string numString = " ";
int iter = 2;
string finalAnswer = " ";
string * allFactP;
int count = 0;
string * allF;
long number;
string correctBase = " ";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// function calculates number of prime factors
void numFact(long int number2){

  // Find number of 2's that divide number and concatenate
  while(number2 % 2 == 0){
    numFactors++;
    number2 = number2/2;
  }

  // find remaining
  for(long int i = 3; i <= sqrt(number2); i = i + 2){

    // while i divides number, concatenate to result string
    while(number2 % i == 0){
      number2 = number2/i;
      numFactors++;
    }
  }
  if(number2 > 2){
    numFactors++;
  }

  return;
}

string baseConversion(long int number, int base){

  string bases = "012356789";
  string conversionResult = " ";

  while( number > 0){
    conversionResult = bases[number % base] + conversionResult;
    number/=base;
  }
  return conversionResult;
}

// Function to find the prime factors of number arg
string allFactors(){

  string res = " ";
  // Find number of 2's that divide number and concatenate
  if(number % 2 == 0){
    allFactP[count++] = to_string(2);
    number = number/2;
  }

  // find remaining
  for(long int i = 3; i <= sqrt(number); i = i + 2){

    // while i divides number, concatenate to result string
    if(number % i == 0){
      allFactP[count++] = to_string(i);
      number = number/i;
    }
  }

  while(number > 2 && number % 2 == 0){
    number = number / 2;
  }

  if(number > 2 ){

      allFactP[count++] = to_string(number);

  }

  return res;
}

void * serverThread(void*arg){
  //cout << "Connection found" << endl;
  char sendBuff[1025];

  int newSock = *((int *) arg);

  pthread_mutex_lock(&lock);

  // Clear the buffer
  memset(sendBuff, 0, 1025);
  numString = to_string(sendNum);
  string numStringFactor = numString;
  long numCopy = sendNum;

  numStringFactor += " ";

  string p = allFactP[count];
  count++;

  send(newSock, numStringFactor.c_str(), numStringFactor.size() + 1, 0);
  sleep(1);
  send(newSock, p.c_str(), p.size() + 1, 0);

    // Wait for a Message
  long bytesRecv = recv(newSock, sendBuff, 1025, 0);
  if(bytesRecv == -1){
    cerr << "There was a connection issue" << endl;
    numClients--;
  }

  if(bytesRecv == 0){
    cout << "The client disconnected" << endl;

  }
    // Display message
  finalAnswer += string(sendBuff, bytesRecv);
  finalAnswer += " ";

  numFactors--;
  if(numFactors == 0){
    close(newSock);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
  }

pthread_mutex_unlock(&lock);
  close(newSock);
  pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    cout << "Server terminal: " << endl;
    int listenfd = 0, connfd = 0, newSockfd;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5437);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    srand (time(NULL));
    sendNum = rand() % LONG_MAX;
    cout << endl;
    int base = 2 + (rand() % 10);

    char b = (char)base;
    string bs = to_string(base);

    string s = to_string(sendNum);

    correctBase = baseConversion(sendNum, base);

    correctBase += "(";
    correctBase += bs;
    correctBase += ")";

    cout << "Number generated by server to send to client: " << endl;
    cout << correctBase << endl;
    cout << endl;

    number = sendNum;
    numFact(number);

    string allFact[numFactors];
    allFactP = allFact;

    allFactors();
    count = 0;

    ofstream myfile;
    myfile.open("input.ini");
    if(myfile.is_open()){
      myfile << "[Connection]\n";
      myfile << "IP = 127.0.0.1\n";
      myfile << "Port = 5437\n";
      myfile << "Timeout = 10\n";
      myfile << "[Other]\n";
      myfile << "n = " << numFactors;
      myfile.close();
    }else{
      cout << "Unable to open file" << endl;
    }

    while(true){

        if(numFactors == 0){
          break;
        }

        int client, addr_size = sizeof(serv_addr);
        pthread_t child;
        client = accept(listenfd, (struct sockaddr*)NULL, NULL);
        //cout << "Connection found" << endl;
        numClients++;

        if(pthread_create(&child, NULL, serverThread, &client) != 0)
          cout << "Thread created" << endl;
        //else
        //int pthread_detach(child);
        pthread_join(child, NULL);

    }
    cout << "Prime factorization of: " << correctBase << ": " << finalAnswer << endl;

     return 0;
}