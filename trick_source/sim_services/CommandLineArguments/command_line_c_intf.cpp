/*
			PURPOSE:	(	Handle	the	simulation	command	line	args	)

			REFERENCE:	(	Trick	Simulation	Environment	)

			ASSUMPTIONS	AND	LIMITATIONS:	(	None	)

			CLASS:	(	N/A	)

			LIBRARY	DEPENDENCY:	(	None	)

			PROGRAMMERS:	(	Keith	Vetter	LinCom	6/2003	)	*/

#include	<iostream>
#include	"trick/CommandLineArguments.hh"

extern	Trick::CommandLineArguments	*	the_cmd_args	;

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::process_sim_args(int	argc	,	char	**argv)
	*/
extern	"C"	int	command_line_args_process_sim_args(int	argc	,	char	**argv)	{
				return(the_cmd_args->process_sim_args(argc,	argv))	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_argc()
	*/
extern	"C"	int	command_line_args_get_argc(void)	{
				return(the_cmd_args->get_argc())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_argv()
	*/
extern	"C"	char	**	command_line_args_get_argv(void)	{
				return(the_cmd_args->get_argv())	;
}

const	char	*	empty_string	=	""	;
/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_output_dir()
	*/
extern	"C"	const	char	*	command_line_args_get_output_dir(void)	{
				return(the_cmd_args->get_output_dir_ref().c_str())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_user_output_dir()
	*/

extern	"C"	const	char	*	command_line_args_get_user_output_dir(void)	{
				return(the_cmd_args->get_user_output_dir_ref().c_str())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_input_file()
	*/
extern	"C"	const	char	*	command_line_args_get_input_file(void)	{
				return(the_cmd_args->get_input_file_ref().c_str())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_default_dir()
	*/
extern	"C"	const	char	*	command_line_args_get_default_dir(void)	{
				return(the_cmd_args->get_default_dir_ref().c_str())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_cmdline_name()
	*/
extern	"C"	const	char	*	command_line_args_get_cmdline_name(void)	{
				return(the_cmd_args->get_cmdline_name_ref().c_str())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::output_dir_timestamped_on()
	*/
extern	"C"	int	output_dir_timestamped_on(void)	{
				return(the_cmd_args->output_dir_timestamped_on())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::output_dir_timestamped_off()
	*/
extern	"C"	int	output_dir_timestamped_off(void)	{
				return(the_cmd_args->output_dir_timestamped_off())	;
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::set_output_dir(std::string)
	*/
extern	"C"	void	set_output_dir(const	char*	output_directory)	{
				the_cmd_args->set_output_dir(output_directory);
}

/**
	@relates	Trick::CommandLineArguments
	@copydoc	Trick::CommandLineArguments::get_input_file_ref()
	*/
std::string	&	command_line_args_get_input_file_ref(void)	{
				return(the_cmd_args->get_input_file_ref())	;
}

void	create_path(const	char*	directory)	{
				Trick::CommandLineArguments::create_path(directory);
}