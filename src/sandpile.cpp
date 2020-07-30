#include "sandpile.h"
#include <cmath>
#include <algorithm>


//
// Constructors and destructor
//

Sandpile::~Sandpile() {
    delete[] _data;
}




//
// Utilities
//

void Sandpile::clear() {
    if (_data) {
        delete[] _data;
        _data = nullptr;
    }

    _width = 0;
    _height = 0;
    _data_count = 0;
    _max_cell_height = 0;
    _avalanches.clear();
}


bool Sandpile::setup(Config const& config) {
    bool result = false;

    if (config.width > 0 && config.height > 0) {
        clear();

        _width  = config.width;
        _height = config.height;
        _data_count = _width * _height;
        _max_cell_height = config.max_cell_height;

        _data = new uint8_t[_data_count];
        for (uint32_t index = 0; index < _data_count; ++index) {
            _data[index] = 0;
        }

        result = true;
    }

    return result;
}


void Sandpile::print_cells() {
    for (uint32_t y = 0; y < _height; ++y) {
        printf("  ");
        for (uint32_t x = 0; x < _width; ++x) {
            printf("%u ", _data[(_width * y) + x]);
        }
        printf("\n");
    }
    printf("\n");
}


void Sandpile::print_statistics() {
    //
    // Cell statistics
    {
        uint32_t sum = 0;
        uint32_t n = 0;
        for (uint32_t index = 0; index < _data_count; ++index) {
            if (_data[index] > 0) {
                ++n;
                sum += _data[index];
            }
        }

        float average_non_empty_cells = static_cast<float>(sum) / static_cast<float>(n);
        float average_all_cells       = static_cast<float>(sum) / static_cast<float>(_data_count);

        printf("Grains:\n");
        printf("  Total count of grains: %u\n", sum);
        printf("  Average height of non-empty cells: %g\n", average_non_empty_cells);
        printf("  Average height of all cells: %g\n", average_all_cells);
    }

    printf("\n");

    //
    // Avalanche statistics
    {
        uint32_t n = static_cast<uint32_t>(_avalanches.size());
        uint32_t size = 0;
        uint32_t time = 0;

        std::for_each(_avalanches.begin(), _avalanches.end(), [&](Avalanche const& avalanche) {
            size += avalanche.size;
            time += avalanche.time;
        });

        float average_avalanche_size = static_cast<float>(size) / static_cast<float>(n);
        float average_avalanche_time = static_cast<float>(time) / static_cast<float>(n);

        printf("Avalanches\n");
        printf("  Total count: %u\n", n);
        printf("  Average size: %g\n", average_avalanche_size);
        printf("  Average time: %g\n", average_avalanche_time);

        /*
        printf("Avalanche data (csv):\n");
        printf("Turn;Time;Size\n");
        for (auto it : _avalanches) {
            printf("%u;%u;%u\n", it.turn, it.time, it.size);
        }
        */
    }
}


uint32_t Sandpile::inc_cell_height_at(Cell_Location location, uint32_t increase) {
    uint32_t result = 0;

    if (location.x < _width && location.y < _height) {
        uint8_t *ptr = &_data[(_width * location.y) + location.x];
        uint8_t byte_increase = increase >= 0xFF ? 0xFF : static_cast<uint8_t>(increase);
        if (*ptr < (0xFFFF - byte_increase)) {
            *ptr += byte_increase;
            result = *ptr;
        }
    }

    return result;
}


void Sandpile::set_cell_height_at(Cell_Location location, uint8_t height) {
    if (location.x < _width && location.y < _height) {
        _data[(_width * location.y) + location.x] = height;
    }
}


void Sandpile::drop_grain(Grain_Drop grain, std::vector<Grain_Drop>& overspill) {
    if (grain.location.x < _width && grain.location.y < _height) {
        uint32_t cell_height = inc_cell_height_at(grain.location, grain.count);
        if (cell_height > _max_cell_height) {
            uint32_t qty_per_cell = cell_height / 4;
            uint32_t rest_amount  = cell_height - (4 * qty_per_cell);

            uint32_t x = grain.location.x;
            uint32_t y = grain.location.y;

            if (x > 0) {
                Grain_Drop spill {x - 1, y, qty_per_cell + rest_amount};
                overspill.push_back(spill);
                rest_amount = 0;
            }
            if (x < _width - 1) {
                Grain_Drop spill {x + 1, y, qty_per_cell + rest_amount};
                overspill.push_back(spill);
                rest_amount = 0;
            }
            if (y > 0) {
                Grain_Drop spill {x, y - 1, qty_per_cell + rest_amount};
                overspill.push_back(spill);
                rest_amount = 0;
            }
            if (y < _height - 1) {
                Grain_Drop spill {x, y + 1, qty_per_cell + rest_amount};
                overspill.push_back(spill);
                rest_amount = 0;
            }
            set_cell_height_at(grain.location, 0);
        }
    }
}


uint8_t *Sandpile::get_data() {
    uint8_t *result = nullptr;

    if (_data && _data_count > 0) {
        result = _data;
    }

    return result;
}


bool Sandpile::run(Run_Session const& session) {
    std::vector<Grain_Drop> overspill;

    for (uint32_t turn_count = 0; turn_count < session.number_of_iterations; ++turn_count) {
        for (auto grain_drop : session.grain_drops) {
            drop_grain(grain_drop, overspill);
        }

        Avalanche avalanche {};
        avalanche.turn = turn_count;
        float time = 0.0f;

        while (overspill.size() > 0) {
            Grain_Drop grain = overspill.back();
            overspill.pop_back();
            drop_grain(grain, overspill);

            avalanche.size += grain.count;
            time += 0.25f;
        }

        avalanche.time = static_cast<uint32_t>(ceil(time));

        if (avalanche.size > 0) {
            _avalanches.push_back(avalanche);
        }
    }

    return true;
}