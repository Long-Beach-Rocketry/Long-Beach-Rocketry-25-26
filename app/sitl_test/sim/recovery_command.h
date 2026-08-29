#pragma once

// Represents the state of the recovery system (not deployed or deployed)
struct RecoveryState
{
    bool deployment = false;
    // false = not deployed
    // true = deployed
};