#include	<iostream>
#include	<stdio.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<pwd.h>


#include	"gtest/gtest.h"
#include	"trick/CommandLineArguments.hh"

namespace	Trick	{


//	Clean	up	the	directory	created
void	rm_dir(const	std::string&	dir)	{
				const	int	result	=	remove(	dir.c_str()	);
				if(	result	!=	0	&&	errno	==	66){
								//	Failed	because	directory	is	not	empty
								//	Remove	stuff	and	try	again
								DIR	*temp_dir;
								struct	dirent	*ent;
								if	((temp_dir	=	opendir	(dir.c_str()))	!=	NULL)	{
												//	Go	through	everything	in	this	directory
												while	((ent	=	readdir	(temp_dir))	!=	NULL)	{
																if	(!(strcmp(".",	ent->d_name)	==	0	||	strcmp("..",	ent->d_name)	==	0	))	{
																				//	remove	it
																				rm_dir(dir	+	"/"	+	std::string(ent->d_name));
																}
												}
												closedir	(temp_dir);
								}

								//	Try	again
								remove(	dir.c_str()	);
				}
}

bool	dir_correct(const	std::string&	dir)	{
				//	Make	sure	that	the	directory	exists	and	is	writeable
				struct	stat	info;
				if(stat(	dir.c_str(),	&info	)	==	0)	{
								if	(info.st_mode	&	S_IFDIR	&&	info.st_mode	&	S_IWUSR	)	{
												return	true;
								}
				}

				return	false;
}

TEST(CreatePathTest,	BasicTest)	{
				std::string	dir	=	"a/b/c";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	0);
				ASSERT_TRUE(dir_correct(dir));

				rm_dir("a");
}

TEST(CreatePathTest,	PreExistingDir)	{
				std::string	dir	=	"pre_existing_output_dir";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	0);
				ASSERT_TRUE(dir_correct(dir));
}

TEST(CreatePathTest,	PreExistingInPath)	{
				std::string	dir	=	"pre_existing_output_dir/a/b/c";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	0);
				ASSERT_TRUE(dir_correct(dir));

				rm_dir("pre_existing_output_dir/a");
}

TEST(CreatePathTest,	FileInPath)	{
				std::string	dir	=	"pre_existing_output_dir/some_file/a";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	1);
}


TEST(CreatePathTest,	AbsolutePath)	{
				char	buf[1024];
				getcwd(buf,	1024);
				std::string	dir	=	std::string(buf)	+	"/a/b/c";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	0);
				ASSERT_TRUE(dir_correct(dir));

				rm_dir(std::string(buf)	+	std::string("/a"));
}

TEST(CreatePathTest,	dotdotinpath)	{
				std::string	dir	=	"../a/b/c";
				ASSERT_EQ(CommandLineArguments::create_path(dir),	0);
				ASSERT_TRUE(dir_correct(dir));

				rm_dir("../a");
}



}
