#include "ROSCommunication/Action/Server/RActionCancelSubscriber.h"
#include "actionlib_msgs/GoalID.h"

DEFINE_LOG_CATEGORY_STATIC(LogRActionCancelSubscriber, Log, All)

URActionCancelSubscriber::URActionCancelSubscriber()
{
  MessageType = TEXT("actionlib_msgs/GoalID");
}

void URActionCancelSubscriber::Init()
{
  if (GetOwner())
  {
    ControllerComponent = Cast<URControllerComponent>(GetOwner()->GetPlugin(TEXT("ControllerComponent")));
  }
}