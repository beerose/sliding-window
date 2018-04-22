#include "utils.h"

int BYTES_IN_SEGMENT = 700;
int WINDOW_SIZE = 4;

/*
selective repeat

Some pseudocode:
while (true <- all received or !timeout)
  for last_ack to last_act + WINDOW_SIZE {
    if !data[i] sent_message
  }

  if received > last_ack && received <= last_ack + WINODW_SIZE {
    data[received] = received.data
  }

  if received == last_ack+1 {
    last_ack ++
  }

*/

int main(int argc, char *argv[]) {
  int size, segments, bytes_left, port;
  const char *filename;
  ProgramParams params;

  if (!validate_args(argc, argv)) {
    std::cout << "Invalid input. \n";
    exit(0);
  }
  port = std::stoi(argv[1]);
  filename = argv[2];
  size = std::stoi(argv[3]);

  params = initialise_socket(port);

  bytes_left = size;
  segments = ceil((float)size / (float)BYTES_IN_SEGMENT);

  std::vector<std::string> data(segments, "");
  int last_ack = 0;

  while (bytes_left > 0) {
    for (int i = last_ack; i < last_ack + WINDOW_SIZE; i++) {
      int start = i * BYTES_IN_SEGMENT;
      int amount =
          ((i + 1 == segments) ? bytes_left : BYTES_IN_SEGMENT); // it works
      std::string message = get_message_to_send(start, amount);
      if (data[i] == "") {
        send_message(&params, message);
      }
    }

    for (int i = last_ack; i < last_ack + WINDOW_SIZE; i++) {
      auto received_data = receive_message(&params);
      int index = received_data.start / BYTES_IN_SEGMENT;
      if (index >= last_ack && data[index] == "") {
        data[index] = received_data.data;
        bytes_left -= BYTES_IN_SEGMENT;
      }
    }

    if (data[last_ack] != "") {
      last_ack++;
    }
  }
  std::cout << "Done! \n";
  save(&data, filename);

  return 1;
}
