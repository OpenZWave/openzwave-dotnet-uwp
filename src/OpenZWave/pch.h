#pragma once

// Platform includes
#include "Windows.h"
#include "stdio.h"

#if __cplusplus_cli
// .NET CLR/CLI includes
#include <msclr/auto_gcroot.h>
#include <msclr/lock.h>
#include <msclr/marshal_cppstd.h>
#endif

// OpenZWave includes
#include "Manager.h"
#include "Value.h"
#include "ValueStore.h"
#include "ValueID.h"
#include "ValueBool.h"
#include "ValueInt.h"
#include "ValueByte.h"
#include "ValueString.h"
#include "ValueShort.h"
#include "ValueDecimal.h"
#include "Notification.h"
#include "Options.h"
#include "ValueID.h"
#include "Driver.h"
#include "Log.h"

//UWP

#include <locale>
#include <codecvt>
#include <string>

#include "Platform.h"