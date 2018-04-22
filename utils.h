#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <queue>
#include <regex>
#include <sstream>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <tgmath.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#define SERVER_IP_ADDRESS "156.17.4.30"
#define MAXBUFLEN 1000000
#define WHITESPACE_CHAR 32
#define NEWLINE_CHAR 13

typedef struct {
  int sockfd;
  sockaddr_in addr;
  socklen_t len;
} ProgramParams;

typedef struct {
  std::string data;
  int start;
  int amount;
} ReceivedData;

int validate_args(int argc, char *argv[]);
ProgramParams initialise_socket(int port);
std::string get_message_to_send(int start, int end);
void send_message(ProgramParams *params, std::string message);
ReceivedData receive_message(ProgramParams *params);
ReceivedData extract_data(char *buf);
std::vector<std::string> split(std::string str, char delimiter);
void save(const std::vector<std::string> *data, const char *filename);
int move_sliding_window(std::vector<std::string> *window, int ack);
