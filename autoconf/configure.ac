dnl To generate a new top level configure script from this autoconf directory
dnl 1.  aclocal
dnl 2.  autoconf -o ../configure

AC_INIT([Trick],[19.6+])
AC_LANG(C++)

AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIRS([m4])

dnl get the host and build.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

dnl check if we are on a mac
TR_MAC_DARWIN()
dnl add x11 libs to environment
TR_X11()

dnl look for X windows libraries and headers we need to compile
dnl these checks must be done before we force 32bit mode.
TR_XWINDOWS()

dnl TODO Need to check 64bit only!
AC_CHECK_LIB(xml2, main,
    [LIBXML=-lxml2
     AC_SUBST([LIBXML])]
)
AC_CHECK_HEADER([libxml/tree.h],[],AC_MSG_ERROR([could not find libxml development headers]))

# If 32bit is specified, set some compilation flags.
AC_ARG_ENABLE([32bit],
  AS_HELP_STRING([--enable-32bit], [Force Trick and sims to compile 32bit]),
  AS_IF([test "x$enable_32bit" = xyes],
  [
   TRICK_FORCE_32BIT=1
   CFLAGS="${CFLAGS} -m32"
   CXXFLAGS="${CXXFLAGS} -m32"
   LDFLAGS="${LDFLAGS} -m32"
   ARCH="i686"
  ], [TRICK_FORCE_32BIT=0]),
  [TRICK_FORCE_32BIT=0]
)
AC_SUBST([TRICK_FORCE_32BIT])

# If offline is specified, set some compilation flags
AC_ARG_ENABLE([offline],
  AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
  AS_IF([test "x$enable_offline" = xyes],
    [
      [TRICK_OFFLINE="1"]
      AC_CHECK_FILE([trick-offline], [], AC_MSG_ERROR([Offline mode requires an uncompressed directory named "trick-offline" in TRICK_HOME]))
    ], 
    [TRICK_OFFLINE="0"]
  ),
  [TRICK_OFFLINE="0"]
)
AC_SUBST([TRICK_OFFLINE])

# If offline is specified, set some compilation flags
AC_ARG_ENABLE([offline],
  AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
  AS_IF([test "x$enable_offline" = xyes],
    [TRICK_OFFLINE="1"], 
    [TRICK_OFFLINE="0"]
  ),
  [TRICK_OFFLINE="0"]
)
AC_SUBST([TRICK_OFFLINE])

# AC_ARG_ENABLE([civet],
#   AS_HELP_STRING([--enable-civet], [Compile Trick with webserver capabilites.]),
#   AS_IF([test "x$enable_civet" = xyes],
#     [
#     TRICK_DISABLE_CIVET="0"
#     ], 
#     [TRICK_DISABLE_CIVET="1"]
#   ),
#   [TRICK_DISABLE_CIVET="0"]
# )
# AC_SUBST([TRICK_DISABLE_CIVET])

# If offline is specified, set some compilation flags
# AC_ARG_ENABLE([offline],
#   AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
#   AS_IF([test "x$enable_offline" = xyes],
#     [TRICK_DISABLE_CIVET="1"], 
#   ),
# )
# AC_SUBST([TRICK_OFFLINE])

dnl look for programs we need to compile and run
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX

dnl do not test gcc version on mac.  clang as gcc will return version and it's not >= 4.8
AS_IF([test "$ON_MAC" = "no"],[TR_GCC_VERSION],[])
dnl if the compiler is gcc, test for gcc >= 4.8
AS_IF([test "x$GCC_VERSION" = "x"],[],
 [AC_MSG_CHECKING([gcc version >= 4.8])
  AX_COMPARE_VERSION([$GCC_VERSION],[ge],[4.8], [AC_MSG_RESULT([yes])],
   [AC_MSG_RESULT([no])
    AC_MSG_ERROR([Trick requires gcc version >= 4.8])
   ])
])
dnl Save the full path of the compiler
CC=`which $CC`
CXX=`which $CXX`

AC_PATH_PROG(TEE, tee, notee)
AS_IF([test "$ac_cv_path_TEE" = "notee"],AC_MSG_ERROR([could not find tee]),[])
AC_PATH_PROG(LD, ld, nold)
AS_IF([test "$ac_cv_path_LD" = "nold"],AC_MSG_ERROR([could not find ld]),[])
AC_PROG_LEX([noyywrap])
AS_IF([test "x$LEX" = "x:"],AC_MSG_ERROR([could not find flex]),[])
AC_PATH_PROG(BISON, bison, nobison)
AS_IF([test "$ac_cv_path_BISON" = "nobison"],AC_MSG_ERROR([could not find bison]),[])
AC_PATH_PROG(CURL, curl, nocurl)
AS_IF([test "$ac_cv_path_CURL" = "nocurl"],AC_MSG_ERROR([could not find curl]),[])
AC_PATH_PROG(PERL, perl)
AX_PROG_PERL_VERSION([5.14.0], [], [AC_MSG_ERROR([Trick requires perl version >= 5.14.0])])
AC_PATH_PROG(ZIP, zip, nozip)
AS_IF([test "$ac_cv_path_ZIP" = "nozip"],AC_MSG_ERROR([Trick now requires zip but it could not be found. Please install zip. We recommend you use your OS package manager]),[])


