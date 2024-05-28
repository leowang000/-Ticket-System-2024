#ifndef TICKET_SYSTEM_CONFIG_H
#define TICKET_SYSTEM_CONFIG_H

#include "../utility/STLite/String.h"

namespace bubble {

using username_t = String<20>;
using password_t = String<30>;
using name_t = String<20>;
using mail_t = String<30>;
using train_id_t = String<20>;
using station_name_t = String<40>;
using HashType = String<0>::HashType;

}

#endif //TICKET_SYSTEM_CONFIG_H