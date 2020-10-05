#include "RigidBody.h"
#include <iomanip>
#include <sstream>

constexpr const int TERMINAL_VELOCITY_PRECISION = 2; // significant figures of precision before stopping terminal velocity recursive calculation

// function that uses recursion to see what velocity will converge to given certain drag and acceleration
static V2_double FindTerminalVelocity(V2_double drag, V2_double maxAcceleration, V2_double initialVelocity = {}) {
	// store previous velocity for precision comparison to know when to exit recursion
	static auto previousVelocity = initialVelocity;
	auto velocity = (initialVelocity + maxAcceleration) * drag;
	std::stringstream ss1, ss2;
	// limit the precision so the recursion doesn't take too long
	ss1 << std::fixed << std::setprecision(TERMINAL_VELOCITY_PRECISION) << velocity.x;
	ss2 << std::fixed << std::setprecision(TERMINAL_VELOCITY_PRECISION) << previousVelocity.x;
	if (ss1.str() != ss2.str()) {
		previousVelocity = velocity;
		return FindTerminalVelocity(drag, maxAcceleration, velocity);
	} else {
		return velocity;
	}
}

void RigidBody::Init() {
	if (mass == 0.0) {
		inverseMass = 0.0;
	} else {
		inverseMass = 1.0 / mass;
	}
	ComputeTerminalVelocity();
}

void RigidBody::ComputeTerminalVelocity() {
	if (terminalVelocity.IsInfinite() && !drag.IsZero() && !maximumAcceleration.IsZero() && !maximumAcceleration.IsInfinite()) { // terminal velocity not set
		terminalVelocity = FindTerminalVelocity(V2_double{ 1.0, 1.0 } - drag, maximumAcceleration);
	}
}