#include "utils.h"

int BYTES_IN_SEGMENT = 700;

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

  std::fstream file(filename, std::ios::out | std::ios::app | std::ios::binary);

  params = initialise_socket(port);

  bytes_left = size;
  segments = ceil((float)size / (float)BYTES_IN_SEGMENT);

  std::vector<std::string> data(segments, "");
  for (int i = 0; i < segments; i++) {

    int start = i * BYTES_IN_SEGMENT;
    int amount =
        ((i + 1 == segments) ? bytes_left : BYTES_IN_SEGMENT); // it works
    std::string message = get_message_to_send(start, amount);

    while (data[i] == "") {
      std::cout << "Sending: " << message;
      send_message(params, message);

      auto received_data = receive_message(params);
      if (received_data.start == start && received_data.amount == amount) {
        data[i] = received_data.data;
        file << received_data.data;
        bytes_left -= BYTES_IN_SEGMENT;
      }
    }
  }
  std::cout << "Done! \n";
  // save(data, filename);
  file.close();
  return 1;
}
