
#include <gtest/gtest.h>
#include "MM_test.hh"
#include "MM_user_defined_types.hh"


/*
 Test Fixture.
 */
class MM_declare_var : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_declare_var() { memmgr = new Trick::MemoryManager; }
		~MM_declare_var() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/*
 This template function validates an ALLOC_INFO record created as a result
 of creating a variable using declare_var.
 */
template <typename T>
void validate_alloc_info_local(Trick::MemoryManager *memmgr,
                T *test_var,
                TRICK_TYPE trick_type,
                const char *type_name,
                const char *var_name,
                int num,
                int num_index,
                int *extent ) {

        ALLOC_INFO* alloc_info = NULL;
        alloc_info = memmgr->get_alloc_info_of(test_var);

        validate_alloc_info(alloc_info, test_var, trick_type, type_name, var_name, num, num_index, extent);

        EXPECT_EQ( alloc_info->stcl, TRICK_LOCAL) <<
                "Storage class (stcl) for a var created with var_declare should always be TRICK_LOCAL.";
}

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_declare_var, Character) {
	char *test_var = (char *)memmgr->declare_var("char");
        validate_alloc_info_local(memmgr, test_var, TRICK_CHARACTER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, String) {
        std::string *test_var = (std::string *)memmgr->declare_var("std::string");
        validate_alloc_info_local(memmgr, test_var, TRICK_STRING, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, wchar) {
        wchar_t *test_var = (wchar_t *)memmgr->declare_var("wchar");
        validate_alloc_info_local(memmgr, test_var, TRICK_WCHAR, NULL, NULL, 1, 0, NULL);
}


TEST_F(MM_declare_var, SignedCharacter) {
	unsigned char *test_var = (unsigned char *)memmgr->declare_var("signed char");
        validate_alloc_info_local(memmgr, test_var, TRICK_CHARACTER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, INT8_T) {
	unsigned char *test_var = (unsigned char *)memmgr->declare_var("int8_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_CHARACTER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UnsignedCharacter) {
	unsigned char *test_var = (unsigned char *)memmgr->declare_var("unsigned char");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_CHARACTER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UINT8_T) {
	unsigned char *test_var = (unsigned char *)memmgr->declare_var("uint8_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_CHARACTER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, Short) {
        short *test_var = (short *)memmgr->declare_var("short");
        validate_alloc_info_local(memmgr, test_var, TRICK_SHORT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, SignedShort) {
        short *test_var = (short *)memmgr->declare_var("signed short");
        validate_alloc_info_local(memmgr, test_var, TRICK_SHORT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, INT16_T) {
        short *test_var = (short *)memmgr->declare_var("int16_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_SHORT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UnsignedShort) {
        unsigned short *test_var = (unsigned short *)memmgr->declare_var("unsigned short");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_SHORT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UINT16_T) {
        short *test_var = (short *)memmgr->declare_var("uint16_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_SHORT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, Integer) {
        int *test_var = (int *)memmgr->declare_var("int");
        validate_alloc_info_local(memmgr, test_var, TRICK_INTEGER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, SignedInteger) {
        int *test_var = (int *)memmgr->declare_var("signed int");
        validate_alloc_info_local(memmgr, test_var, TRICK_INTEGER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, INT32_T) {
        int *test_var = (int *)memmgr->declare_var("int32_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_INTEGER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UnsignedInteger) {
        unsigned int *test_var = (unsigned int *)memmgr->declare_var("unsigned int");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_INTEGER, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UINT32_T) {
        unsigned int *test_var = (unsigned int *)memmgr->declare_var("uint32_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_INTEGER, NULL, NULL, 1, 0, NULL);
}

#if __linux__
#  include <stdint.h>
#  include <sys/types.h>
#  if __WORDSIZE == 64
// These tests should be excluded on a 32 bit architecture
TEST_F(MM_declare_var, INT64_T) {
        long *test_var = (long *)memmgr->declare_var("int64_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_INT64, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UINT64_T) {
        unsigned long *test_var = (unsigned long *)memmgr->declare_var("uint64_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_UINT64, NULL, NULL, 1, 0, NULL);
}
#  endif
#else 

// Any OS other than linux is assumed to be 64 bit
TEST_F(MM_declare_var, INT64_T) {
        long *test_var = (long *)memmgr->declare_var("int64_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_INT64, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UINT64_T) {
        unsigned long *test_var = (unsigned long *)memmgr->declare_var("uint64_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_UINT64, NULL, NULL, 1, 0, NULL);
}
#endif


TEST_F(MM_declare_var, Long) {
        long *test_var = (long *)memmgr->declare_var("long");
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, SignedLong) {
        long *test_var = (long *)memmgr->declare_var("signed long");
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UnsignedLong) {
        unsigned long *test_var = (unsigned long *)memmgr->declare_var("unsigned long");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, SignedLongLong) {
        long long *test_var = (long long *)memmgr->declare_var("signed long long");
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, Float) {
        float *test_var = (float *)memmgr->declare_var("float");
        validate_alloc_info_local(memmgr, test_var, TRICK_FLOAT, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, Double) {
        double *test_var = (double *)memmgr->declare_var("double");
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 1, 0, NULL);
}

// Bitfields are tested elsewhere.

TEST_F(MM_declare_var, LongLong) {
        long long *test_var = (long long *)memmgr->declare_var("long long");
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UnsignedLongLong) {
        unsigned long long *test_var = (unsigned long long *)memmgr->declare_var("unsigned long long");
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_LONG_LONG, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, DELIBERATE_SYNTAX_ERROR_IN_LEXER) {
    std::cout << ISO_6429_White_Background
              << ISO_6429_Blue_Foreground
              << ISO_6429_Underline
              << "NOTE: An error message is expected in this test."
              << ISO_6429_Restore_Default
              << std::endl;
              float *test_var = (float *)memmgr->declare_var("??");
              ASSERT_TRUE(test_var == NULL);
}

// Hmmm. What to do with the silly TRICK_FILE_POINTER type.

TEST_F(MM_declare_var, Boolean) {
        bool *test_var = (bool *)memmgr->declare_var("bool");
        validate_alloc_info_local(memmgr, test_var, TRICK_BOOLEAN, NULL, NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, WideCharacter) {
        wchar_t *test_var = (wchar_t *)memmgr->declare_var("wchar_t");
        validate_alloc_info_local(memmgr, test_var, TRICK_WCHAR, NULL, NULL, 1, 0, NULL);
}

// Named Variables
TEST_F(MM_declare_var, NamedVariable) {
        double *test_var = (double *)memmgr->declare_var("double force");
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, "force", 1, 0, NULL);
}

// Intrinsic Type Arrays

TEST_F(MM_declare_var, AnonOneDimDoubleArray) {
	double *test_var = (double *)memmgr->declare_var("double[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 5, 1, extents);
}

TEST_F(MM_declare_var, NamedOneDimDoubleArray) {
	double *test_var = (double *)memmgr->declare_var("double myarray[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, "myarray", 5, 1, extents);
}

TEST_F(MM_declare_var, NamedOneDimDoubleArrayHexIndex) {
	double *test_var = (double *)memmgr->declare_var("double myarray[0x05]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, "myarray", 5, 1, extents);
}

TEST_F(MM_declare_var, AnonOneDimDoublePtrArray) {
	double **test_var = (double **)memmgr->declare_var("double*[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 5, 2, extents);
}

TEST_F(MM_declare_var, AnonTwoDimDoubleArray) {
	double *test_var = (double *)memmgr->declare_var("double[3][4]");
        int extents[8] = {3,4,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 12, 2, extents);
}

TEST_F(MM_declare_var, AnonThreeDimDoubleArray) {
	double *test_var = (double *)memmgr->declare_var("double[2][3][4]");
        int extents[8] = {2,3,4,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 24, 3, extents);
}

TEST_F(MM_declare_var, TooManyDimensions) {
    std::cout << ISO_6429_White_Background
              << ISO_6429_Blue_Foreground
              << ISO_6429_Underline
              << "NOTE: An error message is expected in this test."
              << ISO_6429_Restore_Default
              << std::endl;
	double *test_var = (double *)memmgr->declare_var("double[2][2][2][2][2][2][2][2][2]");
    ASSERT_TRUE(test_var == NULL);
}

// User-defined types

TEST_F(MM_declare_var, UserDefinedSingleton) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1");
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var, UserDefinedOneDimArray) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1[3]");
        int extents[8] = {3,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 3, 1, extents);
}

TEST_F(MM_declare_var, UserDefinedTwoDimArray) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1[3][4]");
        int extents[8] = {3,4,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 12, 2, extents);
}
