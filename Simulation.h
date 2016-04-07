#ifndef SIMULATION_H
#define SIMULATION_H

class Simulation
{
public:
	virtual ~Simulation() {}

	virtual bool initialize() = 0;

	virtual void update() = 0;

	virtual void render() const = 0;
};

#endif // SIMULATION_H