dnl add PYTHON_VERSION to influential env variables
AC_ARG_VAR([PYTHON_VERSION],[Adds suffix to python search e.g. PYTHON_VERSION=3 will look for python3])

dnl find python
AC_ARG_WITH(
  [python],
  AS_HELP_STRING(
    [--with-python@<:@=DIR@:>@], 
    [python root directory],
  ),
  [
    PYTHON_HOME="$withval"
    AC_PATH_PROGS(PYTHON, python${PYTHON_VERSION} python python3, nopython, "$PYTHON_HOME")
    AS_IF(
      [test "$ac_cv_path_PYTHON" = "" || test "$ac_cv_path_PYTHON" = "nopython"],
      AC_MSG_ERROR([could not find python<version> python or python3 in the specefied path. Please install the python development package]),
      []
    )
    PYTHON_MAJORMINOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]])+"."+str(sys.version_info[[1]]))'`
    PYTHON_MAJOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]]))'`
    AC_PATH_PROGS(PYTHON_CONFIG, python${PYTHON_MAJORMINOR}-config python${PYTHON_MAJOR}-config python${PYTHON_VERSION}-config python-config, nopython-config, "$PYTHON_HOME")
    AS_IF(
      [test "$ac_cv_path_PYTHON_CONFIG" = "" || test "$ac_cv_path_PYTHON_CONFIG" = "nopython-config"],
      AC_MSG_ERROR([could not find python<major>-config python<major.minor>-config or python-config in the specified path. Please install the python development package]),
      []
    )
  ],
  [
    AC_PATH_PROGS(PYTHON, python${PYTHON_VERSION} python python3, nopython)
    AS_IF(
      [test {"$ac_cv_path_PYTHON" = "" || test "${ac_cv_path_PYTHON}" = "nopython"}],
      AC_MSG_ERROR([could not find python<version> python or python3. Please install the python development package]),
      []
    )
    PYTHON_MAJORMINOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]])+"."+str(sys.version_info[[1]]))'`
    PYTHON_MAJOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]]))'`
    AC_PATH_PROGS(PYTHON_CONFIG, python${PYTHON_MAJORMINOR}-config python${PYTHON_MAJOR}-config python${PYTHON_VERSION}-config python-config, nopython-config)
    AS_IF(
      [test "$ac_cv_path_PYTHON_CONFIG" = "" || test "$ac_cv_path_PYTHON_CONFIG" = "nopython-config"],
      AC_MSG_ERROR([could not find python<major>-config python<major.minor>-config or python-config. Please install the python development package]),
      []
    )  
  ]
)

PYTHON_LIBS_COMMAND="${PYTHON_CONFIG} --ldflags --libs"

AX_COMPARE_VERSION(
  [${PYTHON_MAJORMINOR}],
  [ge],
  [3.8],
  [PYTHON_LIBS_COMMAND="${PYTHON_LIBS_COMMAND} --embed"]
)

PYTHON_CPPFLAGS=`${PYTHON_CONFIG} --includes | sed 's/-I/-isystem/g'`
PYTHON_LIBS=`${PYTHON_LIBS_COMMAND} | tr '\r\n' ' '`


AC_SUBST([PYTHON_CPPFLAGS])
AC_SUBST([PYTHON_LIBS])
AC_SUBST([PYTHON_EXTRA_LIBS])
TR_PREPEND_PATH()
AC_PATH_PROG(GNUPLOT, gnuplot, nognuplot)
AS_IF([test "$ac_cv_path_GNUPLOT" = "nognuplot"],AC_MSG_NOTICE([could not find gnuplot]),[])
TR_SWIG_BIN([3.0])
TR_JAVA

AX_PTHREAD()
AX_PROG_PERL_MODULES( Text::Balanced ,[],AC_MSG_ERROR([could not find perl modules Text::Balanced]))
AX_PROG_PERL_MODULES( Digest::MD5,[],AC_MSG_ERROR([could not find perl module Digest::MD5]))

dnl process the optional --with-llvm command line arguments
TR_LLVM_HOME([])
LLVM_LIB_DIR=`$LLVM_CONFIG --libdir`
LLVM_BIN_DIR=`$LLVM_CONFIG --bindir`
LLVM_INCLUDE_DIR=`$LLVM_CONFIG --includedir`
AC_CHECK_FILE([$LLVM_INCLUDE_DIR/clang/Basic/LLVM.h],[],AC_MSG_ERROR([could not find clang development headers]))
AC_CHECK_FILE([$LLVM_LIB_DIR/libLLVMSupport.a],
  [],
  AC_MSG_ERROR([could not find clang library files in $LLVM_LIB_DIR])
)
AC_PATH_PROG(CLANG, clang, noclang, "$LLVM_BIN_DIR:/bin:/usr/bin:/usr/local/bin:/sw/bin")
AS_IF([test "$ac_cv_path_CLANG" = "noclang"],AC_MSG_ERROR([could not find clang]),[])
AC_SUBST([LLVM_HOME])

