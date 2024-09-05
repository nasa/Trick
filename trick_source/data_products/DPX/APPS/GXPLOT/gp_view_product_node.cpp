
#include	"gp_view_product_node.hh"


GPViewProductNode::GPViewProductNode(Widget	Toplevel,	DPC_product*	Product)	{

				product	=	Product;
				toplevel	=	Toplevel;
}


//DESTRUCTOR
GPViewProductNode::~GPViewProductNode()	{

				#ifdef	DEBUG
				cout	<<	"GPViewProductNode	DESTRUCTOR."	<<	endl;
				#endif

				GPViewPageNode		*page_view_node;
				int	n,i;

				n	=	(int)page_node_list.size();
				for	(i=0	;	i<n;	i++	)	{
								page_view_node	=	page_node_list[i];
								delete	page_view_node;
				}

}
