package parse_s_define ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(parse_s_define handle_sim_object handle_integ_loop handle_collects
              handle_user_code handle_user_header handle_user_inline) ;

use Cwd ;
use File::Basename ;
use strict ;
use Cwd 'abs_path';
use IPC::Open3 ;

use edit ;
use find_module ;
use gte ;
use trick_print ;
use trick_version ;
use Text::Balanced qw(extract_bracketed);
use html ;
use get_paths ;

my ($integ_loop_def , $collect_def , $vcollect_def);
my ($job_class_order_def ) ;
my ($sim_class_def , $sim_class_job_def , $instantiation_def , $create_connections_def) ;
my ($compiler_directive_def ) ;
my ($other_tag_def , $comment_def ) ;
my ($user_code_def ) ;
my ($user_header_def ) ;
my ($user_inline_def ) ;
my ($line_tag_def) ;
my $s_define_file ;

#------------------------------------------------------------
# IntegLoop statements

$integ_loop_def = qr/
         (?:IntegLoop|integrate)           # IntegLoop keyword
         \s+
         (?:[CcSs][\d]*\s+)?               # child spec
         \s*
         (?:[\w_]+)                        # IntegLoopSimObject name
         \s*
         \(                                # Entry timing spec
           \s*
           (?:
             (?:[\d\.]+)                   #    cycle
            |(?:[\d\.]+\s*,\s*[\d\.]+)     # OR cycle,cycle
            |(?:[\w_\.]+)                  # OR class
            |(?:[\d\.]+\s*,\s*[\w_\.]+)    # OR float,class
           )
           \s*
         \)                                # close timing spec
         \s*
        (?:.*?)                            # name list
         \s*;                              # terminating semicolon
       /sx ;

#------------------------------------------------------------
# Collect statements
$collect_def = qr/
         collect                  # Collect keyword
         \s*
         (?:[\w_\.\[\]]*)         # Collect name
         \s*
         =                        # Equals sign
         \s*
         {                        # entry
         (?:.*?)                    # collect params
         }\s*;                    # end args
         /sx ;

#------------------------------------------------------------
# Vector collect statements
$vcollect_def = qr/
        vcollect                   # Vcollect keyword
        \s*
        (?:[\w_\.\[\]]+)           # Container name
        \s*
        (?:.*?)                    # Optional constructor
        \s*
        \{\s*                       # entry
        (?:.*?)                    # item list
        \s*\}\s*;                   # end args
        /sx ;

$compiler_directive_def = qr/
         \#\#[^\n]+                # compiler directive
         /sx ;

