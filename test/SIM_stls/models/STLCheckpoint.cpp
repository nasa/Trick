
#include "sim_services/Message/include/message_proto.h"
#include "STLCheckpoint.hh"
#include "trick/memorymanager_c_intf.h"
#include <iostream>
#include <vector>
#include "sim_services/UnitTest/include/trick_tests.h"
/* These 2 constructors add different data to an STLCheckpoint. */

STLCheckpoint::STLCheckpoint() {
    return ;
}

template <typename T>
void compare_data_structures_with_top(T a, T b, std::string test_name) {
    const char *test_suite = "STLCheckpoint";

    TRICK_EXPECT_EQ(a.size(), b.size(), test_suite, test_name.c_str());
    while (!a.empty()) {
        TRICK_EXPECT_EQ(a.top(), b.top(), test_suite, test_name.c_str());
        a.pop();
        b.pop();
    }
}

int STLCheckpoint::addData() {
    dataJobRun = true;
    vector_vector_double = std::vector< std::vector< double > >(4, std::vector<double>(3));
    vector_vector_vector_double = std::vector< std::vector< std::vector< double > > >(5, std::vector<std::vector<double> >(4, std::vector<double>(3)));

    std::cout << "Adding hardcoded data to sim" << std::endl;

    double_map[44.4] = 444.4 ;
    double_map[55.5] = 555.5 ;
    double_map[66.6] = 666.6 ;

    string_key_map[std::string("four")] = 4 ;
    string_key_map[std::string("five")] = 5 ;
    string_key_map[std::string("six")] = 6 ;

    string_data_map[7] = std::string("seiben") ;
    string_data_map[8] = std::string("acht") ;
    string_data_map[9] = std::string("neun") ;

    std::vector< int > v ;
    v.push_back(2) ;
    v.push_back(4) ;
    v.push_back(6) ;
    v.push_back(8) ;
    map_int_vector_int[1] = v ;

    std::pair< int , int > p ;
    p.first = 24 ;
    p.second = 30 ;
    gcd[p] = 6 ;

    p.first = 50 ;
    p.second = 60 ;
    std::pair< int , int > q ;
    q.first = 70 ;
    q.second = 80 ;
    map_pair_pair[p] = q ;

    p.first = 3 ;
    p.second = 5 ;
    v.clear() ;
    v.push_back(15) ;
    v.push_back(30) ;
    v.push_back(45) ;
    v.push_back(60) ;
    common_multiples.insert(std::pair< std::pair< int, int >, std::vector< int > >(p,v)) ;

    p.first = 3 ;
    p.second = 7 ;
    v.clear() ;
    v.push_back(21) ;
    v.push_back(42) ;
    v.push_back(84) ;
    v.push_back(168) ;
    common_multiples.insert(std::pair< std::pair< int, int >, std::vector< int > >(p,v)) ;

    string_map[std::string("sister")] = std::string("Lisa") ;
    string_map[std::string("dog")] = std::string("Santa's Little Helper") ;

    int_multimap.insert(std::pair<int, int>(44,444)) ;
    int_multimap.insert(std::pair<int, int>(55,555)) ;
    int_multimap.insert(std::pair<int, int>(66,666)) ;
    int_multimap.insert(std::pair<int, int>(44,4444)) ;
    int_multimap.insert(std::pair<int, int>(55,5555)) ;
    int_multimap.insert(std::pair<int, int>(66,6666)) ;

    string_key_multimap.insert(std::pair<std::string, int>("four", 4)) ;
    string_key_multimap.insert(std::pair<std::string, int>("five", 5)) ;
    string_key_multimap.insert(std::pair<std::string, int>("six", 6)) ;
    string_key_multimap.insert(std::pair<std::string, int>("four", 44)) ;
    string_key_multimap.insert(std::pair<std::string, int>("five",  55)) ;
    string_key_multimap.insert(std::pair<std::string, int>("six", 66)) ;

    string_data_multimap.insert(std::pair<int, std::string>(7, "seiben")) ;
    string_data_multimap.insert(std::pair<int, std::string>(8, "acht")) ;
    string_data_multimap.insert(std::pair<int, std::string>(9, "neun")) ;
    string_data_multimap.insert(std::pair<int, std::string>(7, "seven")) ;
    string_data_multimap.insert(std::pair<int, std::string>(8, "eight")) ;
    string_data_multimap.insert(std::pair<int, std::string>(9, "nine")) ;

    string_multimap.insert(std::pair<std::string, std::string>("sister","Lisa")) ;
    string_multimap.insert(std::pair<std::string, std::string>("dog","Santa's Little Helper")) ;
    string_multimap.insert(std::pair<std::string, std::string>("sister","Meg")) ;
    string_multimap.insert(std::pair<std::string, std::string>("dog","Brian")) ;

    double_vector.push_back(4.0) ;
    double_vector.push_back(5.0) ;
    double_vector.push_back(6.0) ;

    string_vector.push_back("It") ;
    string_vector.push_back("has") ;
    string_vector.push_back("the") ;
    string_vector.push_back("Wi-Fies") ;

    short_list.push_back(400) ;
    short_list.push_back(401) ;
    short_list.push_back(402) ;

    string_list.push_back("I") ;
    string_list.push_back("don't") ;
    string_list.push_back("care") ;

    float_deque.push_back(98.7) ;
    float_deque.push_back(65.4) ;
    float_deque.push_back(32.1) ;

    string_deque.push_back("Welcome") ;
    string_deque.push_back("to") ;
    string_deque.push_back("PhoneMart") ;

    int_set.insert(8000) ;
    int_set.insert(4000) ;
    int_set.insert(2000) ;
    int_set.insert(1000) ;

    string_set.insert("efg") ;
    string_set.insert("abc") ;
    string_set.insert("def") ;

    for (int j = 0; j < 5; j++) {
        std::vector<int> temp;
        for (int i = j; i < 10; i++) {
            temp.push_back(i);
        }
        vector_set.insert(temp);
    }

    pair_set.emplace("One", 1);
    pair_set.emplace("Two", 2);
    pair_set.emplace("Three", 3);

    std::map<short,double> m;
    m[5] = 500.5;
    m[1] = 100.1;
    std::map<short,double> m2;
    m2[5] = 5000.5;
    m2[1] = 1000.1; 

    nested_map_set.insert(m);
    nested_map_set.insert(m2);

    for (int j = 0; j < 5; j++) {
        std::vector<int> temp;
        for (int i = j; i < 10; i++) {
            temp.push_back(i);
        }
        vector_queue.push(temp);
    }


    long_multiset.insert(8000) ;
    long_multiset.insert(4000) ;
    long_multiset.insert(4000) ;
    long_multiset.insert(2000) ;
    long_multiset.insert(1000) ;

    string_multiset.insert("efg") ;
    string_multiset.insert("abc") ;
    string_multiset.insert("def") ;
    string_multiset.insert("efg") ;
    string_multiset.insert("abc") ;
    string_multiset.insert("def") ;

    pair_multiset.emplace(5, 5);
    pair_multiset.emplace(5, 5);
    pair_multiset.emplace(5, 4);
    pair_multiset.emplace(4, 4);

    vec_multiset.emplace(std::vector<int>({1,2,3,4}));
    vec_multiset.emplace(std::vector<int>({1,2,3,4}));
    vec_multiset.emplace(std::vector<int>({1,2,3,4,5}));


    uint_stack.push(1) ;
    uint_stack.push(2) ;
    uint_stack.push(3) ;
    uint_stack.push(4) ;

    string_stack.push("I") ;
    string_stack.push("want the one") ;
    string_stack.push("with the bigger") ;
    string_stack.push("Gee Bees") ;

    std::vector<int> temp_v1 = {1, 2, 3, 4};
    std::vector<int> temp_v2 = {5, 6, 7, 8};
    stack_vector_int.push(temp_v1) ;
    stack_vector_int.push(temp_v2) ;

    int_queue.push(1) ;
    int_queue.push(2) ;
    int_queue.push(3) ;
    int_queue.push(4) ;

    string_queue.push("I") ;
    string_queue.push("want") ;
    string_queue.push("an") ;
    string_queue.push("iPhone 4") ;

    int_priority_queue.push(3) ;
    int_priority_queue.push(2) ;
    int_priority_queue.push(4) ;
    int_priority_queue.push(1) ;

    string_priority_queue.push("I") ;
    string_priority_queue.push("want") ;
    string_priority_queue.push("an") ;
    string_priority_queue.push("iPhone 4") ;

    queue_vector_int.push(temp_v1) ;
    queue_vector_int.push(temp_v2) ;

    priority_queue_vector_int.push(temp_v1) ;
    priority_queue_vector_int.push(temp_v2) ;

    int_pair.first = 10 ;
    int_pair.second = 20 ;

    string_first_pair.first = "string first" ;
    string_first_pair.second = 25 ;

    string_second_pair.first = 25 ;
    string_second_pair.second = "string second" ;

    string_pair.first = "pair first string" ;
    string_pair.second = "pair second string" ;

    int_pair_int_int.first = 200 ;
    p.first = 10 ;
    p.second = 20 ;
    int_pair_int_int.second = p ;

    p.first = 15 ;
    p.second = 12 ;
    pair_int_int_int.first = p ;
    pair_int_int_int.second = 180 ;

    pair_pair_pair.first.first = 51 ;
    pair_pair_pair.first.second = 52 ;
    pair_pair_pair.second.first = 53 ;
    pair_pair_pair.second.second = 54 ;

    int_vec_pair.first = 5;
    int_vec_pair.second.push_back(5);
    int_vec_pair.second.push_back(10);
    int_vec_pair.second.push_back(15);
    int_vec_pair.second.push_back(20);

    vec_int_pair.second = 5;
    vec_int_pair.first.push_back(5);
    vec_int_pair.first.push_back(10);
    vec_int_pair.first.push_back(15);
    vec_int_pair.first.push_back(20);

    vec_vec_pair.first.push_back(5);
    vec_vec_pair.first.push_back(10);
    vec_vec_pair.first.push_back(15);
    vec_vec_pair.first.push_back(20);
    vec_vec_pair.second.push_back(5);
    vec_vec_pair.second.push_back(10);
    vec_vec_pair.second.push_back(15);
    vec_vec_pair.second.push_back(20);

    vector_vector_double[0][0] = 100 ;
    vector_vector_double[0][1] = 101 ;
    vector_vector_double[0][2] = 102 ;
    vector_vector_double[1][0] = 103 ;
    vector_vector_double[1][1] = 104 ;
    vector_vector_double[1][2] = 105 ;
    vector_vector_double[2][0] = 106 ;
    vector_vector_double[2][1] = 107 ;
    vector_vector_double[2][2] = 108 ;
    vector_vector_double[3][0] = 109 ;
    vector_vector_double[3][1] = 110 ;
    vector_vector_double[3][2] = 111 ;

    vector_vector_vector_double[0][0][0] = 0 ;
    vector_vector_vector_double[0][0][1] = 1 ;
    vector_vector_vector_double[0][0][2] = 2 ;
    vector_vector_vector_double[0][1][0] = 3 ;
    vector_vector_vector_double[0][1][1] = 4 ;
    vector_vector_vector_double[0][1][2] = 5 ;
    vector_vector_vector_double[0][2][0] = 6 ;
    vector_vector_vector_double[0][2][1] = 7 ;
    vector_vector_vector_double[0][2][2] = 8 ;
    vector_vector_vector_double[0][3][0] = 9 ;
    vector_vector_vector_double[0][3][1] = 10 ;
    vector_vector_vector_double[0][3][2] = 11 ;

    vector_vector_vector_double[1][0][0] = 1000 ;
    vector_vector_vector_double[1][0][1] = 1001 ;
    vector_vector_vector_double[1][0][2] = 1002 ;
    vector_vector_vector_double[1][1][0] = 1003 ;
    vector_vector_vector_double[1][1][1] = 1004 ;
    vector_vector_vector_double[1][1][2] = 1005 ;
    vector_vector_vector_double[1][2][0] = 1006 ;
    vector_vector_vector_double[1][2][1] = 1007 ;
    vector_vector_vector_double[1][2][2] = 1008 ;
    vector_vector_vector_double[1][3][0] = 1009 ;
    vector_vector_vector_double[1][3][1] = 1010 ;
    vector_vector_vector_double[1][3][2] = 1011 ;

    vector_vector_vector_double[2][0][0] = 2000 ;
    vector_vector_vector_double[2][0][1] = 2001 ;
    vector_vector_vector_double[2][0][2] = 2002 ;
    vector_vector_vector_double[2][1][0] = 2003 ;
    vector_vector_vector_double[2][1][1] = 2004 ;
    vector_vector_vector_double[2][1][2] = 2005 ;
    vector_vector_vector_double[2][2][0] = 2006 ;
    vector_vector_vector_double[2][2][1] = 2007 ;
    vector_vector_vector_double[2][2][2] = 2008 ;
    vector_vector_vector_double[2][3][0] = 2009 ;
    vector_vector_vector_double[2][3][1] = 2010 ;
    vector_vector_vector_double[2][3][2] = 2011 ;

    vector_vector_vector_double[3][0][0] = 3000 ;
    vector_vector_vector_double[3][0][1] = 3001 ;
    vector_vector_vector_double[3][0][2] = 3002 ;
    vector_vector_vector_double[3][1][0] = 3003 ;
    vector_vector_vector_double[3][1][1] = 3004 ;
    vector_vector_vector_double[3][1][2] = 3005 ;
    vector_vector_vector_double[3][2][0] = 3006 ;
    vector_vector_vector_double[3][2][1] = 3007 ;
    vector_vector_vector_double[3][2][2] = 3008 ;
    vector_vector_vector_double[3][3][0] = 3009 ;
    vector_vector_vector_double[3][3][1] = 3010 ;
    vector_vector_vector_double[3][3][2] = 3011 ;

    vector_vector_vector_double[4][0][0] = 4000 ;
    vector_vector_vector_double[4][0][1] = 4001 ;
    vector_vector_vector_double[4][0][2] = 4002 ;
    vector_vector_vector_double[4][1][0] = 4003 ;
    vector_vector_vector_double[4][1][1] = 4004 ;
    vector_vector_vector_double[4][1][2] = 4005 ;
    vector_vector_vector_double[4][2][0] = 4006 ;
    vector_vector_vector_double[4][2][1] = 4007 ;
    vector_vector_vector_double[4][2][2] = 4008 ;
    vector_vector_vector_double[4][3][0] = 4009 ;
    vector_vector_vector_double[4][3][1] = 4010 ;
    vector_vector_vector_double[4][3][2] = 4011 ;

    for (int i = 0; i < 10; i++) {
        UserClass temp;
        for (int j = 0; j < 5; j++) {
            temp.a[j] = i+j;
        }
        temp.b = 8888888888;
        temp.c = "Here is a test string";

        // We'll just make a pointer to the same stuff 

        UserClass * user_class_ptr = (UserClass *) TMM_declare_var_s("UserClass");
        for (int j = 0; j < 5; j++) {
            user_class_ptr->a[j] = i+j;
        }
        user_class_ptr->b = 8888888888;
        user_class_ptr->c = "Here is a test string";

        temp.d = user_class_ptr;

        vec_user_simple.emplace_back(temp);
    }

    for (int i = 0; i < 10; i++) {
        SimpleWrapper temp_wrapper;
        temp_wrapper.a = 888;
        for (int j = i; j < i+10; j++) {
            temp_wrapper.vec.push_back(j);
        }
        vec_user_defined.emplace_back(temp_wrapper);
    }

    for (int i = 0; i < 10; i++) {
        SimpleWrapper * temp_wrapper = (SimpleWrapper *) TMM_declare_var_s("SimpleWrapper");
        temp_wrapper->a = 888;
        for (int j = i; j < i+10; j++) {
            temp_wrapper->vec.push_back(j);
        }
        vec_user_defined_ptr.push_back(temp_wrapper);
    }

    return 0;
}

