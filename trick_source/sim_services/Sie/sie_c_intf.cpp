
#include	"trick/Sie.hh"
#include	"trick/sie_c_intf.h"

extern	Trick::Sie	*	the_sie	;

extern	"C"	void	sie_print_xml(void)	{
				if	(	the_sie	!=	NULL	)	{
								the_sie->sie_print_xml()	;
				}
}

extern	"C"	void	sie_class_attr_map_print_xml(void)	{
				if	(	the_sie	!=	NULL	)	{
								the_sie->class_attr_map_print_xml()	;
				}
}

extern	"C"	void	sie_enum_attr_map_print_xml(void)	{
				if	(	the_sie	!=	NULL	)	{
								the_sie->enum_attr_map_print_xml()	;
				}
}

extern	"C"	void	sie_top_level_objects_print_xml(void)	{
				if	(	the_sie	!=	NULL	)	{
								the_sie->top_level_objects_print_xml()	;
				}
}

extern	"C"	void	sie_append_runtime_objs(void)	{
				if	(	the_sie	!=	NULL	)	{
								the_sie->sie_append_runtime_objs()	;
				}
}

extern	"C"	std::string	sie_get_runtime_sie_dir(void)	{
				if	(	the_sie	!=	NULL	)	{
								return	the_sie->get_runtime_sie_dir()	;
				}
}

