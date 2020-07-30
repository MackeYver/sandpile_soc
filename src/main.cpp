#include <stdio.h>
#include <stdlib.h>

#include "sandpile.h"
#include "bitmap.cpp"


int main(int argc, char *argv[]) {
    std::vector<Sandpile::Grain_Drop> grain_drops = {
        {{200, 200}, 1},
    };

    Sandpile::Run_Session session {};
    session.number_of_iterations = 200000;
    session.grain_drops = grain_drops;

    Sandpile::Config config {};
    config.width  = 401;
    config.height = 401;

    Sandpile sandpile {};
    sandpile.setup(config);
    bool result = true;
    uint32_t session_index = 0;
    uint32_t number_of_loops = 100;


    #if 0
    result = sandpile.run(session);
    if (result) {
        char buffer[512];
        _snprintf_s(buffer, 512, _TRUNCATE, "..\\run\\output\\output.bmp");
        result = write_bmp_to_disc(config.width, config.height, sandpile.get_data(), buffer);
    }
    #endif


    #if 1
    if (result) {
        session_index = 0;
        session.number_of_iterations = 14000;
        number_of_loops = 100;
        char buffer[512];
        for (; session_index < number_of_loops; ++session_index) {
            result = sandpile.run(session);
            if (result) {
                _snprintf_s(buffer, 512, _TRUNCATE, "..\\run\\output\\%u.bmp", session_index);
                result = write_bmp_to_disc(config.width, config.height, sandpile.get_data(), buffer);
            }
        }
    }
    #endif


    #if 0
    if (result) {
        session.number_of_iterations = 100;
        number_of_loops = session_index + 200;
        char buffer[512];
        for (; session_index < number_of_loops; ++session_index) {
            result = sandpile.run(session);
            if (result) {
                _snprintf_s(buffer, 512, _TRUNCATE, "..\\run\\output\\%u.bmp", session_index);
                result = write_bmp_to_disc(config.width, config.height, sandpile.get_data(), buffer);
            }
        }
    }
    #endif

    return result;
}