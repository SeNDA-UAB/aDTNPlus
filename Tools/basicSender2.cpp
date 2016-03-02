#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <cstring>
#include <ctime>
#include "adtnSocket.h"
#include "Bundle/Bundle.h"
#include "Bundle/ForwardingMEB.h"
#include "Bundle/RouteReportingMEB.h"

static void help(std::string program_name) {
  std::cout << program_name << " is part of the SeNDA aDTNPlus platform\n"
            << "Usage: " << program_name
            << " -i '127.0.0.1' -p '50000' -s 'node1'"
            << " -d 'node2:50' -m 'This is a message for node2.'\n"
            << "Required options:\n"
            << "   [-i | --listeningIP] Ip\t\t\tIP of the listening node.\n"
            << "   [-p | --port] port\t\t\tPort where the node is listening.\n"
            << "   [-d | --destination] destination node\t\t\t"
            << "Which node is the destination.\n"
            << "   [-m | --message] message to send\t\t\tThe message to send.\n"
            << "Supported options:\n"
            << "   [-h | --help]\t\t\t\tShows this help message.\n"
            << std::endl;
}

int main(int argc, char **argv) {
  int opt = -1, option_index = 0;
  std::string ip = "";
  std::string source = "";
  int port = -1;
  std::string destination = "";
  std::string message = "";

  static struct option long_options[] = {
      { "listeningIP", required_argument, 0, 'i' },
      { "port", required_argument, 0, 'p' },
      { "source", required_argument, 0, 's'},
      { "destination", required_argument, 0, 'd' },
      { "message", required_argument, 0, 'm' },
      {"help", no_argument, 0, 'h' },
      { 0, 0, 0, 0 }
  };

  while ((opt = getopt_long(argc, argv, "i:p:s:d:m:h", long_options,
                              &option_index))) {
      switch (opt) {
        case 'i':
          ip = std::string(optarg);
          break;
        case 'p':
          port = std::atoi(optarg);
          break;
        case 's':
          source = std::string(optarg);
          break;
        case 'd':
          destination = std::string(optarg);
          break;
        case 'm':
          message = std::string(optarg);
          break;
        case 'h':
          help(std::string(argv[0]));
          exit(0);
        default:
          break;
      }
      if (opt == -1)
        break;
    }

  if (ip == "" || port == -1 || source == "" || destination == ""
        || message == "") {
      help(std::string(argv[0]));
      exit(0);
    }
    try {
      Bundle b = Bundle(source, destination, message);
      /* std::shared_ptr<ForwardingMEB> fmeb = std::make_shared<ForwardingMEB>(
              ForwardingMEB("return std::vector<std::string>();"));
      b.addBlock(fmeb); */
      std::shared_ptr<RouteReportingMEB> rrm =
          std::make_shared<RouteReportingMEB>(RouteReportingMEB());
      /*time_t aTime = time(NULL);
      time_t dTime = aTime + 3600;
      std::shared_ptr<RouteReportingMEB> rrm =
          std::make_shared<RouteReportingMEB>(
              RouteReportingMEB("node0", aTime, dTime));*/
      b.addBlock(rrm);
      sockaddr_in remoteAddr = { 0 };
      remoteAddr.sin_family = AF_INET;
      remoteAddr.sin_port = htons(port);
      remoteAddr.sin_addr.s_addr = inet_addr(ip.c_str());
      int sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock == -1) {
        std::cout << "Cannot create socket, reason: " << strerror(errno)
                  << std::endl;
      } else {
        if (connect(sock, reinterpret_cast<sockaddr*>(&remoteAddr),
                    sizeof(remoteAddr)) < 0) {
          std::cout << "Cannot connect with node, reason: " << strerror(errno)
                    << std::endl;
        } else {
          send(sock, source.c_str(), 1024, 0);
          std::string bundleRaw = b.toRaw();
          uint32_t bundleLength = bundleRaw.length();
          uint32_t nBundleLength = htonl(bundleLength);
          send(sock, &nBundleLength, sizeof(nBundleLength), 0);
          send(sock, bundleRaw.c_str(), bundleLength, 0);
        }
        close(sock);
      }
    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
    }
    return 0;
}
