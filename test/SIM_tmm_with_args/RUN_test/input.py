import math
from trick.unit_test import *

trick.sim_control_panel_set_enabled(True)
trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)

import trick

# Test
EXPECT_EQ(atwargs_1.some_int, 5)
EXPECT_EQ(atwargs_1.some_double, 10.0)




#Use tmm_alloc_args from input file - thanks convert_swig!
alloc_test.atwargs_input_file = trick.AllocTestWithArguments(5, 7.0, TMMName="tmm_alloc_args_input_test_1")





