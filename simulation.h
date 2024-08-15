#ifndef SIMULATION_H
#define SIMULATION_H

using namespace std;

class Simulation {
public:
    Simulation();
    ~Simulation();

    void run();

    void init();
    void update();
    void render();
    void clean();

    bool isRunning();

private:

};



#endif