// Header file for PLAYERS.C (handles player objects)


struct ship_define {        // Defines that make ships unique
        int accell;         // Acceleration rate
        int topspeed;       // Top speed
        int decell;         // Decceleration rate
};

struct player_objects {     // Player struct (for each ship in the Arena)
        int objectnum;      // Object number (in object pointer)
        int player_type;    // Type of player (1=LOCAL,2=REMOTE,3=COMPUTER)
        int ship_type;      // Type of ship structure to use
        int velocity;       // Current speed of ship
};

