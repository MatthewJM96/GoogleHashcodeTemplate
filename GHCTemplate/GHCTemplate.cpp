#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>

struct Data {
    bool isValid;
};

/// Model that acquires the data.
class Loader {
public:
    Loader() : m_data({}) {}
    ~Loader() {
        dispose();
    }

    void init(std::string filepath = "example.in") {
        m_filepath = filepath;
    }
    void dispose() {
        // Close file is still open.
        if (m_file.is_open()) {
            m_file.close();
        }

        // Clear up data.
        m_data = {};
    }

    void setFilepath(std::string filepath) {
        m_filepath = filepath;
    }

    Data getData() {
        // If we haven't yet loaded data from the file, do so.
        if (m_data.isValid == 0) {
            loadDataFromFile();
        }
        return m_data;
    }
private:
    bool openFile() {
        m_file.open(m_filepath, std::ios::in);
        return m_file.is_open();
    }

    void loadDataFromFile() {
        // If file isn't already open, and failed to open on an attempt, exit the program.
        if (!m_file.is_open() && !openFile()) {
            std::cout << "Could not open file: " << m_filepath << "." << std::endl
                << "Exiting..." << std::endl;
            std::getchar();
            exit(0);
        }

        // Grab metadata.
        std::string metadata;
        std::getline(m_file, metadata);

        std::stringstream stream(metadata);


        // TODO: Acquire metadata and then load data.
    }

    std::fstream m_file;
    std::string m_filepath;

    Data m_data;
};

/// Interface for solvers.
class ISolver {
    using REPLACE_ME = bool;
public:
    virtual void solve() = 0;

    REPLACE_ME getResult() const {
        return m_result;
    }
private:
    REPLACE_ME m_result;
};

// Generic simulated annealing implementation.
template <typename State, typename Energy = double, typename Temperature = double,
    typename Generator = std::mt19937_64, typename Count = size_t,
    typename std::enable_if<std::is_signed<Energy>::value && std::is_arithmetic<Temperature>::value && std::is_arithmetic<Count>::value>::type* = 0>
class SimulatedAnnealer {
    public:
        using EnergyFunc = Energy(*)(State);
        using TemperatureFunc = Temperature(*)(Count);
        using NextFunc = State(*)(State, Energy);

        SimulatedAnnealer() {}
        ~SimulatedAnnealer() {
            dispose();
        }

        SimulatedAnnealer* init(EnergyFunc eFunc = nullptr, TemperatureFunc tFunc = nullptr, NextFunc nFunc = nullptr) {
            if (m_initialised) return this;
            m_initialised = true;

            m_energyFunc = eFunc;
            m_temperatureFunc = tFunc;
            m_nextFunc = nFunc;

            return this;
        }
        SimulatedAnnealer* dispose() {
            if (!m_initialised) return this;
            m_initialised = false;

            m_energyFunc = nullptr;
            m_temperatureFunc = nullptr;
            m_nextFunc = nullptr;

            return this;
        }

        SimulatedAnnealer* setEnergyFunc(EnergyFunc eFunc) {
            m_energyFunc = eFunc;
            return this;
        }
        SimulatedAnnealer* setTemperatureFunc(TemperatureFunc tFunc) {
            m_temperatureFunc = tFunc;
            return this;
        }
        SimulatedAnnealer* setNextFunc(NextFunc nFunc) {
            m_nextFunc = nFunc;
            return this;
        }

        State run(State initialState, Count iterationCount) {
            std::random_device rd;
            Generator generator(rd());
#define oldState initialState
#define count iterationCount
            Energy oldEnergy = m_energyFunc(initialState);

            State  bestState = initialState;
            Energy bestEnergy = oldEnergy;

            std::uniform_real_distribution<Energy> randFunc((Energy)0, (Energy)1);

            for (; count > 0; --count) {
                State newState = m_nextFunc(oldState, std::abs(bestEnergy - oldEnergy));
                Energy newEnergy = m_energyFunc(newState);

                if (newEnergy < bestEnergy) {
                    bestState = newState;
                    bestEnergy = newEnergy;
                    oldState = std::move(newState);
                    oldEnergy = std::move(newEnergy);
                    continue;
                }

                Temperature t = m_temperatureFunc(count);
                //std::cout << t << "  -  " << newEnergy << std::endl;
                if (newEnergy < oldEnergy || std::exp((oldEnergy - newEnergy) / t) > randFunc(generator)) {
                    oldState = std::move(newState);
                    oldEnergy = std::move(newEnergy);
                }
            }

            return bestState;
#undef oldState
#undef count
        }
    private:
        bool m_initialised;

        EnergyFunc      m_energyFunc;
        TemperatureFunc m_temperatureFunc;
        NextFunc        m_nextFunc;
};

int main() {


    std::cout << "Press any key to exit..." << std::endl;
    getchar();
    return 0;
}
