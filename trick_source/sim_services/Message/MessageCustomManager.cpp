#include	"trick/MessageCustomManager.hh"
#include	"trick/Message_proto.hh"
#include	"trick/memorymanager_c_intf.h"

Trick::MessageCustomManager	*	the_message_custom_manager	;

int	open_custom_message_file(std::string	file_name,	std::string	subscriber_name,	int	level)	{
				if	(the_message_custom_manager	==	NULL)	{
								std::cout	<<	"Problem:	custom_message_manager	not	yet	instantiated"	<<	std::endl;
								return	-1;
				}

				return	the_message_custom_manager->open_custom_message_file(file_name,	subscriber_name,	level);
}

/**
	@brief	The	constructor.
				*/
Trick::MessageCustomManager::MessageCustomManager()	{
				the_message_custom_manager	=	this;
}

Trick::MessageCustomManager::~MessageCustomManager()	{
				the_message_custom_manager	=	NULL;
				
				for	(Trick::MessageCustomFile	*		message_file	:	_custom_message_files)	{
								TMM_delete_var_a(message_file);
				}
}

int	Trick::MessageCustomManager::open_custom_message_file(std::string	file_name,	std::string	subscriber_name,	int	level)	{
				Trick::MessageCustomFile	*	new_message_file	=	(Trick::MessageCustomFile	*)	TMM_declare_var_s("Trick::MessageCustomFile");

				new_message_file->set_level(level);
				new_message_file->set_file_name(file_name);
				new_message_file->set_name(subscriber_name);
				new_message_file->init();

				_custom_message_files.push_back(new_message_file);

				return	new_message_file->get_level();
}

/**
	@brief	Output	message	to	the	file.
				*/
void	Trick::MessageCustomManager::update(	unsigned	int	level	,	std::string	header	,	std::string	message	)	{
				for	(auto	message_file	:	_custom_message_files)	{
								message_file->update(level,	header,	message);
				}
}

int	Trick::MessageCustomManager::restart(	)	{
				for	(auto	message_file	:	_custom_message_files)	{
								message_file->restart();
				}

				return	0;
}