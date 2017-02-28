#pragma once


#if !__cplusplus_cli
#define WINRT
#define gcnew ref new

// #define PLATFORM_STRING Platform::String

// #define EXPORT_BYTE_ARRAY_OUT Platform::Array<byte>^ *
// #define EXPORT_STRING_ARRAY_OUT Platform::Array<String^>^ *

#endif


#if __cplusplus_cli

// #define PLATFORM_STRING System::String
// #define EXPORT_BOOL [OUT] System.Boolean
// #define EXPORT_BYTE_ARRAY_OUT [Out] cli::array<Byte>^ %
// #define EXPORT_STRING_ARRAY_OUT [Out] cli::array<String^>^ %

#endif