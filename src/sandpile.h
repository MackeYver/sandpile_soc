#ifndef Sandpile__h
#define Sandpile__h

#include <stdint.h>
#include <stdio.h>
#include <vector>


class Sandpile {
    public:
        //
        // Structs
        struct Cell_Location {
            uint32_t x;
            uint32_t y;
        };

        struct Config {
            uint32_t width = 11;
            uint32_t height = 11;
            uint32_t max_cell_height = 3;
        };

        struct Grain_Drop {
            Cell_Location location;
            uint32_t count;
        };

        struct Run_Session {
            uint32_t number_of_iterations = 100;
            std::vector<Grain_Drop> grain_drops;
        };

        struct Avalanche {
            uint32_t turn = 0;
            uint32_t size = 0;
            uint32_t time = 0;
        };


        //
        // Public methods and members
        ~Sandpile();

        void clear();
        void print_cells();
        void print_statistics();
        uint8_t *get_data();

        bool setup(Config const& config);
        bool run(Run_Session const& session);


    private:
        void drop_grain(Grain_Drop grain, std::vector<Grain_Drop>& overspill);
        uint32_t inc_cell_height_at(Cell_Location location, uint32_t increase = 1);
        void set_cell_height_at(Cell_Location location, uint8_t height);

        std::vector<Avalanche> _avalanches;
        uint8_t *_data = nullptr;
        uint32_t _data_count = 0;
        uint32_t _width = 0;
        uint32_t _height = 0;

        uint32_t _max_cell_height = 0;
};


#endif