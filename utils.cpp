#include "utils.h"

int validate_args(int argc, char *argv[]) {
  int port = std::stoi(argv[1]);
  return (argc == 4 && argv != nullptr && (port >= 0 && port <= 65535));
}

ProgramParams initialise_socket(int port) {
  struct sockaddr_in server_address;
  ProgramParams params = {-1};
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0) {
    std::cerr << "Socket creation exception.\n";
  }

  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  std::cout << "port: " << server_address.sin_port << " mine" << port;
  inet_pton(AF_INET, SERVER_IP_ADDRESS, &server_address.sin_addr);

  int isConnected = connect(sockfd, (struct sockaddr *)&server_address,
                            sizeof(server_address));

  if (isConnected < 0) {
    std::cerr << "Sever connecting exception.\n";
  }

  params.sockfd = sockfd;
  params.addr = server_address;
  return params;
}

std::string get_message_to_send(int start, int end) {
  return "GET " + std::to_string(start) + " " + std::to_string(end) + "\n";
}

void send_message(ProgramParams info, std::string message) {
  int numbytes;
  if ((numbytes =
           sendto(info.sockfd, message.c_str(), strlen(message.c_str()), 0,
                  (struct sockaddr *)&info.addr, sizeof(info.addr))) == -1) {
    perror("sendto");
    return;
  }

  std::cout << "sent " << message << "to: " << SERVER_IP_ADDRESS << "\n\n";
  return;
}

ReceivedData receive_message(ProgramParams params) {
  ReceivedData received;
  int numbytes;
  char buf[MAXBUFLEN];

  fd_set descriptors;
  FD_ZERO(&descriptors);
  FD_SET(params.sockfd, &descriptors);
  struct timeval tv = {1, 0};
  int ready = select(params.sockfd + 1, &descriptors, NULL, NULL, &tv);

  if (ready < 0) {
    std::cerr << "Read from socket error.";
  } else if (ready == 0) {
    std::cout << "End of time.";
  } else {
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(params.sockfd, buf, MAXBUFLEN - 1, MSG_DONTWAIT,
                             (struct sockaddr *)&their_addr, &addr_len)) ==
        -1) {
      perror("recvfrom");
      return received;
    }

    std::cout << "packet contains: " << buf << "\n";
    received = extract_data(buf);
  }

  return received;
}

ReceivedData extract_data(char *buf) {
  ReceivedData data = {"", -1, -1};
  std::string str(buf, 700);

  size_t found = str.find("\n");
  std::string magic_info = str.substr(0, found);

  std::vector<std::string> info = split(magic_info, ' ');
  if (info.size() < 3) {
    std::cerr << "Invalid data received. \n";
    return data;
  }
  data.start = stoi(info[1]);
  data.amount = stoi(info[2]);

  data.data = str.substr(found);

  return data;
}

std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str);
  std::string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

void save(std::vector<std::string> data, const char *filename) {
  std::fstream file(filename, std::ios::out | std::ios::app | std::ios::binary);

  for (int i = 0; i < data.size(); i++) {
    file << data[i];
  }

  file.close();
}