int STLCheckpoint::print() {
    message_publish(1,"Quack!\n") ;
    message_publish(1, "Double vector size: %d", double_vector.size() );
    message_publish(1,"double_vector: %f %f %f\n", double_vector[0], double_vector[1], double_vector[2]) ;
    message_publish(1,"vector_vector_double[1]: %f %f %f\n",
    vector_vector_double[1][0], vector_vector_double[1][1], vector_vector_double[1][2]) ;
    message_publish(1,"vector_vector_vector_double[4][2]: %f %f %f\n",
    vector_vector_vector_double[4][2][0], vector_vector_vector_double[4][2][1], vector_vector_vector_double[4][2][2]) ;
    message_publish(1,"string_vector[0]: %s\n", string_vector[0].c_str()) ;
    message_publish(1,"map_int_vector_int[1][1] = %d\n", map_int_vector_int[1][1]) ;
    message_publish(1,"gcd = %d\n", gcd[std::pair<int, int >(24,30)]) ;
    message_publish(1,"common_multiples = %d\n", common_multiples[std::pair<int, int >(3,5)][1]) ;
    message_publish(1,"common_multiples = %d\n", common_multiples[std::pair<int, int >(3,5)][1]) ;
    message_publish(1,"int_pair_int_int.second.second = %d\n", int_pair_int_int.second.second) ;
    message_publish(1,"pair_int_int_int.first.second = %d\n", pair_int_int_int.first.second) ;
    message_publish(1,"pair_pair_pair.second.first = %d\n", pair_pair_pair.second.first) ;
    message_publish(1,"int_queue.front = %d\n", int_queue.front()) ;
    message_publish(1,"int_priority_queue.top = %d\n", int_priority_queue.top()) ;
    message_publish(1,"uint_stack.top = %d\n", uint_stack.top()) ;
    message_publish(1,"queue_vector_int.front()[3] = %d\n", queue_vector_int.front()[3]) ;
    message_publish(1,"priority_queue_vector_int.top()[2] = %d\n", priority_queue_vector_int.top()[2]) ;
    message_publish(1,"stack_vector_int.top()[1] = %d\n", stack_vector_int.top()[1]) ;
    return 0 ;
}