$sim_class_def = qr/
         (?:template\s+<[^>]+>\s*)?
         class                    # the keyword class
         [^{]+                    # everything up to opening parenthesis
         /sx ;

$sim_class_job_def = qr/
         (?:
           \s*(?:
             (?:[Cc][\w\.\-\>]+) |                       # child spec
             (?:[Pp][\w\.\-\>]+) |                       # phase spec
             (?:
              \(
                (?:
                   (?:
                      \s*(?:[\w.]+)\s*           # cycle time
                      (?:,\s*(?:[\w.]+)\s*)?     # start time
                      (?:,\s*(?:[\w.]+)\s*)?     # stop time
                      (?:,\s*(?:"?\w+"?)\s*)     # class
                   )|
                   \s*(?:"?\w+"?)\s*             # class (by itself)
                   (?:,\s*(&?[\w\.\-\>]+)\s*)?   # integration object
                )
              \)
             ) |                                 # timing spec
             \{(?:[\w_.,\s]+)\}                     # job tag
           )
         )+\s+
         (?:[A-Za-z_][\w\.\-\>]*\s*=)?\s*        # optional return assignment variable
         (?:[A-Za-z_]+[\w\.\:\-\>\[\]]*)         # job name
           \s*
          \(                                     # entry point
            (?:.*?)\s*                           # arg list
          \)\s*;                                 # end arg list
         /sx ;

$instantiation_def = qr/
         (?:[A-Za-z_]+[\w_:\*]*(?:<[^>]+>)?)\s+  # type
         (?:[A-Za-z_]+[\w_]*)\s*                 # name
         (?:\(.*?;|\s*;)                         # arguments
         /sx ;
         #(?:\([^{\)]*\))?\s*;                      # arguments

$create_connections_def = qr/
         void\s+create_connections               # create_connections declaration
         [^{]+                                   # everything up to opening parenthesis
         /sx ;

$job_class_order_def = qr/
         job_class_order          # the word sim_object
         (?:.*?                   # everything
         })\s*;                   # to end of obj def
        /sx ;

$user_code_def = qr/
          %\{\s*
          .*?
          %\}
         /sx ;

$user_header_def = qr/
          %\s*header\s*{\s*
          .*?
          %}
         /sx ;

$user_inline_def = qr/
          %\s*inline\s*{\s*
          .*?
          %}
         /sx ;

$other_tag_def = qr/\#(?:ident|pragma)?.*?\n/s ;
$comment_def = qr/ZZZYYYXXX\d+ZZZYYYXXX/s ;
$line_tag_def = qr/\#(?:line)?\s+\d+.*?(?:\n|$)/s ;

sub parse_s_define ($) {

    my ($sim_ref) = @_ ;

    my ($temp) ;
    my ($CC, $contents) ;
    my (@prescan_job_class_order) ;
    my ($version, $thread, $year) ;
    my @comments ;

    my @preprocess_output;

    @{$$sim_ref{inc_paths}} = (get_include_paths(), $ENV{TRICK_SYSTEM_CFLAGS} =~ /-(?:I|isystem)(\S+)/g, "$ENV{TRICK_HOME}/trick_source" , "../include") ;

    my @valid_inc_paths ;
    foreach (@{$$sim_ref{inc_paths}}) {

        push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @{$$sim_ref{inc_paths}} = @valid_inc_paths ;

    foreach ( @valid_inc_paths ) {
        $_ = abs_path($_) ;
    }

    foreach ( @{$$sim_ref{inc_paths}} ) {
        s/\/+$// ;
        s/\/+/\// ;
        $_ = quotemeta (abs_path(dirname($_)) . "/" . basename($_)) ;
    }

    $s_define_file = "S_define" ;

    $$sim_ref{line_num} = 1 ;

    # Run S_define through C PreProcessor to handle #defines
    $CC = gte("TRICK_CC") ;
    chomp $CC ;

    if (-e $s_define_file) {
        my $cmd;

        $cmd = "$CC -C -E -xc $ENV{TRICK_SFLAGS} $ENV{TRICK_SYSTEM_SFLAGS} $s_define_file";
        #print "$cmd\n" ;

        @preprocess_output = `$cmd`;

# The commented out code is what I would like to use, but it hangs on Macs (Alex 6/13/11)
#        my($wtr, $rdr, $err);
#        my($pid , @error_msg);
#        use Symbol 'gensym'; $err = gensym;
#        $pid = open3($wtr, $rdr, $err, $cmd);
#
#        waitpid( $pid , 0 ) ;
#        @preprocess_output = <$rdr> ;

        if (($? >> 8) != 0) {
            trick_print( $$sim_ref{fh}, "\nError in $s_define_file. Exit!\n\n", "title_red", $$sim_ref{args}{v} );
#            my(@error_msg);
#            @error_msg = <$err> ;
#            trick_print( $$sim_ref{fh}, "@error_msg\n\n", "normal_white", $$sim_ref{args}{v} );
            exit -1 ;
        }

    } else {
        die "Couldn't find file: $s_define_file\n";
    }

    foreach my $each_item (@preprocess_output) {
        $contents .= $each_item;
    }

    @comments = $contents =~ m/((?:\/\*(?:.*?)\*\/)|(?:\/\/(?:.*?)\n))/sg ;

    foreach my $i (@comments) {
        my %header ;
        my @lib_list ;
        %header = extract_trick_header("S_define", $i, 0, 0);
        push @{$$sim_ref{default_data}} , $header{default_data} ;
        $header{libdep} =~ s/\s+//sg ;
        $header{libdep} =~ s/\(\(/\(/ ;
        $header{libdep} =~ s/\)\)/\)/ ;
        @lib_list = $header{libdep} =~  m/\((.+?)\)/sg ;
        # Append any library deps in doxygen style comments.
        push @lib_list , @{$header{liblist}} if ( exists $header{liblist} ) ;
        foreach my $object_file (@lib_list) {
            #print "[33m look for object $object_file[00m\n" ;
            if ( $object_file =~ /^\// ) {
                push @{$$sim_ref{mis_entry_files}}, $object_file ;
            } else {
                my $found = 0 ;
                foreach my $inc_path ( @valid_inc_paths ) {
                    if ( -f "$inc_path/$object_file" ) {
                        push @{$$sim_ref{mis_entry_files}}, "$inc_path/$object_file" ;
                        $found = 1 ;
                        last ;
                    }
                }
                if ( $found == 0 ) {
                    trick_formatted_print($$sim_ref{fh}, "[1m[33m", "Warning    ", "[39m", "S_define\n", "[0m", "           Could not find dependency \"", "[1m", "$object_file", "[0m", "\"\n");
                }
            }
        }
    }
    my $i = 0 ;
    $contents =~ s/\/\*(.*?)\*\/|\/\/(.*?)(\n)/"ZZZYYYXXX" . $i++ . "ZZZYYYXXX" . ((defined $3) ? $3 : "")/esg ;

    # substitue in environment variables in the S_define file.
    # Do that with 1 line in C!  This comment is longer than the code it took!
    $contents =~ s/\$[({]\s*([\w_]+)\s*[)}]/$ENV{$1}/eg ;

    #strip trailing spaces
    $contents =~ s/\s*$// ;
    #rip out define comments left by gcc 2.96
    $contents =~ s/\/\*.*?\*+\///sg ;

    $$sim_ref{sim_class_index}{"Trick::SimObject"} = 0 ;

    # set the default job class order
    @{$$sim_ref{user_class_order}} = qw(automatic random environment sensor sensor_emitter
                                        sensor_reflector sensor_receiver scheduled effector
                                        effector_emitter effector_receiver automatic_last logging
                                        data_record system_checkpoint system_advance_sim_time
                                        system_moding integ_loop) ;
    # search and use a user class order
    @prescan_job_class_order = $contents =~ m/($job_class_order_def)/sg ;
    preparse_job_class_order( \@prescan_job_class_order , $sim_ref ) ;

    while ($contents =~ s/^(\s*)(?:($sim_class_def)|
                                   ($integ_loop_def)|
                                   ($collect_def)|
                                   ($user_code_def) |
                                   ($user_header_def) |
                                   ($user_inline_def) |
                                   ($line_tag_def)|
                                   ($job_class_order_def)|
                                   ($vcollect_def)|
                                   ($create_connections_def)|
                                   ($instantiation_def)|
                                   ($compiler_directive_def)|
                                   ($other_tag_def)|
                                   ($comment_def)
                                )//sx ) {
        if ( defined $1 and $1 ne "" )  {
            $temp = $1 ;
            #trick_print($$sim_ref{fh}, $temp, "debug_white" , $$sim_ref{args}{v});
            $$sim_ref{line_num} += ($temp =~ s/\n/\n/g) ;
        }
        if ( defined $2 and $2 ne "" )  { handle_sim_class($2, \$contents, $sim_ref, \@comments) ; }
        if ( defined $3 and $3 ne "" )  { handle_integ_loop($3, $sim_ref) ; }
        if ( defined $4 and $4 ne "" )  { handle_collects($4, $sim_ref) ; }
        if ( defined $5 and $5 ne "" )  { handle_user_code($5, $sim_ref) ; }
        if ( defined $6 and $6 ne "" )  { handle_user_header($6, $sim_ref) ; }
        if ( defined $7 and $7 ne "" )  { handle_user_inline($7, $sim_ref) ; }
        if ( defined $8 and $8 ne "" )  { handle_line_tag($8, $sim_ref) ; } #line tag
        if ( defined $9 and $9 ne "" )  { } #job class order... preparsed
        if ( defined $10 and $10 ne "" )  { handle_vcollects($10, $sim_ref) ; }
        if ( defined $11 and $11 ne "" )  { handle_create_connections($11, \$contents, $sim_ref) ; }
        if ( defined $12 and $12 ne "" )  { handle_instantiation($12, $sim_ref) ; }
        if ( defined $13 and $13 ne "" )  { handle_compiler_directive($13, $sim_ref) ; }
        if ( defined $14 and $14 ne "" )  { } #ignore these lines
    }

    $contents =~ s/\cZ$//sx ;
    $contents =~ s/^(\s*)//sx ;
    $$sim_ref{line_num} += (($temp = $1) =~ s/\n/\n/g) ;

    if ( $contents ne "" ) {
        trick_print($$sim_ref{fh},"Syntax error in $$sim_ref{last_file}\n", "title_red", $$sim_ref{args}{v}) ;
        my @temp_array = split /\n/ , $contents ;
        my $temp = join "\n" , (splice ( @temp_array , 0 , 10 )) ;
        trick_print($$sim_ref{fh},"Last 10 lines read:\n$temp\n", "title_red", $$sim_ref{args}{v}) ;
        edit_and_exit("CP" , "$s_define_file" , $$sim_ref{line_num} ) ;
    }

}

# Handle Integration Loop Statements
sub handle_integ_loop ($$) {

    my ($integ_loop, $sim_ref) = @_ ;
    my ($child_spec, $name, $cycle, $name_list ) ;
    my %integ_loop_info ;

    trick_print($$sim_ref{fh}, "IntegLoop: $integ_loop\n" , "debug_white" , $$sim_ref{args}{v});

    # remove all comments
    $integ_loop =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;

    ( $child_spec, $name, $cycle, $name_list ) = $integ_loop =~ /
           (?:IntegLoop|integrate)            # IntegLoop keyword
           \s+
           (?:[CcSs]([\d])*\s+)?             # child spec
           ([\w_]+)                          # IntegLoopSimObject name
           \s*
           \(                                # Entry timing spec
           \s*
           ([\d\.]+)                         # cycle
           \s*
           \)                                # close timing spec
           \s*
           (.*?)                             # name list
           \s*;                              # terminating semicolon
           /sx ;

    $integ_loop_info{name} = $name ;
    $integ_loop_info{cycle} = $cycle ;
    $child_spec = 0 if ( $child_spec eq "" ) ;
    $integ_loop_info{child} = $child_spec ;
#    @{$integ_loop_info{name_list}} = split(/\s*,\s*/, $name_list );
    $integ_loop_info{name_list} = $name_list ;
    $integ_loop_info{line_num} = $$sim_ref{line_num} ;

    push @{$$sim_ref{integ_loop}}, \%integ_loop_info ;

    $$sim_ref{line_num} += ($integ_loop =~ s/\n/\n/g) ;

}

# Handle Collect Statements
sub handle_collects ($$) {

    my ( $collect , $sim_ref ) = @_ ;
    my ( $collect_name, $collect_params ) ;
    my %collect_info ;

    trick_print($$sim_ref{fh}, "Collect: $collect\n" , "debug_white" , $$sim_ref{args}{v});
    ( $collect_name, $collect_params ) = $collect =~ /
           collect                  # Collect keyword
           \s*
           ([\w_\.\[\]]*)           # Collect name
           \s*
           =                        # Equals sign
           \s*
           {\s*                     # entry
           (.*?)                    # collect params
           }\s*;                    # end args
           /sx ;
    # Setup collect params for a split (take out newlines and lead/trail space)
    $collect_params =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;
    $collect_params =~ s,\n|^\s*|\s*$,,g ;

    $collect_info{name} = $collect_name ;
    @{$collect_info{params}} = split(/\s*,\s*/ , $collect_params ) ;
    $collect_info{num_params} = $#{$collect_info{params}} + 1 ;

    push @{$$sim_ref{collect}} , \%collect_info ;


    $$sim_ref{line_num} += ($collect =~ s/\n/\n/g) ;
}

# Handle Vcollect Statements
sub handle_vcollects ($$) {

    my ( $vcollect , $sim_ref ) = @_ ;
    my ( $container_name, $constructor, $item_list );
    my %collect_info ;

    trick_print($$sim_ref{fh}, "Vcollect: $vcollect\n" , "debug_white" , $$sim_ref{args}{v}) ;

    $vcollect =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;

    ( $container_name, $constructor, $item_list ) = $vcollect =~ /
            vcollect                    # Vcollect keyword
            \s*
            ([\w_\.\[\]]+)              # Container name
            \s*
            (.*?)                       # Optional constructor
            \s*
            {\s*                        # entry
            (.*?)                       # item list
            \s*}\s*                     # end args
            /sx ;
    # Setup collect params for a split (take out newlines and lead/trail spaces)
    $item_list =~ s,\n|^\s*|\s*$,,g ;

    $collect_info{name} = $container_name ;
    $collect_info{constructor} = $constructor ;
    @{$collect_info{items}} = split(/\s*, \s*/ , $item_list) ;

    push @{$$sim_ref{vcollect}} , \%collect_info ;

    $$sim_ref{line_num} += ($vcollect =~ s/\n/\n/g) ;

}

# Handle User code on Statements
sub handle_user_code ($$) {

    my ($u, $sim_ref) = @_ ;

    trick_print($$sim_ref{fh}, "User code: $u\n" , "debug_white" , $$sim_ref{args}{v});
    $$sim_ref{line_num} += ($u =~ s/\n/\n/g) ;
    $u =~ s/^##/#/mg ;
    $u =~ /%\{(.*?)%\}/s ;
    $u = $1 ;
    $u =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;
    $$sim_ref{user_code} .= $u ;
}

sub handle_user_header ($$) {

    my ($u, $sim_ref) = @_ ;

    trick_print($$sim_ref{fh}, "User header: $u\n" , "debug_white" , $$sim_ref{args}{v});

    $$sim_ref{line_num} += ($u =~ s/\n/\n/g) ;
    $u =~ s/^##/#/mg ;
    $u =~ /%\s*header\s*{(.*?)%}/s ;
    $u = $1 ;
    $u =~ s/^#\s+(\d+)\s+"S_define"\n//mg ;
    $u =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//esg ;
    $$sim_ref{user_header} .= $u ;
}

sub handle_user_inline ($$) {

    my ($u, $sim_ref) = @_ ;

    trick_print($$sim_ref{fh}, "Inline code: $u\n" , "debug_white" , $$sim_ref{args}{v});

    $$sim_ref{line_num} += ($u =~ s/\n/\n/g) ;
    $u =~ s/^##/#/mg ;
    $u =~ /%\s*inline\s*{(.*?)%}/s ;
    $u = $1 ;
    $u =~ s/^#\s+(\d+)\s+"S_define"\n//mg ;
    $u =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//esg ;
    $$sim_ref{sim_class_code} .= $u ;
    $$sim_ref{sim_class_code} .= "\n" ;
}

sub handle_sim_class ($$$$) {

    my ($s, $file_contents, $sim_ref, $comments_ref) = @_;
    my ($full_template_args, $template_args) ;
    my ($class_contents , $constructor_contents) ;
    my ($class_name , $base_clause) ;
    my $final_contents ;
    my $int_call_functions ;
    my $double_call_functions ;
    my $constructor_found = 0 ;
    my $job ;
    #my ($start_index, $ii) ;
    my ($constructor_declare, $temp_content) ;
    my ($job_push , $job_call, $is_dynamic_event) ;

    # remove any comments on the class definition line
    $s =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//esg ;

    # grab the class name and the name of the class we are inheriting from
    ($full_template_args, $class_name, $base_clause) = $s =~ /(?:\s*template\s*<\s*([^>]+?)\s*>)?\s*class\s+(\w+)\s*(?::\s*(.+?)\s*)?$/ ;

    $template_args = $full_template_args ;
    $template_args =~ s/class|typename//g ;
    $template_args =~ s/\s//g ;

    trick_print($$sim_ref{fh}, "Processing " , "normal_blue" , $$sim_ref{args}{v});
    trick_print($$sim_ref{fh}, "$class_name\n" , "normal_white" , $$sim_ref{args}{v});

    # grab the entire contents of the class out of the S_define file.
    ($class_contents, $$file_contents) = extract_bracketed($$file_contents,"{}");
    $class_contents =~ s/$line_tag_def//g ;
    $$file_contents =~ s/^\s*;\s*//s ;

    #final_contents contains the processed class.  Start if off with the incoming class name
    $final_contents = $s ;

    # remove all whitespace from the base-clause to simplify regex patterns
    $base_clause =~ s/\s//g;

    # This is the full regex for matching (I hope) any valid string following the ':' in a class
    # declaration. (?1) is the syntax for recursive matching, which handles arbitrarily deeply
    # nested template brackets. There are only two capture groups (the other groups are non-
    # capturing). Because a class can inherit from any number of parents, the second group is
    # repeated wth the * operator. Unfortunately, only the final capture is retained (until
    # Perl 6), so we can't actually use this regex to capture all the parents. We could use it
    # to validate the string, but the compiler is going to do that anyway.
    #my @parents = $base_clause =~ /^(?:private|protected|public|virtual)*([\w:]+(?:<(?:[^<>]|(?1))*>)?)(?:,(?:private|protected|public)?((?1)))*$/;

    # This regex is less strict and will accept some invalid strings, but it will capture all
    # of the parents. Since the compiler will eventually flag any syntax errors, I think it's
    # ok if we let them pass here.
    my @parents = $base_clause =~ /\G,?(?:private|protected|public|virtual)*([\w:]+(?:<(?:[^<>]|(?1))*>)?)/g;

    my $sim_object_parent;
    foreach (@parents) {
        # remove template arguments
        ($_) = $_ =~ /([^<]+)/;
        if (exists $$sim_ref{sim_class_index}{$_}) {
            $sim_object_parent = $_;
            last;
        }
    }

    # if this class is not a SimObject pass it whole to S_Source.cpp
    if ( !$sim_object_parent ) {
        $class_contents =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX/@$comments_ref[$1]/g ;
        $final_contents .= "$class_contents ;\n\n" ;
        $$sim_ref{sim_class_code} .= $final_contents ;
        return ;
    }

    if ( $full_template_args eq "" ) {
        $int_call_functions = "int ${class_name}" ;
    } else {
        $int_call_functions = "template <$full_template_args> int $class_name<$template_args>" ;
    }
    $int_call_functions .= "::call_function ( Trick::JobData * curr_job ) {\n\n    int trick_ret = 0 ;\n" ;
    $int_call_functions .= "    if ( curr_job->disabled ) return (trick_ret) ;\n\n" ;
    $int_call_functions .= "    switch ( curr_job->id ) {\n" ;

    if ( $full_template_args eq "" ) {
        $double_call_functions = "double ${class_name}" ;
    } else {
        $double_call_functions = "template <$full_template_args> double $class_name<$template_args>" ;
    }
    $double_call_functions .= "::call_function_double ( Trick::JobData * curr_job ) {\n\n" ;
    $double_call_functions .= "    double trick_ret = 0.0 ;\n" ;
    $double_call_functions .= "    if ( curr_job->disabled ) return (trick_ret) ;\n\n" ;
    $double_call_functions .= "    switch ( curr_job->id ) {\n" ;

    $$sim_ref{sim_class_index}{$class_name} = $$sim_ref{sim_class_index}{$sim_object_parent} ;

    # look for constructor
    while ( $class_contents =~ /^(.*?)$class_name\s*\([^;]*{/s ) {
        my (@int_job_calls, @double_job_calls) ;
        $constructor_found = 1 ;
        # grab the constructor's argument list
        $class_contents =~ s/^(.*?$class_name[^(]*)//s ;
        $temp_content = $1 ;
        $final_contents .= $temp_content ;
        ($temp_content, $class_contents) = extract_bracketed($class_contents,"()");
        $final_contents .= $temp_content ;
        # a colon after the constructor arguments starts a member initializer list
        if ( $class_contents =~ /^\s*:/s )
        {
            my $in_init_list = 1 ;
            while ( $in_init_list ) {
                $class_contents =~ s/^([^{(]+)//s ;
                $temp_content = $1 ;
                $final_contents .= $temp_content ;
                # member initializers can have either parentheses or curly braces
                ($temp_content, $class_contents) = extract_bracketed($class_contents,"(){}");
                $final_contents .= $temp_content ;
                # there's another initializer if there is a comma
                if ( $class_contents !~ /^\s*,/ ) {
                    $in_init_list = 0;
                }
            }
        }

        $class_contents =~ s/^([^{]*)//s ;
        $temp_content = $1 ;
        $final_contents .= $temp_content ;

        ($constructor_contents, $class_contents) = extract_bracketed($class_contents,"{}");

        $constructor_contents =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;
        trick_print($$sim_ref{fh}, "  Constructor found\n" , "debug_white" , $$sim_ref{args}{v});

        $constructor_contents =~ s/\{(\s+)/\{\n            Trick::JobData * job __attribute__((unused)) ;\n/ ;
        #$constructor_contents =~ s/\{(\s+)/\{$1int ii __attribute__ ((unused)) = $$sim_ref{sim_class_index}{$class_name} ;\n/ ;
        while ($constructor_contents =~ s/^(.*?)($sim_class_job_def)//s ) {
            $final_contents .= $1 ;
            $job = $2 ;
            # check to see if what we matched is actually a placement new statement.
            if ( $final_contents !~ /new\s*$/ ) {
                # Not a placement new statement.
                trick_print($$sim_ref{fh}, "    Job found $job\n" , "debug_white" , $$sim_ref{args}{v});
                ($job_push , $job_call , $is_dynamic_event) = handle_sim_class_job($job, $$sim_ref{sim_class_index}{$class_name}, $sim_ref ) ;
                $final_contents .= "\n            $job_push" ;
                if ( $is_dynamic_event == 1 ) {
                    push @double_job_calls , $job_call ;
                    $double_call_functions .= "        case $$sim_ref{sim_class_index}{$class_name}:\n            trick_ret = $job_call ;\n            break ;\n" ;
                } else {
                    push @int_job_calls , $job_call ;
                    $int_call_functions .= "        case $$sim_ref{sim_class_index}{$class_name}:\n            $job_call ;\n            break ;\n" ;
                }
                $$sim_ref{sim_class_index}{$class_name}++ ;
            } else {
                # Is a placement new statement.  Just copy contents to final_contents.
                $final_contents .= $job ;
            }
        }
        $final_contents .= $constructor_contents ;
    }

    if ( $constructor_found == 1 ) {

        # if there is an inherited base class then the job id may reside in the base class
        if ( $sim_object_parent eq "Trick::SimObject" or $sim_object_parent eq "SimObject" ) {
            $int_call_functions .= "        default:\n            trick_ret = -1 ;\n            break ;\n" ;
            $double_call_functions .= "        default:\n            trick_ret = 0.0 ;\n            break ;\n" ;
        } else {
            $int_call_functions .= "        default:\n            trick_ret = ${sim_object_parent}::call_function( curr_job ) ;\n            break ;\n" ;
            $double_call_functions .= "        default:\n            trick_ret = ${sim_object_parent}::call_function_double( curr_job ) ;\n            break ;\n" ;
        }

        $int_call_functions .= "    }\n\n    return(trick_ret) ;\n}\n\n" ;
        $double_call_functions .= "    }\n\n    return(trick_ret) ;\n}\n\n" ;

        #TODO: This section should probably go into s_source.pm
        $final_contents .= "\n\n    public:\n" ;
        $final_contents .= "        virtual int call_function( Trick::JobData * curr_job ) ;\n" ;
        $final_contents .= "        virtual double call_function_double( Trick::JobData * curr_job ) ;\n" ;
        $final_contents .= "$class_contents ;\n\n" ;

        #print "[32m$final_contents[00m\n" ;
        $final_contents =~ s/^##/#/mg ;

        #$contents =~ s/\/\*(.*?)\*\/|\/\/(.*?)(\n)/"" . $i++ . " " . ((defined $3) ? $3 : "")/esg ;
        $final_contents =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX/@$comments_ref[$1]/esg ;

        $$sim_ref{sim_class_code} .= $final_contents ;
        $$sim_ref{sim_class_call_functions} .= $int_call_functions . $double_call_functions ;
    } else {
        $s =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX/@$comments_ref[$1]/esg ;
        $$sim_ref{sim_class_code} .= $s ;
        $class_contents =~ s/}\s*$// ;
        $class_contents .= "\n\n    public:\n" ;
        $class_contents .= "        virtual int call_function( Trick::JobData * curr_job ) ;\n" ;
        $class_contents .= "        virtual double call_function_double( Trick::JobData * curr_job ) ;\n" ;
        $class_contents .= "} ;\n\n" ;
        $class_contents =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX/@$comments_ref[$1]/esg ;
        $$sim_ref{sim_class_code} .= $class_contents ;

        if ( $full_template_args eq "" ) {
            $int_call_functions = "int ${class_name}" ;
            $double_call_functions = "double ${class_name}" ;
        } else {
            $int_call_functions = "template <$full_template_args> int ${class_name}<$template_args>" ;
            $double_call_functions = "template <$full_template_args> double ${class_name}<$template_args>" ;
        }
        $int_call_functions .= "::call_function ( Trick::JobData * curr_job __attribute__ ((unused)) ) {return 0;}\n" ;
        $double_call_functions .= "::call_function_double ( Trick::JobData * curr_job __attribute__ ((unused)) ) {return 0.0;}\n" ;
        $$sim_ref{sim_class_call_functions} .= $int_call_functions . $double_call_functions ;
    }

}

sub handle_sim_class_job($$$) {
    my ($in_job, $job_id, $sim_ref) = @_ ;
    my ($job_push, $is_dynamic_event) ;
    my ( $child, $phase, $cycle, $start, $stop, $ov_class ,
         $ov_class_self, $sup_class_data, $tag, $job_call, $job_ret, $job_name, $args , $class ) ;
    my (@tags) ;

    ($child, $phase, $cycle, $start, $stop,
      $ov_class , $ov_class_self , $sup_class_data, $tag, $job_call, $job_ret, $job_name, $args) = $in_job =~ /
         (?:
           \s*(?:
             ([Cc][\w\.\-\>]+)?\s*                       # child spec
             ([Pp][\w\.\-\>]+)?\s*                       # phase spec
             (?:
              \(
                (?:
                   (?:
                      \s*([\w.]+)\s*           # cycle time
                      (?:,\s*([\w.]+)\s*)?     # start time
                      (?:,\s*([\w.]+)\s*)?     # stop time
                      (?:,\s*("?\w+"?)\s*)     # class
                   )|
                   \s*("?\w+"?)\s*             # class (by itself)
                   (?:,\s*(&?[\w\.\-\>]+)\s*)? # integration object
                )
              \)
             ) |                               # timing spec
             \{([\w_.,\s]+)\}                  # job tag
           )
         )+\s+
         (                                     # job call
          ([A-Za-z_][\w\.\-\>]*\s*=)?\s*       # optional return assignment variable
          ([A-Za-z_][\w\.\:\-\>\[\]]*)\s*      # job name
          \((.*?)\s*\)                         # arg list
         )\s*;                                 # end job call
         /sx ;


    $child = 0 if ( $child eq "" ) ;
    $child =~ s/^C// ;

    $cycle = 1.0 if ( $cycle eq "" ) ;

    if ( $ov_class ne "" ) {
        $class = $ov_class ;
    } else {
        $class = $ov_class_self ;
    }

    if ($sup_class_data eq "") {
        $sup_class_data = "NULL";
    }

    if ( $class =~ /dynamic_event/ ) {
        $is_dynamic_event = 1 ;
    } else {
        $is_dynamic_event = 0 ;
    }

    if ( $tag ne "" ) {
        $tag =~ s/\s+//g ;
        @tags = split /,/ , $tag ;
    }
    # do not remove "this->", see issue #532
    $job_push = "job = this->add_job($child, $job_id, $class, $sup_class_data, $cycle, \"$job_name\", \"\"" ;

    if ( $class =~ /^integration$/ ) {
        if ($job_ret !~ /trick_ret/ ) {
            $job_call = "trick_ret = " . $job_call ;
        }
    }

    if ( $phase ne "" ) {
        $phase =~ s/^P// ;
        $job_push .= ", $phase" ;
    } else {
        $job_push .= ", 60000" ;
    }
    if ( $start ne "" ) {
        $job_push .= ", $start" ;
    }
    if ( $stop ne "" ) {
        $job_push .= ", $stop" ;
    }
    $job_push .= ") ;" ;

    foreach my $t ( @tags ) {
        $job_push .= "\n            " ;
        $job_push .= "job->add_tag(\"$t\") ;" ;
    }

    trick_print($$sim_ref{fh},"    Job deconstruction:
      Job_id: $job_id
      Child: $child
      Phase: $phase
      Tag: $tag
      Class: $class
      Supplemental data: $sup_class_data
      Cycle: $cycle
      Start: $start
      Stop: $stop
      Job name: $job_name\n", "debug_white", $$sim_ref{args}{v}) ;

    return $job_push , $job_call , $is_dynamic_event ;

}

sub handle_instantiation ($$) {

    my ($s, $sim_ref) = @_;
    my ($type , $name) ;

    $s =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;
    #print "instance $s\n" ;
    ($type , $name) = $s =~ /([A-Za-z_]+[\w_:\*]*(?:<[^>]+>)?)\s+([A-Za-z_]+[\w_]*)/s ;
    #print "instance type = $type , name = $name\n" ;

    push @{$$sim_ref{instances}} , $name ;
    $$sim_ref{instances_type}{$name} = $type ;

    $$sim_ref{instance_declarations} .= "$s\n" ;

    trick_print($$sim_ref{fh},"Instantiation: $s\n", "debug_white", $$sim_ref{args}{v}) ;

    # remove constructor parameters
    $s =~ s/\(.*/;/s ;

    if ($type =~ /<[^>]+>/) {
        $$sim_ref{template_instance_declarations} .= "$s\n" ;
    }
    $$sim_ref{extern_instance_declarations} .= "extern $s\n" ;

}

sub handle_create_connections($$$) {

    my ($s, $file_contents, $sim_ref) = @_;
    my ($cc_code) ;

    ($cc_code, $$file_contents) = extract_bracketed($$file_contents,"{}");

    trick_print($$sim_ref{fh},"Create connections code: $cc_code\n", "debug_white", $$sim_ref{args}{v}) ;

    $cc_code =~ s/^{//s ;
    $cc_code =~ s/}$//s ;
    $cc_code =~ s/ZZZYYYXXX(\d+)ZZZYYYXXX//g ;

    $$sim_ref{create_connections} .= $cc_code ;
    #print $$sim_ref{create_connections} ;
}

sub handle_line_tag($$) {
    my ($s, $sim_ref) = @_;
    my ($line_num , $file_name) = $s =~ /(\d+)\s*\"(.*?)\"/;
    trick_print($$sim_ref{fh},"Line: $s\n", "debug_yellow", $$sim_ref{args}{v}) ;
    if ( $file_name !~ /^\</ and $file_name ne $$sim_ref{last_file} ) {
        if ( exists $$sim_ref{files_visited}{$file_name} ) {
            trick_print($$sim_ref{fh},"Continuing ", "normal_blue", $$sim_ref{args}{v}) ;
            trick_print($$sim_ref{fh},"$file_name\n", "normal_white", $$sim_ref{args}{v}) ;
        } else {
            trick_print($$sim_ref{fh},"Processing ", "normal_blue", $$sim_ref{args}{v}) ;
            trick_print($$sim_ref{fh},"$file_name\n", "normal_white", $$sim_ref{args}{v}) ;
            $$sim_ref{files_visited}{$file_name} = 1 ;
        }
        $$sim_ref{last_file} = $file_name ;
    }
    $$sim_ref{line_num} = $line_num ;
}

sub handle_compiler_directive($$) {

    my ($s, $sim_ref) = @_;
    my ($rel_file_name) ;
    my ($file_name) ;
    $s =~ s/^#// ;

    trick_print($$sim_ref{fh},"Compiler directive: $s\n", "debug_white", $$sim_ref{args}{v}) ;
    # system header
    if ( $s =~ /^\#include\s+\</ ) {
        $$sim_ref{system_headers} .= "$s\n" ;
    } else {
        #push @{$$sim_ref{sim_class_includes}} , $s ;
        if ( $s =~ s/^\#include \"(\/.*)\"/$1/ ) {
            $$sim_ref{headers_full_path}{$s} = $s ;
            push @{$$sim_ref{sim_class_includes}} , $s ;
        }
        elsif ( $s =~ /include\s*\"([^\"]+)\"/ ) {
            ($rel_file_name) = $1 ;
            $file_name = find_header_file($rel_file_name , \@{$$sim_ref{inc_paths}}) ;
            if ( $file_name eq "" ) {
                trick_formatted_print($$sim_ref{fh}, "[91m[1m", "Error      ", "[0m", "Could not find included file \"", "[1m", "$rel_file_name", "[0m", "\"\n") ;
                exit -1 ;
            }
            trick_print($$sim_ref{fh},"  Found include: $file_name\n", "debug_white", $$sim_ref{args}{v}) ;
            $$sim_ref{headers_full_path}{$rel_file_name} = $file_name ;
            push @{$$sim_ref{sim_class_includes}} , $file_name ;

            my $suffix ;
            if ( $file_name =~ /\.h$/ ) {
                # Try and determine language.
                # extract the Trick header
                my %header ;
                local $/ = undef ;
                open TEXT, '<', $file_name ;
                my $file_contents = <TEXT> ;
                %header = extract_trick_header( $file_name, $file_contents, 0 , 0 );
                # set the language if an override in the header is found
                if ( $header{language} eq "CPP" ) {
                    $suffix = "cpp" ;
                }
                else {
                    $suffix = "c" ;
                }
            } else {
                $suffix = "cpp" ;
            }
            # save off suffix for make_makefile
            $$sim_ref{headers_lang}{$file_name} = $suffix ;

        } else {
            trick_print($$sim_ref{fh},"ERROR: S_define:$$sim_ref{line_num}: ##include expects \"FILENAME\" (double quotes for FILENAME) \n", "title_red", $$sim_ref{args}{v}) ;
        }
    }
}

sub preparse_job_class_order($$) {

    my ( $job_class_order_structs , $sim_ref ) = @_ ;
    my $class_text ;
    my @class_list ;
    my %temp_hash ;

    if ( scalar @{$job_class_order_structs} > 1 ) {
        edit_and_exit("Multiple job order constructs found" , "$s_define_file" , 1 ) ;
    } elsif ( scalar @{$job_class_order_structs} == 0 ) {
        return ;
    }

    # get a list of classes
    ($class_text) = @{$job_class_order_structs}[0] =~ /{(.*?)}/sx ;
    $class_text =~ s/^\s+|\s+$//gs ;
    @class_list = split /\s*,\s*/ , $class_text ;

    # check to make sure the class names are not duplicated
    foreach my $c ( @class_list ) {
        if ( exists $temp_hash{$c} ) {
            trick_print($$sim_ref{fh}, "\nCP ERROR:\n    Job class \"$c\" duplicated in new order.\n" , "title_red" , $$sim_ref{args}{v} ) ;
            edit_and_exit("CP bad job class order" , "$s_define_file" , 1 ) ;
        }
        $temp_hash{$c}++ ;
    }

    # save the new order
    @{$$sim_ref{user_class_order}} = @class_list ;

    # push on classes important to trick system function if not specified
    if ( !exists $temp_hash{automatic} ) {
        unshift @{$$sim_ref{user_class_order}} , "automatic" ;
    }

    if ( !exists $temp_hash{automatic_last} ) {
        push @{$$sim_ref{user_class_order}} , "automatic_last" ;
    }

    if ( !exists $temp_hash{logging} ) {
        push @{$$sim_ref{user_class_order}} , "logging" ;
    }

    if ( !exists $temp_hash{data_record} ) {
        push @{$$sim_ref{user_class_order}} , "data_record" ;
    }

    if ( !exists $temp_hash{system_checkpoint} ) {
        push @{$$sim_ref{user_class_order}} , "system_checkpoint" ;
    }

    if ( !exists $temp_hash{system_advance_sim_time} ) {
        push @{$$sim_ref{user_class_order}} , "system_advance_sim_time" ;
    }

    if ( !exists $temp_hash{system_moding} ) {
        push @{$$sim_ref{user_class_order}} , "system_moding" ;
    }

    if ( !exists $temp_hash{integ_loop} ) {
        push @{$$sim_ref{user_class_order}} , "integ_loop" ;
    }
}

1;
