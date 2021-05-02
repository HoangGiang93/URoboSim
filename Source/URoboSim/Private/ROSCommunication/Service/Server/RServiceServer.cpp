#include "ROSCommunication/Service/Server/RServiceServer.h"

void URServiceServer::SetServiceServerParameters(URServiceServerParameter *&ServiceServerParameters)
{
	if (ServiceServerParameters)
  {
    Name = ServiceServerParameters->Name;
    Type = ServiceServerParameters->Type;
  }
}

void URServiceServer::Init()
{
	CreateServiceServer();
	if (Handler.IsValid())
	{
		Handler->AddServiceServer(ServiceServer);
	}
}