int STLCheckpoint::test() {
    std::cout << "Running test jobs" << std::endl;
    const char *test_suite = "STLCheckpoint";

    TRICK_EXPECT_EQ( double_vector.size(), 3, test_suite, "double_vector size");
    TRICK_EXPECT_EQ( double_vector[0], 4, test_suite, "double_vector[0]");
    TRICK_EXPECT_EQ( double_vector[1], 5, test_suite, "double_vector[1]");
    TRICK_EXPECT_EQ( double_vector[2], 6, test_suite, "double_vector[2]");
    TRICK_EXPECT_EQ( vector_vector_double[1][0], 103, test_suite, "vector_vector_double[1][0]");
    TRICK_EXPECT_EQ( vector_vector_double[1][1], 104, test_suite, "vector_vector_double[1][1]");
    TRICK_EXPECT_EQ( vector_vector_double[1][2], 105, test_suite, "vector_vector_double[1][2]");
    TRICK_EXPECT_EQ( vector_vector_vector_double[4][2][0], 4006, test_suite, "vector_vector_vector_double[4][2][0]");
    TRICK_EXPECT_EQ( vector_vector_vector_double[4][2][1], 4007, test_suite, "vector_vector_vector_double[4][2][1]");
    TRICK_EXPECT_EQ( vector_vector_vector_double[4][2][2], 4008, test_suite, "vector_vector_vector_double[4][2][2]");
    TRICK_EXPECT_EQ( string_vector[0], std::string("It"), test_suite, "string_vector");

    TRICK_EXPECT_EQ( double_map[44.4], 444.4, test_suite, "double_map[44.4]" );

    TRICK_EXPECT_EQ( string_data_map[7], std::string("seiben"), test_suite, "string_data_map[7]" );
    TRICK_EXPECT_EQ( string_data_map[9], std::string("neun") , test_suite, "string_data_map[9]");

    std::vector< int > v ;
    v.push_back(2) ;
    v.push_back(4) ;
    v.push_back(6) ;
    v.push_back(8) ;
    TRICK_EXPECT_EQ( map_int_vector_int[1], v, test_suite, "map_int_vector_int[1]");

    std::pair< int , int > p ;
    p.first = 24 ;
    p.second = 30 ;
    TRICK_EXPECT_EQ(gcd[p], 6, test_suite, "gcd[p]" );

    p.first = 50 ;
    p.second = 60 ;
    std::pair< int , int > q ;
    q.first = 70 ;
    q.second = 80 ;
    TRICK_EXPECT_EQ( map_pair_pair[p], q, test_suite, "map_pair_pair[p]" );

    p.first = 3 ;
    p.second = 5 ;
    v.clear() ;
    v.push_back(15) ;
    v.push_back(30) ;
    v.push_back(45) ;
    v.push_back(60) ;
    TRICK_EXPECT_EQ( common_multiples[p], v, test_suite, "common_multiples" );

    p.first = 3 ;
    p.second = 7 ;
    v.clear() ;
    v.push_back(21) ;
    v.push_back(42) ;
    v.push_back(84) ;
    v.push_back(168) ;
    TRICK_EXPECT_EQ( common_multiples[p], v, test_suite, "common_multiples" );


    TRICK_EXPECT_EQ( string_map["sister"], "Lisa", test_suite, "string_map[sister]" );
    TRICK_EXPECT_EQ( string_map["dog"], "Santa's Little Helper", test_suite, "string_map[dog]" );

    
    // The easiest way to test the multimaps is to just rebuild copies and compare
    std::multimap<int, int> int_multimap_copy;
    int_multimap_copy.insert(std::pair<int, int>(44,444)) ;
    int_multimap_copy.insert(std::pair<int, int>(55,555)) ;
    int_multimap_copy.insert(std::pair<int, int>(66,666)) ;
    int_multimap_copy.insert(std::pair<int, int>(44,4444)) ;
    int_multimap_copy.insert(std::pair<int, int>(55,5555)) ;
    int_multimap_copy.insert(std::pair<int, int>(66,6666)) ;
    TRICK_EXPECT_EQ(int_multimap, int_multimap_copy , test_suite, "int_multimap");

    int_multimap_copy.insert(std::pair<int, int>(66,66666)) ;
    TRICK_EXPECT_NE(int_multimap, int_multimap_copy , test_suite, "int_multimap_fail");

    std::multimap<std::string, int> string_key_multimap_copy;
    string_key_multimap_copy.insert(std::pair<std::string, int>("four", 4)) ;
    string_key_multimap_copy.insert(std::pair<std::string, int>("five", 5)) ;
    string_key_multimap_copy.insert(std::pair<std::string, int>("six", 6)) ;
    string_key_multimap_copy.insert(std::pair<std::string, int>("four", 44)) ;
    string_key_multimap_copy.insert(std::pair<std::string, int>("five",  55)) ;
    string_key_multimap_copy.insert(std::pair<std::string, int>("six", 66)) ;
    TRICK_EXPECT_EQ(string_key_multimap, string_key_multimap_copy , test_suite, "string_key_multimap");

    std::multimap<int, std::string> string_data_multimap_copy;
    string_data_multimap_copy.insert(std::pair<int, std::string>(7, "seiben")) ;
    string_data_multimap_copy.insert(std::pair<int, std::string>(8, "acht")) ;
    string_data_multimap_copy.insert(std::pair<int, std::string>(9, "neun")) ;
    string_data_multimap_copy.insert(std::pair<int, std::string>(7, "seven")) ;
    string_data_multimap_copy.insert(std::pair<int, std::string>(8, "eight")) ;
    string_data_multimap_copy.insert(std::pair<int, std::string>(9, "nine")) ;
    TRICK_EXPECT_EQ(string_data_multimap, string_data_multimap_copy , test_suite, "string_data_multimap");

    std::multimap<std::string, std::string> string_multimap_copy;
    string_multimap_copy.insert(std::pair<std::string, std::string>("sister","Lisa")) ;
    string_multimap_copy.insert(std::pair<std::string, std::string>("dog","Santa's Little Helper")) ;
    string_multimap_copy.insert(std::pair<std::string, std::string>("sister","Meg")) ;
    string_multimap_copy.insert(std::pair<std::string, std::string>("dog","Brian")) ;
    TRICK_EXPECT_EQ(string_multimap, string_multimap_copy , test_suite, "string_multimap");


    // TODO: fix this case
    // TRICK_EXPECT_EQ(vec_user_defined.size(), 10, test_suite, "vec_user_defined");
    // for (int i = 0; i < vec_user_defined.size(); i++) {
    //     TRICK_EXPECT_EQ(vec_user_defined[i].vec.size(), 10, test_suite, "vec_user_defined");
    //     TRICK_EXPECT_EQ(vec_user_defined[i].a, 888, test_suite, "vec_user_defined");
    //     for (int j = i; j < i+vec_user_defined[i].vec.size(); j++) {
    //         TRICK_EXPECT_EQ(vec_user_defined[i].vec[j-i], j, test_suite, "vec_user_defined");
    //     }
    // }

    TRICK_EXPECT_EQ(vec_user_defined_ptr.size(), 10, test_suite, "vec_user_defined_ptr");
    for (int i = 0; i < vec_user_defined_ptr.size(); i++) {
        TRICK_EXPECT_EQ(vec_user_defined_ptr[i]->vec.size(), 10, test_suite, "vec_user_defined_ptr");
        TRICK_EXPECT_EQ(vec_user_defined_ptr[i]->a, 888, test_suite, "vec_user_defined_ptr");
        std::vector<int> test_vec;
        for (int j = i; j < i+vec_user_defined_ptr[i]->vec.size(); j++) {
            test_vec.push_back(j);
        }
        TRICK_EXPECT_EQ(vec_user_defined_ptr[i]->vec, test_vec, test_suite, "vec_user_defined_ptr");
    }


    std::vector<std::string> string_vector_copy = {"It", "has", "the", "Wi-Fies"};
    TRICK_EXPECT_EQ(string_vector, string_vector_copy, test_suite, "string_vector");

    std::list<short> short_list_copy = {400, 401, 402};
    int list_index = 0;
    TRICK_EXPECT_EQ(short_list, short_list_copy, test_suite, "short_list");

    std::list<std::string> string_list_copy = {"I", "don't", "care"};
    TRICK_EXPECT_EQ(string_list, string_list_copy, test_suite, "string_list");

    std::deque<float> float_deque_copy = {98.7, 65.4, 32.1};
    TRICK_EXPECT_EQ(float_deque, float_deque_copy, test_suite, "float_deque");

    std::deque<std::string> string_deque_copy = {"Welcome", "to", "PhoneMart"};
    TRICK_EXPECT_EQ(string_deque, string_deque_copy, test_suite, "string_deque");

    std::set<int> int_set_copy = {8000, 4000, 2000, 1000};
    TRICK_EXPECT_EQ(int_set, int_set_copy, test_suite, "int_set");

    std::set<std::string> string_set_copy = {"efg", "abc", "def"};
    TRICK_EXPECT_EQ(string_set, string_set_copy, test_suite, "string_set");

    std::set<std::vector<int>> vector_set_copy;
    for (int j = 0; j < 5; j++) {
        std::vector<int> temp;
        for (int i = j; i < 10; i++) {
            temp.push_back(i);
        }
        vector_set_copy.insert(temp);
    }
    TRICK_EXPECT_EQ(vector_set, vector_set_copy, test_suite, "vector_set");

    std::set<std::pair<std::string, int>> pair_set_copy;
    pair_set_copy.emplace("One", 1);
    pair_set_copy.emplace("Two", 2);
    pair_set_copy.emplace("Three", 3);
    TRICK_EXPECT_EQ(pair_set, pair_set_copy, test_suite, "pair_set");

    std::set<std::map<short,double>> nested_map_set_copy;
    std::map<short,double> m;
    m[5] = 500.5;
    m[1] = 100.1;
    std::map<short,double> m2;
    m2[5] = 5000.5;
    m2[1] = 1000.1; 
    nested_map_set_copy.insert(m);
    nested_map_set_copy.insert(m2);
    TRICK_EXPECT_EQ(nested_map_set, nested_map_set_copy, test_suite, "nested_map_set");

    std::queue<std::vector<int>> vector_queue_copy;
    for (int j = 0; j < 5; j++) {
        std::vector<int> temp;
        for (int i = j; i < 10; i++) {
            temp.push_back(i);
        }
        vector_queue_copy.push(temp);
    }
    TRICK_EXPECT_EQ(vector_queue, vector_queue_copy, test_suite, "vector_queue");

    std::multiset<long> long_multiset_copy = {8000, 4000, 4000, 2000, 1000};
    TRICK_EXPECT_EQ(long_multiset, long_multiset_copy, test_suite, "long_multiset");

    std::multiset<std::string> string_multiset_copy = {"efg", "abc", "def", "efg", "abc", "def"};
    TRICK_EXPECT_EQ(string_multiset, string_multiset_copy, test_suite, "string_multiset");

    std::multiset<std::pair<int,int>> pair_multiset_copy;
    pair_multiset_copy.emplace(5, 5);
    pair_multiset_copy.emplace(5, 5);
    pair_multiset_copy.emplace(5, 4);
    pair_multiset_copy.emplace(4, 4);
    TRICK_EXPECT_EQ(pair_multiset, pair_multiset_copy, test_suite, "pair_multiset");

    std::multiset<std::vector<int>> vec_multiset_copy;
    vec_multiset_copy.emplace(std::vector<int>({1,2,3,4}));
    vec_multiset_copy.emplace(std::vector<int>({1,2,3,4}));
    vec_multiset_copy.emplace(std::vector<int>({1,2,3,4,5}));
    TRICK_EXPECT_EQ(vec_multiset, vec_multiset_copy, test_suite, "vec_multiset");

    std::stack<unsigned int> uint_stack_copy;
    uint_stack_copy.push(1) ;
    uint_stack_copy.push(2) ;
    uint_stack_copy.push(3) ;
    uint_stack_copy.push(4) ;
    TRICK_EXPECT_EQ(uint_stack, uint_stack_copy, test_suite, "uint_stack");

    std::stack<std::string> string_stack_copy;
    string_stack_copy.push("I") ;
    string_stack_copy.push("want the one") ;
    string_stack_copy.push("with the bigger") ;
    string_stack_copy.push("Gee Bees") ;
    TRICK_EXPECT_EQ(string_stack, string_stack_copy, test_suite, "string_stack");

    std::stack<std::vector<int>> stack_vector_int_copy;
    std::vector<int> temp_v1 = {1, 2, 3, 4};
    std::vector<int> temp_v2 = {5, 6, 7, 8};
    stack_vector_int_copy.emplace(temp_v1) ;
    stack_vector_int_copy.emplace(temp_v2) ;
    TRICK_EXPECT_EQ(stack_vector_int, stack_vector_int_copy, test_suite, "stack_vector_int");

    std::queue<int> int_queue_copy;
    int_queue_copy.push(1) ;
    int_queue_copy.push(2) ;
    int_queue_copy.push(3) ;
    int_queue_copy.push(4) ;
    TRICK_EXPECT_EQ(int_queue, int_queue_copy, test_suite, "int_queue");

    std::queue<std::string> string_queue_copy;
    string_queue_copy.push("I") ;
    string_queue_copy.push("want") ;
    string_queue_copy.push("an") ;
    string_queue_copy.push("iPhone 4") ;
    TRICK_EXPECT_EQ(string_queue, string_queue_copy, test_suite, "string_queue");

    // Why doesn't priority_queue have the == operator but literally everything else does >:(
    std::priority_queue<int> int_priority_queue_copy;
    int_priority_queue_copy.push(3) ;
    int_priority_queue_copy.push(2) ;
    int_priority_queue_copy.push(4) ;
    int_priority_queue_copy.push(1) ;
    compare_data_structures_with_top(int_priority_queue_copy, int_priority_queue, "int_priority_queue");

    std::priority_queue<std::string> string_priority_queue_copy;
    string_priority_queue_copy.push("I") ;
    string_priority_queue_copy.push("want") ;
    string_priority_queue_copy.push("an") ;
    string_priority_queue_copy.push("iPhone 4") ;
    compare_data_structures_with_top(string_priority_queue_copy, string_priority_queue, "string_priority_queue");

    std::queue<std::vector<int>> queue_vector_int_copy;
    queue_vector_int_copy.emplace(temp_v1) ;
    queue_vector_int_copy.emplace(temp_v2) ;
    TRICK_EXPECT_EQ(queue_vector_int, queue_vector_int_copy, test_suite, "queue_vector_int");

    std::priority_queue<std::vector<int>> priority_queue_vector_int_copy;
    priority_queue_vector_int_copy.emplace(temp_v1) ;
    priority_queue_vector_int_copy.emplace(temp_v2) ;
    compare_data_structures_with_top(priority_queue_vector_int_copy, priority_queue_vector_int, "priority_queue_vector_int");

    std::pair<int,int> int_pair_copy(10,20);
    TRICK_EXPECT_EQ(int_pair, int_pair_copy, test_suite, "int_pair");

    std::pair< std::string , int > string_first_pair_copy ("string first", 25);
    TRICK_EXPECT_EQ(string_first_pair, string_first_pair_copy, test_suite, "string_first_pair");

    std::pair< int , std::string > string_second_pair_copy (25, "string second");
    TRICK_EXPECT_EQ(string_second_pair_copy, string_second_pair, test_suite, "string_second_pair");

    std::pair< std::string , std::string > string_pair_copy ("pair first string", "pair second string");
    TRICK_EXPECT_EQ(string_pair_copy, string_pair, test_suite, "string_pair");


    std::pair< int , std::pair< int, int > > int_pair_int_int_copy;
    int_pair_int_int_copy.first = 200 ;
    p.first = 10 ;
    p.second = 20 ;
    int_pair_int_int_copy.second = p ;
    TRICK_EXPECT_EQ(int_pair_int_int_copy, int_pair_int_int, test_suite, "int_pair_int_int");

    std::pair< std::pair< int, int > , int > pair_int_int_int_copy;
    p.first = 15 ;
    p.second = 12 ;
    pair_int_int_int_copy.first = p ;
    pair_int_int_int_copy.second = 180 ;
    TRICK_EXPECT_EQ(pair_int_int_int_copy, pair_int_int_int, test_suite, "pair_int_int_int");

    std::pair< std::pair< int, int > , std::pair< int, int > > pair_pair_pair_copy ;
    pair_pair_pair_copy.first.first = 51 ;
    pair_pair_pair_copy.first.second = 52 ;
    pair_pair_pair_copy.second.first = 53 ;
    pair_pair_pair_copy.second.second = 54 ;
    TRICK_EXPECT_EQ(pair_pair_pair_copy, pair_pair_pair, test_suite, "pair_pair_pair");

    // Check all the int/vec combo pairs together, for laziness
    TRICK_EXPECT_EQ(int_vec_pair.first, 5, test_suite, "int_vec_pair.first");
    TRICK_EXPECT_EQ(vec_int_pair.second, 5, test_suite, "vec_int_pair.second");

    TRICK_EXPECT_EQ(int_vec_pair.second.size(), 4, test_suite, "int_vec_pair.second.size");
    TRICK_EXPECT_EQ(vec_int_pair.first.size(), 4, test_suite, "vec_int_pair.first.size");
    TRICK_EXPECT_EQ(vec_vec_pair.first.size(), 4, test_suite, "vec_vec_pair.first.size");
    TRICK_EXPECT_EQ(vec_vec_pair.second.size(), 4, test_suite, "vec_vec_pair.second.size");

    for (int i = 0; i < int_vec_pair.second.size(); i++) {
        TRICK_EXPECT_EQ(int_vec_pair.second[i], (i+1)*5, test_suite, "int_vec_pair.second elems");
        TRICK_EXPECT_EQ(vec_int_pair.first[i], (i+1)*5, test_suite, "vec_int_pair.first elems");
        TRICK_EXPECT_EQ(vec_vec_pair.first[i], (i+1)*5, test_suite, "vec_vec_pair.first elems");
        TRICK_EXPECT_EQ(vec_vec_pair.second[i], (i+1)*5, test_suite, "vec_vec_pair.second elems");

    }


    TRICK_EXPECT_EQ(vec_user_simple.size(), 10, test_suite, "vec_user_simple")
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 5; j++) {
            TRICK_EXPECT_EQ(vec_user_simple[i].a[j], i+j, test_suite, "vec_user_simple");
            TRICK_EXPECT_EQ(vec_user_simple[i].d->a[j], i+j, test_suite, "vec_user_simple");
        }

        TRICK_EXPECT_EQ(vec_user_simple[i].b, 8888888888, test_suite, "vec_user_simple");
        TRICK_EXPECT_EQ(vec_user_simple[i].d->b, 8888888888, test_suite, "vec_user_simple");

        TRICK_EXPECT_EQ(vec_user_simple[i].c, "Here is a test string", test_suite, "vec_user_simple");
        TRICK_EXPECT_EQ(vec_user_simple[i].d->c, "Here is a test string", test_suite, "vec_user_simple");
    }

    return 0;
    
}
