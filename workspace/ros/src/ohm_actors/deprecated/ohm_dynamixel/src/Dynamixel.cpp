#include "Dynamixel.h"

std::string Dynamixel::modelName(const Model model)
{
    switch (model)
    {
    case MX106:
        return "Mx106";

    case MX28:
        return "Mx28";
    case AX12:
        return "Ax12";

    default:
        return "Not known.";
    }
}
