// @trick_parse{everything}

#include "trick/Event.hh"

/**
 * Induce an `%import sim_services` statement in this class's Python module by inheriting from a
 * Trick class. This allows us to test if `sys.path` contains the correct path to `sim_services.py`
 * (and other modules generated during a sim build) for Trickified projects.
 */
class Bar : public Trick::Event {

    int  process(long long) {return 0;}
    void add();
    void remove();
    void restart();

};
