#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>

using namespace std;

/*
Eric Engman
Lab 4
CS 470
40855107
*/
int numbers;
int n = 5;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
fstream file;
string fileInput[6];
string line;
int num;
int port = 5437;
string ip = "127.0.0.1";
int timeout = 4;
string line2;

// Function to find the prime factors of number arg
string pFactor(long int number, int prime){

  string result = " ";
  // Find number of 2's that divide number and concatenate
  while(number % prime == 0){
    result += to_string(prime);
    result += " ";
    number = number/prime;

  }
  return result;
}

string baseConversion(long number, int base){
  cout << "In base conversion" << endl;
  cout << "Number recieved " << number << " Base: " << base << endl;
  string bases = "012356789";
  string conversionResult = " ";

  while(number > 0){
    conversionResult = bases[number % base] + conversionResult;
    number/=base;
  }
  return conversionResult;
}


void * clientThread(void* arg){

  pthread_mutex_lock(&lock);
  int sockfd = 0;
  char recvBuff[1025];
  struct sockaddr_in serv_addr;

  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      printf("\n Error : Could not create socket \n");
      pthread_mutex_unlock(&lock);
      pthread_exit(NULL);
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)
  {
      printf("\n inet_pton error occured\n");
      pthread_mutex_unlock(&lock);
      pthread_exit(NULL);
  }
  char buf[1025];
  string result;

  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
     printf("\n Error : Connect Failed \n");
     close(sockfd);
     pthread_mutex_unlock(&lock);
     pthread_exit(NULL);
  }

  //  Wait for repsonse
  memset(buf, 0, sizeof(buf));
  long bytesReceived = recv(sockfd, buf, 1025, 0);
  string s(buf);
  memset(buf, 0, sizeof(buf));
  bytesReceived = recv(sockfd, buf, 1025, 0);
  string p(buf);

  cout << "Client recieved prime factor: " << p << endl;
  string::size_type sz;

  long bytes = stol(s, &sz);

  long prime = stol(p);

  result = pFactor(bytes, prime);
  cout << "Sending back prime factor(s): " << result << endl;


  if(bytesReceived < LONG_MAX){

    int sendRes = send(sockfd, result.c_str(), result.size() + 1, 0);

    // Check if it failed
    if(sendRes == -1){
      cout << "Could not send to server! Whoops!\r\n";
      pthread_mutex_unlock(&lock);
      pthread_exit(NULL);

    }
  }

  cout << "Client finished" << endl;
  cout << endl;
  pthread_mutex_unlock(&lock);
  pthread_exit(NULL);
}



int main(int argc, char *argv[])
{

  if(argc == 2){

    file.open(argv[1], ios::in);
    if(file.is_open()){
      num = 0;
      while(getline(file, line)){
        int pos = line.find("=");
        line2 = line.substr(pos + 2);
        fileInput[num++] = line2;
      }
    }
    file.close();

    ip = fileInput[1];
    port = stoi(fileInput[2]);
    timeout = stoi(fileInput[3]);
    n = stoi(fileInput[5]);
  }


    cout << "Client terminal" << endl;
    cout << endl;
    /*
      Multi threading approach
    */

    pthread_t tid[n];

    for(int i = 0; i < n; i++){


      if(pthread_create(&tid[i], NULL, clientThread, NULL) != 0)
        cout << "Failed to create thread" << endl;

    }

    for(int i = 0; i < n; i++){
      pthread_join(tid[i], NULL);
    }

  return 0;
}
