#include "NotifyEvent.h"

int CreatEventFd()
{
	int fd = eventfd(0, EFD_NONBLOCK);
	if (fd < 0)
	{
		LOG_ERROR << "create eventfd failed";
		exit(0);
	}
	return fd;
}