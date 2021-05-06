#include "ROSCommunication/Service/Server/RServiceServer.h"

void URServiceServer::SetServiceServerParameters(URServiceServerParameter *&ServiceServerParameters)
{
	if (ServiceServerParameters)
  {
    CommonServiceServerParameters = ServiceServerParameters->CommonServiceServerParameters;
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