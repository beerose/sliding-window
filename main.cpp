#include "utils.h"

int BYTES_IN_SEGMENT = 1000;
int WINDOW_SIZE = 1000;
int TV_USEC = 200;

int main(int argc, char *argv[]) {
  int size, segments, bytes_left, port, last_segment_size, i;
  const char *filename;
  ProgramParams params;
  bool filled;

  if (!validate_args(argc, argv)) {
    std::cout << "Invalid input. \n";
    exit(0);
  }

  port = std::stoi(argv[1]);
  filename = argv[2];
  size = std::stoi(argv[3]);

  std::fstream file(filename, std::ios::out | std::ios::binary);

  params = initialise_socket(port);

  bytes_left = size;
  segments = ceil((float)size / (float)BYTES_IN_SEGMENT);
  last_segment_size = (size % BYTES_IN_SEGMENT) == 0
                          ? BYTES_IN_SEGMENT
                          : (size % BYTES_IN_SEGMENT);

  std::vector<std::string> data(WINDOW_SIZE + 1, "");
  std::vector<int> acknowledged(segments + 1, -1);
  int ack = 0;

  while (bytes_left > 0) {
    for (i = ack; i < ack + WINDOW_SIZE; i++) {
      if (i < segments && acknowledged[i] == -1) {
        int start = i * BYTES_IN_SEGMENT;
        int amount =
            ((i == segments - 1) ? last_segment_size : BYTES_IN_SEGMENT);
        send_message(&params, start, amount);
      }
    }

    int ready = select(params.sockfd, TV_USEC);
    if (ready < 0) {
      std::cerr << "Read from socket error. \n";
      return 1;
    }

    if (ready > 0) {
      auto received_data = receive_message(&params);
      int index = received_data.start / BYTES_IN_SEGMENT;

      if (index >= ack && index < (ack + WINDOW_SIZE) &&
          acknowledged[index] == -1) {
        acknowledged[index] = 1;
        data[index % WINDOW_SIZE] = received_data.data;
        bytes_left -=
            ((index == segments - 1) ? last_segment_size : BYTES_IN_SEGMENT);
        print_progress(size, bytes_left);
      }
    }

    filled = true;
    for (int j = ack; j < ack + WINDOW_SIZE; j++) {
      if (j == segments) {
        break;
      }
      if (acknowledged[j] == -1) {
        filled = false;
        break;
      }
    }

    if (filled) {
      int how_many =
          (segments - ack) < WINDOW_SIZE ? (segments - ack) : WINDOW_SIZE;
      ack += how_many;
      for (int d = 0; d < how_many; d++) { // last segment case
        file << data[d];
      }
    }
  }
  file.close();
  return 0;
}
