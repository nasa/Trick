package trick.jobperf;

/**
* Class InvalidFrameBoundsExpection is an exception indicating
* that the user has specified an illegal range for the frames
* to be rendered.
*/
class InvalidFrameBoundsExpection extends Exception {
    public InvalidFrameBoundsExpection(String message) {
        super(message);
    }
}
