import math
from trick.unit_test import *

def test_single_alloc():
    test_suite = "AllocatingUserDefinedTypes"

    test_case = "type_AllocTestWithArguments"

    alloc_test.atwargs_input_file = trick.AllocTestWithArguments(5, 7.0)

    TRICK_EXPECT_EQ(alloc_test.atwargs_1.some_int, 0, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_1.some_double, 0.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_2.some_int, 0, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_2.some_double, 1.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_3.some_int, 5, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_3.some_double, 10.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_4.some_int, 4, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_4.some_double, 8.0, 1e-6, test_case, test_suite)

def test_placement_new():
    test_suite = "AllocatingUserDefinedTypes"
    test_case = "placement_new"
    #
    TRICK_EXPECT_EQ(alloc_test.atwargs_5.some_int, 4, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_5.some_double, 8.0, 1e-6, test_case, test_suite)


def test_python_alloc():
    test_suite = "AllocatingUserDefinedTypes"
    test_case = "IP_allocs"

    some_str = "test_str"

    #Default constructor
    alloc_1 = trick.AllocTestWithArguments()
    #alloc_1.thisown = 0
    TRICK_EXPECT_EQ(alloc_1.some_int, 0, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_1.some_double, 0, 1e-6, test_case, test_suite)

    alloc_2 = trick.AllocTestWithArguments(5, 10.0, TMMName="Alloc2")

    TRICK_EXPECT_EQ(alloc_2.some_int, 5, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_2.some_double, 10.0, 1e-6, test_case, test_suite)

    #Constructor: AllocTestWithArguments::AllocTestWithArguments(int in_int, double in_double, const char* a_name)
    alloc_3 = trick.AllocTestWithArguments(5, 10.0, some_str)
    #alloc_3.thisown = 0
    TRICK_EXPECT_EQ(alloc_3.some_int, 5, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_3.some_double, 10.0, 1e-6, test_case, test_suite)

    #AllocTestWithArguments::AllocTestWithArguments(int in_int, double in_double, const char* a_name)
    alloc_4 = trick.AllocTestWithArguments(5, 10.0,  some_str)
    #alloc_4.thisown = 0
    TRICK_EXPECT_EQ(alloc_4.some_int, 5, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_4.some_double, 10.0, 1e-6, test_case, test_suite)
    
    
def main():
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_tmm_alloc_args.xml" ) 
    trick_utest.unit_tests.set_test_name( "TMMAllocWithArgsTest" ) 

    trick.add_read(2.0, """test_single_alloc()""")
    trick.add_read(2.0, """test_placement_new()""")
    trick.add_read(2.0, """test_python_alloc()""")


trick.stop(5.0)


if __name__ == "__main__":
    main()