TR_CLANG_VERSION
dnl if llvm/clang, test for version >= 3.4.2
AS_IF([test "x$CLANG_VERSION" = "x"],[],
 [AC_MSG_CHECKING([clang version >= 3.4.2])
  AX_COMPARE_VERSION([$CLANG_VERSION],[ge],[3.4.2], [AC_MSG_RESULT([yes])],
   [AC_MSG_RESULT([no])
    AC_MSG_ERROR([Trick requires llvm/clang version >= 3.4.2])
   ])
])

AS_IF([test "x$CLANG_VERSION" = "x"],[],
  [AX_COMPARE_VERSION([$CLANG_VERSION],[ge],[18.0.0], 
   [OLD_CLANG_LIBS="-lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangASTMatchers -lclangAPINotes -lclangLex -lclangBasic"],
   [OLD_CLANG_LIBS="-lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangLex -lclangBasic"]
   )]
)

NEW_CLANG_LIBS="-lclang-cpp"
AC_CHECK_FILE([$LLVM_LIB_DIR/libclangFrontend.a],
  [
    ICG_CLANGLIBS="$OLD_CLANG_LIBS"
  ],
  AC_CHECK_FILE([$LLVM_LIB_DIR/libclangFrontend.so],
    [
      ICG_CLANGLIBS="$OLD_CLANG_LIBS"
    ],
    AC_CHECK_FILE([$LLVM_LIB_DIR/libclang-cpp.a],
      [
        ICG_CLANGLIBS="$NEW_CLANG_LIBS"
      ],
      AC_CHECK_FILE([$LLVM_LIB_DIR/libclang-cpp.so],
        [
          ICG_CLANGLIBS="$NEW_CLANG_LIBS"
        ],
        AC_MSG_ERROR([could not find clang libs in LLVM library: "$LLVM_LIB_DIR"])
      )
    )
  )
)

AC_CHECK_FILE([$LLVM_LIB_DIR/libclangSupport.a],[ICG_CLANGLIBS="$ICG_CLANGLIBS -lclangSupport"],[])

AC_SUBST([ICG_CLANGLIBS])

AX_CHECK_ZLIB([],AC_MSG_ERROR([could not find zlib]))

dnl look for udunits in /usr/include and /usr/include/udunits2
TR_UDUNITS_HOME([])
AS_IF([test "$UDUNITS_HOME" = ""],
 [
  AC_CHECK_HEADER([udunits2.h],
   [UDUNITS_INCLUDES=],
   [AC_CHECK_HEADER([udunits2/udunits2.h],[UDUNITS_INCLUDES=-I/usr/include/udunits2],AC_MSG_ERROR([could not find udunits development headers]))]
  )
  UDUNITS_LDFLAGS=-ludunits2
 ],
 [
  UDUNITS_EXCLUDE=$UDUNITS_HOME
  AC_CHECK_FILE([$UDUNITS_HOME/include/udunits2.h],
   [UDUNITS_INCLUDES=-I$UDUNITS_HOME/include
    UDUNITS_LDFLAGS="-Wl,-rpath,$UDUNITS_HOME/lib -L$UDUNITS_HOME/lib -ludunits2"
   ],
   [AC_CHECK_FILE([$UDUNITS_HOME/lib/udunits2.h],
    [UDUNITS_INCLUDES=-I$UDUNITS_HOME/lib
     UDUNITS_LDFLAGS="-Wl,-rpath,$UDUNITS_HOME/lib -L$UDUNITS_HOME/lib -ludunits2"
    ],
    AC_MSG_ERROR([could not find udunits2]))
   ]
  )
  CFLAGS="${CFLAGS} ${UDUNITS_INCLUDES}"
  LDFLAGS="${LDFLAGS} ${UDUNITS_LDFLAGS}"
 ]
)
AC_CHECK_LIB(udunits2, main, [],AC_MSG_ERROR([could not find libudunits]))
AC_SUBST([UDUNITS_INCLUDES])
AC_SUBST([UDUNITS_LDFLAGS])
AC_SUBST([UDUNITS_EXCLUDE])

dnl process the other optional command line arguments
TR_HDF5_HOME([])
TR_GSL_HOME([])
TR_GTEST_HOME([])
TR_CIVETWEB_HOME([])
TR_ER7_UTILS([])

AC_CONFIG_FILES([share/trick/makefiles/config_user.mk])
AC_OUTPUT

printf "\033@<:@32mconfigure script successfully completed\033@<:@0m\n"

