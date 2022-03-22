#include <cstdlib>
#include <iostream>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "Flight.pb.h"
#include "Flight.grpc.pb.h"

namespace pb = arrow::flight::protocol;

class GrpcServiceHandler final : public pb::FlightService::Service {};

void Main() {
  std::cout << "Starting server" << std::endl;
  std::unique_ptr<GrpcServiceHandler> service(new GrpcServiceHandler());
  grpc::ServerBuilder builder;
  int port = 0;
  builder.AddListeningPort("localhost:0", grpc::InsecureServerCredentials(), &port);
  builder.RegisterService(service.get());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  std::cout << "Creating client" << std::endl;
  std::string grpc_uri = "localhost:" + std::to_string(port);
  auto creds = ::grpc::InsecureChannelCredentials();
  auto stub = pb::FlightService::NewStub(::grpc::CreateChannel(grpc_uri.c_str(), creds));

  std::cout << "Making RPC call" << std::endl;
  {
    grpc::ClientContext context;
    pb::FlightDescriptor descr;
    pb::FlightInfo reply;
    grpc::Status status = stub->GetFlightInfo(&context, descr, &reply);
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
  }
}

int main(int, char**) {
  Main();
  std::cout << "Done!" << std::endl;
  return EXIT_SUCCESS;
}
