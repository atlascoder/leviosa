#include "ClientConfig.h"

ClientConfig & ClientConfig::instance()
{
    static ClientConfig singleton;
    return singleton;
}
