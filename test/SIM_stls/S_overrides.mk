
TRICK_CFLAGS += -I./models -DTRICK_UNIT_TEST
TRICK_CXXFLAGS += -I./models

TRICK_CXXFLAGS += -std=c++11 -DTRICK_UNIT_TEST

clean: checkpoint_clean

checkpoint_clean:
	rm -f RUN_test/chkpnt_in
