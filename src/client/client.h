// alsa-control-server
// shane tully (shane@shanetully.com)
// shanetully.com
// https://github.com/shanet/Alsa-Channel-Control

#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "Client.h"
#include "Constants.h"


#define NAME    "Alsa Control Client"

#define DEFAULT_CLIENT "127.0.0.1"
#define DEFAULT_PORT   4242

using namespace std;


const char *prog;    // Name of the program
int    verbose;      // Level of verbosity to use
int    useEnc;       // Encrypt communications with the server
Client client;       // The client object that communicates with the server

int serverHandshake();

int sendVolCmd(vector<string> channels, vector<string> vols);

int sendMediaCmd(int commandType);

void printUsage();

void printVersion();
