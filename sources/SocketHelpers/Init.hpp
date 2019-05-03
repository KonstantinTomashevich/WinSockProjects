#pragma once
namespace Init
{
void LoadWindowsSocketLibrary ();
void UnloadWindowsSocketLibrary ();

namespace Exceptions
{
class UnableToLoadWSA;
}
}
