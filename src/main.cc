#include <cstdlib>
#include <iostream>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "Flight.pb.h"
#include "Flight.grpc.pb.h"

namespace pb = arrow::flight::protocol;

class GrpcServiceHandler final : public pb::FlightService::Service {};

void Main() {
  std::unique_ptr<GrpcServiceHandler> service(new GrpcServiceHandler());
  grpc::ServerBuilder builder;
  int port = 0;
  builder.AddListeningPort("localhost:0", grpc::InsecureServerCredentials(), &port);
  builder.RegisterService(service.get());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  // Set up the client
  std::string grpc_uri = "localhost:" + std::to_string(port);
  auto creds = ::grpc::InsecureChannelCredentials();
  auto stub = pb::FlightService::NewStub(::grpc::CreateChannel(grpc_uri.c_str(), creds));

  // Make an RPC call
  {
    grpc::ClientContext context;
    pb::FlightDescriptor descr;
    pb::FlightInfo reply;
    grpc::Status status = stub->GetFlightInfo(&context, descr, &reply);
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
  }

  // Explicitly clean up
  stub.reset();
  server->Shutdown();
  server.reset();
  service.reset();
}

int main(int, char**) {
  Main();
  return EXIT_SUCCESS;
}
