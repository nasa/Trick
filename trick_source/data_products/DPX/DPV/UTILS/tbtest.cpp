
#include	"DPV_textbuffer.hh"

#include	<iostream>

int	main(	int	argc,	char	*	argv[])	{

				DPV_TextBuffer	my_text;

				my_text.print("Old	McDonald	had	a	farm.	EIEIO.\n");
				my_text.print("On	his	farm	he	had	a	ANIMAL1.	EIEIO.\n");
				my_text.print("On	his	farm	he	had	a	ANIMAL2.	EIEIO.\n");
				my_text.print("On	his	farm	he	had	a	ANIMAL3.	EIEIO.\n");
				my_text.print("On	his	farm	he	had	a	ANIMAL1.	EIEIO.\n");
				my_text.print("On	his	farm	he	had	a	ANIMAL1.	EIEIO.\n");
				my_text.subst_g("ANIMAL1","cow");
				my_text.subst_g("ANIMAL2","ostrich");
				my_text.subst_g("ANIMAL3","hippopotamus");
				my_text.subst_g("ANIMAL4","gnu");
				my_text.subst_g("ANIMAL5","pig");

				cout	<<	my_text.getText();
}


